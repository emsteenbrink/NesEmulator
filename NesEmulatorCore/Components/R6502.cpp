#include "R6502.h"
#include <cassert>
#include <iostream>

using namespace std;

R6502::R6502(Bus& bus, bool decimalModeEnabled)
: _bus(bus)
, m_decimalModeEnabled(decimalModeEnabled)
{
  _instructions = {
    // 0
    {"BRK", AddresMode::Implied,  &R6502::BRK, 1, 7, false},
    {"ORA", AddresMode::IND_X,    &R6502::ORA, 2, 6, false},
    {"JAM", AddresMode::Implied,  &R6502::JAM, 1, 0, false},
    {"SLO", AddresMode::IND_X,    &R6502::SLO, 2, 8, false},
    {"NOP", AddresMode::ZP,       &R6502::NOP, 2, 3, false},
    {"ORA", AddresMode::ZP,       &R6502::ORA, 2, 3, false},
    {"ASL", AddresMode::ZP,       &R6502::ASL, 2, 5, false},
    {"SLO", AddresMode::ZP,       &R6502::SLO, 2, 5, false},
    {"PHP", AddresMode::Implied,  &R6502::PHP, 1, 3, false},
    {"ORA", AddresMode::IMM,      &R6502::ORA, 2, 2, false},
    {"ASL", AddresMode::Accum,    &R6502::ASL, 1, 2, false},
    {"ANC", AddresMode::IMM,      &R6502::ANC, 2, 2, false},
    {"NOP", AddresMode::ABS,      &R6502::NOP, 3, 4, false},
    {"ORA", AddresMode::ABS,      &R6502::ORA, 3, 4, false},
    {"ASL", AddresMode::ABS,      &R6502::ASL, 3, 6, false},
    {"SLO", AddresMode::ABS,      &R6502::SLO, 3, 6, false},
    // 1
    {"BPL", AddresMode::Relative, &R6502::BPL, 2, 2, true},
    {"ORA", AddresMode::IND_Y,    &R6502::ORA, 2, 5, true},
    {"JAM", AddresMode::Implied,  &R6502::JAM, 1, 0, false},
    {"SLO", AddresMode::IND_Y,    &R6502::SLO, 2, 8, false},
    {"NOP", AddresMode::ZP_X,     &R6502::NOP, 2, 4, false},
    {"ORA", AddresMode::ZP_X,     &R6502::ORA, 2, 4, false},
    {"ASL", AddresMode::ZP_X,     &R6502::ASL, 2, 6, false},
    {"SLO", AddresMode::ZP_X,     &R6502::SLO, 2, 6, false},
    {"CLC", AddresMode::Implied,  &R6502::CLC, 1, 2, false},
    {"ORA", AddresMode::ABS_Y,    &R6502::ORA, 3, 4, true},
    {"NOP", AddresMode::Implied,  &R6502::NOP, 1, 2, false},
    {"SLO", AddresMode::ABS_Y,    &R6502::SLO, 3, 7, false},
    {"NOP", AddresMode::ABS_X,    &R6502::NOP, 3, 4, true},
    {"ORA", AddresMode::ABS_X,    &R6502::ORA, 3, 4, true},
    {"ASL", AddresMode::ABS_X,    &R6502::ASL, 3, 7, false},
    {"SLO", AddresMode::ABS_X,    &R6502::SLO, 3, 7, false},
    // 2
    {"JSR", AddresMode::ABS,      &R6502::JSR, 3, 6, false},
    {"AND", AddresMode::IND_X,    &R6502::AND, 2, 6, false},
    {"JAM", AddresMode::Implied,  &R6502::JAM, 1, 0, false},
    {"RLA", AddresMode::IND_X,    &R6502::RLA, 2, 8, false},
    {"BIT", AddresMode::ZP,       &R6502::BIT, 2, 3, false},
    {"AND", AddresMode::ZP,       &R6502::AND, 2, 3, false},
    {"ROL", AddresMode::ZP,       &R6502::ROL, 2, 5, false},
    {"RLA", AddresMode::ZP,       &R6502::RLA, 2, 5, false},
    {"PLP", AddresMode::Implied,  &R6502::PLP, 1, 4, false},
    {"AND", AddresMode::IMM,      &R6502::AND, 2, 2, false},
    {"ROL", AddresMode::Accum,    &R6502::ROL, 1, 2, false},
    {"ANC", AddresMode::IMM,      &R6502::ANC, 2, 2, false},
    {"BIT", AddresMode::ABS,      &R6502::BIT, 3, 4, false},
    {"AND", AddresMode::ABS,      &R6502::AND, 3, 4, false},
    {"ROL", AddresMode::ABS,      &R6502::ROL, 3, 6, false},
    {"RLA", AddresMode::ABS,      &R6502::RLA, 3, 6, false},
    // 3
    {"BMI", AddresMode::Relative, &R6502::BMI, 2, 2, true},
    {"AND", AddresMode::IND_Y,    &R6502::AND, 2, 5, true},
    {"JAM", AddresMode::Implied,  &R6502::JAM, 1, 0, false},
    {"RLA", AddresMode::IND_Y,    &R6502::RLA, 2, 8, false},
    {"NOP", AddresMode::ZP_X,     &R6502::NOP, 2, 4, false},
    {"AND", AddresMode::ZP_X,     &R6502::AND, 2, 4, false},
    {"ROL", AddresMode::ZP_X,     &R6502::ROL, 2, 6, false},
    {"RLA", AddresMode::ZP_X,     &R6502::RLA, 2, 6, false},
    {"SEC", AddresMode::Implied,  &R6502::SEC, 1, 2, false},
    {"AND", AddresMode::ABS_Y,    &R6502::AND, 3, 4, true},
    {"NOP", AddresMode::Implied,  &R6502::NOP, 1, 2, false},
    {"RLA", AddresMode::ABS_Y,    &R6502::RLA, 3, 7, false},
    {"NOP", AddresMode::ABS_X,    &R6502::NOP, 3, 4, true},
    {"AND", AddresMode::ABS_X,    &R6502::AND, 3, 4, true},
    {"ROL", AddresMode::ABS_X,    &R6502::ROL, 3, 7, false},
    {"RLA", AddresMode::ABS_X,    &R6502::RLA, 3, 7, false},
    // 4
    {"RTI", AddresMode::Implied,  &R6502::RTI, 1, 6, false},
    {"EOR", AddresMode::IND_X,    &R6502::EOR, 2, 6, false},
    {"JAM", AddresMode::Implied,  &R6502::JAM, 1, 0, false},
    {"SRE", AddresMode::IND_X,    &R6502::SRE, 2, 8, false},
    {"NOP", AddresMode::ZP,       &R6502::NOP, 2, 3, false},
    {"EOR", AddresMode::ZP,       &R6502::EOR, 2, 3, false},
    {"LSR", AddresMode::ZP,       &R6502::LSR, 2, 5, false},
    {"SRE", AddresMode::ZP,       &R6502::SRE, 2, 5, false},
    {"PHA", AddresMode::Implied,  &R6502::PHA, 1, 3, false},
    {"EOR", AddresMode::IMM,      &R6502::EOR, 2, 2, false},
    {"LSR", AddresMode::Accum,    &R6502::LSR, 1, 2, false},
    {},
    {"JMP", AddresMode::ABS,      &R6502::JMP, 3, 3, false},
    {"EOR", AddresMode::ABS,      &R6502::EOR, 3, 4, false},
    {"LSR", AddresMode::ABS,      &R6502::LSR, 3, 6, false},
    {"SRE", AddresMode::ABS,      &R6502::SRE, 3, 6, false},
    // 5
    {"BVC", AddresMode::Relative, &R6502::BVC, 2, 2, true},
    {"EOR", AddresMode::IND_Y,    &R6502::EOR, 2, 5, true},
    {"JAM", AddresMode::Implied,  &R6502::JAM, 1, 0, false},
    {"SRE", AddresMode::IND_Y,    &R6502::SRE, 2, 8, false},
    {"NOP", AddresMode::ZP_X,     &R6502::NOP, 2, 4, false},
    {"EOR", AddresMode::ZP_X,     &R6502::EOR, 2, 4, false},
    {"LSR", AddresMode::ZP_X,     &R6502::LSR, 2, 6, false},
    {"SRE", AddresMode::ZP_X,     &R6502::SRE, 2, 6, false},
    {"CLI", AddresMode::Implied,  &R6502::CLI, 1, 2, false},
    {"EOR", AddresMode::ABS_Y,    &R6502::EOR, 3, 4, true},
    {"NOP", AddresMode::Implied,  &R6502::NOP, 1, 2, false},
    {"SRE", AddresMode::ABS_Y,    &R6502::SRE, 3, 7, false},
    {"NOP", AddresMode::ABS_X,    &R6502::NOP, 3, 4, true},
    {"EOR", AddresMode::ABS_X,    &R6502::EOR, 3, 4, true},
    {"LSR", AddresMode::ABS_X,    &R6502::LSR, 3, 7, false},
    {"SRE", AddresMode::ABS_X,    &R6502::SRE, 3, 7, false},
    // 6
    {"RTS", AddresMode::Implied,  &R6502::RTS, 1, 6, false},
    {"ADC", AddresMode::IND_X,    &R6502::ADC, 2, 6, false},
    {"JAM", AddresMode::Implied,  &R6502::JAM, 1, 0, false},
    {"RRA", AddresMode::IND_X,    &R6502::RRA, 2, 8, false},
    {"NOP", AddresMode::ZP,       &R6502::NOP, 2, 3, false},
    {"ADC", AddresMode::ZP,       &R6502::ADC, 2, 3, false},
    {"ROR", AddresMode::ZP,       &R6502::ROR, 2, 5, false},
    {"RRA", AddresMode::ZP,       &R6502::RRA, 2, 5, false},
    {"PLA", AddresMode::Implied,  &R6502::PLA, 1, 4, false},
    {"ADC", AddresMode::IMM,      &R6502::ADC, 2, 2, false},
    {"ROR", AddresMode::Accum,    &R6502::ROR, 1, 2, false},
    {},
    {"JMP", AddresMode::Indirect, &R6502::JMP, 3, 5, false},
    {"ADC", AddresMode::ABS,      &R6502::ADC, 3, 4, false},
    {"ROR", AddresMode::ABS,      &R6502::ROR, 3, 6, false},
    {"RRA", AddresMode::ABS,      &R6502::RRA, 3, 6, false},
    // 7
    {"BVS", AddresMode::Relative, &R6502::BVS, 2, 2, true},
    {"ADC", AddresMode::IND_Y,    &R6502::ADC, 2, 5, true},
    {"JAM", AddresMode::Implied,  &R6502::JAM, 1, 0, false},
    {"RRA", AddresMode::IND_Y,    &R6502::RRA, 2, 8, false},
    {"NOP", AddresMode::ZP_X,     &R6502::NOP, 2, 4, false},
    {"ADC", AddresMode::ZP_X,     &R6502::ADC, 2, 4, false},
    {"ROR", AddresMode::ZP_X,     &R6502::ROR, 2, 6, false},
    {"RRA", AddresMode::ZP_X,     &R6502::RRA, 2, 6, false},
    {"SEI", AddresMode::Implied,  &R6502::SEI, 1, 2, false},
    {"ADC", AddresMode::ABS_Y,    &R6502::ADC, 3, 4, true},
    {"NOP", AddresMode::Implied,  &R6502::NOP, 1, 2, false},
    {"RRA", AddresMode::ABS_Y,    &R6502::RRA, 3, 7, false},
    {"NOP", AddresMode::ABS_X,    &R6502::NOP, 3, 4, true},
    {"ADC", AddresMode::ABS_X,    &R6502::ADC, 3, 4, true},
    {"ROR", AddresMode::ABS_X,    &R6502::ROR, 3, 7, false},
    {"RRA", AddresMode::ABS_X,    &R6502::RRA, 3, 7, false},
    // 8
    {"NOP", AddresMode::IMM,      &R6502::NOP, 2, 2, false},
    {"STA", AddresMode::IND_X,    &R6502::STA, 2, 6, false},
    {"NOP", AddresMode::IMM,      &R6502::NOP, 2, 2, false},
    {"SAX", AddresMode::IND_X,    &R6502::SAX, 2, 6, false},
    {"STY", AddresMode::ZP,       &R6502::STY, 2, 3, false},
    {"STA", AddresMode::ZP,       &R6502::STA, 2, 3, false},
    {"STX", AddresMode::ZP,       &R6502::STX, 2, 3, false},
    {"SAX", AddresMode::ZP,       &R6502::SAX, 2, 3, false},
    {"DEY", AddresMode::Implied,  &R6502::DEY, 1, 2, false},
    {"NOP", AddresMode::IMM,      &R6502::NOP, 2, 2, false},
    {"TXA", AddresMode::Implied,  &R6502::TXA, 1, 2, false},
    {},
    {"STY", AddresMode::ABS,      &R6502::STY, 3, 4, false},
    {"STA", AddresMode::ABS,      &R6502::STA, 3, 4, false},
    {"STX", AddresMode::ABS,      &R6502::STX, 3, 4, false},
    {"SAX", AddresMode::ABS,      &R6502::SAX, 3, 4, false},
    // 9
    {"BCC", AddresMode::Relative, &R6502::BCC, 2, 2, true},
    {"STA", AddresMode::IND_Y,    &R6502::STA, 2, 6, false},
    {"JAM", AddresMode::Implied,  &R6502::JAM, 1, 0, false},
    {},
    {"STY", AddresMode::ZP_X,     &R6502::STY, 2, 4, false},
    {"STA", AddresMode::ZP_X,     &R6502::STA, 2, 4, false},
    {"STX", AddresMode::ZP_Y,     &R6502::STX, 2, 4, false},
    {"SAX", AddresMode::ZP_Y,     &R6502::SAX, 2, 4, false},
    {"TYA", AddresMode::Implied,  &R6502::TYA, 1, 2, false},
    {"STA", AddresMode::ABS_Y,    &R6502::STA, 3, 5, false},
    {"TXS", AddresMode::Implied,  &R6502::TXS, 1, 2, false},
    {},
    {},
    {"STA", AddresMode::ABS_X,    &R6502::STA, 3, 5, false},
    {},
    {},
    // A
    {"LDY", AddresMode::IMM,      &R6502::LDY, 2, 2, false},
    {"LDA", AddresMode::IND_X,    &R6502::LDA, 2, 6, false},
    {"LDX", AddresMode::IMM,      &R6502::LDX, 2, 2, false},
    {"LAX", AddresMode::IND_X,    &R6502::LAX, 2, 6, false},
    {"LDY", AddresMode::ZP,       &R6502::LDY, 2, 3, false},
    {"LDA", AddresMode::ZP,       &R6502::LDA, 2, 3, false},
    {"LDX", AddresMode::ZP,       &R6502::LDX, 2, 3, false},
    {"LAX", AddresMode::ZP,       &R6502::LAX, 2, 3, false},
    {"TAY", AddresMode::Implied,  &R6502::TAY, 1, 2, false},
    {"LDA", AddresMode::IMM,      &R6502::LDA, 2, 2, false},
    {"TAX", AddresMode::Implied,  &R6502::TAX, 1, 2, false},
    {},
    {"LDY", AddresMode::ABS,      &R6502::LDY, 3, 4, false},
    {"LDA", AddresMode::ABS,      &R6502::LDA, 3, 4, false},
    {"LDX", AddresMode::ABS,      &R6502::LDX, 3, 4, false},
    {"LAX", AddresMode::ABS,      &R6502::LAX, 3, 4, false},
    // B
    {"BCS", AddresMode::Relative, &R6502::BCS, 2, 2, true},
    {"LDA", AddresMode::IND_Y,    &R6502::LDA, 2, 5, true},
    {"JAM", AddresMode::Implied,  &R6502::JAM, 1, 0, false},
    {"LAX", AddresMode::IND_Y,    &R6502::LAX, 2, 5, true},
    {"LDY", AddresMode::ZP_X,     &R6502::LDY, 2, 4, false},
    {"LDA", AddresMode::ZP_X,     &R6502::LDA, 2, 4, false},
    {"LDX", AddresMode::ZP_Y,     &R6502::LDX, 2, 4, false},
    {"LAX", AddresMode::ZP_Y,     &R6502::LAX, 2, 4, false},
    {"CLV", AddresMode::Implied,  &R6502::CLV, 1, 2, false},
    {"LDA", AddresMode::ABS_Y,    &R6502::LDA, 3, 4, true},
    {"TSX", AddresMode::Implied,  &R6502::TSX, 1, 2, false},
    {},
    {"LDY", AddresMode::ABS_X,    &R6502::LDY, 3, 4, true},
    {"LDA", AddresMode::ABS_X,    &R6502::LDA, 3, 4, true},
    {"LDX", AddresMode::ABS_Y,    &R6502::LDX, 3, 4, true},
    {"LAX", AddresMode::ABS_Y,    &R6502::LAX, 3, 4, true},
    // C
    {"CPY", AddresMode::IMM,      &R6502::CPY, 2, 2, false},
    {"CMP", AddresMode::IND_X,    &R6502::CMP, 2, 6, false},
    {"NOP", AddresMode::IMM,      &R6502::NOP, 2, 2, false},
    {"DCP", AddresMode::IND_X,    &R6502::DCP, 2, 8, false},
    {"CPY", AddresMode::ZP,       &R6502::CPY, 2, 3, false},
    {"CMP", AddresMode::ZP,       &R6502::CMP, 2, 3, false},
    {"DEC", AddresMode::ZP,       &R6502::DEC, 2, 5, false},
    {"DCP", AddresMode::ZP,       &R6502::DCP, 2, 5, false},
    {"INY", AddresMode::Implied,  &R6502::INY, 1, 2, false},
    {"CMP", AddresMode::IMM,      &R6502::CMP, 2, 2, false},
    {"DEX", AddresMode::Implied,  &R6502::DEX, 1, 2, false},
    {},
    {"CPY", AddresMode::ABS,      &R6502::CPY, 3, 4, false},
    {"CMP", AddresMode::ABS,      &R6502::CMP, 3, 4, false},
    {"DEC", AddresMode::ABS,      &R6502::DEC, 3, 6, false},
    {"DCP", AddresMode::ABS,      &R6502::DCP, 3, 6, false},
    // D
    {"BNE", AddresMode::Relative, &R6502::BNE, 2, 2, true},
    {"CMP", AddresMode::IND_Y,    &R6502::CMP, 2, 5, true},
    {"JAM", AddresMode::Implied,  &R6502::JAM, 1, 0, false},
    {"DCP", AddresMode::IND_Y,    &R6502::DCP, 2, 8, false},
    {"NOP", AddresMode::ZP_X,     &R6502::NOP, 2, 4, false},
    {"CMP", AddresMode::ZP_X,     &R6502::CMP, 2, 4, false},
    {"DEC", AddresMode::ZP_X,     &R6502::DEC, 2, 6, false},
    {"DCP", AddresMode::ZP_X,     &R6502::DCP, 2, 6, false},
    {"CLD", AddresMode::Implied,  &R6502::CLD, 1, 2, false},
    {"CMP", AddresMode::ABS_Y,    &R6502::CMP, 3, 4, true},
    {"NOP", AddresMode::Implied,  &R6502::NOP, 1, 2, false},
    {"DCP", AddresMode::ABS_Y,    &R6502::DCP, 3, 7, false},
    {"NOP", AddresMode::ABS_X,    &R6502::NOP, 3, 4, true},
    {"CMP", AddresMode::ABS_X,    &R6502::CMP, 3, 4, true},
    {"DEC", AddresMode::ABS_X,    &R6502::DEC, 3, 7, false},
    {"DCP", AddresMode::ABS_X,    &R6502::DCP, 3, 7, false},
    // E
    {"CPX", AddresMode::IMM,      &R6502::CPX, 2, 2, false},
    {"SBC", AddresMode::IND_X,    &R6502::SBC, 2, 6, false},
    {"NOP", AddresMode::IMM,      &R6502::NOP, 2, 2, false},
    {"ISB", AddresMode::IND_X,    &R6502::ISB, 2, 8, false},
    {"CPX", AddresMode::ZP,       &R6502::CPX, 2, 3, false},
    {"SBC", AddresMode::ZP,       &R6502::SBC, 2, 3, false},
    {"INC", AddresMode::ZP,       &R6502::INC, 2, 5, false},
    {"ISB", AddresMode::ZP,       &R6502::ISB, 2, 5, false},
    {"INX", AddresMode::Implied,  &R6502::INX, 1, 2, false},
    {"SBC", AddresMode::IMM,      &R6502::SBC, 2, 2, false},
    {"NOP", AddresMode::Implied,  &R6502::NOP, 1, 2, false},
    {"SBC", AddresMode::IMM,      &R6502::SBC, 2, 2, false},
    {"CPX", AddresMode::ABS,      &R6502::CPX, 3, 4, false},
    {"SBC", AddresMode::ABS,      &R6502::SBC, 3, 4, false},
    {"INC", AddresMode::ABS,      &R6502::INC, 3, 6, false},
    {"ISB", AddresMode::ABS,      &R6502::ISB, 3, 6, false},
    // F
    {"BEQ", AddresMode::Relative, &R6502::BEQ, 2, 2, true},
    {"SBC", AddresMode::IND_Y,    &R6502::SBC, 2, 5, true},
    {"JAM", AddresMode::Implied,  &R6502::JAM, 1, 0, false},
    {"ISB", AddresMode::IND_Y,    &R6502::ISB, 2, 8, false},
    {"NOP", AddresMode::ZP_X,     &R6502::NOP, 2, 4, false},
    {"SBC", AddresMode::ZP_X,     &R6502::SBC, 2, 4, false},
    {"INC", AddresMode::ZP_X,     &R6502::INC, 2, 6, false},
    {"ISB", AddresMode::ZP_X,     &R6502::ISB, 2, 6, false},
    {"SED", AddresMode::Implied,  &R6502::SED, 1, 2, false},
    {"SBC", AddresMode::ABS_Y,    &R6502::SBC, 3, 4, true},
    {"NOP", AddresMode::Implied,  &R6502::NOP, 1, 2, false},
    {"ISB", AddresMode::ABS_Y,    &R6502::ISB, 3, 7, false},
    {"NOP", AddresMode::ABS_X,    &R6502::NOP, 3, 4, true},
    {"SBC", AddresMode::ABS_X,    &R6502::SBC, 3, 4, true},
    {"INC", AddresMode::ABS_X,    &R6502::INC, 3, 7, false},
    {"ISB", AddresMode::ABS_X,    &R6502::ISB, 3, 7, false},
  };
}

