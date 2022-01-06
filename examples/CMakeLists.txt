function(fmod_post_build tar library cpu)
	if(WIN32)
		string(REPLACE "api/core/inc" "api" FMOD_BASE "${FMOD_CORE_DIR}")
		add_custom_command(TARGET ${tar} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<IF:$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">,"${FMOD_BASE}/${library}/lib/${cpu}/fmodL.dll","${FMOD_BASE}/${library}/lib/${cpu}/fmod.dll">
		$<TARGET_FILE_DIR:${tar}>)
	endif()
endfunction()

if(WIN32)
	set(PLATFORM_FILE "win32/common_platform.cpp")
endif()
if(UNIX)
	set(PLATFORM_FILE "linux/common_platform.cpp")
endif()

add_executable(ClassTest 
    class_test.cpp 
    common.cpp 
    "${PLATFORM_FILE}")

add_executable(FmodTest 
	fmod_test.cpp 
	common.cpp 
	"${PLATFORM_FILE}")

if(UNIX)
	find_package(cppzmq)

	add_executable(DebugTerminal "linux/debug/debug_terminal.cpp")
endif()

target_link_libraries(ClassTest PRIVATE Mx3)
target_link_libraries(FmodTest PRIVATE Mx3)

if(UNIX)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	target_link_libraries(ClassTest PRIVATE Threads::Threads cppzmq)
	target_link_libraries(FmodTest PRIVATE Threads::Threads cppzmq)
	target_link_libraries(DebugTerminal Threads::Threads cppzmq)
endif(UNIX)

if(DEFINED CPU)
	if(${CPU} STREQUAL "x86")
#		message(STATUS "Linking with FMOD 32-bit libraries.")
		target_link_libraries(FmodTest PRIVATE FMOD::Core_x86)
		target_link_libraries(ClassTest PRIVATE FMOD::Core_x86)
		FMOD_POST_BUILD("FmodTest" "core" "x86")
		FMOD_POST_BUILD("ClassTest" "core" "x86")
	elseif(${CPU} STREQUAL "x64")
#		message(STATUS "Linking with FMOD 64-bit libraries.")
		target_link_libraries(FmodTest PRIVATE FMOD::Core_x64)
		target_link_libraries(ClassTest PRIVATE FMOD::Core_x64)
		FMOD_POST_BUILD("FmodTest" "core" "x64")
		FMOD_POST_BUILD("ClassTest" "core" "x64")
	else()
		message(FATAL_ERROR "Invalid CPU variable defined for FMOD library.")
	endif()
else()
#	message(STATUS "No CPU variable defined, defaulting to FMOD 64-bit libraries.")
	target_link_libraries(FmodTest PRIVATE FMOD::Core_x64)
	target_link_libraries(ClassTest PRIVATE FMOD::Core_x64)
	FMOD_POST_BUILD("FmodTest" "core" "x64")
	FMOD_POST_BUILD("ClassTest" "core" "x64")
endif()