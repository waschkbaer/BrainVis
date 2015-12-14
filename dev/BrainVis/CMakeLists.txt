cmake_minimum_required(VERSION 2.8.11)

file(GLOB BRAINVIS_SOURCE 	*.cpp
				DriveTool/*.cpp
				Utils/*.cpp)
file(GLOB BRAINVIS_HEADER 	*.h
				DriveTool/*.h
				Utils/*.h)
file(GLOB SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/Shader/*.glsl)
file(GLOB BUTTONS ${CMAKE_CURRENT_SOURCE_DIR}/Buttons/*.jpg)
file(GLOB DEMOMER ${CMAKE_CURRENT_SOURCE_DIR}/MER/*.csv)

#set (CMAKE_LIBRARY_PATH "C:\\Program Files (x86)\\Windows Kits\\8.0\\Lib\\win8\\um\\x64")
#set (CMAKE_PREFIX_PATH "C:\\Qt5.5\\msvc2013_64")
#set (CMAKE_PREFIX_PATH "/home/waschkbaer/Qt/5.5/gcc_64/lib/cmake/Qt5Widgets")
set (CMAKE_PREFIX_PATH "C:\\Qt\\5.5\\msvc2013_64")

project(BrainVis)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)

QT5_WRAP_UI(UIS_HDRS mainwindow.ui)
QT5_WRAP_UI(UIS_HDRS renderwidget.ui)
QT5_WRAP_UI(UIS_HDRS informationwidget.ui)
QT5_WRAP_UI(UIS_HDRS planingwidget.ui)
QT5_WRAP_UI(UIS_HDRS drivewidget.ui)
QT5_WRAP_UI(UIS_HDRS ctregistrationwidget.ui)
QT5_WRAP_UI(UIS_HDRS histogrammwidget.ui)

add_executable(BrainVis ${BRAINVIS_SOURCE} ${BRAINVIS_HEADER} ${UIS_HDRS} $<TARGET_OBJECTS:mocca> brainvis.rc)

set_target_properties( BrainVis PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BRAINVIS_ROOT_DIR}/build/bin )


file(COPY ${SHADERS} DESTINATION ${CMAKE_SOURCE_DIR}/build/bin/Shader)
file(COPY ${BUTTONS} DESTINATION ${CMAKE_SOURCE_DIR}/build/bin/Buttons)
file(COPY ${DEMOMER} DESTINATION ${CMAKE_SOURCE_DIR}/build/bin/MER)

include_directories(../src/Externals/glm)
target_link_libraries(BrainVis LINK_PUBLIC Qt5::Widgets)
target_link_libraries(BrainVis LINK_PUBLIC Renderer)
target_link_libraries(BrainVis LINK_PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(BrainVis LINK_PUBLIC Core)
target_link_libraries(BrainVis LINK_PUBLIC ghoul)
target_link_libraries(BrainVis LINK_PUBLIC BRAINVISIO)

#GHOUL
include_directories(
					${BRAINVIS_ROOT_DIR}/ext/ghoul/include/
					${BRAINVIS_ROOT_DIR}/ext/ghoul/include/ghoul/)

#MOCCA
include_directories(
					${BRAINVIS_ROOT_DIR}/src/mocca/include/
					${BRAINVIS_ROOT_DIR}/src/mocca/include/mocca/)

