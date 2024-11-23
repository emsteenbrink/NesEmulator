#pragma once

#include "Mapper.h"
#include "iNesConfiguration.h"

#include <memory>
#include <cstdint>
#include <filesystem>

class MapperFactory
{
public:
  static std::shared_ptr<Mapper> CreateMapper(uint16_t mapperId, iNesConfiguration& configuration, const std::filesystem::path& filePath);
};