MACRO(ExtFolder EXT_TARGET)
	IF(TARGET ${EXT_TARGET})
		SET_TARGET_PROPERTIES(${EXT_TARGET} PROPERTIES FOLDER "Externals")
	ENDIF()
ENDMACRO(ExtFolder)