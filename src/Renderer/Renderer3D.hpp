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
    // TODO: accept an entity
    void submit(const Mesh::Handle& mesh, const Material::Handle& material, const Mat4& transform = Mat4(1.0f), u32 entityId = 0);

    void flush();
    void end();

    void waitAndRender();
    void invalidate(u32 width, u32 height);

    #if GAME_EDITOR
      u32 readPixel(u32 x, u32 y);
      void setSelectedEntity(u32 entityId);
    #endif

  private:
    void renderAllUnits();
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
      Mat4 modelMatrix;
      Mat3 normalMatrix;

      u32 entityId;
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

      PostProcessing bloomFbo;
      std::array<Texture2D::Handle, 6> mipmaps;
      Shader::Handle bloomPreFilterShader;
      Shader::Handle bloomDownsampleShader;
      Shader::Handle bloomUpsampleShader;
      Shader::Handle bloomFinalShader;

      Shader::Handle shader;

      std::vector<RenderUnit> units;
      std::unordered_map<Material::Handle, std::unordered_map<Mesh::Handle, std::vector<u32>>> opaqueUnits;

      struct Instance {
        Mat4 transformMatrix;
        Mat3 normalMatrix;

        #if GAME_EDITOR
          u32 entityId;
        #endif
      };

      VertexBuffer::Handle instancedBuffer;
      Instance* instancedBasePtr = nullptr;
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
    static constexpr const auto INSTANCE_COUNT       = 1024;
    static constexpr const auto INSTANCE_BUFFER_SIZE = sizeof(Pipeline::Instance) * INSTANCE_COUNT;

    static constexpr const auto MAX_MATERIALS = 32;

  private:
    Pipeline mPipeline;
    Environment mEnvironment;
    UniformBuffer::Handle mCameraUniformBuffer;

    #if GAME_EDITOR
      u32 mSelectedEntity = UINT32_MAX;
      Shader::Handle mOutlineShader;
    #endif
  };

} // namespace Game
