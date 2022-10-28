#pragma once


#include "person.h"
#include <string>
#include <fstream>
#include <vector>
#include <memory>

namespace genea {

class Tree {

public:
  Tree();
  Tree(const std::ifstream& in);

  void addPerson(struct Person person);

private:

  std::vector<std::shared_ptr<struct Person>> people_;
};


} //namespace genea