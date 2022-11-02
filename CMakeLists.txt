cmake_minimum_required(VERSION 3.5)
project(genea CXX)

set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g")
set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} src/main.cc src/cli/cli.cc src/cli/utils.cc)