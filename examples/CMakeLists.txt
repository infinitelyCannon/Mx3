# These functions grab the appropriate DLLs (Debug or Release) and copies it to the build folder
function(fmod_post_build tar library cpu)
	if(WIN32)
		string(REPLACE "api/core/inc" "api" FMOD_BASE "${FMOD_CORE_DIR}")
		add_custom_command(TARGET ${tar} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<IF:$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">,"${FMOD_BASE}/${library}/lib/${cpu}/fmodL.dll","${FMOD_BASE}/${library}/lib/${cpu}/fmod.dll">
		$<TARGET_FILE_DIR:${tar}>)
	endif()
endfunction()

function(fmod_studio_post_build tar library cpu)
	if(WIN32)
		string(REPLACE "api/core/inc" "api" FMOD_BASE "${FMOD_CORE_DIR}")
		add_custom_command(TARGET ${tar} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<IF:$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">,"${FMOD_BASE}/${library}/lib/${cpu}/fmodstudioL.dll","${FMOD_BASE}/${library}/lib/${cpu}/fmodstudio.dll">
		$<TARGET_FILE_DIR:${tar}>)
	endif()
endfunction()

# Copies dependent DLLs to the target's build folder
function(copy_library_to_target tar lib)
	if(WIN32)
		add_custom_command(TARGET ${tar} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<TARGET_FILE:${lib}>
		$<TARGET_FILE_DIR:${tar}>)
	endif()
endfunction()

if(WIN32)
	set(PLATFORM_FILE "win32/common_platform.cpp")
endif()
if(UNIX)
	set(PLATFORM_FILE "linux/common_platform.cpp")
endif()

add_executable(FmodTest WIN32 
	fmod_test.cpp 
	common.cpp 
	"${PLATFORM_FILE}")

add_executable(LoopTest WIN32 loop_test.cpp common.cpp "${PLATFORM_FILE}")

if(UNIX AND USE_ZMQ) #If you want to use a seperate terminal for output (and have FMOD print to std out)
	find_package(cppzmq)

	add_compile_definitions(USING_ZMQ)
	add_executable(DebugTerminal "linux/debug/debug_terminal.cpp")
	target_link_libraries(FmodTest PRIVATE cppzmq)
	target_link_libraries(LoopTest PRIVATE cppzmq)
	target_link_libraries(DebugTerminal cppzmq)
endif()

#target_link_libraries(FmodTest PRIVATE Mx3)
#COPY_LIBRARY_TO_TARGET("ClassTest" "Mx3")

if(UNIX)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	target_link_libraries(FmodTest PRIVATE Threads::Threads)
	target_link_libraries(LoopTest PRIVATE Threads::Threads)

	if(USE_ZMQ)
		target_link_libraries(DebugTerminal Threads::Threads)
	endif()
endif(UNIX)

if(DEFINED CPU)
	if(${CPU} STREQUAL "x86")
#		message(STATUS "Linking with FMOD 32-bit libraries.")
		target_link_libraries(FmodTest PRIVATE FMOD::Core_x86)
		target_link_libraries(FmodTest PRIVATE FMOD::Studio_x86)
		target_link_libraries(LoopTest PRIVATE FMOD::Core_x86)
		FMOD_POST_BUILD("FmodTest" "core" "x86")
		FMOD_STUDIO_POST_BUILD("FmodTest" "studio" "x86")
		FMOD_POST_BUILD("LoopTest" "core" "x86")
	elseif(${CPU} STREQUAL "x64")
#		message(STATUS "Linking with FMOD 64-bit libraries.")
		target_link_libraries(FmodTest PRIVATE FMOD::Core_x64)
		target_link_libraries(FmodTest PRIVATE FMOD::Studio_x64)
		target_link_libraries(LoopTest PRIVATE FMOD::Core_x64)
		FMOD_POST_BUILD("FmodTest" "core" "x64")
		FMOD_STUDIO_POST_BUILD("FmodTest" "studio" "x64")
		FMOD_POST_BUILD("LoopTest" "core" "x64")
	else()
		message(FATAL_ERROR "Invalid CPU variable defined for FMOD library.")
	endif()
else()
#	message(STATUS "No CPU variable defined, defaulting to FMOD 64-bit libraries.")
	target_link_libraries(FmodTest PRIVATE FMOD::Core_x64)
	target_link_libraries(FmodTest PRIVATE FMOD::Studio_x64)
	target_link_libraries(LoopTest PRIVATE FMOD::Core_x64)
	FMOD_POST_BUILD("FmodTest" "core" "x64")
	FMOD_STUDIO_POST_BUILD("FmodTest" "studio" "x64")
	FMOD_POST_BUILD("LoopTest" "core" "x64")
endif()