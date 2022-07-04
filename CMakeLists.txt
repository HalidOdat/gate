cmake_minimum_required(VERSION 3.0..3.20 FATAL_ERROR)

set(This Game)

project(${This}
  VERSION 0.1.0
  LANGUAGES CXX C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(vendor)
add_subdirectory(Gear)
add_subdirectory(GearBox)
