file(GLOB CORE_SOURCE   *.cpp
                        IO/*.cpp
                        Math/*.cpp
			Other/*.cpp
			Checksums/*.cpp
			Controller/*.cpp
			Systeminfo/*.cpp
                        Time/*.cpp)
file(GLOB CORE_HEADER   *.h
                        IO/*.h
                        Math/*.h
			Other/*.h
			Checksums/*.h
			Controller/*.h
			Systeminfo/*.h
                        Time/*.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library (Core ${CORE_SOURCE} ${CORE_HEADER})

target_include_directories (Core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)


if(WIN32)
    target_link_libraries (Core LINK_PUBLIC Shlwapi)
endif(WIN32)
