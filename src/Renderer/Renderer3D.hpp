#pragma once

#include "Core/Math.hpp"
#include "Utils/Color.hpp"
#include "Resource/Manager.hpp"
#include "Renderer/VertexArray.hpp"
#include "Resource/UniformBuffer.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/CameraController.hpp"

#include <unordered_map>

namespace Game {

  class Renderer3D {
    friend class Application;

  public:
    Renderer3D();
    ~Renderer3D();

    void enableBlending(bool  yes = true);
    void enableDepthTest(bool yes = true);
    void enableCullFace(bool  yes = true);

    void begin3D(const PerspectiveCameraController& cameraController);
    void submit(const Mesh::Handle& mesh, const Material::Handle& material, const Mat4& transform = Mat4(1.0f));

    void flush();
    void end();

    void waitAndRender();
    void invalidate(u32 width, u32 height);

  private:
    static constexpr const auto INSTANCE_COUNT       = 1024;
    static constexpr const auto INSTANCE_BUFFER_SIZE = (sizeof(Mat4) + sizeof(Mat3)) * INSTANCE_COUNT;

    static constexpr const auto MAX_MATERIALS = 32;

  private:
    void renderAllUnits();
    void renderUnit(u32 unitIndex);
    void renderSkybox();

  private:

    class PostProcessing {
    public:
      PostProcessing();
      ~PostProcessing();

      void bind();
      void unbind();
      void setTexture(Texture2D::Handle& dest);
      void clear();

    private:
      u32 mId;
    };

    struct RenderUnit {
      Mesh::Handle     mesh;
      Material::Handle material;

      Mat4 modelMatrix;
      Mat3 normalMatrix;
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

      PostProcessing bloomFbo;
      std::array<Texture2D::Handle, 6> mipmaps;
      Shader::Handle bloomPreFilterShader;
      Shader::Handle bloomDownsampleShader;
      Shader::Handle bloomUpsampleShader;
      Shader::Handle bloomFinalShader;

      Shader::Handle shader;

      std::vector<RenderUnit>                               units;
      std::unordered_map<Material::Handle, std::unordered_map<Mesh::Handle, std::vector<u32>>> opaqueUnits;

      std::vector<std::pair<f32, u32>> transparentUnitIndices; // distance from camera and index

      struct Instance {
        Mat4 transformMatrix;
        Mat3 normalMatrix;
      };

      VertexBuffer::Handle instancedBuffer;
      Instance* instancedBasePtr    = nullptr;
      Instance* instancedCurrentPtr = nullptr;

      UniformBuffer::Handle materialsUniformBuffer;
      MaterialInternal* materialsBasePtr = nullptr;
      MaterialInternal* materialsCurrentPtr = nullptr;
    };

    struct Environment {
      Texture2D::Handle defaultDiffuseMap;
      Texture2D::Handle defaultSpecularMap;
      Texture2D::Handle defaultEmissionMap;
    };

  private:
    Pipeline mPipeline;
    Environment mEnvironment;
    UniformBuffer::Handle mCameraUniformBuffer;
  };

} // namespace Game