R6502::~R6502()
{
}

void R6502::Clock()
{
  if (_cycles == 0)
  {
    uint8_t opcode = BusRead(pc++);
    _currentInstruction = &_instructions[opcode];

    if (_currentInstruction->instruction_bytes > 1)
      _currentData = this->ReadData(_currentInstruction->instruction_bytes - 1);
    _cycles = _currentInstruction->cycles;
    (this->*_currentInstruction->operation)();
  }
  _cycles--;
}

void R6502::Reset()
{
  // Get address to set program counter to
  uint16_t addr_abs = 0xFFFC;
  uint16_t lo = BusRead(addr_abs + 0);
  uint16_t hi = BusRead(addr_abs + 1);

  // Set it
  pc = (hi << 8) | lo;

  // Reset internal registers
  regA = 0x00;
  regX = 0x00;
  regY = 0x00;
  sp = 0xFD;
  status.Reset();

  // Reset takes time
  _cycles = 8;
}

void R6502::IRQ_Interrupt()
{
  if (!status.IRQ_Disable())
  {
    BusWrite(0x0100 + sp, (uint8_t)(pc >> 8));
    sp--;
    BusWrite(0x0100 + sp, (uint8_t)(pc & 0x00FF));
    sp--;
    status.SetBrkCommand(false);
    BusWrite(0x0100 + sp, status);
    sp--;
    status.SetIRQ_Disable(true);
    pc = BusRead(0xFFFF) << 8 | BusRead(0xFFFE);
    _cycles = 7;
  }
}

