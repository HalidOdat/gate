cmake_minimum_required(VERSION 3.8.0 FATAL_ERROR)

set(This Game)

project(${This}
  VERSION 0.1.0
  LANGUAGES CXX C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Headers
    # include/include.hpp
)

set(Sources
    src/main.cpp
)

add_executable(${This} ${Sources} ${Headers})
