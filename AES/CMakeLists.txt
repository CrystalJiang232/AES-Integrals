cmake_minimum_required(VERSION 3.20)
project(AESIntegralAttack LANGUAGES CXX)

# Global compiler settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform-specific compiler options
if(MSVC)
    add_compile_options(/W3 /WX- /sdl /Ox /Oi /Ot /fp:fast /DNDEBUG)
    foreach(var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
        string(REGEX REPLACE "/RTC[0-9]" "" ${var} "${${var}}")
    endforeach()
    
else()
    add_compile_options(-O3 -march=native -ffast-math -Wall -Wstack-usage=16384 -Wno-narrowing)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(fundamentals)
add_subdirectory(cmdline_args)
add_subdirectory(4_1)
add_subdirectory(4_2)
add_subdirectory(cipher_gen)