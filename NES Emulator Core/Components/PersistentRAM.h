#pragma once

#include "RAM.h"

#include <array>
#include <filesystem>

namespace fs = std::filesystem;

class PersistentRAM : public RAM
{
public:
  PersistentRAM(unsigned int size, uint16_t beginAddress, uint16_t endAddress, const fs::path& filePath);

  void Save();
  void Load();

private:
  const fs::path m_filePath;
};
