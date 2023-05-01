function(fmod_post_build tar, lib, cpu)
	if(WIN32)
		string(REPLACE "api/core/inc" "api" FMOD_BASE "${FMOD_CORE_DIR}")
		add_custom_command(TARGET ${tar} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		$<IF:$<STREQUAL:"${CMAKE_BUILD_TYPE}","Debug">,"${FMOD_BASE}/${library}/lib/${cpu}/fmodL.dll","${FMOD_BASE}/${library}/lib/${cpu}/fmod.dll">
		$<TARGET_FILE_DIR:${tar}>)
	endif()
endfunction()

add_library(Demo
	Engine.cpp
	Channel.cpp
)