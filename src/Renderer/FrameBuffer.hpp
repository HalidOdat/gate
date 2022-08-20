#pragma once

#include <vector>
#include "Core/Type.hpp"
#include "Core/Math.hpp"
#include "Resource/Texture.hpp"
#include "Resource/Resource.hpp"

namespace Game {

  class FrameBuffer {
  public:
    using Handle = Resource<FrameBuffer>;

    struct Specification {
      Specification() {}
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

  private:
    template<typename T>
    friend class ResourceFactory;
  };

  GAME_FACTORY_HEADER(FrameBuffer)

} // namespace Game
