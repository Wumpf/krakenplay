#cmake

cmake_minimum_required(VERSION 2.8)

project("Krakenplay")

# Remove defines for min and max from windows.h
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_definitions(-DNOMINMAX)
endif()

# Set various compiler settings levels
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
	"${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(compilerConfig "-std=c++11 -Wall -Wextra -Werror -Wno-unknown-pragmas")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(compilerConfig "/W4 /WX /EHsc")
endif()
set(CMAKE_CXX_FLAGS "${compilerConfig} ${CMAKE_CXX_FLAGS}")
set(CMAKE_C_FLAGS   "${compilerConfig} ${CMAKE_CXX_FLAGS}")

# -----------------------------------------------------------------------------------------------
# Function to add source directories, automatically search them for source files and create source groups accordingly.
# -----------------------------------------------------------------------------------------------
function(addSourceDir relativeDirectoryName sourcesDstVar)
	set(sourceGroupName ${relativeDirectoryName})

	if("${relativeDirectoryName}" STREQUAL "")
		file(GLOB newSources "*.h" "*.cpp" "*.inl")
	else()
		file(GLOB newSources "${relativeDirectoryName}/*.h" "${relativeDirectoryName}/*.cpp" "${relativeDirectoryName}/*.inl")

		# Nested groups need \ in visual studio
		if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
			string(REPLACE "/" "\\" sourceGroupName "${sourceGroupName}")
		endif()
	endif()

	message(STATUS "Added source directory ${relativeDirectoryName}")
	source_group("${sourceGroupName}" FILES ${newSources})
	set(${sourcesDstVar} ${${sourcesDstVar}} ${newSources} PARENT_SCOPE)
endfunction()


# -----------------------------------------------------------------------------------------------
# Add subdirectories with actual projects
# -----------------------------------------------------------------------------------------------
message(STATUS "Adding SubDirectory 'krakenplay'")
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/krakenplay)

message(STATUS "Adding SubDirectory 'client'")
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/client)

message(STATUS "Adding SubDirectory 'sample_console'")
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/sample_console)

message(STATUS "Adding SubDirectory 'sample_sfml'")
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/sample_sfml)
