#pragma once

#include "Resource/Texture.hpp"
#include "Resource/Shader.hpp"

namespace Game {

  class ResourceManager {
  public:
    // static Texture::Handle loadTexture(const StringView& filepath, Texture::Specification specification = {});
    // static Texture::Handle textureFromBytes(const u8 bytes[], const u32 width, const u32 height, const u32 channels = 4, Texture::Specification specification = {});
    // static Shader::Handle    loadShader(const StringView& filepath);
    // static Mesh::Handle      loadMesh(const StringView& filepath);
    // static CubeMap::Handle   loadCubeMap(const CubeMap::FilePaths& filepaths);

    // static Mesh::Handle cubeMesh();

  private:
    ResourceManager() = delete;

    static void initialize();
    static void shutdown();

  private:
    friend class Application;
  };

} // namespace Game
