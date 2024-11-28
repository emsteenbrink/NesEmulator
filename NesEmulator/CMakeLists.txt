
add_executable(NesEmulator WIN32
       source/KeyboardController.cpp
       source/KeyboardController.h
       source/Main.cpp
       source/MainWindow.cpp
       source/MainWindow.h
       source/NesEmulator.cpp
       source/NesEmulator.h
       source/SoundController.cpp
       source/SoundController.h
       source/SoundUtils/BlockBuffer.cpp
       source/SoundUtils/BlockBuffer.h
       source/SoundUtils/Resampler.cpp
       source/SoundUtils/Resampler.h)

if(${WIN32})
  list(APPEND CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/ThirdParty/SDL2/cmake/)
endif()
include_directories(${SDL_INCLUDE_DIR})
find_package(SDL2 REQUIRED)

target_link_directories(NesEmulator PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(NesEmulator PRIVATE NesEmulatorCore ${SDL2_LIBRARIES})

if(${WIN32})
add_custom_command(TARGET NesEmulator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:NesEmulator> $<TARGET_RUNTIME_DLLS:NesEmulator>
  COMMAND_EXPAND_LISTS
)
endif()
