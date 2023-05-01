if(WIN32)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(WIN32)

function(fmod_post_build tar library cpu)
	if(WIN32)
		string(REPLACE "api/core/inc" "api" FMOD_BASE "${FMOD_CORE_DIR}")
		add_custom_command(TARGET ${tar} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<IF:$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">,"${FMOD_BASE}/${library}/lib/${cpu}/fmodL.dll","${FMOD_BASE}/${library}/lib/${cpu}/fmod.dll">
		$<TARGET_FILE_DIR:${tar}>)
	endif()
endfunction()

# Adding header files to the mix to have them appear in IDE targets (purely organizational).
file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/inc/*.hpp")

add_library(Mx3 SHARED Mx3.cpp Song.cpp ${HEADER_LIST})
target_include_directories(Mx3 PUBLIC "../inc")
target_compile_features(Mx3 PUBLIC cxx_std_11)

#source_group(TREE "${PROJECT_SOURCE_DIR}/inc" PREFIX "Header Files" FILES ${HEADER_LIST})
source_group("Header Files" ${HEADER_LIST})

if(UNIX)
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	target_link_libraries(Mx3 PRIVATE Threads::Threads)
endif(UNIX)

if(DEFINED CPU)
	if(${CPU} STREQUAL "x86")
		message(STATUS "Linking Mx3 with FMOD 32-bit libraries.")
		target_link_libraries(Mx3 PRIVATE FMOD::Core_x86)
		FMOD_POST_BUILD("Mx3" "core" "x86")
	elseif(${CPU} STREQUAL "x64")
		message(STATUS "Linking Mx3 with FMOD 64-bit libraries.")
		target_link_libraries(Mx3 PRIVATE FMOD::Core_x64)
		FMOD_POST_BUILD("Mx3" "core" "x64")
	else()
		message(FATAL_ERROR "Invalid CPU variable defined for FMOD library.")
	endif()
else()
	message(STATUS "No CPU variable defined, defaulting to FMOD 64-bit libraries.")
	target_link_libraries(Mx3 PRIVATE FMOD::Core_x64)
	FMOD_POST_BUILD("Mx3" "core" "x64")
endif()