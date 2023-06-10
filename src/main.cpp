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

#include "asio.hpp"

#include "asio/buffer.hpp"
#include "asio/completion_condition.hpp"
#include "asio/execution/execute.hpp"
#include "asio/impl/read_until.hpp"
#include "asio/io_service.hpp"
#include "asio/ip/address.hpp"
#include "asio/ip/tcp.hpp"
#include "asio/ip/udp.hpp"
#include "asio/read.hpp"
#include "asio/read_until.hpp"
#include "asio/registered_buffer.hpp"
#include "asio/streambuf.hpp"
#include "asio/system_error.hpp"
#include "asio/write.hpp"
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

template<typename T1, typename T2>//
void insert_element(std::map<T1, T2> &map, T2 &element)
{
  if (map.contains(element->id())) { map.erase(map.find(element->id())); }

  map.insert(std::pair<T1, T2>{ element->id(), std::move(element) });
}

template<typename T1, typename T2>//
std::vector<T2 *> map_values(std::map<T1, std::unique_ptr<T2>> const &map)
{
  std::vector<T2 *> vector;

  std::for_each(map.begin(), map.end(), [&vector](auto const &pair) { vector.push_back(pair.second.get()); });

  return vector;
}

namespace connection {

int constexpr default_port{ 45678 };
std::string const end_of_message{ "\n\r\r" };
std::string const default_host{ "127.0.0.1" };
bool constexpr debug_networking{ false };

void debug(std::string const &message)
{
  if (debug_networking) { fmt::print("[DEBUG] [NET] {}", message); }
}

[[nodiscard]] int calculate_port_from_string(std::string const &str)
{

  try {
    return std::stoi(str);
  } catch (std::exception &e) {}
  return connection::default_port;
}

class Connection
{
private:
  asio::io_service m_io_service;
  asio::ip::tcp::socket m_socket{ m_io_service };
  std::vector<std::function<void(GameUpdate const &)>> m_receivers;
  bool m_connected{ false }, m_sending{ false };
  std::queue<GameUpdate> m_next_game_updates;
  std::mutex m_receivers_mutex, m_conntected_mutex, m_send_mutex, m_send_c_mutex, m_receive_c_mutex;
  int m_gu_send_counter{ 0 }, m_gu_receive_counter{ 0 };

  void connect_to_on_this_thread(std::string const &host, int const &port)
  {
    try {
      if (has_connected()) { return; }

      std::lock_guard<std::mutex> lock{ m_conntected_mutex };
      m_socket.connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(host), port));
      m_connected = true;
    } catch (asio::system_error &e) {
      fmt::print("Fehler beim Verbinden.");
    }
  }

  void wait_for_connection_on_this_thread(int const &port)
  {
    try {
      if (has_connected()) { return; }

      asio::ip::tcp::acceptor acceptor{ m_io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port) };


      std::lock_guard<std::mutex> lock{ m_conntected_mutex };
      acceptor.accept(m_socket);
      m_connected = true;
    } catch (asio::system_error &e) {
      fmt::print("Fehler beim Warten auf Verbindung.");
    }
  }

  void call_receivers(GameUpdate const &update)
  {
    std::lock_guard<std::mutex> lock{ m_receivers_mutex };
    std::for_each(m_receivers.begin(),
      m_receivers.end(),
      [update](std::function<void(GameUpdate const &)> const &receiver) { receiver(update); });
  }

  void read_listener()
  {
    std::thread{
      [this]() {
        while (true) {
          asio::streambuf buffer;
          asio::error_code ec;
          GameUpdate update;

          try {
            std::size_t const bytes_transferred = asio::read_until(m_socket, buffer, end_of_message, ec);

            if (ec) { break; }

            std::string message{ buffers_begin(buffer.data()),
              buffers_begin(buffer.data()) + (bytes_transferred - end_of_message.size()) };
            buffer.consume(bytes_transferred);

            if (update.ParseFromString(message)) { call_receivers(update); }
            {
              std::lock_guard<std::mutex> c_lock{ m_receive_c_mutex };
              ++m_gu_receive_counter;
            }

            debug(fmt::format("Nachricht empfangen: {}\n", message));
          } catch (asio::system_error const &e) {

            {
              std::lock_guard<std::mutex> lock{ m_conntected_mutex };
              m_connected = false;
            }

            break;
          }
        }
      }
    }.detach();
  }


