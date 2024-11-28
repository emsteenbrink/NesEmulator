#pragma once

#include "Nes.h"
#include "KeyboardController.h"
#include "SoundController.h"
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

  MainWindow          m_mainWindow;
  KeyBoardController  m_keyboardController;
  SoundController     m_soundController;
  Nes                 m_nes;
};