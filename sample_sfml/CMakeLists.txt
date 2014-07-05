#cmake

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

addSourceDir("" sample_sfml_SRC)

add_executable(sample_sfml ${sample_sfml_SRC})
target_link_libraries(sample_sfml krakenplay)

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics)
if(SFML_FOUND)
	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(sample_sfml ${SFML_LIBRARIES})
endif()

install(TARGETS sample_sfml DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/../bin")