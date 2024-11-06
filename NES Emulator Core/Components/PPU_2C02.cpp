#include "PPU_2C02.h"
#include <iostream>
using namespace std;

PPU_2C02::PPU_2C02(IPixelWindow& window, Bus& ppuBus, Bus& cpuBus)
  : m_window(window)
  , m_ppuBus(ppuBus)
  , m_cpuBus(cpuBus)
  , m_oddFrame(false)
{
  AddAddressRange(0x2000, 0x3FFF);
  AddAddress(0x4014);
  m_currentVRamAddressReg = 0x00;
  m_tempVRAMAddress.coarseXScroll = 0x12;
  m_tempVRAMAddressReg = 0x00;
}

uint8_t PPU_2C02::Read(uint16_t address)
{
  uint8_t result = 0x00;

  assert(address == 0x4014 || (address >= 0x2000 && address <= 0x3FFF));
  if (address != 0x4014)
  {
    address = (address & 0x0007U) + 0x2000U;
  }

  switch (address)
  {
  case PPUSTATUS_2002:
    m_firstWrite = true;
    result = m_ppuStatusReg & 0xE0 | (m_delayedData & 0x1F);
    m_ppuStatus.verticalBlankStarted = false;
    break;
  case OAMDATA_2004:
    result = m_pOAM[m_oamAddress];
    break;
  case PPUDATA_2007:
    result = m_delayedData;
    // then update the buffer for next time
    m_delayedData = PpuRead(m_currentVRamAddressReg);
    // However, if the address was in the palette range, the
    // data is not delayed, so it returns immediately
    if (m_currentVRamAddressReg >= 0x3F00) result = m_delayedData;
    // All reads from PPU data automatically increment the nametable
    // address depending upon the mode set in the control register.
    // If set to vertical mode, the increment is 32, so it skips
    // one whole nametable row; in horizontal mode it just increments
    // by 1, moving to the next column
    m_currentVRamAddressReg += (m_ppuCtrl.VRAM_Inc ? 32 : 1);
    break;

    // These registers cannot be read.
  case PPUCTRL_2000:
    assert(!"Read from PPUCTRL ???");
  case PPUMASK_2001:
    assert(!"Read from PPUMASK ???");
  case OAMADDR_2003:
    assert(!"Read from OAMADDR ???");
  case PPUSCROLL_2005:
    assert(!"Read from PPUSCROLL ???");
  case PPUADDR_2006:
    assert(!"Read from PPUADDR ???");
  case OAMDMA_4014:
    assert(!"Read from OAMDMA ???");
  default:
    assert(!"PPU_2C02::Read Unknown address");
  }
  return result;
}

void PPU_2C02::Write(uint16_t address, uint8_t data)
{
  assert(address == 0x4014 || (address >= 0x2000 && address <= 0x3FFF));
  if (address != 0x4014)
  {
    address = (address & 0x0007U) + 0x2000U;
  }

  switch (address)
  {
  case PPUCTRL_2000:
    m_ppuCtrlReg = data; 
    m_tempVRAMAddress.nameTableSelectX = m_ppuCtrl.baseNameTableAddressX;
    m_tempVRAMAddress.nameTableSelectY = m_ppuCtrl.baseNameTableAddressY;
    break;
  case PPUMASK_2001:
    m_ppuMaskReg = data;
    break;
  case OAMADDR_2003:
    m_oamAddress = data;
    break;
  case OAMDATA_2004:
    m_pOAM[m_oamAddress] = data;
    break;
  case PPUSCROLL_2005:
    if (m_firstWrite)
    {
      m_tempVRAMAddress.coarseXScroll = data >> 3;
      m_fineScrollX = data & 0x07;
    }
    else
    {
      m_tempVRAMAddress.fineYScroll = data & 0x07U;
      m_tempVRAMAddress.coarseYScroll = data >> 3;
    }
    m_firstWrite = !m_firstWrite;
    break;
  case PPUADDR_2006:
    if (m_firstWrite)
    {
      m_tempVRAMAddressReg = (m_tempVRAMAddressReg & 0x00FF) | (data & 0x7F) << 8;
    }
    else
    {
      m_tempVRAMAddressReg = (m_tempVRAMAddressReg & 0xFF00) | data;
      m_currentVRamAddress = m_tempVRAMAddress;
    }
    m_firstWrite = !m_firstWrite;
    break;
  case PPUDATA_2007:
    PpuWrite(m_currentVRamAddressReg, data);
    m_currentVRamAddressReg += (m_ppuCtrl.VRAM_Inc ? 32 : 1);
    break;
  case OAMDMA_4014:
    m_dmaPage = data;
    m_dmaAddress = 0x00;
    m_dmaTransfer = true;
    break;
  case PPUSTATUS_2002: // cannot be written to.
    assert(!"Write to PPUCTRL ???");
  default:
    assert(!"PPU_2C02::Write Unknown address");
  }
}


