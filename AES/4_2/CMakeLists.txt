add_executable(aes4-2
    main4_2.cpp
    atk4_2.cpp
)

find_package(Threads REQUIRED)

target_link_libraries(aes4-2
    PRIVATE
        aesatk-lib
        Threads::Threads
        cmdline_args
)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_options(aes4-2 PRIVATE "-pthread")
endif()

set_target_properties(aes4-2 PROPERTIES
    OUTPUT_NAME "aes4-2"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)