set(CORE_SOURCES
    WraithW60Device.cpp
    WraithW60Protocol.cpp
    WraithW60Leds.cpp
    WraithW60Modes.cpp
    ../platform/HidDevice.cpp
)

add_library(wraith_core SHARED ${CORE_SOURCES})

target_link_libraries(wraith_core
    PRIVATE ${HIDAPI_LIBRARIES}
)

target_include_directories(wraith_core
    PUBLIC ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${HIDAPI_INCLUDE_DIRS}
)

if(UNIX AND NOT APPLE)
    target_link_libraries(wraith_core PRIVATE udev)
endif()

set_target_properties(wraith_core PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME wraith
)

install(TARGETS wraith_core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
