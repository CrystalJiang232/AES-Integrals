cmake_minimum_required(VERSION 3.20)
project(AESIntegralAttack LANGUAGES CXX)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/Ox /fp:fast)
    foreach(var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
        string(REGEX REPLACE "/RTC[0-9]" "" ${var} "${${var}}")

    endforeach()
else()
    include_directories(/usr/include/c++/14)  # Add this to CMakeLists.txt
    add_compile_options(-O3 -march=native -ffast-math)
endif()



if(MSVC)
    add_compile_options(/W3 /WX- /sdl /Ox /Oi /Ot /fp:fast /DNDEBUG)
else()
    add_compile_options(-Wall -Wstack-usage=1024)
endif()

file(GLOB SOURCES
"atk_impl.cpp"
"test.cpp"
"aes_io.cpp")

add_executable(aes_integral_attack ${SOURCES})

target_link_libraries(aes_integral_attack PRIVATE Threads::Threads)

# Additional pthread flags for Linux/GCC
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_options(aes_integral_attack PRIVATE "-pthread")
endif()



target_include_directories(aes_integral_attack PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(aes_integral_attack PROPERTIES OUTPUT_NAME "aes-attack")