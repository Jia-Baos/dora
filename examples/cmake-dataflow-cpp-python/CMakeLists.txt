cmake_minimum_required(VERSION 3.21)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(cmake-dataflow LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fPIC")

include(DoraTargets.cmake)

link_directories(${dora_link_dirs})

add_executable(node_cpp_api node-cpp-api/main.cc ${node_bridge})
add_dependencies(node_cpp_api Dora_cxx)
target_include_directories(node_cpp_api PRIVATE ${dora_cxx_include_dir})
target_link_libraries(node_cpp_api m rt dl pthread dora_node_api_cxx)

install(TARGETS node_cpp_api DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)
