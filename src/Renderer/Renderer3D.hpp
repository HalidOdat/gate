#pragma once

#include "Core/Math.hpp"
#include "Utils/Color.hpp"
#include "Resource/Manager.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/CameraController.hpp"

namespace Game {
  
  class Application;

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
      Mesh::Handle mesh;
      Material     material;

      Mat4 modelMatrix;
      Mat3 normalMatrix;
    };

    struct RenderCamera {
      Mat4 projection;
      Mat4 view;
      
      Vec3 position;
      Vec3 front;
    };

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
      std::vector<RenderUnit>          units;
      std::vector<u32>                 opaqueUnitIndices;
      std::vector<std::pair<f32, u32>> transparentUnitIndices; // distance from camera and index
    };

    struct Environment {
      Texture2D::Handle defaultDiffuseMap;
      Texture2D::Handle defaultSpecularMap;
      Texture2D::Handle defaultEmissionMap;
    };

  private:
    Pipeline mPipeline;
    Environment mEnvironment;
  };

} // namespace Game
