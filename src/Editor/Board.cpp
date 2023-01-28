#include "Editor/Board.hpp"

#include "Application.hpp"

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

  void Board::renderAll(Renderer& renderer) {
    renderGrid(renderer);
    render(renderer);
  }

  void Board::render(Renderer& renderer) {
    for (auto component : components) {
      if (component) {
        component->render(renderer);
      }
    }
  }

  void Board::renderGrid(Renderer& renderer) {
    auto width  = Application::getWindow().getWidth();
    auto height = Application::getWindow().getHeight();
    for (int i = 0; i < width; i += GRID_SIZE) {
      for (int j = 0; j < height; j += GRID_SIZE) {
        renderer.drawQuad({i, j}, {1.5, 1.5}, Color::BLACK);
      }
    }
  }

}