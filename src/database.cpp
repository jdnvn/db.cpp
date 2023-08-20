#include <iostream>
#include <fstream>
#include <string>
#include "database.h"
#include <future>

std::unordered_map<std::string, std::string> records;

Database::Database() : DATABASE_FILENAME("../database.txt") {
  std::ifstream db = openDbForReading();
  Record record;

  while (db >> record.key >> record.value) {
    records[record.key] = record.value;
  }

  db.close();
}

Database::~Database() {}

void Database::writeRecordToFile(const Record &record) {
  std::ofstream db = openDbForWriting();
  db << record.toString();
  db.close();
}

std::string Database::insert(const std::string &key, const std::string &value) {
  Record newRecord = {key, value};

  std::async([this, newRecord]() { writeRecordToFile(newRecord); });

  records[key] = value;

  return "INSERT";
}

std::string Database::find(const std::string& key) {
  Record record;
  record.key = key;

  try {
    record.value = records.at(key);
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

    if (!found) throw std::runtime_error("error: record not found");
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

  return output;
}

void Database::removeRecordFromFile(const std::string& key) {
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
    throw std::runtime_error("error: could not remove record");
  }

  std::remove(DATABASE_FILENAME.c_str());
  std::rename("temp.txt", DATABASE_FILENAME.c_str());
}

std::string Database::remove(const std::string& key) {
  try {
    find(key);
  } catch (const std::exception &e) {
    return "error: record not found";
  }

  records.erase(key);

  std::async([this, key]() { removeRecordFromFile(key); });

  std::cout << "after async" << std::endl;

  return "REMOVE";
}

void Database::updateRecordInFile(const std::string& key, const std::string& value) {
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
    throw std::runtime_error("error: could not update record");
  }

  std::remove(DATABASE_FILENAME.c_str());
  std::rename("temp.txt", DATABASE_FILENAME.c_str());
}

std::string Database::update(const std::string& key, const std::string& value) {
  try {
    find(key);
  } catch (const std::exception &e) {
    return "error: record not found";
  }

  std::async([this, key, value]() { updateRecordInFile(key, value); });
  records[key] = value;

  return "UPDATE";
}

std::ifstream Database::openFileForReading(const std::string& filename) {
  std::ifstream inFile(filename);
  if (!inFile) {
    std::ofstream createFile(filename);
    createFile.close();
    std::ifstream inFile(filename);
  }
  return inFile;
}

std::ofstream Database::openFileForWriting(const std::string& filename) {
  std::ofstream outFile(filename, std::ios::app);
  if (!outFile) {
    std::cerr << "error: cannot open file for writing: " << filename << "\n";
  }
  return outFile;
}

std::ifstream Database::openDbForReading() {
  return openFileForReading(DATABASE_FILENAME);
}

std::ofstream Database::openDbForWriting() {
  return openFileForWriting(DATABASE_FILENAME);
}
