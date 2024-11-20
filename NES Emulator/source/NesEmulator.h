#pragma once

#include "Nes.h"
#include "KeyboardController.h"
#include "MainWindow.h"

#include <thread>

#include <SDL.h>


class NesEmulator
{
public:
  NesEmulator();
  virtual ~NesEmulator();

  void Run();

private:
  void HandleEvents();
  void RunEmulator();

  bool                m_running = true;
  std::thread         m_emulatorThread;
  std::atomic_bool    m_emulatorThreadActive;

  Nes                 m_nes;
  MainWindow          m_mainWindow;
  KeyBoardController  m_keyboardController;
};