void R6502::NMI_Interrupt()
{
  BusWrite(0x0100 + sp, (uint8_t)(pc >> 8));
  sp--;
  BusWrite(0x0100 + sp, (uint8_t)(pc & 0x00FF));
  sp--;
  status.SetBrkCommand(false);
  status.SetIRQ_Disable(true);
  BusWrite(0x100 + sp, status);
  sp--;
  pc = BusRead(0xFFFB) << 8 | BusRead(0xFFFA);
  _cycles = 8;
}

uint16_t R6502::Addr_ABS_XY(uint8_t regValue)
{
  uint16_t result = _currentData + regValue;
  if (_currentInstruction->cyclesCanExtend && (_currentData & 0xFF00) != (result & 0xFF00))
    _cycles++;
  return result;
}

uint16_t R6502::Addr_IND_X()
{
  uint8_t addres = (uint8_t)(_currentData + regX);
  uint16_t loByte = BusRead(addres);
//  uint16_t hiByte = BusRead((uint8_t)(addres + 1));
  uint16_t hiByte;
  if ((addres & 0x00FF) == 0x00FF) // Simulate page boundary bug
    hiByte = BusRead(addres & 0xFF00);
  else // Simulate normally
    hiByte = BusRead(addres + 1);
  return hiByte << 8 | loByte;
}

