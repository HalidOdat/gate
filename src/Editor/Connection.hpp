#include "Core/Base.hpp"

namespace Gate {
  
  struct Connection {
    enum Type {
      Component,
      Wire,
    };

    Type type;
    u32 index;
    u32 componentIndex = UINT32_MAX;
  };

  struct ConnectionResult {
    bool successful;
    u32  connectionIndex;

    ConnectionResult()
      : successful{false}, connectionIndex{UINT32_MAX}
    {}
    ConnectionResult(u32 index)
      : successful{true}, connectionIndex{index}
    {}
  };

  struct ConnectionState {
    bool active;
    bool visited;

    ConnectionState(bool active_, bool visited_)
      : active{active_}, visited{visited_}
    {}
  };

}