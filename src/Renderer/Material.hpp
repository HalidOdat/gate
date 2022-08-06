#pragma once

#include "Resource/Manager.hpp"

namespace Game {
  
  class Material {
  public:
    using Handle = Ref<Material>;

    enum class AlphaMode {
      Opaque,
    };

  public:
    Material() {}

    inline void setDiffuseMap(const Texture2D::Handle& texture) {  mDiffuseMap = texture; }
    inline void setSpecularMap(const Texture2D::Handle& texture) { mSpecularMap = texture; }
    inline void setEmissionMap(const Texture2D::Handle& texture) { mEmissionMap = texture; }
    inline void setShininess(f32 value) { mShininess = value; }
    inline void setTransparecy(f32 value) { mTransparency = value; }
    inline void setAlphaMode(AlphaMode mode) { mAlphaMode = mode; }

    inline const Texture2D::Handle& getDiffuseMap()  const { return mDiffuseMap; }
    inline const Texture2D::Handle& getSpecularMap() const { return mSpecularMap; }
    inline const Texture2D::Handle& getEmissionMap() const { return mEmissionMap; }
    inline f32& getShininess() { return mShininess; }
    inline f32& getTransparency() { return mTransparency; }
    inline const f32& getShininess() const { return mShininess; }
    inline const f32& getTransparency() const { return mTransparency; }
    inline AlphaMode getAlphaMode() const { return mAlphaMode; }

    inline bool hasTransparency() const { return mTransparency < 1.0f; }

  private:
    Texture2D::Handle mDiffuseMap;
    Texture2D::Handle mSpecularMap;
    Texture2D::Handle mEmissionMap;
    f32       mShininess;
    f32       mTransparency = 1.0f;
    AlphaMode mAlphaMode = AlphaMode::Opaque;

    friend class Renderer;
  };

} // namespace Game

