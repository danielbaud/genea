#pragma once

#include <string>
#include <optional>
#include <memory>
#include <vector>
#include <iostream>

namespace genea {

enum class Sex {
  MALE,
  FEMALE
};

struct Date {

  Date(): year_(-1), month_(-1), day_(-1) {};
  Date(int year): year_(year), month_(-1), day_(-1) {};
  Date(int year, int month): year_(year), month_(month), day_(-1) {};
  Date(int year, int month, int day) : year_(year), month_(month), day_(day) {};

  std::string toString() {
    if (year_ == -1)
      return "?";
    std::string res = "";
    if (day_ != -1)
      res = std::to_string(day_) + "/";
    if (month_ != -1)
      res += std::to_string(month_) + "/";
    return res + std::to_string(year_);
  }

  int year_;
  int month_;
  int day_;
};

struct Person {

public:
  Person() {}

  Person(const std::string& firstName, const std::string& lastName, Sex sex, struct Date born):
  firstName_(firstName), lastName_(lastName), sex_(sex), born_(born), dead_({}), mother_(nullptr), father_(nullptr), children_({}), id(-1) {};

  Person(const std::string& firstName, const std::string& lastName, Sex sex, struct Date born, struct Date dead):
  firstName_(firstName), lastName_(lastName), sex_(sex), born_(born), dead_(dead), mother_(nullptr), father_(nullptr), children_({}), id(-1) {};

  void info(int space = 1) {
    std::cout << "Person ID " << id << std::endl;
    std::cout << std::string(space, ' ') << (sex_ == Sex::MALE ? "(M) " : "(F) ");
    std::cout << firstName_ << ' ' << lastName_ << std::endl;
    std::cout << std::string(space, ' ') << born_.toString() << " - ";
    if (dead_)
      std::cout << dead_->toString();
    std::cout << std::endl;
  }

  std::string dotId() {
    return "n" + std::to_string(id);
  }

  std::string dot() {
    std::string name = dotId();
    std::string color = sex_ == Sex::MALE ? "lightblue" : "pink";
    std::string label = firstName_ + ' ' + lastName_ + "\\n" + born_.toString() + " - " + (dead_ ? dead_->toString() : "");
    return name + " [shape=box, style=filled, color=" + color + ", label=\"" + label + "\"]";
  }

  std::string dump() {
    return firstName_ + ' ' + lastName_ + ' ' + (sex_ == Sex::MALE ? 'M' : 'F') + ' ' + born_.toString() + (dead_ ? dead_->toString() : "");
  }

  std::string firstName_;
  std::string lastName_;
  Sex sex_;
  struct Date born_;
  std::optional<struct Date> dead_;

  std::shared_ptr<struct Person> mother_;
  std::shared_ptr<struct Person> father_;

  std::vector<std::shared_ptr<struct Person>> children_;

  int id;
};

} // namespace genea