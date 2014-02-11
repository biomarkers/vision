#include <string>
#include <vector>
#include <sqlite3.h>

#define DB_NAME "jkk_store.sqlite3"

class ModelEntry {
public:
  std::string name;
  std::string data;

  ModelEntry(std::string name, std::string data) : name(name), data(data) { }
};

class ResultEntry {
public:
  int id;
  std::string modelName;
  std::string subjectName;
  std::string notes;
  // TODO: date
  float value;

  ResultEntry(int id, std::string modelName, std::string subjectName, std::string notes, float value) : id(id), modelName(modelName), subjectName(subjectName), notes(notes), value(value) { }
};

class DataStore {
public:
  static DataStore open();

  void createTables();
  void close();

  std::vector<ModelEntry> findAllModelEntries();
private:
  DataStore(sqlite3 *db);

  sqlite3 *db;
};
