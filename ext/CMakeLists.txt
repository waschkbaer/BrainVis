add_subdirectory(glew)
add_subdirectory(glm)
add_subdirectory(jpeglib)
#add_subdirectory(morphableui)


macro(ext_folder EXT_TARGET)
	if(TARGET ${EXT_TARGET})
		set_target_properties(${EXT_TARGET} PROPERTIES FOLDER "Externals")
	endif()
endmacro(ext_folder)

ext_folder(glew)
ext_folder(jpeglib)
#ext_folder(morphableui)
ext_folder(uninstall) #from glfw