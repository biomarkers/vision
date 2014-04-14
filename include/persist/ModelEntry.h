#ifndef _MODEL_ENTRY_H_
#define _MODEL_ENTRY_H_

#include <string>

class ModelEntry {
public:
  std::string name;
  const void *data;
  int length;
  std::string units;

  ModelEntry(std::string name, const void *data, int length, std::string units) : name(name), data(data), length(length), units(units) { }
};

#endif
