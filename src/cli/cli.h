#pragma once

#include "person.h"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>
#include <cstdio>

#ifndef PS1
  #define PS1 "genea>> "
#endif



namespace genea {

/* utilities */
std::vector<std::shared_ptr<struct Person>> computeRelation(std::vector<std::string> relations, std::shared_ptr<struct Person> start);
bool setRelation(const std::string& relation, std::shared_ptr<struct Person> of, std::shared_ptr<struct Person> p);
std::shared_ptr<struct Person> parsePerson(std::vector<std::string> args);
bool parseDate(const std::string& s, struct Date* d);
std::vector<std::string> parseLine(const std::string& line, char sep);
/* utilities */


class CLI {

public:
  CLI(const std::string& file);
  void run();

private:

  static std::string banner;
  
  std::vector<std::shared_ptr<struct Person>> people_;
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
