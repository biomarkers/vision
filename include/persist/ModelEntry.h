#ifndef _MODEL_ENTRY_H_
#define _MODEL_ENTRY_H_

#include <string>

class ModelEntry {
public:
  std::string name;
  const void *data;
  int length;

  ModelEntry(std::string name, const void *data, int length) : name(name), data(data), length(length) { }
};

#endif
