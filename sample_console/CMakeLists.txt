#cmake

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

addSourceDir("" sample_console_SRC)

add_executable(sample_console ${sample_console_SRC})
target_link_libraries(sample_console krakenplay)

install(TARGETS sample_console DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/../bin")