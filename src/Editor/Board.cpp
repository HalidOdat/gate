#include "Editor/Board.hpp"
#include "Application.hpp"

#include <string>

#include "Core/OpenGL.hpp"

#include "Editor/Wire.hpp"

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

  void renderGridCenteredQuad(Renderer2D& renderer, Point from, Point to, Vec4 color = Color::BLACK) {
    f32 width = 8;

    Vec2 size = (to.toVec2() - from.toVec2()) * (f32)config.grid.cell.size;
    if (from.x == to.x) {
      size.x  = width;
      size.y += width;
    } else {
      size.x += width;
      size.y  = width;
    }

    renderer.drawQuad(
      from.toVec2() * (f32)config.grid.cell.size - Vec2(width / 2.0f),
      size,
      color
    );
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

      // Draw dotted grid
      for (u32 i = config.grid.cell.size; i < width - config.grid.cell.size; i += config.grid.cell.size) {
        for (u32 j = config.grid.cell.size * 2; j < height - config.grid.cell.size; j += config.grid.cell.size) {
          renderer.drawCenteredQuad({i, j}, Vec2{0.08f} * (f32)config.grid.cell.size, config.grid.color);
        }
      }

      // Draw boarders
      const auto xUnits = width / config.grid.cell.size;
      const auto yUnits = height / config.grid.cell.size;
      renderGridCenteredQuad(renderer, {1, 2}, {xUnits - 1, 2});
      renderGridCenteredQuad(renderer, {1, 2}, {1, yUnits - 1});
      renderGridCenteredQuad(renderer, {xUnits - 1, 2}, {xUnits - 1, yUnits - 1});
      renderGridCenteredQuad(renderer, {1, yUnits - 1}, {xUnits - 1, yUnits - 1});

      renderer.flush();
      mGridFrameBuffer->unbind();
      mGridTexture = mGridFrameBuffer->getColorAttachment(0);
    }

    renderer.clearScreen(mGridTexture);
  }

  Board::MiniMap Board::calculateMiniMapLocationAndSize() {
    auto wWidth  = Application::getWindow().getWidth();
    auto wHeight = Application::getWindow().getHeight();

    u32 w = u32(config.minimap.spacePercent * wWidth);
    u32 h = u32(config.minimap.spacePercent * wHeight);

    w = w < h ? w : h;
    h = w;

    u32 padding = u32(config.minimap.padding * w);

    u32 x;
    u32 y;

    switch (config.minimap.position) {
      case Config::MiniMap::Position::TopRight: {
        x = wWidth - w;
        y = padding;
      } break;
      case Config::MiniMap::Position::BottomRight: {
        x = wWidth - w;
        y = wHeight - h;
      } break;
      case Config::MiniMap::Position::BottomLeft: {
        x = padding;
        y = wHeight - h;
      } break;
      case Config::MiniMap::Position::TopLeft: {
        x = padding;
        y = padding;
      } break;
      case Config::MiniMap::Position::Center: {
        x = wWidth  / 2  - (w - padding) / 2;
        y = wHeight / 2  - (h - padding) / 2;
      } break;
      case Config::MiniMap::Position::None: break;
    }
    w -= padding;
    h -= padding;

    return {x, y, w, h};
  }

  void Board::invalidateMiniMap(Renderer2D& renderer) {
    if (config.minimap.position == Config::MiniMap::Position::None) {
      return;
    }
    mMiniMapFrameBuffer = nullptr;
    mMiniMapTexture = nullptr;
    renderMiniMap(renderer);
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
    for (usize i = 0; i < mChips.size(); ++i) {
      if (mIndex == i) {
        continue;
      }

      config._3dZOffset = f32(i + 1) * config.grid.cell.size3d * 10.0f;
      mChips[i]->render(renderer);
    }

    config._3dZOffset = 0.0f;
    mChips[mIndex]->render(renderer);

    if (config.minimap.position != Config::MiniMap::Position::None) {
      renderMiniMap(Application::getRenderer2D());
    }
  }

  void Board::moveCurrentChipDown() {
    const auto last_element = mChips.size() - 1;
    if (mIndex == 0) {
      mIndex = last_element;
      return;
    }

    mIndex--;
  }
  void Board::moveCurrentChipUp() {
    const auto last_element = mChips.size() - 1;
    if (mIndex == last_element) {
      mIndex = 0;
      return;
    }

    mIndex++;
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

  bool Board::isValidPosition(Point point) {
    const auto wWidth  = Application::getWindow().getWidth();
    const auto wHeight = Application::getWindow().getHeight();
    const auto xUnits = wWidth  / config.grid.cell.size;
    const auto yUnits = wHeight / config.grid.cell.size;
    return point.x >= 1 && point.x <= (xUnits - 1) && point.y >= 2 && point.y <= (yUnits - 1);
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
