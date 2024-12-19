#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include "IPixelWindow.h"
#include "ISoundSampleProcessor.h"
#include "Nes.h"
#include "CpuLogger.h"

#include <fstream>

class DummyPixelWindow : public IPixelWindow
{
  // Inherited via IPixelWindow
  void SetPixel(uint16_t x, uint16_t y, Color color) override
  {
    // Do nothing.
  }
};

class DummySoundSampleProcessor : public ISoundSampleProcessor
{
  // Inherited via ISoundSampleProcessor
  void AddSample(int16_t sample) override
  {
    // Do nothing.
  }
};

void CompareFiles()
{
  std::ifstream logFile;
  std::ifstream refFile;
  logFile.open("instructions.log");
  refFile.open("TestData/nestest.log");
  std::string logLine;
  std::string refLine;
  int line = 0;
  while (std::getline(logFile, logLine) && std::getline(refFile, refLine))
  {
    line++;
    if (logLine.substr(0, 73) != refLine.substr(0, 73))
    {
      FAIL(std::format("Loglines are different({}): \nlog:{}\nref:{}", line, logLine, refLine));
    }
  }

}

TEST_CASE("Run nestest.nes", "[nestest]")
{
  DummyPixelWindow dummyPixelWindow;
  DummySoundSampleProcessor dummySoundSampleProcessor;
  std::shared_ptr<CpuLogger> cpuLogger = std::make_shared<CpuLogger>("instructions.log");

  Nes nes(dummyPixelWindow, dummySoundSampleProcessor, cpuLogger);

  REQUIRE(nes.InsertCartridge("TestData/nestest.nes", 0xc000));
  auto& cpu = nes.GetCpu();

  try
  {
    while (cpu.getPc() != 0xC66E)
    {
      nes.Clock();
    }
    for (int i = 0; i < 18; ++i)
    {
      nes.Clock();
    }
  }
  catch (...)
  {
    REQUIRE(!"Exception happened!!!!");
  }
  uint8_t result = nes.ReadCpuBus(0x02);

  REQUIRE(result == 0x00);
  CompareFiles();
}