public:
  explicit Connection() = default;

  Connection(Connection const &) = delete;
  Connection &operator=(Connection const &) = delete;

  void connect_to(
    std::string const &host,
    int const &port,
    std::function<void()> const &on_finish = []() {})
  {
    std::thread{
      [host, port, this, on_finish]() {
        connect_to_on_this_thread(host, port);
        on_finish();
      }
    }.join();

    if (has_connected()) {
      read_listener();
      m_io_service.run();
    }
  }

  void wait_for_connection(
    int const &port,
    std::function<void()> const &on_finish = []() {})
  {
    std::thread{
      [port, this, on_finish]() {
        wait_for_connection_on_this_thread(port);
        on_finish();
      }
    }.join();

    if (has_connected()) {
      read_listener();
      m_io_service.run();
    }
  }

  void close()
  {
    std::lock_guard<std::mutex> lock{ m_conntected_mutex };
    m_socket.close();
  }

  void send(GameUpdate game_update, bool const add_to_queue_if_currently_in_use = false)
  {
    {
      std::lock_guard<std::mutex> lock{ m_send_mutex };
      if (!add_to_queue_if_currently_in_use && m_sending) { return; }
    }
    auto message = game_update.SerializeAsString();

    std::thread([this, message]() {
      {
        std::lock_guard<std::mutex> lock{ m_send_mutex };
        m_sending = true;
      }
      try {
        std::size_t const t = m_socket.write_some(asio::buffer(message + end_of_message));
        debug(fmt::format("{} Bytes versendet.", t));
      } catch (asio::system_error const &e) {}

      {
        std::lock_guard<std::mutex> lock{ m_send_mutex };
        m_sending = false;
      }
      {
        std::lock_guard<std::mutex> c_lock{ m_send_c_mutex };
        ++m_gu_send_counter;
      }
    }).detach();
  }

  void register_receiver(std::function<void(GameUpdate const &)> const &receiver)
  {
    std::lock_guard<std::mutex> lock{ m_receivers_mutex };
    m_receivers.push_back(receiver);
  }

  [[nodiscard]] bool has_connected()
  {
    std::lock_guard<std::mutex> lock{ m_conntected_mutex };
    return m_connected;
  }

  [[nodiscard]] int game_updates_sent()
  {
    std::lock_guard<std::mutex> lock{ m_send_c_mutex };
    return m_gu_send_counter;
  }

  [[nodiscard]] int game_updates_received()
  {
    std::lock_guard<std::mutex> lock{ m_receive_c_mutex };
    return m_gu_receive_counter;
  }
};

}// namespace connection

namespace arkanoid {

int constexpr canvas_width{ 167 }, canvas_height{ 200 };
int constexpr playing_field_top{ 5 }, playing_field_bottom{ canvas_height - 30 }, playing_field_left{ 1 },
  playing_field_right{ canvas_width - 2 };
int constexpr ball_radius{ 1 }, ball_speed{ 1 };
int constexpr paddle_width{ 14 }, paddle_height{ 1 };
int constexpr brick_width{ 14 }, brick_height{ 5 };
int constexpr num_bricks_y{ 8 };
int constexpr brick_distance_x{ 2 }, brick_distance_y{ 3 };
float constexpr b2_coord_convertion_rate{ 140.0F };
int constexpr brick_max_duration{ 3 }, brick_min_duration{ 1 };

class IdGenerator
{
private:
  int m_id;

public:
  explicit IdGenerator(int const start_with) : m_id{ start_with - 1 } {}
  [[nodiscard]] int next() { return ++m_id; }

