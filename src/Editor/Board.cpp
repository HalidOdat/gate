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

    mMiniMapTexture     = nullptr;
    mMiniMapFrameBuffer = nullptr;
  }

  void Board::renderGrid(Renderer2D& renderer) {
    auto width  = Application::getWindow().getWidth();
    auto height = Application::getWindow().getHeight();

    if (!mGridTexture) {
      renderer.flush();
      // Unbind all textutes so it doesn't cause a write-read FBO feadback.
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

  Board::MiniMap Board::calculateMiniMapLocationAndSize() {
    auto wWidth  = Application::getWindow().getWidth();
    auto wHeight = Application::getWindow().getHeight();

    u32 w = u32(mMiniMapSpacePercent * wWidth);
    u32 h = u32(mMiniMapSpacePercent * wHeight);

    w = w < h ? w : h;
    h = w;

    u32 padding = u32(mMiniMapPadding * w);

    u32 x = wWidth - w; //  + padding;
    u32 y = padding;

    w -= padding;
    h -= padding;

    return {x, y, w, h};
  }

  void Board::renderMiniMap(Renderer2D& renderer) {
    auto[x, y, w, h] = calculateMiniMapLocationAndSize();
    if (!mMiniMapFrameBuffer) {
      mMiniMapFrameBuffer = FrameBuffer::builder()
      .clearColor(0.9f, 0.9f, 0.9f, 1.0f)
      .clear(FrameBuffer::Clear::Color)
      .clearOnBind(true)
      .attach(
        FrameBuffer::Attachment::Type::Texture,
        FrameBuffer::Attachment::Format::Rgba8
      )
      .build();
    }

    if (!mMiniMapTexture) {
      renderer.flush();
      // Unbind all textutes so it doesn't cause a write-read FBO feadback.
      for (u32 i = 0; i < 16; i++) {
        glActiveTexture(GL_TEXTURE0 + (GLenum)i);
        glBindTexture(GL_TEXTURE_2D, 0);
      }
      mMiniMapFrameBuffer->bind();

      auto oldSize = config.grid.cell.size;
      config.grid.cell.size = getCurrentChip().getOptimalCellSize();
      getCurrentChip().render(renderer);
      config.grid.cell.size = oldSize;

      renderer.flush();
      mMiniMapFrameBuffer->unbind();
      mMiniMapTexture = mMiniMapFrameBuffer->getColorAttachment(0);
    }

    renderer.drawQuad(Vec2{x, y}, Vec2{w, h}, mMiniMapTexture, Color::WHITE, Effect::Type::Rounded);
  }

  void Board::render(Renderer2D& renderer) {
    renderGrid(renderer);
    getCurrentChip().render(renderer);

    const auto size = 16;
    String text = "Name: " + getCurrentChip().getName();
    renderer.drawText(text, Vec2{size}, (f32)size, config.text.color);
  }

  void Board::render(Renderer3D& renderer) {
    getCurrentChip().render(renderer);
    renderMiniMap(Application::getRenderer2D());
  }
  
  Chip& Board::getCurrentChip() {
    if (mChips.size() == 0) {
      mChips.emplace_back(
        Chip::create(String("chip ") + std::to_string(mChips.size()))
      );
    }
    return *mChips[mIndex];
  }

  void Board::pushChip(Chip::Handle chip) {
    mIndex = mChips.size();
    mChips.push_back(std::move(chip));
  }

  void Board::tick() {
    getCurrentChip().tick();
    mMiniMapTexture = nullptr;
  }
  bool Board::pushComponent(Component* component) {
    mMiniMapTexture = nullptr;
    return getCurrentChip().pushComponent(component);
  }
  void Board::removeComponent(Point position) {
    mMiniMapTexture = nullptr;
    return getCurrentChip().removeComponent(position);
  }
  void Board::removeWire(Point position) {
    mMiniMapTexture = nullptr;
    return getCurrentChip().removeWire(position);
  }
  WirePushState Board::pushWire(Wire wire) {
    mMiniMapTexture = nullptr;
    return getCurrentChip().pushWire(wire);
  }
  bool Board::click(Point position) {
    mMiniMapTexture = nullptr;
    return getCurrentChip().click(position);
  }
  bool Board::click(u32 id) {
    mMiniMapTexture = nullptr;
    return getCurrentChip().click(id);
  }

}

namespace Gate::Serializer {

  Node Convert<Board>::encode(Board& value) {
    auto node = Node::object();
    auto chips = Node::array();
    for (auto& chip : value.getChips()) {
      chips.push(Convert<Chip>::encode(*chip));
    }
    node["chips"] = chips;
    return node;
  }
  bool Convert<Board>::decode(const Node& node, Board& value) {
    if (!node.isObject()) return false;

    auto* chipsNode  = node.get("chips");
    if (!chipsNode || !chipsNode->isArray()) return false;
    
    auto& chipsArray = *chipsNode->asArray();
    for (auto& chip : chipsArray) {
      Chip::Handle chipValue = Chip::create();
      if (!Convert<Chip>::decode(chip, *chipValue)) return false;
      value.pushChip(std::move(chipValue));
    }
    
    return true;
  }

}