uint16_t R6502::Addr_IND_Y()
{
  uint16_t addres = _currentData;
  uint16_t loByte = BusRead(addres);
//  uint16_t hiByte = BusRead((addres + 1) & 0x00FF);
  uint16_t hiByte;
  if ((addres & 0x00FF) == 0x00FF) // Simulate page boundary bug
    hiByte = BusRead(addres & 0xFF00);
  else // Simulate normally
    hiByte = BusRead(addres + 1);
  uint16_t rawAddres = loByte | hiByte << 8;
  uint16_t result = rawAddres + regY;
  if (_currentInstruction->cyclesCanExtend && (rawAddres & 0xFF00) != (result & 0xFF00))
    _cycles++;
  return result;
}

uint16_t R6502::Addr_Indirect()
{
  uint16_t addres = _currentData;
  uint16_t loByte = BusRead(addres);
  uint16_t hiByte;
  if ((addres & 0x00FF) == 0x00FF) // Simulate page boundary bug
    hiByte = BusRead(addres & 0xFF00);
  else // Simulate normally
    hiByte = BusRead(addres + 1);
  return  hiByte << 8 | loByte;
}

uint16_t R6502::ReadData(uint8_t bytes)
{
  assert(bytes > 0 && bytes <= 2);
  uint16_t result = 0;
  result = BusRead(pc++);
  if (bytes == 2)
    result |= BusRead(pc++) << 8;
  return result;
}

