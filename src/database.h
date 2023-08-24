#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>

class Database {
public:
  Database();
  ~Database();
  std::string insert(const std::string& key, const std::string& value);
  std::string find(const std::string& key);
  std::string all();
  std::string remove(const std::string& key);
  std::string update(const std::string& key, const std::string& value);
private:
  const std::string DATABASE_FILENAME_;
  struct Record {
    std::string key;
    std::string value;

    std::string toString() const {
      return key + " " + value + "\n";
    }
  };
  std::unordered_map<std::string, std::string> records_;
  void populateMapFromFile();
  void writeRecordToFile(const Record& record);
  void removeRecordFromFile(const std::string& key);
  void updateRecordInFile(const std::string& key, const std::string& value);
  std::ifstream openFileForReading(const std::string& filename);
  std::ofstream openFileForWriting(const std::string& filename);
  std::ifstream openDbForReading();
  std::ofstream openDbForWriting();
};

#endif
