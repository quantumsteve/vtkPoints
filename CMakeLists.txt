
cmake_minimum_required(VERSION 3.1)

PROJECT(setvtkPoints)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 14)

find_package(VTK REQUIRED)
find_package(TBB REQUIRED)
include(${VTK_USE_FILE})

add_executable(setvtkPoints setvtkPoints.cxx)
add_executable(createvtkPoints createvtkPoints.cxx)

target_link_libraries(setvtkPoints ${VTK_LIBRARIES} ${TBB_LIBRARIES})
target_link_libraries(createvtkPoints ${VTK_LIBRARIES} ${TBB_LIBRARIES})
