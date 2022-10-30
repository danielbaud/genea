#include "person.h"
#include <map>
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
  if (specifier == "") {
    std::cerr << "child: needs a specifier" << std::endl;
    return nullptr;
  }
  for (auto& c : p->children_) {
    if (c->firstName_ == specifier)
      return c;
  }
  return nullptr;
}

std::shared_ptr<struct Person> sibling(std::shared_ptr<struct Person> p, const std::string& specifier) {
  if (specifier == "") {
    std::cerr << "sibling: needs a specifier" << std::endl;
    return nullptr;
  }
  std::vector<std::shared_ptr<struct Person>> s = siblings(p);
  for (auto& sib : s) {
    if (sib->firstName_ == specifier)
      return sib;
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
  if (other->sex_ == Sex::MALE)
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
    std::cerr << "child: needs a specifier" << std::endl;
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
  { "sibling", &sibling }
};

std::map<std::string, std::function<std::vector<std::shared_ptr<struct Person>>(std::shared_ptr<struct Person>)>> getRelationGroup = {
  { "child", &children },
  { "sibling", &siblings }
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
        std::cerr << "Relation '" << r << "' (" << cpt << "): does not exist" << std::endl;
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
     std::cerr << "ID " << arg << "is not an valid" << std::endl;
     return -1;
  }
  return id;
}

} // namespace utils

} // namespace genea