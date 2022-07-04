cmake_minimum_required(VERSION 3.0..3.20 FATAL_ERROR)

set(This GearBox)

set(Headers
    # include/.hpp
)

set(Sources
    src/main.cpp
)

add_executable(${This} ${Sources} ${Headers})

target_link_libraries(${This} PUBLIC
  Gear
)
