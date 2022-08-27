#include "Scene/SceneSerializer.hpp"
#include "Serializer/Serializer.hpp"
#include "Utils/File.hpp"

namespace Game {
  
  String SceneSerializer::serialize(Scene& scene) {
    GAME_PROFILE_FUNCTION();
    return Serializer::Node(scene).toString();
  }

  namespace Serializer {

    template<>
    struct Convert<Entity> {
      static Node encode(Entity entity) {
        GAME_PROFILE_FUNCTION();

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
        GAME_PROFILE_FUNCTION();

        auto node = Node::array();
        node.push(vec.x);
        node.push(vec.y);
        node.push(vec.z);
        node.setStyle(Node::Style::Flat);
        return node;
      }
      static bool decode(const Node& node, Vec3& value) {
        GAME_PROFILE_FUNCTION();

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
        GAME_PROFILE_FUNCTION();
        auto node = Node::object();
        node["tag"] = component.tag;
        return node;
      }
      static bool decode(const Node& node, TagComponent& component) {
        GAME_PROFILE_FUNCTION();
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
        GAME_PROFILE_FUNCTION();

        auto node = Node::object();
        node["translation"] = component.translation;
        node["rotation"]    = component.rotation;
        node["scale"]       = component.scale;
        return node;
      }
      static bool decode(const Node& node, TransformComponent& component) {
        GAME_PROFILE_FUNCTION();

        if (!node.isObject()) {
          return false;
        }
        if (auto translation = node.get("translation"); translation) {
          if (!Convert<Vec3>::decode(*translation, component.translation)) {
            return false;
          }
        }
        if (auto rotation = node.get("rotation"); rotation) {
          if (!Convert<Vec3>::decode(*rotation, component.rotation)) {
            return false;
          }
        }
        if (auto scale = node.get("scale"); scale) {
          if (!Convert<Vec3>::decode(*scale, component.scale)) {
            return false;
          }
        }
        return true;
      }
    };

    template<>
    struct Convert<MeshSourceComponent> {
      static Node encode(const MeshSourceComponent& component) {
        GAME_PROFILE_FUNCTION();

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
        GAME_PROFILE_FUNCTION();

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
    struct Convert<Texture2D::Handle> {
      static Node encode(const Texture2D::Handle& texture) {
        GAME_PROFILE_FUNCTION();

        auto node = Node::object();
        node["path"] = texture->getFilePath();
        return node;
      }
      static bool decode(const Node& node, Texture2D::Handle& texture) {
        GAME_PROFILE_FUNCTION();

        if (!node.isObject()) {
          return false;
        }
        const auto& path = node.get("path");
        if (!path || !path->isString()) {
          return false;
        }
        // TODO: decode Texture2D specification
        texture = Texture2D::load(*path->asString())
          .build();
        return true;
      }
    };

    template<>
    struct Convert<Material::Handle> {
      static Node encode(const Material::Handle& material) {
        GAME_PROFILE_FUNCTION();

        auto node = Node::object();
        node["diffuseMap"] = material->diffuseMap;
        node["specularMap"] = material->specularMap;
        node["emissionMap"] = material->emissionMap;
        node["transparancy"] = material->transparency;
        node["shininess"] = material->shininess;
        return node;
      }
      static bool decode(const Node& node, Material::Handle& material) {
        GAME_PROFILE_FUNCTION();

        if (!node.isObject()) {
          return false;
        }
        material = Material::create();
        if (const auto& texture = node.get("diffuseMap"); texture) {
          if (!Convert<Texture2D::Handle>::decode(*texture, material->diffuseMap)) {
            return false;
          }
        }
        if (const auto& texture = node.get("specularMap"); texture) {
          if (!Convert<Texture2D::Handle>::decode(*texture, material->specularMap)) {
            return false;
          }
        }
        if (const auto& texture = node.get("emissionMap"); texture) {
          if (!Convert<Texture2D::Handle>::decode(*texture, material->emissionMap)) {
            return false;
          }
        }
        if (const auto& transparency = node.get("transparency"); transparency) {
          if (transparency->isFloat()) {
            material->transparency = (f32)*transparency->asFloat();
          } else if (transparency->isInteger()) {
            material->transparency = (f32)*transparency->asInteger();
          } else {
            return false;
          }
        }
        if (const auto& shininess = node.get("shininess"); shininess) {
          if (shininess->isFloat()) {
            material->shininess = (f32) *shininess->asFloat();
          } else if (shininess->isInteger()) {
            material->shininess = (f32) *shininess->asInteger();
          } else {
            return false;
          }
        }
        return true;
      }
    };

    template<>
    struct Convert<MeshRendererComponent> {
      static Node encode(const MeshRendererComponent& component) {
        GAME_PROFILE_FUNCTION();

        auto node = Node::object();
        node["material"] = component.material;
        return node;
      }
      static bool decode(const Node& node, MeshRendererComponent& component) {
        GAME_PROFILE_FUNCTION();

        if (!node.isObject()) {
          return false;
        }

        const auto& material = node.get("material");
        if (material) {
          if (!Convert<Material::Handle>::decode(*material, component.material)) {
            return false;
          }
        }
        return true;
      }
    };

    template<>
    struct Convert<Scene> {
      static Node encode(const Scene& scene) {
        GAME_PROFILE_FUNCTION();

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
        GAME_PROFILE_FUNCTION();

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
    GAME_PROFILE_FUNCTION();
    
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

