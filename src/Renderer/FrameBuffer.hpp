#pragma once

#include <vector>
#include "Core/Type.hpp"

namespace Game {

  class FrameBuffer {
  public:
    [[nodiscard]] static Ref<FrameBuffer> create(u32 width, u32 height);
    DISALLOW_MOVE_AND_COPY(FrameBuffer);
    ~FrameBuffer();

    void invalidate(u32 width, u32 height);

    void bind();
    void unbind();

    // TODO: return a Texture2D
    inline u32 getColorAttachmentId() const { return mColorAttachment; }

  private:
    FrameBuffer(u32 width, u32 height);
    void destroy();

  private:
    u32 mWidth;
    u32 mHeight;
    u32 mId;
    u32 mColorAttachment;
    u32 mDepthStencilAttachment;
  };

} // namespace Game
