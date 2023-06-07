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
  std::mutex m_receivers_mutex, m_conntected_mutex, m_send_mutex;

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

  void close() { m_socket.close(); }

  void send(GameUpdate &game_update, bool const add_to_queue_if_currently_in_use = false)
  {

    auto recursive{ [this]() {
      if (m_next_game_updates.empty()) { return; }
      GameUpdate update = m_next_game_updates.front();
      GameUpdate update_copy = update;// todo - einfacher ?
      m_next_game_updates.pop();
      send(update_copy);
    } };

    {
      std::lock_guard<std::mutex> lock{ m_send_mutex };
      if (m_sending) {
        if (add_to_queue_if_currently_in_use) { m_next_game_updates.push(game_update); }

        return;
      }
    }

    auto message = game_update.SerializeAsString();

    std::thread([this, message, recursive]() {
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
      recursive();
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
};

}// namespace connection

namespace arkanoid {

int constexpr canvas_width{ 164 }, canvas_height{ 180 };
int constexpr playing_field_top{ 5 }, playing_field_bottom{ canvas_height - 10 }, playing_field_left{ 1 },
  playing_field_right{ canvas_width - 2 };
int constexpr ball_radius{ 1 }, ball_speed{ 1 };
int constexpr paddle_width{ 14 }, paddle_height{ 1 };
int constexpr brick_width{ 14 }, brick_height{ 5 };
int constexpr num_bricks_y{ 8 };
int constexpr brick_distance_x{ 2 }, brick_distance_y{ 3 };

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

class Element
{
public:
  [[nodiscard]] virtual ElementType get_type() const = 0;
  virtual void update() = 0;
  virtual void set_position(Vector const position) = 0;
  virtual Vector center_position() const = 0;
  virtual int width() const = 0;
  virtual int height() const = 0;

protected:
  int m_id;
  ftxui::Color m_color;

  friend void fill_game_element(GameElement *const &, arkanoid::Element const *);// todo: außerhalb von namespace ??
  friend void parse_game_update(std::map<int, std::unique_ptr<Element>> &, GameUpdate const &, b2World *);
  friend void parse_game_element(Element *, GameElement const &);

public:
  IdGenerator static id_generator;

  explicit Element(ftxui::Color const color = ftxui::Color::White) : m_color{ color }, m_id{ id_generator.next() } {}

  void invert_position()
  {
    Vector const position = center_position();
    set_position({ playing_field_right - position.x, playing_field_bottom - position.y });
  }

  [[nodiscard]] int left() const { return std::round(center_position().x - (width() / 2.0f)); }

  [[nodiscard]] int right() const { return left() + width(); }

  [[nodiscard]] int top() const { return std::round(center_position().y - (height() / 2.0f)); }

  [[nodiscard]] int bottom() const { return top() + height(); }

  [[nodiscard]] bool exists() const { return true; }

  [[nodiscard]] int id() const { return m_id; }

  [[nodiscard]] ftxui::Color color() const { return m_color; };
};

IdGenerator Element::id_generator = IdGenerator{ 0 };

class Ball : public Element
{

private:
  b2Body *m_body_ptr = nullptr;
  Vector m_velocity{ 0.0f, 0.00025f };

  friend void parse_game_element(Element *, GameElement const &);

public:
  explicit Ball(Vector const position, b2World *arkanoid_world) : Element{ ftxui::Color::Red }
  {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(position.x, position.y);
    // bodyDef.userData = b2BodyUserData{}; // todo: verlinkung auf dieses objekt.
    m_body_ptr = arkanoid_world->CreateBody(&bodyDef);
    bodyDef.linearDamping = 0;
    bodyDef.angularDamping = 0;

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(width(), height());

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0F;

    m_body_ptr->CreateFixture(&fixtureDef);
    m_body_ptr->SetLinearVelocity({ 0.2F, 0.3F });
  }

  [[nodiscard]] ElementType get_type() const override { return BALL; }

  void update() override {}

  void set_position(Vector const position) override
  {
    m_body_ptr->SetTransform({ position.x, position.y }, m_body_ptr->GetAngle());
  }

  [[nodiscard]] Vector center_position() const override
  {
    auto pos = m_body_ptr->GetPosition();
    return { pos.x, pos.y };
  }

  [[nodiscard]] int width() const override { return ball_radius; }
  [[nodiscard]] int height() const override { return ball_radius; }
};

class Paddle : public Element
{
private:
  b2Body *m_body_ptr = nullptr;

public:
  explicit Paddle(Vector const position, b2World *arkanoid_world) : Element{}
  {
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(position.x + (width() / 2.0f), position.y + (height() / 2.0f));

    m_body_ptr = arkanoid_world->CreateBody(&groundBodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(width(), height());

    m_body_ptr->CreateFixture(&groundBox, 0.0f);
  }
  [[nodiscard]] ElementType get_type() const override { return PADDLE; }

  [[nodiscard]] bool update_left(int const new_x)
  {
    Vector pos = center_position();
    pos.x = new_x + (width() / 2);
    set_position(pos);
    return true;
  }

  void update() override {}

  void set_position(Vector const position) override
  {
    m_body_ptr->SetTransform({ position.x, position.y }, m_body_ptr->GetAngle());
  }

  [[nodiscard]] Vector center_position() const override
  {
    auto pos = m_body_ptr->GetPosition();
    return { pos.x, pos.y };
  }

  [[nodiscard]] int width() const override { return paddle_width; }
  [[nodiscard]] int height() const override { return paddle_height; }
};

class Brick : public Element
{

private:
  b2Body *m_body_ptr = nullptr;

public:
  explicit Brick(Vector const position, b2World *arkanoid_world)
    : Element{}// todo: b2world muss als pointer, da sonst make_unique nicht funktioniert
  {
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(position.x + (width() / 2.0f), position.y + (height() / 2.0f));

    m_body_ptr = arkanoid_world->CreateBody(&groundBodyDef);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(width(), height());

    m_body_ptr->CreateFixture(&groundBox, 0.0f);
  }

  void update() override {}
  [[nodiscard]] ElementType get_type() const override { return BRICK; }
  void set_position(Vector const position) override
  {
    m_body_ptr->SetTransform({ position.x, position.y }, m_body_ptr->GetAngle());
  }

  [[nodiscard]] Vector center_position() const override
  {
    auto pos = m_body_ptr->GetPosition();
    return { pos.x, pos.y };
  }

  [[nodiscard]] int width() const override { return brick_width; }
  [[nodiscard]] int height() const override { return brick_height; }
};

void fill_game_element(GameElement *const &game_element, arkanoid::Element const *element)
{
  ElementPosition *position = new ElementPosition;

  auto const pos = element->center_position();

  float const x = pos.x;
  float const y = pos.y;

  position->set_x(x);
  position->set_y(y);

  game_element->set_id(element->id());
  game_element->set_type(element->get_type());
  game_element->set_allocated_element_position(position);
}

void fill_game_update(GameUpdate *update, std::vector<arkanoid::Element *> const &elements)
{

  std::for_each(elements.begin(), elements.end(), [&update](auto const &element) {
    GameElement *game_element = update->add_element();

    fill_game_element(game_element, element);
  });
}

void parse_game_element(Element *element, GameElement const &net_element)
{
  element->set_position({ net_element.element_position().x(), net_element.element_position().y() });
  element->invert_position();

  if (element->get_type() == BALL) {
    Ball *ball = static_cast<Ball *>(element);
    ball->m_velocity = ball->m_velocity.invert();
  }
}

void parse_game_update(std::map<int, std::unique_ptr<Element>> &map, GameUpdate const &update, b2World *world)
{
  for (int i = 0; i < update.element_size(); ++i) {
    auto const net_element = update.element(i);
    Vector position{ net_element.element_position().x(), net_element.element_position().y() };

    bool new_id{ !map.contains(net_element.id()) };

    if (new_id) {
      std::unique_ptr<Element> arkanoid_element_ptr = nullptr;

      switch (net_element.type()) {
      case BALL: {
        arkanoid_element_ptr = std::make_unique<Ball>(position, world);
        break;
      }

      case PADDLE: {
        arkanoid_element_ptr = std::make_unique<Paddle>(position, world);
        break;
      }
      case BRICK: {
        arkanoid_element_ptr = std::make_unique<Brick>(position, world);
        break;
      }
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

void generate_bricks(std::map<int, std::unique_ptr<arkanoid::Element>> &elements, b2World *world)
{
  using namespace arkanoid;

  int const num_bricks_x = (playing_field_right - brick_distance_x) / (brick_width + brick_distance_x);

  int const total_brick_height = (num_bricks_y * (brick_height + brick_distance_y)) + brick_distance_y;
  int const top = (playing_field_bottom / 2) - (total_brick_height / 2);

  for (int i_x{ 0 }; i_x < num_bricks_x; ++i_x) {
    for (int i_y{ 0 }; i_y < num_bricks_y; ++i_y) {

      int const x{ playing_field_left + ((brick_distance_x + brick_width) * i_x) };
      int const y{ top + ((brick_distance_y + brick_height) * i_y) };

      std::unique_ptr<arkanoid::Element> brick = std::make_unique<arkanoid::Brick>(arkanoid::Vector{ x, y }, world);
      insert_element(elements, brick);
    }
  }
}

template<typename T1>//
void draw(ftxui::Canvas &canvas, T1 const &drawable)
{
  if (!drawable.exists()) { return; }

  for (int y = drawable.top(); y <= drawable.bottom(); y += 2) {
    canvas.DrawBlockLine(drawable.left(), y, drawable.right(), y, drawable.color());
  }
}

int find_id_of_controller(std::map<int, std::unique_ptr<arkanoid::Element>> const &element_map, bool const as_host)
{
  int id = (as_host) ? 0 : 1;

  if (element_map.contains(id)) { return id; }

  return -1;
}

void create_and_send_new_game_update(std::vector<arkanoid::Element *> const send_elements,
  connection::Connection &connection)
{
  std::thread([send_elements, &connection]() {// todo: sicherheit?
    GameUpdate update;
    arkanoid::fill_game_update(&update, send_elements);

    connection.send(update);
  })
    .detach();
}

void test_box2d()
{
  b2World world{ { 0, 0 } };

  b2BodyDef groundBodyDef;
  groundBodyDef.position.Set(0.0f, -10.0f);

  b2Body *groundBody = world.CreateBody(&groundBodyDef);

  b2PolygonShape groundBox;
  groundBox.SetAsBox(50.0f, 10.0f);

  groundBody->CreateFixture(&groundBox, 0.0f);

  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(0.0f, 4.0f);
  b2Body *body = world.CreateBody(&bodyDef);

  b2PolygonShape dynamicBox;
  dynamicBox.SetAsBox(1.0f, 1.0f);

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &dynamicBox;
  fixtureDef.density = 1.0f;
  fixtureDef.friction = 0.3f;

  body->CreateFixture(&fixtureDef);

  for (int32 i = 0; i < 60; ++i) {
    if (i < 20) {
      body->ApplyForceToCenter({ 0, -10.0f }, true);
    } else {
      body->ApplyForceToCenter({ 0, 10.0f }, true);
    }
    world.Step(1 / 60.0f, 6, 2);
    b2Vec2 position = body->GetPosition();
    printf("%4.2f %4.2f %4.2f\n", position.x, position.y);
  }
}

class ContactListener : public b2ContactListener
{

  void PreSolve(b2Contact *contact, const b2Manifold *oldManifold) { contact->SetRestitution(1.0F); }
};

void build_world_border(b2World *world) {}

int main()
{
  using namespace ftxui;
  using namespace arkanoid;


  show_connection_methods([](bool const as_host, int const &port) {
    std::mutex element_mutex;
    std::map<int, std::unique_ptr<arkanoid::Element>> element_map;
    connection::Connection connection;
    auto screen = ScreenInteractive::FitComponent();
    int const paddle_y{ playing_field_bottom - paddle_height };
    b2World arkanoid_world{ { 0, 0 } };
    ContactListener listener;
    arkanoid_world.SetContactListener(&listener);

    connection.register_receiver([&element_map, &element_mutex, &arkanoid_world](GameUpdate const &update) {
      std::lock_guard<std::mutex> lock{ element_mutex };
      arkanoid::parse_game_update(element_map, update, &arkanoid_world);
    });

    connect_to_peer(connection, as_host, port);
    show_connecting_state(connection);

    if (as_host) {
      arkanoid::Vector const paddle_position = { (canvas_width / 2) - (paddle_width / 2), paddle_y };

      std::unique_ptr<arkanoid::Element> paddle = std::make_unique<Paddle>(paddle_position, &arkanoid_world);
      std::unique_ptr<arkanoid::Element> paddle_enemy =
        std::make_unique<Paddle>(paddle_position.sub(0, paddle_y), &arkanoid_world);
      std::unique_ptr<arkanoid::Element> ball =
        std::make_unique<Ball>(paddle_position.add(paddle_width / 2, -10), &arkanoid_world);

      insert_element(element_map, paddle);
      insert_element(element_map, paddle_enemy);
      insert_element(element_map, ball);

      generate_bricks(element_map, &arkanoid_world);

      {
        std::lock_guard<std::mutex> lock{ element_mutex };
        GameUpdate update;
        arkanoid::fill_game_update(&update, map_values(element_map));

        connection.send(update, true);
      }
    }

    if (connection.has_connected()) {
      int controlling_paddle_id{ -1 };
      int mouse_x{};
      std::vector<arkanoid::Element *> updated_elements;

      auto renderer = Renderer([&] {
        Canvas can = Canvas(canvas_width, canvas_height);

        {
          std::lock_guard<std::mutex> lock{ element_mutex };
          std::for_each(
            element_map.begin(), element_map.end(), [&can](auto const &pair) { draw(can, *(pair.second)); });

          if (element_map.contains(2)) {
            auto pos = element_map.find(2)->second->center_position();
            can.DrawText(0, 20, fmt::format("x: {}, y: {}", pos.x, pos.y));
          }
        }

        long const timestamp =
          std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();

        can.DrawText(0, 0, std::to_string(timestamp));


        return canvas(can) | border;
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
                                &controlling_paddle_id,
                                as_host,
                                &updated_elements,
                                &mouse_x,
                                frame_rate,
                                &arkanoid_world]() {// todo - mouse_x als kopie buggt
        {
          {
            std::lock_guard<std::mutex> lock{ element_mutex };
            if (controlling_paddle_id < 0) { controlling_paddle_id = find_id_of_controller(element_map, as_host); }

            if (controlling_paddle_id >= 0) {
              arkanoid::Element *paddle_element = element_map.find(controlling_paddle_id)->second.get();
              arkanoid::Paddle *paddle_ptr = static_cast<Paddle *>(paddle_element);

              int const new_paddle_x =
                (mouse_x > playing_field_right - paddle_width) ? playing_field_right - paddle_width : mouse_x;

              if (paddle_ptr->update_left(new_paddle_x)) { updated_elements.push_back(paddle_element); }
            }

            std::for_each(element_map.begin(), element_map.end(), [](auto const &pair) { pair.second->update(); });
            arkanoid_world.Step(1.0F / (frame_rate), 1, 1);
          }
        }
      };

      constexpr auto frame_time_budget{ std::chrono::seconds(1) / frame_rate };

      while (!loop.HasQuitted()) {
        const auto frame_start_time{ std::chrono::steady_clock::now() };

        screen.RequestAnimationFrame();// wichtig, da sonst keine aktualisierung, wenn aus fokus
        loop.RunOnce();

        game_update_loop();

        if (!updated_elements.empty()) {
          create_and_send_new_game_update(updated_elements, connection);
          updated_elements.clear();
        }

        const auto frame_end_time{ std::chrono::steady_clock::now() };
        const auto unused_frame_time{ frame_time_budget - (frame_end_time - frame_start_time) };
        if (unused_frame_time > std::chrono::seconds(0)) { std::this_thread::sleep_for(unused_frame_time); }
      }

      connection.close();
    }
  });

  google::protobuf::ShutdownProtobufLibrary();

  return EXIT_SUCCESS;
}
