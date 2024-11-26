

add_library(NesEmulatorCore STATIC 
            Nes.cpp
            Components/APU.cpp
            Components/Bus.cpp
            Components/Cartridge.cpp
            Components/ColorTable.cpp
            Components/Component.cpp
            Components/CPU_2A03.cpp
            Components/PersistentRAM.cpp
            Components/PPU_2C02.cpp
            Components/RAM.cpp
            Components/R6502.cpp
            Components/R6502ProcessorStatusRegister.cpp
            Configuration/iNesConfiguration.cpp
            Mappers/Mapper.cpp
            Mappers/MapperFactory.cpp
            Mappers/Mapper_000.cpp
            Mappers/Mapper_001.cpp
            Sound/BlockBuffer.cpp
            Sound/DMCChannel.cpp
            Sound/Envelope.cpp
            Sound/LengthCounter.cpp
            Sound/LinearCounter.cpp
            Sound/NoiseChannel.cpp
            Sound/PulseChannel.cpp
            Sound/Resampler.cpp
            Sound/Sequencer.cpp
            Sound/Sweep.cpp
            Sound/TriangleChannel.cpp
            Utils/Range.cpp
)

target_include_directories(NesEmulatorCore PUBLIC 
          Components
          Configuration
          Mappers
          Sound
          Utils
          ./
          ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(NesEmulatorCore PRIVATE ${SDL2_LIBRARIES})