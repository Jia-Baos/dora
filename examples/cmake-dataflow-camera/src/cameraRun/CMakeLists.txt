cmake_minimum_required(VERSION 3.21)

project(camera_run)

message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Project Dir: ${CMAKE_CURRENT_SOURCE_DIR}")

# 将源代码添加到此项目的可执行文件。
# aux_source_directory(. main_files)
file(GLOB FILES_SRC "./*.cc")
file(GLOB FILES_HEADERS "./*.h")
add_executable(${PROJECT_NAME} ${FILES_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ${BASE_FILES_HEADERS_PATH} ../camera2D/hikang ../camera2D/orbbec)

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/install/lib)

target_link_libraries(${PROJECT_NAME} PRIVATE pthread orbbec hikang)

install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