  [[nodiscard]] int current() const { return m_id; }
};

struct Vector
{
public:
  float x, y;

  Vector(int const x, int const y) : x(x), y(y) {}
  Vector(float const x, float const y) : x{ x }, y{ y } {}// todo explicit

  [[nodiscard]] Vector invert() const { return { -x, -y }; }

  [[nodiscard]] Vector add(float const x, float const y) const { return { this->x + x, this->y + y }; }
  [[nodiscard]] Vector add(Vector const &vec) const { return add(vec.x, vec.y); }


  [[nodiscard]] Vector sub(float const x, float const y) const { return { this->x - x, this->y - y }; }
  [[nodiscard]] Vector sub(Vector const &vec) const { return sub(vec.x, vec.y); }

  [[nodistcard]] Vector set_abs(float const abs) const
  {
    Vector const norm = normalize();

    return { norm.x * abs, norm.y * abs };
  }

  [[nodiscard]] Vector normalize() const
  {
    float const a = abs();

    return { x / a, y / a };
  }

  [[nodiscard]] float abs() const { return std::sqrt((x * x) + (y * y)); }

  [[nodiscard]] int x_i() const { return std::round(x); }
  [[nodiscard]] int y_i() const { return std::round(y); }
};

[[nodiscard]] Vector convert_to_b2_coords(Vector const vector)
{
  return { vector.x * b2_coord_convertion_rate, vector.y * b2_coord_convertion_rate };
}
[[nodiscard]] Vector convert_to_arkanoid_coords(Vector const vector)
{
  return { vector.x / b2_coord_convertion_rate, vector.y / b2_coord_convertion_rate };
}


enum ElementType { BALL, BRICK, PADDLE };
class Element
{
public:
  [[nodiscard]] virtual ElementType get_type() const = 0;
  virtual bool did_update() = 0;
  virtual void set_position(Vector const) = 0;
  [[nodiscard]] virtual Vector center_position() const = 0;
  [[nodiscard]] virtual int width() const = 0;
  [[nodiscard]] virtual int height() const = 0;
  [[nodiscard]] virtual bool exists() { return true; }
  [[nodiscard]] virtual ftxui::Color color() const { return m_color; };

protected:
  int m_id;
  ftxui::Color m_color;

  friend void fill_game_element(GameElement *const &, arkanoid::Element const *);// todo: außerhalb von namespace ??
  friend void parse_game_update(std::map<int, std::unique_ptr<Element>> &,
    GameUpdate const &,
    b2World *,
    std::map<b2Fixture *, arkanoid::Element *> &);
  friend void parse_game_element(Element *, GameElement const &);

public:
  IdGenerator static id_generator;

  explicit Element(ftxui::Color const color = ftxui::Color::White) : m_color{ color }, m_id{ id_generator.next() } {}

  void invert_position()
  {
    Vector const position = center_position();
    set_position(
      { playing_field_right + playing_field_left - position.x, playing_field_bottom + playing_field_top - position.y });
  }

  [[nodiscard]] int left() const { return std::round(center_position().x - (width() / 2.0f)); }

  [[nodiscard]] int right() const { return left() + width(); }

  [[nodiscard]] int top() const { return std::round(center_position().y - (height() / 2.0f)); }

  [[nodiscard]] int bottom() const { return top() + height(); }

