#include "Editor/Board.hpp"
#include "Application.hpp"

#include <string>

namespace Gate {

  Board::Board() {
    mGridFrameBuffer = FrameBuffer::builder()
      .clearColor(1.0f, 1.0f, 1.0f, 1.0f)
      .clear(FrameBuffer::Clear::Color | FrameBuffer::Clear::Depth)
      .clearOnBind(true)
      .attach(
        FrameBuffer::Attachment::Type::Texture,
        FrameBuffer::Attachment::Format::Rgb8
      )
      .build();
  }
  Board::~Board() {}

  void Board::onResize(u32 width, u32 height) {
    mGridFrameBuffer->invalidate(width, height);
    mGridTexture = nullptr;
  }

  void Board::renderGrid(Renderer& renderer) {
    auto width  = Application::getWindow().getWidth();
    auto height = Application::getWindow().getHeight();

    if (!mGridTexture) {
      renderer.end();
      mGridFrameBuffer->bind();
      renderer.clearScreen(config.grid.background);
      for (u32 i = 0; i < width; i += config.grid.cell.size) {
        for (u32 j = 0; j < height; j += config.grid.cell.size) {
          renderer.drawCenteredQuad({i, j}, {2.0f, 2.0f}, config.grid.color);
        }
      }
      renderer.flush();
      mGridFrameBuffer->unbind();  

      mGridTexture = mGridFrameBuffer->getColorAttachment(0);
    }

    renderer.clearScreen(mGridTexture);
  }

  void Board::render(Renderer& renderer) {
    renderGrid(renderer);
    getCurrentChip().render(renderer);
  }
  
  Chip& Board::getCurrentChip() {
    if (mChips.size() == 0) {
      mChips.emplace_back(String("chip ") + std::to_string(mChips.size()));
    }
    return mChips[mIndex];
  }

}