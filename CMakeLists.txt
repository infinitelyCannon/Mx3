cmake_minimum_required(VERSION 3.8.0)
project(Mx3)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(FMOD REQUIRED)

set(SOURCES src/Mx3.cpp
	src/LoopRegion.cpp
)

if(WIN32)
	set(TEST1_SOURCES "tests/common.cpp" "tests/win32/common_platform.cpp" "tests/NestedLoop.cpp")
	set(TEST2_SOURCES "tests/common.cpp" "tests/win32/common_platform.cpp" "tests/TrackSamples.cpp")
endif(WIN32)

add_library(Mx3 SHARED ${SOURCES})
target_include_directories(Mx3 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(Mx3 PUBLIC cxx_std_11)
set_target_properties(Mx3 PROPERTIES CXX_EXTENSIONS OFF)

add_executable(NestedLoopTest ${TEST1_SOURCES})
target_include_directories(NestedLoopTest PRIVATE ${FMOD_INCLUDE_DIRS})

add_executable(TrackSamples ${TEST2_SOURCES})
target_include_directories(TrackSamples PRIVATE ${FMOD_INCLUDE_DIRS})

if(WIN32)
	target_include_directories(NestedLoopTest PRIVATE "${PROJECT_SOURCE_DIR}/tests/win32")
	target_include_directories(TrackSamples PRIVATE "${PROJECT_SOURCE_DIR}/tests/win32")
endif(WIN32)
target_link_libraries(NestedLoopTest Mx3)
target_link_libraries(TrackSamples Mx3)

if(DEFINED CPU)
	if(${CPU} STREQUAL "x86")
		message(STATUS "Linking with FMOD 32-bit libraries.")
		target_link_libraries(Mx3 FMOD::FMOD_x86)
	elseif(${CPU} STREQUAL "x64")
		message(STATUS "Linking with FMOD 64-bit libraries.")
		target_link_libraries(Mx3 FMOD::FMOD_x64)
	else()
		message(FATAL_ERROR "Invalid CPU variable defined for FMOD library.")
	endif()
else()
	message(STATUS "No CPU variable defined, defaulting to FMOD 64-bit libraries.")
	target_link_libraries(Mx3 FMOD::FMOD_x64)
endif()