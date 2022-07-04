#include <iostream>

#include <Gear/Window.hpp>

int main() {
  std::cout << "Initializing..." << std::endl;
  Gear::Initialize();
  std::cout << "Initialized" << std::endl;
  
  std::cout << "Creating a window!" << std::endl;
  auto window = Gear::Window("Hello", 500, 500);

  while (!window.shouldClose()) {
    window.pollEvents();
  }

  std::cout << "Terminating..." << std::endl;
  Gear::Terminate();
  std::cout << "Terminated" << std::endl;
}
