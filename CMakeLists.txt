cmake_minimum_required(VERSION 3.16)
project(RegionOfInterest)

set(CMAKE_CXX_STANDARD 14)

add_executable(${PROJECT_NAME} src/main.cpp)

# ADDED (Optimum Tips)
set(OpenCV_STATIC ON)
set(OpenCV_CUDA OFF)
#set(OpenCV_DIR C:\\opencv\\mingw-build)

set(OpenCV_DIR C:\\opencv\\mingw-build\\install)
set(OpenCV_INCLUDE_DIR C:\\opencv\\mingw-build\\install\\include)

find_package(OpenCV REQUIRED)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
# ADDED
