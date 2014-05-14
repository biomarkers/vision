#ifndef _RESULT_ENTRY_H_
#define _RESULT_ENTRY_H_

#include <string>

class ResultEntry {
public:
  int id;
  std::string modelName;
  std::string subjectName;
  std::string notes;
  std::string date;
  float value;
  std::string exportedData;
  std::string exportedMessage;
  std::string statistics;

  ResultEntry(int id, std::string modelName, std::string subjectName, std::string notes, std::string date, float value, std::string exportedData, std::string exportedMessage, std::string statistics) : id(id), modelName(modelName), subjectName(subjectName), notes(notes), date(date), value(value), exportedData(exportedData), exportedMessage(exportedMessage), statistics(statistics) { }
};

#endif
