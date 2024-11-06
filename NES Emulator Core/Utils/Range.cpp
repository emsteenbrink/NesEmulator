#include "Range.h"

Range::Range(uint16_t value)
: m_low(value)
, m_high(value)
{
}

Range::Range(uint16_t low, uint16_t high)
: m_low(low)
, m_high(high)
{
}

Range::Range(const Range& other)
  : m_low(other.m_low)
  , m_high(other.m_high)
{
}

uint16_t Range::high() const
{ 
  return m_high; 
}

uint16_t Range::low() const
{ 
  return m_low; 
}

bool Range::operator<(const Range& rhs) const
{
  if (m_low < rhs.m_low)
  {
    assert(m_high < rhs.m_low); // sanity check
    return true;
  }
  return false;
}
