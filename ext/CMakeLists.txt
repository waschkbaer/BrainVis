add_subdirectory(glew)
add_subdirectory(glfw-3.1)
add_subdirectory(glm)
add_subdirectory(jpeglib)

target_include_directories(glfw PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.1/include>
    $<INSTALL_INTERFACE:glfw-3.1/include>  # <prefix>/include/mylib
)


macro(ext_folder EXT_TARGET)
	if(TARGET ${EXT_TARGET})
		set_target_properties(${EXT_TARGET} PROPERTIES FOLDER "Externals")
	endif()
endmacro(ext_folder)

ext_folder(glew)
ext_folder(jpeglib)
ext_folder(uninstall) #from glfw