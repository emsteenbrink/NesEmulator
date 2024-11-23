#pragma once

#include <cstdint>
#include <cassert>

struct Range
{
public:
  explicit Range(uint16_t value);  // [value, value]
  Range(uint16_t low, uint16_t high);  // [low, high]
  Range(const Range& other);
  bool operator<(const Range& rhs) const;


  uint16_t low() const;
  uint16_t high() const;

private:
  uint16_t m_low;
  uint16_t m_high;
}; // class Range