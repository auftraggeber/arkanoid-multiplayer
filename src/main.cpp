#include <algorithm>
#include <asm-generic/errno.h>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <google/protobuf/stubs/common.h>
#include <iostream>

#include <fmt/format.h>
#include <istream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <random>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <chrono>

#include "box2d-incl/box2d/b2_body.h"
#include "box2d-incl/box2d/b2_broad_phase.h"
#include "box2d-incl/box2d/b2_circle_shape.h"
#include "box2d-incl/box2d/b2_contact.h"
#include "box2d-incl/box2d/b2_fixture.h"
#include "box2d-incl/box2d/b2_math.h"
#include "box2d-incl/box2d/b2_polygon_shape.h"
#include "box2d-incl/box2d/b2_settings.h"
#include "box2d-incl/box2d/b2_world.h"
#include "box2d-incl/box2d/b2_world_callbacks.h"
#include "fmt/core.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

#include "arkanoid.pb.h"

#include "box2d-incl/box2d/box2d.h"

#include "arkanoid_elements.hpp"
#include "connection.hpp"
#include "utils.hpp"

void show_connection_methods(std::function<void(bool const &, int const &)> callback)
{
  using namespace ftxui;

  auto screen = ScreenInteractive::TerminalOutput();

  bool is_host{ false };
  std::string port_string{ std::to_string(connection::default_port) };


  auto button_host = Button("Als Host auf Verbindung warten", [&] {
    screen.Exit();
    is_host = true;
  });
  auto button_client = Button("Zu einem Host verbinden", [&] {
    screen.Exit();
    is_host = false;
  });
  auto port_input = Input(&port_string, "Geben Sie den Port ein (Verbinden zu / Warten auf)");

  auto children = Container::Vertical({ port_input, Container::Horizontal({ button_host, button_client }) });


  auto render = Renderer(children, [&] {
    auto vb = vbox({ text("Willkommen. Wie möchten Sie sich zum Partner verbinden?"),
      separator(),
      text("Port:"),
      children->Render() });

    return vb | border;
  });


  screen.Loop(render);


  int const port = connection::calculate_port_from_string(port_string);
  callback(is_host, port);
}


void show_connecting_state(connection::Connection &connection)
{
  using namespace ftxui;

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Clear();

  auto renderer = Renderer([&connection]() {
    return vbox({ text((connection.has_connected()) ? "Verbunden" : "Konnte nicht verbinden") }) | border;
  });

  Loop{ &screen, std::move(renderer) }.RunOnce();

  screen.Exit();
}

void show_winner(int const winner, std::array<arkanoid::Paddle *, 2> const &paddles)
{
  using namespace ftxui;

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Clear();

  auto renderer = Renderer([&]() {
    std::string winner_str{ "Unentschieden" };

    if (winner < paddles.size() && winner >= 0) {
      if (paddles[winner]->is_controlled_by_this_game_instance()) {
        winner_str = "Du hast gewonnnen!";
      } else {
        winner_str = "Dein Gegner hat gewonnen!";
      }
    } else if (winner < 0) {
      winner_str = "Das Spiel wurde abgebrochen!";
    }

    return vbox({ text(winner_str) }) | border;
  });

  Loop{ &screen, std::move(renderer) }.Run();

  screen.Exit();
}

void connect_to_peer(connection::Connection &connection, bool const as_host, int const port)
{
  using namespace ftxui;

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Clear();
  auto renderer = Renderer([]() { return vbox(text("Versuche zu Host zu verbinden.")) | border; });

  if (as_host) {

    renderer = Renderer([&port]() {
      return vbox({ text(fmt::format("Warte auf Port {} auf eingehende Verbindung.", std::to_string(port))) }) | border;
    });
  }

  Loop loop{ &screen, std::move(renderer) };
  loop.RunOnce();
  screen.Exit();


  if (as_host) {
    connection.wait_for_connection(port);
  } else {
    connection.connect_to(connection::default_host, port);
  }
}

