#include "Utils/Memory/BumpAllocator.hpp"

#include <cstdlib>


namespace Gate::Utils {

  u8* alignUp(u8* stack, isize align) {
    GAME_DEBUG_ASSERT(align > 0 && (align & (align - 1)) == 0); /* Power of 2 */
    GAME_DEBUG_ASSERT(stack != nullptr);

    usize addr  = (usize)stack;
    addr = (addr + (align - 1)) & -align;
    GAME_DEBUG_ASSERT(addr >= (usize)stack);
    return (u8*)addr;
  }

  u8* alignDown(u8* stack, isize align) {
    GAME_DEBUG_ASSERT(align > 0 && (align & (align - 1)) == 0); /* Power of 2 */
    GAME_DEBUG_ASSERT(stack != 0);

    usize addr  = (usize)stack;
    addr &= -align;
    GAME_DEBUG_ASSERT(addr <= (usize)stack);
    return (u8*)addr;
  }


  BumpAllocator::BumpAllocator(usize size) {
    mBasePtr = (u8*)malloc(size);
    if (!mBasePtr) {
      Logger::error("BumpAllocator: couldn't allocate %zu amount of memory", size);
      exit(1);
    }
    mCurrentPtr = mBasePtr;
    mSize = size;
  }
  BumpAllocator::~BumpAllocator() {
    free(mBasePtr);
  }
  void* BumpAllocator::allocate(usize size, usize alignment) {
    mCurrentPtr = alignUp(mCurrentPtr, alignment);
    u8* result = mCurrentPtr;
    mCurrentPtr += size;
    if ((usize)mCurrentPtr - (usize)mBasePtr >= mSize) {
      return nullptr;
    }
    return result;
  }
  void BumpAllocator::clear() {
    mCurrentPtr = mBasePtr;
  }

}