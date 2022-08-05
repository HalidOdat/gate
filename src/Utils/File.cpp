#include "Utils/File.hpp"

#include <fstream>
#include <sstream>

namespace Game {

  namespace Utils {
    
      Option<String> fileToString(const StringView& filename) {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try {
          file.open(filename.data());
          std::stringstream stream;
          stream << file.rdbuf();
          file.close();
          return stream.str();
        } catch (std::ifstream::failure e) {
          return std::nullopt;
        }
      }

  } // namespace Utils
  

} // namespace Game