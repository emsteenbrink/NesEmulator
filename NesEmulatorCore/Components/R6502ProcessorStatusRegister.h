#pragma once

#include <cstdint>
#include <bitset>

class R6502ProcessorStatusRegister
{
public:
  enum class Flag : size_t
  {
    Carry = 0,
    Zero = 1,
    IRQDisable = 2,
    DecimalMode = 3,
    BrkCommand = 4,
    Unused = 5,
    Overflow = 6,
    Negative = 7,
  };

  R6502ProcessorStatusRegister();
  R6502ProcessorStatusRegister(uint8_t value);

  void Reset();

  bool Negative();
  bool Overflow();
  bool Unused();
  bool BrkCommand();
  bool DecimalMode();
  bool IRQ_Disable();
  bool Zero();
  bool Carry();

  void SetNegative(bool value);
  void SetOverflow(bool value);
  void SetBrkCommand(bool value);
  void SetDecimalMode(bool value);
  void SetIRQ_Disable(bool value);
  void SetZero(bool value);
  void SetCarry(bool value);

  operator uint8_t();
  R6502ProcessorStatusRegister& operator=(R6502ProcessorStatusRegister& other) = default;
  R6502ProcessorStatusRegister& operator=(uint8_t value);

private:
  bool Test(Flag flag);
  void Set(Flag flag, bool value);

  std::bitset<8> m_status;
};