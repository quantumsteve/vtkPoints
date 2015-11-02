
cmake_minimum_required(VERSION 3.1)

PROJECT(setvtkPoints)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_STANDARD 14)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(setvtkPoints setvtkPoints.cxx)
add_executable(createvtkPoints createvtkPoints.cxx)

target_link_libraries(setvtkPoints ${VTK_LIBRARIES})
target_link_libraries(createvtkPoints ${VTK_LIBRARIES})
