#include <iostream>
#include <fstream>
#include <string>
#include "database.h"

Database::Database() : DATABASE_FILENAME("database.txt") {
  std::cout << "Database created\n";
}

Database::~Database() {
  std::cout << "Database destroyed\n";
}

std::string Database::insert(const std::string &key, const std::string &value) {
  std::ofstream db = openDbForWriting();
  Record newRecord = {key, value};
  db << newRecord.toString();
  db.close();
  return "INSERT\n";
}

std::string Database::find(const std::string& key) {
  std::ifstream db = openDbForReading();
  Record record;
  bool found = false;

  while (db >> record.key >> record.value) {
    if (record.key.compare(key) == 0) {
      found = true;
      break;
    }
  }

  db.close();

  if (!found) throw std::runtime_error("Error: Record not found");

  return record.value;
}

std::string Database::findAll() {
  std::ifstream db = openDbForReading();
  Record record;
  int numRecords = 0;

  std::string output = "";
  while (db >> record.key >> record.value) {
    output.append(record.toString());
    numRecords += 1;
  }

  db.close();

  output.append("\n----\n");
  output.append(std::to_string(numRecords));
  output.append("\n");

  return output;
}

std::string Database::remove(const std::string& key) {
  std::ifstream db = openDbForReading();
  std::ofstream tempDb = openFileForWriting("temp.txt");
  bool removed = false;
  Record record;

  while (db >> record.key >> record.value) {
    if(record.key == key) {
      removed = true;
    } else {
      tempDb << record.toString();
    }
  }

  db.close();
  tempDb.close();

  if (!removed) {
    std::remove("temp.txt");
    throw std::runtime_error("Error: Could not remove record");
  }

  std::remove(DATABASE_FILENAME.c_str());
  std::rename("temp.txt", DATABASE_FILENAME.c_str());

  return "REMOVE";
}

std::string Database::update(const std::string& key, const std::string &value) {
  std::ifstream db = openDbForReading();
  std::ofstream tempDb = openFileForWriting("temp.txt");
  Record record;
  bool found = false;

  while (db >> record.key >> record.value) {
    if (record.key == key) {
      found = true;
      record.value = value;
    }
    tempDb << record.toString();
  }

  db.close();
  tempDb.close();

  if (!found) {
    std::remove("temp.txt");
    throw std::runtime_error("Error: Could not update record");
  }

  std::remove(DATABASE_FILENAME.c_str());
  std::rename("temp.txt", DATABASE_FILENAME.c_str());

  return "UPDATE\n";
}

std::ifstream Database::openFileForReading(const std::string& filename) {
  std::ifstream inFile(filename);
  if (!inFile) {
    std::cerr << "Error: cannot open file for reading: " << filename << "\n";
  }
  return inFile;
}

std::ofstream Database::openFileForWriting(const std::string& filename) {
  std::ofstream outFile(filename, std::ios::app);
  if (!outFile) {
    std::cerr << "Error: cannot open file for writing: " << filename << "\n";
  }
  return outFile;
}

std::ifstream Database::openDbForReading() {
  return openFileForReading(DATABASE_FILENAME);
}

std::ofstream Database::openDbForWriting() {
  return openFileForWriting(DATABASE_FILENAME);
}
