
cmake_minimum_required(VERSION 3.21)

project(hikang)

# 定义项目版本号宏
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

# 生成版本号字符串
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "Project Dir: ${CMAKE_CURRENT_SOURCE_DIR}")

# 将源代码添加到此项目的可执行文件。
# aux_source_directory(. main_files)
file(GLOB FILES_SRC "./*.cc")
file(GLOB FILES_HEADERS "./*.h")

add_library(${PROJECT_NAME} SHARED ${FILES_SRC} ${FILES_HEADERS})
# set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.${PROJECT_VERSION}")

target_include_directories(${PROJECT_NAME} PUBLIC ${BASE_FILES_HEADERS_PATH} ${HiKang_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS})

target_link_directories(${PROJECT_NAME} PUBLIC ${HiKang_LIBRARY_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS} MvCameraControl)

install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(FILES ${BASE_FILES_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "CMakeLists.txt" EXCLUDE)
