#pragma once

#include "Core/Type.hpp"

namespace Game {
  
  class ResourceManager;

  template<typename T>
  class Resource {
  public:
    using Id = u32;
  
  public:
    static constexpr const Id InvalidId = UINT32_MAX;

  public:
    inline Resource()
      : mId(InvalidId)
    {}

    inline Resource(const Resource& other)
      : mId(other.mId)
    {
      if (other.isValid()) {
        other.incrementReferenceCount();
      }
    }

    inline Resource& operator=(const Resource& other) {
      if (this == &other) {
        return *this;
      }

      if (isValid()) {
        decrementReferenceCount();
      }
      if (other.isValid()) {
        other.incrementReferenceCount();
      }
      mId = other.mId;
      return *this;
    }

    inline Resource(Resource&& other)
      : mId(other.mId)
    {
      other.mId = InvalidId;
    }

    inline Resource& operator=(const Resource&& other) {
      if (this == &other) {
        return *this;
      }

      if (isValid()) {
        decrementReferenceCount();
      }
      mId = other.mId;
      other.mId = InvalidId;
    }

    inline ~Resource() {
      if (isValid()) {
        decrementReferenceCount();
      }
    }

    inline bool isValid() const { return mId != InvalidId; }
    inline operator bool() const { return isValid(); }

    inline bool operator==(const Resource& other) const { return mId == other.mId; }
    inline bool operator!=(const Resource& other) const { return mId != other.mId; }

    inline T* get();
    inline const T* get() const;

    inline T* operator->() { return get(); }
    inline T& operator*() { return *get(); }
    inline const T* operator->() const { return get(); }
    inline const T& operator*() const { return *get(); }
    
  private:
    Resource(Id id)
      : mId(id)
    {}

    inline void decrementReferenceCount() const;
    inline void incrementReferenceCount() const;

  private:
    Id mId;

    template<typename T>
    friend class ResourceFactory;
  };

} // namespace Game
