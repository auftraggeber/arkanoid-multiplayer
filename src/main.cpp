#include <algorithm>
#include <exception>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>

#include <fmt/format.h>
#include <string>
#include <thread>
#include <vector>

#include "boost/asio.hpp"

#include "boost/asio/io_service.hpp"
#include "boost/asio/ip/address.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "boost/asio/ip/udp.hpp"
#include "boost/system/system_error.hpp"
#include "fmt/core.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"

namespace connection {

int constexpr default_port{ 45678 };
std::string const default_host{ "localhost" };

[[nodiscard]] boost::asio::ip::tcp::socket connect_to(std::string const &host, int const &port)
{
  using namespace boost::asio::ip;

  boost::asio::io_service io_service;

  tcp::socket socket{ io_service };

  socket.connect(tcp::endpoint(address::from_string(host), port));

  return socket;
}

[[nodiscard]] boost::asio::ip::tcp::socket wait_for_connection(int const &port)
{
  using namespace boost::asio::ip;

  boost::asio::io_service io_service;

  tcp::acceptor acceptor{ io_service, tcp::endpoint(tcp::v4(), port) };

  tcp::socket socket{ io_service };
  acceptor.accept(socket);

  return socket;
}

}// namespace connection

namespace menu {
using namespace ftxui;

ScreenInteractive show_connecting()
{
  auto screen = ScreenInteractive::TerminalOutput();

  int dots = 1;

  auto renderer = Renderer([&dots] {
    std::string dots_string{ "" };

    for (int i = 0; i < dots; ++i) { dots_string += "."; }

    return text(fmt::format("Baue Verbindung auf{}", dots_string)) | border;
  });

  screen.Loop(renderer);

  return screen;
}

[[nodiscard]] int calculate_port(std::string const &str)
{

  try {
    return std::stoi(str);
  } catch (std::exception e) {}
  return connection::default_port;
}

ScreenInteractive show_waiting_for_connection(int const &port = connection::default_port)
{
  auto screen = ScreenInteractive::TerminalOutput();

  return screen;
}

void show_connection_methods(std::function<void(bool const &, int const &)> callback)
{

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


  int const port = calculate_port(port_string);

  callback(is_host, port);
}

}// namespace menu

[[nodiscard]] boost::asio::ip::tcp::socket connect_to_peer(bool const as_host, int const port)
{
  if (as_host) { return connection::wait_for_connection(port); }

  return connection::connect_to(connection::default_host, port);
}

[[nodiscard]] ftxui::ScreenInteractive show_connecting(bool const as_host, int const port)
{
  if (as_host) {
    return menu::show_waiting_for_connection(port);
  } else {
    return menu::show_connecting();
  }
}

int main()
{
  menu::show_connection_methods([](bool const &as_host, int const &port) {
    auto screen = show_connecting(as_host, port);

    try {
      boost::asio::ip::tcp::socket peer = connect_to_peer(as_host, port);
      screen.Exit();
    } catch (boost::system::system_error e) {
      fmt::print("Es konnte keine Verbindung hergestellt werden.");
      screen.Exit();
    }
  });
  return 0;
}
