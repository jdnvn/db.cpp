#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <fstream>
#include <string>

class Database {
public:
  const std::string DATABASE_FILENAME;
  Database();
  ~Database();
  std::string insert(const std::string &key, const std::string &value);
  std::string find(const std::string &key);
  std::string findAll();
  std::string remove(const std::string &key);
  std::string update(const std::string &key, const std::string &value);
private:
  struct Record {
    std::string key;
    std::string value;

    std::string toString() const {
      return key + " " + value + "\n";
    }
  };
  std::ifstream openFileForReading(const std::string &filename);
  std::ofstream openFileForWriting(const std::string &filename);
  std::ifstream openDbForReading();
  std::ofstream openDbForWriting();
};

#endif
