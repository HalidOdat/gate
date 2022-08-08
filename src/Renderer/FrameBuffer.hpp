#pragma once

#include <vector>
#include "Core/Type.hpp"
#include "Core/Math.hpp"
#include "Resource/Texture.hpp"

namespace Game {

  class FrameBuffer {
  public:
    using Handle = Ref<FrameBuffer>;

    struct Specification {
      Vec4 clearColor{1.0f};
    };

  public:
    [[nodiscard]] static FrameBuffer::Handle create(u32 width, u32 height, Specification specification = {});
    DISALLOW_MOVE_AND_COPY(FrameBuffer);
    ~FrameBuffer();

    void invalidate(u32 width, u32 height);

    void bind();
    void unbind();

    // TODO: return a Texture2D
    inline u32 getColorAttachmentId() const { return mColorAttachment; }

  private:
    FrameBuffer(u32 width, u32 height, Specification specification);
    void destroy();

  private:
    u32 mWidth;
    u32 mHeight;
    u32 mId;
    u32 mColorAttachment;
    u32 mDepthStencilAttachment;

    Specification mSpecification;
  };

} // namespace Game
