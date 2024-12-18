#pragma once

#include "Bus.h"
#include "R6502ProcessorStatusRegister.h"
#include "ICpuLogger.h"

#include <vector>
#include <string>
#include <map>
#include <optional>
#include <fstream>

class R6502
{
public:
  R6502(Bus& bus, bool decimalModeEnabled, std::shared_ptr<ICpuLogger> cpuLogger=nullptr);
  ~R6502();

  void Clock(uint16_t ppuX, uint16_t ppuY);
  void Reset(std::optional<uint16_t> programCounter = std::nullopt);
  void IRQ_Interrupt();
  void NMI_Interrupt();

  // for testing purposes
  uint16_t getPc() const { return pc; };

private:
  enum class AddresMode
  {
    Accum, 
    IMM, 
    ABS, 
    ZP, 
    ZP_X, 
    ZP_Y, 
    ABS_X, 
    ABS_Y, 
    Implied, 
    Relative, 
    IND_X, 
    IND_Y, 
    Indirect,
  };

  struct Instruction
  {
    Instruction()
      : opcode("???")
      , addresMode(AddresMode::Implied)
      , operation(nullptr)
      , instruction_bytes(1)
      , cycles(2)
      , cyclesCanExtend(false)
      , isInvalid(false)
    {
    }

    Instruction(std::string opcode_, 
                AddresMode addresMode_, 
                void(R6502::* operation_)(),
                uint8_t instruction_bytes_, 
                uint8_t cycles_, 
                bool cyclesCanExtend_,
                bool isInvalid_ = false)
      : opcode(opcode_)
      , addresMode(addresMode_)
      , operation(operation_)
      , instruction_bytes(instruction_bytes_)
      , cycles(cycles_)
      , cyclesCanExtend(cyclesCanExtend_)
      , isInvalid(isInvalid_)
    {
    }

    std::string opcode;
    AddresMode addresMode;
    void(R6502::* operation)();
    uint8_t instruction_bytes;
    uint8_t cycles;
    bool cyclesCanExtend;
    bool isInvalid;
  };

  uint16_t Addr_ABS() const;
  uint16_t Addr_ZP() const;
  uint16_t Addr_ZP_X() const ;
  uint16_t Addr_ZP_Y() const;
  uint16_t Addr_ABS_X();
  uint16_t Addr_ABS_Y();
  uint16_t Addr_Relative() const;
  uint16_t Addr_IND_X() const;
  uint16_t Addr_IND_Y();
  uint16_t Addr_Indirect() const;

  uint16_t ReadData(uint8_t bytes);
  uint16_t GetAddr();
  std::string GetAddrString() const;

  uint8_t FetchData();
  void WriteData(uint8_t data);

  void ADC(); void AND(); void ASL(); void BCC(); void BCS(); void BEQ(); void BIT(); void BMI(); void BNE();
  void BPL(); void BRK(); void BVC(); void BVS(); void CLC(); void CLD(); void CLI(); void CLV(); void CMP();
  void CPX(); void CPY(); void DEC(); void DEX(); void DEY(); void EOR(); void INC(); void INX(); void INY();
  void JMP(); void JSR(); void LDA(); void LDX(); void LDY(); void LSR(); void NOP(); void ORA(); void PHA();
  void PHP(); void PLA(); void PLP(); void ROL(); void ROR(); void RTI(); void RTS(); void SBC(); void SEC();
  void SED(); void SEI(); void STA(); void STX(); void STY(); void TAX(); void TAY(); void TSX(); void TXA();
  void TXS(); void TYA();

  // Illegal opcodes:
  void ANC(); void DCP(); void ISB(); void JAM(); void LAX(); void RLA(); void RRA(); void SAX(); void SLO(); 
  void SRE();

  uint8_t BusRead(uint16_t address) const;
  uint8_t BusWrite(uint16_t address, uint8_t data);

  void HandleBranch(bool branch);

  bool m_decimalModeEnabled;

  Instruction* _currentInstruction = nullptr;
  uint16_t _currentData = 0x0000;
  uint8_t _cycles = 0x00;
  Bus& _bus;
  std::vector<Instruction> _instructions;

  uint8_t regA = 0x00;                      // Accumulator
  uint8_t regY = 0x00;                      // Index Register Y
  uint8_t regX = 0x00;                      // Index Register X
  uint16_t pc = 0x0000;                     // Program Counter 'PC'
  uint8_t sp = 0x00;                        // Stack Pointer 'S'
  R6502ProcessorStatusRegister status;      // Processor Status Reg 'P'

  std::shared_ptr<ICpuLogger>         m_cpuLogger;
  mutable ICpuLogger::CpuLogLine      m_cpuLogline;
};
