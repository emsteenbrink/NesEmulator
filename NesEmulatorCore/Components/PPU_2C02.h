#pragma once

#include "Component.h"
#include "ColorTable.h"
#include "IPixelWindow.h"
#include "Bus.h"

class PPU_2C02 : public Component
{
public:
  static const int SCREEN_WIDTH = 256;
  static const int SCREEN_HEIGHT = 240;

  PPU_2C02(IPixelWindow& window, Bus& ppuBus, Bus& cpuBus);

  // Inherited via Component
  virtual uint8_t Read(uint16_t address) override;
  virtual uint8_t Write(uint16_t address, uint8_t data) override;

  void Clock();
  bool CPUClock(unsigned int counter);

  uint8_t PpuBusReadDebug(uint16_t address) { return PpuRead(address); };

  bool nmi = false;

  int16_t GetCycle() const { return m_cycle; };
  int16_t GetScanLine() const { return m_scanline; };

private:
  uint8_t PpuRead(uint16_t address);
  void PpuWrite(uint16_t address, uint8_t data);

  void IncrementScrollX();
  void IncrementScrollY();
  void TransferAddressX();
  void TransferAddressY();

  void LoadBackgroundShifters();
  void UpdateShifters();

  enum Registers
  {
    PPUCTRL_2000 = 0x2000,
    PPUMASK_2001 = 0x2001,
    PPUSTATUS_2002 = 0x2002,
    OAMADDR_2003 = 0x2003,
    OAMDATA_2004 = 0x2004,
    PPUSCROLL_2005 = 0x2005,
    PPUADDR_2006 = 0x2006,
    PPUDATA_2007 = 0x2007,
    OAMDMA_4014 = 0x4014
  };

  struct PpuCtrl // 0x2000
  {
    uint8_t baseNameTableAddressX : 1;
    uint8_t baseNameTableAddressY : 1;
    uint8_t VRAM_Inc : 1;
    uint8_t SpritePatternTableAddress : 1;
    uint8_t BackGroundPatternTableAddress : 1;
    uint8_t SpriteSize : 1;
    uint8_t PPU_MasterSlaveSelect : 1;
    bool NMI_Enabled : 1;
  };

  static_assert(sizeof(PpuCtrl) == 1, "packing fails");

  struct PpuMask  // 0x2001
  {
    bool greyScale : 1;
    bool showBckGndLeft8Px : 1;
    bool showSpritesLeft8Px : 1;
    bool showBackGround : 1;
    bool showSprites : 1;
    bool emphasizeRed : 1;
    bool emphasizeGreen : 1;
    bool emphasizeBlue : 1;
  };
  static_assert(sizeof(PpuMask) == 1, "packing fails");

  struct PpuStatus       // 0x2002
  {
    uint8_t leastBitsWritten : 5;
    bool spriteOverflow : 1;
    bool sprite0Hit : 1;
    bool verticalBlankStarted : 1;
  };
  static_assert(sizeof(PpuStatus) == 1, "packing fails");
  struct VRamAddress
  {
    uint16_t coarseXScroll    : 5;
    uint16_t coarseYScroll    : 5;
    uint16_t nameTableSelectX : 1;
    uint16_t nameTableSelectY : 1;
    uint16_t fineYScroll      : 3;
    uint16_t                  : 1;
  };

  static_assert(sizeof(VRamAddress) == 2, "packing fails");

  PpuCtrl         m_ppuCtrl = {};
  uint8_t&        m_ppuCtrlReg = (uint8_t&)m_ppuCtrl;
  PpuMask         m_ppuMask = {};
  uint8_t&        m_ppuMaskReg = (uint8_t&)(m_ppuMask);
  PpuStatus       m_ppuStatus = {};
  uint8_t&        m_ppuStatusReg = (uint8_t&)(m_ppuStatus);

  int16_t         m_scanline = 0;
  int16_t         m_cycle = -4;
  bool            m_oddFrame = false;

  VRamAddress     m_currentVRamAddress = {};
  uint16_t&       m_currentVRamAddressReg = (uint16_t&) m_currentVRamAddress;
  VRamAddress     m_tempVRAMAddress = {};
  uint16_t&       m_tempVRAMAddressReg = (uint16_t&) m_tempVRAMAddress;
  uint8_t         m_fineScrollX = 0x00;
  bool            m_firstWrite = true;
  uint8_t         m_delayedData = 0x00;

  IPixelWindow&   m_window;
  Bus&            m_ppuBus;
  Bus&            m_cpuBus; // needed for DMA;

  uint8_t         m_tblPalette[32] = {}; // The palette table is internal.

  uint8_t         m_bgNextTileId      = 0x00;
  uint8_t         m_bgNextTileAttrib  = 0x00;
  uint8_t         m_bgNextTileLsb     = 0x00;
  uint8_t         m_bgNextTileMsb     = 0x00;
  uint16_t        m_bgShifterPatternLo  = 0x0000;
  uint16_t        m_bgShifterPatternHi  = 0x0000;
  uint16_t        m_bgShifterAttribLo   = 0x0000;
  uint16_t        m_bgShifterAttribHi   = 0x0000;

  struct ObjectAttributeItem
  {
    uint8_t yPos = 0x00;
    uint8_t tileIndex = 0x00;			// ID of tile from pattern memory
    uint8_t attribute = 0x00;	// Flags define how sprite should be rendered
    uint8_t x = 0x00;			// X position of sprite
  };

  // A register to store the address when the CPU manually communicates
  // with OAM via PPU registers. This is not commonly used because it 
  // is very slow, and instead a 256-Byte DMA transfer is used. See
  // the Bus header for a description of this.
  uint8_t             m_oamAddress = 0x00;
  uint8_t             m_oamDmaHigh = 0x00;
  ObjectAttributeItem m_oam[64];
  static_assert(sizeof(m_oam) == 64 * 4, "blah");

  // The OAM is conveniently package above to work with, but the DMA
  // mechanism will need access to it for writing one byute at a time
  uint8_t*            m_pOAM = (uint8_t*)m_oam;

  ObjectAttributeItem m_spriteScanline[8];
  uint8_t m_spriteCount = 0;
  uint8_t m_spriteShifterPatternLo[8] = {};
  uint8_t m_spriteShifterPatternHi[8] = {};

  // Sprite Zero Collision Flags
  bool m_spriteZeroHitPossible = false;
  bool m_spriteZeroBeingRendered = false;

  // A simple form of Direct Memory Access is used to swiftly
// transfer data from CPU bus memory into the OAM memory. It would
// take too long to sensibly do this manually using a CPU loop, so
// the program prepares a page of memory with the sprite info required
// for the next frame and initiates a DMA transfer. This suspends the
// CPU momentarily while the PPU gets sent data at PPU clock speeds.
// Note here, that dma_page and dma_addr form a 16-bit address in 
// the CPU bus address space
  uint8_t m_dmaPage = 0x00;
  uint8_t m_dmaAddress = 0x00;
  uint8_t m_dmaData = 0x00;

  // DMA transfers need to be timed accurately. In principle it takes
  // 512 cycles to read and write the 256 bytes of the OAM memory, a
  // read followed by a write. However, the CPU needs to be on an "even"
  // clock cycle, so a dummy cycle of idleness may be required
  bool m_dmaDummy = true;

  // Finally a flag to indicate that a DMA transfer is happening
  bool m_dmaTransfer = false;
};