void create_and_send_new_game_update(std::vector<arkanoid::Element *> const &send_elements,
  connection::Connection &connection,
  std::mutex &game_update_mutex)
{
  std::thread([send_elements, &connection, &game_update_mutex]() {// todo: sicherheit?
    GameUpdate update;
    {
      std::lock_guard<std::mutex> game_update_lock{ game_update_mutex };
      arkanoid::fill_game_update(&update, send_elements);

      bool force_sending{ false };

      for (auto *const element_ptr : send_elements) {
        if (element_ptr->get_type() != arkanoid::PADDLE) {
          force_sending = true;
          break;
        }
      }

      /*std::for_each(send_elements.begin(), send_elements.end(), [&force_sending](auto const element_ptr) {
        if (force_sending || element_ptr->get_type() == arkanoid::PADDLE) { return; }
        force_sending = true;
      });*/ // todo: problem hierbei: kein break!

      connection.send(update, force_sending);
    }

  })
    .detach();
}

void generate_bricks(std::map<int, std::unique_ptr<arkanoid::Element>> &elements,
  b2World *world,
  std::map<b2Fixture *, arkanoid::Element *> &map)
{
  using namespace arkanoid;
  std::random_device random_device;
  std::default_random_engine random_engine{ random_device() };
  std::uniform_int_distribution<int> uni_dist{ brick_min_duration, brick_max_duration };

  int const num_bricks_x =
    (playing_field_right - playing_field_left - brick_distance_x) / (brick_width + brick_distance_x);

  int const total_brick_height = (num_bricks_y * (brick_height + brick_distance_y)) + brick_distance_y;
  int const total_brick_width = (num_bricks_x * (brick_width + brick_distance_x)) - brick_distance_x;
  int const top = std::round(((playing_field_bottom - playing_field_top) / 2.0F) - (total_brick_height / 2.0F));
  int const left = std::round(((playing_field_right - playing_field_left) - total_brick_width) / 2.0F);

  for (int i_x{ 0 }; i_x < num_bricks_x; ++i_x) {
    for (int i_y{ 0 }; i_y < num_bricks_y; ++i_y) {

      int const x{ playing_field_left + left + ((brick_distance_x + brick_width) * i_x) };
      int const y{ playing_field_top + top + ((brick_distance_y + brick_height) * i_y) };

      std::unique_ptr<arkanoid::Element> brick =
        std::make_unique<arkanoid::Brick>(arkanoid::Vector{ x, y }, world, map, uni_dist(random_engine));
      insert_element(elements, brick);
    }
  }
}

void generate_arkanoid_elements(arkanoid::Vector const paddle_position,
  int const paddle_y,
  arkanoid::Vector const ball_position_add,
  arkanoid::Vector const ball_velocity,
  b2World *arkanoid_world,
  std::map<int, std::unique_ptr<arkanoid::Element>> &element_map,
  std::map<b2Fixture *, arkanoid::Element *> &b2_element_map)
{
  using namespace arkanoid;

  std::unique_ptr<arkanoid::Element> paddle = std::make_unique<Paddle>(paddle_position, arkanoid_world, b2_element_map);
  std::unique_ptr<arkanoid::Element> paddle_enemy = std::make_unique<Paddle>(
    paddle_position.sub(0, paddle_y).add(0, playing_field_top), arkanoid_world, b2_element_map);

  auto *paddle_ptr = dynamic_cast<Paddle *>(paddle.get());
  paddle_ptr->set_is_controlled_by_this_game_instance(true);

  auto *paddle_enemy_ptr = dynamic_cast<Paddle *>(paddle_enemy.get());
  paddle_enemy_ptr->set_is_controlled_by_this_game_instance(false);

  std::unique_ptr<arkanoid::Element> ball =
    std::make_unique<Ball>(paddle_position.add(ball_position_add), arkanoid_world, b2_element_map, ball_velocity);
  std::unique_ptr<arkanoid::Element> ball_enemy = std::make_unique<Ball>(
    paddle_position.sub(0, paddle_y).add(0, playing_field_top).add({ ball_position_add.x, -ball_position_add.y }),
    arkanoid_world,
    b2_element_map,
    ball_velocity.invert());

  insert_element(element_map, paddle);
  insert_element(element_map, paddle_enemy);
  insert_element(element_map, ball);
  insert_element(element_map, ball_enemy);

  generate_bricks(element_map, arkanoid_world, b2_element_map);
}


