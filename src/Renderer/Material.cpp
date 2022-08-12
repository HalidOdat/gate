#include "Renderer/Material.hpp"
#include "Resource/Factory.hpp"

namespace Game {

  GAME_FACTORY_IMPLEMENTATION(Material, factory)

  Material::Handle Material::create() {
    return factory.emplace();
  }

} // namespace Game