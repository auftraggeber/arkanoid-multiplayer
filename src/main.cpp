#include <algorithm>
#include <asm-generic/errno.h>
#include <cstdarg>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <google/protobuf/stubs/common.h>
#include <iostream>

#include <fmt/format.h>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include <chrono>

#include "boost/asio.hpp"

#include "boost/asio/buffer.hpp"
#include "boost/asio/execution/execute.hpp"
#include "boost/asio/io_service.hpp"
#include "boost/asio/ip/address.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/ip/udp.hpp"
#include "boost/asio/read.hpp"
#include "boost/asio/registered_buffer.hpp"
#include "boost/asio/streambuf.hpp"
#include "boost/core/addressof.hpp"
#include "boost/system/detail/error_code.hpp"
#include "boost/system/system_error.hpp"
#include "fmt/core.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

#include "arkanoid.pb.h"

namespace connection {

int constexpr default_port{ 45678 };
std::string const default_host{ "127.0.0.1" };

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
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::socket m_socket{ io_service };
  std::vector<std::function<void(GameUpdate const &)>> m_receivers;
  bool m_connected{ false };// todo: unschöne Lösung
  std::mutex receivers_mutex, m_conntected_mutex;

  void connect_to_on_this_thread(std::string const &host, int const &port)
  {
    try {
      if (has_connected()) { return; }

      std::lock_guard<std::mutex>{ m_conntected_mutex, std::adopt_lock };
      m_socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port));
      m_connected = true;
      read_listener();
      io_service.run();

    } catch (boost::system::system_error &e) {
      fmt::print("Fehler beim Verbinden.");
    }
  }

  void wait_for_connection_on_this_thread(int const &port)
  {
    try {
      if (has_connected()) { return; }

      boost::asio::ip::tcp::acceptor acceptor{ io_service,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port) };


      std::lock_guard<std::mutex>{ m_conntected_mutex, std::adopt_lock };
      acceptor.accept(m_socket);
      m_connected = true;
      read_listener();
      io_service.run();
    } catch (boost::system::system_error &e) {
      fmt::print("Fehler beim Warten auf Verbindung.");
    }
  }

  void call_receivers(GameUpdate const &update)// todo kopie besser?
  {
    std::lock_guard<std::mutex>{ receivers_mutex, std::adopt_lock };
    std::for_each(m_receivers.begin(),
      m_receivers.end(),
      [update](std::function<void(GameUpdate const &)> const &receiver) { receiver(update); });
  }

  void read_listener()
  {
    std::thread{
      [this]() {
        std::array<char, 4096> buffer;

        boost::asio::async_read(m_socket,
          boost::asio::buffer(buffer),
          boost::asio::transfer_all(),
          [](boost::system::error_code const &ec, std::size_t bytes_transferred) { std::cout << "done reading!"; });
      }
    }.detach();
  }


public:
  explicit Connection() = default;

  Connection(Connection const &) = delete;
  Connection &operator=(Connection const &) = delete;

  std::thread connect_to(
    std::string const &host,
    int const &port,
    std::function<void()> const &on_finish = []() {})
  {
    return std::thread{ [host, port, this, on_finish]() {
      connect_to_on_this_thread(host, port);
      on_finish();
    } };
  }

  std::thread wait_for_connection(
    int const &port,
    std::function<void()> const &on_finish = []() {})
  {
    return std::thread{ [port, this, on_finish]() {
      wait_for_connection_on_this_thread(port);
      on_finish();
    } };
  }

  void close() { m_socket.close(); }

  [[nodiscard]] bool send(GameUpdate &game_update)
  {
    if (!has_connected()) { return false; }

    std::thread([game_update, this]() {// todo: kopie besser?
      auto buffer = boost::asio::buffer(game_update.SerializeAsString());
      m_socket.send(buffer);
    })
      .detach();

    return true;
  }

  void register_receiver(std::function<void(GameUpdate const &update)> const &receiver)
  {
    std::lock_guard<std::mutex>{ receivers_mutex, std::adopt_lock };// todo: varname?
    m_receivers.push_back(receiver);
  }

  [[nodiscard]] bool has_connected()
  {
    std::lock_guard<std::mutex>{ m_conntected_mutex, std::adopt_lock };
    return m_connected;
  }
};

}// namespace connection

