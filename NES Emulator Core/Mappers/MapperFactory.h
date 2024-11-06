#pragma once

#include "Mapper.h"
#include "iNesConfiguration.h"

#include <memory>
#include <cstdint>
#include <filesystem>

namespace fs = std::filesystem;

class MapperFactory
{
public:
  static std::shared_ptr<Mapper> CreateMapper(uint16_t mapperId, iNesConfiguration& configuration, const fs::path& filePath);
};