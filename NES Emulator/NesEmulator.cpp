#define OLC_PGE_APPLICATION
#include "NesEmulator.h"

#include "SoundDriver.h"

const int SCREEN_WIDTH = 256;
const int SCREEN_HEIGHT = 240;

void checkJoy()
{
  // Variables for controllers and joysticks
  SDL_GameController* controller = NULL;
  SDL_Joystick* joy = NULL;

  // Enumerate joysticks
  for (int i = 0; i < SDL_NumJoysticks(); ++i) {

    // Check to see if joystick supports SDL's game controller interface
    if (SDL_IsGameController(i)) {
      controller = SDL_GameControllerOpen(i);
      if (controller) {
        printf("Found a valid controller, named: %s\n",
          SDL_GameControllerName(controller));
        break;  // Break after first available controller
      }
      else {
        printf("Could not open game controller %i: %s\n", i, SDL_GetError());
      }

      // Controller interface not supported, try to open as joystick
    }
    else {
      printf("Joystick %i is not supported by the game controller interface", i);
      joy = SDL_JoystickOpen(i);

      // Joystick is valid
      if (joy) {
        printf(
          "Opened Joystick %i\n"
          "Name: %s\n"
          "Axes: %d\n"
          "Buttons: %d\n"
          "Balls: %d\n",
          i, SDL_JoystickName(joy), SDL_JoystickNumAxes(joy),
          SDL_JoystickNumButtons(joy), SDL_JoystickNumBalls(joy)
        );

        // Joystick not valid
      }
      else {
        printf("Could not open Joystick %i", i);
      }

      break;  // Break after first available joystick
    }
  }
}

NesEmulator::NesEmulator() 
  : m_nes(m_mainWindow)
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
  {
    cout << "SDL could not initialize! SDL_Error:" <<  SDL_GetError() << endl;
    return;;
  }
  checkJoy();
  //Create window
  m_window = SDL_CreateWindow("Nes Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH*3, SCREEN_HEIGHT *3, SDL_WINDOW_SHOWN| SDL_WINDOW_RESIZABLE);
  if (m_window == nullptr)
  {
    cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
    return;
  }

  m_renderer = SDL_CreateRenderer(m_window, -1, 0);
  if (m_renderer == nullptr)
  {
    cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
    return;
  }
  SDL_RenderSetLogicalSize(m_renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
  m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);

  m_nes.AddController(m_keyboardController);

  // Test
  //m_nes.InsertCartridge("D:/Nes/Test Roms/test_apu_env.nes");
  //m_nes.InsertCartridge("D:/Nes/Test Roms/Sound/volume_tests/volumes.nes");
  //m_nes.InsertCartridge("D:/Nes/Test Roms/Sound/apu_mixer/square.nes");
  //m_nes.InsertCartridge("D:/Nes/Test Roms/Sound/dmc_test/status_irq.nes");
  //m_nes.InsertCartridge("D:/Nes/Roms/nestest.nes");
  //m_nes.InsertCartridge(R"(D:\Nes\Test Roms\nes-test-roms\branch_timing_tests\1.Branch_Basics.nes)");
  //m_nes.InsertCartridge(R"(D:\Nes\Test Roms\nes-test-roms\branch_timing_tests\2.Backward_Branch.nes)");
  //m_nes.InsertCartridge(R"(D:\Nes\Test Roms\nes-test-roms\branch_timing_tests\3.Forward_Branch.nes)");
  //Needs mapper 003: m_nes.InsertCartridge(R"(D:\Nes\Test Roms\nes-test-roms\cpu_dummy_reads\cpu_dummy_reads.nes)");
  //fail: m_nes.InsertCartridge(R"(D:\Nes\Test Roms\nes-test-roms\cpu_dummy_writes\cpu_dummy_writes_oam.nes)");
  m_nes.InsertCartridge(R"(D:\Nes\Test Roms\nes-test-roms\cpu_dummy_writes\cpu_dummy_writes_ppumem.nes)");


  // ???
  //m_nes.InsertCartridge("D:/NES/Roms/Donkey Kong 3 (JUE) [p1].nes");  
  //m_nes.InsertCartridge("D:/NES/Roms/Exed Exes (J) [p1].nes");
  //m_nes.InsertCartridge("D:/NES/Roms/Kung Fu (U).nes");
  //m_nes.InsertCartridge("D:/NES/Roms/Pac-Man (U) [!].nes");
  //m_nes.InsertCartridge("D:/NES/Roms/Excitebike (E).nes");
  //m_nes.InsertCartridge("D:/NES/Roms/Elevator Action (U).nes");
  //m_nes.InsertCartridge("D:/NES/Roms/Donkey Kong (World) (Rev A).nes");
  
  // Mapper 000
  //m_nes.InsertCartridge("D:/NES/Roms/Super_mario_brothers.nes");

  // Mapper 001
  //m_nes.InsertCartridge("D:/Nes/Roms/Bomberman 2 (U).nes");
  //m_nes.InsertCartridge("D:/Nes/Roms/The Addams Family (U).nes");
  //m_nes.InsertCartridge("D:/Nes/Roms/The Chessmaster (U).nes");
  
  m_emulatorThreadActive = true;
  m_emulatorThread = std::thread(&NesEmulator::RunEmulator, this);
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

  m_windowVisible = true;
}

