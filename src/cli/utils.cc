#include "person.h"
#include <map>
#include <functional>

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
  return p->father_;
}

std::shared_ptr<struct Person> mother(std::shared_ptr<struct Person> p, const std::string& specifier) {
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

std::map<std::string, std::function<std::shared_ptr<struct Person>(std::shared_ptr<struct Person>, std::string)>> relations = {
  { "father", &father },
  { "mother", &mother },
  { "child", &child },
  { "sibling", &sibling }
};

std::map<std::string, std::function<std::vector<std::shared_ptr<struct Person>>(std::shared_ptr<struct Person>)>> relationGroups = {
  { "child", &children },
  { "sibling", &siblings }
};


} // namespace relation

/* utilities */

std::vector<std::shared_ptr<struct Person>> computeRelation(std::vector<std::string> relations, std::shared_ptr<struct Person> start) {
  std::shared_ptr<struct Person> p = start;
  unsigned cpt = 0;
  char name[100];
  for (auto& r : relations) {
    cpt++;
    bool group = relation::relationGroups.contains(r);
    bool last = (&r == &relations.back());
    if (last && group) {
      return relation::relationGroups[r](p);
    }
    if (!last && group) {
      std::cerr << "Relation '" << r << "' (" << cpt << "): grouping relations must be placed last" << std::endl;
    }
    auto colon = r.find(':');
    std::string rel = (colon == std::string::npos ? r : r.substr(0, colon));
    std::string spec = (colon == std::string::npos ? "" : r.substr(colon + 1));
    if (relation::relations.contains(rel)) {
      p = relation::relations[rel](p, spec);
      if (!p) {
        std::cerr << "Relation '" << r << "' (" << cpt << "): does not exist (yet?)" << std::endl;
        return {};
      }
    } else {
      std::cerr << "Relation '" << r << "' (" << cpt << "): unknown relation" << std::endl;
      return {};
    }
  }
  return { p };
}

bool setRelation(const std::string& relation, std::shared_ptr<struct Person> of, std::shared_ptr<struct Person> p) {
  if (relation == "father") {
    if (of->father_) {
      std::cerr << "Error: Father already exists" << std::endl;
      return false;
    }
    of->father_ = p;
    p->children_.push_back(of);
    return true;
  }
  if (relation == "mother") {
    if (of->mother_) {
      std::cerr << "Error: Mother already exists" << std::endl;
      return false;
    }
    of->mother_ = p;
    p->children_.push_back(of);
    return true;
  }
  if (relation == "children") {
    if (of->sex_ == Sex::MALE) {
      if (p->father_) {
        std::cerr << "Error: Child already has a father" << std::endl;
        return false;
      }
      of->children_.push_back(p);
      p->father_ = of;
      return true;
    }
    if (of->sex_ == Sex::FEMALE) {
      if (p->mother_) {
        std::cerr << "Error: Child already has a mother" << std::endl;
        return false;
      }
      of->children_.push_back(p);
      p->mother_ = of;
      return true;
    }
  }
  if (relation == "siblings") {
    if (!of->father_ && !of->mother_) {
      std::cerr << "Error: can't add a sibling if no parent is known" << std::endl;
      return false;
    }
    if (of->father_) {
      if (p->father_) {
        std::cerr << "Error: can't add sibling that already has a father" << std::endl;
        return false;
      }
    }
    if (of->mother_) {
      if (p->mother_) {
        std::cerr << "Error: can't add sibling that already has a father" << std::endl;
        return false;
      }
    }
    if (of->father_) {
      of->father_->children_.push_back(p);
      p->father_ = of->father_;
    }
    if (of->mother_) {
      of->mother_->children_.push_back(p);
      p->mother_ = of->mother_;
    }
    return true;
  }
  std::cerr << "Error: when adding a relation, the last one must be either mother, father, children or siblings" << std::endl;
  return false;
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
/* utilities */

} // namespace genea