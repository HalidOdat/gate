#include <iostream>

#include <Gear/Window.hpp>

int main() {
  std::cout << "Creating a window!" << std::endl;

  auto window = Gear::Window("Hello", 500, 500);
}
