#cmake

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

addSourceDir("" krakenplay_SRC)
addSourceDir("implementation" krakenplay_SRC)
addSourceDir("time" krakenplay_SRC)
addSourceDir("time/implementation" krakenplay_SRC)

add_library(krakenplay STATIC ${krakenplay_SRC})
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_libraries(krakenplay Ws2_32.lib)
endif()

install(TARGETS krakenplay DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/../lib")