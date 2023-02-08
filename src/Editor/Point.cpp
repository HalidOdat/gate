#include "Editor/Point.hpp"

namespace Gate::Serializer {

  Node Convert<Point>::encode(const Point& point) {
    auto node = Node::array();
    node.push(point.x);
    node.push(point.y);
    node.setStyle(Node::Style::Flat);
    return node;
  }
  bool Convert<Point>::decode(const Node& node, Point& value) {
    auto* array = node.asArray();
    if (!array) return false;
    if (array->size() != 2) {
      return false;
    }
    u32 x, y;
    if (!Convert<u32>::decode(array->at(0), x)) return false;
    if (!Convert<u32>::decode(array->at(1), y)) return false;

    value.x = x;
    value.y = y;
    return true;
  }

}
