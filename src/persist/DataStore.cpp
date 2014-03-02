#include "persist/DataStore.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

DataStore::DataStore(sqlite3 *db) : db(db) {}

DataStore DataStore::open(std::string db_path) {
  sqlite3 *db;
  if(sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
    std::cout << "Error opening SQLite database" << std::endl;
  }

  return DataStore(db);
}

int callback(void * param, int argc, char **argv, char **azColName) {
  std::cout << "Got data!" << std::endl;

  for(int i = 0; i < argc; i++) {
    std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
  }

  return 0;
}

void DataStore::createTables() {
  const char *q =
    "CREATE TABLE IF NOT EXISTS model ("
       "name CHAR(50) PRIMARY KEY,"
       "data BLOB"
     ");";
  sqlite3_stmt *stmt = query(q);
  sqlite3_step(stmt);

  const char *q2 =
     "CREATE TABLE IF NOT EXISTS result ("
       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
       "model_name CHAR(50),"
       "subject_name CHAR(50),"
       "notes TEXT,"
       "date TEXT,"
       "value REAL,"
       "FOREIGN KEY(model_name) REFERENCES model(name)"
     ");";

  sqlite3_stmt *stmt2 = query(q2);
  sqlite3_step(stmt2);
}

std::vector<ModelEntry> DataStore::findAllModelEntries() {
  std::vector<ModelEntry> entries;

  const char *q = "select name, data from model";
  sqlite3_stmt *stmt = query(q);

  int rc;
  while((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    switch(rc) {
      case SQLITE_ROW:
        std::string name(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 0)));
        int len = sqlite3_column_bytes(stmt, 1);
        void *data = std::malloc(len);
        std::memcpy(data, sqlite3_column_blob(stmt, 1), len);
        entries.push_back(ModelEntry(name, data, len));
        break;
    }
  }

  return entries;
}

std::vector<ResultEntry> DataStore::findAllResultEntries() {
  std::vector<ResultEntry> entries;

  const char *q = "select id, model_name, subject_name, notes, date, value from result";
  sqlite3_stmt *stmt = query(q);

  int rc;
  while((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    switch(rc) {
      case SQLITE_ROW:
        int id = sqlite3_column_int(stmt, 0);
        std::string modelName(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 1)));
        std::string subjectName(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 2)));
        std::string notes(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 3)));
        std::string date(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 4)));
        double value = sqlite3_column_double(stmt, 5);

        entries.push_back(ResultEntry(id, modelName, subjectName, notes, value));
        break;
    }
  }

  return entries;
}

std::vector<ResultEntry> DataStore::findResultsForModelName(std::string modelName) {
  std::vector<ResultEntry> entries;

  const char *q = ("select id, model_name, subject_name, notes, date, value from result where model_name = '" + modelName + "'").c_str();
  sqlite3_stmt *stmt = query(q);

  int rc;
  while((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
    switch(rc) {
      case SQLITE_ROW:
        int id = sqlite3_column_int(stmt, 0);
        std::string modelName(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 1)));
        std::string subjectName(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 2)));
        std::string notes(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 3)));
        std::string date(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 4)));
        double value = sqlite3_column_double(stmt, 5);

        entries.push_back(ResultEntry(id, modelName, subjectName, notes, value));
        break;
    }
  }

  return entries;
}

void DataStore::insertModelEntry(ModelEntry entry) {
  const char *q = "INSERT INTO model" "(name, data) " "VALUES (?, ?)";

  sqlite3_stmt *stmt = query(q);
  int rc = sqlite3_prepare_v2(db, q, -1, &stmt, 0);
  sqlite3_bind_text(stmt, 1, entry.name.c_str(), -1, NULL);
  sqlite3_bind_blob(stmt, 2, entry.data, entry.length, NULL);

  sqlite3_step(stmt);

  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
  }
}

void DataStore::insertResultEntry(ResultEntry entry) {
  const char *q = sqlite3_mprintf("INSERT INTO result "
      "(model_name, subject_name, notes, date, value) "
      "VALUES ('%q', '%q', '%q', 'today', '%f')",
      entry.modelName.c_str(), entry.subjectName.c_str(), entry.notes.c_str(), entry.value);
  query(q);
  sqlite3_stmt *stmt = query(q);
  sqlite3_step(stmt);

  // sqlite3_free(q);
}

sqlite3_stmt *DataStore::query(const char *q) {
  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, q, -1, &stmt, 0);
  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
    return NULL;
  }

  return stmt;
}

void DataStore::close() {
  sqlite3_close(db);
}
