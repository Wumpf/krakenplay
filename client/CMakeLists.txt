#cmake

# Adding OIS
set(OIS_INCLUDE_PATH "${CMAKE_CURRENT_BINARY_DIR}/../dependencies/ois/includes/" CACHE FILEPATH OIS_INCLUDE_PATH)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(OIS_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/../dependencies/ois/lib64/" CACHE FILEPATH OIS_LIB_PATH)
else()
        set(OIS_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/../dependencies/ois/lib/" CACHE FILEPATH OIS_LIB_PATH)
endif()
set(OIS_LIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/../dependencies/ois/lib/" CACHE FILEPATH OIS_LIB_PATH)
set(OIS_LIB_DEBUG "OIS_static_d" CACHE STRING OIS_LIB_DEBUG)
set(OIS_LIB_RELEASE "OIS_static" CACHE STRING OIS_LIB_RELEASE)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # Use as system header, so that all warnings will automatically be ignored.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${OIS_INCLUDE_PATH}")
else()
    include_directories(${OIS_INCLUDE_PATH})
endif()
link_directories(${OIS_LIB_PATH})

addSourceDir("" client_SRC)
addSourceDir("implementation" client_SRC)

add_executable(client ${client_SRC})
target_link_libraries(client krakenplay)
target_link_libraries(client debug ${OIS_LIB_DEBUG} optimized ${OIS_LIB_RELEASE})
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_libraries(client Ws2_32.lib)
endif()

install(TARGETS client DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/../bin")
