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
    return false;
  }

}
