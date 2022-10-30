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
people_(std::vector<std::shared_ptr<struct Person>>()),
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
    std::cout << "Created empty tree" << std::endl;
    return;
  }
  std::ifstream f(file);
  if (!f.good()) {
    f.close();
    std::cerr << "Warning: file " << file << " does not exists" << std::endl;
    std::cout << "Created empty tree" << std::endl;
    return;
  }
  // parse file
  f.close();
  std::cout << "Tree loaded from " << file << std::endl;
}

void CLI::run() {
  if (isatty(STDIN_FILENO))
    std::cerr << PS1;

  std::string line;
  std::getline(std::cin, line);

  bool exit = false;

  while (!std::cin.eof() && !exit) {
    std::vector<std::string> command = parseLine(line, ' ');
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

/* commands */
void CLI::help(commandArgs args) {
  std::cerr << std::endl << "At all times (except when no person exists), the cursor is on a person on the genealogic tree" << std::endl;
  
  // General commands
  std::cerr << std::endl << "General commands:" << std::endl;
  std::cerr << "\t help\t\t\t\t\t Displays this message" << std::endl;

  // Creation/Deletion commands
  std::cerr << std::endl << "Creation/Deletion commands:" << std::endl;
  std::cerr << "\t create <first name> <last name> <sex> <birth> [<death>]" << std::endl;
  std::cerr << "\t\t\t\t\t\t Creates a new person which is linked to nobody. It will be reachable from IDs" << std::endl;
  std::cerr << "\t add <relation> <first name> <last name> <sex> <birth> [<death>]" << std::endl;
  std::cerr << "\t\t\t\t\t\t Creates a new person which is <relation> of the current person" << std::endl;
  std::cerr << "\t attach <relation> <id>\t\t\t Sets the person whose ID is <id> to be <relation> of the current person" << std::endl;
  std::cerr << "\t attach <relation> <id1> <id2>\t\t Sets the person whose ID is <id1> to be <relation> of the person whose ID is <id2>" << std::endl;
  std::cerr << "\t remove <relation> \t\t\t Removes the person who is <relation> of the current person" << std::endl;
  std::cerr << "\t remove <id> \t\t\t\t Removes the person whose ID is <id>" << std::endl;
  std::cerr << "\t overwrite <first name> <last name> <sex> <birth> [<death>]" << std::endl;
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
  std::cerr << "\t father, mother, child:<first name>, sibling:<first name>, child (grouping), sibling (grouping)" << std::endl;
  std::cerr << std::endl << "Relations can be chained separated by a point ('.')" << std::endl;
  std::cerr << "\t Ex: select father.mother.sibling[Alice].child[Bob].father" << std::endl;
  std::cerr << "\t Ex: info child[Charlie].mother.sibling" << std::endl << std::endl;
}

void CLI::create(commandArgs args) {
  if (args.size() != 4 && args.size() != 5) {
    std::cerr << "Usage:" << std::endl << "\t create <first name> <last name> <sex> <birth> [<death>]" << std::endl;
    return;
  }
  std::shared_ptr<struct Person> created = parsePerson(args);
  if (!created) {
    std::cerr << "create: Could not create person" << std::endl;
    return;
  }
  people_.push_back(created);
  std::cout << "Created person ID " << people_.size() - 1 << std::endl;
  if (!current_) {
    current_ = created;
    std::cout << "(Cursor set to this person)" << std::endl;
  }
  created->info();
}

void CLI::add(commandArgs args) {
  if (!current_) {
    std::cerr << "add: You must create at least one person before. Your cursor is nobody!" << std::endl;
    return;
  }
  if (args.size() != 5 && args.size() != 6) {
    std::cerr << "Usage:" << std::endl << "\t add <relation> <first name> <last name> <sex> <birth> [<death>]" << std::endl;
    return;
  }
  std::vector<std::string> relationChain = parseLine(args[0], '.');
  std::vector<std::shared_ptr<struct Person>> p = computeRelation(std::vector<std::string>(relationChain.begin(), relationChain.end() - 1), current_);
  if (!p.size()) {
    std::cerr << "add: Could not get to that relation" << std::endl;
    return;
  }
  if (p.size() > 1) {
    std::cerr << "add: Grouping relation must be last" << std::endl;
    return;
  }
  std::shared_ptr<struct Person> created = parsePerson(std::vector<std::string>(args.begin() + 1, args.end()));
  if (!created) {
    std::cerr << "add: Could not create person" << std::endl;
    return;
  }
  if (!setRelation(relationChain.back(), p[0], created)) {
    std::cerr << "add: Could not create relation" << std::endl;
    return;
  }
  people_.push_back(created);
  std::cout << "Created person ID " << people_.size() - 1 << std::endl;
  created->info();
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
  if (!current_) {
    std::cerr << "info: You must create at least one person before. Your cursor is nobody!" << std::endl;
    return;
  }
  if (args.size() > 1) {
    std::cerr << "Usage:" << std::endl << "\t info [<relation>]" << std::endl;
    return;
  }
  if (args.empty()) {
    current_->info();
    return;
  }
  std::vector<std::string> relationChain = parseLine(args[0], '.');
  std::vector<std::shared_ptr<struct Person>> people = computeRelation(relationChain, current_);
  if (!people.size()) {
    std::cout << "Nobody" << std::endl;
    return;
  }
  for (auto person : people) {
    person->info();
  }
}

void CLI::list(commandArgs args) {
  unsigned i = 0;
  if (people_.empty()) {
    std::cout << "No person exists yet" << std::endl;
    return;
  }
  for (auto person : people_) {
    std::cout << i << ") ";
    person->info();
    i++;
  }
}

void CLI::select(commandArgs args) {
  if (args.size() != 1) {
    std::cerr << "Usage:" << std::endl << "\t select <relation>" << std::endl << "\t select <id>" << std::endl;
    return;
  }
  int id;
  if (sscanf(args[0].c_str(), "%d", &id) == 1) {
    if (id < 0 || id >= people_.size()) {
      std::cerr << "select: invalid/non-existent ID" << std::endl;
      return;
    }
    current_ = people_[id];
    current_->info();
    return;
  }
  std::vector<std::string> relationChain = parseLine(args[0], '.');
  std::vector<std::shared_ptr<struct Person>> p = computeRelation(relationChain, current_);
  if (!p.size()) {
    std::cerr << "select: Could not get to that relation" << std::endl;
    return;
  }
  if (p.size() > 1) {
    std::cerr << "select: Can't select a grouping relation" << std::endl;
    return;
  }
  current_ = p[0];
  current_->info();
}

void CLI::dump(commandArgs args) {
  std::cerr << "Still not supported" << std::endl;
}

void CLI::generateImage(commandArgs args) {
  std::cerr << "Still not supported" << std::endl;
}
/* commands */

} // namespace genea