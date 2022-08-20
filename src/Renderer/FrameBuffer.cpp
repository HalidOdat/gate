#include <glad/glad.h>

#include "Core/Base.hpp"
#include "Renderer/FrameBuffer.hpp"

#include "Resource/Factory.hpp"

namespace Game {

  GAME_FACTORY_IMPLEMENTATION(FrameBuffer, factory)

  FrameBuffer::Handle FrameBuffer::create(u32 width, u32 height, Specification specification) {
    return factory.emplace(width, height, specification);
  }

  FrameBuffer::FrameBuffer(u32 width, u32 height, Specification specification)
    : mId{0}, mSpecification{std::move(specification)}
  {
    invalidate(width, height);
  }

  void FrameBuffer::invalidate(u32 width, u32 height) {
    GAME_DEBUG_ASSERT(width != 0 && height != 0, "");

    if (mId != 0) {
      destroy();
      mId = 0;
    }

    // Create and bind
    GAME_GL_CHECK(glGenFramebuffers(1, &mId));
    GAME_GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, mId));

    // create color attachment texture
    GAME_GL_CHECK(glGenTextures(1, &mColorAttachment));
    GAME_GL_CHECK(glBindTexture(GL_TEXTURE_2D, mColorAttachment));
    GAME_GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GAME_GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    // bind color attachment
    GAME_GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mColorAttachment, 0));

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    GAME_GL_CHECK(glGenRenderbuffers(1, &mDepthStencilAttachment));
    GAME_GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilAttachment));
    GAME_GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
    
    // bind depth and stencil attachment
    GAME_GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthStencilAttachment));

    GAME_ASSERT_WITH_MESSAGE(
      glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
      "framebuffer is not complete!"
    );

    // unbind framebuffer
    GAME_GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
  }

  void FrameBuffer::destroy() {
    GAME_GL_CHECK(glDeleteFramebuffers(1, &mId));
    GAME_GL_CHECK(glDeleteTextures(1, &mColorAttachment));
    GAME_GL_CHECK(glDeleteRenderbuffers(1, &mDepthStencilAttachment));
  }

  FrameBuffer::~FrameBuffer() {
    destroy();
  }

  void FrameBuffer::bind() {
    GAME_GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, mId));
    GAME_GL_CHECK(
      glClearColor(
        mSpecification.clearColor.r,
        mSpecification.clearColor.g,
        mSpecification.clearColor.b,
        mSpecification.clearColor.a
      )
    );
  }

  void FrameBuffer::unbind() {
    GAME_GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
  }

} // namespace Game
