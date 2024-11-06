#pragma once

#include <cstdint>
#include <array>

struct Color
{
  uint8_t R;
  uint8_t G;
  uint8_t B;
};

extern std::array<Color, 64> NesColorTable;

