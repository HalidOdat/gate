#include "Scene/SceneSerializer.hpp"
#include "Serializer/Serializer.hpp"

namespace Game {
  
  String SceneSerializer::serialize(Scene& scene) {
    return Serializer::Node(scene).toString();
  }

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
        if (entity.has<MeshRendererComponent>()) {
          components["MeshRendererComponent"] = entity.get<MeshRendererComponent>();
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

    template<>
    struct Convert<MeshRendererComponent> {
      static Node encode(const MeshRendererComponent& component) {
        auto node = Node::object();
        node["material"] = component.material;
        return node;
      }
    };

    template<>
    struct Convert<Material::Handle> {
      static Node encode(const Material::Handle& material) {
        auto node = Node::object();
        node["diffuseMap"] = material->diffuseMap;
        node["specularMap"] = material->specularMap;
        node["emissionMap"] = material->emissionMap;
        node["transparancy"] = material->transparency;
        node["shininess"] = material->shininess;
        return node;
      }
    };

    template<>
    struct Convert<Texture2D::Handle> {
      static Node encode(const Texture2D::Handle& texture) {
        auto node = Node::object();
        node["path"] = texture->getFilePath();
        return node;
      }
    };

  }

} // namespace Game

