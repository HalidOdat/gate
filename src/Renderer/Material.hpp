#pragma once

#include "Resource/Manager.hpp"

namespace Game {
  
  class Material {
  public:
    enum class AlphaMode {
      Opaque,
    };

  public:
    Material() {}

    inline void setDiffuseMap(const Resource<Texture2D>& texture) {  mDiffuseMap = texture; }
    inline void setSpecularMap(const Resource<Texture2D>& texture) { mSpecularMap = texture; }
    inline void setEmissionMap(const Resource<Texture2D>& texture) { mEmissionMap = texture; }
    inline void setShininess(f32 value) { mShininess = value; }
    inline void setTransparecy(f32 value) { mTransparency = value; }
    inline void setAlphaMode(AlphaMode mode) { mAlphaMode = mode; }

    inline const Resource<Texture2D>& getDiffuseMap()  const { return mDiffuseMap; }
    inline const Resource<Texture2D>& getSpecularMap() const { return mSpecularMap; }
    inline const Resource<Texture2D>& getEmissionMap() const { return mEmissionMap; }
    inline f32& getShininess() { return mShininess; }
    inline f32& getTransparency() { return mTransparency; }
    inline const f32& getShininess() const { return mShininess; }
    inline const f32& getTransparency() const { return mTransparency; }
    inline AlphaMode getAlphaMode() const { return mAlphaMode; }

    inline bool hasTransparency() const { return mTransparency < 1.0f; }

  private:
    Resource<Texture2D> mDiffuseMap;
    Resource<Texture2D> mSpecularMap;
    Resource<Texture2D> mEmissionMap;
    f32       mShininess;
    f32       mTransparency = 1.0f;
    AlphaMode mAlphaMode = AlphaMode::Opaque;

    friend class Renderer;
  };

} // namespace Game

