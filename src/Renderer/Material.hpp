#pragma once

#include "Resource/Texture.hpp"
#include "Resource/Resource.hpp"

#include <unordered_map>

namespace Game {
  
  class Material {
  public:
    using Handle = Resource<Material>;

    enum class AlphaMode : u8 {
      Opaque,
    };

  public:
    static Material::Handle get(const String& name);
    static Material::Handle create(const String& name);
    static Material::Handle getOrCreate(const String& name);
    DISALLOW_MOVE_AND_COPY(Material);

  public:
    const String name;
    Texture2D::Handle diffuseMap;
    Texture2D::Handle specularMap;
    Texture2D::Handle emissionMap;
    AlphaMode         alphaMode    = AlphaMode::Opaque;
    f32               shininess    = 32.0f;
    f32               transparency = 1.0f;

  private:
    static bool initialize();
    static void destroy();

  private:
    Material(const String& name)
      : name(name)
    {}

  private:
    template<typename T>
    friend class ResourceFactory;
    friend class ResourceManager;
  };

  GAME_FACTORY_HEADER(Material)

} // namespace Game

namespace std {
  template<>
  struct hash<Game::Material::Handle> {
    size_t operator()(Game::Material::Handle const& handle) const noexcept {
      return hash<Game::u32>{}(handle.getId());
    }
  };
}
