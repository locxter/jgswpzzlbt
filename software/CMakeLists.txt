cmake_minimum_required(VERSION 3.0.0)
project(software VERSION 1.0)

include(CTest)
enable_testing()

add_executable(software main.cpp lib/opencv-helpers.hpp lib/opencv-helpers.cpp lib/libserial-helpers.hpp lib/libserial-helpers.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

find_package(OpenCV)
find_package(PkgConfig)
pkg_check_modules(SERIAL libserial)
include_directories(${OpenCV_INCLUDE_DIRS} ${SERIAL_INCLUDE_DIRS})
target_link_libraries(software ${OpenCV_LIBS} ${SERIAL_LDFLAGS})
