#include "cli.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <set>

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
  { "create", std::bind(&CLI::create, this, std::placeholders::_1) },
  { "add", std::bind(&CLI::add, this, std::placeholders::_1) },
  { "attach", std::bind(&CLI::attach, this, std::placeholders::_1) },
  { "remove", std::bind(&CLI::remove, this, std::placeholders::_1) },
  { "overwrite", std::bind(&CLI::overwrite, this, std::placeholders::_1) },
  { "info", std::bind(&CLI::info, this, std::placeholders::_1) },
  { "list", std::bind(&CLI::list, this, std::placeholders::_1) },
  { "search", std::bind(&CLI::search, this, std::placeholders::_1) },
  { "select", std::bind(&CLI::select, this, std::placeholders::_1) },
  { "dump", std::bind(&CLI::dump, this, std::placeholders::_1) },
  { "load", std::bind(&CLI::load, this, std::placeholders::_1)},
  { "generate-image", std::bind(&CLI::generateImage, this, std::placeholders::_1) }
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
  std::vector<std::shared_ptr<struct Person>> people = utils::parseFile(f);
  if (!people.size()) {
    std::cerr << "Warning: file " << file << " is corrupted/incorrect" << std::endl;
    std::cout << "Created empty tree" << std::endl;
    return;
  }
  people_ = people;
  int i = 0;
  for (auto& person : people_) {
    person->id = i;
    i++;
  }
  current_ = people_[0];
  std::cout << "Tree loaded from " << file << std::endl;
  std::cout << "(Cursor set to person ID 0)" << std::endl;
}

