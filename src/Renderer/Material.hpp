#pragma once

#include "Resource/Texture.hpp"
#include "Resource/Resource.hpp"

namespace Game {
  
  class Material {
  public:
    using Handle = Resource<Material>;

    enum class AlphaMode : u8 {
      Opaque,
    };

  public:
    Material() {}
    static Material::Handle create();


  public:
    Texture2D::Handle diffuseMap;
    Texture2D::Handle specularMap;
    Texture2D::Handle emissionMap;
    AlphaMode         alphaMode    = AlphaMode::Opaque;
    f32               shininess    = 32.0f;
    f32               transparency = 1.0f;
  };

  GAME_FACTORY_HEADER(Material)

} // namespace Game