  [[nodiscard]] int id() const { return m_id; }
};

IdGenerator Element::id_generator = IdGenerator{ 0 };


class Paddle : public Element
{
private:
  b2Body *m_body_ptr = nullptr;
  bool m_updated{ false };
  bool m_is_controlled_by_this_game_instance{ false };
  int m_score{ 0 };

public:
  explicit Paddle(Vector const pos, b2World *arkanoid_world, std::map<b2Fixture *, Element *> &map) : Element{}
  {
    auto const position = convert_to_b2_coords(pos);
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(position.x + (width() / 2.0f), position.y + (height() / 2.0f));

    m_body_ptr = arkanoid_world->CreateBody(&groundBodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(width() * b2_coord_convertion_rate, height() * b2_coord_convertion_rate);

    auto *fixture = m_body_ptr->CreateFixture(&groundBox, 0.0f);

    map.insert({ fixture, this });
  }

  [[nodiscard]] ElementType get_type() const override { return PADDLE; }

  bool update_x(int const new_x)
  {
    Vector old_position = center_position();
    Vector pos = old_position;

    pos.x = new_x;
    set_position(pos);
    if (old_position.x < new_x + 1.0F && old_position.x > new_x - 1.0F) { return false; }// todo: möglicher feinschliff

    m_updated = true;
    return true;
  }

  void add_score(int const add)
  {
    m_score += add;
    m_updated = true;
  }

  bool did_update() override
  {
    if (m_updated) {
      m_updated = false;
      return true;
    }

    return false;
  }


  void set_position(Vector const pos) override
  {
    auto const position = convert_to_b2_coords(pos);
    m_body_ptr->SetTransform({ position.x, position.y }, m_body_ptr->GetAngle());
  }

  void set_is_controlled_by_this_game_instance(bool const c) { m_is_controlled_by_this_game_instance = c; }

  [[nodiscard]] Vector center_position() const override
  {
    auto pos = m_body_ptr->GetPosition();
    return convert_to_arkanoid_coords({ pos.x, pos.y });
  }

  [[nodiscard]] int width() const override { return paddle_width; }
  [[nodiscard]] int height() const override { return paddle_height; }
  [[nodiscard]] bool is_controlled_by_this_game_instance() const { return m_is_controlled_by_this_game_instance; }
  [[nodiscard]] ftxui::Color color() const override
  {
    return (is_controlled_by_this_game_instance()) ? ftxui::Color::White : ftxui::Color::GrayDark;
  }
  [[nodiscard]] int score() const { return m_score; }
};

class Ball : public Element
{

private:
  b2Body *m_body_ptr = nullptr;
  bool m_updated{ false };
  Paddle *m_paddle_ptr{ nullptr };

  friend void parse_game_element(Element *, GameElement const &);

public:
  explicit Ball(Vector const pos, b2World *arkanoid_world, std::map<b2Fixture *, Element *> &map)
    : Element{ ftxui::Color::Red }
  {
    auto const position = convert_to_b2_coords(pos);
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(position.x, position.y);
    // bodyDef.userData = b2BodyUserData{}; // todo: verlinkung auf dieses objekt.
    m_body_ptr = arkanoid_world->CreateBody(&bodyDef);
    bodyDef.linearDamping = 0.0F;
    bodyDef.angularDamping = 0.0F;

    b2CircleShape dynamicBox;
    dynamicBox.m_radius = 1.0F * b2_coord_convertion_rate;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0F;
    fixtureDef.friction = 0.0F;// todo: andere friction = fehlerhaftes verhalten

    auto *fixture = m_body_ptr->CreateFixture(&fixtureDef);
    m_body_ptr->SetLinearVelocity({ 2.0F, 3.0F });
    m_body_ptr->SetAngularVelocity(0);
    m_body_ptr->SetFixedRotation(true);

    map.insert({ fixture, this });
  }

  [[nodiscard]] ElementType get_type() const override { return BALL; }

  bool did_update() override
  {

    if (m_updated) {
      m_updated = false;
      return true;
    }
    return false;
  }

  void set_position(Vector const pos) override
  {
    auto const position = convert_to_b2_coords(pos);
    m_body_ptr->SetTransform({ position.x, position.y }, m_body_ptr->GetAngle());
  }

  void set_velocity(Vector const vector)
  {
    if (m_body_ptr != nullptr) { m_body_ptr->SetLinearVelocity({ vector.x, vector.y }); }
  }

