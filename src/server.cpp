#include <iostream>
#include <sstream>
#include <fstream>
#include <asio.hpp>
#include "database.h"
#include <boost/algorithm/string.hpp>

class Server {
  public:
    Server(asio::io_context& io_context, unsigned short port)
      : io_context(io_context),
        acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
      std::cout << "smalltable server listening on port " << port << std::endl;
    }

    void start() {
      while (true) {
        try {
          asio::ip::tcp::socket socket(io_context);
          acceptor.accept(socket);
          std::string ip = socket.remote_endpoint().address().to_string();

          std::cout << ip << " connected" << std::endl;

          while (true) {
            char data[1024];

            try {
              std::size_t bytes_read = socket.read_some(asio::buffer(data, sizeof(data)));

              if (bytes_read == 0) throw std::runtime_error("no bytes read");

              std::cout << ip << ": " << data << std::endl;

              std::vector<std::string> args = parseArgs(data);
              std::string response = executeCommand(args);

              asio::write(socket, asio::buffer(response));
            } catch (const std::exception &e) {
              // respond with error message
              asio::write(socket, asio::buffer(std::string(e.what())));
            }

            // clear buffer
            memset(data, 0, 1024);
          }
        } catch (const std::exception &e) {
          std::cerr << "client disconnected - " << e.what() << std::endl;
        }
      }
    }

  private:
    asio::io_context &io_context;
    asio::ip::tcp::acceptor acceptor;
    Database db;

    std::vector<std::string> parseArgs(const std::string& command) {
      std::istringstream iss(command);
      std::string str;
      std::vector<std::string> args;

      while (getline(iss, str, ' ')) {
        args.push_back(str);
      }

      return args;
    }

    std::string executeCommand(const std::vector<std::string>& args) {
      std::string response;

      if (boost::iequals(args[0], "ALL")) {
        response = db.all();
      } else if (boost::iequals(args[0], "INSERT")) {
        if (args.size() != 3) throw std::runtime_error("error: INSERT requires 2 arguments");
        response = db.insert(args[1], args[2]);
      } else if (boost::iequals(args[0], "FIND")) {
        if (args.size() != 2) throw std::runtime_error("error: FIND requires 1 argument");
        response = db.find(args[1]);
      } else if (boost::iequals(args[0], "REMOVE")) {
        if (args.size() != 2) throw std::runtime_error("error: REMOVE requires 1 argument");
        response = db.remove(args[1]);
      } else if (boost::iequals(args[0], "UPDATE")) {
        if (args.size() != 3) throw std::runtime_error("error: UPDATE requires 2 arguments");
        response = db.update(args[1], args[2]);
      } else {
        response = "error: invalid command";
      }

      return response;
    }
};

int main() {
  asio::io_context io_context;
  Server server(io_context, 5433);
  server.start();

  return 0;
}
