#include <iostream>
#include <fstream>
#include <string>
#include "database.h"

std::unordered_map<std::string, std::string> records;

Database::Database() : DATABASE_FILENAME("database.txt") {
  std::cout << "Database created\n";

  // store records in memory
  std::ifstream db = openDbForReading();
  Record record;
  while (db >> record.key >> record.value) {
    records[record.key] = record.value;
  }
  db.close();
}

Database::~Database() {
  std::cout << "Database destroyed\n";
}

std::string Database::insert(const std::string &key, const std::string &value) {
  std::ofstream db = openDbForWriting();
  Record newRecord = {key, value};
  db << newRecord.toString();
  db.close();

  records[key] = value;

  return "INSERT\n";
}

std::string Database::find(const std::string& key) {
  Record record;
  record.key = key;

  try {
    record.value = records.at(key);
    std::cout << "Found record in memory\n";
  } catch (const std::out_of_range &e) {
    std::ifstream db = openDbForReading();
    bool found = false;

    while (db >> record.key >> record.value) {
      if (record.key.compare(key) == 0) {
        found = true;
        break;
      }
    }

    db.close();

    if (!found) throw std::runtime_error("Error: Record not found");
    records[key] = record.value;
  }

  return record.value;
}

std::string Database::findAll() {
  Record record;

  std::string output = "";
  for (const auto &pair : records) {
    record.key = pair.first;
    record.value = pair.second;
    output.append(record.toString());
  }

  output.append("----\n");
  output.append(std::to_string(records.size()));
  output.append("\n");

  return output;
}

std::string Database::remove(const std::string& key) {
  std::ifstream db = openDbForReading();
  std::ofstream tempDb = openFileForWriting("temp.txt");
  bool removed = false;
  Record record;

  records.erase(key);

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

  records[key] = value;

  // can this be done in the background?
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
