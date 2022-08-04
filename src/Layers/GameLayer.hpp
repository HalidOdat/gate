#pragma once

#include "Core/Math.hpp"
#include "Events/KeyEvent.hpp"
#include "Events/WindowEvent.hpp"
#include "Events/MouseEvent.hpp"
#include "Resource/Texture.hpp"
#include "Resource/Mesh.hpp"
#include "Resource/Shader.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/CameraController.hpp"
#include "Ecs/Ecs.hpp"
#include "Layers/Layer.hpp"

namespace Game {
    
  class GameLayer : public Layer {
    struct Light {
      Vec3 position;

      Vec3 ambient;
      Vec3 diffuse;
      Vec3 specular;
    };

  public:
    GameLayer();

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
    PerspectiveCameraController mCameraController;
    bool mCaptureCursor = false;

    Registry mRegistry;

    Resource<Shader> mShader;
    Resource<Mesh>   mCubeMesh;
    u32       mCount = 1;
    bool mMoveLight = true;

    Material mMaterial;

    constexpr static const auto lightColor = Vec3{1.0f, 1.0f, 1.0f};
    Light mLight = {
      Vec3(),
      lightColor * Vec3(0.5f),
      lightColor * Vec3(0.2f),
      Vec3(1.0f, 1.0f, 1.0f),
    };
  };

} // namespace Game