  void add_to_next_update() { m_updated = true; }

  void set_last_paddle(Paddle *const paddle) { m_paddle_ptr = paddle; }

  [[nodiscard]] Vector center_position() const override
  {
    auto pos = m_body_ptr->GetPosition();
    return convert_to_arkanoid_coords({ pos.x, pos.y });
  }

  [[nodiscard]] int width() const override { return ball_radius; }
  [[nodiscard]] int height() const override { return ball_radius; }
  [[nodiscard]] Vector velocity() const
  {
    auto vel = m_body_ptr->GetLinearVelocity();
    return { vel.x, vel.y };
  }
  [[nodiscard]] Paddle *last_paddle() const { return m_paddle_ptr; }
};

class Brick : public Element
{

private:
  b2Body *m_body_ptr = nullptr;
  int m_duration;// todo -> variable duration

  friend void parse_game_element(Element *, GameElement const &);

public:
  explicit Brick(Vector const pos, b2World *arkanoid_world, std::map<b2Fixture *, Element *> &map, int const duration)
    : Element{}, m_duration{ duration }
  // todo: b2world muss als pointer, da sonst make_unique nicht funktioniert
  {

    auto const position = convert_to_b2_coords(pos.add(width() / 2.0F, height() / 2.0F));
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(position.x, position.y);

    m_body_ptr = arkanoid_world->CreateBody(&groundBodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(width() * b2_coord_convertion_rate, height() * b2_coord_convertion_rate);

    auto *fixture = m_body_ptr->CreateFixture(&groundBox, 0.0f);

    map.insert({ fixture, this });
  }

  void hit() { --m_duration; }


  [[nodiscard]] ElementType get_type() const override { return BRICK; }
  void set_position(Vector const pos) override
  {
    auto const position = convert_to_b2_coords(pos);
    m_body_ptr->SetTransform({ position.x, position.y }, m_body_ptr->GetAngle());
  }

  [[nodiscard]] Vector center_position() const override
  {
    auto pos = m_body_ptr->GetPosition();
    return convert_to_arkanoid_coords({ pos.x, pos.y });
  }

  [[nodiscard]] int width() const override { return brick_width; }
  [[nodiscard]] int height() const override { return brick_height; }
  [[nodiscard]] bool exists() override
  {
    if (m_body_ptr != nullptr && m_duration <= 0 && m_body_ptr->IsEnabled()) { m_body_ptr->SetEnabled(false); }
    return m_duration > 0;
  }
  [[nodiscard]] bool did_update() override { return false; }
  [[nodiscard]] int duration() const { return m_duration; }
  [[nodiscard]] ftxui::Color color() const override
  {
    auto const duration = static_cast<float>(m_duration);
    float const ratio = duration / brick_max_duration;

    if (ratio >= (4.0F / 6.0F)) {
      return ftxui::Color::Yellow;
    } else if (ratio >= (2.0F / 6.0F)) {
      return ftxui::Color::Green;
    }

    return ftxui::Color::White;
  }
};

void fill_game_element(GameElement *const &game_element, arkanoid::Element const *element)
{
  auto *position = new ElementPosition;

  auto const pos = element->center_position();

  float const x = pos.x;
  float const y = pos.y;

  position->set_x(x);
  position->set_y(y);

  game_element->set_id(element->id());
  game_element->set_allocated_element_position(position);

  if (element->get_type() == BALL) {
    auto const *ball = dynamic_cast<const Ball *>(element);
    auto *net_ball = new NetBall;

    auto vel = ball->velocity();

    net_ball->set_velocity_x(vel.x);
    net_ball->set_velocity_y(vel.y);

    game_element->set_allocated_ball(net_ball);
  } else if (element->get_type() == BRICK) {
    auto const *brick = dynamic_cast<const Brick *>(element);
    auto *net_brick = new NetBrick;

    net_brick->set_duration(brick->duration());

    game_element->set_allocated_brick(net_brick);
  } else if (element->get_type() == PADDLE) {
    auto const *paddle = dynamic_cast<const Paddle *>(element);
    auto *net_paddle = new NetPaddle;

    net_paddle->set_controlled_by_sender(paddle->is_controlled_by_this_game_instance());
    net_paddle->set_score(0);// todo

    game_element->set_allocated_paddle(net_paddle);
  }
}

void fill_game_update(GameUpdate *update, std::vector<arkanoid::Element *> const &elements)
{

  std::for_each(elements.begin(), elements.end(), [&update](arkanoid::Element const *element_ptr) {
    if (element_ptr == nullptr) { return; }
    GameElement *game_element = update->add_element();

    fill_game_element(game_element, element_ptr);
  });
}

void parse_game_element(Element *element, GameElement const &net_element)
{
  element->set_position({ net_element.element_position().x(), net_element.element_position().y() });
  // element->invert_position();

  if (element->get_type() == BALL && net_element.has_ball()) {
    auto *ball = dynamic_cast<Ball *>(element);
    ball->set_velocity(
      arkanoid::Vector{ net_element.ball().velocity_x(), net_element.ball().velocity_y() } /*.invert()*/);
  } else if (element->get_type() == BRICK && net_element.has_brick()) {
    auto *brick = dynamic_cast<Brick *>(element);
    brick->m_duration = net_element.brick().duration();
  } else if (element->get_type() == PADDLE && net_element.has_paddle()) {
    auto *paddle = dynamic_cast<Paddle *>(element);
    paddle->set_is_controlled_by_this_game_instance(!net_element.paddle().controlled_by_sender());
    // todo: score
  }
}

void parse_game_update(std::map<int, std::unique_ptr<Element>> &map,
  GameUpdate const &update,
  b2World *world,
  std::map<b2Fixture *, arkanoid::Element *> &b2_map)
{
  for (int i = 0; i < update.element_size(); ++i) {
    auto const net_element = update.element(i);
    Vector position{ net_element.element_position().x(), net_element.element_position().y() };

    bool new_id{ !map.contains(net_element.id()) };

    if (new_id) {
      std::unique_ptr<Element> arkanoid_element_ptr = nullptr;

      if (net_element.has_ball()) {
        arkanoid_element_ptr = std::make_unique<Ball>(position, world, b2_map);
      } else if (net_element.has_brick()) {
        arkanoid_element_ptr = std::make_unique<Brick>(position, world, b2_map, net_element.brick().duration());
      } else if (net_element.has_paddle()) {
        arkanoid_element_ptr = std::make_unique<Paddle>(position, world, b2_map);
      }
      arkanoid_element_ptr->m_id = net_element.id();
      parse_game_element(arkanoid_element_ptr.get(), net_element);
      insert_element(map, arkanoid_element_ptr);
    } else {
      parse_game_element(map.find(net_element.id())->second.get(), net_element);
    }
  }
}

}// namespace arkanoid

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

template<typename T1>//
void draw(ftxui::Canvas &canvas, T1 &drawable)
{
  if (!drawable.exists()) { return; }

  for (int y = drawable.top(); y <= drawable.bottom(); y += 2) {
    canvas.DrawBlockLine(drawable.left(), y, drawable.right(), y, drawable.color());
  }
}

int find_id_of_controller(std::map<int, std::unique_ptr<arkanoid::Element>> const &element_map, bool const as_host)
{
  int const id = (as_host) ? 0 : 1;

  if (element_map.contains(id)) { return id; }

  return -1;
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

template<typename T1, typename T2>//
[[nodiscard]] T1 dynamic_multiple_cast(T2 t2_1, T2 t2_2)// todo: ...
{
  auto t1_1 = dynamic_cast<T1>(t2_1);
  auto t1_2 = dynamic_cast<T1>(t2_2);

  if (t1_1 == nullptr) { return t1_2; }
  return t1_1;
}

class ContactListener : public b2ContactListener
{
private:
  std::map<b2Fixture *, arkanoid::Element *> &m_b2_element_map;

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
      brick_ptr->hit();
      if (ball_ptr->last_paddle() != nullptr) { ball_ptr->last_paddle()->add_score(1); }
    }
    if (ball_ptr != nullptr && paddle_ptr != nullptr) {
      if (paddle_ptr->is_controlled_by_this_game_instance()) {
        // ball_ptr->set_last_paddle(paddle_ptr);
        ball_ptr->add_to_next_update();
      }
    }
  }

public:
  explicit ContactListener(std::map<b2Fixture *, arkanoid::Element *> &b2_map) : m_b2_element_map{ b2_map } {}


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
      }
    }
  }
};

