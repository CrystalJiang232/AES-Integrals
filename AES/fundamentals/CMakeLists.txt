add_library(aesatk-lib
    aes_io.cpp
    timer.cpp
)

target_include_directories(aesatk-lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

