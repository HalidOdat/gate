#pragma once

#include "Core/Macro.hpp"

namespace Gate {

  class Logger {
  public:
#ifdef GATE_DEBUG_MODE
    static void trace(const char *format, ...);
    static void info(const char* format, ...);
#else
    static void trace(const char *format, ...) { (void)format; }
    static void info(const char* format, ...) { (void)format; }
#endif
    static void warn(const char* format, ...);
    static void error(const char* format, ...);
  };

} // namespace Gate