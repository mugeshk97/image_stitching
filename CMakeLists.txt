cmake_minimum_required( VERSION 3.6 )

project( stitcher )
add_executable( ${PROJECT_NAME} main.cpp )
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_package(OpenCV 4.5.2 REQUIRED PATHS "/home/mugesh/Project/vechile_distance_estimation/opencv")
find_package(Eigen3 REQUIRED)
find_package(VTK REQUIRED)

include_directories( ${OpenCV_INCLUDE_DIRS} )
link_directories( ${OpenCV_LIB_DIR} )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
