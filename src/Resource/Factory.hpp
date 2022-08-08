#include "Core/Type.hpp"

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

}