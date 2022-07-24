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
    };

  public:
    Resource() {}
    ~Resource();
    Resource(const Resource& other);
    Resource& operator=(const Resource& other);
    Resource(Resource&& other);
    Resource& operator=(Resource&& other);

    inline bool isValid() { return this->id.type != Type::None; }

  protected:
    struct Id {
      Type type  = Type::None;
      u32  index = 0;  
    };

  protected:
    Resource(Id id) 
      : id{id}
    {}

  protected:
    Id id;
  };

} // namespace Game
