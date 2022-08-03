#pragma once

#include "Resource/Texture.hpp"
#include "Resource/Shader.hpp"

namespace Game {
  
  class Material {
  public:
    enum class AlphaMode {
      Opaque,
    };

  public:
    Material() {}

    inline void setDiffuseMap(const Texture2D& texture) {  mDiffuseMap = texture; }
    inline void setSpecularMap(const Texture2D& texture) { mSpecularMap = texture; }
    inline void setEmissionMap(const Texture2D& texture) { mEmissionMap = texture; }
    inline void setShininess(f32 value) { mShininess = value; }
    inline void setAlphaMode(AlphaMode mode) { mAlphaMode = mode; }

    inline const Texture2D& getDiffuseMap()  const { return mDiffuseMap; }
    inline const Texture2D& getSpecularMap() const { return mSpecularMap; }
    inline const Texture2D& getEmissionMap() const { return mEmissionMap; }
    inline f32 getShininess() const { return mShininess; }
    inline AlphaMode getAlphaMode() const { return mAlphaMode; }

  private:
    Texture2D mDiffuseMap;
    Texture2D mSpecularMap;
    Texture2D mEmissionMap;
    f32       mShininess;
    AlphaMode mAlphaMode = AlphaMode::Opaque;

    friend class Renderer;
  };

} // namespace Game

