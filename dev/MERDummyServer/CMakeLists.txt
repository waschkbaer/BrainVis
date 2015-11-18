file(GLOB MERDUMMY_SOURCE *.cpp)
file(GLOB MERDUMMY_HEADER *.h)

add_executable(MERDummyServer ${MERDUMMY_SOURCE} ${MERDUMMY_HEADER} $<TARGET_OBJECTS:mocca>)

set_target_properties( MERDummyServer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BRAINVIS_ROOT_DIR}/build/bin )

target_link_libraries(MERDummyServer  LINK_PUBLIC Core)
target_link_libraries(MERDummyServer  LINK_PUBLIC ghoul)

#GHOUL
include_directories(
					${BRAINVIS_ROOT_DIR}/ext/ghoul/include/
					${BRAINVIS_ROOT_DIR}/ext/ghoul/include/ghoul/)

#MOCCA
include_directories(
					${BRAINVIS_ROOT_DIR}/src/mocca/include/
					${BRAINVIS_ROOT_DIR}/src/mocca/include/mocca/)

