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

void DataStore::createTables() {
  const char *q =
    "CREATE TABLE IF NOT EXISTS model ("
       "name CHAR(50) PRIMARY KEY,"
       "data BLOB"
     ");";
  sqlite3_stmt *stmt = query(q);
  sqlite3_step(stmt);

  sqlite3_finalize(stmt);

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

  sqlite3_finalize(stmt2);
}

void DataStore::close() {
  sqlite3_close(db);
}

std::vector<ModelEntry> DataStore::findAllModelEntries() {
  std::vector<ModelEntry> entries;

  const char *q = "select name, data from model order by name desc";
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

  sqlite3_finalize(stmt);

  return entries;
}

std::vector<ResultEntry> DataStore::findAllResultEntries() {
  std::vector<ResultEntry> entries;

  const char *q = "select id, model_name, subject_name, notes, date, value from result order by date desc";
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

        entries.push_back(ResultEntry(id, modelName, subjectName, notes, date, value));
        break;
    }
  }

  sqlite3_finalize(stmt);

  return entries;
}

std::vector<ResultEntry> DataStore::findResultsForModelName(std::string modelName) {
  std::vector<ResultEntry> entries;

  const char *q = sqlite3_mprintf("select id, model_name, subject_name, notes, date, value from result where model_name = '%q'", modelName.c_str());
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

        entries.push_back(ResultEntry(id, modelName, subjectName, notes, date, value));
        break;
    }
  }

  sqlite3_free((void *) q);
  sqlite3_finalize(stmt);

  return entries;
}

void DataStore::insertModelEntry(ModelEntry entry) {
  const char *q = "insert into model (name, data) values (?, ?)";

  sqlite3_stmt *stmt = query(q);
  int rc = sqlite3_prepare_v2(db, q, -1, &stmt, 0);
  sqlite3_bind_text(stmt, 1, entry.name.c_str(), -1, NULL);
  sqlite3_bind_blob(stmt, 2, entry.data, entry.length, NULL);

  sqlite3_step(stmt);

  if(rc != SQLITE_OK) {
    std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
  }

  sqlite3_finalize(stmt);
}

void DataStore::insertResultEntry(ResultEntry entry) {
  const char *q = sqlite3_mprintf("insert into result "
      "(model_name, subject_name, notes, date, value) "
      "values ('%q', '%q', '%q', '%q', %f)",
      entry.modelName.c_str(), entry.subjectName.c_str(), entry.notes.c_str(), entry.date.c_str(), entry.value);

  sqlite3_stmt *stmt = query(q);
  sqlite3_step(stmt);

  sqlite3_free((void *) q);
  sqlite3_finalize(stmt);
}

void DataStore::deleteModelEntry(std::string name) {
  const char *q = sqlite3_mprintf("delete from model where name = '%q'", name.c_str());

  sqlite3_stmt *stmt = query(q);
  sqlite3_step(stmt);

  sqlite3_free((void *) q);
  sqlite3_finalize(stmt);
}

void DataStore::deleteResultEntry(int id) {
  const char *q = sqlite3_mprintf("delete from result where id = %d", id);

  sqlite3_stmt *stmt = query(q);
  sqlite3_step(stmt);

  sqlite3_free((void *) q);
  sqlite3_finalize(stmt);
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

