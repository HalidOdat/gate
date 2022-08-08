#pragma once

#include "Resource/Texture.hpp"
#include "Resource/Shader.hpp"
#include "Resource/Mesh.hpp"
#include "Resource/Factory.hpp"

namespace Game {

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
