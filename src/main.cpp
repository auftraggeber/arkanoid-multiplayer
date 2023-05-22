#include <algorithm>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/dom/elements.hpp>
#include <iostream>

#include <fmt/format.h>
#include <vector>

#include "fmt/core.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/loop.hpp"
#include "ftxui/component/screen_interactive.hpp"

namespace menu {
using namespace ftxui;
void show_connecting()
{
  auto screen = ScreenInteractive::FitComponent();

  int dots = 1;

  auto renderer = Renderer([&dots] {
    std::string dots_string{ "" };

    for (int i = 0; i < dots; ++i) { dots_string += "."; }

    return text(fmt::format("Baue Verbindung auf{}", dots_string)) | border;
  });

  screen.Loop(renderer);
}

void show_waiting_for_connection(int const port) {}

void show_connection_methods()
{

  auto screen = ScreenInteractive::FitComponent();
  bool is_host{ false };

  std::string port_string{ "45678" };

  auto button_host = Button("Als Host auf Verbindung warten", [&] {
    screen.Exit();
    is_host = true;
  });

  auto button_client = Button("Zu einem Host verbinden", [&] {
    screen.Exit();
    is_host = false;
  });

  auto port_input = Input(&port_string, "Geben Sie den Port ein (Verbinden zu / Warten auf)");

  // The tree of components. This defines how to navigate using the keyboard.
  auto children = Container::Vertical({ port_input, Container::Horizontal({ button_host, button_client }) });


  auto render = Renderer(children, [&] {
    auto vb = vbox({ text("Willkommen. Wie möchten Sie sich zum Partner verbinden?"),
      separator(),
      text("Port:"),
      children->Render() });

    return vb | border;
  });


  screen.Loop(render);

  if (is_host) {
    show_waiting_for_connection(1);
  } else {
    show_connecting();
  }
}

}// namespace menu

int main()
{
  menu::show_connection_methods();
  return 0;
}