class ContactListener : public b2ContactListener
{
private:
  static void BackPlateHit(arkanoid::Paddle *const paddle) { paddle->add_score(-5); }

  std::map<b2Fixture *, arkanoid::Element *> &m_b2_element_map;
  std::map<b2Fixture *, arkanoid::Paddle *> m_back_plates;

  void UpdateElementsAfterContact(std::map<b2Fixture *, arkanoid::Element *>::iterator const &first_iterator,
    std::map<b2Fixture *, arkanoid::Element *>::iterator const &second_iterator)
  {
    using namespace arkanoid;

    auto *first_element_ptr = first_iterator->second;
    auto *secound_element_ptr = second_iterator->second;

    if (first_element_ptr->get_type() == secound_element_ptr->get_type()) { return; }

    auto *brick_ptr = dynamic_multiple_cast<Brick *>(first_element_ptr, secound_element_ptr);
    auto *ball_ptr = dynamic_multiple_cast<Ball *>(first_element_ptr, secound_element_ptr);
    auto *paddle_ptr = dynamic_multiple_cast<Paddle *>(first_element_ptr, secound_element_ptr);


    if (brick_ptr != nullptr && ball_ptr != nullptr) {
      brick_ptr->hit(ball_ptr);
      if (ball_ptr->last_paddle() != nullptr) { ball_ptr->last_paddle()->add_score(1); }
    }
    if (ball_ptr != nullptr && paddle_ptr != nullptr) {
      if (paddle_ptr->is_controlled_by_this_game_instance()) {
        ball_ptr->add_to_next_update();
        ball_ptr->set_last_paddle(paddle_ptr);
      }
    }
  }


  void CheckIfBackPlateWasHit(b2Contact *contact)
  {
    arkanoid::Paddle *paddle_ptr{ nullptr };
    arkanoid::Ball *ball_ptr{ nullptr };
    if (m_back_plates.contains(contact->GetFixtureA()) && m_b2_element_map.contains(contact->GetFixtureB())) {
      paddle_ptr = m_back_plates.find(contact->GetFixtureA())->second;
      ball_ptr = dynamic_cast<arkanoid::Ball *>(m_b2_element_map.find(contact->GetFixtureB())->second);
    } else if (m_back_plates.contains(contact->GetFixtureB()) && m_b2_element_map.contains(contact->GetFixtureA())) {
      paddle_ptr = m_back_plates.find(contact->GetFixtureB())->second;
      ball_ptr = dynamic_cast<arkanoid::Ball *>(m_b2_element_map.find(contact->GetFixtureA())->second);
    }

    if (paddle_ptr != nullptr && ball_ptr != nullptr && ball_ptr->last_paddle() != nullptr
        && ball_ptr->last_paddle()->is_controlled_by_this_game_instance()) {
      BackPlateHit(paddle_ptr);
    }
  }

public:
  explicit ContactListener(std::map<b2Fixture *, arkanoid::Element *> &b2_map) : m_b2_element_map{ b2_map } {};

  void add_back_plate(b2Fixture *const fixture, arkanoid::Paddle *const paddle)
  {
    m_back_plates.insert({ fixture, paddle });
  }


  void PreSolve(b2Contact *contact, const b2Manifold *oldManifold) override
  {
    contact->SetFriction(0.0F);
    contact->SetRestitution(1.0F);
    contact->SetTangentSpeed(0.5F);
  }

  void EndContact(b2Contact *contact) override
  {
    if (m_b2_element_map.contains(contact->GetFixtureA())) {
      auto pairA = m_b2_element_map.find(contact->GetFixtureA());
      if (m_b2_element_map.contains(contact->GetFixtureB())) {
        auto pairB = m_b2_element_map.find(contact->GetFixtureB());
        UpdateElementsAfterContact(pairA, pairB);
        return;
      }
    }

    CheckIfBackPlateWasHit(contact);
  }
};

