#include "DataPersistence.h"

DataStore::DataStore(sqlite3 *db) : db(db) {}

DataStore DataStore::open() {
  sqlite3 *db;
  sqlite3_open(DB_NAME, &db);

  return DataStore(db);
}

void DataStore::close() {
  sqlite3_close(db);
}
