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


  typedef std::vector<std::string> commandArgs;
  /* commands */
  void help(commandArgs args);
  void create(commandArgs args);
  void add(commandArgs args);
  void attach(commandArgs args);
  void remove(commandArgs args);
  void overwrite(commandArgs args);
  void info(commandArgs args);
  void list(commandArgs args);
  void select(commandArgs args);
  void dump(commandArgs args);
  void generateImage(commandArgs args);
  /* commands */
};

} // namespace genea