#pragma once

#include <cstdint>
#include <vector>
#include <string>

class ICpuLogger
{
public:
  struct CpuLogLine
  {
    uint16_t PC;
    uint8_t opcode;
    std::vector<uint8_t> data;
    std::string instruction;
    std::string dataAddressString;
    uint8_t A;
    uint8_t X;
    uint8_t Y;
    uint8_t P;
    uint8_t SP;
    bool isInvalid;
    int cycle;
    uint16_t ppuX;
    uint16_t ppuY;
  };


  virtual void AddLogline(const CpuLogLine logLine) = 0;
};