void PPU_2C02::IncrementScrollX()
{
  if (m_ppuMask.showBackGround || m_ppuMask.showSprites)
  {
    if (m_currentVRamAddress.coarseXScroll == 31)
    {
      // Leaving nametable so wrap address round
      m_currentVRamAddress.coarseXScroll = 0;
      // Flip target nametable bit
      m_currentVRamAddress.nameTableSelectX = ~m_currentVRamAddress.nameTableSelectX;
    }
    else
    {
      // Staying in current nametable, so just increment
      m_currentVRamAddress.coarseXScroll++;
    }
  }
};

// ==============================================================================
// Increment the background tile "pointer" one scanline vertically
void PPU_2C02::IncrementScrollY()
{
  // Incrementing vertically is more complicated. The visible nametable
  // is 32x30 tiles, but in memory there is enough room for 32x32 tiles.
  // The bottom two rows of tiles are in fact not tiles at all, they
  // contain the "attribute" information for the entire table. This is
  // information that describes which palettes are used for different 
  // regions of the nametable.

  // In addition, the NES doesnt scroll vertically in chunks of 8 pixels
  // i.e. the height of a tile, it can perform fine scrolling by using
  // the fine_y component of the register. This means an increment in Y
  // first adjusts the fine offset, but may need to adjust the whole
  // row offset, since fine_y is a value 0 to 7, and a row is 8 pixels high

  // Ony if rendering is enabled
  if (m_ppuMask.showBackGround || m_ppuMask.showSprites)
  {
    // If possible, just increment the fine y offset
    if (m_currentVRamAddress.fineYScroll < 7)
    {
      m_currentVRamAddress.fineYScroll++;
    }
    else
    {
      // If we have gone beyond the height of a row, we need to
      // increment the row, potentially wrapping into neighbouring
      // vertical nametables. Dont forget however, the bottom two rows
      // do not contain tile information. The coarse y offset is used
      // to identify which row of the nametable we want, and the fine
      // y offset is the specific "scanline"

      // Reset fine y offset
      m_currentVRamAddress.fineYScroll = 0;

      // Check if we need to swap vertical nametable targets
      if (m_currentVRamAddress.coarseYScroll == 29)
      {
        // We do, so reset coarse y offset
        m_currentVRamAddress.coarseYScroll = 0;
        // And flip the target nametable bit
        m_currentVRamAddress.nameTableSelectY = ~m_currentVRamAddress.nameTableSelectY;
      }
      else if (m_currentVRamAddress.coarseYScroll == 31)
      {
        // In case the pointer is in the attribute memory, we
        // just wrap around the current nametable
        m_currentVRamAddress.coarseYScroll = 0;
      }
      else
      {
        // None of the above boundary/wrapping conditions apply
        // so just increment the coarse y offset
        m_currentVRamAddress.coarseYScroll++;
      }
    }
  }
};

// ==============================================================================
// Transfer the temporarily stored horizontal nametable access information
// into the "pointer". Note that fine x scrolling is not part of the "pointer"
// addressing mechanism
void PPU_2C02::TransferAddressX()
{
  // Ony if rendering is enabled
  if (m_ppuMask.showBackGround || m_ppuMask.showSprites)
  {
    m_currentVRamAddress.nameTableSelectX = m_tempVRAMAddress.nameTableSelectX;
    m_currentVRamAddress.coarseXScroll = m_tempVRAMAddress.coarseXScroll;
  }
};

// ==============================================================================
// Transfer the temporarily stored vertical nametable access information
// into the "pointer". Note that fine y scrolling is part of the "pointer"
// addressing mechanism
void PPU_2C02::TransferAddressY()
{
  // Ony if rendering is enabled
  if (m_ppuMask.showBackGround || m_ppuMask.showSprites)
  {
    m_currentVRamAddress.fineYScroll = m_tempVRAMAddress.fineYScroll;
    m_currentVRamAddress.nameTableSelectY = m_tempVRAMAddress.nameTableSelectY;
    m_currentVRamAddress.coarseYScroll = m_tempVRAMAddress.coarseYScroll;
  }
};


// ==============================================================================
// Prime the "in-effect" background tile shifters ready for outputting next
// 8 pixels in scanline.
void PPU_2C02::LoadBackgroundShifters()
{
  // Each PPU update we calculate one pixel. These shifters shift 1 bit along
  // feeding the pixel compositor with the binary information it needs. Its
  // 16 bits wide, because the top 8 bits are the current 8 pixels being drawn
  // and the bottom 8 bits are the next 8 pixels to be drawn. Naturally this means
  // the required bit is always the MSB of the shifter. However, "fine x" scrolling
  // plays a part in this too, whcih is seen later, so in fact we can choose
  // any one of the top 8 bits.
  m_bgShifterPatternLo = (m_bgShifterPatternLo & 0xFF00U) | m_bgNextTileLsb;
  m_bgShifterPatternHi = (m_bgShifterPatternHi & 0xFF00U) | m_bgNextTileMsb;

  // Attribute bits do not change per pixel, rather they change every 8 pixels
  // but are synchronised with the pattern shifters for convenience, so here
  // we take the bottom 2 bits of the attribute word which represent which 
  // palette is being used for the current 8 pixels and the next 8 pixels, and 
  // "inflate" them to 8 bit words.
  m_bgShifterAttribLo = (m_bgShifterAttribLo & 0xFF00U) | ((m_bgNextTileAttrib & 0b01) ? 0xFFU : 0x00U);
  m_bgShifterAttribHi = (m_bgShifterAttribHi & 0xFF00U) | ((m_bgNextTileAttrib & 0b10) ? 0xFFU : 0x00U);
}

