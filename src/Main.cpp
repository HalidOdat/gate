#include "Application.hpp"

#include "Core/Base.hpp"

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
  
  Game::Application("Game", 1340, 640).start();
}
