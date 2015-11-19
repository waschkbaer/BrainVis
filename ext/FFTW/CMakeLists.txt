# searching for include directory
find_path(FFTW_INCLUDE_DIR fftw3.h)

# searching for library file
find_library(FFTW_LIBRARY libfftw3-3)

MESSAGE("HERE")
if (FFTW_INCLUDE_DIR AND FFTW_LIBRARY)
MESSAGE("HERE")
    # you may need that if further action in your CMakeLists.txt depends
    # on detecting your library
    set(FFTW_FOUND TRUE)

    # you may need that if you want to conditionally compile some parts
    # of your code depending on library availability
    add_definitions(-DHAVE_LIBFFTW=1)

    # those two, you really need
    include_directories(${FFTW_INCLUDE_DIR})
    set(YOUR_LIBRARIES ${YOUR_LIBRARIES} ${FFTW_LIBRARY})
endif ()