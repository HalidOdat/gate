#pragma once

#include "Core/Math.hpp"
#include "Resource/Mesh.hpp"
#include "Renderer/Material.hpp"
#include "Ecs/Component.hpp"
#include "Renderer/CameraController.hpp"

namespace Game {

  struct TagComponent : Ecs::Component<TagComponent> {
    TagComponent(String tag)
      : tag{std::move(tag)}
    {}

    String tag;
  };

  struct TransformComponent : Ecs::Component<TransformComponent> {
    TransformComponent(
      const Vec3& translation = {0.0f, 0.0f, 0.0f},
      const Vec3& rotation    = {0.0f, 0.0f, 0.0f},
      const Vec3& scale       = {1.0f, 1.0f, 1.0f}
    ) : translation{translation}, rotation{rotation}, scale{scale}
    {}

    Vec3 translation;
    Vec3 rotation;
    Vec3 scale;

    Mat4 getTranformMatrix() const;
  };

  struct MeshSourceComponent : Ecs::Component<MeshSourceComponent> {
    MeshSourceComponent() = default;
    MeshSourceComponent(const Mesh::Handle& mesh)
      : mesh{mesh}
    {}

    Mesh::Handle mesh;
  };

  struct MeshRendererComponent : Ecs::Component<MeshSourceComponent> {
    MeshRendererComponent(const Material::Handle& material)
      : material{material}
    {}

    Material::Handle material;
  };

} // namespace Game
