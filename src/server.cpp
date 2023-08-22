#include <iostream>
#include <sstream>
#include <fstream>
#include <asio.hpp>
#include "database.h"
#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include <memory>
#include <utility>

using asio::ip::tcp;

// database is global
Database db;

std::vector<std::string> parseCommandArgs(const std::string& command) {
  std::istringstream iss(command);
  std::string str;
  std::vector<std::string> args;

  while (getline(iss, str, ' ')) {
    args.push_back(str);
  }

  return args;
}

std::string executeCommand(const std::vector<std::string>& args) {
  if (boost::iequals(args[0], "ALL")) {
    return db.all();
  }
  if (boost::iequals(args[0], "INSERT")) {
    if (args.size() != 3) throw std::runtime_error("error: INSERT requires 2 arguments");
    return db.insert(args[1], args[2]);
  }
  if (boost::iequals(args[0], "FIND")) {
    if (args.size() != 2) throw std::runtime_error("error: FIND requires 1 argument");
    return db.find(args[1]);
  }
  if (boost::iequals(args[0], "REMOVE")) {
    if (args.size() != 2) throw std::runtime_error("error: REMOVE requires 1 argument");
    return db.remove(args[1]);
  }
  if (boost::iequals(args[0], "UPDATE")) {
    if (args.size() != 3) throw std::runtime_error("error: UPDATE requires 2 arguments");
    return db.update(args[1], args[2]);
  }

  return "error: invalid command";
}

class Session : public std::enable_shared_from_this<Session> {
public:
  Session(tcp::socket socket, int id) : socket_(std::move(socket)), id_(id) {
    ip_ = socket_.remote_endpoint().address().to_string();
    std::cout << "Client " << idStr() << " connected from " << ip_ << "\n";
  }

  ~Session() {
    std::cout << "Client " << idStr() << " disconnected" << "\n";
  }

  void start() {
    read();
  }

private:
  tcp::socket socket_;
  std::string ip_;
  int id_;
  enum { max_length = 1024 };
  char data_[max_length];

  void read() {
    auto self(shared_from_this());

    socket_.async_read_some(asio::buffer(data_, max_length),
        [this, self](std::error_code ec, std::size_t length) {
          if (!ec) {
            try {
              if (length == 0) throw std::runtime_error("no bytes read");
              std::string command(data_, length);

              std::cout << idStr() << ": " << command << std::endl;

              std::vector<std::string> args = parseCommandArgs(command);
              std::string response = executeCommand(args);

              write(response.c_str());
            } catch (const std::exception &e) {
              // respond with error message
              write(e.what());
            }
          }
        });
  }

  void write(const char *response) {
    auto self(shared_from_this());

    std::size_t length = strlen(response);
    std::memcpy(data_, response, length);

    asio::async_write(socket_, asio::buffer(data_, length),
        [this, self](std::error_code ec, std::size_t /*length*/) {
          if (!ec) {
            read();
          }
        });
  }

  std::string idStr() {
    return std::to_string(id_);
  }
};


class Server {
  public:
    Server(asio::io_context& io_context, unsigned short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
      std::cout << "smalltable server listening on port " << port << std::endl;
      accept();
    }

  private:
    tcp::acceptor acceptor_;
    int nextId_ = 0;

    void accept() {
      acceptor_.async_accept(
          [this](std::error_code ec, tcp::socket socket) {
            if (!ec) {
              std::make_shared<Session>(std::move(socket), nextId_++)->start();
            }

            accept();
          });
    }
};

int main(int argc, char* argv[]) {
  try {
    asio::io_context io_context;
    unsigned short port = (argc >= 2) ? std::atoi(argv[1]) : 6969;

    Server server(io_context, port);

    io_context.run();
  } catch (std::exception& e) {
    std::cerr << "error in main: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
