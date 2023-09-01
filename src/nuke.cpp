#include <iostream>
#include <asio.hpp>
#include <boost/algorithm/string.hpp>

int main(int argc, char **argv) {
  if (argc < 2) exit(1);

  const unsigned short numBots = std::atoi(argv[1]);
  asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string("127.0.0.1"), 6969);
  asio::io_context io_context;

  std::vector<asio::ip::tcp::socket> sockets;

  for (int i = 0; i<numBots; ++i) {
    sockets.emplace_back(io_context);
    sockets[i].connect(endpoint);
  }

  char buffer[1024];

  for (int i = 0; i < numBots; ++i) {
    std::string command = "insert " + std::to_string(i) + " " + std::to_string(i + 1);
    asio::write(sockets[i], asio::buffer(command));
    int readbytes = sockets[i].read_some(asio::buffer(buffer, sizeof(buffer)));
    std::cout.write(buffer, readbytes);
    std::cout << std::endl;
  }

  for (int i = 0; i < numBots; ++i) {
    std::string command = "find " + std::to_string(i);
    asio::write(sockets[i], asio::buffer(command));
    int readbytes = sockets[i].read_some(asio::buffer(buffer, sizeof(buffer)));
    std::cout.write(buffer, readbytes);
    std::cout << std::endl;
  }

  for (int i = 0; i < numBots; ++i) {
    std::string command = "remove " + std::to_string(i);
    asio::write(sockets[i], asio::buffer(command));
    int readbytes = sockets[i].read_some(asio::buffer(buffer, sizeof(buffer)));
    std::cout.write(buffer, readbytes);
    std::cout << std::endl;
  }

  for (int i = 0; i < numBots; ++i) {
    sockets[i].close();
  }

  return 0;
}
