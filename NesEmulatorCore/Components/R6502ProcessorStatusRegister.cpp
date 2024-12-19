#include "R6502ProcessorStatusRegister.h"



R6502ProcessorStatusRegister::R6502ProcessorStatusRegister()
 : m_status(0x00)
{
  Set(Flag::Unused, true);
  Set(Flag::IRQDisable, true);
}

R6502ProcessorStatusRegister::R6502ProcessorStatusRegister(uint8_t value)
  : m_status(value)
{
  Set(Flag::Unused, true);
}

void R6502ProcessorStatusRegister::Reset()
{
  m_status.reset();
  Set(Flag::Unused, true);
  Set(Flag::IRQDisable, true);
}

bool R6502ProcessorStatusRegister::Negative()
{
  return Test(Flag::Negative);
}

bool R6502ProcessorStatusRegister::Overflow()
{
  return Test(Flag::Overflow);
}

bool R6502ProcessorStatusRegister::Unused()
{
  return Test(Flag::Unused);
}

bool R6502ProcessorStatusRegister::BrkCommand()
{
  return Test(Flag::BrkCommand);
}

bool R6502ProcessorStatusRegister::DecimalMode()
{
  return Test(Flag::DecimalMode);
}

bool R6502ProcessorStatusRegister::IRQ_Disable()
{
  return Test(Flag::IRQDisable);
}

bool R6502ProcessorStatusRegister::Zero()
{
  return Test(Flag::Zero);
}

bool R6502ProcessorStatusRegister::Carry()
{
  return Test(Flag::Carry);
}


void R6502ProcessorStatusRegister::SetNegative(bool value)
{
  Set(Flag::Negative, value);
}

void R6502ProcessorStatusRegister::SetOverflow(bool value)
{
  Set(Flag::Overflow, value);
}

void R6502ProcessorStatusRegister::SetBrkCommand(bool value)
{
  Set(Flag::BrkCommand, value);
}

void R6502ProcessorStatusRegister::SetDecimalMode(bool value)
{
  Set(Flag::DecimalMode, value);
}

void R6502ProcessorStatusRegister::SetIRQ_Disable(bool value)
{
  Set(Flag::IRQDisable, value);
}

void R6502ProcessorStatusRegister::SetZero(bool value)
{
  Set(Flag::Zero, value);
}

void R6502ProcessorStatusRegister::SetCarry(bool value)
{
  Set(Flag::Carry, value);
}

R6502ProcessorStatusRegister::operator uint8_t()
{
  return (uint8_t)m_status.to_ulong();
}

R6502ProcessorStatusRegister& R6502ProcessorStatusRegister::operator=(uint8_t value)
{
  m_status = value;
  Set(Flag::Unused, true);
  Set(Flag::BrkCommand, false);
  return *this;
}

bool R6502ProcessorStatusRegister::Test(Flag flag)
{
  return m_status.test(static_cast<size_t>(flag));
}

void R6502ProcessorStatusRegister::Set(Flag flag, bool value)
{
  m_status.set(static_cast<size_t>(flag), value);
}