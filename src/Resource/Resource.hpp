#pragma once

#include "Core/Type.hpp"

namespace Game {
  
  class ResourceManager;

  class Resource {
    friend class ResourceManager;

  public:
    enum class Type : u32 {
      None,
      Texture,
      Shader,
      Mesh,
    };

  public:
    Resource() {}
    ~Resource();
    Resource(const Resource& other);
    Resource& operator=(const Resource& other);
    Resource(Resource&& other);
    Resource& operator=(Resource&& other);

    inline bool isValid() { return this->id.type != Type::None; }

    bool operator==(const Resource& other) { return this->idToU64() == other.idToU64(); }
    bool operator!=(const Resource& other) { return this->idToU64() != other.idToU64(); }

  protected:
    struct Id {
      Type type  = Type::None;
      u32  index = 0;
    };

    static_assert(sizeof(Id) == 8, "expected 64 identifier");

  protected:
    Resource(Id id) 
      : id{id}
    {}

    inline u64 idToU64() const { return *reinterpret_cast<const u64*>(&this->id); }

  protected:
    Id id;
  };

} // namespace Game