void build_world_border(b2World *world)
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
    body->CreateFixture(&box, 1.0f);
  };

  // links
  generate(arkanoid::playing_field_left - 2, arkanoid::playing_field_top - 12, 1, playing_field_height + 24);
  // rechts
  generate(arkanoid::playing_field_right + 1, arkanoid::playing_field_top - 12, 1, playing_field_height + 24);
  // oben
  generate(arkanoid::playing_field_left - 2, arkanoid::playing_field_top - 11, playing_field_width + 4, 1);
  // unten
  generate(arkanoid::playing_field_left - 2, arkanoid::playing_field_bottom + 10, playing_field_width + 4, 1);
}

int main()
{
  using namespace ftxui;
  using namespace arkanoid;


  show_connection_methods([](bool const as_host, int const &port) {
    std::mutex element_mutex;
    std::mutex game_update_mutex;
    std::map<int, std::unique_ptr<arkanoid::Element>> element_map;
    std::map<b2Fixture *, arkanoid::Element *> b2_element_map;
    connection::Connection connection;
    auto screen = ScreenInteractive::FitComponent();
    int const paddle_y{ playing_field_bottom - paddle_height };
    b2World arkanoid_world{ { 0, 0 } };
    ContactListener listener{ b2_element_map };
    arkanoid_world.SetContactListener(&listener);
    arkanoid::Vector const paddle_position = { (canvas_width / 2) - (paddle_width / 2), paddle_y };

    build_world_border(&arkanoid_world);

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

        std::unique_ptr<arkanoid::Element> paddle =
          std::make_unique<Paddle>(paddle_position, &arkanoid_world, b2_element_map);
        std::unique_ptr<arkanoid::Element> paddle_enemy = std::make_unique<Paddle>(
          paddle_position.sub(0, paddle_y).add(0, playing_field_top), &arkanoid_world, b2_element_map);

        auto *paddle_ptr = dynamic_cast<Paddle *>(paddle.get());
        paddle_ptr->set_is_controlled_by_this_game_instance(true);

        auto *paddle_enemy_ptr = dynamic_cast<Paddle *>(paddle_enemy.get());
        paddle_enemy_ptr->set_is_controlled_by_this_game_instance(false);

        std::unique_ptr<arkanoid::Element> ball =
          std::make_unique<Ball>(paddle_position.add(paddle_width / 2, -10), &arkanoid_world, b2_element_map);

        insert_element(element_map, paddle);
        insert_element(element_map, paddle_enemy);
        insert_element(element_map, ball);

        generate_bricks(element_map, &arkanoid_world, b2_element_map);

        GameUpdate update;
        arkanoid::fill_game_update(&update, map_values(element_map));

        connection.send(update, true);
      }
    }

    if (connection.has_connected()) {
      Paddle *controlling_paddle_ptr{ nullptr };
      Paddle *enemy_paddle_ptr{ nullptr };
      int mouse_x{ paddle_position.x_i() };
      std::vector<arkanoid::Element *> updated_elements;

      auto renderer = Renderer([&] {
        Canvas can = Canvas(canvas_width, canvas_height);
        int your_score{ 0 }, enemy_score{ 0 };

        {
          std::lock_guard<std::mutex> lock{ element_mutex };
          std::for_each(
            element_map.begin(), element_map.end(), [&can](auto const &pair) { draw(can, *(pair.second)); });

          if (controlling_paddle_ptr != nullptr) { your_score = controlling_paddle_ptr->score(); }
          if (enemy_paddle_ptr != nullptr) { enemy_score = enemy_paddle_ptr->score(); }
        }

        can.DrawText(15,
          playing_field_bottom + 10,
          fmt::format("Synchronisationen versendet: {}", connection.game_updates_sent()));
        can.DrawText(15,
          playing_field_bottom + 15,
          fmt::format("Synchronisationen empfangen: {}", connection.game_updates_received()));

        can.DrawText(playing_field_right - 30, playing_field_bottom + 10, fmt::format("Deine Punkte: {}", your_score));
        can.DrawText(playing_field_right - 30, playing_field_top + 15, fmt::format("Punkte (Gegner): {}", enemy_score));


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

      constexpr int frame_rate = 40.0;

      auto game_update_loop = [&element_map,
                                &element_mutex,
                                &controlling_paddle_ptr,
                                &enemy_paddle_ptr,
                                as_host,
                                &updated_elements,
                                &mouse_x,
                                frame_rate,
                                &arkanoid_world]() {// todo - mouse_x als kopie buggt
        {
          {
            std::lock_guard<std::mutex> lock{ element_mutex };

            if (controlling_paddle_ptr == nullptr || enemy_paddle_ptr == nullptr) {
              std::for_each(
                element_map.begin(), element_map.end(), [&controlling_paddle_ptr, &enemy_paddle_ptr](auto const &pair) {
                  if (pair.second->get_type() == PADDLE) {
                    auto *const paddle_ptr = dynamic_cast<Paddle *>(pair.second.get());

                    if (paddle_ptr != nullptr) {
                      if (paddle_ptr->is_controlled_by_this_game_instance()) {
                        controlling_paddle_ptr = paddle_ptr;
                      } else {
                        enemy_paddle_ptr = paddle_ptr;
                      }
                    }
                  }
                });
            }

            if (controlling_paddle_ptr != nullptr) {
              int new_paddle_x = mouse_x;
              int const half_width = controlling_paddle_ptr->width() / 2;
              std::pair<int, int> constrains{ playing_field_left + half_width, playing_field_right - half_width };

              if (new_paddle_x < constrains.first) { new_paddle_x = constrains.first; }
              if (new_paddle_x > constrains.second) { new_paddle_x = constrains.second; }// todo: auslagern
              controlling_paddle_ptr->update_x(new_paddle_x);
            }
          }

          arkanoid_world.Step(1.0F / (frame_rate), 4, 2);

          std::for_each(element_map.begin(), element_map.end(), [&updated_elements](auto const &pair) {
            if (pair.second->did_update()) { updated_elements.push_back(pair.second.get()); }
          });
        }

      };

      constexpr auto frame_time_budget{ std::chrono::seconds(1) / frame_rate };

      while (!loop.HasQuitted()) {
        const auto frame_start_time{ std::chrono::steady_clock::now() };

        screen.RequestAnimationFrame();// wichtig, da sonst keine aktualisierung, wenn aus fokus
        loop.RunOnce();

        game_update_loop();

        if (!updated_elements.empty()) {
          create_and_send_new_game_update(updated_elements, connection, game_update_mutex);
          updated_elements.clear();
        }

        const auto frame_end_time{ std::chrono::steady_clock::now() };
        const auto unused_frame_time{ frame_time_budget - (frame_end_time - frame_start_time) };
        if (unused_frame_time > std::chrono::seconds(0)) { std::this_thread::sleep_for(unused_frame_time); }
      }

      std::lock_guard<std::mutex> game_update_lock{ game_update_mutex };
      connection.close();
    }
  });

  google::protobuf::ShutdownProtobufLibrary();

  return EXIT_SUCCESS;
}
