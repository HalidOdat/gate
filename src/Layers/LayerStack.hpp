#pragma once

#include <vector>

#include "Layer.hpp"

namespace Game {

  class LayerStack {
  private:
    using Stack          = std::vector<Layer*>;

  public:
    using reference              = Stack::reference;
    using pointer                = Stack::pointer;
    using const_reference        = Stack::const_reference;
    using const_pointer          = Stack::const_pointer;

    using iterator               = Stack::iterator;
    using reverse_iterator       = Stack::reverse_iterator;
    using const_iterator         = Stack::const_iterator;
    using const_reverse_iterator = Stack::const_reverse_iterator;

  public:
    LayerStack() = default;
    DISALLOW_COPY_AND_ASSIGN(LayerStack);
    ~LayerStack();

    void push(Layer* layer);
    void pop();

    void onUpdate(); 
    void onUiRender(Ui& ui); 
    void onEvent(const Event& event);

    void clear();

    inline iterator begin()                       { return layers.begin(); }
    inline iterator end()                         { return layers.end(); }
    inline reverse_iterator rbegin()              { return layers.rbegin(); }
    inline reverse_iterator rend()                { return layers.rend(); }
    inline const_iterator cbegin()          const { return layers.cbegin(); }
    inline const_iterator cend()            const { return layers.cend(); }
    inline const_reverse_iterator crbegin() const { return layers.crbegin(); }
    inline const_reverse_iterator crend()   const { return layers.crend(); }

  private:
    Stack layers;
  };

} // namespace Game