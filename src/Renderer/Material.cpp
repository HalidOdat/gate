#include "Renderer/Material.hpp"
#include "Resource/Factory.hpp"
#include "Utils/File.hpp"
#include "Serializer/Serializer.hpp"

namespace Game {

  GAME_FACTORY_IMPLEMENTATION(Material, factory)

  static std::unordered_map<String, Material::Handle> materials;

  Material::Handle Material::get(const String& name) {
    if (!materials.contains(name)) {
      return {};
    }
    
    return materials.at(name);
  }

  Material::Handle Material::create(const String& name) {
    if (materials.contains(name)) {
      Logger::error("Material with name '%s' already exists", name.c_str());
      GAME_ASSERT_WITH_MESSAGE(false, "Attempting to create already existing material");
    }

    auto result = factory.emplace(name);
    materials[name] = result;
    return result;
  }

  Material::Handle Material::getOrCreate(const String& name) {
    if (materials.contains(name)) {
      return materials.at(name);
    }

    auto result = factory.emplace(name);
    materials[name] = result;
    return result;
  }

  bool Material::initialize() {
    auto filePath = String("assets/materials.json");
    auto source = Utils::fileToString(filePath);
    if (!source) {
      Logger::error("Material serializer: Couldn't open materials file: %s", filePath.c_str());
      return false;
    }

    const auto node = Serializer::Json::parse(*source);
    if (!node) {
      Logger::error("Material serializer: Couldn't parse material file: %s", filePath.c_str());
    }

    if (!node->isArray()) {
      return false;
    }

    auto& array = *(node->asArray());
    for (auto& node : array) {
      String nameMaterial;
      if (const auto& name = node.get("name"); name) {
        if (!name->isString()) {
          return false;
        }
        nameMaterial = *(name->asString());
      }
      Logger::trace("Material: Loaded material: %s", nameMaterial.c_str());
      auto material = Material::create(nameMaterial);

      if (const auto& texture = node.get("diffuseMap"); texture) {
        if (!texture->isObject()) {
          return false;
        }
        const auto& path = texture->get("path");
        if (!path || !path->isString()) {
          return false;
        }
        // TODO: decode Texture2D specification
        material->diffuseMap = Texture2D::load(*path->asString())
          .build();
      }
      if (const auto& texture = node.get("specularMap"); texture) {
        if (!texture->isObject()) {
          return false;
        }
        const auto& path = texture->get("path");
        if (!path || !path->isString()) {
          return false;
        }
        // TODO: decode Texture2D specification
        material->specularMap = Texture2D::load(*path->asString())
          .build();
      }
      if (const auto& texture = node.get("emissionMap"); texture) {
        if (!texture->isObject()) {
          return false;
        }
        const auto& path = texture->get("path");
        if (!path || !path->isString()) {
          return false;
        }
        // TODO: decode Texture2D specification
        material->emissionMap = Texture2D::load(*path->asString())
          .build();
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
    }
    return true; 
  }

  void Material::destroy() {
    materials.clear();
    factory.clear();
  }

//       template<>
//     struct Convert<Texture2D::Handle> {
//       static Node encode(const Texture2D::Handle& texture) {
//         GAME_PROFILE_FUNCTION();
// 
//         auto node = Node::object();
//         node["path"] = texture->getFilePath();
//         return node;
//       }
//       static bool decode(const Node& node, Texture2D::Handle& texture) {
//         GAME_PROFILE_FUNCTION();
// 
//         if (!node.isObject()) {
//           return false;
//         }
//         const auto& path = node.get("path");
//         if (!path || !path->isString()) {
//           return false;
//         }
//         // TODO: decode Texture2D specification
//         texture = Texture2D::load(*path->asString())
//           .build();
//         return true;
//       }
//     };
// 
//     template<>
//     struct Convert<Material::Handle> {
//       static Node encode(const Material::Handle& material) {
//         GAME_PROFILE_FUNCTION();
// 
//         auto node = Node::object();
//         node["diffuseMap"] = material->diffuseMap;
//         node["specularMap"] = material->specularMap;
//         node["emissionMap"] = material->emissionMap;
//         node["transparancy"] = material->transparency;
//         node["shininess"] = material->shininess;
//         return node;
//       }
//       static bool decode(const Node& node, Material::Handle& material) {
//         GAME_PROFILE_FUNCTION();
// 
//         if (!node.isObject()) {
//           return false;
//         }
//         material = Material::create();
//         if (const auto& texture = node.get("diffuseMap"); texture) {
//           if (!Convert<Texture2D::Handle>::decode(*texture, material->diffuseMap)) {
//             return false;
//           }
//         }
//         if (const auto& texture = node.get("specularMap"); texture) {
//           if (!Convert<Texture2D::Handle>::decode(*texture, material->specularMap)) {
//             return false;
//           }
//         }
//         if (const auto& texture = node.get("emissionMap"); texture) {
//           if (!Convert<Texture2D::Handle>::decode(*texture, material->emissionMap)) {
//             return false;
//           }
//         }
//         if (const auto& transparency = node.get("transparency"); transparency) {
//           if (transparency->isFloat()) {
//             material->transparency = (f32)*transparency->asFloat();
//           } else if (transparency->isInteger()) {
//             material->transparency = (f32)*transparency->asInteger();
//           } else {
//             return false;
//           }
//         }
//         if (const auto& shininess = node.get("shininess"); shininess) {
//           if (shininess->isFloat()) {
//             material->shininess = (f32) *shininess->asFloat();
//           } else if (shininess->isInteger()) {
//             material->shininess = (f32) *shininess->asInteger();
//           } else {
//             return false;
//           }
//         }
//         return true;
//       }
//     };


} // namespace Game