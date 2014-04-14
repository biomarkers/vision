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
  ModelEntry findModelEntryById(int id);
  std::vector<ResultEntry> findAllResultEntries();
  std::vector<ResultEntry> findResultsForModelName(std::string name);
  ResultEntry findResultForIdWithExportdData(int id);

  void insertModelEntry(ModelEntry entry);
  int insertResultEntry(ResultEntry entry);

  void deleteModelEntry(std::string name);
  void deleteResultEntry(int id);
private:
  DataStore(sqlite3 *db);

  sqlite3_stmt *query(const char *q);

  sqlite3 *db;
};

#endif