namespace arkanoid {

int constexpr canvas_width{ 164 }, canvas_height{ 100 };
int constexpr playing_field_top{ 5 }, playing_field_bottom{ canvas_height - 10 }, playing_field_left{ 1 },
  playing_field_right{ canvas_width - 2 };
int constexpr ball_radius{ 1 }, ball_speed{ 1 };
int constexpr paddle_width{ 14 }, paddle_height{ 1 };
int constexpr brick_width{ 14 }, brick_height{ 5 };
int constexpr num_bricks_y{ 5 };
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

struct Position
{
public:
  int x;
  int y;

  [[nodiscard]] Position add(int const &x, int const &y) const { return Position{ this->x + x, this->y + y }; }
};

class Element
{

protected:
  Position m_position;
  int m_id;// todo: const kann nicht kopiert werden - aufgefallen bei: map[id] = obj
  int const m_width;
  int const m_height;
  ftxui::Color m_color;

  friend void fill_game_element(GameElement *const &, arkanoid::Element const &);// todo: außerhalb von namespace ??
  friend std::vector<arkanoid::Element> parse_game_update(GameUpdate const &);

public:
  IdGenerator static id_generator;

  explicit Element(Position const position,
    int const width,
    int const height,
    ftxui::Color const color = ftxui::Color::White)
    : m_position{ position }, m_width{ width }, m_height{ height }, m_color{ color }, m_id{ id_generator.next() }
  {}

  [[nodiscard]] int left() const { return m_position.x; }

  [[nodiscard]] int right() const { return left() + m_width; }

  [[nodiscard]] int top() const { return m_position.y; }

  [[nodiscard]] int bottom() const { return top() + m_height; }

  [[nodiscard]] bool exists() const { return true; }

  [[nodiscard]] int id() const { return m_id; }

