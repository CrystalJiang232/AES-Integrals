add_library(cmdline_args STATIC
    phase_impl.cpp
)

target_include_directories(cmdline_args PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})