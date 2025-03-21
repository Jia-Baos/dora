cmake_minimum_required(VERSION 3.21)

set(BASE_FILES_HEADERS_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# collect the necessary header files.
file(GLOB BASE_FILES_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_subdirectory(camera2D)
# add_subdirectory(camera3D)
add_subdirectory(cameraRun)
add_subdirectory(node-cpp-api)
add_subdirectory(node-plot)
add_subdirectory(operator-plot)
