#include <algorithm>
#include <asm-generic/errno.h>
#include <cstdlib>
#include <exception>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <iostream>

#include <fmt/format.h>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <chrono>

#include "boost/asio.hpp"

#include "boost/asio/io_service.hpp"
#include "boost/asio/ip/address.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/ip/udp.hpp"
#include "boost/core/addressof.hpp"
#include "boost/system/system_error.hpp"
#include "fmt/core.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

namespace connection {

int constexpr default_port{ 45678 };
std::string const default_host{ "127.0.0.1" };
/* todo: in Klasse einbauen */

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
  std::vector<std::function<void()>> m_receivers;
  /* std::mutex receivers_mutex; todo: gibt fehler */

  void connect_to_on_this_thread(std::string const &host, int const &port)
  {
    try {
      if (m_socket.is_open()) { return; }
      m_socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(host), port));
      listen();
    } catch (boost::system::system_error &e) {
      fmt::print("Fehler beim Verbinden.");
    }
  }

  void wait_for_connection_on_this_thread(int const &port)
  {
    try {
      if (m_socket.is_open()) { return; }

      boost::asio::ip::tcp::acceptor acceptor{ io_service,
        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port) };

      acceptor.accept(m_socket);
      io_service.run();

      listen();
    } catch (boost::system::system_error &e) {
      fmt::print("Fehler beim Warten auf Verbindung.");
    }
  }

  void call_receivers()
  {
    /* std::lock_guard<std::mutex> lg{ receivers_mutex, std::adopt_lock }; */
    std::for_each(m_receivers.begin(), m_receivers.end(), [](std::function<void()> const &r) { r(); });
  }

  void listen()
  {
    std::thread{
      [this]() {
        /* ... */
        call_receivers();
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

  void send() const {}

  void register_receiver(std::function<void()> const &receiver)
  {
    /* std::lock_guard<std::mutex> lg{ receivers_mutex, std::adopt_lock }; */
    m_receivers.push_back(receiver);
  }

  [[nodiscard]] bool is_open() const { return m_socket.is_open(); } /* todo: auch nach abbruch true?? */
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
  int const m_width;
  int const m_height;
  ftxui::Color m_color;

public:
  explicit Element(Position const position,
    int const width,
    int const height,
    ftxui::Color const color = ftxui::Color::White)
    : m_position{ position }, m_width{ width }, m_height{ height }, m_color{ color }
  {}

  void draw(ftxui::Canvas &canvas) const
  {
    if (!exists()) { return; }

    int const start{ top() % 2 == 0 ? top() - 1 : top() }; /* Zeichenfehler beheben */

    for (int y = start; y <= bottom(); y += 2) { canvas.DrawBlockLine(left(), y, right(), y, m_color); }
  }

  [[nodiscard]] int left() const { return m_position.x; }

  [[nodiscard]] int right() const { return left() + m_width; }

  [[nodiscard]] int top() const { return m_position.y; }

  [[nodiscard]] int bottom() const { return top() + m_height; }

  [[nodiscard]] bool exists() const { return true; }
};

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

[[nodiscard]] void connect_to_peer(connection::Connection &connection, bool const as_host, int const port)
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

void show_connecting_state(connection::Connection const &connection)
{
  using namespace ftxui;

  auto screen = ScreenInteractive::TerminalOutput();
  screen.Clear();

  auto renderer = Renderer([&connection]() {
    return vbox({ text((connection.is_open()) ? "Verbunden" : "Konnte nicht verbinden") }) | border;
  });

  Loop{ &screen, std::move(renderer) }.RunOnce();

  screen.Exit();
}

void generate_bricks(std::vector<arkanoid::Element> &elements)
{
  using namespace arkanoid;

  int const num_bricks_x = (playing_field_right - brick_distance_x) / (brick_width + brick_distance_x);

  for (int i_x{ 0 }; i_x < num_bricks_x; ++i_x) {
    for (int i_y{ 0 }; i_y < num_bricks_y; ++i_y) {

      int const x{ playing_field_left + ((brick_distance_x + brick_width) * i_x) };
      int const y{ playing_field_top + ((brick_distance_y + brick_height) * i_y) };

      arkanoid::Brick brick{ { x, y } };
      elements.push_back(brick);
    }
  }
}

void game(connection::Connection const &connection)
{

  using namespace arkanoid;
  using namespace ftxui;

  Position const paddle_position = { (canvas_width / 2) - (paddle_width / 2), playing_field_bottom - paddle_height };

  Paddle paddle{ paddle_position };
  Ball ball{ paddle_position.add(paddle_width / 2, -10) };

  std::vector<arkanoid::Element> elements;

  elements.push_back(paddle);
  elements.push_back(ball);

  generate_bricks(elements);

  auto renderer = Renderer([&] {
    Canvas can = Canvas(canvas_width, canvas_height);

    // score
    /*can.DrawText(0, 0, "Score: ");*/

    std::for_each(elements.begin(), elements.end(), [&can](arkanoid::Element element) { element.draw(can); });

    // wrap the element with a border
    return canvas(can) | border;
  });

  // because we define the canvas size, we can set the screen to FitComponent
  auto screen = ScreenInteractive::FitComponent();

  Loop loop{ &screen, std::move(renderer) };

  loop.Run();
}

int main()
{
  show_connection_methods([](bool const &as_host, int const &port) {
    connection::Connection connection;
    connect_to_peer(connection, as_host, port);

    show_connecting_state(connection);

    if (connection.is_open()) {
      game(connection);

      connection.close();
    }
  });
  return EXIT_SUCCESS;
}
