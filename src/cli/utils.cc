#include "person.h"
#include <map>
#include <set>
#include <algorithm>
#include <utility>
#include <fstream>
#include <functional>
#include <cassert>

namespace genea {

namespace relation {

std::vector<std::shared_ptr<struct Person>> children(std::shared_ptr<struct Person> p) {
  return p->children_;
}

std::vector<std::shared_ptr<struct Person>> siblings(std::shared_ptr<struct Person> p) {
  std::vector<std::shared_ptr<struct Person>> res;
  if (p->father_) {
    for (auto& child : p->father_->children_) {
      if (child != p)
        res.push_back(child);
    }
  }
  if (p->mother_) {
    for (auto& child : p->mother_->children_) {
      auto c = std::find(res.begin(), res.end(), child);
      if (c == res.end() && child != p)
        res.push_back(child);
    }
  }
  return res;
}

std::shared_ptr<struct Person> father(std::shared_ptr<struct Person> p, const std::string& specifier) {
  if (specifier != "") {
    std::cerr << "father: can't use specifier" << std::endl;
    return nullptr;
  }
  return p->father_;
}

std::shared_ptr<struct Person> mother(std::shared_ptr<struct Person> p, const std::string& specifier) {
  if (specifier != "") {
    std::cerr << "mother: can't use specifier" << std::endl;
    return nullptr;
  }
  return p->mother_;
}

std::shared_ptr<struct Person> child(std::shared_ptr<struct Person> p, const std::string& specifier) {
  for (auto& c : p->children_) {
    if (c->firstName_ == specifier || specifier == "")
      return c;
  }
  return nullptr;
}

std::shared_ptr<struct Person> sibling(std::shared_ptr<struct Person> p, const std::string& specifier) {
  std::vector<std::shared_ptr<struct Person>> s = siblings(p);
  for (auto& sib : s) {
    if (sib->firstName_ == specifier || specifier == "")
      return sib;
  }
  return nullptr;
}

std::shared_ptr<struct Person> spouse(std::shared_ptr<struct Person> p, const std::string& specifier) {
  for (auto& child : p->children_) {
    if (p == child->father_ && child->mother_) {
      if (specifier == "" || child->mother_->firstName_ == specifier)
        return child->mother_;
    }
    if (p == child->mother_ && child->father_) {
      if (specifier == "" || child->father_->firstName_ == specifier)
        return child->father_;
    }
  }
  return nullptr;
}

bool setFather(std::shared_ptr<struct Person> p, std::shared_ptr<struct Person> other) {
  if (p->father_) {
    std::cout << "Warning: father already exists and is being replaced" << std::endl;
    auto child = std::find(p->father_->children_.begin(), p->father_->children_.end(), p);
    assert(child != p->father_->children_.end());
    p->father_->children_.erase(child);
  }
  p->father_ = other;
  other->children_.push_back(p);
  return true;
}

bool setMother(std::shared_ptr<struct Person> p, std::shared_ptr<struct Person> other) {
  if (p->mother_) {
    std::cout << "Warning: mother already exists and is being replaced" << std::endl;
    auto child = std::find(p->mother_->children_.begin(), p->mother_->children_.end(), p);
    assert(child != p->mother_->children_.end());
    p->mother_->children_.erase(child);
  }
  p->mother_ = other;
  other->children_.push_back(p);
  return true;
}

bool setChild(std::shared_ptr<struct Person> p, std::shared_ptr<struct Person> other) {
  if (p->sex_ == Sex::MALE)
    return setFather(other, p);
  return setMother(other, p);
}

bool setSibling(std::shared_ptr<struct Person> p, std::shared_ptr<struct Person> other) {
  if (!p->father_ && !p->mother_) {
    std::cerr << "Error: No parent known, impossible to create sibling" << std::endl;
    return false;
  }
  if (p->father_) {
    setFather(other, p->father_);
  }
  if (p->mother_) {
    setMother(other, p->mother_);
  }
  return true;
}

bool rmFather(std::shared_ptr<struct Person> p, const std::string& specifier) {
  if (specifier != "") {
    std::cerr << "father: can't use specifier" << std::endl;
    return false;
  }
  if (!p->father_) {
    std::cerr << "Warning: father does not exist" << std::endl;
    return false;
  }
  auto child = std::find(p->father_->children_.begin(), p->father_->children_.end(), p);
  assert(child != p->father_->children_.end());
  p->father_->children_.erase(child);
  p->father_ = nullptr;
  return true;
}

bool rmMother(std::shared_ptr<struct Person> p, const std::string& specifier) {
  if (specifier != "") {
    std::cerr << "mother: can't use specifier" << std::endl;
    return false;
  }
  if (!p->mother_) {
    std::cerr << "Warning: mother does not exist" << std::endl;
    return false;
  }
  auto child = std::find(p->mother_->children_.begin(), p->mother_->children_.end(), p);
  assert(child != p->mother_->children_.end());
  p->mother_->children_.erase(child);
  p->mother_ = nullptr;
  return true;
}

bool rmChild(std::shared_ptr<struct Person> p, const std::string& specifier) {
  if (specifier == "") {
    std::cerr << "child: removing needs a specifier" << std::endl;
    return false;
  }
  auto child = std::find_if(p->children_.begin(), p->children_.end(), [&specifier](std::shared_ptr<struct Person> c) {
    return c->firstName_ == specifier;
  });
  if (child == p->children_.end()) {
    std::cerr << "child: " << specifier << " not found" << std::endl;
    return false;
  }
  if (p == (*child)->mother_) {
    (*child)->mother_ = nullptr;
    p->children_.erase(child);
    return true;
  }
  if (p == (*child)->father_) {
    (*child)->father_ = nullptr;
    p->children_.erase(child);
    return true;
  }
  assert(false);
}

std::map<std::string, std::function<std::shared_ptr<struct Person>(std::shared_ptr<struct Person>, std::string)>> getRelation = {
  { "father", &father },
  { "mother", &mother },
  { "child", &child },
  { "sibling", &sibling },
  { "spouse", &spouse }
};

std::map<std::string, std::function<std::vector<std::shared_ptr<struct Person>>(std::shared_ptr<struct Person>)>> getRelationGroup = {
  { "children", &children },
  { "siblings", &siblings }
};

std::map<std::string, std::function<bool(std::shared_ptr<struct Person>, std::shared_ptr<struct Person>)>> setRelation = {
  { "father", &setFather },
  { "mother", &setMother },
  { "child", &setChild },
  { "sibling", &setSibling }
};

std::map<std::string, std::function<bool(std::shared_ptr<struct Person>, std::string)>> rmRelation = {
  { "father", &rmFather },
  { "mother", &rmMother },
  { "child", &rmChild }
};


} // namespace relation

namespace utils {

std::vector<std::shared_ptr<struct Person>> computeRelation(std::vector<std::string> relations, std::shared_ptr<struct Person> start) {
  std::shared_ptr<struct Person> p = start;
  unsigned cpt = 0;
  char name[100];
  for (auto& r : relations) {
    cpt++;
    bool group = relation::getRelationGroup.contains(r);
    bool last = (&r == &relations.back());
    if (last && group) {
      return relation::getRelationGroup[r](p);
    }
    if (!last && group) {
      std::cerr << "Relation '" << r << "' (" << cpt << "): grouping relations must be placed last" << std::endl;
    }
    auto colon = r.find(':');
    std::string rel = (colon == std::string::npos ? r : r.substr(0, colon));
    std::string spec = (colon == std::string::npos ? "" : r.substr(colon + 1));
    if (relation::getRelation.contains(rel)) {
      p = relation::getRelation[rel](p, spec);
      if (!p) {
        std::cerr << "Relation '" << r << "' (" << cpt << "): is not set" << std::endl;
        return {};
      }
    } else {
      std::cerr << "Relation '" << r << "' (" << cpt << "): unknown relation" << std::endl;
      return {};
    }
  }
  return { p };
}


bool setRelation(const std::string& relation, std::shared_ptr<struct Person> p, std::shared_ptr<struct Person> other) {
  if (!relation::setRelation.contains(relation)) {
    std::cerr << "Relation '" << relation << "' (last): Unknown relation" << std::endl;
    return false;
  }
  return relation::setRelation[relation](p, other);
}

bool rmRelation(const std::string& relation, std::shared_ptr<struct Person> p) {
  auto colon = relation.find(':');
  std::string rel = (colon == std::string::npos ? relation : relation.substr(0, colon));
  std::string spec = (colon == std::string::npos ? "" : relation.substr(colon + 1));
  if (!relation::rmRelation.contains(rel)) {
    std::cerr << "Relation '" << relation << "' (last): Unknown relation" << std::endl;
    return false;
  }
  return relation::rmRelation[rel](p, spec);
}

bool parseDate(const std::string& s, struct Date* d) {
  if (s == "?")
    return true;
  int day, month, year;
  if (sscanf(s.c_str(), "%d/%d/%d", &day, &month, &year) == 3) {
    d->day_ = day;
    d->month_ = month;
    d->year_ = year;
    return true;
  }
  if (sscanf(s.c_str(), "%d/%d", &month, &year) == 2) {
    d->month_ = month;
    d->year_ = year;
    return true;
  }
  if (sscanf(s.c_str(), "%d", &year) == 1) {
    d->year_ = year;
    return true;
  }
  return false;
}


std::shared_ptr<struct Person> parsePerson(std::vector<std::string> args) {
  if (args.size() != 4 && args.size() != 5) {
    std::cerr << "Person: invalid number of arguments" << std::endl;
    return nullptr;
  }
  std::string fname = args[0];
  std::string lname = args[1];
  if (args[2] != "M" && args[2] != "F") {
    std::cerr << "Error: sex must be either M of F" << std::endl;
    return nullptr;
  }
  Sex sex = args[2] == "M" ? Sex::MALE : Sex::FEMALE;
  struct Date birth = Date();
  if (!parseDate(args[3], &birth)) {
    std::cerr << "Error: birth date must be either dd/mm/yyyy, mm/yyyy, yyyy or ? if unknown" << std::endl;
    return nullptr;
  }
  if (args.size() == 5) {
    struct Date death = Date();
    if (!parseDate(args[4], &death)) {
      std::cerr << "Error: death date must be either dd/mm/yyyy, mm/yyyy, yyyy or ? if unknown" << std::endl;
      return nullptr;
    }
    return std::make_shared<struct Person>(fname, lname, sex, birth, death);
  }
  return std::make_shared<struct Person>(fname, lname, sex, birth);
}


std::vector<std::string> parseLine(const std::string& line, char sep) {
  std::vector<std::string> command = std::vector<std::string>();
  unsigned start = 0;
  unsigned end = 0;
  while (line[end] == sep)
    end++;
  while (end < line.size()) {
    start = end;
    while (end < line.size() && line[end] != sep)
      end++;
    command.push_back(line.substr(start, end - start));
    while (end < line.size() && line[end] == sep)
      end++;
  }
  return command;
}

int parseId(const std::string& arg) {
  int id;
  if (sscanf(arg.c_str(), "%d", &id) != 1) {
     return -1;
  }
  return id;
}

std::vector<std::shared_ptr<struct Person>> parseFile(std::ifstream& in) {
  std::vector<std::shared_ptr<struct Person>> res;
  std::string line;
  int n;
  in >> n;
  std::getline(in, line);
  if (!in.good()) {
    in.close();
    std::cerr << "File is invalid or corrupted" << std::endl;
    return {};
  }
  for (int i = 0; i < n; ++i) {
    std::getline(in, line);
    std::vector<std::string> args = parseLine(line, ' ');
    std::shared_ptr<struct Person> p = parsePerson(args);
    if (!p) {
      in.close();
      return {};
    }
    res.push_back(p);
  }
  for (int i = 0; i < n; ++i) {
    std::getline(in, line);
    int id1, id2;
    if (sscanf(line.c_str(), "%d %d\n", &id1, &id2) != 2) {
      in.close();
      return {};
    }
    if (id1 >= 0 && id1 < n)
      setRelation("father", res[i], res[id1]);
    if (id2 >= 0 && id2 < n)
      setRelation("mother", res[i], res[id2]);
  }
  in.close();
  std::cout << "Loaded " << res.size() << " people" << std::endl;
  return res;
}

// recursive exploration
void treeExplore(
  std::shared_ptr<struct Person> p,
  int level,
  std::vector<std::pair<int, std::shared_ptr<struct Person>>>& list,
  std::vector<bool>& map
) {
  if (!p || map[p->id]) {
    return;
  }
  map[p->id] = true;
  for (auto& child : p->children_) {
    treeExplore(child, level + 1, list, map);
  }
  list.push_back(std::make_pair(level, p));
  treeExplore(p->father_, level - 1, list, map);
  treeExplore(p->mother_, level - 1, list, map);
}

std::vector<std::vector<std::shared_ptr<struct Person>>> generations(std::shared_ptr<struct Person> start, int maxPeople) {
  std::vector<std::pair<int, std::shared_ptr<struct Person>>> people;
  std::vector<bool> travelMap = std::vector<bool>(maxPeople, false);
  treeExplore(start, 0, people, travelMap);
  int minGen = 0;
  int maxGen = 0;
  for (auto& person : people) {
    if (person.first < minGen)
      minGen = person.first;
    if (person.first > maxGen)
      maxGen = person.first;
  }
  auto res = std::vector<std::vector<std::shared_ptr<struct Person>>>(
    maxGen - minGen + 1, 
    std::vector<std::shared_ptr<struct Person>>()
  );
  for (auto& person : people) {
    res[person.first - minGen].push_back(person.second);
  }
  return res;
}

std::string uniqueDualId(std::shared_ptr<struct Person> a, std::shared_ptr<struct Person> b) {
  return "r" + std::to_string(std::min(a->id, b->id)) + "x" + std::to_string(std::max(a->id, b->id));
}

std::string dotCompleteSpouses(std::ofstream& out, std::set<int>& ids, std::shared_ptr<struct Person> p) {
  std::set<std::shared_ptr<struct Person>> spouses = {};
  for (auto& child : p->children_) {
    if (child->father_ && child->mother_) {
      if (child->father_ == p && !spouses.contains(child->mother_)) {
        spouses.insert(child->mother_);
      } else if (child->mother_ == p && !spouses.contains(child->father_)) {
        spouses.insert(child->father_);
      }
    }
  }
  std::string prevId = p->dotId();
  for (auto& spouse : spouses) {
    if (!ids.contains(spouse->id)) {
      std::string comb = utils::uniqueDualId(p, spouse);
      out << spouse->dot() << std::endl;;
      out << comb << " [shape=point, width=0.05]" << std::endl;
      out << p->dotId() << "--" << comb << "--" << spouse->dotId() << std::endl;
      ids.insert(spouse->id);
      prevId = dotCompleteSpouses(out, ids, spouse);
    }
  }
  return prevId;
}

} // namespace utils

} // namespace genea