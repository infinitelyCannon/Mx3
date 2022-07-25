cmake_minimum_required(VERSION 3.12.0)
project(Mx3 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	set(CMAKE_CXX_EXTENTIONS OFF)

	set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

find_package(FMOD REQUIRED)

add_subdirectory(src)

if(BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()