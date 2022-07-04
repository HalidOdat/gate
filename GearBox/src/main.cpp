#include <iostream>

#include <Gear/Gear.hpp>

class Example : public Gear::Application {
public:
  Example() {}

  virtual void onUpdate() override {

  }
};

int main() {
  std::cout << "Initializing..." << std::endl;
  Gear::Initialize();
  std::cout << "Initialized" << std::endl;

  auto example = Example();
  example.start();

  std::cout << "Terminating..." << std::endl;
  Gear::Terminate();
  std::cout << "Terminated" << std::endl;

}
