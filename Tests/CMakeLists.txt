
enable_testing()

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)

FetchContent_MakeAvailable(Catch2)

add_executable(Tests "Test_Components.cpp"
                     "CpuLogger.cpp"
                     "CpuLogger.h")

target_link_libraries(Tests PRIVATE NesEmulatorCore Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(Tests)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/TestData/nestest.log
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/TestData/)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/TestData/nestest.nes
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/TestData/)
