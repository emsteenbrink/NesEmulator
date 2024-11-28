

add_library(NesEmulatorCore STATIC 
            Nes.cpp
            Nes.h
            Components/APU.cpp
            Components/APU.h
            Components/Bus.cpp
            Components/Bus.h
            Components/Cartridge.cpp
            Components/Cartridge.h
            Components/ColorTable.cpp
            Components/ColorTable.h
            Components/Component.cpp
            Components/Component.h
            Components/CPU_2A03.cpp
            Components/CPU_2A03.h
            Components/PersistentRAM.cpp
            Components/PersistentRAM.h
            Components/PPU_2C02.cpp
            Components/PPU_2C02.h
            Components/RAM.cpp
            Components/RAM.h
            Components/R6502.cpp
            Components/R6502.h
            Components/R6502ProcessorStatusRegister.cpp
            Components/R6502ProcessorStatusRegister.h
            Configuration/iNesConfiguration.cpp
            Configuration/iNesConfiguration.h
            Mappers/Mapper.cpp
            Mappers/Mapper.h
            Mappers/MapperFactory.cpp
            Mappers/MapperFactory.h
            Mappers/Mapper_000.cpp
            Mappers/Mapper_000.h
            Mappers/Mapper_001.cpp
            Mappers/Mapper_001.h
            Sound/DMCChannel.cpp
            Sound/DMCChannel.h
            Sound/Envelope.cpp
            Sound/Envelope.h
            Sound/LengthCounter.cpp
            Sound/LengthCounter.h
            Sound/LinearCounter.cpp
            Sound/LinearCounter.h
            Sound/NoiseChannel.cpp
            Sound/NoiseChannel.h
            Sound/PulseChannel.cpp
            Sound/PulseChannel.h
            Sound/Sequencer.cpp
            Sound/Sequencer.h
            Sound/Sweep.cpp
            Sound/Sweep.h
            Sound/TriangleChannel.cpp
            Sound/TriangleChannel.h
            Utils/Range.cpp
            Utils/Range.h
)

target_include_directories(NesEmulatorCore PUBLIC 
          Components
          Configuration
          Mappers
          Sound
          Utils
          ./
)

#target_link_libraries(NesEmulatorCore PRIVATE ${SDL2_LIBRARIES})