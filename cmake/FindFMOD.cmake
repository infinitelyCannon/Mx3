# - Locate FMOD library (By Matt Raykowski, OpenNeL Project http://www.opennel.org/)
# http://www.opennel.org/fisheye/browse/~raw,r=1353/NeL/trunk/nel/CMakeModules/FindFMOD.cmake
# (with permission to relicense as LGPL-with-staticlink-exemption by Matt Raykowski)
# This module defines
#  FMOD_LIBRARY, the library to link against
#  FMOD_FOUND, if false, do not try to link to FMOD
#  FMOD_INCLUDE_DIRS, where to find headers.

IF(FMOD_LIBRARY_x64 AND FMOD_LIBRARY_x64_DEBUG AND FMOD_LIBRARY_x86 AND FMOD_LIBRARY_x86_DEBUG AND FMOD_INCLUDE_DIR)
  # in cache already
  SET(FMOD_FIND_QUIETLY TRUE)
ENDIF(FMOD_LIBRARY_x64 AND FMOD_LIBRARY_x64_DEBUG AND FMOD_LIBRARY_x86 AND FMOD_LIBRARY_x86_DEBUG AND FMOD_INCLUDE_DIR)

if(UNIX)
FIND_PATH(FMOD_INCLUDE_DIR
  fmod.h
  PATHS
  $ENV{FMOD_DIR}/inc
  /usr/local/include
  /usr/local/include/fmodex/
  /usr/include
  /usr/include/fmodex/
  /sw/include
  /sw/include/fmodex/
  /opt/local/include
  /opt/local/include/fmodex/
  /opt/csw/include
  /opt/csw/include/fmodex/
  /opt/include
  /opt/include/fmodex/
  PATH_SUFFIXES fmod
)

FIND_LIBRARY(FMOD_LIBRARY_x64_DEBUG
  NAMES fmodL
  PATHS
  $ENV{FMOD_DIR}/lib
  /usr/local/lib
  /usr/lib
  /usr/local/X11R6/lib
  /usr/X11R6/lib
  /sw/lib
  /opt/local/lib
  /opt/csw/lib
  /opt/lib
  /usr/freeware/lib64
  PATH_SUFFIXES
  x86_64
)

FIND_LIBRARY(FMOD_LIBRARY_x64
  NAMES fmod
  PATHS
  $ENV{FMOD_DIR}/lib
  /usr/local/lib
  /usr/lib
  /usr/local/X11R6/lib
  /usr/X11R6/lib
  /sw/lib
  /opt/local/lib
  /opt/csw/lib
  /opt/lib
  /usr/freeware/lib64
  PATH_SUFFIXES
  x86_64
)

FIND_LIBRARY(FMOD_LIBRARY_x86
  NAMES fmod
  PATHS
  $ENV{FMOD_DIR}/lib
  /usr/local/lib
  /usr/lib
  /usr/local/X11R6/lib
  /usr/X11R6/lib
  /sw/lib
  /opt/local/lib
  /opt/csw/lib
  /opt/lib
  /usr/freeware/lib64
  PATH_SUFFIXES
  x86
)

FIND_LIBRARY(FMOD_LIBRARY_x86_DEBUG
  NAMES fmodL
  PATHS
  $ENV{FMOD_DIR}/lib
  /usr/local/lib
  /usr/lib
  /usr/local/X11R6/lib
  /usr/X11R6/lib
  /sw/lib
  /opt/local/lib
  /opt/csw/lib
  /opt/lib
  /usr/freeware/lib64
  PATH_SUFFIXES
  x86
)
endif(UNIX)

if(WIN32)
find_path(FMOD_INCLUDE_DIR
    fmod.h
    PATHS
    "$ENV{FMOD_DIR}\\inc"
    "C:\\Program Files (x86)\\FMOD SoundSystem\\FMOD Studio API Windows\\api\\lowlevel\\inc"
)

find_library(FMOD_LIBRARY_x64_DEBUG
    NAMES fmodL64_vc
    PATHS
    "$ENV{FMOD_DIR}\\lib"
    "C:\\Program Files (x86)\\FMOD SoundSystem\\FMOD Studio API Windows\\api\\lowlevel\\lib"
)

find_library(FMOD_LIBRARY_x64
    NAMES fmod64_vc
    PATHS
    "$ENV{FMOD_DIR}\\lib"
    "C:\\Program Files (x86)\\FMOD SoundSystem\\FMOD Studio API Windows\\api\\lowlevel\\lib"
)

find_library(FMOD_LIBRARY_x86_DEBUG
    NAMES fmodL_vc
    PATHS
    "$ENV{FMOD_DIR}\\lib"
    "C:\\Program Files (x86)\\FMOD SoundSystem\\FMOD Studio API Windows\\api\\lowlevel\\lib"
)

find_library(FMOD_LIBRARY_x86
    NAMES fmod_vc
    PATHS
    "$ENV{FMOD_DIR}\\lib"
    "C:\\Program Files (x86)\\FMOD SoundSystem\\FMOD Studio API Windows\\api\\lowlevel\\lib"
)
endif(WIN32)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FMOD DEFAULT_MSG
	FMOD_INCLUDE_DIR
	FMOD_LIBRARY_x64
	FMOD_LIBRARY_x64_DEBUG
	FMOD_LIBRARY_x86
	FMOD_LIBRARY_x86_DEBUG
)

if(FMOD_FOUND)
	set(FMOD_INCLUDE_DIRS ${FMOD_INCLUDE_DIR})
	if(NOT TARGET FMOD::FMOD_x86)
		add_library(FMOD::FMOD_x86 SHARED IMPORTED)
		set_target_properties(FMOD::FMOD_x86 PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${FMOD_INCLUDE_DIRS}"
			#IMPORTED_LOCATION_DEBUG "${FMOD_LIBRARY_x86_DEBUG}"
			IMPORTED_LOCATION "${FMOD_LIBRARY_x86}"
			IMPORTED_IMPLIB "${FMOD_LIBRARY_x86}"
			IMPORTED_IMPLIB_DEBUG "${FMOD_LIBRARY_x86_DEBUG}"
			IMPORTED_IMPLIB_RELEASE "${FMOD_LIBRARY_x86}"
		)
	endif()
	if(NOT TARGET FMOD::FMOD_x64)
		add_library(FMOD::FMOD_x64 SHARED IMPORTED)
		set_target_properties(FMOD::FMOD_x64 PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${FMOD_INCLUDE_DIRS}"
			#IMPORTED_LOCATION_DEBUG "${FMOD_LIBRARY_x64_DEBUG}"
			IMPORTED_LOCATION "${FMOD_LIBRARY_x64}"
			IMPORTED_IMPLIB "${FMOD_LIBRARY_x64}"
			IMPORTED_IMPLIB_DEBUG "${FMOD_LIBRARY_x64_DEBUG}"
			IMPORTED_IMPLIB_RELEASE "${FMOD_LIBRARY_x64}"
		)
	endif()
else()
	message(STATUS "Warning: Unable to find FMOD.")
endif(FMOD_FOUND)
