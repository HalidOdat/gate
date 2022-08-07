#pragma once

#include "Core/Type.hpp"
#include "Core/Timestep.hpp"
#include "Ecs/Ecs.hpp"
#include "Scene/Components.hpp"
#include "Serializer/Serializer.hpp"

namespace Game {
  
  class Entity;

  class Scene {
  public:
    Scene(String name);

    inline const String& getName() const { return mName; }

    Entity createEntity(String tag = "Entity");

    void render(const PerspectiveCameraController& camera);
    void onUpdate(Timestep ts);

  private:
    String mName;
    Ecs::Registry mRegistry;

  private:
    friend class Entity;
    friend struct Serializer::Convert<Scene>;
  };

  class Entity {
  public:
    Entity()
      : mEntityId{0}, mScene{nullptr}
    {}

    template<typename T, typename ...Args>
    T& add(Args&&... args) {
      return mScene->mRegistry.assign<T>(
        Ecs::Entity(mEntityId),
        std::forward<Args>(args)...
      );
    }

    template<typename T, typename ...Ts>
    bool has() {
      return mScene->mRegistry.hasComponent<T, Ts...>(Ecs::Entity(mEntityId));
    }

    template<typename T, typename ...Ts>
    auto get() {
      return mScene->mRegistry.get<T, Ts...>(Ecs::Entity(mEntityId));
    }

  private:
    Entity(Ecs::Entity entity, Scene* scene)
      : mEntityId{entity.getId()}, mScene{scene}
    {}

  private:
    Ecs::Entity::Id mEntityId;
    Scene* mScene;

  private:
    friend class  Scene;
    friend struct Serializer::Convert<Scene>;
  };

  namespace Serializer {

    template<>
    struct Convert<Scene> {
      static Node encode(const Scene& scene) {
        auto node = Node::object();
        node["name"] = scene.getName();

        auto entities = Node::array();
        auto view = scene.mRegistry.view<TagComponent>();
        for (auto[entity, _] : view) {
          entities.push(Entity(entity, (Scene*)&scene));
        }
        node["entities"] = entities;
        return node;
      }
    };

    template<>
    struct Convert<Entity> {
      static Node encode(Entity entity) {
        auto node = Node::object();

        auto components = Node::object();
        if (entity.has<TagComponent>()) {
          components["TagComponent"] = entity.get<TagComponent>();
        }
        if (entity.has<TransformComponent>()) {
          components["TransformComponent"] = entity.get<TransformComponent>();
        }
        node["components"] = components;
        return node;
      }
    };

    template<>
    struct Convert<TagComponent> {
      static Node encode(const TagComponent& component) {
        auto node = Node::object();
        node["tag"] = component.tag;
        return node;
      }
    };

    template<>
    struct Convert<TransformComponent> {
      static Node encode(const TransformComponent& component) {
        auto node = Node::object();
        node["translation"] = component.translation;
        node["rotation"]    = component.rotation;
        node["scale"]       = component.scale;
        return node;
      }
    };

    template<>
    struct Convert<Vec3> {
      static Node encode(const Vec3& vec) {
        auto node = Node::array();
        node.push(vec.x);
        node.push(vec.y);
        node.push(vec.z);
        node.setStyle(Node::Style::Flat);
        return node;
      }
    };

  }

} // namespace Game
