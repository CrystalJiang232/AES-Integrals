add_executable(gencp
    gen_cipher.cpp)

target_link_libraries(gencp PRIVATE aesatk-lib)

set_target_properties(gencp PROPERTIES
    OUTPUT_NAME "gencp"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

