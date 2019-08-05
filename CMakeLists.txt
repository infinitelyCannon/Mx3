cmake_minimum_required(VERSION 3.8.0)
project(Mx3)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(FMOD REQUIRED)

if(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
	enable_testing()
endif()

set(SOURCES src/Mx3.cpp
	src/LoopRegion.cpp
	src/Track.cpp
	src/Parameter.cpp
)

if(WIN32)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(WIN32)

add_library(Mx3 SHARED ${SOURCES})
target_include_directories(Mx3 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(Mx3 PUBLIC cxx_std_11)
set_target_properties(Mx3 PROPERTIES CXX_EXTENSIONS OFF)

if(UNIX)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	target_link_libraries(Mx3 Threads::Threads)
endif(UNIX)

#CTest Modules
add_executable(FunctionTest "${CMAKE_CURRENT_SOURCE_DIR}/tests/FunctionTest.cpp")
target_link_libraries(FunctionTest Mx3)
add_test(NAME FunctionTest COMMAND FunctionTest WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests/data")

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