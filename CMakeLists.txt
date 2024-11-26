# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("NesEmulator")

if(${WIN32})
  list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/SDL2/cmake/)
endif()
#set(SDL2_DIR ${CMAKE_CURRENT_LIST_DIR}/ThirdParty/SDL2/cmake/)
include_directories(${SDL_INCLUDE_DIR})
find_package(SDL2 REQUIRED)

# Include sub-projects.
add_subdirectory ("NesEmulatorCore")
add_subdirectory ("NesEmulator")