std::array<b2Fixture *, 2> build_b2_world_border(b2World *world)
{
  int const playing_field_width = arkanoid::playing_field_right - arkanoid::playing_field_left;
  int const playing_field_height = arkanoid::playing_field_bottom - arkanoid::playing_field_top;


  auto generate = [&world](float const x, float const y, float width, float height) {
    auto const position = arkanoid::convert_to_b2_coords(arkanoid::Vector{ x, y });
    width *= arkanoid::b2_coord_convertion_rate;
    height *= arkanoid::b2_coord_convertion_rate;

    b2BodyDef def;
    def.position.Set(position.x, position.y);
    b2Body *body = world->CreateBody(&def);
    b2PolygonShape box;
    box.SetAsBox(width, height);
    return body->CreateFixture(&box, 1.0f);
  };

  // links
  generate(arkanoid::playing_field_left - 2, arkanoid::playing_field_top - 12, 1, playing_field_height + 24);
  // rechts
  generate(arkanoid::playing_field_right + 1, arkanoid::playing_field_top - 12, 1, playing_field_height + 24);
  // oben
  auto *const fix_1 =
    generate(arkanoid::playing_field_left - 2, arkanoid::playing_field_top - 11, playing_field_width + 4, 1);
  // unten
  auto *const fix_2 =
    generate(arkanoid::playing_field_left - 2, arkanoid::playing_field_bottom + 10, playing_field_width + 4, 1);

  return { fix_1, fix_2 };
}

void draw_information_texts(ftxui::Canvas &can,
  connection::Connection &connection,
  int const your_score,
  int const enemy_score)
{
  using namespace arkanoid;
  can.DrawText(
    15, playing_field_bottom + 10, fmt::format("Synchronisationen versendet: {}", connection.game_updates_sent()));
  can.DrawText(
    15, playing_field_bottom + 15, fmt::format("Synchronisationen empfangen: {}", connection.game_updates_received()));

  can.DrawText(playing_field_right - 40, playing_field_bottom + 10, fmt::format("Deine Punkte: {}", your_score));
  can.DrawText(playing_field_right - 40, playing_field_bottom + 15, fmt::format("Punkte Gegner: {}", enemy_score));
}

[[nodiscard]] bool find_paddle_ptrs(std::array<arkanoid::Paddle *, 2> &paddle_ptrs,
  std::array<b2Fixture *, 2> const &back_plates,
  std::map<int, std::unique_ptr<arkanoid::Element>> const &element_map,
  ContactListener &listener)
{
  using namespace arkanoid;
  if (paddle_ptrs[0] == nullptr || paddle_ptrs[1] == nullptr) {
    auto found_iterator = std::find_if(
      element_map.begin(), element_map.end(), [](const auto &pair) { return pair.second->get_type() == PADDLE; });

    if (found_iterator != element_map.end()) {
      std::for_each(found_iterator, element_map.end(), [&paddle_ptrs](auto const &pair) {
        auto *element_ptr = pair.second.get();

        if (element_ptr->get_type() == PADDLE) {
          auto *paddle_ptr = dynamic_cast<Paddle *>(element_ptr);

          if (paddle_ptr->is_controlled_by_this_game_instance()) {
            paddle_ptrs[0] = paddle_ptr;
          } else {
            paddle_ptrs[1] = paddle_ptr;
          }
        }
      });

      if (paddle_ptrs[0] != nullptr && paddle_ptrs[1] != nullptr && back_plates[0] != nullptr
          && back_plates[1] != nullptr) {
        auto const distance_paddle_0_to_plate_0 =
          std::abs(arkanoid::convert_to_arkanoid_coords(back_plates[0]->GetBody()->GetPosition()).y
                   - paddle_ptrs[0]->center_position().y);
        auto const distance_paddle_1_to_plate_0 =
          std::abs(arkanoid::convert_to_arkanoid_coords(back_plates[0]->GetBody()->GetPosition()).y
                   - paddle_ptrs[1]->center_position().y);
        if (distance_paddle_0_to_plate_0 <= distance_paddle_1_to_plate_0) {
          listener.add_back_plate(back_plates[0], paddle_ptrs[0]);
          listener.add_back_plate(back_plates[1], paddle_ptrs[1]);
        } else {
          listener.add_back_plate(back_plates[0], paddle_ptrs[1]);
          listener.add_back_plate(back_plates[1], paddle_ptrs[0]);
        }
      }
    }
  }

  return paddle_ptrs[0] != nullptr;
}

