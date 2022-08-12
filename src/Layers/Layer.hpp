#pragma once

#include "Core/Timestep.hpp"
#include "Ui/Ui.hpp"

namespace Game {
  
  class Layer {
  public:
    virtual ~Layer() = default;
    DISALLOW_MOVE_AND_COPY(Layer);
    
    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate([[maybe_unused]] Timestep ts) {}
    virtual void onUiRender([[maybe_unused]] Ui& ui) {}
    virtual void onEvent([[maybe_unused]] const Event& event) {}

    inline bool isEnabled() const { return this->enabled; }
    inline void enable(const bool yes = true) {
      this->enabled = yes;
    }

  protected:
    Layer() = default;
  
  private:
    bool enabled = true;
  };

} // namespace Game
