set(SOURCE_ROOT_DIR ${TRINITY_ROOT_DIR}/src)
# for now, we will keep our headers at the same location
set(HEADER_ROOT_DIR ${TRINITY_ROOT_DIR}/src)

# global visibility
include_directories(
					${TRINITY_ROOT_DIR}/ext/ghoul/include/
					${TRINITY_ROOT_DIR}/ext/ghoul/include/ghoul/)

add_subdirectory(renderer)
add_subdirectory(BrainVisIO)
add_subdirectory(core)

########## Ghoul Objects +++++++++++>

file(GLOB GHOUL_SOURCE ${TRINITY_ROOT_DIR}/ext/ghoul/src/logging/*.cpp
					   ${TRINITY_ROOT_DIR}/ext/ghoul/src/cmdparser/*.cpp)
file(GLOB GHOUL_HEADER ${TRINITY_ROOT_DIR}/ext/ghoul/include/ghoul/logging/*.h
					   ${TRINITY_ROOT_DIR}/ext/ghoul/include/ghoul/cmdparser/*.h)

add_library(ghoul ${GHOUL_SOURCE} ${GHOUL_HEADER})


######### MOCCA +++++++>

INCLUDE_DIRECTORIES(${TRINITY_ROOT_DIR}/src)
INCLUDE_DIRECTORIES(${TRINITY_ROOT_DIR}/src/mocca/include)

# level 0 modules
ADD_SUBDIRECTORY(mocca)