void update_paddle_position(arkanoid::Paddle *paddle_ptr, int const mouse_x)
{
  using namespace arkanoid;
  int new_paddle_x = mouse_x;
  int const half_width = paddle_ptr->width() / 2;
  std::pair<int, int> constrains{ playing_field_left + half_width, playing_field_right - half_width };

  if (new_paddle_x < constrains.first) { new_paddle_x = constrains.first; }
  if (new_paddle_x > constrains.second) { new_paddle_x = constrains.second; }// todo: auslagern
  paddle_ptr->update_x(new_paddle_x);
}

[[nodiscard]] int get_winner(std::map<int, std::unique_ptr<arkanoid::Element>> const &elements,
  std::array<arkanoid::Paddle *, 2> const &paddles)
{
  auto found = std::find_if(elements.begin(), elements.end(), [](auto const &pair) {
    auto *const element = pair.second.get();

    return element->get_type() == arkanoid::BRICK && element->exists();
  });

  if (found == elements.end()) {
    if (paddles[0] != nullptr && paddles[1] != nullptr) {
      if (paddles[0]->score() > paddles[1]->score()) {
        return 0;
      } else if (paddles[0]->score() < paddles[1]->score()) {
        return 1;
      }

      return paddles.max_size();
    }
  }

  return -1;
}

void run_game(int const frame_rate,
  ftxui::ScreenInteractive &screen,
  ftxui::Loop &loop,
  int &mouse_x,
  connection::Connection &connection,
  std::mutex &element_mutex,
  std::map<int, std::unique_ptr<arkanoid::Element>> &element_map,
  std::array<arkanoid::Paddle *, 2> &paddle_ptrs,
  std::array<b2Fixture *, 2> const &back_plates,
  std::mutex &game_update_mutex,
  std::vector<arkanoid::Element *> &updated_elements,
  b2World &arkanoid_world,
  ContactListener &listener)
{
  using namespace ftxui;
  using namespace arkanoid;
  auto const frame_time_budget{ std::chrono::seconds(1) / frame_rate };
  long frame{ 0 };
  int winner{ -1 };

  while (!loop.HasQuitted()) {
    const auto frame_start_time{ std::chrono::steady_clock::now() };

    screen.RequestAnimationFrame();// wichtig, da sonst keine aktualisierung, wenn aus fokus
    loop.RunOnce();

    {
      std::lock_guard<std::mutex> lock{ element_mutex };

      if (find_paddle_ptrs(paddle_ptrs, back_plates, element_map, listener)) {
        auto *paddle_ptr = paddle_ptrs[0];
        update_paddle_position(paddle_ptr, mouse_x);
      }

      arkanoid_world.Step(1.0F / (frame_rate), 4, 2);

      std::for_each(element_map.begin(), element_map.end(), [&updated_elements](auto const &pair) {
        if (pair.second->did_update()) { updated_elements.push_back(pair.second.get()); }
      });
    }

    if (!updated_elements.empty()) {
      create_and_send_new_game_update(updated_elements, connection, game_update_mutex);
      updated_elements.clear();
    }

    {
      if (frame % frame_rate == 0) {
        std::lock_guard<std::mutex> lock{ element_mutex };
        winner = get_winner(element_map, paddle_ptrs);

        if (winner >= 0) {
          screen.Exit();
          break;
        }
      }
    }

    ++frame;
    const auto frame_end_time{ std::chrono::steady_clock::now() };
    const auto unused_frame_time{ frame_time_budget - (frame_end_time - frame_start_time) };
    if (unused_frame_time > std::chrono::seconds(0)) { std::this_thread::sleep_for(unused_frame_time); }
  }
  show_winner(winner, paddle_ptrs);
}

