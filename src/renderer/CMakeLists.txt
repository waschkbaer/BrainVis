file(GLOB REND2_SOURCE   *.cpp
			OpenGL/*.cpp
			NetworkRenderer/*.cpp
			DICOMRenderer/*.cpp
			DICOMRenderer/Fusion/*.cpp
                        OpenGL/GLCore/*.cpp
                        OpenGL/GLVolume/*.cpp
                        Context/*.cpp
						Service/*.cpp)
file(GLOB REND2_HEADER   *.h
                        OpenGL/*.h
			NetworkRenderer/*.h
			DICOMRenderer/*.h
			DICOMRenderer/Fusion/*.h
                        OpenGL/GLCore/*.h
                        OpenGL/GLVolume/*.h
                        Context/*.h
			Service/*.h)
file(GLOB REND2_SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/Shader/*.glsl)

option(ADD_CUDA_SUPPORT "adds cuda classes" off)

if (TRINITY_BUILD_FRONTEND)
	find_package(CUDA REQUIRED)  

	set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS})  
	cuda_compile(CUDA_OBJECTS C:/Users/andre/Documents/BrainVis/src/renderer/DICOMRenderer/Fusion/CudaKernel.cu)  
endif()

add_definitions(-DGLEW_STATIC)
add_library (Renderer ${REND2_SOURCE} ${REND2_HEADER} ${REND2_SHADERS} $<TARGET_OBJECTS:mocca> ${CUDA_OBJECTS})

target_include_directories (Renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories (Renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Shader)

target_link_libraries (Renderer Core)
target_link_libraries (Renderer BRAINVISIO)
target_link_libraries (Renderer glew)
target_link_libraries (Renderer ghoul)

if (TRINITY_BUILD_FRONTEND)
	target_link_libraries (Renderer ${CUDA_LIBRARIES}) 

	CUDA_ADD_CUBLAS_TO_TARGET (Renderer)
	CUDA_ADD_CUFFT_TO_TARGET (Renderer)
endif()

#MOCCA
include_directories(
					${BRAINVIS_ROOT_DIR}/src/mocca/include/
					${BRAINVIS_ROOT_DIR}/src/mocca/include/mocca/)