uint16_t R6502::GetAddr()
{
  switch (_currentInstruction->addresMode)
  {
  case AddresMode::ABS:
    return _currentData;
  case AddresMode::ZP:
    return _currentData;
  case AddresMode::ZP_X:
    return (_currentData + regX) & 0x00FF;
  case AddresMode::ZP_Y:
    return (_currentData + regY) & 0x00FF;
  case AddresMode::ABS_X:
    return Addr_ABS_XY(regX);
  case AddresMode::ABS_Y:
    return Addr_ABS_XY(regY);
  case AddresMode::Relative:
    return pc + (int8_t)_currentData;
  case AddresMode::IND_X:
    return Addr_IND_X();
  case AddresMode::IND_Y:
    return Addr_IND_Y();
  case AddresMode::Indirect:
    return Addr_Indirect();
  case AddresMode::Accum:
    assert(!"Getting addres with accum ???"); [[fallthrough]];
  case AddresMode::Implied:
    assert(!"Getting addres with implied ???"); [[fallthrough]];
  case AddresMode::IMM:
    assert(!"Getting addres with IMM ???"); [[fallthrough]];
  default:
    assert(!"Getting addres with unknown AddresMode");
  }
  return 0;
}

uint8_t R6502::FetchData()
{
  assert(_currentInstruction->addresMode != AddresMode::Implied);
  
  if (_currentInstruction->addresMode == AddresMode::Accum)
    return regA;

  if (_currentInstruction->addresMode == AddresMode::IMM)
    return (uint8_t)_currentData;

  return BusRead(GetAddr());
}

