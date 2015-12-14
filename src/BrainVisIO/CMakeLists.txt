file(GLOB BRAINVISIO_SOURCE *.c *.cpp
                DICOM/*.cpp
				MER-Data/*.cpp
                FFT/*.cpp)
file(GLOB BRAINVISIO_HEADER *.h
                DICOM/*.h
				MER-Data/*.h
                FFT/*.h)



add_library (BRAINVISIO ${BRAINVISIO_SOURCE} ${BRAINVISIO_HEADER} $<TARGET_OBJECTS:mocca>)

target_include_directories (BRAINVISIO PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries (BRAINVISIO LINK_PUBLIC Core)
target_link_libraries (BRAINVISIO LINK_PUBLIC jpeglib)

if (CUDA_FOUND)
	target_link_libraries (BRAINVISIO LINK_PUBLIC CUDAIO)
endif(CUDA_FOUND)

if(WIN32)
    target_link_libraries (BRAINVISIO LINK_PUBLIC Shlwapi)
endif(WIN32)

#MOCCA
include_directories(
					${BRAINVIS_ROOT_DIR}/src/mocca/include/
					${BRAINVIS_ROOT_DIR}/src/mocca/include/mocca/)