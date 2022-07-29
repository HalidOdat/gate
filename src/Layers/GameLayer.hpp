#pragma once

#include "Core/Math.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Resource/Texture.hpp"
#include "Resource/Mesh.hpp"
#include "Resource/Shader.hpp"
#include "Renderer/CameraController.hpp"
#include "Ecs/Ecs.hpp"
#include "Layers/Layer.hpp"

namespace Game {
    
  class GameLayer : public Layer {
    struct Material {
      Vec3 ambient;
      Vec3 diffuse;
      Vec3 specular;
      float shininess;
    };

    struct Light {
      Vec3 position;

      Vec3 ambient;
      Vec3 diffuse;
      Vec3 specular;
    };

  public:
    GameLayer(f32 aspectRatio);

    virtual void onAttach() override;
    virtual void onDetach() override;
    virtual void onUpdate(Timestep ts) override;
    virtual void onUiRender(Ui& ui) override;
    virtual void onEvent(const Event& event) override;
  
  private:
    bool onWindowResizeEvent(const WindowResizeEvent& event);
    bool onMouseScrollEvent(const MouseScrollEvent& event);
    bool onKeyPressedEvent(const KeyPressedEvent& event);

  private:
    OrthographicCameraController mO;
    PerspectiveCameraController mCameraController;
    Registry mRegistry;

    Texture2D mTexture;
    Vec4 mColor = Vec4(1.0f);

    bool mCaptureCursor = true;

    Shader    mShader;
    Mesh      mCubeMesh;
    u32       mCount = 1;
    bool mMoveLight = true;

    Material mMaterial = {
      Vec3{1.0f, 0.5f, 0.31f},
      Vec3{1.0f, 0.5f, 0.31f},
      Vec3{ 0.5f, 0.5f, 0.5f},
      32.0f
    };

    constexpr static const auto lightColor = Vec3{1.0f, 1.0f, 1.0f};
    Light mLight = {
      Vec3(),
      lightColor * Vec3(0.5f),
      lightColor * Vec3(0.2f),
      Vec3(1.0f, 1.0f, 1.0f),
    };
  };

} // namespace Game
