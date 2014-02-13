#ifndef _DATA_STORE_H_
#define _DATA_STORE_H_

#include <string>
#include <vector>
#include <sqlite3.h>

#include "ModelEntry.h"
#include "ResultEntry.h"

class DataStore {
public:
  static DataStore open(std::string db_path);

  void createTables();
  void close();

  std::vector<ModelEntry> findAllModelEntries();
  std::vector<ResultEntry> findAllResultEntries();
  std::vector<ResultEntry> findResultsForModelName(std::string name);
private:
  DataStore(sqlite3 *db);

  sqlite3 *db;
};

#endif
