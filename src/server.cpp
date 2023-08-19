#include <iostream>
#include <sstream>
#include <fstream>
#include <asio.hpp>
#include "database.h"

int main() {
  int port = 5433;
  asio::io_context io_context;
  asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
  std::cout << "Server listening on port " << port << "\n";

  Database db;

  while (true) {
    asio::ip::tcp::socket socket(io_context);
    acceptor.accept(socket);

    try {
      char data[1024];
      std::size_t bytes_read = socket.read_some(asio::buffer(data, sizeof(data)));

      if (bytes_read == 0) throw std::runtime_error("No bytes read");

      std::istringstream iss(data);
      std::string command;


      iss >> command;

      std::string response;
      if (command == "FINDALL") {
        response = db.findAll();
      } else if (command == "INSERT") {
        std::string key;
        std::string value;
        iss >> key >> value;
        response = db.insert(key, value);
      } else if (command == "FIND") {
        std::string key;
        iss >> key;
        response = db.find(key);
      } else if (command == "REMOVE") {
        std::string key;
        iss >> key;
        response = db.remove(key);
      } else if (command == "UPDATE") {
        std::string key;
        std::string value;
        iss >> key >> value;
        response = db.update(key, value);
      } else {
        response = "Invalid command";
      }

      asio::write(socket, asio::buffer(response));

      socket.close();
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << std::endl;
      socket.close();
    }
  }

  return 0;
}