void CLI::run() {
  if (isatty(STDIN_FILENO))
    std::cerr << PS1;

  std::string line;
  std::getline(std::cin, line);

  bool exit = false;

  while (!std::cin.eof() && !exit) {
    std::vector<std::string> command = utils::parseLine(line, ' ');
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
  std::cerr << "\t\t\t\t\t\t Creates a new person which is related to nobody. It will be reachable from IDs" << std::endl;
  std::cerr << "\t add <relation> <first name> <last name> <sex> <birth> [<death>]" << std::endl;
  std::cerr << "\t\t\t\t\t\t Creates a new person which is <relation> of the current person" << std::endl;
  std::cerr << "\t overwrite <first name> <last name> <sex> <birth> [<death>]" << std::endl;
  std::cerr << "\t\t\t\t\t\t Overwrite the current person with given information" << std::endl;
  std::cerr << "\t attach <relation> <id>\t\t\t Sets the person whose ID is <id> to be <relation> of the current person" << std::endl;
  std::cerr << "\t attach <relation> <id1> <id2>\t\t Sets the person whose ID is <id2> to be <relation> of the person whose ID is <id1>" << std::endl;
  std::cerr << "\t remove <relation> \t\t\t Removes the person who is <relation> of the current person" << std::endl;
  std::cerr << "\t remove <id> \t\t\t\t Removes the person whose ID is <id>. Warning, the person is entirely removed" << std::endl;

  // Info commands
  std::cerr << std::endl << "Information commands:" << std::endl;
  std::cerr << "\t info\t\t\t\t\t Displays information about the current person" << std::endl;
  std::cerr << "\t info <relation>\t\t\t Displays information about the <relation> of the current person" << std::endl;
  std::cerr << "\t info <id>\t\t\t\t Displays information about the person whose ID is <id>" << std::endl;
  std::cerr << "\t list\t\t\t\t\t Displays a list of all people of the tree with their given ID" << std::endl;
  std::cerr << "\t search <name>\t\t\t\t Displays all the people whose first name or last name matches <name>" << std::endl;

  // Move commands
  std::cerr << std::endl << "Move commands:" << std::endl;
  std::cerr << "\t select <relation>\t\t\t Moves the cursor to the <relation> of the current person" << std::endl;
  std::cerr << "\t select <id>\t\t\t\t Moves the cursor to the person whose ID is <id>" << std::endl;

  // Dump commands
  std::cerr << std::endl << "File commands:" << std::endl;
  std::cerr << "\t dump <file>\t\t\t\t Dumps the current tree to <file>" << std::endl;
  std::cerr << "\t load <file>\t\t\t\t Loads the file <file> into the current tree" << std::endl;
  std::cerr << "\t generate-image <file>\t\t\t Generates a graph view of the genealogical tree to <file>" << std::endl;
  std::cerr << "\t\t\t\t\t\t The generated graph will not contain people that are not related to the current person" << std::endl;
  std::cerr << "\t\t\t\t\t\t (e.g loaded people or created & non-attached people)" << std::endl;
  // Relations
  std::cerr << std::endl << "Available relations are:" << std::endl;
  std::cerr << "\t father, mother, child:<first name>, sibling:<first name>, child (grouping), sibling (grouping)" << std::endl;
  std::cerr << std::endl << "Relations can be chained separated by a point ('.')" << std::endl;
  std::cerr << "\t Ex: select father.mother.sibling:Alice.child:Bob.father" << std::endl;
  std::cerr << "\t Ex: info child:Charlie.mother.sibling" << std::endl << std::endl;
}

void CLI::create(commandArgs args) {
  if (args.size() != 4 && args.size() != 5) {
    std::cerr << "Usage:" << std::endl << "\t create <first name> <last name> <sex> <birth> [<death>]" << std::endl;
    return;
  }
  std::shared_ptr<struct Person> created = utils::parsePerson(args);
  if (!created) {
    std::cerr << "create: Could not create person" << std::endl;
    return;
  }
  created->id = people_.size();
  people_.push_back(created);
  std::cout << "Created person ID " << created->id << std::endl;
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
  std::vector<std::string> relationChain = utils::parseLine(args[0], '.');
  std::vector<std::shared_ptr<struct Person>> p = utils::computeRelation(std::vector<std::string>(relationChain.begin(), relationChain.end() - 1), current_);
  if (!p.size()) {
    std::cerr << "add: Could not get to that relation" << std::endl;
    return;
  }
  if (p.size() > 1) {
    std::cerr << "add: Grouping relation must be last" << std::endl;
    return;
  }
  std::shared_ptr<struct Person> created = utils::parsePerson(std::vector<std::string>(args.begin() + 1, args.end()));
  if (!created) {
    std::cerr << "add: Could not create person" << std::endl;
    return;
  }
  if (!utils::setRelation(relationChain.back(), p[0], created)) {
    std::cerr << "add: Could not create relation" << std::endl;
    return;
  }
  created->id = people_.size();
  people_.push_back(created);
  std::cout << "Created person ID " << created->id << std::endl;
  created->info();
}

void CLI::attach(commandArgs args) {
  if (!current_) {
    std::cerr << "attach: You must create at least one person before. Your cursor is nobody!" << std::endl;
    return;
  }
  if (args.size() != 2 && args.size() != 3) {
    std::cerr << "Usage:" << std::endl << "\t attach <relation> <id>" << std::endl << "\t attach <relation> <id1> <id2>" << std::endl;
    return;
  }
  std::vector<std::string> relationChain = utils::parseLine(args[0], '.');
  int id1 = utils::parseId(args[1]);
  if (id1 < 0 || id1 >= people_.size()) {
    std::cerr << "attach: " << args[1] << "is not a valid ID" << std::endl;
    return;
  }
  std::vector<std::shared_ptr<struct Person>> p = utils::computeRelation(std::vector<std::string>(relationChain.begin(), relationChain.end() - 1), current_);
  if (!p.size()) {
    std::cerr << "attach: Could not get to that relation" << std::endl;
    return;
  }
  if (p.size() > 1) {
    std::cerr << "attach: Grouping relation must be last" << std::endl;
    return;
  }
  if (args.size() == 3) {
    int id2 = utils::parseId(args[2]);
    if (id2 < 0 || id2 >= people_.size()) {
      std::cerr << "attach: " << args[2] << "is not a valid ID" << std::endl;
      return;
    }
    if (!utils::setRelation(relationChain.back(), people_[id1], people_[id2])) {
      std::cerr << "attach: Could not set relation" << std::endl;
    }
    return;
  }
  if (!utils::setRelation(relationChain.back(), current_, people_[id1])) {
     std::cerr << "attach: Could not set relation" << std::endl;
  }
}

void CLI::remove(commandArgs args) {
  if (!current_) {
    std::cerr << "remove: You must create at least one person before. Your cursor is nobody!" << std::endl;
    return;
  }
  if (args.size() != 1) {
    std::cerr << "Usage:" << std::endl << "\t remove <relation>" << std::endl << "\t remove <id>" << std::endl;
    return;
  }
  int id = utils::parseId(args[0]);
  if (id >= 0 && id < people_.size()) {
    utils::rmRelation("father", people_[id]);
    utils::rmRelation("mother", people_[id]);
    while (people_[id]->children_.size()) {
      utils::rmRelation("child:" + people_[id]->children_[0]->firstName_, people_[id]);
    }
    if (current_ == people_[id]) {
      if (people_.size() == 1) {
        std::cout << "Warning: cursor set to nobody" << std::endl;
        current_ = nullptr;
      } else {
        int newCursor = id == 0 ? 1 : 0;
        std::cout << "(Cursor set to person 0)" << std::endl;
        current_ = people_[newCursor];
      }
    }
    people_.erase(people_.begin() + id);
    for (auto person = people_.begin() + id; person != people_.end(); ++person) {
      (*person)->id--;
    }
    return;
  }
  std::vector<std::string> relationChain = utils::parseLine(args[0], '.');
  std::vector<std::shared_ptr<struct Person>> p = utils::computeRelation(relationChain, current_);
  if (!p.size()) {
    std::cerr << "remove: Could not get to that relation" << std::endl;
    return;
  }
  if (p.size() > 1) {
    std::cerr << "remove: Can't remove a grouping relation" << std::endl;
    return;
  }
  if (!utils::rmRelation(relationChain.back(), p[0])) {
    std::cerr << "remove: Could not remove relation" << std::endl;
  }
}

void CLI::overwrite(commandArgs args) {
  if (!current_) {
    std::cerr << "overwrite: You must create at least one person before. Your cursor is nobody!" << std::endl;
    return;
  }
  if (args.size() != 4 && args.size() != 5) {
    std::cerr << "Usage:" << std::endl << "\t overwrite <first name> <last name> <sex> <birth> [<death>]" << std::endl;
    return;
  }
  std::shared_ptr<struct Person> created = utils::parsePerson(args);
  if (!created) {
    std::cerr << "overwrite: Could not modify person" << std::endl;
    return;
  }
  current_->firstName_ = created->firstName_;
  current_->lastName_ = created->lastName_;
  current_->sex_ = created->sex_;
  current_->born_ = created->born_;
  current_->dead_ = created->dead_;
  current_->info();
}

void CLI::info(commandArgs args) {
  if (!current_) {
    std::cerr << "info: You must create at least one person before. Your cursor is nobody!" << std::endl;
    return;
  }
  if (args.size() > 1) {
    std::cerr << "Usage:" << std::endl << "\t info [<relation> | <id>]" << std::endl;
    return;
  }
  if (args.empty()) {
    current_->info();
    return;
  }
  int id = utils::parseId(args[0]);
  if (id >= 0 && id < people_.size()) {
    people_[id]->info();
    return;
  }
  std::vector<std::string> relationChain = utils::parseLine(args[0], '.');
  std::vector<std::shared_ptr<struct Person>> people = utils::computeRelation(relationChain, current_);
  if (!people.size()) {
    std::cout << "Nobody" << std::endl;
    return;
  }
  for (auto& person : people) {
    person->info();
  }
}

void CLI::list(commandArgs args) {
  if (people_.empty()) {
    std::cout << "No person exists yet" << std::endl;
    return;
  }
  for (auto& person : people_) {
    person->info();
  }
}

void CLI::search(commandArgs args) {
  if (args.size() != 1) {
    std::cerr << "Usage:" << std::endl << "\t search <name>" << std::endl;
    return;
  }
  if (people_.empty()) {
    std::cout << "No person exists yet" << std::endl;
    return;
  }
  for (auto& person : people_) {
    if (person->firstName_ == args[0] || person->lastName_ == args[0]) {
      person->info();
    }
  }
}

void CLI::select(commandArgs args) {
  if (!current_) {
    std::cerr << "select: You must create at least one person before. Your cursor is nobody!" << std::endl;
    return;
  }
  if (args.size() != 1) {
    std::cerr << "Usage:" << std::endl << "\t select <relation>" << std::endl << "\t select <id>" << std::endl;
    return;
  }
  int id = utils::parseId(args[0]);
  if (id != -1) {
    if (id < 0 || id >= people_.size()) {
      std::cerr << "select: ID does not exist" << std::endl;
      return;
    }
    current_ = people_[id];
    current_->info();
    return;
  }
  std::vector<std::string> relationChain = utils::parseLine(args[0], '.');
  std::vector<std::shared_ptr<struct Person>> p = utils::computeRelation(relationChain, current_);
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
  if (!people_.size()) {
    std::cerr << "Nobody exists" << std::endl;
    return;
  }
  if (args.size() != 1) {
    std::cerr << "Usage:" << std::endl << "\t dump <file>" << std::endl;
    return;
  }
  std::ofstream out(args[0]);
  if (!out.good()) {
    std::cerr << "dump: Could not write to file " << args[0] << std::endl;
    return;
  }
  out << people_.size() << std::endl;
  int i = 0;
  for (auto& person : people_) {
    person->id = i;
    i++;
    out << person->dump() << std::endl;
  }
  for (auto& person : people_) {
    out << (person->father_ ? person->father_->id : -1) << ' ' << (person->mother_ ? person->mother_->id : -1) << std::endl;
  }
  out.close();
  std::cout << "Tree dumped to " << args[0] << std::endl;
}

void CLI::load(commandArgs args) {
  if (args.size() != 1) {
    std::cerr << "Usage:" << std::endl << "\t load <file>" << std::endl;
    return;
  }
  std::ifstream in(args[0]);
  if (!in.good()) {
    std::cerr << "load: Could not open " << args[0] << std::endl;
    return;
  }
  std::vector<std::shared_ptr<struct Person>> people = utils::parseFile(in);
  if (!people.size()) {
    std::cerr << "load: Could not load file" << std::endl;
    return;
  }
  int i = 0;
  for (auto& person : people) {
    person->id = i + people_.size();
    people_.push_back(person);
  }
  if (!current_) {
    current_ = people_[0];
    std::cout << "(Cursor set to ID 0)" << std::endl;
  }
}

void CLI::generateImage(commandArgs args) {
  if (!current_) {
    std::cerr << "generate-image: You must create at least one person before. Your cursor is nobody!" << std::endl;
    return;
  }
  if (args.size() != 1) {
    std::cerr << "Usage:" << std::endl << "\t generate-image <file>" << std::endl;
    return;
  }
  std::string dotFile = args[0] + ".dot";
  std::ofstream out(dotFile);
  if (!out.good()) {
    std::cerr << "generate-image: Could not write DOT file " << dotFile << std::endl;
    return;
  }
  auto gens = utils::generations(current_, people_.size());
  int iGen = 0;
  assert(gens.size() > 0);
  // from oldest to get a proper order
  gens = utils::generations(gens[0][0], people_.size());

  out << "graph G {" << std::endl;
  out << "graph [newrank=true, ranksep=3, concentrate=true, overlap=false, splines=true]" << std::endl;
  out << "edge [dir=none]" << std::endl;
  while (iGen < gens.size()) {
    out << "subgraph gen" << iGen << " {" << std::endl << "rank = same" << std::endl;
    std::set<int> ids = {};
    std::string prevId = "";
    for (auto& person : gens[iGen]) {
      std::set<std::shared_ptr<struct Person>> spouses = {};
      if (!ids.contains(person->id)) {
        out << person->dot() << std::endl;
        ids.insert(person->id);
        for (auto& child : person->children_) {
          if (child->father_ && child->mother_) {
            if (child->father_ == person && !spouses.contains(child->mother_)) {
              spouses.insert(child->mother_);
            } else if (child->mother_ == person && !spouses.contains(child->father_)) {
              spouses.insert(child->father_);
            }
          }
        }
        if (prevId != "") {
          out << prevId << "--" << person->dotId() << " [style=invis]" << std::endl;
        }
        prevId = person->dotId();
        bool firstSpouse = true;
        for (auto& spouse : spouses) {
          std::string comb = utils::uniqueDualId(person, spouse);
          out << spouse->dot() << std::endl;;
          out << comb << " [shape=point, width=0.05]" << std::endl;
          out << person->dotId() << "--" << comb << "--" << spouse->dotId() << std::endl;
          if (!firstSpouse)
            out << prevId << "--" << comb << " [style=invis]" <<std::endl;
          prevId = spouse->dotId();
          ids.insert(spouse->id);
          firstSpouse = false;
        }
      }
    }
    out << '}' << std::endl;

    for (auto& person : gens[iGen]) {
      if (person->father_ && person->mother_) {
        out << utils::uniqueDualId(person->father_, person->mother_)  + ":s" << "--" << person->dotId() + ":n" << std::endl;
      } else if (person->father_) {
        out << person->father_->dotId() + ":s" << "--" << person->dotId() + ":n" << std::endl;
      } else if (person->mother_) {
        out << person->mother_->dotId() + ":s" << "--" << person->dotId() + ":n" << std::endl;
      }
    }
    iGen++;
  }
  out << "}" << std::endl;
  out.close();

  int status = system(("2> /dev/null dot -Tpng " + dotFile + " 1> " + args[0]).c_str());
  if (status == 127) {
    std::cerr << "generate-image: Graphviz is not installed. Generated DOT file at " << dotFile << std::endl;
    return;
  }
  //std::remove(dotFile.c_str());
  if (status) {
    std::cerr << "generate-image: error in image generation from graphviz (code " << status << ")" << std::endl;
    std::remove(args[0].c_str());
    return;
  }
  std::cout << "Generated PNG file at " << args[0] << std::endl;
}
/* commands */

} // namespace genea