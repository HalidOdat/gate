#include "Editor/Config.hpp"

namespace Gate {

  // Default construct it
  Config config{};

  void Config::apply(Theme::Settings settings) {
    grid.background = rgba(settings.board.background);
    grid.color = rgba(settings.board.points);

    wire.activeColor = rgba(settings.wire.active);
    wire.inactiveColor = rgba(settings.wire.inactive);
    wire.invalidColor = rgba(settings.wire.invalid);

    text.color = rgba(settings.text.color);
  }

}
