//#include "pch.h"
//#include "CppUnitTest.h"
//#include "Components/R6502.h"
//#include "Components/Bus.h"
//#include "Components/RAM.h"
//#include <iostream>
//#include <sstream>
//#include <fstream>
//
//using namespace std;
//using namespace Microsoft::VisualStudio::CppUnitTestFramework;
//
//namespace R6502Tests
//{
//  TEST_CLASS(TestR6502)
//  {
//  public:
//    TEST_METHOD(TestOpcodes)
//    {
//      //std::unique_ptr<Bus> bus = make_unique<Bus>();
//      //R6502 cpu(*bus, false);
//      //unique_ptr<RAM> pRam = make_unique<RAM>(0x10000, (uint16_t)0x0000, (uint16_t)0xFFFF);
//      //bus->AddComponent(*pRam);
//
//      //std::ifstream fin("TestData/6502_functional_test.bin", std::ios::binary);
//      ////fin.read((char*)pRam->GetRamPtr(), 0x10000);
//      //cpu.pc = 0x0400;
//
//      //uint16_t prevPc;
//      //do
//      //{
//      //  prevPc = cpu.pc;
//      //  cpu.Clock();
//      //  //do
//      //  //{
//      //  //  cpu.Clock();
//      //  //} while (!cpu.Complete());
//
//      //  //if (cpu.pc == 0x336e)
//      //  //  break;
//      //} while (prevPc = cpu.pc);
//      //Assert::IsTrue(cpu.pc == 0x3469);
//    }
//  };
//}

#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

unsigned int Factorial(unsigned int number) {
  if (number == 0)
    return 1;
  return number <= 1 ? number : Factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed", "[factorial]") {
  REQUIRE(Factorial(0) == 1);
  REQUIRE(Factorial(1) == 1);
  REQUIRE(Factorial(2) == 2);
  REQUIRE(Factorial(3) == 6);
  REQUIRE(Factorial(10) == 3628800);
}