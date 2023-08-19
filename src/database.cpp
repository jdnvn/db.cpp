#include <iostream>
#include <fstream>

const std::string DATABASE_FILENAME = "database.txt";

struct Record {
  int id;
  std::string value;

  std::string toString() const {
    return std::to_string(id) + " " + value + "\n";
  }
};

std::ifstream openFileForReading(const std::string& filename) {
  std::ifstream inFile(filename);
  if (!inFile) {
    std::cerr << "Error: cannot open file for reading: " << filename << "\n";
  }
  return inFile;
}

std::ofstream openFileForWriting(const std::string& filename) {
  std::ofstream outFile(filename, std::ios::app);
  if (!outFile) {
    std::cerr << "Error: cannot open file for writing: " << filename << "\n";
  }
  return outFile;
}

std::ifstream openDbForReading() {
  return openFileForReading(DATABASE_FILENAME);
}

std::ofstream openDbForWriting() {
  return openFileForWriting(DATABASE_FILENAME);
}

void insert(const Record& record) {
  std::ofstream db = openDbForWriting();
  db << record.toString();
  db.close();
  std::cout << "INSERT\n";
}

void find(const int& id) {
  std::ifstream db = openDbForReading();
  Record record;
  bool found = false;

  while (db >> record.id >> record.value) {
    if (record.id == id) {
      found = true;
      break;
    }
  }

  db.close();

  if (!found) throw std::runtime_error("Error: Record not found");

  std::cout << record.toString();
}

void findAll() {
  std::ifstream db = openDbForReading();
  Record record;
  int numRecords = 0;
  while (db >> record.id >> record.value) {
    std::cout << record.toString();
    numRecords += 1;
  }
  std::cout << "\n----\n" << numRecords << "\n";
  db.close();
}

void remove(const int& id) {
  std::ifstream db = openDbForReading();
  std::ofstream tempDb = openFileForWriting("temp.txt");
  bool removed = false;
  Record record;

  while (db >> record.id >> record.value) {
    if(record.id == id) {
      removed = true;
    } else {
      tempDb << record.toString();
    }
  }

  db.close();
  tempDb.close();

  if (removed) {
    std::remove(DATABASE_FILENAME.c_str());
    std::rename("temp.txt", DATABASE_FILENAME.c_str());
    std::cout << "REMOVE";
  } else {
    std::remove("temp.txt");
    std::cerr << "Error: Could not remove record with ID " << id << "\n";
  }
}

void update(const int& id, const std::string& value) {
  std::ifstream db = openDbForReading();
  std::ofstream tempDb = openFileForWriting("temp.txt");
  Record record;
  bool found = false;

  while (db >> record.id >> record.value) {
    if (record.id == id) {
      found = true;
      record.value = value;
    }
    tempDb << record.toString();
  }

  db.close();
  tempDb.close();

  if (found) {
    std::remove(DATABASE_FILENAME.c_str());
    std::rename("temp.txt", DATABASE_FILENAME.c_str());
    std::cout << "UPDATE\n";
  } else {
    std::remove("temp.txt");
    std::cerr << "Error: Could not remove record with ID " << id << "\n";
  }
}

int main() {
  findAll();

  return 0;
}
