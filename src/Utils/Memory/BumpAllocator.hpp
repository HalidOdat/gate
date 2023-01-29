#pragma once

#include "Core/Base.hpp"

namespace Gate::Utils {

  u8* alignUp(u8* stack, isize align);
  u8* alignDown(u8* stack, isize align);

  class BumpAllocator {
  public:
    BumpAllocator(usize size);
	  ~BumpAllocator();

    void* allocate(usize size, usize alignment);
    void clear();
   private:
     u8* mCurrentPtr;
     u8* mBasePtr;
     usize mSize;
   };
}