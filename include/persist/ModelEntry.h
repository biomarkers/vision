#ifndef _MODEL_ENTRY_H_
#define _MODEL_ENTRY_H_

class ModelEntry {
public:
  std::string name;
  std::string data;

  ModelEntry(std::string name, std::string data) : name(name), data(data) { }
};

#endif
