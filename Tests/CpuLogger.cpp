#include "CpuLogger.h"

CpuLogger::CpuLogger(const std::filesystem::path& filePath)
{
  m_logFile.open(filePath);
}

CpuLogger::~CpuLogger()
{
  m_logFile.close();
}
void CpuLogger::AddLogline(const CpuLogLine logLine)
{
  std::stringstream ss;
  ss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (uint16_t)logLine.opcode;
  for (auto d : logLine.data)
  {
    ss << " " << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (uint16_t)d;
  }

  m_logFile << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << logLine.PC << "  ";
  m_logFile << std::setfill(' ') << std::setw(9) << std::left << ss.str();
  m_logFile << (logLine.isInvalid ? "*" : " ") << logLine.instruction;
  m_logFile << " " << std::setfill(' ') << std::setw(27) << std::left << logLine.dataAddressString << std::right;
  m_logFile << " A:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (uint16_t)logLine.A;
  m_logFile << " X:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (uint16_t)logLine.X;
  m_logFile << " Y:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (uint16_t)logLine.Y;
  m_logFile << " P:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (uint16_t)logLine.P;
  m_logFile << " SP:" << std::hex << std::uppercase << std::setw(2) << std::setfill('0')  << (uint16_t)logLine.SP;
  m_logFile << " PPU:" << std::dec << std::setw(3) << std::setfill(' ') << std::right << logLine.ppuY << "," << std::setw(3) << std::setfill(' ') << std::right << logLine.ppuX;
  m_logFile << " CYC:" << logLine.cycle;
  m_logFile << std::endl;
}
