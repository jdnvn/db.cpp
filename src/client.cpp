#include <iostream>
#include <asio.hpp>

int main(int argc, char **argv) {
  std::string command = argv[1];

  asio::io_context io_context;
  asio::ip::tcp::socket socket(io_context);

  asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"), 5433);
  socket.connect(endpoint);

  asio::write(socket, asio::buffer(command));
  char buffer[1024];
  std::size_t bytes_received = socket.read_some(asio::buffer(buffer, sizeof(buffer)));

  std::cout << "Received: " << std::string(buffer, bytes_received) << std::endl;

  return 0;
}
