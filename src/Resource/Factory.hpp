#include "Core/Type.hpp"
#include "Resource/Resource.hpp"

#include <vector>
#include <type_traits>

namespace Game {

  template<typename T>
  struct FactoryCallback {
    inline static void created(T&, u32) {}
    inline static void destroyed(T&, u32) {}
  };

  template<typename T>
  class ResourceFactory {
  private:
    using Data = Resource<T>::Data;
  public:
    ResourceFactory() = default;

    Resource<T>::Data& get(Resource<T>::Id id) { return mResources[id]; }

    template<typename ...Args>
    Resource<T> emplace(Args&&... args) {
      if (!mFreeList.empty()) {
        u32 index = mFreeList.back();
        mFreeList.pop_back();

        emplaceAtIndex(index, std::forward<Args>(args)...);
        return index;
      }

      u32 index = (u32)mResources.size();
      mResources.emplace_back();

      emplaceAtIndex(index, std::forward<Args>(args)...);
      return Resource<T>(index);
    }

    void clear() {
      for (u32 i = 0; i < mResources.size(); ++i) {
        if (mResources[i].referenceCount == 0) {
          continue;
        }

        destroyAtIndex(i);
      }
      mResources.clear();
      mFreeList.clear();
    }

    void decrementReferenceCountAtIndex(Resource<T>::Id id) {
      if (mResources[id].referenceCount == 1) {
        destroyAtIndex(id);
        mFreeList.emplace_back(id);
      }
      mResources[id].referenceCount--;
    }

  private:
    inline bool isFree(Resource<T>::Id id) const { return !mResources[id].referenceCount; }

    inline void destroyAtIndex(Resource<T>::Id id) {
      T* ptr = std::launder(reinterpret_cast<T*>(mResources[id].data));
      FactoryCallback<T>::destroyed(*ptr, id);
      ptr->~T();
      mResources[id].referenceCount = 0;
    }

    template<typename ...Args>
    inline void emplaceAtIndex(Resource<T>::Id id, Args&& ...args) {
      mResources[id].referenceCount = 1;
      new (mResources[id].data) T{std::forward<Args>(args)...};
      T* ptr = std::launder(reinterpret_cast<T*>(mResources[id].data));
      FactoryCallback<T>::created(*ptr, id);
    }

  private:
    std::vector<Data> mResources;
    std::vector<u32>  mFreeList;

  private:
    friend class ResourceManager;
  };

# define GAME_FACTORY_IMPLEMENTATION(T, name)                    \
  static ResourceFactory<T> name;                                \
  template<> Resource<T>::Data& Resource<T>::getRaw() const {    \
    return name.get(mId);                                        \
  }                                                              \
  template<> void Resource<T>::decrementReferenceCount() const { \
    name.decrementReferenceCountAtIndex(mId);                    \
  }

}