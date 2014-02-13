#ifndef _RESULT_ENTRY_H_
#define _RESULT_ENTRY_H_

#include <string>

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

#endif
