cmake_minimum_required(VERSION 2.8.12)

file(GLOB BRAINVIS_SOURCE 	*.cpp
				uis/*.cpp
				uis/widgets/*.cpp
				uis/settings/*.cpp
				uis/tools/*.cpp
				DriveTool/*.cpp
				merwidgets/*.cpp
				Utils/*.cpp
			 	muihandler/*.cpp)
file(GLOB BRAINVIS_HEADER 	*.h
				uis/*.h
				uis/widgets/*.h
				uis/settings/*.h
				uis/tools/*.h
				DriveTool/*.h
				merwidgets/*.h
				Utils/*.h
				muihandler/*.h)
file(GLOB SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/Shader/*.glsl)
file(GLOB BUTTONS ${CMAKE_CURRENT_SOURCE_DIR}/Buttons/*.jpg)
file(GLOB MUIJSON ${CMAKE_CURRENT_SOURCE_DIR}/types/BrainVis.json)

project(brainVis)
set(CMAKE_AUTOMOC ON) 
set(CMAKE_INCLUDE_CURRENT_DIR ON)  

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)  

QT5_WRAP_UI(UIS_HDRS uis/mainwindow.ui)
QT5_WRAP_UI(UIS_HDRS uis/widgets/renderwidget.ui)
QT5_WRAP_UI(UIS_HDRS uis/settings/planingwidget.ui)
QT5_WRAP_UI(UIS_HDRS uis/tools/ctregistrationwidget.ui)
QT5_WRAP_UI(UIS_HDRS uis/widgets/histogrammwidget.ui)
QT5_WRAP_UI(UIS_HDRS uis/tools/framewidget.ui)
QT5_WRAP_UI(UIS_HDRS uis/widgets/mertool.ui)
QT5_WRAP_UI(UIS_HDRS uis/settings/rendersettings.ui)
QT5_WRAP_UI(UIS_HDRS merwidgets/merimageentry.ui)
QT5_WRAP_UI(UIS_HDRS merwidgets/merelectrodeentry.ui)
QT5_WRAP_UI(UIS_HDRS merwidgets/meroptions.ui)
QT5_WRAP_UI(UIS_HDRS uis/tools/muiconnector.ui)
GET_PROPERTY(muipath GLOBAL PROPERTY MUILIB)

add_executable(brainVis ${BRAINVIS_SOURCE} ${BRAINVIS_HEADER} ${UIS_HDRS} $<TARGET_OBJECTS:mocca> brainvis.rc )

set_target_properties( brainVis PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BRAINVIS_ROOT_DIR}/build/bin )


file(COPY ${SHADERS} DESTINATION ${CMAKE_SOURCE_DIR}/build/bin/Shader)
file(COPY ${BUTTONS} DESTINATION ${CMAKE_SOURCE_DIR}/build/bin/Buttons)
file(COPY ${MUIJSON} DESTINATION ${CMAKE_SOURCE_DIR}/build/bin/types)

include_directories(../src/Externals/glm)
target_link_libraries(brainVis Qt5::Widgets)
target_link_libraries(brainVis Renderer)
target_link_libraries(brainVis ${OPENGL_LIBRARIES})
target_link_libraries(brainVis Core)
target_link_libraries(brainVis ghoul)
target_link_libraries(brainVis BRAINVISIO)
target_link_libraries(brainVis ${muipath})

#qt5_use_modules(brainVis Widgets OpenGL)  

#GHOUL
include_directories(
					${BRAINVIS_ROOT_DIR}/ext/ghoul/include/
					${BRAINVIS_ROOT_DIR}/ext/ghoul/include/ghoul/)

#MOCCA
include_directories(
					${BRAINVIS_ROOT_DIR}/src/mocca/include/
					${BRAINVIS_ROOT_DIR}/src/mocca/include/mocca/)

