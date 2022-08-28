#include <glad/glad.h>

#include "Core/Base.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Resource/Factory.hpp"
#include "Application.hpp"

namespace Game {

  namespace {
    constexpr static GLenum clearToOpenGLType(FrameBuffer::Clear clear) {
      GLenum openGLClear = 0;
      if (static_cast<u8>(clear) & static_cast<u8>(FrameBuffer::Clear::Color)) {
        openGLClear |= GL_COLOR_BUFFER_BIT;
      }
      if (static_cast<u8>(clear) & static_cast<u8>(FrameBuffer::Clear::Depth)) {
        openGLClear |= GL_DEPTH_BUFFER_BIT;
      }
      if (static_cast<u8>(clear) & static_cast<u8>(FrameBuffer::Clear::Stencil)) {
        openGLClear |= GL_STENCIL_BUFFER_BIT;
      }

      return openGLClear;
    }
  }

  GAME_FACTORY_IMPLEMENTATION(FrameBuffer, factory)

  FrameBuffer::Builder& FrameBuffer::Builder::width(u32 width) {
    mWidth = width;
    return *this;
  }

  FrameBuffer::Builder& FrameBuffer::Builder::height(u32 height) {
    mHeight = height;
    return *this;
  }

  FrameBuffer::Builder& FrameBuffer::Builder::clearColor(f32 r, f32 g, f32 b, f32 a) {
    mClearColor = Vec4(r, g, b, a);
    return *this;
  }

  FrameBuffer::Builder& FrameBuffer::Builder::clearColor(const Vec3& color) {
    mClearColor = Vec4(color, 1.0f);
    return *this;
  }

  FrameBuffer::Builder& FrameBuffer::Builder::clearColor(const Vec4& color) {
    mClearColor = color;
    return *this;
  }

  FrameBuffer::Builder& FrameBuffer::Builder::clear(FrameBuffer::Clear clear) {
    mClear = clear;
    return *this;
  }

  FrameBuffer::Builder& FrameBuffer::Builder::clearOnBind(bool yes) {
    mClearOnBind = yes;
    return *this;
  }

  FrameBuffer::Builder& FrameBuffer::Builder::attach(Attachment::Type type, Attachment::Format format, bool isMultisample) {
    mAttachments.push_back(FrameBuffer::Attachment{type, format, isMultisample});
    return *this;
  }

  FrameBuffer::Builder& FrameBuffer::Builder::attachDefaultDepthStencilBuffer() {
    mDepthStencilAttachment = {
      Attachment::Type::RenderBuffer,
      Attachment::Format::DefaultDepthStencil,
      true
    };
    return *this;
  }

  FrameBuffer::Handle FrameBuffer::Builder::build() {
    if (mWidth == 0) {
      mWidth = Application::getWindow().getWidth();
    }
    if (mHeight == 0) {
      mHeight = Application::getWindow().getHeight();
    }

    return FrameBuffer::create(*this);
  }

  FrameBuffer::Builder FrameBuffer::builder() {
    return FrameBuffer::Builder();
  }

  FrameBuffer::Handle FrameBuffer::create(Builder& builder) {
    return factory.emplace(builder);
  }

  FrameBuffer::FrameBuffer(Builder& builder)
    : mId{0}
  {
    mWidth  = builder.mWidth;
    mHeight = builder.mHeight;

    mClearOnBind = builder.mClearOnBind;
    mClear       = builder.mClear;
    mClearColor  = builder.mClearColor;
    mAttachmentsSpecification = std::move(builder.mAttachments);

    invalidate(mWidth, mHeight);
  }

  void FrameBuffer::invalidate(u32 width, u32 height) {
    GAME_DEBUG_ASSERT(width != 0 && height != 0);

    mWidth  = width;
    mHeight = height;

    if (mId != 0) {
      destroy();
      mId = 0;
    }

    // Create and bind
    glGenFramebuffers(1, &mId);
    glBindFramebuffer(GL_FRAMEBUFFER, mId);

    for (u32 i = 0; i < mAttachmentsSpecification.size(); ++i) {
      Attachment& attachment = mAttachmentsSpecification[i];

      // TODO: implement other attachment types
      GAME_ASSERT(attachment.type == Attachment::Type::Texture2D);

      // TODO: implement multisampled attachment types
      GAME_ASSERT(!attachment.isMultisample);

      // create color attachment texture
      auto texture = Texture2D::buffer(width, height)
        .format(attachment.format)
        .filtering(Texture::FilteringMode::Linear)
        .gammaCorrected(false)
        .build();

      // bind color attachment
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->getId(), 0);

      mColorAttachments.emplace_back(std::move(texture));
    }

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &mDepthStencilAttachment);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    // bind depth and stencil attachment
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mDepthStencilAttachment);

    GAME_ASSERT_WITH_MESSAGE(
      glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
      "framebuffer is not complete!"
    );

    // unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void FrameBuffer::destroy() {
    glDeleteFramebuffers(1, &mId);
    mColorAttachments.clear();
    glDeleteRenderbuffers(1, &mDepthStencilAttachment);
  }

  FrameBuffer::~FrameBuffer() {
    destroy();
  }

  void FrameBuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, mId);
    glClearColor(mClearColor.r, mClearColor.g, mClearColor.b, mClearColor.a);

    if (mClearOnBind) {
      this->clear();
    }
  }

  void FrameBuffer::clear() {
    glClear(clearToOpenGLType(mClear));
  }

  void FrameBuffer::clear(FrameBuffer::Clear clear) {
    glClear(clearToOpenGLType(clear));
  }

  void FrameBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

} // namespace Game
