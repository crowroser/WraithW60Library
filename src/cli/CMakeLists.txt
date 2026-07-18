add_executable(wraith_cli wraith_cli.cpp)

target_link_libraries(wraith_cli
    PRIVATE wraith_core
    PRIVATE ${HIDAPI_LIBRARIES}
)

target_include_directories(wraith_cli
    PRIVATE ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${HIDAPI_INCLUDE_DIRS}
)

install(TARGETS wraith_cli RUNTIME DESTINATION bin)

add_executable(wraith_heatmap wraith_heatmap.cpp)

target_link_libraries(wraith_heatmap
    PRIVATE user32
)

install(TARGETS wraith_heatmap RUNTIME DESTINATION bin)
