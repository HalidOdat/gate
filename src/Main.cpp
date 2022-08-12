#include "Application.hpp"

#include "Core/Base.hpp"

int main(int argc, char* argv[]) {
  GAME_PROFILE_BEGIN_SESSION("game");
  Game::Application("Game", 1340, 640).start();
  GAME_PROFILE_END_SESSION();
}