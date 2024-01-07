cmake_minimum_required(VERSION 3.28.0)
project(AppleMusicFindCurrentSong)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src/AppleMusicFindCurrentSong)

if(CMAKE_SYSTEM MATCHES Windows)
   message(STATUS "Target system is Windows, building AppleMusicPlayer windows version")
   add_subdirectory(src/AppleMusicPlayer)
endif()