void R6502::WriteData(uint8_t data)
{
  assert(_currentInstruction->addresMode != AddresMode::Implied);

  if (_currentInstruction->addresMode == AddresMode::Accum)
  {
    regA = data;
    return;
  }

  BusWrite(GetAddr(), data);
}

void R6502::HandleBranch(bool branch)
{
  uint16_t addres = GetAddr();
  if (branch)
  {
    _cycles++;
    if ((pc & 0xFF00) != (addres & 0xFF00))
      _cycles++;
    pc = addres;
  }
}

void R6502::ADC()
{
  uint8_t data = FetchData();
  uint8_t carry = status.Carry() ? 1 : 0;
  uint16_t result = regA + data + carry;
  status.SetZero((result & 0xFF) == 0x00);
  if (status.DecimalMode() && m_decimalModeEnabled)
  {
    if (((regA & 0xf) + (data & 0xf) + carry) > 9) 
      result += 6;
    status.SetNegative( result & 0x80);
    status.SetOverflow(!((regA ^ data) & 0x80) && ((regA ^ result) & 0x80));
    if (result > 0x99) result += 96;
    status.SetCarry(result > 0x99);
  }
  else
  {
    status.SetNegative(result & 0x80);
    status.SetCarry(result > 0xFF);
    bool overflow = ((regA ^ result) & ~(regA ^ data)) & 0x0080;
    status.SetOverflow(overflow);
  }
  regA = (uint8_t)(result & 0x00FF);
}

void R6502::AND()
{
  uint8_t data = FetchData();
  regA &= data;
  status.SetZero(regA == 0x00);
  status.SetNegative(regA & 0x80);
}

void R6502::ASL()
{
  uint8_t data = FetchData();
  status.SetCarry(data & 0x80);
  data <<= 1;
  status.SetZero(data == 0x00);
  status.SetNegative(data & 0x80);
  WriteData((uint8_t)(data & 0x00FF));
}

void R6502::BCC()
{
  HandleBranch(!status.Carry());
}

void R6502::BCS()
{
  HandleBranch(status.Carry());
}

void R6502::BEQ()
{
  HandleBranch(status.Zero());
}

void R6502::BIT()
{
  uint8_t data = FetchData();
  status.SetNegative(data & 0x80);
  status.SetOverflow(data & 0x40);
  status.SetZero((data & regA) == 0x00);
}

void R6502::BMI()
{
  HandleBranch(status.Negative());
}

void R6502::BNE()
{
  HandleBranch(!status.Zero());
}

void R6502::BPL()
{
  HandleBranch(!status.Negative());
}

void R6502::BRK()
{
  pc++; // WHY ?????

  BusWrite(0x0100 + sp, (uint8_t)(pc >> 8));
  sp--;
  BusWrite(0x0100 + sp, (uint8_t)(pc & 0x00FF));
  sp--;

  status.SetBrkCommand(true);
  BusWrite(0x0100 + sp, status);
  sp--;

  status.SetBrkCommand(false);
  status.SetIRQ_Disable(true);

  pc = (uint16_t)BusRead(0xFFFF) << 8 | BusRead(0xFFFE);
}

void R6502::BVC()
{
  HandleBranch(!status.Overflow());
}

void R6502::BVS()
{
  HandleBranch(status.Overflow());
}

void R6502::CLC()
{
  status.SetCarry(false);
}

void R6502::CLD()
{
  status.SetDecimalMode(false);
}

void R6502::CLI()
{
  status.SetIRQ_Disable(false);
}

void R6502::CLV()
{
  status.SetOverflow(false);
}

void R6502::CMP()
{
  uint8_t data = FetchData();
  uint8_t res = regA - data;
  status.SetNegative(res & 0x80);
  status.SetZero(res == 0x00);
  status.SetCarry(regA >= data);
}

void R6502::CPX()
{
  uint8_t data = FetchData();
  uint8_t res = regX - data;
  status.SetNegative(res & 0x80);
  status.SetZero(res == 0x00);
  status.SetCarry(regX >= data);
}

