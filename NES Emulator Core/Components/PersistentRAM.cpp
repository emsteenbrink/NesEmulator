#include "PersistentRAM.h"
#include <fstream>
#include <iostream>

using namespace std;

PersistentRAM::PersistentRAM(unsigned int size, uint16_t beginAddress, uint16_t endAddress, const fs::path& filePath)
  : RAM(size, beginAddress, endAddress)
  , m_filePath(filePath)
{
}

void PersistentRAM::Save()
{
  cout << "Write save game" << endl;
  ofstream fout(m_filePath, ios::out | ios::binary);
  fout.write((char*)&m_ram[0], m_ram.size());
  fout.close();
  for (int i = 0; i < m_ram.size();++i)
  {
    if (i % 10 == 0)
      cout << endl;
    cout << " " << hex << (int)m_ram[i] << dec;
  }
  cout << "Write save game -> Done" << endl;
}

void PersistentRAM::Load()
{
  cout << "Load save game" << endl;
  if (!fs::exists(m_filePath))
    return; // No savegame yet.

  
  auto length = fs::file_size(m_filePath);
  if (length != m_ram.size())
  {
    cout << "Save game is corrupt Expected = " << m_ram.size() << " bytes, Read: " << length << " bytes." << endl;
    return;
  }

  fstream inputFile(m_filePath, ios::in | ios::binary);
  inputFile.read((char*)m_ram.data(), m_ram.size());
  cout << "Read save game -> Done" << endl;
}
