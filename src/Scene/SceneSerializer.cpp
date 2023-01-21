#include "Scene/SceneSerializer.hpp"
#include "Serializer/Serializer.hpp"
#include "Utils/File.hpp"

namespace Game {
  
  String SceneSerializer::serialize(Scene& scene) {
    return Serializer::Node(scene).toString();
  }

  namespace Serializer {

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
        if (entity.has<MeshSourceComponent>()) {
          components["MeshSourceComponent"] = entity.get<MeshSourceComponent>();
        }
        node["components"] = components;
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
      static bool decode(const Node& node, Vec3& value) {
        if (!node.isArray() || node.asArray()->size() != 3) {
          return false;
        }

        const auto& x = node[0];
        if (x.isFloat()) {
          value.x = (f32) *x.asFloat();
        } else if (x.isInteger()) {
          value.x = (f32) *x.asInteger();
        } else {
          return false;
        }

        const auto& y = node[1];
        if (y.isFloat()) {
          value.y = (f32) *y.asFloat();
        } else if (y.isInteger()) {
          value.y = (f32) *y.asInteger();
        } else {
          return false;
        }

        const auto& z = node[2];
        if (z.isFloat()) {
          value.z = (f32) *z.asFloat();
        } else if (z.isInteger()) {
          value.z = (f32) *z.asInteger();
        } else {
          return false;
        }
        return true;
      }
    };

    template<>
    struct Convert<TagComponent> {
      static Node encode(const TagComponent& component) {
        auto node = Node::object();
        node["tag"] = component.tag;
        return node;
      }
      static bool decode(const Node& node, TagComponent& component) {
        if (!node.isObject()) {
          return false;
        }
        if (auto tag = node.get("tag"); tag) {
          if (!tag->isString()) {
            return false;
          }
          component.tag = *tag->asString();
          return true;
        }
        return false;
      }
    };

    template<>
    struct Convert<TransformComponent> {
      static Node encode(const TransformComponent& component) {
        auto node = Node::object();
        node["translation"] = component.getTranslation();
        node["rotation"]    = component.getRotation();
        node["scale"]       = component.getScale();
        return node;
      }
      static bool decode(const Node& node, TransformComponent& component) {
        if (!node.isObject()) {
          return false;
        }
        Vec3 translation;
        Vec3 rotation;
        Vec3 scale;
        if (auto translationNode = node.get("translation"); translationNode) {
          if (!Convert<Vec3>::decode(*translationNode, translation)) {
            return false;
          }
        }
        if (auto rotationNode = node.get("rotation"); rotationNode) {
          if (!Convert<Vec3>::decode(*rotationNode, rotation)) {
            return false;
          }
        }
        if (auto scaleNode = node.get("scale"); scaleNode) {
          if (!Convert<Vec3>::decode(*scaleNode, scale)) {
            return false;
          }
        }
        component = TransformComponent(translation, rotation, scale);
        return true;
      }
    };

    template<>
    struct Convert<MeshSourceComponent> {
      static Node encode(const MeshSourceComponent& component) {
        auto node = Node::object();
        switch (component.mesh->getType()) {
          case Mesh::Type::Cube:
            node["type"] = String("Cube");
            break;
          case Mesh::Type::File:
            node["type"] = String("File");
            node["path"] = *component.mesh->getFilePath();
            break;
          default:
            GAME_UNREACHABLE("unknown mesh type!");
        }
        return node;
      }
      static bool decode(const Node& node, MeshSourceComponent& component) {
        if (!node.isObject()) {
          return false;
        }
        auto* typeNode = node.get("type");
        if (!typeNode) {
          return false;
        }
        const auto* type = typeNode->asString();
        if (!type) {
          return false;
        }
        if (*type == "Cube") {
          component.mesh = Mesh::cube();
        } else if (*type == "File") {
          const auto& path = node.get("path");
          if (!path || !path->isString()) {
            return false;
          }
          component.mesh = Mesh::load(*path->asString());
        } else {
          return false;
        }
        return true;
      }
    };

    template<>
    struct Convert<MeshRendererComponent> {
      static Node encode(const MeshRendererComponent& component) {
        auto node = Node::object();
        node["material"] = component.material->name;
        return node;
      }
      static bool decode(const Node& node, MeshRendererComponent& component) {
        if (!node.isObject()) {
          return false;
        }

        if (const auto& material = node.get("material"); material) {
          if (!material->isString()) {
            return false;
          }
          component.material = Material::get(*material->asString());
        }
        return true;
      }
    };

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

      static bool decode(const Node& node, Scene& scene) {
        if (!node.isObject()) {
          return false;
        }
        if (auto name = node.get("name"); name) {
          if (!Convert<String>::decode(*name, scene.mName)) {
            return false;
          }
        }
        if (auto entitiesOption = node.get("entities"); entitiesOption) {
          const Node& entities = *entitiesOption;
          if (!entities.isArray()) {
            return false;
          }
          const usize size = entities.size();
          for (usize i = 0; i < size; ++i) {
            const Node& entity = entities[i];

            if (!entity.isObject()) {
              return false;
            }

            const auto* components = entity.get("components");
            if (!components) {
              return false;
            }

            auto entityId = scene.mRegistry.create();
            if (const auto component = components->get("TagComponent"); component) {
              TagComponent tag;
              if (!Convert<TagComponent>::decode(*component, tag)) {
                return false;
              }
              scene.mRegistry.assign<TagComponent>(entityId, tag);
            }
            if (const auto component = components->get("TransformComponent"); component) {
              TransformComponent transform;
              if (!Convert<TransformComponent>::decode(*component, transform)) {
                return false;
              }
              scene.mRegistry.assign<TransformComponent>(entityId, transform);
            }
            if (const auto component = components->get("MeshSourceComponent"); component) {
              MeshSourceComponent meshSource;
              if (!Convert<MeshSourceComponent>::decode(*component, meshSource)) {
                return false;
              }
              scene.mRegistry.assign<MeshSourceComponent>(entityId, meshSource);
            }
            if (const auto component = components->get("MeshRendererComponent"); component) {
              MeshRendererComponent value;
              if (!Convert<MeshRendererComponent>::decode(*component, value)) {
                return false;
              }
              scene.mRegistry.assign<MeshRendererComponent>(entityId, value);
            }
          }
        }
        return true;
      }
    };

  }

  bool SceneSerializer::deserializeFromFile(const StringView& file, Scene& scene) {
    auto filePath = String(file);
    auto source = Utils::fileToString(filePath);
    if (!source) {
      Logger::error("Scene serializer: Couldn't open scene file: %s", filePath.c_str());
      return false;
    }

    auto node = Serializer::Json::parse(*source);
    if (!node) {
      Logger::error("Scene serializer: Couldn't parse scene file: %s", filePath.c_str());
    }

    return Serializer::Convert<Scene>::decode(*node, scene);
  }

} // namespace Game

