#include <cctype>
#include <array>
#include <algorithm>
#include <stdint.h>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Base.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Resource/Manager.hpp"
#include "Application.hpp"


namespace std {

  template<>
  struct std::hash<Game::Mesh::Handle> {
    std::size_t operator()(Game::Mesh::Handle const& handle) const noexcept {
      return std::hash<Game::u32>{}(handle.getId());
    }
  };

}

namespace Game {

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
      #if GAME_EDITOR
        .clear(FrameBuffer::Clear::Color | FrameBuffer::Clear::Depth | FrameBuffer::Clear::Stencil)
      #else
        .clear(FrameBuffer::Clear::Color | FrameBuffer::Clear::Depth)
      #endif
      .clearOnBind(true)
      .attach(
        FrameBuffer::Attachment::Type::Texture2D,
        FrameBuffer::Attachment::Format::Rgba16F
      )
      #if GAME_EDITOR
        .attach(
          FrameBuffer::Attachment::Type::Texture2D,
          FrameBuffer::Attachment::Format::R32UI
        )
      #endif
      .depthStencilType(FrameBuffer::Attachment::Type::Texture2D)
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
    mPipeline.shader = Shader::load("assets/shaders/SpotLight.glsl")
      #if GAME_EDITOR
        .define("EDITOR", "1")
      #endif
      .build();
    mPipeline.shader->bind();
    i32 samples[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    mPipeline.shader->setIntArray("uTextures", samples, 16);

    mEnvironment.defaultDiffuseMap  = Texture2D::color(0xFF'FF'FF'FF).build();
    mEnvironment.defaultSpecularMap = Texture2D::color(0x00'00'00'FF).build();
    mEnvironment.defaultEmissionMap = Texture2D::color(128, 128, 128).build();

    #if 0
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
    #endif
    
    #if GAME_EDITOR
      mOutlineShader = Shader::load("assets/shaders/Outline.glsl")
        .define("EDITOR", "1")
        .build();
    #endif

    mPipeline.instancedBuffer = VertexBuffer::builder()
      .storage(Buffer::StorageType::Dynamic)
      .size(INSTANCE_BUFFER_SIZE)
      .layout(BufferElement::Type::Mat4, "aModelMatrix", 1)
      .layout(BufferElement::Type::Mat3, "aNormalMatrix", 1)
      #if GAME_EDITOR
        .layout(BufferElement::Type::Int, "aEntityId", 1)
      #endif
      .build();

    mPipeline.instancedBasePtr = new Pipeline::Instance[INSTANCE_COUNT];
    mPipeline.instancedCurrentPtr = mPipeline.instancedBasePtr;

    mCameraUniformBuffer = UniformBuffer::builder(0)
      .size(sizeof(RenderCamera))
      .storage(UniformBuffer::StorageType::Dynamic)
      .build();

    mPipeline.materialsUniformBuffer = UniformBuffer::builder(2)
      .size(sizeof(MaterialInternal) * MAX_MATERIALS)
      .storage(UniformBuffer::StorageType::Dynamic)
      .build();
    mPipeline.materialsBasePtr = new MaterialInternal[MAX_MATERIALS];
    mPipeline.materialsCurrentPtr = mPipeline.materialsBasePtr;
  }

  Renderer3D::~Renderer3D() {
    GAME_PROFILE_FUNCTION();

    delete mPipeline.instancedBasePtr;
    delete mPipeline.materialsBasePtr;
  }

  void Renderer3D::invalidate(u32 width, u32 height) {
    GAME_PROFILE_FUNCTION();

    mPipeline.frameBuffer->invalidate(width, height);
  }
  
  void Renderer3D::begin3D(const PerspectiveCameraController& cameraController) {
    GAME_PROFILE_FUNCTION();

    mPipeline.camera.projection     = cameraController.getCamera().getProjectionMatrix();
    mPipeline.camera.view           = cameraController.getCamera().getViewMatrix();
    mPipeline.camera.projectionView = mPipeline.camera.projection * mPipeline.camera.view;
    mPipeline.camera.position       = Vec4(cameraController.getPosition(), 0.0f);
    mPipeline.camera.front          = Vec4(cameraController.getFront(), 0.0f);
    mPipeline.camera.clippingPlane  = Vec4(cameraController.getNear(), cameraController.getFar(), 0.0f, 0.0f);

    mCameraUniformBuffer->bind();
    mCameraUniformBuffer->set({&mPipeline.camera, 1});
  }

  void Renderer3D::submit(const Mesh::Handle& _mesh, const Material::Handle& material, const Mat4& transform, u32 entityId) {
    GAME_PROFILE_FUNCTION();

    Mesh::Handle mesh = _mesh;

    if (!mesh->mData.hasInstanced) {
      mesh->mData.vertexArray->bind();
      mPipeline.instancedBuffer->bind();
      mesh->mData.vertexArray->addVertexBuffer(mPipeline.instancedBuffer);
      mesh->mData.vertexArray->unbind();

      mesh->mData.hasInstanced = true;
    }

    // Don't render fully transparent objects
    if (material->transparency == 0.0f) {
      return;
    }

    // TODO: Sort by shader
    u32 index = (u32)mPipeline.units.size();
    RenderUnit unit {
      mesh,
      material,
      transform,
      Mat3(glm::transpose(glm::inverse(transform))),
      entityId,
    };

    if (!unit.material->diffuseMap.isValid())  unit.material->diffuseMap  = mEnvironment.defaultDiffuseMap;
    if (!unit.material->specularMap.isValid()) unit.material->specularMap = mEnvironment.defaultSpecularMap;
    if (!unit.material->emissionMap.isValid()) unit.material->emissionMap = mEnvironment.defaultEmissionMap;

    mPipeline.units.push_back(unit);
    if (material->transparency >= 1.0f) {
      mPipeline.opaqueUnits[material][mesh].push_back(index);
    } else {
      Vec3 position = {transform[3][0], transform[3][1], transform[3][2]};
      f32 distance = glm::length(position - Vec3(mPipeline.camera.position));
      mPipeline.transparentUnitIndices.push_back(std::pair(distance, index));
    }
  }

  void Renderer3D::renderUnit(u32 unitIndex) {
    GAME_TODO("Implement transparency in a better way");
    GAME_PROFILE_FUNCTION();

    RenderUnit& unit = mPipeline.units[unitIndex];

    // TODO: Make this more dynamic
    unit.material->diffuseMap->bind(0);
    unit.material->specularMap->bind(1);
    unit.material->emissionMap->bind(2);

    auto vao = unit.mesh->getVertexArray();
    vao->bind();
    vao->drawIndices();
    vao->unbind();
  }

  void Renderer3D::renderAllUnits() {
    GAME_PROFILE_FUNCTION();

    u32 drawCalls = 0;

    #if GAME_EDITOR

    // Editor selected entity outline
    if (mSelectedEntity != UINT32_MAX) {
      mOutlineShader->bind();

      glEnable(GL_STENCIL_TEST);
      glStencilMask(0xFF);
      glStencilFunc(GL_ALWAYS, 1, 0xFF);
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

      for (auto&[material, meshes] : mPipeline.opaqueUnits) {
        // Sorted by mesh
        for (auto&[mesh, unitIndices] : meshes) {
          mPipeline.instancedCurrentPtr = mPipeline.instancedBasePtr;
          usize count = 0;
          for (auto unitIndex : unitIndices) {
            if (mPipeline.units[unitIndex].entityId == mSelectedEntity) {
              *mPipeline.instancedCurrentPtr = {
                mPipeline.units[unitIndex].modelMatrix,
                mPipeline.units[unitIndex].normalMatrix,
                #if GAME_EDITOR
                  mPipeline.units[unitIndex].entityId,
                #endif
              };
              mPipeline.instancedCurrentPtr++;
              count++;
            }
          }
          
          auto vao = mesh->getVertexArray();
          vao->bind();
          mPipeline.instancedBuffer->bind();
          mPipeline.instancedBuffer->set({mPipeline.instancedBasePtr, count});
          glDrawElementsInstanced(
            GL_LINE_STRIP,
            mesh->mData.indexBuffer->getCount(),
            GL_UNSIGNED_INT,
            0,
            (GLsizei)count
          );

          drawCalls++;
        }
      }
      glStencilFunc(GL_EQUAL, 0, 0xFF);
      glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
    }
    #endif

    mPipeline.shader->bind();
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

    // Sorted by material
    for (auto&[material, meshes] : mPipeline.opaqueUnits) {
      material->diffuseMap->bind(0);
      material->specularMap->bind(1);
      material->emissionMap->bind(2);

      *mPipeline.materialsBasePtr = {0, 1, 2, 0, 0, 0, material->shininess, material->transparency};
      mPipeline.materialsUniformBuffer->bind();
      mPipeline.materialsUniformBuffer->set({mPipeline.materialsBasePtr, 1});

      // Sorted by mesh
      for (auto&[mesh, unitIndices] : meshes) {
        mPipeline.instancedCurrentPtr = mPipeline.instancedBasePtr;
        usize count = 0;
        for (auto unitIndex : unitIndices) {
          *mPipeline.instancedCurrentPtr = {
            mPipeline.units[unitIndex].modelMatrix,
            mPipeline.units[unitIndex].normalMatrix,
            #if GAME_EDITOR
              mPipeline.units[unitIndex].entityId,
            #endif
          };
          mPipeline.instancedCurrentPtr++;
          count++;
        }
  
        auto vao = mesh->getVertexArray();
        vao->bind();
        mPipeline.instancedBuffer->bind();
        mPipeline.instancedBuffer->set({mPipeline.instancedBasePtr, count});
        glDrawElementsInstanced(
          GL_TRIANGLES,
          mesh->mData.indexBuffer->getCount(),
          GL_UNSIGNED_INT,
          0,
          (GLsizei)count
        );

        drawCalls++;
      }
    }

    #if GAME_EDITOR
      glDisable(GL_STENCIL_TEST);
    #endif

    // Logger::trace("Draw calls: %d", drawCalls);

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
    mPipeline.opaqueUnits.clear();
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
    glDisable(GL_BLEND);

    mPipeline.frameBuffer->bind();
    #if GAME_EDITOR
      u32 clearValue = UINT32_MAX;
      glClearTexImage(mPipeline.frameBuffer->getColorAttachment(1)->getId(), 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &clearValue);
    #endif

    Renderer3D::enableDepthTest(true);
    renderAllUnits();
    renderSkybox();
    Renderer3D::enableDepthTest(false);

    mPipeline.frameBuffer->unbind();
    //  glDisable(GL_DEPTH_TEST);

    mPipeline.quadVertexArray->bind();
    
    auto texture = mPipeline.frameBuffer->getColorAttachment();
    texture->bind(0);

    #if 0
    // Bloom Effect
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

    mPipeline.bloomFbo.unbind();
  
    #endif
    
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // Last pass
    #ifndef GAME_PLATFORM_WEB
      glEnable(GL_FRAMEBUFFER_SRGB);
    #endif

    // glViewport(0, 0, mPipeline.mipmaps[0]->getWidth(), mPipeline.mipmaps[0]->getHeight());

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    #if 0
      mPipeline.mipmaps[0]->bind(0);
    #else
      texture->bind(0);
    #endif

    mPipeline.frameBuffer->getDepthAttachment()->bind(1);

    mPipeline.postProcesingShader->bind();
    mPipeline.postProcesingShader->setInt("uScreenTexture", 0);
    mPipeline.postProcesingShader->setInt("uDepthMap", 1);
    mPipeline.quadVertexArray->drawArrays(6);

    #ifndef GAME_PLATFORM_WEB
      glDisable(GL_FRAMEBUFFER_SRGB);
    #endif
  }

  void Renderer3D::end() {
  }

  #if GAME_EDITOR
    u32 Renderer3D::readPixel(u32 x, u32 y) {
      u32 entity = 0;
      
      mPipeline.frameBuffer->bind(false);
      glReadBuffer(GL_COLOR_ATTACHMENT1);
      glReadPixels(x, Application::getWindow().getHeight() - y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &entity);
      glReadBuffer(GL_NONE);

      return entity;
    }

    void Renderer3D::setSelectedEntity(u32 entityId) {
      mSelectedEntity = entityId;
    }
  #endif

} // namespace Game
