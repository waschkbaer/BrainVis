set(SOURCE_ROOT_DIR ${BRAINVIS_ROOT_DIR}/src)
# for now, we will keep our headers at the same location
set(HEADER_ROOT_DIR ${BRAINVIS_ROOT_DIR}/src)

# global visibility
include_directories(
					${BRAINVIS_ROOT_DIR}/ext/ghoul/include/
					${BRAINVIS_ROOT_DIR}/ext/ghoul/include/ghoul/)





########## Ghoul Objects +++++++++++>

file(GLOB GHOUL_SOURCE ${BRAINVIS_ROOT_DIR}/ext/ghoul/src/logging/*.cpp
					   ${BRAINVIS_ROOT_DIR}/ext/ghoul/src/cmdparser/*.cpp)
file(GLOB GHOUL_HEADER ${BRAINVIS_ROOT_DIR}/ext/ghoul/include/ghoul/logging/*.h
					   ${BRAINVIS_ROOT_DIR}/ext/ghoul/include/ghoul/cmdparser/*.h)

add_library(ghoul ${GHOUL_SOURCE} ${GHOUL_HEADER})


######### MOCCA +++++++>

INCLUDE_DIRECTORIES(${BRAINVIS_ROOT_DIR}/src)
INCLUDE_DIRECTORIES(${BRAINVIS_ROOT_DIR}/src/mocca/include)

#ADD_SUBDIRECTORY(CudaIO)
ADD_SUBDIRECTORY(renderer)
ADD_SUBDIRECTORY(io)
ADD_SUBDIRECTORY(core)
ADD_SUBDIRECTORY(mocca)
ADD_SUBDIRECTORY(mui)
ADD_SUBDIRECTORY(brainVis)
