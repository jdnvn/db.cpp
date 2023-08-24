#include <iostream>
#include <fstream>
#include <string>
#include "database.h"
#include <future>

std::unordered_map<std::string, std::string> records_;

Database::Database() : DATABASE_FILENAME_("../database.txt") {
  populateMapFromFile();
}

Database::~Database() {}

std::string Database::all() {
  Record record;
  std::string output = "";

  for (const auto& pair : records_) {
    record.key = pair.first;
    record.value = pair.second;
    output.append(record.toString());
  }

  output.append("----\n");
  output.append(std::to_string(records_.size()));

  return output;
}

std::string Database::find(const std::string& key) {
  Record record;

  try {
    record.value = records_.at(key);
  } catch (const std::out_of_range& e) {
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
    records_[key] = record.value;
  }

  return record.value;
}

std::string Database::insert(const std::string& key, const std::string& value) {
  try {
    find(key);
    return "error: record already exists";
  } catch (const std::exception& e) {
    // key is free, intentionally swallow the error
  }

  Record newRecord = {key, value};

  std::async([this, newRecord]() { writeRecordToFile(newRecord); });

  records_[key] = value;

  return "INSERT";
}

std::string Database::remove(const std::string& key) {
  try {
    find(key);
  } catch (const std::exception& e) {
    return "error: record not found";
  }

  records_.erase(key);

  std::async([this, key]() { removeRecordFromFile(key); });

  return "REMOVE";
}

std::string Database::update(const std::string& key, const std::string& value) {
  try {
    find(key);
  } catch (const std::exception& e) {
    return "error: record not found";
  }

  std::async([this, key, value]() { updateRecordInFile(key, value); });
  records_[key] = value;

  return "UPDATE";
}

void Database::populateMapFromFile() {
  std::ifstream db = openDbForReading();
  Record record;

  while (db >> record.key >> record.value) {
    records_[record.key] = record.value;
  }

  db.close();
}

void Database::writeRecordToFile(const Record& record) {
  std::ofstream db = openDbForWriting();
  db << record.toString();
  db.close();
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

  std::remove(DATABASE_FILENAME_.c_str());
  std::rename("temp.txt", DATABASE_FILENAME_.c_str());
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

  std::remove(DATABASE_FILENAME_.c_str());
  std::rename("temp.txt", DATABASE_FILENAME_.c_str());
}

std::ifstream Database::openFileForReading(const std::string& filename) {
  std::ifstream inFile(filename);

  // if it doesn't exist, create one
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
  return openFileForReading(DATABASE_FILENAME_);
}

std::ofstream Database::openDbForWriting() {
  return openFileForWriting(DATABASE_FILENAME_);
}
