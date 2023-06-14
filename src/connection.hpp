#ifndef CONNECTION_CPP
#define CONNECTION_CPP

#include "asio.hpp"
#include <fmt/format.h>
#include <iostream>
#include <queue>

#include "arkanoid.pb.h"
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
#endif