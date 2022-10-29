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
    std::vector<std::string> command = splitLine(line, ' ');
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
  std::cerr << std::endl << "At all times (except when no person exists), a cursor is on a person on the genealogic tree" << std::endl;
  
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
  std::cerr << "\t father, mother, child:<first name>, sibling:<first name>, children (info only), siblings (info only)" << std::endl;
  std::cerr << std::endl << "Relations can be chained separated by a point ('.')" << std::endl;
  std::cerr << "\t Ex: select father.mother.sibling[Alice].child[Bob].father" << std::endl;
  std::cerr << "\t Ex: info child[Charlie].mother.siblings" << std::endl << std::endl;
}

void CLI::create(commandArgs args) {
  if (args.size() != 4 && args.size() != 5) {
    std::cerr << "Usage: create <first name> <last name> <sex> <birth> [<death>]" << std::endl;
    return;
  }
  std::string fname = args[0];
  std::string lname = args[1];
  if (args[2] != "M" && args[2] != "F") {
    std::cerr << "Error: sex must be either M of F" << std::endl;
    return;
  }
  Sex sex = args[2] == "M" ? Sex::MALE : Sex::FEMALE;
  struct Date birth = Date();
  if (!parseDate(args[3], &birth)) {
    std::cerr << "Error: birth date must be either dd/mm/yyyy, mm/yyyy, yyyy or ? if unknown" << std::endl;
    return;
  }
  if (args.size() == 5) {
    struct Date death = Date();
    if (!parseDate(args[4], &death)) {
      std::cerr << "Error: death date must be either dd/mm/yyyy, mm/yyyy, yyyy or ? if unknown" << std::endl;
      return;
    }

    people_.push_back(std::make_shared<struct Person>(fname, lname, sex, birth, death));
  } else {
    people_.push_back(std::make_shared<struct Person>(fname, lname, sex, birth));
  }
  std::cout << "Created person ID " << people_.size() - 1 << std::endl;
  people_.back()->info();
  if (!current_) {
    current_ = people_.back();
    std::cout << "Cursor set to this person" << std::endl;
  }
}

void CLI::add(commandArgs args) {
  if (!current_) {
    std::cerr << "Error: You must create at least one person before. Your cursor is nobody!" << std::endl;
    return;
  }
  if (args.size() != 5 && args.size() != 6) {
    std::cerr << "Usage: add <relation> <first name> <last name> <sex> <birth> [<death>]" << std::endl;
    return;
  }
  std::vector<std::string> relationChain = splitLine(args[0], '.');
  std::shared_ptr<struct Person> p = computeRelation(std::vector<std::string>(relationChain.begin(), relationChain.end() - 1), current_);
  if (!p) {
    std::cerr << "Error: could not add that relation" << std::endl;
    return;
  }
  std::string lastRelation = relationChain.back();
  create(std::vector<std::string>(args.begin() + 1, args.end()));
  if (!setRelation(lastRelation, p, people_.back())) {
    people_.pop_back();
    std::cerr << "Removed person ID " << people_.size() << std::endl;
    return;
  }
}

void CLI::attach(commandArgs args) {
  return;
}

void CLI::remove(commandArgs args) {
  if (args.size() != 1) {
    std::cerr << "Usage: remove <relation>" << std::endl;
    return;
  }
  
}

void CLI::overwrite(commandArgs args) {
  return;
}

void CLI::info(commandArgs args) {
  if (args.size() > 1) {
    std::cerr << "Usage: info [relation]" << std::endl;
    return;
  }
  if (args.empty()) {
    current_->info();
    return;
  }
  std::vector<std::string> relationChain = splitLine(args[0], '.');
  std::shared_ptr<struct Person> p = computeRelation(std::vector<std::string>(relationChain.begin(), relationChain.end() - 1), current_);
  if (!p) {
    std::cerr << "Error: could not compute that relation" << std::endl;
    return;
  }
  std::string lastRelation = relationChain.back();
  if (lastRelation == "children") {
    if (p->children_.empty()) {
      std::cout << "No children" << std::endl;
      return;
    }
    for (auto child : p->children_) {
      child->info();
    }
  } else if (lastRelation == "siblings") {
    std::vector<std::shared_ptr<struct Person>> siblings = getSiblings(p);
    if (siblings.empty()) {
      std::cout << "No siblings" << std::endl;
      return;
    }
    for (auto sibling : siblings) {
      sibling->info();
    }
  } else {
    std::shared_ptr<struct Person> t = computeRelation({lastRelation}, p);
    if (t) {
      t->info();
    }
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
  std::cerr << "Still not supported" << std::endl;
}

void CLI::dump(commandArgs args) {
  std::cerr << "Still not supported" << std::endl;
}

void CLI::generateImage(commandArgs args) {
  std::cerr << "Still not supported" << std::endl;
}
/* commands */

} // namespace genea