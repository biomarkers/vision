#include "persist/DataStore.h"

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
  std::string sql =
    "CREATE TABLE IF NOT EXISTS model ("
       "name CHAR(50) PRIMARY KEY,"
       "data BLOB"
     ");"
     "CREATE TABLE IF NOT EXISTS result ("
       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
       "model_name CHAR(50),"
       "subject_name CHAR(50),"
       "notes TEXT,"
       "date TEXT,"
       "value REAL,"
       "FOREIGN KEY(model_name) REFERENCES model(name)"
     ");";

  char *errmsg;
  int r = sqlite3_exec(db, sql.c_str(), callback, 0, &errmsg);
  if(r != SQLITE_OK) {
    std::cout << "Error: " << errmsg << std::endl;
    sqlite3_free(errmsg);
  } else {
    std::cout << "Running..." << std::endl;
  }
}

std::vector<ModelEntry> DataStore::findAllModelEntries() {
  std::vector<ModelEntry> entries;

  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, "select name, data from model", -1, &stmt, 0);
  if(rc == SQLITE_OK) {
    while((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
      switch(rc) {
        case SQLITE_BUSY:
          std::cout << "Waiting..." << std::endl;
          break;
        case SQLITE_ERROR:
          std::cerr << "Error: " << sqlite3_errmsg(db) << std::endl;
          break;
        case SQLITE_ROW:
          std::string name(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 0)));
          std::string blob(reinterpret_cast<char const*>(sqlite3_column_text(stmt, 1)));

          entries.push_back(ModelEntry(name, blob));
          break;
      }
    }
  } else {
    std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
  }

  return entries;
}

std::vector<ResultEntry> DataStore::findAllResultEntries() {
  std::vector<ResultEntry> entries;

  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, "select id, model_name, subject_name, notes, date, value from result", -1, &stmt, 0);
  if(rc == SQLITE_OK) {
    while((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
      switch(rc) {
        case SQLITE_BUSY:
          std::cout << "Waiting..." << std::endl;
          break;
        case SQLITE_ERROR:
          std::cerr << "Error: " << sqlite3_errmsg(db) << std::endl;
          break;
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
  } else {
    std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
  }

  return entries;
}

std::vector<ResultEntry> DataStore::findResultsForModelName(std::string modelName) {
  std::vector<ResultEntry> entries;

  sqlite3_stmt *stmt;
  int rc = sqlite3_prepare_v2(db, ("select id, model_name, subject_name, notes, date, value from result where model_name = '" + modelName + "'").c_str(), -1, &stmt, 0);
  if(rc == SQLITE_OK) {
    while((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
      switch(rc) {
        case SQLITE_BUSY:
          std::cout << "Waiting..." << std::endl;
          break;
        case SQLITE_ERROR:
          std::cerr << "Error: " << sqlite3_errmsg(db) << std::endl;
          break;
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
  } else {
    std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
  }

  return entries;
}

void DataStore::insertResultEntry(ResultEntry entry) {
  sqlite3_stmt *stmt;

  char *query = sqlite3_mprintf("INSERT INTO result "
      "(model_name, subject_name, notes, date, value) "
      "VALUES ('%q', '%q', '%q', 'today', '%f')",
      entry.modelName.c_str(), entry.subjectName.c_str(), entry.notes.c_str(), entry.value);
  int rc = sqlite3_prepare_v2(db, query,-1, &stmt, 0);
  if(rc == SQLITE_OK) {
    while((rc = sqlite3_step(stmt)) != SQLITE_DONE) {
      switch(rc) {
        case SQLITE_BUSY:
          std::cout << "Waiting..." << std::endl;
          break;
        case SQLITE_ERROR:
          std::cerr << "Error: " << sqlite3_errmsg(db) << std::endl;
          break;
      }
    }
  } else {
    std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
  }

  sqlite3_free(query);
}

void DataStore::close() {
  sqlite3_close(db);
}
