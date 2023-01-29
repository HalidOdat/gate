#pragma once

#include "Core/Base.hpp"

namespace Gate {

  template<typename T>
  class Resource {
  public:
    using Id = u32;
    using ReferenceCount = u32;

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

    inline Resource& operator=(Resource&& other) {
      if (this == &other) {
        return *this;
      }

      if (isValid()) {
        decrementReferenceCount();
      }
      mId = other.mId;
      other.mId = InvalidId;
      return *this;
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

    inline T* get() { 
      GAME_DEBUG_ASSERT(isValid());
      return std::launder(reinterpret_cast<T*>(getRaw().data));
    }
    inline const T* get() const {
      GAME_DEBUG_ASSERT(isValid());
      return std::launder(reinterpret_cast<T*>(getRaw().data));
    }

    inline T* operator->() { return get(); }
    inline T& operator*() { return *get(); }
    inline const T* operator->() const { return get(); }
    inline const T& operator*() const { return *get(); }

    inline Id getId() const { return mId; }
    inline ReferenceCount getReferenceCount() const { return getRaw().referenceCount; }
    
  private:
    struct Data {
      alignas(T) u8 data[sizeof(T)] = {0};
      ReferenceCount referenceCount = 0;
    };
  
  private:
    Resource(Id id)
      : mId(id)
    {}

    Data& getRaw() const;
    void incrementReferenceCount() const { getRaw().referenceCount += 1; }
    void decrementReferenceCount() const;

  private:
    Id mId;

  private:
    template<typename U>
    friend class ResourceFactory;
  };

# define GAME_FACTORY_HEADER(T)                                 \
  template<> Resource<T>::Data& Resource<T>::getRaw() const;    \
  template<> void Resource<T>::decrementReferenceCount() const;

} // namespace Gate
