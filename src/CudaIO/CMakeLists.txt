find_package(CUDA)

if (CUDA_FOUND)
	file(GLOB CUDAIO_SOURCE *.cu)
	file(GLOB CUDAIO_HEADER *.h)

	CUDA_ADD_LIBRARY(CUDAIO ${CUDAIO_SOURCE} ${CUDAIO_HEADER} )

	#target_include_directories (CUDAIO  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
	#target_link_libraries (CUDAIO  LINK_PUBLIC Core)
	#target_link_libraries (CUDAIO  LINK_PUBLIC jpeglib)
endif(CUDA_FOUND)