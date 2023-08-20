#include <iostream>
#include <sstream>
#include <fstream>
#include <asio.hpp>
#include "database.h"

int main() {
  int port = 5433;
  asio::io_context io_context;
  asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));
  std::cout << "smalltable server listening on port " << port << "\n";

  Database db;

  while (true) {
    try {
      asio::ip::tcp::socket socket(io_context);
      acceptor.accept(socket);
      std::string ip = socket.remote_endpoint().address().to_string();
      std::cout << ip << " connected." << std::endl;

      while (true) {
        char data[1024];

        try {
          std::size_t bytes_read = socket.read_some(asio::buffer(data, sizeof(data)));

          if (bytes_read == 0) throw std::runtime_error("no bytes read");

          std::cout << ip << ": " << data << std::endl;

          // parse string into arguments
          std::istringstream iss(data);
          std::string str;
          std::vector<std::string> args;
          while (getline(iss, str, ' ')) {
            args.push_back(str);
          }
          iss.clear();

          // match and execute command
          std::string response;
          if (args[0] == "FINDALL") {
            response = db.findAll();
          } else if (args[0] == "INSERT") {
            if (args.size() != 3) throw std::runtime_error("error: INSERT requires 2 arguments");
            response = db.insert(args[1], args[2]);
          } else if (args[0] == "FIND") {
            if (args.size() != 2) throw std::runtime_error("error: FIND requires 1 argument");
            response = db.find(args[1]);
          } else if (args[0] == "REMOVE") {
            if (args.size() != 2) throw std::runtime_error("error: REMOVE requires 1 argument");
            response = db.remove(args[1]);
          } else if (args[0] == "UPDATE") {
            if (args.size() != 3) throw std::runtime_error("error: UPDATE requires 2 arguments");
            response = db.update(args[1], args[2]);
          } else {
            response = "error: invalid command";
          }

          asio::write(socket, asio::buffer(response));
        } catch (const std::exception &e) {
          asio::write(socket, asio::buffer(std::string(e.what())));
        }

        // clear buffer
        memset(data, 0, 1024);
      }
    } catch (const std::exception &e) {
      std::cerr << "client disconnected - " << e.what() << std::endl;
    }
  }

  return 0;
}
