#pragma once

#include "Resource/Texture.hpp"

namespace Game {
  
  class Material {
  public:
    using Handle = Ref<Material>;

    enum class AlphaMode : u8 {
      Opaque,
    };

  public:
    Material() {}

  public:
    Texture2D::Handle diffuseMap;
    Texture2D::Handle specularMap;
    Texture2D::Handle emissionMap;
    AlphaMode         alphaMode    = AlphaMode::Opaque;
    f32               shininess    = 32.0f;
    f32               transparency = 1.0f;
  };

} // namespace Game

