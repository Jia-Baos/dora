cmake_minimum_required(VERSION 3.21)

project(operator_plot)

message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Project Dir: ${CMAKE_CURRENT_SOURCE_DIR}")

link_directories(${dora_link_dirs})

file(GLOB FILES_SRC "./*.cc")
file(GLOB FILES_HEADERS "./*.h")

add_library(${PROJECT_NAME} SHARED ${FILES_SRC} ${operator_bridge})
add_dependencies(${PROJECT_NAME} Dora_cxx)

target_include_directories(${PROJECT_NAME} PRIVATE ${dora_cxx_include_dir} ${dora_c_include_dir} ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} dora_operator_api_cxx ${OpenCV_LIBS})

install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "CMakeLists.txt" EXCLUDE)
