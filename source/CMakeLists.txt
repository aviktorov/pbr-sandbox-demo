cmake_minimum_required(VERSION 3.10)
set(TARGET ScapesDemo)

project(${TARGET})

# ==================================================================================================
# Variables
# ==================================================================================================
if (WIN32)
	set(SCAPES_PLATFORM windows)
	set(SCAPES_ABI x64)
endif()

# ==================================================================================================
# Dependencies
# ==================================================================================================
add_library(scapes SHARED IMPORTED)

if (WIN32)
	set_target_properties(scapes PROPERTIES IMPORTED_LOCATION_RELEASE ${DIR_THIRDPARTY}/scapes/lib/${SCAPES_PLATFORM}/${SCAPES_ABI}/scapes.dll)
	set_target_properties(scapes PROPERTIES IMPORTED_LOCATION_DEBUG ${DIR_THIRDPARTY}/scapes/lib/${SCAPES_PLATFORM}/${SCAPES_ABI}/scapesd.dll)
	set_target_properties(scapes PROPERTIES IMPORTED_IMPLIB_RELEASE ${DIR_THIRDPARTY}/scapes/lib/${SCAPES_PLATFORM}/${SCAPES_ABI}/scapes.lib)
	set_target_properties(scapes PROPERTIES IMPORTED_IMPLIB_DEBUG ${DIR_THIRDPARTY}/scapes/lib/${SCAPES_PLATFORM}/${SCAPES_ABI}/scapesd.lib)
endif()

add_library(glfw3 STATIC IMPORTED)

if (WIN32)
	set_target_properties(glfw3 PROPERTIES IMPORTED_LOCATION ${DIR_THIRDPARTY}/glfw/lib/${SCAPES_PLATFORM}/${SCAPES_ABI}/glfw3.lib)
endif()

# ==================================================================================================
# Sources
# ==================================================================================================
file(GLOB SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

file(GLOB HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# ==================================================================================================
# Target
# ==================================================================================================
add_executable(${TARGET} ${SOURCES} ${HEADERS})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${DIR_EXPORT}/${SCAPES_PLATFORM}/${SCAPES_ABI})
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${DIR_EXPORT}/${SCAPES_PLATFORM}/${SCAPES_ABI})

# ==================================================================================================
# Preprocessor
# ==================================================================================================
if (WIN32)
	target_compile_definitions(${TARGET} PRIVATE GLFW_EXPOSE_NATIVE_WIN32)
endif()

# ==================================================================================================
# Compile flags
# ==================================================================================================

# ==================================================================================================
# Includes
# ==================================================================================================
target_include_directories(${TARGET} PRIVATE ${DIR_THIRDPARTY})
target_include_directories(${TARGET} PRIVATE ${DIR_THIRDPARTY}/glfw/include)
target_include_directories(${TARGET} PRIVATE ${DIR_THIRDPARTY}/scapes/include)

# ==================================================================================================
# Libraries
# ==================================================================================================
target_link_libraries(${TARGET} PUBLIC glfw3 scapes)


# ==================================================================================================
# Custom commands
# ==================================================================================================
add_custom_command(
	TARGET ${TARGET} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		${DIR_THIRDPARTY}/scapes/lib/${SCAPES_PLATFORM}/${SCAPES_ABI}/scapes.dll
		${DIR_EXPORT}/${SCAPES_PLATFORM}/${SCAPES_ABI}/scapes.dll
)

add_custom_command(
	TARGET ${TARGET} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		${DIR_THIRDPARTY}/scapes/lib/${SCAPES_PLATFORM}/${SCAPES_ABI}/scapesd.dll
		${DIR_EXPORT}/${SCAPES_PLATFORM}/${SCAPES_ABI}/scapesd.dll
)

add_custom_command(
	TARGET ${TARGET} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
		${DIR_THIRDPARTY}/scapes/lib/${SCAPES_PLATFORM}/${SCAPES_ABI}/scapesd.pdb
		${DIR_EXPORT}/${SCAPES_PLATFORM}/${SCAPES_ABI}/scapesd.pdb
)
