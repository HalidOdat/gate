#include "Editor/Board.hpp"

#include "Application.hpp"

namespace Game {

  Board::Board() {
    gridFrameBuffer = FrameBuffer::builder()
      .clearColor(1.0f, 1.0f, 1.0f, 1.0f)
      .clear(FrameBuffer::Clear::Color | FrameBuffer::Clear::Depth)
      .clearOnBind(true)
      .attach(
        FrameBuffer::Attachment::Type::Texture2D,
        FrameBuffer::Attachment::Format::Rgb8
      )
      .depthStencilType(FrameBuffer::Attachment::Type::Texture2D)
      .build();
  }

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

  void Board::invalidate(u32 width, u32 height) {
    gridFrameBuffer->invalidate(width, height);
    gridTexture = Texture2D::Handle();
  }

  void Board::renderGrid(Renderer& renderer) {
    auto width  = Application::getWindow().getWidth();
    auto height = Application::getWindow().getHeight();

    if (!gridTexture) {
      renderer.end();
      gridFrameBuffer->bind();
      for (int i = 0; i < width; i += GRID_SIZE) {
        for (int j = 0; j < height; j += GRID_SIZE) {
          renderer.drawQuad({i, j}, {2.0f, 2.0f}, Color::BLACK);
        }
      }
      renderer.flush();
      gridFrameBuffer->unbind();  

      gridTexture = gridFrameBuffer->getColorAttachment(0);
    }

    renderer.drawQuad({0, 0}, {width, height}, gridTexture, Color::WHITE);
  }

}