add_executable(aes4-1
    atk4_1.cpp
    main4_1.cpp
    phase4_1.cpp
)

target_link_libraries(aes4-1
    PRIVATE 
        aesatk-lib
        cmdline_args
)

set_target_properties(aes4-1 PROPERTIES
    OUTPUT_NAME "aes4-1"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)