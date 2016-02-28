cmake_minimum_required(VERSION 2.8.12)

file(GLOB BRAINVIS_SOURCE 	*.cpp
				DriveTool/*.cpp
				merwidgets/*.cpp
				Utils/*.cpp
			 	muihandler/*.cpp)
file(GLOB BRAINVIS_HEADER 	*.h
				DriveTool/*.h
				merwidgets/*.h
				Utils/*.h
				muihandler/*.h)
file(GLOB SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/Shader/*.glsl)
file(GLOB BUTTONS ${CMAKE_CURRENT_SOURCE_DIR}/Buttons/*.jpg)
file(GLOB MUIJSON ${CMAKE_CURRENT_SOURCE_DIR}/types/BrainVis.json)

project(BrainVis)
set(CMAKE_AUTOMOC ON) 
set(CMAKE_INCLUDE_CURRENT_DIR ON)  

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)  

QT5_WRAP_UI(UIS_HDRS mainwindow.ui)
QT5_WRAP_UI(UIS_HDRS renderwidget.ui)
QT5_WRAP_UI(UIS_HDRS informationwidget.ui)
QT5_WRAP_UI(UIS_HDRS planingwidget.ui)
QT5_WRAP_UI(UIS_HDRS ctregistrationwidget.ui)
QT5_WRAP_UI(UIS_HDRS histogrammwidget.ui)
QT5_WRAP_UI(UIS_HDRS framewidget.ui)
QT5_WRAP_UI(UIS_HDRS mertool.ui)
QT5_WRAP_UI(UIS_HDRS rendersettings.ui)
QT5_WRAP_UI(UIS_HDRS merwidgets/merimageentry.ui)
QT5_WRAP_UI(UIS_HDRS merwidgets/merelectrodeentry.ui)
QT5_WRAP_UI(UIS_HDRS merwidgets/meroptions.ui)
QT5_WRAP_UI(UIS_HDRS muiconnector.ui)
GET_PROPERTY(muipath GLOBAL PROPERTY MUILIB)

add_executable(BrainVis ${BRAINVIS_SOURCE} ${BRAINVIS_HEADER} ${UIS_HDRS} $<TARGET_OBJECTS:mocca> brainvis.rc )

set_target_properties( BrainVis PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BRAINVIS_ROOT_DIR}/build/bin )


file(COPY ${SHADERS} DESTINATION ${CMAKE_SOURCE_DIR}/build/bin/Shader)
file(COPY ${BUTTONS} DESTINATION ${CMAKE_SOURCE_DIR}/build/bin/Buttons)
file(COPY ${MUIJSON} DESTINATION ${CMAKE_SOURCE_DIR}/build/bin/types)

include_directories(../src/Externals/glm)
target_link_libraries(BrainVis Qt5::Widgets)
target_link_libraries(BrainVis Renderer)
target_link_libraries(BrainVis ${OPENGL_LIBRARIES})
target_link_libraries(BrainVis Core)
target_link_libraries(BrainVis ghoul)
target_link_libraries(BrainVis BRAINVISIO)
target_link_libraries(BrainVis ${muipath})

#qt5_use_modules(BrainVis Widgets OpenGL)  

#GHOUL
include_directories(
					${BRAINVIS_ROOT_DIR}/ext/ghoul/include/
					${BRAINVIS_ROOT_DIR}/ext/ghoul/include/ghoul/)

#MOCCA
include_directories(
					${BRAINVIS_ROOT_DIR}/src/mocca/include/
					${BRAINVIS_ROOT_DIR}/src/mocca/include/mocca/)

