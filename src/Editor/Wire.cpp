#include "Editor/Wire.hpp"
#include "Editor/Config.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Gate {
  
  void Wire::render(Renderer2D& renderer) {
    f32 width = config.wire.width;

    Vec2 size = (to.toVec2() - from.toVec2()) * (f32)config.grid.cell.size;
    if (from.x == to.x) {
      size.x  = width;
      size.y += width;
    } else {
      size.x += width;
      size.y  = width;
    }

    auto effect = Effect::Type::None;
    Vec4 color = config.wire.inactiveColor;
    if (active) {
      color = config.wire.activeColor;
      effect = Effect::Type::Striped;
    }

    if (!visited) {
      color = config.wire.invalidColor;
      effect = Effect::Type::Static;
    }

    renderer.drawQuad(
      from.toVec2() * (f32)config.grid.cell.size - Vec2(width / 2.0f),
      size,
      color,
      effect
    );
    renderer.drawCenteredQuad(
      from.toVec2() * (f32)config.grid.cell.size,
      config.wire.endsSize,
      color,
      effect
    );
    renderer.drawCenteredQuad(
      to.toVec2() * (f32)config.grid.cell.size,
      config.wire.endsSize,
      color,
      effect
    );
  }


  void Wire::render(Renderer3D& renderer) {
    f32 wireWidth    = 0.25f;
    f32 gridCellSize = config.grid.cell.size3d;

    // TODO: move to constructor
    Vec3 scale = Vec3{wireWidth};
    Vec3 offset = Vec3{0, 0, 0};
    i32 cellDistance = 1;
    if (from.x == to.x && from.y == to.y) {
      if (from.z > to.z) {
        i32 temp = from.z;
        from.z = to.z;
        to.z = temp;
      }
      cellDistance = to.z - from.z;
      offset.z = cellDistance * gridCellSize / 2.0f;
      scale.z = cellDistance * gridCellSize;
    } else if (from.y == to.y && from.z == to.z) {
      if (from.x > to.x) {
        i32 temp = from.x;
        from.x = to.x;
        to.x = temp;
      }
      cellDistance = to.x - from.x;
      offset.x = cellDistance * gridCellSize / 2.0f;
      scale.x = cellDistance * gridCellSize;
    } else if (from.z == to.z && from.x == to.x) {
      if (from.y > to.y) {
        i32 temp = from.y;
        from.y = to.y;
        to.y = temp;
      }
      cellDistance = to.y - from.y;
      offset.y = cellDistance * gridCellSize / 2.0f;
      scale.y = cellDistance * gridCellSize;
    }

    Material::Handle material = config.inactiveMaterial;
    if (active) {
      material = config.activeMaterial;
    }
    
    Mat4 model{1.0f};
    model = glm::translate(model, (from.toVec3() * Vec3{gridCellSize} + offset) * Vec3{1.0f, -1.0f, 1.0f});
    model = glm::scale(model, scale);

    renderer.submit(config.pinMesh, material, model);
  }

}