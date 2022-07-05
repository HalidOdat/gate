#include <iostream>

#include <Gear/Gear.hpp>

class Example : public Gear::Application {
public:
  Example() {}

  virtual void onCreate() override {
    Gear::Logger::info("onCreate was called");
  }

  virtual void onUpdate() override {
    Gear::Logger::warn("onUpdate");
  }

  virtual void onDestroy() override {
    Gear::Logger::info("onDestroy was called");
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
