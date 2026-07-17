set(OPENRGB_SOURCES
    WraithW60Controller.cpp
    RGBController_WraithW60.cpp
    WraithW60ControllerDetect.cpp
)

add_library(wraith_openrgb SHARED ${OPENRGB_SOURCES})

target_include_directories(wraith_openrgb PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${HIDAPI_INCLUDE_DIRS}
    ${OPENRGB_INCLUDE_DIR}
)

target_link_libraries(wraith_openrgb
    PRIVATE wraith_core
    PRIVATE ${HIDAPI_LIBRARIES}
)

set_target_properties(wraith_openrgb PROPERTIES
    PREFIX ""
    OUTPUT_NAME RGBController_WraithW60
)
