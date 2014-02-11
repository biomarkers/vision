#include <sqlite3.h>

#define DB_NAME "jkk_store.sqlite3"

class DataStore {
public:
  static DataStore open();

  void close();
private:
  DataStore(sqlite3 *db);

  sqlite3 *db;
};
