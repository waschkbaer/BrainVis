set(Python_ADDITIONAL_VERSIONS 2.7)
find_package(PythonInterp REQUIRED)
if (${PYTHON_VERSION_MAJOR} EQUAL "3")
	MESSAGE(FATAL_ERROR "python version 2.x required")
endif ()
