# - Find FFTW
# Find the native FFTW includes and library
#
#  FFTW_INCLUDES    - where to find fftw3.h
#  FFTW_LIBRARIES   - List of libraries when using FFTW.
#  FFTW_FOUND       - True if FFTW found.


MESSAGE("STARTING FFTW MODULE")
if(!WIN32 OR APPLE)
	MESSAGE("SEARCHING FFTW FOR UNIX")
	if (FFTW_INCLUDES)
	  # Already in cache, be silent
	  set (FFTW_FIND_QUIETLY TRUE)
	endif (FFTW_INCLUDES)

IF(APPLE)
	MESSAGE("SEARCHING APPLE STYLE FFTW")
	find_path (FFTW_INCLUDES fftw3.h PATHS ${BRAINVIS_ROOT_DIR}/ext/FFTWOSX)
	find_library (FFTW_LIBRARIES NAMES libfftw3 PATHS ${BRAINVIS_ROOT_DIR}/ext/FFTWOSX)

	#find_path (FFTW_INCLUDES fftw3.h)
	#find_library (FFTW_LIBRARIES NAMES libfftw3)

ELSE(APPLE)
	find_path (FFTW_INCLUDES fftw3.h)
	find_library (FFTW_LIBRARIES NAMES fftw3)
ENDIF(APPLE)

	if(FFTW_INCLUDES)
		MESSAGE("found header")
	endif(FFTW_INCLUDES)

	if(FFTW_LIBRARIES)
		MESSAGE("found lib")
	endif(FFTW_LIBRARIES)

	# handle the QUIETLY and REQUIRED arguments and set FFTW_FOUND to TRUE if
	# all listed variables are TRUE
	include (FindPackageHandleStandardArgs)
	find_package_handle_standard_args (FFTW DEFAULT_MSG FFTW_LIBRARIES FFTW_INCLUDES)

	mark_as_advanced (FFTW_LIBRARIES FFTW_INCLUDES)

else(!WIN32 OR APPLE)
	MESSAGE("SEARCHING FFTW FOR WIN32")
		set( FFTW_DIR "${BRAINVIS_ROOT_DIR}/ext/FFTW" CACHE FILEPATH FFTW_DIR )
		include_directories(${FFTW_DIR})
		link_directories(${FFTW_DIR})
endif(!WIN32 OR APPLE)