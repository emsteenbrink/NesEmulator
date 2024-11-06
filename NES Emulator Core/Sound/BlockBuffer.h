#pragma once

#include "Resampler.h"

#include <cstdint>
#include <array>
#include <atomic>

class BlockBuffer
{
public:
  static const int BLOCK_SIZE = 1024;
  static const int BLOCK_COUNT = 10;

  BlockBuffer(uint64_t inputRate, uint64_t outputRate);
  virtual ~BlockBuffer();

  void AddSample(int16_t sample);

  void FillSamples(uint8_t* buffer, size_t size);



private:
  bool CanIncrementReadIndex();
  bool CanIncrementWriteIndex();

  void IncrementReadIndex();
  void IncrementWriteIndex();

  uint16_t CalcAvailable();

  Resampler m_resampler;
  std::array<std::array<uint8_t, BLOCK_SIZE>, BLOCK_COUNT>* m_buffers;

  std::atomic<uint16_t> m_writeIndex;
  std::atomic<uint16_t> m_readIndex;
};