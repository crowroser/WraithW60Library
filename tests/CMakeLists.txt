add_executable(test_protocol test_protocol.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WraithW60Protocol.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WraithW60Modes.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WraithW60Leds.cpp
)

target_include_directories(test_protocol
    PRIVATE ${CMAKE_SOURCE_DIR}/include
)

target_compile_definitions(test_protocol PRIVATE WRAITH_TESTING)

add_test(NAME test_protocol COMMAND test_protocol)
