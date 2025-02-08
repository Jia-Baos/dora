cmake_minimum_required(VERSION 3.21)

# 手动设置编译器
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(cmake-dataflow LANGUAGES C CXX)

message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Project Dir: ${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fPIC")    # 用于生成可以在内存中任意位置加载并执行的代码

include(DoraTargets.cmake)

# obsensor sdk
set(HiKangSDK_ROOT_DIR ${CMAKE_SOURCE_DIR}/SDK)
set(HiKangSDK_INCLUDE_DIR ${HiKangSDK_ROOT_DIR}/include)
set(HiKangSDK_LIBRARY_DIRS ${HiKangSDK_ROOT_DIR}/lib/x86_64)

message(STATUS "Jia-Baos added, CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "Jia-Baos added, CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB HiKang_LIBS "${HiKangSDK_LIBRARY_DIRS}/lib*.so")
message(STATUS "library: ${HiKang_LIBS}")


find_package(OpenCV REQUIRED)

if(OpenCV_FOUND)
        message(STATUS "OpenCV library status: ")
        message(STATUS "version: ${OpenCV_VERSION}")
        message(STATUS "library: ${OpenCV_LIBS}")
        message(STATUS "include path: ${OpenCV_INCLUDE_DIRS}")
else()
        message(STATUS "Could not find OpenCV library")
endif()

# 指定头文件链接路径
include_directories(${HiKangSDK_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})

# 指定动态库链接路径
link_directories(${HiKangSDK_LIBRARY_DIRS} ${dora_link_dirs})

file(GLOB FILES_SRC "./node-rust-api/*.cc")
file(GLOB FILES_HEADERS "./node-rust-api/*.h")
add_executable(node_rust_api ${FILES_SRC} ${FILES_HEADERS} ${node_bridge})
add_dependencies(node_rust_api Dora_cxx)
target_include_directories(node_rust_api PRIVATE ${dora_cxx_include_dir})
target_link_libraries(node_rust_api PRIVATE m rt dl pthread dora_node_api_cxx ${OpenCV_LIBS} MvCameraControl)

install(TARGETS node_rust_api DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)