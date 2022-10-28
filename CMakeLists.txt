cmake_minimum_required(VERSION 3.5)
project(genea CXX)

set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

add_executable(${PROJECT_NAME} src/main.cc)