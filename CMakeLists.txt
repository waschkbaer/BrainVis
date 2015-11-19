cmake_minimum_required(VERSION 2.8.4)
project(BrainVis)

# setting the project root directory
set(BRAINVIS_ROOT_DIR ${PROJECT_SOURCE_DIR})
add_definitions(-DBRAINVIS_ROOT_DIR="${BRAINVIS_ROOT_DIR}")

# install libraries to ~/local if you don't have sudo permissions; fixme dmc: remove when refactoring is finished
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ~/local)


set(CMAKE_INSTALL_PREFIX ${BRAINVIS_ROOT_DIR}/build/install)
set(CMAKE_MODULE_PATH ${BRAINVIS_ROOT_DIR}/cmake/modules)
set(CMAKE_PLATFORM_PATH ${BRAINVIS_ROOT_DIR}/cmake/platform)


SET(CMAKE_FIND_LIBRARY_PREFIXES "")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")




#turn on using solution folders
#todo verify if needed
set_property( GLOBAL PROPERTY USE_FOLDERS ON)

#todo is this really necessary?
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif(!WIN32)
        message(STATUS "${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
   message(STATUS "Setting build type to 'Release' as none was specified.")
   set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
   # Set the possible values of build type for cmake-gui
   set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Release"
     "MinSizeRel" "RelWithDebInfo")
endif()

find_package(OpenGL REQUIRED)

add_definitions(-DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED)

if(WIN32)
    ADD_DEFINITIONS(-DUNICODE)
    ADD_DEFINITIONS(-D_UNICODE)
endif(WIN32)

if(UNIX AND NOT APPLE)
    find_package(Threads)
endif(UNIX AND NOT APPLE)

add_subdirectory(src)
add_subdirectory(ext)
add_subdirectory(dev)
