#pragma once

#include <cstddef>

// Файлы ресурсов, преобразованные CMake в массивы байтов автономной сборки.
namespace EmbeddedResources {
struct EmbeddedResource {
  const unsigned char *data;
  std::size_t size;
};

extern const EmbeddedResource LEVEL;
extern const EmbeddedResource TILESET;
extern const EmbeddedResource FONT;
} // namespace EmbeddedResources
