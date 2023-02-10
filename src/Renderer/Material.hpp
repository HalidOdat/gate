#pragma once

#include "Renderer/Texture.hpp"

#include <functional>

namespace Gate {
  
  class Material {
    friend class Application;
  public:
    using Handle = std::shared_ptr<Material>;

  public:
    static Material::Handle get(const String& name);
    static Material::Handle create(const String& name);
    static Material::Handle getOrCreate(const String& name);
    DISALLOW_MOVE_AND_COPY(Material);

  public:
    const String name;
    Texture::Handle diffuseMap;
    Texture::Handle specularMap;
    f32             shininess = 32.0f;

  private:
    static bool initialize();
    static void destroyAll();

  public:
    Material(const String& name)
      : name(name)
    {}
  };

} // namespace Gate

namespace std {
  template<>
  struct hash<Gate::Material::Handle> {
    size_t operator()(Gate::Material::Handle const& handle) const noexcept {
      return hash<Gate::usize>{}((Gate::usize)handle.get());
    }
  };
}
