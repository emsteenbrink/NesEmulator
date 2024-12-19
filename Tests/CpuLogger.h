#pragma once

#include "ICpuLogger.h"

#include <fstream>
#include <filesystem>

class CpuLogger : public ICpuLogger
{
public:
  CpuLogger(const std::filesystem::path& filePath);
  virtual ~CpuLogger();

  // From ICpuLogger
  virtual void AddLogline(const CpuLogLine logLine) override;

private:
  std::ofstream m_logFile;                  // If enabled, we can log the instructions and state.
};