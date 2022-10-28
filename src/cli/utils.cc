#include "person.h"

namespace genea {
/* utilities */
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

std::shared_ptr<struct Person> computeRelation(std::vector<std::string> relations, std::shared_ptr<struct Person> start) {
  std::shared_ptr<struct Person> p = start;
  unsigned cpt = 0;
  char name[100];
  for (auto relation : relations) {
    cpt++;
    if (relation == "father") {
      p = p->father_;
    } else if (relation == "mother") {
      p = p->mother_;
    } else if (sscanf(relation.c_str(), "child:%s", name) == 1) {
      std::string n(name);
      unsigned i = 0;
      bool found = false;
      while (i < p->children_.size() && !found) {
        if (p->children_[i]->firstName_ == n) {
          found = true;
          p = p->children_[i];
        }
        i++;
      }
      if (!found)
        p = nullptr;
    } else if (sscanf(relation.c_str(), "sibling:%s", name) == 1) {
      std::string n(name);
      unsigned i = 0;
      bool found = false;
      if (p->father_) {
        while (i < p->father_->children_.size() && !found) {
          if (p->father_->children_[i]->firstName_ == n) {
            found = true;
            p = p->father_->children_[i];
          }
          i++;
        }
      }
      i = 0;
      if (p->mother_) {
        while (i < p->mother_->children_.size() && !found) {
          if (p->mother_->children_[i]->firstName_ == n) {
            found = true;
            p = p->mother_->children_[i];
          }
          i++;
        }
      }
      if (!found)
        p = nullptr;
    } else if (relation == "children" || relation == "siblings") {
      std::cerr << "Relation " << relation << " can't be used unless it is the last one" << std::endl;
      return nullptr;
    } else {
      std::cerr << "Relation " << relation << " is unknown" << std::endl;
      return nullptr;
    }
    if (!p) {
      std::cerr << "Relation " << cpt << ": " << relation << " does not exist" << std::endl;
    }
  }
  return p;
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
      p->mother_ = of->father_;
    }
    return true;
  }
  std::cerr << "Error: when adding a relation, the last one must be either mother, father, children or siblings" << std::endl;
  return false;
}

std::vector<std::string> splitLine(const std::string& line, char sep) {
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