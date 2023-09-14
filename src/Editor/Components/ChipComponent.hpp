#pragma once

#include "Core/Base.hpp"
#include "Editor/Point.hpp"
#include "Renderer/Renderer2D.hpp"

#include "Editor/Components/Component.hpp"

#include "Editor/Chip.hpp"

namespace Gate {
 
  class SwitchComponent;
  class OutputComponent;
  class Chip;

  class ChipComponent : public Component {
  public:
    ChipComponent(Point position, Ref<Chip> chip);
    virtual bool update() override;
    virtual void renderBody(Renderer2D& renderer) override;
    virtual void renderBody(Renderer3D& renderer, u32 id) override;
    virtual void renderConnectors(Renderer3D&, [[maybe_unused]] u32 id) override {}

    virtual Serializer::Node encode() const override;
  
  private:
    Ref<Chip> mChip = nullptr;

    std::vector<SwitchComponent*> mChipInputs;
    std::vector<OutputComponent*> mChipOutputs;
  };

}
