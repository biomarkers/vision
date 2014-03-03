#ifndef _DATA_STORE_H_
#define _DATA_STORE_H_

#include <string>
#include <vector>
#include <sqlite3.h>

#include "ModelEntry.h"
#include "ResultEntry.h"

class DataStore {
public:
  /* Open a database file. If it does not exist, it will be created. */
  static DataStore open(std::string db_path);

  /* Create the database schema. If the schema already exists, nothing will be
   * changed. */
  void createTables();

  /* Close the database file. */
  void close();

  std::vector<ModelEntry> findAllModelEntries();
  std::vector<ResultEntry> findAllResultEntries();
  std::vector<ResultEntry> findResultsForModelName(std::string name);

  void insertModelEntry(ModelEntry entry);
  void insertResultEntry(ResultEntry entry);
private:
  DataStore(sqlite3 *db);

  sqlite3_stmt *query(const char *q);

  sqlite3 *db;
};

#endif
