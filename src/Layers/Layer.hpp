#pragma once

#include "Core/Type.hpp"
#include "Events/Event.hpp"

#include "REnderer/Camera.hpp"

namespace Game {
  
  class Layer {
  public:
    virtual ~Layer() = default;
    DISALLOW_MOVE_AND_COPY(Layer);
    
    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate() {}
    virtual void onEvent(const Event& event) {}

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
