#include "Editor/Board.hpp"

namespace Game {

  Board::~Board() {
    for (auto component : components) {
      if (component) {
        delete component;
      }
    }
  }

  void Board::push_line(const Line& line) {
    // TODO: Check if lines intersect
    // TODO: assert from is smaller than to
    lines.push_back(line);
  }

  u32 Board::push_component(Component* component) {
    u32 index = (u32)components.size();
    components.push_back(component);
    return index;
  }

  void Board::render(Renderer& renderer) {
    for (auto component : components) {
      if (component) {
        component->render(renderer);
      }
    }
  }

}