#pragma once

#include "Resource/Manager.hpp"

namespace Game {

  Resource::Resource(const Resource& other)
    : id{other.id}
  {
    ResourceManager::incrementRefCount(other.id);
  }

  Resource::~Resource() {
    ResourceManager::decrementRefCount(this->id);
    this->id.type = Type::None;
  }

  Resource& Resource::operator=(const Resource& other) {
    if (this == &other) {
      return *this;
    }

    ResourceManager::decrementRefCount(this->id);
    ResourceManager::incrementRefCount(other.id);
    this->id = id;

    return *this;
  }

  Resource::Resource(Resource&& other)
    : id{other.id}
  {
    other.id.type = Type::None;
  }

  Resource& Resource::operator=(Resource&& other) {
    if (this == &other) {
      return *this;
    }

    ResourceManager::decrementRefCount(this->id);
    this->id = other.id;
    other.id.type = Type::None;
    return *this;
  }

} // namespace Game