void R6502::CPY()
{
  uint8_t data = FetchData();
  uint8_t res = regY - data;
  status.SetNegative(res & 0x80);
  status.SetZero(res == 0x00);
  status.SetCarry(regY >= data);
}

void R6502::DEC()
{
  uint8_t data = FetchData();
  data--;
  WriteData(data);
  status.SetNegative(data & 0x80);
  status.SetZero(data == 0x00);
}

void R6502::DEX()
{
  regX--;
  status.SetNegative(regX & 0x80);
  status.SetZero(regX == 0x00);
}

void R6502::DEY()
{
  regY--;
  status.SetNegative(regY & 0x80);
  status.SetZero(regY == 0x00);
}

void R6502::EOR()
{
  uint8_t data = FetchData();
  regA ^= data;
  status.SetNegative(regA & 0x80);
  status.SetZero(regA == 0x00);
}

void R6502::INC()
{
  uint8_t data = FetchData();
  data++;
  WriteData(data);
  status.SetNegative(data & 0x80);
  status.SetZero(data == 0x00);
}

void R6502::INX()
{
  regX++;
  status.SetNegative(regX & 0x80);
  status.SetZero(regX == 0x00);
}

void R6502::INY()
{
  regY++;
  status.SetNegative(regY & 0x80);
  status.SetZero(regY == 0x00);
}

void R6502::JMP()
{
  pc = GetAddr();
}

void R6502::JSR()
{
  pc--;
  uint16_t addres = GetAddr();
  BusWrite(0x0100 + sp, (uint8_t)(pc >> 8));
  sp--;
  BusWrite(0x0100 + sp, (uint8_t)(pc & 0x00FF));
  sp--;
  pc = addres;
}

void R6502::LDA()
{
  regA = FetchData();
  status.SetNegative(regA & 0x80);
  status.SetZero(regA == 0x00);
}

void R6502::LDX()
{
  regX = FetchData();
  status.SetNegative(regX & 0x80);
  status.SetZero(regX == 0x00);
}

void R6502::LDY()
{
  regY = FetchData();
  status.SetNegative(regY & 0x80);
  status.SetZero(regY == 0x00);
}

void R6502::LSR()
{
  uint8_t data = FetchData();
  status.SetCarry(data & 0x01);
  data >>= 1;
  status.SetZero(data == 0x00);
  status.SetNegative(data & 0x80);
  WriteData((uint8_t)(data & 0x00FF));
}

void R6502::NOP()
{

}

void R6502::ORA()
{
  uint8_t data = FetchData();
  regA |= data;
  status.SetZero(regA == 0x00);
  status.SetNegative(regA & 0x80);
}

void R6502::PHA()
{
  BusWrite(0x0100 + sp, regA);
  sp--;
}

void R6502::PHP()
{
  auto tmpStatus = status;
  tmpStatus.SetBrkCommand(true);
  BusWrite(0x0100 + sp, (uint8_t)tmpStatus);
  status.SetBrkCommand(0);

  sp--;
}

void R6502::PLA()
{
  sp++;
  regA = BusRead(0x0100 + sp);
  status.SetZero(regA == 0x00);
  status.SetNegative(regA & 0x80);
}

void R6502::PLP()
{
  sp++;
  status = BusRead(0x0100 + sp); 
}

void R6502::ROL()
{
  uint8_t data = FetchData();
  bool carry = data & 0x80;
  data <<= 1;
  if (status.Carry())
    data |= 0x01;
  status.SetNegative(data & 0x80);
  status.SetZero(data == 0x00);
  status.SetCarry(carry);
  WriteData(data);
}

void R6502::ROR()
{
  uint8_t data = FetchData();
  bool carry = data & 0x01;
  data >>= 1;
  if (status.Carry())
    data |= 0x80;
  status.SetNegative(data & 0x80);
  status.SetZero(data == 0x00);
  status.SetCarry(carry);
  WriteData(data);
}

void R6502::RTI()
{
  sp++;
  status = BusRead(0x0100 + sp);
  status.SetBrkCommand(false);

  sp++;
  pc = BusRead(0x0100 + sp);
  sp++;
  pc |= BusRead(0x0100 + sp) << 8;
}

void R6502::RTS()
{
  sp++;
  pc = BusRead(0x0100 + sp);
  sp++;
  pc |= BusRead(0x0100 + sp) << 8;
  pc++;
}

void R6502::SBC()
{
  uint8_t data = FetchData();
  //data = ~data;
  uint8_t carry = status.Carry() ? 0 : 1;
  unsigned int result = regA - data - carry;
  status.SetNegative(result & 0x0080);
  status.SetZero((result & 0xFF) == 0x00);
  bool overflow = ((regA ^ result) & (regA ^ data)) & 0x0080;
  status.SetOverflow(overflow);
  if (status.DecimalMode() && m_decimalModeEnabled)
  {
    if ((regA & 0xf) - carry < (data & 0xf)) /* EP */ 
      result -= 6;
    if (result > 0x99) result -= 0x60;
  }

  status.SetCarry(result < 0x100);
  regA = result & 0x00FF;

  //uint8_t carry = status.Carry() ? 1 : 0;
  //uint16_t result = regA + data + carry;
  //status.SetZero((result & 0xFF) == 0x00);
  //status.SetNegative(result & 0x0080);
  //status.SetCarry(result > 0xFF);
  //bool overflow = ((regA ^ result) & ~(regA ^ data)) & 0x0080;
  //status.SetOverflow(overflow);
  //regA = result & 0x00FF;
}

void R6502::SEC()
{
  status.SetCarry(true);
}

