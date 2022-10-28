#include <iostream>
#include "cli/cli.h"

void help(char *argv0) {
  std::cerr << "Usage:" << std::endl;
  std::cerr << '\t' << argv0 << "\t\t\t\t # Starts a new empty tree" << std::endl;
  std::cerr << '\t' << argv0 << " [/path/to/file.genea]\t # Loads an existing tree" << std::endl;
  std::cerr << '\t' << argv0 << " [-h | --help]\t\t # Prints this message" << std::endl;
}


int main(int argc, char **argv) {
  if (argc > 2) {
    help(argv[0]);
    return 1;
  }
  if (argc == 2) {
    std::string arg(argv[1]);
    if (arg == "-h" || arg == "--help") {
      help(argv[0]);
      return 0;
    }
    genea::CLI cli = genea::CLI(arg);
    cli.run();
    return 0;
  }
  genea::CLI cli = genea::CLI("");
  cli.run();
  return 0;
}