cmake_minimum_required(VERSION 3.0.0)
project(manual-control VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(manual-control main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

find_package(OpenCV REQUIRED)
find_package(PkgConfig)
pkg_check_modules(SERIAL libserial)
include_directories(${OpenCV_INCLUDE_DIRS} ${SERIAL_INCLUDE_DIRS})
target_link_libraries(manual-control ${OpenCV_LIBS} ${SERIAL_LDFLAGS})