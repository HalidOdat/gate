#include "Renderer/Material.hpp"
#include "Serializer/Serializer.hpp"
#include "Utils/File.hpp"

namespace Gate {

  static std::unordered_map<String, Material::Handle> materials;

  Material::Handle Material::get(const String& name) {
    if (auto it = materials.find(name); it != materials.end()) {
      return it->second;
    }
    
    return nullptr;
  }

  Material::Handle Material::create(const String& name) {
    if (auto it = materials.find(name); it != materials.end()) {
      Logger::error("Material with name '%s' already exists", name.c_str());
      GATE_ASSERT_WITH_MESSAGE(false, "Attempting to create already existing material");
    }

    auto result = std::make_shared<Material>(name);
    materials[name] = result;
    return result;
  }

  Material::Handle Material::getOrCreate(const String& name) {
    if (auto it = materials.find(name); it != materials.end()) {
      return it->second;
    }

    auto result = std::make_shared<Material>(name);
    materials[name] = result;
    return result;
  }

  bool Material::initialize() {
    auto filePath = String("assets/3D/materials.json");
    auto source = Utils::fileToString(filePath);
    if (!source) {
      Logger::error("Material serializer: Couldn't open materials file: %s", filePath.c_str());
      return false;
    }

    const auto node = Serializer::Json::parse(source);
    free(source);
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
        material->diffuseMap = Texture::load(*path->asString())
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
        material->specularMap = Texture::load(*path->asString())
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
        material->emissionMap = Texture::load(*path->asString())
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

  void Material::destroyAll() {
    materials.clear();
  }
} // namespace Game

