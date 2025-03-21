cmake_minimum_required(VERSION 3.21)

project(node_cpp_api)

message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Project Dir: ${CMAKE_CURRENT_SOURCE_DIR}")

link_directories(${dora_link_dirs})

file(GLOB FILES_SRC "./*.cc")
file(GLOB FILES_HEADERS "./*.h")

add_executable(${PROJECT_NAME} ${FILES_SRC} ${node_bridge})
add_dependencies(${PROJECT_NAME} Dora_cxx)

target_include_directories(${PROJECT_NAME} PRIVATE ${dora_cxx_include_dir})
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS} ${BASE_FILES_HEADERS_PATH} ../camera2D/hikang ../camera2D/orbbec)

target_link_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/install/lib)

target_link_libraries(${PROJECT_NAME} PRIVATE  m rt dl pthread dora_node_api_cxx orbbec hikang ${OpenCV_LIBS})
# install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_SOURCE_DIR}/bin)

install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
