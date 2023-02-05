#include <cctype>
#include <array>
#include <algorithm>
#include <stdint.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Core/Base.hpp"
#include "Core/OpenGL.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Application.hpp"

namespace Gate {

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mPipeline.postProcesingShader = Shader::load("assets/3D/shaders/PostProcessing.glsl").build();

    mPipeline.frameBuffer = FrameBuffer::builder()
      .clearColor(1.0f, 1.0f, 1.0f, 1.0f)
      .clear(FrameBuffer::Clear::Color | FrameBuffer::Clear::Depth)
      .clearOnBind(true)
      .attach(
        FrameBuffer::Attachment::Type::Texture,
        FrameBuffer::Attachment::Format::Rgba16F
      )
      .attach(
        FrameBuffer::Attachment::Type::Texture,
        #ifdef GATE_PLATFORM_WEB
          FrameBuffer::Attachment::Format::Rgba8
        #else
          FrameBuffer::Attachment::Format::R32UI
        #endif
      )
      .build();

    static const float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
      // positions   // texCoords
      -1.0f,  1.0f,  0.0f, 1.0f,
      -1.0f, -1.0f,  0.0f, 0.0f,
       1.0f, -1.0f,  1.0f, 0.0f,

      -1.0f,  1.0f,  0.0f, 1.0f,
       1.0f, -1.0f,  1.0f, 0.0f,
       1.0f,  1.0f,  1.0f, 1.0f,
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
      "assets/3D/textures/skybox/right.jpg",
      "assets/3D/textures/skybox/left.jpg",
      "assets/3D/textures/skybox/top.jpg",
      "assets/3D/textures/skybox/bottom.jpg",
      "assets/3D/textures/skybox/front.jpg",
      "assets/3D/textures/skybox/back.jpg",
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

    mPipeline.skyboxShader = Shader::load("assets/3D/shaders/Skybox.glsl")
      .build();
    mPipeline.skyboxShader->bind();
    mPipeline.skyboxShader->setVec4("uInvalidEntity", Vec4{
      f32(((UINT32_MAX >>  0) & 0xFF) / 0xFF),
      f32(((UINT32_MAX >>  8) & 0xFF) / 0xFF),
      f32(((UINT32_MAX >> 16) & 0xFF) / 0xFF),
      f32(((UINT32_MAX >> 24) & 0xFF) / 0xFF),
    });

