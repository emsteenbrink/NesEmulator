
add_executable(NesEmulator
       source/KeyboardController.cpp
       source/Main.cpp
       source/MainWindow.cpp
       source/NesEmulator.cpp
)

target_link_directories(NesEmulator PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(NesEmulator PRIVATE NesEmulatorCore ${SDL2_LIBRARIES})

if(${WIN32})
add_custom_command(TARGET NesEmulator POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:NesEmulator> $<TARGET_RUNTIME_DLLS:NesEmulator>
  COMMAND_EXPAND_LISTS
)
endif()
