#pragma once

#include "Core/Base.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/UniformBuffer.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/CameraController.hpp"

#include <unordered_map>

namespace Gate {

  class Renderer3D {
    friend class Application;

  public:
    Renderer3D();
    ~Renderer3D();

    void enableBlending(bool  yes = true);
    void enableDepthTest(bool yes = true);
    void enableCullFace(bool  yes = true);

    void begin(const PerspectiveCameraController& cameraController);
    void submit(const Mesh::Handle& mesh, const Material::Handle& material, const Mat4& transform = Mat4(1.0f), u32 entityId = UINT32_MAX);

    void flush();
    void end();

    void waitAndRender();
    void invalidate(u32 width, u32 height);

    u32 readPixel(u32 x, u32 y);
    void setSelectedEntity(u32 entityId);

  private:
    void renderAllUnits();
    void renderSkybox();

  private:
    struct RenderUnit {
      Mat4 modelMatrix;
      Mat3 normalMatrix;
      glm::uvec4 entityId;
    };

    // NOTE: This struct must be aligned accroding to the std140 standard.
    struct RenderCamera {
      Mat4 projection;
      Mat4 view;
      Mat4 projectionView;

      // Note: Because some implementations get the aligment of vec3 wrong, we are advised to use vec4 here instead.
      //       Check: https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)
      Vec4 position;
      Vec4 front;
      Vec4 clippingPlane;
    };

    // NOTE: This struct must be aligned accroding to the std140 standard.
    struct MaterialInternal {
      u32 diffuse;
      u32 specular;
      u32 emission;
      u32 padding0;
      u32 padding1;
      u32 padding2;
      f32 shininess;
      f32 transparency;
    };

    static_assert(sizeof(MaterialInternal) == 32);

    struct Pipeline {
      Shader::Handle postProcesingShader;
      RenderCamera camera;
      FrameBuffer::Handle frameBuffer;
      VertexArray::Handle quadVertexArray;

      CubeMap::Handle  skyboxTexture;
      VertexArray::Handle skyboxVertexArray;
      Shader::Handle   skyboxShader;

      Shader::Handle shader;

      std::vector<RenderUnit> units;
      std::unordered_map<Material::Handle, std::unordered_map<Mesh::Handle, std::vector<u32>>> opaqueUnits;

      struct Instance {
        Mat4 transformMatrix;
        Mat3 normalMatrix;
        glm::uvec4 entityId;
      };

      VertexBuffer::Handle instancedBuffer;
      Instance* instancedBasePtr = nullptr;
      Instance* instancedCurrentPtr = nullptr;

      UniformBuffer::Handle materialsUniformBuffer;
      MaterialInternal* materialsBasePtr = nullptr;
      MaterialInternal* materialsCurrentPtr = nullptr;
    };

    struct Environment {
      Texture::Handle defaultDiffuseMap;
      Texture::Handle defaultSpecularMap;
      Texture::Handle defaultEmissionMap;
    };
  private:
    static constexpr const auto INSTANCE_COUNT       = 512;
    static constexpr const auto INSTANCE_BUFFER_SIZE = sizeof(Pipeline::Instance) * INSTANCE_COUNT;

    static constexpr const auto MAX_MATERIALS = 32;

  private:
    Pipeline mPipeline;
    Environment mEnvironment;
    UniformBuffer::Handle mCameraUniformBuffer;

    u32 mSelectedEntity = UINT32_MAX;
    Shader::Handle mOutlineShader;
  };

} // namespace Gate
