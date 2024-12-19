#include "RAM.h"
#include <fstream>
#include <iostream>

using namespace std;

RAM::RAM(unsigned int size, uint16_t beginAddress, uint16_t endAddress)
  : Component(Range(beginAddress, endAddress))
  , m_ram(size, 0)
  , m_size(size)
  , m_beginAddress(beginAddress)
  , m_endAddress(endAddress)
{
}

uint8_t RAM::Read(uint16_t address)
{
  auto realAddress = (address - m_beginAddress) % m_size;
  return m_ram[realAddress];
}

uint8_t RAM::Write(uint16_t address, uint8_t data)
{
  auto realAddress = (address - m_beginAddress) % m_size;
  uint8_t prevValue = m_ram[realAddress];
  m_ram[realAddress] = data;
  return prevValue;
}
