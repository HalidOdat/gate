#include <iostream>

#include <Gear/Gear.hpp>
#include <Gear/Core/Assert.hpp>

class ExampleApplication : public Gear::Application {
public:
  ExampleApplication() {}

  virtual void onCreate() override {
    Gear::Logger::info("onCreate was called");
  }

  virtual void onUpdate() override {
    // Gear::Logger::warn("onUpdate");
  }

  virtual void onDestroy() override {
    Gear::Logger::info("onDestroy was called");
  }
};

int main() {
  ExampleApplication().start();
}