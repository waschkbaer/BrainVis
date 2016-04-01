file(GLOB REND2_SOURCE   *.cpp
			openGL/*.cpp
			fusion/*.cpp
                        openGL/GLCore/*.cpp
                        openGL/GLVolume/*.cpp
			tools/*.cpp)
file(GLOB REND2_HEADER   *.h
                        openGL/*.h
			fusion/*.h
                        openGL/GLCore/*.h
                        openGL/GLVolume/*.h
			tools/*.h)
file(GLOB REND2_SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/Shader/*.glsl)

option(ADD_CUDA_SUPPORT "adds cuda classes" off)

if (ADD_CUDA_SUPPORT)
	find_package(CUDA REQUIRED)  

	set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS})  
	cuda_compile(CUDA_OBJECTS C:/Users/andre/Documents/BrainVis/src/renderer/Fusion/CudaKernel.cu)  
endif()

add_definitions(-DGLEW_STATIC)
add_library (Renderer ${REND2_SOURCE} ${REND2_HEADER} ${REND2_SHADERS} $<TARGET_OBJECTS:mocca> ${CUDA_OBJECTS})

target_include_directories (Renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories (Renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Shader)

target_link_libraries (Renderer Core)
target_link_libraries (Renderer BRAINVISIO)
target_link_libraries (Renderer glew)
target_link_libraries (Renderer ghoul)

if (ADD_CUDA_SUPPORT)
	target_link_libraries (Renderer ${CUDA_LIBRARIES}) 

	CUDA_ADD_CUBLAS_TO_TARGET (Renderer)
	CUDA_ADD_CUFFT_TO_TARGET (Renderer)
endif()

#MOCCA
include_directories(
					${BRAINVIS_ROOT_DIR}/src/mocca/include/
					${BRAINVIS_ROOT_DIR}/src/mocca/include/mocca/)
