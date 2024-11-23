
add_executable(NesEmulator
       source/KeyBoardController.cpp
       source/Main.cpp
       source/MainWindow.cpp
       source/NesEmulator.cpp
)

target_link_directories(NesEmulator PRIVATE ${PROJECT_SOURCE_DIR}/ThirdParty/SDL2/lib/x64/)
target_link_libraries(NesEmulator PRIVATE NesEmulatorCore SDL2::SDL2 SDL2::SDL2main)

if(${WIN32})
add_custom_command(TARGET NesEmulator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:NesEmulator> $<TARGET_RUNTIME_DLLS:NesEmulator>
  COMMAND_EXPAND_LISTS
)
endif()