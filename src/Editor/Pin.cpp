#include "Editor/Pin.hpp"

#include "Editor/Config.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Gate {

  void Pin::render(Renderer2D& renderer, bool isOutput) {
    renderer.drawCenteredCircle(
      position.toVec2() * (f32)config.grid.cell.size,
      (isOutput ? config.component.output.size : config.component.input.size) * config.grid.cell.size,
      isOutput ? config.component.output.color : config.component.input.color
    );
    renderer.drawCenteredCircle(
      position.toVec2() * (f32)config.grid.cell.size,
      (isOutput ? config.component.output.size : config.component.input.size) * config.grid.cell.size,
      Color::BLUE,
      0.2f, // thickness
      0.001f
    );
  }

  void Pin::render(Renderer3D& renderer, bool isOutput, u32 id) {
    (void)isOutput;
    Material::Handle material = config.inactiveMaterial;
    if (active) {
      material = config.activeMaterial;
    }

    Mat4 model{1.0f};
    model = glm::translate(model, (position.toVec3() * config.grid.cell.size3d) * Vec3{1.0f, -1.0f, 1.0f});
    model = glm::scale(model, Vec3{config.grid.cell.size3d * 0.5f});

    renderer.submit(config.pinMesh, material, model, id);
  }

}