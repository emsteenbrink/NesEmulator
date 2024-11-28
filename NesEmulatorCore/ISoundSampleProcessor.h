#pragma once

#include <cstdint>


class ISoundSampleProcessor
{
public:
  virtual void AddSample(int16_t sample) = 0;
};