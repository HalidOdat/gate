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
    enum class State : u8 {
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
    
    // TODO: destruct if reference count is 0
    inline void decrementReferenceCount() { mReferenceCount -= 1; }
    inline void incrementReferenceCount() { mReferenceCount += 1; }

    inline void makeImmortal() { mState = State::Immortal; }

    inline void destroy() {
      if (mState != State::Free) {
        std::launder(reinterpret_cast<T*>(mData))->~T();
        mState = State::Free;
      }
    }


    template<typename ...Args>
    inline void emplace(Args&& ...args) {
      destroy();
      new (mData) T{std::forward<Args>(args)...};
    }

    inline bool isFree() const { return mState == State::Free; }
    
  private:
    State          mState;
    ReferenceCount mReferenceCount;
    alignas(T)     u8 mData[sizeof(T)];

  private:

    template<typename T>
    friend class ResourceFactory;
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

    bool reload(Resource<T>& resource) {
      if (!resource) {
        return false;
      }

      return reloadById(resource.mId);
    }

    void reloadAll() {
      for (u32 i = 0; i < mManagedResources.size(); ++i) {
        if (mManagedResources[i].isFree()) {
          continue;
        }

        reloadById(i);
      }
    }
  
  private:
    bool reloadById(Resource<T>::Id id) {
      return mManagedResources[id].getResource().reload();
    }

  private:
    std::vector<ManagedResource<T>> mManagedResources;
    std::vector<u32>                mFreeList;
  
  private:

    friend class ResourceManager;
  };

  class ResourceManager {
  public:
    static Texture2D::Handle loadTexture(const StringView& filepath, Texture2D::Specification specification = {});
    static Texture2D::Handle textureFromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels = 4, Texture2D::Specification specification = {});
    static Shader::Handle    loadShader(const StringView& filepath);
    static Mesh::Handle      loadMesh(const StringView& filepath);
    static CubeMap::Handle   loadCubeMap(const CubeMap::FilePaths& filepaths);

    static Mesh::Handle cubeMesh();

    template<typename T> inline static bool reload(Resource<T>& resource) { return getFactory<T>().reload(resource); }
    template<typename T> inline static void reloadAll() { getFactory<T>().reloadAll(); }

  public:
    template<typename T>
    inline static ResourceFactory<T>& getFactory() {
      static_assert(!std::is_arithmetic_v<Resource<T>::Id>, "Unknown resource type");
    }

    template<> inline static ResourceFactory<Texture2D>& getFactory<Texture2D>() { return sInstance.texturesFactory; }
    template<> inline static ResourceFactory<Shader>&    getFactory<Shader>() { return sInstance.shadersFactory; }
    template<> inline static ResourceFactory<Mesh>&      getFactory<Mesh>() { return sInstance.meshesFactory; }
    template<> inline static ResourceFactory<CubeMap>&   getFactory<CubeMap>() { return sInstance.cubeMapsFactory; }

    template<typename T>
    inline static T& getResource(Resource<T>::Id id) { return getFactory<T>().get(id).getResource(); }

  private:
    ResourceManager() {}

    static void initialize();
    static void shutdown();

  private:
    static ResourceManager sInstance;

  private:
    ResourceFactory<Texture2D> texturesFactory;
    ResourceFactory<Shader>    shadersFactory;
    ResourceFactory<Mesh>      meshesFactory;
    ResourceFactory<CubeMap>   cubeMapsFactory;

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