// ==============================================================================
// Every cycle the shifters storing pattern and attribute information shift
// their contents by 1 bit. This is because every cycle, the output progresses
// by 1 pixel. This means relatively, the state of the shifter is in sync
// with the pixels being drawn for that 8 pixel section of the scanline.
void PPU_2C02::UpdateShifters()
{
  if (m_ppuMask.showBackGround)
  {
    // Shifting background tile pattern row
    m_bgShifterPatternLo <<= 1;
    m_bgShifterPatternHi <<= 1;

    // Shifting palette attributes by 1
    m_bgShifterAttribLo <<= 1;
    m_bgShifterAttribHi <<= 1;
  }

  if (m_ppuMask.showSprites && m_cycle >= 1 && m_cycle < 258)
  {
    for (int i = 0; i < m_spriteCount; i++)
    {
      if (m_spriteScanline[i].x > 0)
      {
        m_spriteScanline[i].x--;
      }
      else
      {
        m_spriteShifterPatternLo[i] <<= 1;
        m_spriteShifterPatternHi[i] <<= 1;
      }
    }
  }
};

void PPU_2C02::Clock()
{

  // All but 1 of the secanlines is visible to the user. The pre-render m_scanline
// at -1, is used to configure the "shifters" for the first visible m_scanline, 0.
  if (m_scanline >= -1 && m_scanline < 240)
  {
    if (m_scanline == 0 && m_cycle == 0 && m_oddFrame && (m_ppuMask.showBackGround || m_ppuMask.showSprites))
    {
      // "Odd Frame" m_cycle skip
      m_cycle = 1;
    }

    if (m_scanline == -1 && m_cycle == 1)
    {
      // Effectively start of new frame, so clear vertical blank flag
      m_ppuStatus.verticalBlankStarted = false;

      // Clear sprite overflow flag
      m_ppuStatus.spriteOverflow = false;

      // Clear the sprite zero hit flag
      m_ppuStatus.sprite0Hit = false;

      // Clear Shifters
      for (int i = 0; i < 8; i++)
      {
        m_spriteShifterPatternLo[i] = 0;
        m_spriteShifterPatternHi[i] = 0;
      }
    }


    if ((m_cycle >= 2 && m_cycle < 258) || (m_cycle >= 321 && m_cycle < 338))
    {
      UpdateShifters();


      // In these cycles we are collecting and working with visible data
      // The "shifters" have been preloaded by the end of the previous
      // m_scanline with the data for the start of this m_scanline. Once we
      // leave the visible region, we go dormant until the shifters are
      // preloaded for the next m_scanline.

      // Fortunately, for background rendering, we go through a fairly
      // repeatable sequence of events, every 2 clock cycles.
      switch ((m_cycle - 1) % 8)
      {
      case 0:
        // Load the current background tile pattern and attributes into the "shifter"
        LoadBackgroundShifters();

        // Fetch the next background tile ID
        // "(m_currentVRamAddressReg & 0x0FFF)" : Mask to 12 bits that are relevant
        // "| 0x2000"                 : Offset into nametable space on PPU address bus
        m_bgNextTileId = PpuRead(0x2000 | (m_currentVRamAddressReg & 0x0FFF));

        // Explanation:
        // The bottom 12 bits of the loopy register provide an index into
        // the 4 nametables, regardless of nametable mirroring configuration.
        // nametable_y(1) nametable_x(1) coarseYScroll(5) coarse_x(5)
        //
        // Consider a single nametable is a 32x32 array, and we have four of them
        //   0                1
        // 0 +----------------+----------------+
        //   |                |                |
        //   |                |                |
        //   |    (32x32)     |    (32x32)     |
        //   |                |                |
        //   |                |                |
        // 1 +----------------+----------------+
        //   |                |                |
        //   |                |                |
        //   |    (32x32)     |    (32x32)     |
        //   |                |                |
        //   |                |                |
        //   +----------------+----------------+
        //
        // This means there are 4096 potential locations in this array, which 
        // just so happens to be 2^12!
        break;
      case 2:
        // Fetch the next background tile attribute. OK, so this one is a bit
        // more involved :P

        // Recall that each nametable has two rows of cells that are not tile 
        // information, instead they represent the attribute information that
        // indicates which palettes are applied to which area on the screen.
        // Importantly (and frustratingly) there is not a 1 to 1 correspondance
        // between background tile and palette. Two rows of tile data holds
        // 64 attributes. Therfore we can assume that the attributes affect
        // 8x8 zones on the screen for that nametable. Given a working resolution
        // of 256x240, we can further assume that each zone is 32x32 pixels
        // in screen space, or 4x4 tiles. Four system palettes are allocated
        // to background rendering, so a palette can be specified using just
        // 2 bits. The attribute byte therefore can specify 4 distinct palettes.
        // Therefore we can even further assume that a single palette is
        // applied to a 2x2 tile combination of the 4x4 tile zone. The very fact
        // that background tiles "share" a palette locally is the reason why
        // in some games you see distortion in the colours at screen edges.

        // As before when choosing the tile ID, we can use the bottom 12 bits of
        // the loopy register, but we need to make the implementation "coarser"
        // because instead of a specific tile, we want the attribute byte for a 
        // group of 4x4 tiles, or in other words, we divide our 32x32 address
        // by 4 to give us an equivalent 8x8 address, and we offset this address
        // into the attribute section of the target nametable.

        // Reconstruct the 12 bit loopy address into an offset into the
        // attribute memory

        // "(m_currentVRamAddress.coarse_x >> 2)"        : integer divide coarse x by 4, 
        //                                      from 5 bits to 3 bits
        // "((m_currentVRamAddress.coarseYScroll >> 2) << 3)" : integer divide coarse y by 4, 
        //                                      from 5 bits to 3 bits,
        //                                      shift to make room for coarse x

        // Result so far: YX00 00yy yxxx

        // All attribute memory begins at 0x03C0 within a nametable, so OR with
        // result to select target nametable, and attribute byte offset. Finally
        // OR with 0x2000 to offset into nametable address space on PPU bus.				
        m_bgNextTileAttrib = PpuRead(0x23C0 | (m_currentVRamAddress.nameTableSelectY << 11)
          | (m_currentVRamAddress.nameTableSelectX << 10)
          | ((m_currentVRamAddress.coarseYScroll >> 2) << 3)
          | (m_currentVRamAddress.coarseXScroll >> 2));

        // Right we've read the correct attribute byte for a specified address,
        // but the byte itself is broken down further into the 2x2 tile groups
        // in the 4x4 attribute zone.

        // The attribute byte is assembled thus: BR(76) BL(54) TR(32) TL(10)
        //
        // +----+----+			    +----+----+
        // | TL | TR |			    | ID | ID |
        // +----+----+ where TL =   +----+----+
        // | BL | BR |			    | ID | ID |
        // +----+----+			    +----+----+
        //
        // Since we know we can access a tile directly from the 12 bit address, we
        // can analyse the bottom bits of the coarse coordinates to provide us with
        // the correct offset into the 8-bit word, to yield the 2 bits we are
        // actually interested in which specifies the palette for the 2x2 group of
        // tiles. We know if "coarse y % 4" < 2 we are in the top half else bottom half.
        // Likewise if "coarse x % 4" < 2 we are in the left half else right half.
        // Ultimately we want the bottom two bits of our attribute word to be the
        // palette selected. So shift as required...				
        if (m_currentVRamAddress.coarseYScroll & 0x02) m_bgNextTileAttrib >>= 4;
        if (m_currentVRamAddress.coarseXScroll & 0x02) m_bgNextTileAttrib >>= 2;
        m_bgNextTileAttrib &= 0x03;
        break;

        // Compared to the last two, the next two are the easy ones... :P

      case 4:
        // Fetch the next background tile LSB bit plane from the pattern memory
        // The Tile ID has been read from the nametable. We will use this id to 
        // index into the pattern memory to find the correct sprite (assuming
        // the sprites lie on 8x8 pixel boundaries in that memory, which they do
        // even though 8x16 sprites exist, as background tiles are always 8x8).
        //
        // Since the sprites are effectively 1 bit deep, but 8 pixels wide, we 
        // can represent a whole sprite row as a single byte, so offsetting
        // into the pattern memory is easy. In total there is 8KB so we need a 
        // 13 bit address.

        // "(control.pattern_background << 12)"  : the pattern memory selector 
        //                                         from control register, either 0K
        //                                         or 4K offset
        // "((uint16_t)m_bgNextTileId << 4)"    : the tile id multiplied by 16, as
        //                                         2 lots of 8 rows of 8 bit pixels
        // "(m_currentVRamAddress.fine_y)"                  : Offset into which row based on
        //                                         vertical scroll offset
        // "+ 0"                                 : Mental clarity for plane offset
        // Note: No PPU address bus offset required as it starts at 0x0000
        m_bgNextTileLsb = PpuRead((m_ppuCtrl.BackGroundPatternTableAddress << 12)
          + ((uint16_t)m_bgNextTileId << 4)
          + (m_currentVRamAddress.fineYScroll) + 0);

        break;
      case 6:
        // Fetch the next background tile MSB bit plane from the pattern memory
        // This is the same as above, but has a +8 offset to select the next bit plane
        m_bgNextTileMsb = PpuRead((m_ppuCtrl.BackGroundPatternTableAddress << 12)
          + ((uint16_t)m_bgNextTileId << 4)
          + (m_currentVRamAddress.fineYScroll) + 8);
        break;
      case 7:
        // Increment the background tile "pointer" to the next tile horizontally
        // in the nametable memory. Note this may cross nametable boundaries which
        // is a little complex, but essential to implement scrolling
        IncrementScrollX();
        break;
      }
    }

    // End of a visible m_scanline, so increment downwards...
    if (m_cycle == 256)
    {
      IncrementScrollY();
    }

    //...and reset the x position
    if (m_cycle == 257)
    {
      LoadBackgroundShifters();
      TransferAddressX();
    }

    // Superfluous reads of tile id at end of m_scanline
    if (m_cycle == 338 || m_cycle == 340)
    {
      m_bgNextTileId = PpuRead(0x2000 | (m_currentVRamAddressReg & 0x0FFF));
    }

    if (m_scanline == -1 && m_cycle >= 280 && m_cycle < 305)
    {
      // End of vertical blank period so reset the Y address ready for rendering
      TransferAddressY();
    }

    // Foreground Rendering ========================================================
// I'm gonna cheat a bit here, which may reduce compatibility, but greatly
// simplifies delivering an intuitive understanding of what exactly is going
// on. The PPU loads sprite information successively during the region that
// background tiles are not being drawn. Instead, I'm going to perform
// all sprite evaluation in one hit. THE NES DOES NOT DO IT LIKE THIS! This makes
// it easier to see the process of sprite evaluation.
    if (m_cycle == 257 && m_scanline >= 0)
    {
      // We've reached the end of a visible scanline. It is now time to determine
      // which sprites are visible on the next scanline, and preload this info
      // into buffers that we can work with while the scanline scans the row.

      // Firstly, clear out the sprite memory. This memory is used to store the
      // sprites to be rendered. It is not the OAM.
      std::memset(m_spriteScanline, 0xFF, 8 * sizeof(ObjectAttributeItem));

      // The NES supports a maximum number of sprites per scanline. Nominally
      // this is 8 or fewer sprites. This is why in some games you see sprites
      // flicker or disappear when the scene gets busy.
      m_spriteCount = 0;

      // Secondly, clear out any residual information in sprite pattern shifters
      for (uint8_t i = 0; i < 8; i++)
      {
        m_spriteShifterPatternLo[i] = 0;
        m_spriteShifterPatternHi[i] = 0;
      }

      // Thirdly, Evaluate which sprites are visible in the next scanline. We need
      // to iterate through the OAM until we have found 8 sprites that have Y-positions
      // and heights that are within vertical range of the next scanline. Once we have
      // found 8 or exhausted the OAM we stop. Now, notice I count to 9 sprites. This
      // is so I can set the sprite overflow flag in the event of there being > 8 sprites.
      uint8_t nOAMEntry = 0;

      // New set of sprites. Sprite zero may not exist in the new set, so clear this
      // flag.
      m_spriteZeroHitPossible = false;

      while (nOAMEntry < 64 && m_spriteCount < 9)
      {
        // Note the conversion to signed numbers here
        int16_t diff = ((int16_t)m_scanline - (int16_t)m_oam[nOAMEntry].yPos);

        // If the difference is positive then the scanline is at least at the
        // same height as the sprite, so check if it resides in the sprite vertically
        // depending on the current "sprite height mode"
        // FLAGGED

        if (diff >= 0 && diff < (m_ppuCtrl.SpriteSize ? 16 : 8) && m_spriteCount < 8)
        {
          // Sprite is visible, so copy the attribute entry over to our
          // scanline sprite cache. Ive added < 8 here to guard the array
          // being written to.
          if (m_spriteCount < 8)
          {
            // Is this sprite sprite zero?
            if (nOAMEntry == 0)
            {
              // It is, so its possible it may trigger a 
              // sprite zero hit when drawn
              m_spriteZeroHitPossible = true;
            }

            memcpy(&m_spriteScanline[m_spriteCount], &m_oam[nOAMEntry], sizeof(ObjectAttributeItem));
          }
          m_spriteCount++;
        }
        nOAMEntry++;
      } // End of sprite evaluation for next scanline

      // Set sprite overflow flag
      m_ppuStatus.spriteOverflow = (m_spriteCount >= 8);

      // Now we have an array of the 8 visible sprites for the next scanline. By 
      // the nature of this search, they are also ranked in priority, because
      // those lower down in the OAM have the higher priority.

      // We also guarantee that "Sprite Zero" will exist in spriteScanline[0] if
      // it is evaluated to be visible. 
    }

    if (m_cycle == 340)
    {
      // Now we're at the very end of the scanline, I'm going to prepare the 
      // sprite shifters with the 8 or less selected sprites.

      for (uint8_t i = 0; i < m_spriteCount; i++)
      {
        // We need to extract the 8-bit row patterns of the sprite with the
        // correct vertical offset. The "Sprite Mode" also affects this as
        // the sprites may be 8 or 16 rows high. Additionally, the sprite
        // can be flipped both vertically and horizontally. So there's a lot
        // going on here :P

        uint8_t sprite_pattern_bits_lo, sprite_pattern_bits_hi;
        uint16_t sprite_pattern_addr_lo, sprite_pattern_addr_hi;

        // Determine the memory addresses that contain the byte of pattern data. We
        // only need the lo pattern address, because the hi pattern address is always
        // offset by 8 from the lo address.
        if (!m_ppuCtrl.SpriteSize)
        {
          // 8x8 Sprite Mode - The control register determines the pattern table
          if (!(m_spriteScanline[i].attribute & 0x80))
          {
            // Sprite is NOT flipped vertically, i.e. normal    
            sprite_pattern_addr_lo =
              (m_ppuCtrl.SpritePatternTableAddress << 12)  // Which Pattern Table? 0KB or 4KB offset
              | (m_spriteScanline[i].tileIndex << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
              | (m_scanline - m_spriteScanline[i].yPos); // Which Row in cell? (0->7)

          }
          else
          {
            // Sprite is flipped vertically, i.e. upside down
            sprite_pattern_addr_lo =
              (m_ppuCtrl.SpritePatternTableAddress << 12)  // Which Pattern Table? 0KB or 4KB offset
              | (m_spriteScanline[i].tileIndex << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
              | (7 - (m_scanline - m_spriteScanline[i].yPos)); // Which Row in cell? (7->0)
          }

        }
        else
        {
          // 8x16 Sprite Mode - The sprite attribute determines the pattern table
          if (!(m_spriteScanline[i].attribute & 0x80))
          {
            // Sprite is NOT flipped vertically, i.e. normal
            if (m_scanline - m_spriteScanline[i].yPos < 8)
            {
              // Reading Top half Tile
              sprite_pattern_addr_lo =
                ((m_spriteScanline[i].tileIndex & 0x01) << 12)  // Which Pattern Table? 0KB or 4KB offset
                | ((m_spriteScanline[i].tileIndex & 0xFE) << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
                | ((m_scanline - m_spriteScanline[i].yPos) & 0x07); // Which Row in cell? (0->7)
            }
            else
            {
              // Reading Bottom Half Tile
              sprite_pattern_addr_lo =
                ((m_spriteScanline[i].tileIndex & 0x01) << 12)  // Which Pattern Table? 0KB or 4KB offset
                | (((m_spriteScanline[i].tileIndex & 0xFE) + 1) << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
                | ((m_scanline - m_spriteScanline[i].yPos) & 0x07); // Which Row in cell? (0->7)
            }
          }
          else
          {
            // Sprite is flipped vertically, i.e. upside down
            if (m_scanline - m_spriteScanline[i].yPos < 8)
            {
              // Reading Top half Tile
              sprite_pattern_addr_lo =
                ((m_spriteScanline[i].tileIndex & 0x01) << 12)    // Which Pattern Table? 0KB or 4KB offset
                | (((m_spriteScanline[i].tileIndex & 0xFE) + 1) << 4)    // Which Cell? Tile ID * 16 (16 bytes per tile)
                | (7 - (m_scanline - m_spriteScanline[i].yPos) & 0x07); // Which Row in cell? (0->7)
            }
            else
            {
              // Reading Bottom Half Tile
              sprite_pattern_addr_lo =
                ((m_spriteScanline[i].tileIndex & 0x01) << 12)    // Which Pattern Table? 0KB or 4KB offset
                | ((m_spriteScanline[i].tileIndex & 0xFE) << 4)    // Which Cell? Tile ID * 16 (16 bytes per tile)
                | (7 - (m_scanline - m_spriteScanline[i].yPos) & 0x07); // Which Row in cell? (0->7)
            }
          }
        }

        // Phew... XD I'm absolutely certain you can use some fantastic bit 
        // manipulation to reduce all of that to a few one liners, but in this
        // form it's easy to see the processes required for the different
        // sizes and vertical orientations

        // Hi bit plane equivalent is always offset by 8 bytes from lo bit plane
        sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;

        // Now we have the address of the sprite patterns, we can read them
        sprite_pattern_bits_lo = PpuRead(sprite_pattern_addr_lo);
        sprite_pattern_bits_hi = PpuRead(sprite_pattern_addr_hi);

        // If the sprite is flipped horizontally, we need to flip the 
        // pattern bytes. 
        if (m_spriteScanline[i].attribute & 0x40)
        {
          // This little lambda function "flips" a byte
          // so 0b11100000 becomes 0b00000111. It's very
          // clever, and stolen completely from here:
          // https://stackoverflow.com/a/2602885
          auto flipbyte = [](uint8_t b)
          {
            b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
            b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
            b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
            return b;
          };

          // Flip Patterns Horizontally
          sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
          sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
        }

        // Finally! We can load the pattern into our sprite shift registers
        // ready for rendering on the next scanline
        m_spriteShifterPatternLo[i] = sprite_pattern_bits_lo;
        m_spriteShifterPatternHi[i] = sprite_pattern_bits_hi;
      }
    }
  }

  if (m_scanline == 240)
  {
    // Post Render Scanline - Do Nothing!
  }

  if (m_scanline >= 241 && m_scanline < 261)
  {
    if (m_scanline == 241 && m_cycle == 1)
    {
      // Effectively end of frame, so set vertical blank flag
      m_ppuStatus.verticalBlankStarted = true;

      // If the control register tells us to emit a NMI when
      // entering vertical blanking period, do it! The CPU
      // will be informed that rendering is complete so it can
      // perform operations with the PPU knowing it wont
      // produce visible artefacts
      if (m_ppuCtrl.NMI_Enabled)
         nmi = true;
    }
  }



  // Composition - We now have background & foreground pixel information for this cycle

  // Background =============================================================
  uint8_t bg_pixel = 0x00;   // The 2-bit pixel to be rendered
  uint8_t bg_palette = 0x00; // The 3-bit index of the palette the pixel indexes

  // We only render backgrounds if the PPU is enabled to do so. Note if 
  // background rendering is disabled, the pixel and palette combine
  // to form 0x00. This will fall through the colour tables to yield
  // the current background colour in effect
  if (m_ppuMask.showBackGround)
  {
    if (m_ppuMask.showBckGndLeft8Px || (m_cycle >= 9))
    {
      // Handle Pixel Selection by selecting the relevant bit
      // depending upon fine x scolling. This has the effect of
      // offsetting ALL background rendering by a set number
      // of pixels, permitting smooth scrolling
      uint16_t bit_mux = 0x8000 >> m_fineScrollX;

      // Select Plane pixels by extracting from the shifter 
      // at the required location. 
      uint8_t p0_pixel = (m_bgShifterPatternLo & bit_mux) > 0;
      uint8_t p1_pixel = (m_bgShifterPatternHi & bit_mux) > 0;

      // Combine to form pixel index
      bg_pixel = (p1_pixel << 1) | p0_pixel;

      // Get palette
      uint8_t bg_pal0 = (m_bgShifterAttribLo & bit_mux) > 0;
      uint8_t bg_pal1 = (m_bgShifterAttribHi & bit_mux) > 0;
      bg_palette = (bg_pal1 << 1) | bg_pal0;
    }
  }

  // Foreground =============================================================
  uint8_t fg_pixel = 0x00;   // The 2-bit pixel to be rendered
  uint8_t fg_palette = 0x00; // The 3-bit index of the palette the pixel indexes
  uint8_t fg_priority = 0x00;// A bit of the sprite attribute indicates if its
                 // more important than the background
  if (m_ppuMask.showSprites)
  {
    // Iterate through all sprites for this scanline. This is to maintain
    // sprite priority. As soon as we find a non transparent pixel of
    // a sprite we can abort
    if (m_ppuMask.showSpritesLeft8Px || (m_cycle >= 9))
    {

      m_spriteZeroBeingRendered = false;

      for (uint8_t i = 0; i < m_spriteCount; i++)
      {
        // Scanline cycle has "collided" with sprite, shifters taking over
        if (m_spriteScanline[i].x == 0)
        {
          // Note Fine X scrolling does not apply to sprites, the game
          // should maintain their relationship with the background. So
          // we'll just use the MSB of the shifter

          // Determine the pixel value...
          uint8_t fg_pixel_lo = (m_spriteShifterPatternLo[i] & 0x80) > 0;
          uint8_t fg_pixel_hi = (m_spriteShifterPatternHi[i] & 0x80) > 0;
          fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;

          // Extract the palette from the bottom two bits. Recall
          // that foreground palettes are the latter 4 in the 
          // palette memory.
          fg_palette = (m_spriteScanline[i].attribute & 0x03) + 0x04;
          fg_priority = (m_spriteScanline[i].attribute & 0x20) == 0;

          // If pixel is not transparent, we render it, and dont
          // bother checking the rest because the earlier sprites
          // in the list are higher priority
          if (fg_pixel != 0)
          {
            if (m_spriteZeroHitPossible && i == 0) // Is this sprite zero?
            {
              m_spriteZeroBeingRendered = true;
            }

            break;
          }
        }
      }
    }
  }

  // Now we have a background pixel and a foreground pixel. They need
  // to be combined. It is possible for sprites to go behind background
  // tiles that are not "transparent", yet another neat trick of the PPU
  // that adds complexity for us poor emulator developers...

  uint8_t pixel = 0x00;   // The FINAL Pixel...
  uint8_t palette = 0x00; // The FINAL Palette...

  if (bg_pixel == 0 && fg_pixel == 0)
  {
    // The background pixel is transparent
    // The foreground pixel is transparent
    // No winner, draw "background" colour
    pixel = 0x00;
    palette = 0x00;
  }
  else if (bg_pixel == 0 && fg_pixel > 0)
  {
    // The background pixel is transparent
    // The foreground pixel is visible
    // Foreground wins!
    pixel = fg_pixel;
    palette = fg_palette;
  }
  else if (bg_pixel > 0 && fg_pixel == 0)
  {
    // The background pixel is visible
    // The foreground pixel is transparent
    // Background wins!
    pixel = bg_pixel;
    palette = bg_palette;
  }
  else if (bg_pixel > 0 && fg_pixel > 0)
  {
    // The background pixel is visible
    // The foreground pixel is visible
    // Hmmm...
    if (fg_priority)
    {
      // Foreground cheats its way to victory!
      pixel = fg_pixel;
      palette = fg_palette;
    }
    else
    {
      // Background is considered more important!
      pixel = bg_pixel;
      palette = bg_palette;
    }

    // Sprite Zero Hit detection
    if (m_spriteZeroHitPossible && m_spriteZeroBeingRendered)
    {
      // Sprite zero is a collision between foreground and background
      // so they must both be enabled
      if (m_ppuMask.showBackGround && m_ppuMask.showSprites)
      {
        // The left edge of the screen has specific switches to control
        // its appearance. This is used to smooth inconsistencies when
        // scrolling (since sprites x coord must be >= 0)
        if (!(m_ppuMask.showBckGndLeft8Px || m_ppuMask.showSpritesLeft8Px))
        {
          if (m_cycle >= 9 && m_cycle < 258)
          {
            m_ppuStatus.sprite0Hit = true;
          }
        }
        else
        {
          if (m_cycle >= 1 && m_cycle < 258)
          {
            m_ppuStatus.sprite0Hit = true;
          }
        }
      }
    }
  }



  // Now we have a final pixel colour, and a palette for this m_cycle
  // of the current m_scanline. Let's at long last, draw that ^&%*er :P
  const Color& color = NesColorTable[PpuRead(0x3F00 + (palette << 2) + pixel) & 0x3F];
  m_window.SetPixel(m_cycle - 1, m_scanline, color);

   // Advance renderer - it never stops, it's relentless
  m_cycle++;
  //if (m_ppuMask.showBackGround || m_ppuMask.showSprites)
  //  if (m_cycle == 260 && m_scanline < 240)
  //    cart->GetMapper()->scanline();

  if (m_cycle >= 341)
  {
    m_cycle = 0;
    m_scanline++;
    if (m_scanline >= 261)
    {
      m_scanline = -1;
      m_oddFrame = !m_oddFrame;
    }
  }
}


uint8_t PPU_2C02::PpuRead(uint16_t address)
{
  uint8_t data = 0x00;
  if (m_ppuBus.Read(address, data))
  {
    return data;
  }
  else if (address >= 0x3F00 && address <= 0x3FFF)
  {
    address &= 0x001F;
    if (address == 0x0010) address = 0x0000;
    if (address == 0x0014) address = 0x0004;
    if (address == 0x0018) address = 0x0008;
    if (address == 0x001C) address = 0x000C;
    return m_tblPalette[address] & (m_ppuMask.greyScale ? 0x30 : 0x3F);
  }
  else
  {
    //assert(!"PPU_2C02::PpuRead: Failed to read");
  }
  return data;
}

void PPU_2C02::PpuWrite(uint16_t address, uint8_t data)
{
  if (m_ppuBus.Write(address, data))
  {
    return;
  }
  else if (address >= 0x3F00 && address <= 0x3FFF)
  {
    address &= 0x001F;
    if (address == 0x0010) address = 0x0000;
    if (address == 0x0014) address = 0x0004;
    if (address == 0x0018) address = 0x0008;
    if (address == 0x001C) address = 0x000C;
    m_tblPalette[address] = data;
    return;
  }
  else
  {
    assert(!"PPU_2C02::PpuWrite: Failed to read");
  }
}

bool PPU_2C02::CPUClock(unsigned int counter)
{
  if (m_dmaTransfer)
  {
    // ...Yes! We need to wait until the next even CPU clock cycle
    // before it starts...
    if (m_dmaDummy)
    {
      // ...So hang around in here each clock until 1 or 2 cycles
      // have elapsed...
      if (counter % 2 == 1)
      {
        // ...and finally allow DMA to start
        m_dmaDummy = false;
      }
    }
    else
    {
      // DMA can take place!
      if (counter % 2 == 0)
      {
        // On even clock cycles, read from CPU bus
        if (!m_cpuBus.Read(m_dmaPage << 8 | m_dmaAddress, m_dmaData))
        {
          assert(!"PPU_2C02::CPUClock: Failed to read from cpubus.");
        }
      }
      else
      {
        // On odd clock cycles, write to PPU OAM
        m_pOAM[m_dmaAddress] = m_dmaData;
        // Increment the lo byte of the address
        m_dmaAddress++;
        // If this wraps around, we know that 256
        // bytes have been written, so end the DMA
        // transfer, and proceed as normal
        if (m_dmaAddress == 0x00)
        {
          m_dmaTransfer = false;
          m_dmaDummy = true;
        }
      }
    }
    return true;
  }
  return false;
}
