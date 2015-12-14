set(SOURCE_ROOT_DIR ${BRAINVIS_ROOT_DIR}/src)
# for now, we will keep our headers at the same location
set(HEADER_ROOT_DIR ${BRAINVIS_ROOT_DIR}/src)

# global visibility
include_directories(
					${BRAINVIS_ROOT_DIR}/ext/ghoul/include/
					${BRAINVIS_ROOT_DIR}/ext/ghoul/include/ghoul/)

add_subdirectory(CudaIO)
add_subdirectory(renderer)
add_subdirectory(BrainVisIO)
add_subdirectory(core)


########## Ghoul Objects +++++++++++>

file(GLOB GHOUL_SOURCE ${BRAINVIS_ROOT_DIR}/ext/ghoul/src/logging/*.cpp
					   ${BRAINVIS_ROOT_DIR}/ext/ghoul/src/cmdparser/*.cpp)
file(GLOB GHOUL_HEADER ${BRAINVIS_ROOT_DIR}/ext/ghoul/include/ghoul/logging/*.h
					   ${BRAINVIS_ROOT_DIR}/ext/ghoul/include/ghoul/cmdparser/*.h)

add_library(ghoul ${GHOUL_SOURCE} ${GHOUL_HEADER})


######### MOCCA +++++++>

INCLUDE_DIRECTORIES(${BRAINVIS_ROOT_DIR}/src)
INCLUDE_DIRECTORIES(${BRAINVIS_ROOT_DIR}/src/mocca/include)

# level 0 modules
ADD_SUBDIRECTORY(mocca)

