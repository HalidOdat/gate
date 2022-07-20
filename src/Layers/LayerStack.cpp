#include "LayerStack.hpp"

namespace Game {
  
  LayerStack::~LayerStack() {
    this->clear();
  }

  void LayerStack::push(Layer* layer) {
    layer->onAttach();
    this->layers.push_back(layer);
  }

  void LayerStack::pop() {
    this->layers.pop_back();
  }

  void LayerStack::onUpdate() {
    for (auto it = this->begin(); it != this->end(); ++it) {
      (*it)->onUpdate();
    }
  }

  void LayerStack::onUiRender(Ui& ui) {
    for (auto it = this->begin(); it != this->end(); ++it) {
      (*it)->onUiRender(ui);
    }
  }

  void LayerStack::onEvent(const Event& event) {
    for (auto it = this->rbegin(); it != this->rend(); ++it) {
      if (event.isHandled()) {
        break;
      }
      (*it)->onEvent(event);
    }
  }

  void LayerStack::clear() {
    for (auto it = this->rbegin(); it != this->rend(); ++it) {
      Layer* layer = *it;
      layer->onDetach();
      delete layer;
    }
    this->layers.clear();
  }

} // namespace Game
