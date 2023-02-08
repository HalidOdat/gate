#include "Editor/Board.hpp"
#include "Application.hpp"

#include <string>

#include "Core/OpenGL.hpp"

namespace Gate {

  Board::Board() {
    mGridFrameBuffer = FrameBuffer::builder()
      .clearColor(1.0f, 1.0f, 1.0f, 1.0f)
      .clear(FrameBuffer::Clear::Color)
      .clearOnBind(true)
      .attach(
        FrameBuffer::Attachment::Type::Texture,
        FrameBuffer::Attachment::Format::Rgba8
      )
      .build();
  }
  Board::~Board() {}

  void Board::onResize(u32 width, u32 height) {
    mGridFrameBuffer->invalidate(width, height);
    mGridTexture = nullptr;
  }

  void Board::renderGrid(Renderer2D& renderer) {
    auto width  = Application::getWindow().getWidth();
    auto height = Application::getWindow().getHeight();

    if (!mGridTexture) {
      renderer.flush();
      // Unbind all textutes so it doesn't cause a write-read FBO feadback.
      // Mabye solve this in a better way...
      for (u32 i = 0; i < 16; i++) {
        glActiveTexture(GL_TEXTURE0 + (GLenum)i);
        glBindTexture(GL_TEXTURE_2D, 0);
      }
      mGridFrameBuffer->bind();
      renderer.clearScreen(config.grid.background);
      for (u32 i = 0; i < width; i += config.grid.cell.size) {
        for (u32 j = 0; j < height; j += config.grid.cell.size) {
          renderer.drawCenteredQuad({i, j}, Vec2{0.08f} * (f32)config.grid.cell.size, config.grid.color);
        }
      }
      renderer.flush();
      mGridFrameBuffer->unbind();
      mGridTexture = mGridFrameBuffer->getColorAttachment(0);
    }

    renderer.clearScreen(mGridTexture);
  }

  void Board::render(Renderer2D& renderer) {
    renderGrid(renderer);
    getCurrentChip().render(renderer);
  }

  void Board::render(Renderer3D& renderer) {
    getCurrentChip().render(renderer);
  }
  
  Chip& Board::getCurrentChip() {
    if (mChips.size() == 0) {
      mChips.emplace_back(String("chip ") + std::to_string(mChips.size()));
    }
    return mChips[mIndex];
  }

}