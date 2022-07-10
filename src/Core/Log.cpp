#include <cstdarg>
#include <cstdio>

#include "Core/Log.hpp"

namespace Game {

  void Logger::trace(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stdout, "[Trace ]: ");
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
    va_end(args);
  }

  void Logger::info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stdout, "[ Info ]: ");
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
    va_end(args);
  }

  void Logger::warn(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stdout, "[ Warn ]: ");
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
    va_end(args);
  }

  void Logger::error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stdout, "[Error]: ");
    vfprintf(stdout, format, args);
    fprintf(stdout, "\n");
    va_end(args);
  }

} // namespace Game