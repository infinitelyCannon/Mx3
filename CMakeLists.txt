cmake_minimum_required(VERSION 3.8.0)
project(Mx3)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/cmake")

find_package(FMOD REQUIRED)

set(${SOURCES} Mx3.cpp)

add_library(Mx3 SOURCES)
target_include_directories(Mx3 PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_compile_features(Mx3 PUBLIC cxx_std_11)
set_target_properties(Mx3 PROPERTIES CXX_EXTENSIONS_OFF)

if(DEFINED CPU)
	if(${CPU} STREQUAL "x86")
		message(STATUS "Linking with FMOD 32-bit libraries.")
		target_link_libraries(FmodTest FMOD::FMOD_x86)
	elseif(${CPU} STREQUAL "x64")
		message(STATUS "Linking with FMOD 64-bit libraries.")
		target_link_libraries(FmodTest FMOD::FMOD_x64)
	else()
		message(FATAL_ERROR "Invalid CPU variable defined for FMOD library.")
	endif()
else()
	message(STATUS "No CPU variable defined, defaulting to FMOD 64-bit libraries.")
	target_link_libraries(FmodTest FMOD::FMOD_x64)
endif()