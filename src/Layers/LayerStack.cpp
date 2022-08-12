#include "LayerStack.hpp"

#include "Core/Base.hpp"

namespace Game {
  
  LayerStack::~LayerStack() {
    this->clear();
  }

  void LayerStack::push(Layer* layer) {
    GAME_PROFILE_FUNCTION();
    layer->onAttach();
    this->layers.push_back(layer);
  }

  void LayerStack::pop() {
    GAME_PROFILE_FUNCTION();
    this->layers.pop_back();
  }

  void LayerStack::onUpdate(Timestep ts) {
    GAME_PROFILE_FUNCTION();
    for (auto it = this->begin(); it != this->end(); ++it) {
      (*it)->onUpdate(ts);
    }
  }

  void LayerStack::onUiRender(Ui& ui) {
    GAME_PROFILE_FUNCTION();
    for (auto it = this->begin(); it != this->end(); ++it) {
      (*it)->onUiRender(ui);
    }
  }

  void LayerStack::onEvent(const Event& event) {
    GAME_PROFILE_FUNCTION();
    for (auto it = this->rbegin(); it != this->rend(); ++it) {
      if (event.isHandled()) {
        break;
      }
      (*it)->onEvent(event);
    }
  }

  void LayerStack::clear() {
    GAME_PROFILE_FUNCTION();
    for (auto it = this->rbegin(); it != this->rend(); ++it) {
      Layer* layer = *it;
      layer->onDetach();
      delete layer;
    }
    this->layers.clear();
  }

} // namespace Game