NesEmulator::~NesEmulator()
{
  m_emulatorThreadActive = false;
  m_emulatorThread.join();
}

void NesEmulator::Run()
{
  static Uint32 next_time;

  while (m_windowVisible)
  {
    HandleEvents();
    Update();
    Render();
    
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
    switch (event.type)
    {
    case SDL_QUIT:
      m_windowVisible = false;
      break;
    case SDL_KEYDOWN:
      if (event.key.repeat == 0)
      {
        m_keyboardController.HandleKeyDown(event.key.keysym.sym);
      }
      break;
    case SDL_KEYUP:
      if (event.key.repeat == 0)
      {
        m_keyboardController.HandleKeyUp(event.key.keysym.sym);
      }
      break;
    case SDL_CONTROLLERBUTTONDOWN:
      m_keyboardController.HandleButtonDown((SDL_GameControllerButton)event.cbutton.button);
      break;
    case SDL_CONTROLLERBUTTONUP:
      m_keyboardController.HandleButtonUp((SDL_GameControllerButton)event.cbutton.button);
      break;
    }
  }
}

void NesEmulator::Update()
{
}

extern uint8_t g_overrrun;
extern uint8_t g_underrun;

void NesEmulator::Render()
{

  SDL_UpdateTexture(m_texture, nullptr, m_mainWindow.GetData(), SCREEN_WIDTH * sizeof(uint32_t));

  SDL_RenderClear(m_renderer);
  SDL_RenderCopy(m_renderer, m_texture, nullptr, nullptr);
  if (g_overrrun)
  {
    SDL_Rect rect;
    rect.x = 10;
    rect.y = 10;
    rect.w = 5;
    rect.h = 5;
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 0);
    SDL_RenderFillRect(m_renderer, &rect);
  }
  if (g_underrun)
  {
    SDL_Rect rect;
    rect.x = 10;
    rect.y = 10;
    rect.w = 5;
    rect.h = 5;
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 0);
    SDL_RenderFillRect(m_renderer, &rect);
  }
  SDL_RenderPresent(m_renderer);
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
    auto elapsedTime = chrono::duration_cast<chrono::nanoseconds>(now - last).count();
    last = now;
    if (residualTime > 0)
      residualTime -= elapsedTime;
    else
    {
      residualTime += increment - elapsedTime;
      m_nes.Clock();
    }
  }
}


std::string NesEmulator::Hex(uint32_t n, uint8_t d)
{
  std::string s(d, '0');
  for (int i = d - 1; i >= 0; i--, n >>= 4)
    s[i] = "0123456789ABCDEF"[n & 0xF];
  return s;
}

