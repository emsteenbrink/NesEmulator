#pragma once

#include "Components/ColorTable.h"

#include <cstdint>

class IPixelWindow
{
public:
  virtual void SetPixel(uint16_t x, uint16_t y, Color color) = 0;
};
