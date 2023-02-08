#pragma once

#include "Editor/Point.hpp"
#include "Renderer/Renderer2D.hpp"
#include "Renderer/Renderer3D.hpp"
#include "Serializer/Serializer.hpp"

namespace Gate {
  
  struct Wire {
    Point from;
    Point to;

    std::vector<u32> connectionIndexes{};

    bool active = false;
    bool visited = false;

    bool free = false;

    void render(Renderer2D& renderer);
    void render(Renderer3D& renderer);
  };

}

namespace Gate::Serializer {

  template<>
  struct Convert<Wire> {
    static Node encode(const Wire& value);
    static bool decode(const Node& node, Wire& value);
  };

}
