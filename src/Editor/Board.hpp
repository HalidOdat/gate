#pragma once

#include "Core/Base.hpp"
#include "Editor/Chip.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Serializer/Serializer.hpp"

namespace Gate {

  class Board {
  public:
    Board();
    ~Board();

    void render(Renderer2D& renderer);
    void render(Renderer3D& renderer);
    
    void pushChip(Chip::Handle chip);
    void pushNewChip();

    void moveCurrentChipDown();
    void moveCurrentChipUp();

    Chip& getCurrentChip();
    Chip::Handle getCurrentChipPtr() { return mChips[mIndex]; }
    
    std::vector<Chip::Handle>& getChips() { return mChips; }
    const std::vector<Chip::Handle>& getChips() const { return mChips; }
    u32 getChipsCount() const { return mChips.size(); }

    void onResize(u32 width, u32 height);

    void tick();
    bool pushComponent(Component* component);
    void removeComponent(Point position);
    void removeWire(Point position);
    WirePushState pushWire(Wire wire);
    bool click(Point position);
    bool click(u32 id);

    void cycleMiniMapPositions();
    void invalidateMiniMap(Renderer2D& renderer);

    bool isValidPosition(Point point);

  private:
    struct MiniMap {
      u32 x;
      u32 y;
      u32 w;
      u32 h;
    };
    void renderGrid(Renderer2D& renderer);
    void renderMiniMap(Renderer2D& renderer);
    void createMiniMapFrameBuffer();
    MiniMap calculateMiniMapLocationAndSize();

  private:
    u32 mIndex = 0;
    std::vector<Chip::Handle> mChips;

    // Grid drawing & caching
    FrameBuffer::Handle mGridFrameBuffer;
    Texture::Handle     mGridTexture;

    // Drawing minimap & caching
    Texture::Handle mMiniMapTexture = nullptr;
    FrameBuffer::Handle mMiniMapFrameBuffer = nullptr;
  };

}

namespace Gate::Serializer {

  template<>
  struct Convert<Board> {
    static Node encode(Board& value);
    static bool decode(const Node& node, Board& value);
  };

}
