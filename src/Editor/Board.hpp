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

    // bool pushComponent(Component* component);
    // WirePushState pushWire(Wire wire);
    // bool click(Point position);
    // void tick();
    
    void pushChip(Chip::Handle chip);

    Chip& getCurrentChip();
    
    std::vector<Chip::Handle>& getChips() { return mChips; }
    const std::vector<Chip::Handle>& getChips() const { return mChips; }

    void onResize(u32 width, u32 height);

  private:
    void renderGrid(Renderer2D& renderer);

  private:
    // u32 currentId = 0;
    // struct Locator {
    //   u32 chipIndex;
    //   u32 componentIndex;
    // };

    u32 mIndex = 0;
    std::vector<Chip::Handle> mChips;

    // std::uordered_map<u32, Locator> mLocator;

    // Grid drawing & caching
    FrameBuffer::Handle mGridFrameBuffer;
    Texture::Handle     mGridTexture;
  };

}

namespace Gate::Serializer {

  template<>
  struct Convert<Board> {
    static Node encode(Board& value);
    static bool decode(const Node& node, Board& value);
  };

}
