#include "cli.h"

#include <iostream>
#include <fstream>
#include <unistd.h>

namespace genea {


std::string CLI::banner =
"      ....        .                                                   \n"
"   .x88\" `^x~  xH(`                                                   \n"
"  X888   x8 ` 8888h                 u.    u.                          \n"
" 88888  888.  %8888        .u     x@88k u@88c.      .u          u     \n"
"<8888X X8888   X8?      ud8888.  ^\"8888\"\"8888\"   ud8888.     us888u.  \n"
"X8888> 488888>\"8888x  :888'8888.   8888  888R  :888'8888. .@88 \"8888\" \n"
"X8888>  888888 '8888L d888 '88%\"   8888  888R  d888 '88%\" 9888  9888  \n"
"?8888X   ?8888>'8888X 8888.+\"      8888  888R  8888.+\"    9888  9888  \n"
" 8888X h  8888 '8888~ 8888L        8888  888R  8888L      9888  9888  \n"
"  ?888  -:8*\"  <888\"  '8888c. .+  \"*88*\" 8888\" '8888c. .+ 9888  9888  \n"
"   `*88.      :88%     \"88888%      \"\"   'Y\"    \"88888%   \"888*\"\"888\" \n"
"      ^\"~====\"\"`         \"YP'                     \"YP'     ^Y\"   ^Y'  \n";


CLI::CLI(const std::string& file):
current_(nullptr),
commands_({
  { "help", std::bind(&CLI::help, this, std::placeholders::_1) },
  { "create", std::bind(&CLI::create, this, std::placeholders::_1)},
  { "add", std::bind(&CLI::add, this, std::placeholders::_1)},
  { "attach", std::bind(&CLI::attach, this, std::placeholders::_1)},
  { "remove", std::bind(&CLI::remove, this, std::placeholders::_1)},
  { "overwrite", std::bind(&CLI::overwrite, this, std::placeholders::_1)},
  { "info", std::bind(&CLI::info, this, std::placeholders::_1)},
  { "list", std::bind(&CLI::list, this, std::placeholders::_1)},
  { "select", std::bind(&CLI::select, this, std::placeholders::_1)},
  { "dump", std::bind(&CLI::dump, this, std::placeholders::_1)},
  { "generate-image", std::bind(&CLI::generateImage, this, std::placeholders::_1)}
}) {
  if (isatty(STDIN_FILENO))
    std::cerr << banner << std::endl;
  if (file == "") {
    tree_ = Tree();
    std::cout << "Created empty tree" << std::endl;
    return;
  }
  std::ifstream f(file);
  if (!f.good()) {
    f.close();
    std::cerr << "Warning: file " << file << " does not exists" << std::endl;
    tree_ = Tree();
    std::cout << "Created empty tree" << std::endl;
    return;
  }
  tree_ = Tree(f);
  std::cout << "Tree loaded from " << file << std::endl;
}

void CLI::run() {
  if (isatty(STDIN_FILENO))
    std::cerr << PS1;

  std::string line;
  std::getline(std::cin, line);

  bool exit = false;

  while (!std::cin.eof() && !exit) {
    std::vector<std::string> command = splitLine(line);
    if (command.size()) {
      std::string arg0 = command[0];
      if (!commands_.contains(arg0)) {
        std::cerr << "Unknown command: " << arg0 << std::endl;
        std::cerr << "Type 'help' to obtain help a list of available commands" << std::endl;
      } else {
        commands_[arg0](std::vector<std::string>(command.begin() + 1, command.end()));
      }
    }
    if (isatty(STDIN_FILENO))
      std::cerr << PS1;
    std::getline(std::cin, line);
  }
}

std::vector<std::string> CLI::splitLine(const std::string& line) {
  std::vector<std::string> command = std::vector<std::string>();
  unsigned start = 0;
  unsigned end = 0;
  while (line[end] == ' ')
    end++;
  while (end < line.size()) {
    start = end;
    while (end < line.size() && line[end] != ' ')
      end++;
    command.push_back(line.substr(start, end - start));
    while (end < line.size() && line[end] == ' ')
      end++;
  }
  return command;
}

/* commands */

void CLI::help(commandArgs args) {
  std::cerr << std::endl << "At all times (except when no person exists), a cursor is on a person on the genealogic tree" << std::endl;
  
  // General commands
  std::cerr << std::endl << "General commands:" << std::endl;
  std::cerr << "\t help\t\t\t\t\t Displays this message" << std::endl;

  // Creation/Deletion commands
  std::cerr << std::endl << "Creation/Deletion commands:" << std::endl;
  std::cerr << "\t create <first name> <last name> <sex> <born in> [<dead in>]" << std::endl;
  std::cerr << "\t\t\t\t\t\t Creates a new person which is linked to nobody. It will be reachable from IDs" << std::endl;
  std::cerr << "\t add <relation> <first name> <last name> <sex> <born in> [<dead in>]" << std::endl;
  std::cerr << "\t\t\t\t\t\t Creates a new person which is <relation> of the current person" << std::endl;
  std::cerr << "\t attach <relation> <id>\t\t\t Sets the person whose ID is <id> to be <relation> of the current person" << std::endl;
  std::cerr << "\t attach <relation> <id1> <id2>\t\t Sets the person whose ID is <id1> to be <relation> of the person whose ID is <id2>" << std::endl;
  std::cerr << "\t remove <relation> \t\t\t Removes the person who is <relation> of the current person" << std::endl;
  std::cerr << "\t overwrite <first name> <last name> <sex> <born in> [<dead in>]" << std::endl;
  std::cerr << "\t\t\t\t\t\t Overwrite the current person with given information" << std::endl;

  // Info commands
  std::cerr << std::endl << "Information commands:" << std::endl;
  std::cerr << "\t info\t\t\t\t\t Displays information about the current person" << std::endl;
  std::cerr << "\t info <relation>\t\t\t Displays information about the <relation> of the current person" << std::endl;
  std::cerr << "\t list\t\t\t\t\t Displays a list of all people of the tree with their given ID" << std::endl;

  // Move commands
  std::cerr << std::endl << "Move commands:" << std::endl;
  std::cerr << "\t select <relation>\t\t\t Moves the cursor to the <relation> of the current person" << std::endl;
  std::cerr << "\t select <id>\t\t\t\t Moves the cursor to the person whose ID is <id>" << std::endl;

  // Dump commands
  std::cerr << std::endl << "Dump commands:" << std::endl;
  std::cerr << "\t dump <file>\t\t\t\t Dumps the current tree to <file>" << std::endl;
  std::cerr << "\t generate-image <file>\t\t\t Generates a graph view of the genealogical tree to <file>" << std::endl;

  // Relations
  std::cerr << std::endl << "Available relations are:" << std::endl;
  std::cerr << "\t father, mother, child[<first name>], sibling[<first name>], children (info only), siblings (info only)" << std::endl;
  std::cerr << std::endl << "Relations can be chained separated by a point ('.')" << std::endl;
  std::cerr << "\t Ex: select father.mother.sibling[Alice].child[Bob].father" << std::endl;
  std::cerr << "\t Ex: info child[Charlie].mother.siblings" << std::endl << std::endl;
}

  void CLI::create(commandArgs args) {
    return;
  }

  void CLI::add(commandArgs args) {
    return;
  }

  void CLI::attach(commandArgs args) {
    return;
  }

  void CLI::remove(commandArgs args) {
    return;
  }

  void CLI::overwrite(commandArgs args) {
    return;
  }

  void CLI::info(commandArgs args) {
    return;
  }

  void CLI::list(commandArgs args) {
    return;
  }

  void CLI::select(commandArgs args) {
    return;
  }

  void CLI::dump(commandArgs args) {
    return;
  }

  void CLI::generateImage(commandArgs args) {
    return;
  }

/* commands */


} // namespace genea