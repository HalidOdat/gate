#pragma once

namespace Gear {

  class Logger {
  public:
    static void info(const char* format, ...);
    static void warn(const char* format, ...);
    static void error(const char* format, ...);
  };

} // namespace Gear