#include "Utils/File.hpp"

#include <cstdio>
#include <cstring>
#include <cerrno>

namespace Gate::Utils {

   char* fileToString(const StringView& filename) {
    FILE* f = fopen(filename.data(), "r");
    if (!f) {
      Logger::error("Couldn't open file '%s': %s", strerror(errno));
      return NULL;
    }

    fseek(f, 0, SEEK_END);
    usize length = ftell(f);
    fseek(f, 0, SEEK_SET);
    usize size = sizeof(char) * (length + 1);
    char* buffer = (char*)malloc(size);
    if (!buffer) {
      Logger::error("Couldn't allocate %zu for file '%s'", size, filename.data());
      fclose(f);
      return NULL;
    }
    fread(buffer, sizeof(char), length, f);
    fclose(f);
    buffer[length] = '\0';
    return buffer;
  }

} // namespace Gate::Utils