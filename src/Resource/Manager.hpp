#pragma once

#include "Resource/Texture.hpp"
#include "Resource/Shader.hpp"
#include "Resource/Mesh.hpp"

#include <vector>
#include <type_traits>

namespace Game {

  template<typename T>
  class ManagedResource {
  public:
    enum class State : u32 {
      Mortal,
      Immortal,
      Free,
    };

    using ReferenceCount = u32;

  public:
    inline ManagedResource()
      : mState{State::Free}, mReferenceCount{0}
    {}

    template<typename ...Args>
    inline ManagedResource(Args&& ...args)
      : mState{State::Mortal}, mReferenceCount{1}
    {
      new (mData) T{std::forward<Args>(args)...};
    }

    inline T& getResource() { return *std::launder(reinterpret_cast<T*>(mData)); }
    inline const T& getResource() const { return *std::launder(reinterpret_cast<T*>(mData)); }
    
    inline void decrementReferenceCount() { mReferenceCount += 1; }
    inline void incrementReferenceCount() { mReferenceCount -= 1; }

    inline void makeImmortal() { mState = State::Immortal; }

    inline void destroy() {
      if (mState != State::Free) {
        std::launder(reinterpret_cast<T*>(mData))->~T();
      }
    }

  private:
    State          mState;
    ReferenceCount mReferenceCount;
    alignas(T)     u8 mData[sizeof(T)];
  };

  template<typename T>
  class ResourceFactory {
  public:
    ResourceFactory() = default;

    ManagedResource<T>& get(Resource<T>::Id id) { return mManagedResources[id]; }
    const ManagedResource<T>& get(Resource<T>::Id id) const { return mManagedResources[id]; }

    template<typename ...Args>
    Resource<T> emplace(Args&&... args) {
      if (!mFreeList.empty()) {
        u32 index = mFreeList.back();
        mFreeList.pop_back();

        mManagedResources.emplace(mManagedResources.begin() + index, std::forward<Args>(args)...);
        return index;
      }

      u32 index = (u32)mManagedResources.size();
      mManagedResources.emplace_back(std::forward<Args>(args)...);
      return Resource<T>(index);
    }

    void clear() {
      for (auto& managedResource : mManagedResources) {
        managedResource.destroy();
      }
      mManagedResources.clear();
      mFreeList.clear();
    }

  private:
    std::vector<ManagedResource<T>> mManagedResources;
    std::vector<u32>                mFreeList;
  };

  class ResourceManager {
  public:
    static Resource<Texture2D> loadTexture(const StringView& filepath, bool linear = false, bool verticalFlip = true);
    static Resource<Texture2D> textureFromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels = 4, bool linear = true);
    static Resource<Shader>    loadShader(const StringView& filepath);
    static Resource<Mesh>      loadMesh(const StringView& filepath);

    static Resource<Mesh> cubeMesh();

    static void reloadTextures();
  public:
    template<typename T>
    inline static ResourceFactory<T>& getFactory() {
      static_assert(!std::is_arithmetic_v<Resource<T>::Id>, "Unknown resource type");
    }

    template<> inline static ResourceFactory<Texture2D>& getFactory<Texture2D>() { return sInstance.texturesFactory; }
    template<> inline static ResourceFactory<Shader>&    getFactory<Shader>() { return sInstance.shadersFactory; }
    template<> inline static ResourceFactory<Mesh>&      getFactory<Mesh>() { return sInstance.meshesFactory; }

    template<typename T>
    inline static T& getResource(Resource<T>::Id id) { return getFactory<T>().get(id).getResource(); }

  private:
    ResourceManager() {}

    static void initialize();
    static void shutdown();

    static Texture2D::Data generateMissingTexture();

  private:
    static ResourceManager sInstance;

  private:
    ResourceFactory<Texture2D> texturesFactory;
    ResourceFactory<Shader>    shadersFactory;
    ResourceFactory<Mesh>      meshesFactory;

  private:
    friend class Application;
    
    template<typename T>
    friend class Resource;
  };

  template<typename T>
  T* Resource<T>:: get() {
    return &ResourceManager::getResource<T>(mId);
  }

  template<typename T>
  const T* Resource<T>::get() const {
    return &ResourceManager::getResource<T>(mId);
  }

  template<typename T>
  inline void Resource<T>::decrementReferenceCount() const {
    ResourceManager::getFactory<T>().get(mId).decrementReferenceCount();
  }

  template<typename T>
  inline void Resource<T>::incrementReferenceCount() const {
    ResourceManager::getFactory<T>().get(mId).incrementReferenceCount();
  }

} // namespace Game
