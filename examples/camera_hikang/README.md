# Dora CMake Dataflow Example

This example shows how to create dora operators and custom nodes in CMake build system.

See also [c++-example](https://github.com/dora-rs/dora/blob/main/examples/c%2B%2B-dataflow/README.md) for the implementation details of operator and node.

## Compile and Run

To try it out, you can use the [`run.rs`](./run.rs) binary. It performs all required build steps and then starts the dataflow. Use the following command to run it: `cargo run --example cmake-dataflow`.

## Out-of-tree compile

This example also can be ran in a separate root directory.
```
cd <path-to-cmake-dataflow>
mkdir build
cd build && cmake ..
make install
cd ..
dora up
dora start dataflow.yml
```

为用户添加打开 usb 设备的权限，设置后需要重启电脑

```
sudo usermod -a -G dialout username
```

我们已经使得代码顺利编译通过，但是通过 dora 启动的过程中，程序似乎无法正确打开 usb 设备，这这有待进一步探索

```
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
set(OrbbecSDK_ROOT_DIR ${CMAKE_SOURCE_DIR}/SDK1.10.18)
set(OrbbecSDK_INCLUDE_DIR ${OrbbecSDK_ROOT_DIR}/include)
set(OrbbecSDK_LIBRARY_DIRS ${OrbbecSDK_ROOT_DIR}/lib/x86_64)

message(STATUS "Jia-Baos added, CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "Jia-Baos added, CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB ORBBEC_LIBS "${OrbbecSDK_LIBRARY_DIRS}/lib*.so")
message(STATUS "library: ${ORBBEC_LIBS}")


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
include_directories(${OrbbecSDK_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})

# 指定动态库链接路径
link_directories(${OrbbecSDK_LIBRARY_DIRS} ${dora_link_dirs})

file(GLOB FILES_SRC "./node-rust-api/*.cc")
file(GLOB FILES_HEADERS "./node-rust-api/*.h")
add_executable(node_rust_api ${FILES_SRC} ${FILES_HEADERS} ${node_bridge})
add_dependencies(node_rust_api Dora_cxx)
target_include_directories(node_rust_api PRIVATE ${dora_cxx_include_dir})
target_link_libraries(node_rust_api PUBLIC m rt dl pthread dora_node_api_cxx ${ORBBEC_LIBS} ${OpenCV_LIBS})

install(TARGETS node_rust_api DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
```