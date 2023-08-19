#include <iostream>
#include <asio.hpp>
#include <boost/algorithm/string.hpp>

void send(std::string const &command) {
  asio::io_context io_context;
  asio::ip::tcp::socket socket(io_context);

  asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"), 5433);
  socket.connect(endpoint);

  asio::write(socket, asio::buffer(command));

  char buffer[1024];
  std::size_t bytes_received = socket.read_some(asio::buffer(buffer, sizeof(buffer)));

  std::cout << "\n" << std::string(buffer, bytes_received) << std::endl;
}

int main(int argc, char **argv) {
  for (std::string command; std::cout << "smalltable > " && std::getline(std::cin, command);) {
    boost::trim(command);
    if (command == "exit") break;
    if (!command.empty()) send(command);
  }

  return 0;
}
