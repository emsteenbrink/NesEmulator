#pragma once

#include <cstdint>
#include <array>

class LengthCounter
{
public:
  LengthCounter(bool extraCycle = true);

  void Enabled(bool enabled);
  void Halt(bool halt);
  void Load(uint8_t value);
  void Clock();

  bool Silenced();

private:
  bool      m_enabled;
  bool      m_halt;
  uint8_t   m_lengthCounter;
  uint8_t   m_extraCycle;

  static const std::array<uint8_t, 32> m_LengthTable;
};