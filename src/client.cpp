#include <iostream>
#include <asio.hpp>
#include <boost/algorithm/string.hpp>

void send(std::string const &command, asio::ip::tcp::socket &socket) {
  asio::write(socket, asio::buffer(command));

  char buffer[1024];
  std::size_t bytes_received = socket.read_some(asio::buffer(buffer, sizeof(buffer)));

  std::cout << "\n" << std::string(buffer, bytes_received) << std::endl;
}

int main(int argc, char **argv) {
  // open up a connection to the server
  asio::io_context io_context;
  asio::ip::tcp::socket socket(io_context);

  try {
    asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"), 5433);
    socket.connect(endpoint);

    // listen for user input
    for (std::string command; std::cout << "smalltable > " && std::getline(std::cin, command);) {
      boost::trim(command);
      if (command == "exit") break;
      if (!command.empty()) send(command, socket);
    }
  } catch (std::exception &e) {
    std::cerr << "error connecting to server: " << e.what() << std::endl;
    return 1;
  }

  socket.close();

  return 0;
}
