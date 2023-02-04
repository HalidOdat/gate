#pragma once

#include "Core/Base.hpp"
#include "Editor/Chip.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/Renderer.hpp"

namespace Gate {

  class Board {
  public:
    Board();
    ~Board();

    void render(Renderer& renderer);
    
    Chip& getCurrentChip();

    void onResize(u32 width, u32 height);

  private:
    void renderGrid(Renderer& renderer);

  private:
    u32 mIndex = 0;
    std::vector<Chip> mChips;

    // Grid drawing & caching
    FrameBuffer::Handle mGridFrameBuffer;
    Texture::Handle     mGridTexture;
  };

}
