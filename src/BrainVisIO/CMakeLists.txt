file(GLOB BRAINVISIO_SOURCE *.c *.cpp
                DICOM/*.cpp
		MER-Data/*.cpp
                FFT/*.cpp)
file(GLOB BRAINVISIO_HEADER *.h
                DICOM/*.h
		MER-Data/*.h
                FFT/*.h)

add_library (BRAINVISIO ${BRAINVISIO_SOURCE} ${BRAINVISIO_HEADER})

target_include_directories (BRAINVISIO PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries (BRAINVISIO LINK_PUBLIC Core)
target_link_libraries (BRAINVISIO LINK_PUBLIC jpeglib)

if(WIN32)
    target_link_libraries (BRAINVISIO LINK_PUBLIC Shlwapi)
endif(WIN32)