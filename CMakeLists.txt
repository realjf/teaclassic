cmake_minimum_required(VERSION 3.12)
set(PROJECT_NAME teaclassic)

set(TEACLASSIC_MAJOR_VERSION 0)
set(TEACLASSIC_MINOR_VERSION 0)
set(TEACLASSIC_MICRO_VERSION 1)
set(TEACLASSIC_VERSION "${TEACLASSIC_MAJOR_VERSION}.${TEACLASSIC_MINOR_VERSION}.${TEACLASSIC_MICRO_VERSION}")


project(${PROJECT_NAME} LANGUAGES C CXX VERSION ${TEACLASSIC_VERSION})

# if(CMAKE_VERSION VERSION_GREATER 3.12)
#     set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
# endif()


if(POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()

# set executable output path
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# set library output path
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_VERBOSE_MAKEFILE ON)

include(cmake/common.cmake)
include(cmake/common_macros.cmake)
include(cmake/sdl.cmake)
include(cmake/glew.cmake)



include_directories(.)
include_directories(deps/SDL2/include)
include_directories(deps/openal-soft/include)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-volatile")


SETUP_APP(${PROJECT_NAME} ${TEACLASSIC_VERSION})


target_link_libraries(${PROJECT_NAME} sdl2)
