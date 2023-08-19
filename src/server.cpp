#include <iostream>
#include <fstream>
#include <asio.hpp>
#include "database.h"

int main() {
  asio::io_context io_context;
  asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 5433));

  while (true) {
    asio::ip::tcp::socket socket(io_context);
    acceptor.accept(socket);

    try {
      // Read data from the client
      char data[1024];
      std::size_t bytes_read = socket.read_some(asio::buffer(data, sizeof(data)));

      // Process the data, e.g., send a response
      std::string response = "Server received: ";
      response.append(data, bytes_read);
      asio::write(socket, asio::buffer(response));

      // Close the connection
      socket.close();
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }

  return 0;
}
