#cmake

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 COMPONENTS system window graphics)
if(SFML_FOUND)

  include_directories(${CMAKE_CURRENT_SOURCE_DIR})

  addSourceDir("" sample_sfml_SRC)

  add_executable(sample_sfml ${sample_sfml_SRC})
  target_link_libraries(sample_sfml krakenplay)

	include_directories(${SFML_INCLUDE_DIR})
	target_link_libraries(sample_sfml ${SFML_LIBRARIES})

  install(TARGETS sample_sfml DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/../bin")

else()
  set(BUILD_SFML_SAMPLE OFF CACHE BOOL "True to build the graphical SFML2 based sample." FORCE)
endif()

