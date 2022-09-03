#include <array>
#include <cctype>
#include <algorithm>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Base.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Resource/Manager.hpp"
#include "Application.hpp"


namespace Game {

  // TODO: Use uniform buffer for camera and static data

  Renderer3D::PostProcessing::PostProcessing() {
    glGenFramebuffers(1, &mId);
    glBindFramebuffer(GL_FRAMEBUFFER, mId);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  Renderer3D::PostProcessing::~PostProcessing() {
    glDeleteFramebuffers(1, &mId);
  }

  void Renderer3D::PostProcessing::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, mId);
  }

  void Renderer3D::PostProcessing::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void Renderer3D::PostProcessing::setTexture(Texture2D::Handle& dest) {
    if (dest->getSpecification().samples == 0) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dest->getId(), 0);
    } else {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, dest->getId(), 0);
    }
  }


  void Renderer3D::PostProcessing::clear() {
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void Renderer3D::enableBlending(bool yes) {
    if (yes) {
      glEnable(GL_BLEND);
    } else {
      glDisable(GL_BLEND);
    }
  }

  void Renderer3D::enableDepthTest(bool yes) {
    if (yes) {
      glEnable(GL_DEPTH_TEST);
    } else {
      glDisable(GL_DEPTH_TEST);
    }
  }

  void Renderer3D::enableCullFace(bool yes) {
    if (yes) {
      glEnable(GL_CULL_FACE);
    } else {
      glDisable(GL_CULL_FACE);
    }
  }

  Renderer3D::Renderer3D() {
    GAME_PROFILE_FUNCTION();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mPipeline.postProcesingShader = Shader::load("assets/shaders/PostProcessing.glsl").build();

    mPipeline.frameBuffer = FrameBuffer::builder()
      .clearColor(1.0f, 1.0f, 1.0f, 1.0f)
      .clear(FrameBuffer::Clear::Color | FrameBuffer::Clear::Depth)
      .clearOnBind(true)
      .attach(
        FrameBuffer::Attachment::Type::Texture2D,
        FrameBuffer::Attachment::Format::Rgba16F
      )
      .attachDefaultDepthStencilBuffer()
      .build();

    static const float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
      // positions   // texCoords
      -1.0f,  1.0f,  0.0f, 1.0f,
      -1.0f, -1.0f,  0.0f, 0.0f,
       1.0f, -1.0f,  1.0f, 0.0f,

      -1.0f,  1.0f,  0.0f, 1.0f,
       1.0f, -1.0f,  1.0f, 0.0f,
       1.0f,  1.0f,  1.0f, 1.0f
    };

    auto quadVertexArray = VertexArray::create();
    auto quadVertexBuffer = VertexBuffer::builder()
      .data(quadVertices)
      .layout(BufferElement::Type::Float2, "position")
      .layout(BufferElement::Type::Float2, "texture")
      .build();

    quadVertexArray->addVertexBuffer(quadVertexBuffer);
    quadVertexArray->unbind();
    mPipeline.quadVertexArray = quadVertexArray;

    mPipeline.skyboxTexture = CubeMap::load({
      "assets/textures/skybox/right.jpg",
      "assets/textures/skybox/left.jpg",
      "assets/textures/skybox/top.jpg",
      "assets/textures/skybox/bottom.jpg",
      "assets/textures/skybox/front.jpg",
      "assets/textures/skybox/back.jpg",
    });

    static const f32 skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    auto skyboxVertexArray = VertexArray::create();
    auto skyboxVertexBuffer = VertexBuffer::builder()
      .data(skyboxVertices)
      .layout(BufferElement::Type::Float3, "position")
      .build();
    skyboxVertexArray->addVertexBuffer(skyboxVertexBuffer);
    skyboxVertexArray->unbind();
    mPipeline.skyboxVertexArray = skyboxVertexArray;

    mPipeline.skyboxShader = Shader::load("assets/shaders/Skybox.glsl").build();
    mPipeline.shader = Shader::load("assets/shaders/SpotLight.glsl").build();

    mEnvironment.defaultDiffuseMap  = Texture2D::color(0xFF'FF'FF'FF).build();
    mEnvironment.defaultSpecularMap = Texture2D::color(0x00'00'00'FF).build();
    mEnvironment.defaultEmissionMap = Texture2D::color(128, 128, 128).build();

    // Bloom
    mPipeline.bloomPreFilterShader = Shader::load("assets/shaders/Bloom/PreFilter.glsl").build();
    mPipeline.bloomDownsampleShader = Shader::load("assets/shaders/Bloom/Downsample.glsl").build();
    mPipeline.bloomUpsampleShader = Shader::load("assets/shaders/Bloom/Upsample.glsl").build();
    mPipeline.bloomFinalShader = Shader::load("assets/shaders/Bloom/Final.glsl").build();

    for (u32 i = 0; i < mPipeline.mipmaps.size(); ++i) {
      u32 width;
      u32 height;
      if (i == 0) {
        width = Application::getWindow().getWidth();
        height = Application::getWindow().getHeight();
      } else {
        width  = mPipeline.mipmaps[i - 1]->getWidth() / 2;
        height = mPipeline.mipmaps[i - 1]->getHeight() / 2;
      }
      mPipeline.mipmaps[i] = Texture2D::buffer(width, height)
        .format(Texture::Format::R11FG11FB10F)
        .mipmap(Texture::MipmapMode::None)
        .gammaCorrected(false)
        .filtering(Texture::FilteringMode::Linear)
        .wrapping(Texture::WrappingMode::ClampToEdge)
        // .samples(4)
        .build();
    }
  }

  Renderer3D::~Renderer3D() {
    GAME_PROFILE_FUNCTION();
  }

  void Renderer3D::invalidate(u32 width, u32 height) {
    GAME_PROFILE_FUNCTION();

    mPipeline.frameBuffer->invalidate(width, height);
  }
  
  void Renderer3D::begin3D(const PerspectiveCameraController& cameraController) {
    GAME_PROFILE_FUNCTION();

    mPipeline.camera.projection = cameraController.getCamera().getProjectionMatrix();
    mPipeline.camera.view       = cameraController.getCamera().getViewMatrix();
    mPipeline.camera.position   = cameraController.getPosition();
    mPipeline.camera.front      = cameraController.getFront();
  }

  void Renderer3D::submit(const Mesh::Handle& mesh, const Material::Handle& material, const Mat4& transform) {
    GAME_PROFILE_FUNCTION();

    // Don't render fully transparent objects
    if (material->transparency == 0.0f) {
      return;
    }

    // TODO: Sort by shader
    u32 index = (u32)mPipeline.units.size();
    RenderUnit unit {
      mesh,
      *material, // Copy material
      transform,
      Mat3(glm::transpose(glm::inverse(transform))),
    };

    if (!unit.material.diffuseMap.isValid())  unit.material.diffuseMap  = mEnvironment.defaultDiffuseMap;
    if (!unit.material.specularMap.isValid()) unit.material.specularMap = mEnvironment.defaultSpecularMap;
    if (!unit.material.emissionMap.isValid()) unit.material.emissionMap = mEnvironment.defaultEmissionMap;

    mPipeline.units.push_back(unit);
    if (!material->transparency) {
      mPipeline.opaqueUnitIndices.emplace_back(index);
    } else {
      Vec3 position = {transform[3][0], transform[3][1], transform[3][2]};
      f32 distance = glm::length(position - mPipeline.camera.position);
      mPipeline.transparentUnitIndices.push_back(std::pair(distance, index));
    }
  }

  void Renderer3D::renderUnit(u32 unitIndex) {
    GAME_PROFILE_FUNCTION();

    RenderUnit& unit = mPipeline.units[unitIndex];

    mPipeline.shader->setMat4("uModelMatrix", unit.modelMatrix);
    mPipeline.shader->setMat3("uNormalMatrix", unit.normalMatrix);

    // TODO: Make this more dynamic
    unit.material.diffuseMap->bind(0);
    unit.material.specularMap->bind(1);
    unit.material.emissionMap->bind(2);

    mPipeline.shader->setInt("uMaterial.diffuse",  0);
    mPipeline.shader->setInt("uMaterial.specular", 1);
    mPipeline.shader->setInt("uMaterial.emission", 2);

    mPipeline.shader->setFloat("uMaterial.shininess", unit.material.shininess);
    mPipeline.shader->setFloat("uMaterial.transparency", unit.material.transparency);

    auto vao = unit.mesh->getVertexArray();
    vao->bind();
    vao->drawIndices();
    vao->unbind();
  }

  void Renderer3D::renderAllUnits() {
    GAME_PROFILE_FUNCTION();

    mPipeline.shader->bind();
    mPipeline.shader->setMat4("uProjectionMatrix", mPipeline.camera.projection);
    mPipeline.shader->setMat4("uViewMatrix", mPipeline.camera.view);

    mPipeline.shader->setVec3("uViewPosition", mPipeline.camera.position);
    mPipeline.shader->setVec3("uLight.position", mPipeline.camera.position);
    mPipeline.shader->setVec3("uLight.direction", mPipeline.camera.front);
    mPipeline.shader->setFloat("uLight.cutOff", glm::cos(glm::radians(12.5f)));
    mPipeline.shader->setFloat("uLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    mPipeline.shader->setVec3("uLight.ambient",  Vec3(0.3f, 0.3f, 0.3f));
    mPipeline.shader->setVec3("uLight.diffuse",  Vec3(1.0f, 1.0f, 1.0f));
    mPipeline.shader->setVec3("uLight.specular", Vec3(1.0f, 1.0f, 1.0f));
    mPipeline.shader->setFloat("uLight.constant",  1.0f);
    mPipeline.shader->setFloat("uLight.linear",    0.09f);
    mPipeline.shader->setFloat("uLight.quadratic", 0.032f);

    for (auto& index : mPipeline.opaqueUnitIndices) {
      renderUnit(index);
    }

    std::sort(
      mPipeline.transparentUnitIndices.begin(),
      mPipeline.transparentUnitIndices.end(),
      [](auto& a, auto& b) {
        return a.first > b.first;
      }
    );

    for (auto& unitIndex : mPipeline.transparentUnitIndices) {
      renderUnit(unitIndex.second);
    }

    mPipeline.units.clear();
    mPipeline.opaqueUnitIndices.clear();
    mPipeline.transparentUnitIndices.clear();
  }

  void Renderer3D::renderSkybox() {
    GAME_PROFILE_FUNCTION();

    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    mPipeline.skyboxTexture->bind();
    mPipeline.skyboxShader->bind();
    mPipeline.skyboxShader->setMat4("uView", Mat4(Mat3(mPipeline.camera.view)));
    mPipeline.skyboxShader->setMat4("uProjection", mPipeline.camera.projection);
    mPipeline.skyboxShader->setInt("uSkybox", 0);
    mPipeline.skyboxVertexArray->bind();
    mPipeline.skyboxVertexArray->drawArrays(36);
    glDepthFunc(GL_LESS); // set depth function back to default
  }

  void Renderer3D::waitAndRender() {
    GAME_PROFILE_FUNCTION();

    // First Pass
    mPipeline.frameBuffer->bind();
    Renderer3D::enableDepthTest(true);
    renderAllUnits();
    renderSkybox();
    Renderer3D::enableDepthTest(false);
    mPipeline.frameBuffer->unbind();

    //  glDisable(GL_DEPTH_TEST);

    // Bloom Effect
    mPipeline.quadVertexArray->bind();

    auto texture = mPipeline.frameBuffer->getColorAttachment();
    texture->bind(0);

    mPipeline.bloomFbo.bind();

      // Pre-Filter Bloom stage
      mPipeline.bloomPreFilterShader->bind();
      mPipeline.bloomPreFilterShader->setInt("uScreenTexture", 0);

      mPipeline.bloomFbo.setTexture(mPipeline.mipmaps[0]);
      mPipeline.quadVertexArray->drawArrays(6);

      // Downsample
      mPipeline.bloomDownsampleShader->bind();
      mPipeline.bloomDownsampleShader->setInt("uScreenTexture", 0);

      for (u32 i = 0; i < mPipeline.mipmaps.size() - 1; ++i) {
        glViewport(0, 0, mPipeline.mipmaps[i + 1]->getWidth(), mPipeline.mipmaps[i + 1]->getHeight());
        mPipeline.bloomDownsampleShader->setVec2("uResolution", Vec2(mPipeline.mipmaps[i]->getWidth(), mPipeline.mipmaps[i]->getHeight()));
        mPipeline.bloomFbo.setTexture(mPipeline.mipmaps[i + 1]);
        mPipeline.mipmaps[i]->bind();
        mPipeline.quadVertexArray->drawArrays(6);  
      }

      // Upsample
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);
      glBlendEquation(GL_FUNC_ADD);

      mPipeline.bloomUpsampleShader->bind();
      mPipeline.bloomUpsampleShader->setInt("uTexture", 0);
      mPipeline.bloomUpsampleShader->setFloat("uFilterRadius", 0.005f);

      for (u32 i = mPipeline.mipmaps.size() - 1; i > 0; --i) {
        glViewport(0, 0, mPipeline.mipmaps[i - 1]->getWidth(), mPipeline.mipmaps[i - 1]->getHeight());
        mPipeline.bloomFbo.setTexture(mPipeline.mipmaps[i - 1]);
        mPipeline.mipmaps[i]->bind();
        mPipeline.quadVertexArray->drawArrays(6);
      }

      mPipeline.bloomFinalShader->bind();
      mPipeline.bloomFinalShader->setInt("uTexture", 0);
      texture->bind();

      mPipeline.bloomFbo.setTexture(mPipeline.mipmaps[0]);
      mPipeline.quadVertexArray->drawArrays(6);

      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);


    mPipeline.bloomFbo.unbind();

    // Last pass
    #ifndef GAME_PLATFORM_WEB
      glEnable(GL_FRAMEBUFFER_SRGB);
    #endif

    glViewport(0, 0, mPipeline.mipmaps[0]->getWidth(), mPipeline.mipmaps[0]->getHeight());

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mPipeline.mipmaps[0]->bind(0);

    mPipeline.postProcesingShader->bind();
    mPipeline.postProcesingShader->setInt("uScreenTexture", 0);
    mPipeline.quadVertexArray->drawArrays(6);


    #ifndef GAME_PLATFORM_WEB
      glDisable(GL_FRAMEBUFFER_SRGB);
    #endif
  }

  void Renderer3D::end() {
  }

} // namespace Game
