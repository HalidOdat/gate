#pragma once

#include "Core/Base.hpp"
#include "Events/Event.hpp"

#include <vector>

namespace Gate {

  class FileDropEvent : public Event {
  public:
    inline static constexpr const auto TYPE = Event::Type::FileDropEvent;

  public:
    FileDropEvent(std::vector<String> paths)
      : Event(TYPE), mPaths{ paths }
    {}

    inline const std::vector<String>& getPaths() const { return mPaths; }

  private:
    std::vector<String> mPaths;
  };

} // namespace Gate