  [[nodiscard]] ftxui::Color color() const { return m_color; };
};

IdGenerator Element::id_generator = IdGenerator{ 0 };// todo: static init in Klasse

class Ball : public Element
{

public:
  explicit Ball(Position const position) : Element{ position, ball_radius, ball_radius, ftxui::Color::Red } {}
};

class Paddle : public Element
{
public:
  explicit Paddle(Position const position) : Element{ position, paddle_width, paddle_height } {}
};

class Brick : public Element
{

public:
  explicit Brick(Position const position) : Element{ position, brick_width, brick_height } {}
};

void fill_game_element(GameElement *const &game_element, arkanoid::Element const &element)// todo
{
  ElementPosition *position = new ElementPosition{};// muss auf heap, da sonst null-pointer, verfahren da hinter?

  int const x = element.m_position.x;
  int const y = element.m_position.y;

  position->set_x(x);
  position->set_y(y);

  game_element->set_id(element.id());
  game_element->set_type(BALL);
  game_element->set_allocated_element_position(position);
}

void fill_game_update(GameUpdate *update, std::vector<arkanoid::Element> const &elements)
{

  std::for_each(elements.begin(), elements.end(), [&update](arkanoid::Element const &element) {
    GameElement *game_element = update->add_element();

    fill_game_element(game_element, element);
  });
}

[[nodiscard]] std::vector<arkanoid::Element> parse_game_update(GameUpdate const &update)
{
  std::vector<arkanoid::Element> elements;

  for (int i = 0; i < update.element_size(); ++i) {
    auto const element = update.element(i);
    Position position{ element.element_position().x(), element.element_position().y() };

    switch (element.type()) {
    case BALL:
      Ball ball{ position };
      ball.m_id = element.id();
      elements.push_back(ball);
    }
  }

  return elements;
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
    connection.wait_for_connection(port).join();
  } else {
    connection.connect_to(connection::default_host, port).join();
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

void insert_element(std::map<int, arkanoid::Element> &map, arkanoid::Element const &element)
{
  map.insert(std::pair<int, arkanoid::Element>{ element.id(), element });
}

void generate_bricks(std::map<int, arkanoid::Element> &elements)
{
  using namespace arkanoid;

  int const num_bricks_x = (playing_field_right - brick_distance_x) / (brick_width + brick_distance_x);

  for (int i_x{ 0 }; i_x < num_bricks_x; ++i_x) {
    for (int i_y{ 0 }; i_y < num_bricks_y; ++i_y) {

      int const x{ playing_field_left + ((brick_distance_x + brick_width) * i_x) };
      int const y{ playing_field_top + ((brick_distance_y + brick_height) * i_y) };

      arkanoid::Brick brick{ { x, y } };
      insert_element(elements, brick);
    }
  }
}

template<typename T1>//
void draw(ftxui::Canvas &canvas, T1 const &drawable)
{
  if (!drawable.exists()) { return; }

  int const start{ drawable.top() % 2 == 0 ? drawable.top() - 1 : drawable.top() }; /* Zeichenfehler beheben */

  for (int y = start; y <= drawable.bottom(); y += 2) {
    canvas.DrawBlockLine(drawable.left(), y, drawable.right(), y, drawable.color());
  }
}

template<typename T1, typename T2>// todo: gibt es schon eine implementierung?
[[nodiscard]] std::vector<T2> map_values(std::map<T1, T2> const &map)
{
  std::vector<T2> vector;

  std::for_each(map.begin(), map.end(), [&vector](std::pair<T1, T2> const &p) { vector.push_back(p.second); });

  return vector;
}

int main()
{
  using namespace ftxui;
  using namespace arkanoid;

  show_connection_methods([](bool const &as_host, int const &port) {
    std::mutex element_mutex;
    std::map<int, arkanoid::Element> element_map;
    connection::Connection connection;
    auto screen = ScreenInteractive::FitComponent();

    connection.register_receiver([&element_map, &element_mutex](GameUpdate const &update) {
      std::vector<arkanoid::Element> elements_to_update = arkanoid::parse_game_update(update);

      std::lock_guard<std::mutex>{ element_mutex, std::adopt_lock };
      std::for_each(elements_to_update.begin(),
        elements_to_update.end(),
        [&element_map](arkanoid::Element const &element) { insert_element(element_map, element); });
    });

    connect_to_peer(connection, as_host, port);
    show_connecting_state(connection);

    if (as_host) {
      arkanoid::Position const paddle_position = { (canvas_width / 2) - (paddle_width / 2),
        playing_field_bottom - paddle_height };

      Paddle paddle{ paddle_position };
      Ball ball{ paddle_position.add(paddle_width / 2, -10) };

      insert_element(element_map, paddle);
      insert_element(element_map, ball);

      generate_bricks(element_map);

      {
        std::lock_guard<std::mutex>{ element_mutex, std::adopt_lock };
        GameUpdate update;
        arkanoid::fill_game_update(&update, map_values(element_map));


        if (!connection.send(update)) {
          connection.close();
          return EXIT_FAILURE;
        }// todo
      }
    }

    if (connection.has_connected()) {
      auto renderer = Renderer([&] {
        Canvas can = Canvas(canvas_width, canvas_height);

        {
          std::lock_guard<std::mutex>{ element_mutex, std::adopt_lock };
          std::for_each(element_map.begin(), element_map.end(), [&can](std::pair<int, arkanoid::Element> const &pair) {
            draw(can, pair.second);
          });
        }


        return canvas(can) | border;
      });

      Loop loop{ &screen, std::move(renderer) };

      loop.Run();
      connection.close();
    }
  });

  return EXIT_SUCCESS;
}