void R6502::SED()
{
  status.SetDecimalMode(true);
}

void R6502::SEI()
{
  status.SetIRQ_Disable(true);
}

void R6502::STA()
{
  WriteData(regA);
}

void R6502::STX()
{
  WriteData(regX);
}

void R6502::STY()
{
  WriteData(regY);
}

void R6502::TAX()
{
  regX = regA;
  status.SetZero(regX == 0x00);
  status.SetNegative(regX & 0x80);
}

void R6502::TAY()
{
  regY = regA;
  status.SetZero(regY == 0x00);
  status.SetNegative(regY & 0x80);
}

void R6502::TSX()
{
  regX = sp;
  status.SetZero(regX == 0x00);
  status.SetNegative(regX & 0x80);
}

void R6502::TXA()
{
  regA = regX;
  status.SetZero(regA == 0x00);
  status.SetNegative(regA & 0x80);
}

void R6502::TXS()
{
  sp = regX;
}

void R6502::TYA()
{
  regA = regY;
  status.SetZero(regA == 0x00);
  status.SetNegative(regA & 0x80);
}

////// Illegal opcodes:
void R6502::ANC()
{
  uint8_t data = FetchData();
  regA &= data;
  status.SetZero(regA == 0x00);
  status.SetCarry(regA & 0x80);
}

void R6502::DCP()
{
  uint8_t data = FetchData();
  data--;
  WriteData(data);
  status.SetZero(data == 0x00);

  uint8_t res = regA - data;
  status.SetNegative(res & 0x80);
  status.SetZero(res == 0x00);
  status.SetCarry(regA >= res);
}

void R6502::ISB()
{
  uint8_t data = FetchData();
  data++;
  WriteData(data);
  status.SetNegative(data & 0x80);
  status.SetZero(data == 0x00);

  uint8_t carry = status.Carry() ? 0 : 1;
  int16_t result = (int16_t)regA - (int16_t)data - (int16_t)carry;
  uint8_t res8 = (uint8_t)result;
  status.SetNegative(res8 & 0x80);
  status.SetZero((res8 & 0xFF) == 0x00);
  bool overflow = (((regA ^ data) & 0x0080) && ((regA ^ res8) & 0x0080));
  status.SetOverflow(overflow);
  status.SetCarry(result >= 0);
  regA = res8;
}

void R6502::JAM()
{
  cout << "JAM operation" << endl;
  assert(!"JAM operation");
}

void R6502::LAX()
{
  uint8_t data = FetchData();
  regA = data;
  regX = data;
  status.SetZero(data == 0x00);
  status.SetNegative(data & 0x80);
}

void R6502::RLA()
{
  uint8_t data = FetchData();
  bool carry = data & 0x80;
  data <<= 1;
  if (status.Carry())
    data |= 0x01;
  status.SetNegative(data & 0x80);
  status.SetZero(data == 0x00);
  status.SetCarry(carry);
  WriteData(data);

  regA &= data;
  status.SetZero(regA == 0x00);
  status.SetNegative(regA & 0x80);
}

void R6502::RRA()
{
  uint8_t data = FetchData();
  uint8_t carry = data & 0x01;
  data >>= 1;
  if (status.Carry())
    data |= 0x80;
  status.SetNegative(data & 0x80);
  status.SetZero(data == 0x00);
  status.SetCarry(carry);
  WriteData(data);

//  uint8_t carry = status.Carry() ? 1 : 0;
  uint16_t result = regA + data + carry;
  status.SetZero((result & 0xFF) == 0x00);
  if (status.DecimalMode() && m_decimalModeEnabled)
  {
    if (((regA & 0xf) + (data & 0xf) + carry) > 9)
      result += 6;
    status.SetNegative(result & 0x80);
    status.SetOverflow(!((regA ^ data) & 0x80) && ((regA ^ result) & 0x80));
    if (result > 0x99) result += 96;
    status.SetCarry(result > 0x99);
  }
  else
  {
    status.SetNegative(result & 0x80);
    status.SetCarry(result > 0xFF);
    bool overflow = ((regA ^ result) & ~(regA ^ data)) & 0x0080;
    status.SetOverflow(overflow);
  }
  regA = (uint8_t)(result & 0x00FF);
}

void R6502::SAX()
{
  WriteData(regA & regX);
}

void R6502::SLO()
{
  uint8_t data = FetchData();
  status.SetCarry(data & 0x80);
  data <<= 1;
  //status.SetZero(data == 0x00);
  //status.SetNegative(data & 0x80);
  WriteData((uint8_t)(data & 0x00FF));

  regA |= data;
  status.SetZero(regA == 0x00);
  status.SetNegative(regA & 0x80);
}

void R6502::SRE()
{
  uint8_t data = FetchData();
  status.SetCarry(data & 0x01);
  data >>= 1;
  status.SetZero(data == 0x00);
  status.SetNegative(data & 0x80);
  WriteData((uint8_t)(data & 0x00FF));

  regA ^= data;
  status.SetNegative(regA & 0x80);
  status.SetZero(regA == 0x00);
}

uint8_t R6502::BusRead(uint16_t address) const
{
  uint8_t data;
  if (!_bus.Read(address, data))
  {
    assert(!"R6502::BusRead: Failed to read from the bus.");
  }
  return data;
}

void R6502::BusWrite(uint16_t address, uint8_t data)
{
  if (!_bus.Write(address, data))
  {
    assert(!"R6502::BusWrite: Failed to write to the bus.");
  }
}

