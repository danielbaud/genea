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
      return "???";
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
  Person(const std::string& firstName, const std::string& lastName, Sex sex, struct Date born):
  firstName_(firstName), lastName_(lastName), sex_(sex), born_(born), dead_({}), mother_(nullptr), father_(nullptr), children_({}) {};

  Person(const std::string& firstName, const std::string& lastName, Sex sex, struct Date born, struct Date dead):
  firstName_(firstName), lastName_(lastName), sex_(sex), born_(born), dead_(dead), mother_(nullptr), father_(nullptr), children_({}) {};

  void info() {
    std::cout << (sex_ == Sex::MALE ? "Mr " : "Ms ");
    std::cout << firstName_ << ' ' << lastName_ << ", born on " << born_.toString();
    if (dead_)
      std::cout << "and dead on " << dead_->toString();
    std::cout << std::endl;
  }

  std::string firstName_;
  std::string lastName_;
  Sex sex_;
  struct Date born_;
  std::optional<struct Date> dead_;

  std::shared_ptr<struct Person> mother_;
  std::shared_ptr<struct Person> father_;

  std::vector<std::shared_ptr<struct Person>> children_;
};


} // namespace genea