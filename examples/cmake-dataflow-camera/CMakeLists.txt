cmake_minimum_required(VERSION 3.21)

# set clang compiler
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(cmake-dataflow LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fPIC")

include(DoraTargets.cmake)

# HiKang SDK
set(HiKang_ROOT_DIR ${PROJECT_SOURCE_DIR}/3rdparty/hikang)
set(HiKang_INCLUDE_DIR ${HiKang_ROOT_DIR}/include)
set(HiKang_LIBRARY_DIR ${HiKang_ROOT_DIR}/lib/x86_64)

file(GLOB HiKang_LIBS "${HiKang_LIBRARY_DIR}/lib*.so")
# message(STATUS "library: ${HiKang_LIBS}")

# Orbbec SDK
set(Orbbec_ROOT_DIR ${PROJECT_SOURCE_DIR}/3rdparty/orbbec)
set(Orbbec_INCLUDE_DIR ${Orbbec_ROOT_DIR}/include)
set(Orbbec_LIBRARY_DIR ${Orbbec_ROOT_DIR}/lib/x86_64)

file(GLOB ORBBEC_LIBS "${Orbbec_LIBRARY_DIR}/lib*.so")
# message(STATUS "library: ${ORBBEC_LIBS}")

find_package(OpenCV REQUIRED)

if(OpenCV_FOUND)
        message(STATUS "OpenCV library status: ")
        message(STATUS "version: ${OpenCV_VERSION}")
        message(STATUS "library: ${OpenCV_LIBS}")
        message(STATUS "include path: ${OpenCV_INCLUDE_DIRS}")
else()
        message(STATUS "Could not find OpenCV library")
endif()

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)
file(COPY ${PROJECT_SOURCE_DIR}/README.md DESTINATION ${PROJECT_SOURCE_DIR}/install)
add_subdirectory(src)
