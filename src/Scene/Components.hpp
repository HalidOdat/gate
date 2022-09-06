#pragma once

#include "Core/Math.hpp"
#include "Resource/Mesh.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/CameraController.hpp"
#include "Ecs/Component.hpp"
#include "Scene/Entity.hpp"

namespace Game {

  struct TagComponent : Ecs::Component<TagComponent> {
    TagComponent() {}
    TagComponent(String tag)
      : tag{std::move(tag)}
    {}

    String tag;
  };

  struct RelationshipComponent {
    RelationshipComponent(Entity parent)
      : parent{parent}
    {}
    Entity parent;
  };

  class TransformComponent : Ecs::Component<TransformComponent> {
  public:
    TransformComponent(
      const Vec3& translation = {0.0f, 0.0f, 0.0f},
      const Vec3& rotation    = {0.0f, 0.0f, 0.0f},
      const Vec3& scale       = {1.0f, 1.0f, 1.0f}
    );

    inline const Vec3& getTranslation() const { return mTranslation; }
    inline const Vec3& getRotation() const { return mRotation; }
    inline const Vec3& getScale() const { return mScale; }

    inline void offsetTranslation(const Vec3& offset) { setTranslation(mTranslation + offset); }
    inline void offsetRotation(const Vec3& offset) { setRotation(mRotation + offset); }
    inline void offsetScale(const Vec3& offset) { setScale(mScale + offset); }

    void setTranslation(const Vec3& value);
    void setRotation(const Vec3& value);
    void setScale(const Vec3& value);

    inline const Mat4& getTransform() const { return mTransform; }

  private:
    void recalculateTransform();

  private:
    Mat4 mTransform;
    Vec3 mTranslation;
    Vec3 mRotation;
    Vec3 mScale;
  };

  struct VelocityComponent : Ecs::Component<VelocityComponent> {
    VelocityComponent() {}
    VelocityComponent(const Vec3& velocity)
      : velocity{velocity}
    {}

    Vec3 velocity = {0.0f, 0.0f, 0.0f};
  };

  struct MeshSourceComponent : Ecs::Component<MeshSourceComponent> {
    MeshSourceComponent() = default;
    MeshSourceComponent(const Mesh::Handle& mesh)
      : mesh{mesh}
    {}

    Mesh::Handle mesh;
  };

  struct MeshRendererComponent : Ecs::Component<MeshSourceComponent> {
    MeshRendererComponent() {}
    MeshRendererComponent(const Material::Handle& material)
      : material{material}
    {}

    Material::Handle material;
  };

} // namespace Game
