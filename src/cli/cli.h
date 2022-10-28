#pragma once

#include "../tree/tree.h"
#include "../tree/person.h"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>

#ifndef PS1
  #define PS1 "genea>> "
#endif

namespace genea {

class CLI {

public:
  CLI(const std::string& file);
  void run();

private:

  static std::string banner;

  std::vector<std::string> splitLine(const std::string& line);
  
  Tree tree_;
  std::map<std::string, std::function<void(std::vector<std::string>)>> commands_;
  std::shared_ptr<struct Person> current_;

  /* commands */
  void help(std::vector<std::string> args);
  void addPerson(std::vector<std::string> args);
  /* commands */
};

} // namespace genea