    mPipeline.shader = Shader::load("assets/3D/shaders/SpotLight.glsl")
      .define("MAX_MATERIALS", std::to_string(MAX_MATERIALS))
      .build();
    mPipeline.shader->bind();
    i32 samples[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    mPipeline.shader->setIntArray("uTextures", samples, 16);

    mEnvironment.defaultDiffuseMap  = Texture::color(0xFF'FF'FF'FF).build();
    mEnvironment.defaultSpecularMap = Texture::color(0x00'00'00'FF).build();
    mEnvironment.defaultEmissionMap = Texture::color(128, 128, 128).build();

    mPipeline.instancedBuffer = VertexBuffer::builder()
      .storage(Buffer::StorageType::Dynamic)
      .size(INSTANCE_BUFFER_SIZE)
      .layout(BufferElement::Type::Mat4, "aModelMatrix", 1)
      .layout(BufferElement::Type::Mat3, "aNormalMatrix", 1)
      .layout(BufferElement::Type::Float4, "aEntityId", 1)
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
    delete[] mPipeline.instancedBasePtr;
    delete[] mPipeline.materialsBasePtr;
  }

  void Renderer3D::invalidate(u32 width, u32 height) {
    mPipeline.frameBuffer->invalidate(width, height);
  }

  void Renderer3D::begin3D(const PerspectiveCameraController& cameraController) {
    mPipeline.camera.projection     = cameraController.getCamera().getProjectionMatrix();
    mPipeline.camera.view           = cameraController.getCamera().getViewMatrix();
    mPipeline.camera.projectionView = mPipeline.camera.projection * mPipeline.camera.view;
    mPipeline.camera.position       = Vec4(cameraController.getPosition(), 0.0f);
    mPipeline.camera.front          = Vec4(cameraController.getFront(), 0.0f);
    mPipeline.camera.clippingPlane  = Vec4(cameraController.getNear(), cameraController.getFar(), 0.0f, 0.0f);

    mCameraUniformBuffer->bind();
    mCameraUniformBuffer->set({&mPipeline.camera, 1});
  }

  void Renderer3D::submit(const Mesh::Handle& _mesh, const Material::Handle& _material, const Mat4& transform, u32 entityId) {
    Mesh::Handle mesh = _mesh;
    Material::Handle material = _material;

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

    u32 index = (u32)mPipeline.units.size();
    RenderUnit unit {
      transform,
      Mat3(glm::transpose(glm::inverse(transform))),
      Vec4{
        f32(((entityId >>  0) & 0xFF) / 0xFF),
        f32(((entityId >>  8) & 0xFF) / 0xFF),
        f32(((entityId >> 16) & 0xFF) / 0xFF),
        f32(((entityId >> 24) & 0xFF) / 0xFF),
      },
    };

    // TODO: don't change material
    if (!material->diffuseMap)  material->diffuseMap  = mEnvironment.defaultDiffuseMap;
    if (!material->specularMap) material->specularMap = mEnvironment.defaultSpecularMap;
    if (!material->emissionMap) material->emissionMap = mEnvironment.defaultEmissionMap;

    mPipeline.units.push_back(unit);
    if (material->transparency >= 1.0f) {
      mPipeline.opaqueUnits[material][mesh].push_back(index);
    } else {
      // Vec3 position = {transform[3][0], transform[3][1], transform[3][2]};
      // f32 distance = glm::length(position - Vec3(mPipeline.camera.position));
      // mPipeline.transparentUnitIndices.push_back(std::pair(distance, index));
      GATE_TODO("Support transparent objects");
    }
  }

  void Renderer3D::renderAllUnits() {
    u32 drawCalls = 0;

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
          GATE_ASSERT(count + 1 < INSTANCE_COUNT);
          *mPipeline.instancedCurrentPtr = {
            mPipeline.units[unitIndex].modelMatrix,
            mPipeline.units[unitIndex].normalMatrix,
            mPipeline.units[unitIndex].entityId,
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

    // Logger::trace("Draw calls: %d", drawCalls);

    mPipeline.units.clear();
    mPipeline.opaqueUnits.clear();
  }

  void Renderer3D::renderSkybox() {
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
    // First Pass
    glDisable(GL_BLEND);

    mPipeline.frameBuffer->bind();
    // u32 clearValue = UINT32_MAX;
    // glClearTexImage(mPipeline.frameBuffer->getColorAttachment(1)->getId(), 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &clearValue);

    Renderer3D::enableDepthTest(true);
    renderAllUnits();
    renderSkybox();
    Renderer3D::enableDepthTest(false);

    mPipeline.frameBuffer->unbind();


    mPipeline.quadVertexArray->bind();

    auto texture = mPipeline.frameBuffer->getColorAttachment();
    texture->bind(0);

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    texture->bind(0);

    mPipeline.postProcesingShader->bind();
    mPipeline.postProcesingShader->setInt("uScreenTexture", 0);
    mPipeline.postProcesingShader->setInt("uDepthMap", 1);
    mPipeline.quadVertexArray->drawArrays(6);
  }

  void Renderer3D::end() {
  }

  u32 Renderer3D::readPixel(u32 x, u32 y) {
    u32 entity = 0;

    mPipeline.frameBuffer->bind(false);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    u8 data[4];
    glReadPixels(x, Application::getWindow().getHeight() - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    entity = data[0] + (u32(data[1]) << 8) + (u32(data[2]) << 16) + (u32(data[3]) << 24);
    
    glReadBuffer(GL_NONE);

    return entity;
  }

  void Renderer3D::setSelectedEntity(u32 entityId) {
    mSelectedEntity = entityId;
  }

} // namespace Gate