int main()
{
  using namespace ftxui;
  using namespace arkanoid;


  show_connection_methods([](bool const as_host, int const &port) {
    auto screen = ScreenInteractive::FitComponent();
    constexpr int frame_rate = 40.0;

    std::mutex element_mutex;
    std::mutex game_update_mutex;
    std::map<int, std::unique_ptr<arkanoid::Element>> element_map;
    std::map<b2Fixture *, arkanoid::Element *> b2_element_map;
    std::vector<arkanoid::Element *> updated_elements;

    int const paddle_y{ playing_field_bottom - paddle_height };
    Vector const paddle_position{ (canvas_width / 2) - (paddle_width / 2), paddle_y };
    std::array<Paddle *, 2> paddle_ptrs{ nullptr, nullptr };// {your, enemy}

    int mouse_x{ paddle_position.x_i() };

    b2World arkanoid_world{ { 0, 0 } };
    ContactListener listener{ b2_element_map };
    arkanoid_world.SetContactListener(&listener);
    auto const back_plates = build_b2_world_border(&arkanoid_world);


    connection::Connection connection;
    connection.register_receiver(
      [&element_map, &element_mutex, &arkanoid_world, &b2_element_map](GameUpdate const &update) {
        std::lock_guard<std::mutex> lock{ element_mutex };
        arkanoid::parse_game_update(element_map, update, &arkanoid_world, b2_element_map);
      });

    connect_to_peer(connection, as_host, port);
    show_connecting_state(connection);

    if (as_host) {

      {
        std::lock_guard<std::mutex> lock{ element_mutex };

        generate_arkanoid_elements(paddle_position,
          { playing_field_bottom - paddle_height },
          { paddle_width / 2, -10 },
          { ball_velocity_x, ball_velocity_y },
          &arkanoid_world,
          element_map,
          b2_element_map);

        GameUpdate update;
        arkanoid::fill_game_update(&update, map_values(element_map));

        connection.send(update, true);
      }
    }

    if (connection.has_connected()) {

      auto renderer = Renderer([&] {
        Canvas can = Canvas(canvas_width, canvas_height);

        int your_score{ 0 };
        int enemy_score{ 0 };

        {
          std::lock_guard<std::mutex> lock{ element_mutex };
          std::for_each(
            element_map.begin(), element_map.end(), [&can](auto const &pair) { draw(can, *(pair.second)); });

          if (paddle_ptrs[0] != nullptr) { your_score = paddle_ptrs[0]->score(); }
          if (paddle_ptrs[1] != nullptr) { enemy_score = paddle_ptrs[1]->score(); }
        }

        draw_information_texts(can, connection, your_score, enemy_score);

        can.DrawBlockLine(
          playing_field_left, playing_field_top, playing_field_left, playing_field_bottom, ftxui::Color::GrayLight);
        can.DrawBlockLine(
          playing_field_right, playing_field_top, playing_field_right, playing_field_bottom, ftxui::Color::GrayLight);


        return canvas(can);
      });

      renderer |= CatchEvent([&](Event event) {
        if (event == Event::Escape) {
          screen.Exit();
        } else if (event.is_mouse()) {
          mouse_x = (event.mouse().x - 1) * 2;// recommended translation of captured x
        }
        return true;
      });

      Loop loop{ &screen, std::move(renderer) };

      run_game(frame_rate,
        screen,
        loop,
        mouse_x,
        connection,
        element_mutex,
        element_map,
        paddle_ptrs,
        back_plates,
        game_update_mutex,
        updated_elements,
        arkanoid_world,
        listener);

      std::lock_guard<std::mutex> game_update_lock{ game_update_mutex };
      connection.close();
    }
  });

  google::protobuf::ShutdownProtobufLibrary();


  return EXIT_SUCCESS;
}
