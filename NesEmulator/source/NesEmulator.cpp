#include "NesEmulator.h"

#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;


NesEmulator::NesEmulator() 
  : m_mainWindow(PPU_2C02::SCREEN_WIDTH, PPU_2C02::SCREEN_HEIGHT)
  , m_soundController()
  , m_nes(m_mainWindow, m_soundController.GetSoundSampleProcessor())
{
  m_nes.AddController(m_keyboardController);
  
  fs::path cartridgePath;
  // Test
  //cartridgePath = "C:/Data/Test Roms/test_apu_env.nes";
  //cartridgePath = "C:/Data/Test Roms/Sound/volume_tests/volumes.nes";
  //cartridgePath = "C:/Data/Test Roms/Sound/apu_mixer/square.nes";
  //cartridgePath = "C:/Data/Test Roms/Sound/dmc_test/status_irq.nes";
  //cartridgePath = "C:/Data/Test Roms/apu_test.nes"; 
  //cartridgePath = "C:/Data/Roms/nestest.nes";
  //cartridgePath = "C:/Data/Test Roms/nes-test-roms/branch_timing_tests/1.Branch_Basics.nes";
  //cartridgePath = "C:/Data/Test Roms/nes-test-roms/branch_timing_tests/2.Backward_Branch.nes";
  //cartridgePath = "C:/Data/Test Roms/nes-test-roms/branch_timing_tests/3.Forward_Branch.nes";
  //Needs mapper 003: cartridgePath = "C:/Data/Test Roms/nes-test-roms/cpu_dummy_reads/cpu_dummy_reads.nes";
  //fail: cartridgePath = "C:/Data/Test Roms/nes-test-roms/cpu_dummy_writes/cpu_dummy_writes_oam.nes";
  //fail: cartridgePath = "C:/Data/Test Roms/nes-test-roms/cpu_dummy_writes/cpu_dummy_writes_ppumem.nes";


  // ???
  //Fail: cartridgePath = "C:/Data/Roms/Donkey Kong 3 (JUE) [p1].nes";  
  //cartridgePath = "C:/Data/Roms/Exed Exes (J) [p1].nes";
  //cartridgePath = "C:/Data/Roms/Kung Fu (U).nes";
  //cartridgePath = "C:/Data/Roms/Pac-Man (U) [!].nes";
  //cartridgePath = "C:/Data/Roms/Excitebike (E).nes";
  //cartridgePath = "C:/Data/Roms/Elevator Action (U).nes";
  //cartridgePath = "C:/Data/Roms/Donkey Kong (World) (Rev A).nes";
  //cartridgePath = "C:/Data/Roms/Adventures of Lolo (USA).nes";

  // Mapper 000
  cartridgePath = "c:/Data/Roms/Super_mario_brothers.nes";

  // Mapper 001
  //cartridgePath = "C:/Data/Roms/Bomberman 2 (U).nes";
  //cartridgePath = "C:/Data/Roms/The Addams Family (U).nes";
  //cartridgePath = "C:/Data/Roms/The Chessmaster (U).nes";

  // Mapper 003
  //cartridgePath = "C:/Data/Roms/Donkey Kong Classics (U).nes";

  bool catridgeOk = m_nes.InsertCartridge(cartridgePath);
  if (!catridgeOk)
  {
    SDL_ShowSimpleMessageBox(0, "Error", m_nes.getCartridgeError().c_str(), NULL);
    return;
  }
  m_emulatorThreadActive = true;
  m_emulatorThread = std::thread(&NesEmulator::RunEmulator, this);
}

NesEmulator::~NesEmulator()
{
  if (m_emulatorThreadActive)
  {
    m_emulatorThreadActive = false;
    m_emulatorThread.join();
  }
}

void NesEmulator::Run()
{
  static Uint32 next_time;

  while (m_running)
  {
    HandleEvents();
    m_mainWindow.Render();
    
    // some delay to get to about 60 frames a second
    Uint32 now = SDL_GetTicks();
    if (next_time <= now)
      SDL_Delay(0);
    else
      SDL_Delay(next_time - now);
    next_time += 20;
  }
}

void NesEmulator::HandleEvents()
{
  SDL_Event event;
  if (SDL_PollEvent(&event))
  {
    if (event.type == SDL_QUIT)
      m_running = false;
    else
      m_keyboardController.HandleEvent(event);
  }
}

void NesEmulator::RunEmulator()
{
  long long residualTime = 0;

  auto last = std::chrono::high_resolution_clock::now();
  auto now = std::chrono::high_resolution_clock::now();
  long long increment = (long long)(1.0 / (21477272.0 / 4.0) * 1000000000.0);

  while (m_emulatorThreadActive)
  {
    now = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(now - last).count();
    last = now;
    residualTime -= elapsedTime;
    if (residualTime < 0)
    {
      residualTime += increment;
      m_nes.Clock();
    }
  }
}
