#pragma once

#include "Nes.h"

#include <SDL.h>

class KeyBoardController : public Component
{
public:
  KeyBoardController();

  void HandleEvent(const SDL_Event& event);

private:
  // Inherited via Component
  virtual uint8_t Read(uint16_t address) override;
  virtual void Write(uint16_t address, uint8_t data) override;

  void HandleButtonDown(SDL_GameControllerButton button);
  void HandleButtonUp(SDL_GameControllerButton button);

  void HandleKeyUp(SDL_Keycode key);
  void HandleKeyDown(SDL_Keycode key);

  uint8_t       m_controller1 = 0x00;
  uint8_t       m_controller2 = 0x00;
  uint8_t       m_controllerState1 = 0x00;
  uint8_t       m_controllerState2 = 0x00;
};
