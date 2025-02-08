set(DORA_ROOT_DIR "/home/seer/Project-Rust/dora" CACHE FILEPATH "Path to the root of dora")

set(dora_c_include_dir "${CMAKE_CURRENT_BINARY_DIR}/include/c")
set(dora_cxx_include_dir "${CMAKE_CURRENT_BINARY_DIR}/include/cxx")

set(node_bridge "${CMAKE_CURRENT_BINARY_DIR}/node_bridge.cc")
set(operator_bridge "${CMAKE_CURRENT_BINARY_DIR}/operator_bridge.cc")

message(STATUS "Jia-Baos added, DORA_ROOT_DIR: ${DORA_ROOT_DIR}")
message(STATUS "Jia-Baos added, CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

if(DORA_ROOT_DIR)

    # build package
    include(ExternalProject)
    ExternalProject_Add(
        Dora    # 定义一个外部项目 Dora，并指定其构建过程
        SOURCE_DIR ${DORA_ROOT_DIR} # 指定外部项目的源代码目录为变量 DORA_ROOT_DIR 的值
        BUILD_IN_SOURCE True    # 指定在源代码目录中进行构建（而不是在单独的构建目录中）
        CONFIGURE_COMMAND ""
        BUILD_COMMAND
            cargo build
            --package dora-node-api-c
            --release
            &&
            cargo build
            --package dora-operator-api-c
            --release
            &&
            cargo build
            --package dora-node-api-cxx
            --release
            &&
            cargo build
            --package dora-operator-api-cxx
            --release
        INSTALL_COMMAND ""
    )
    
    add_custom_command(OUTPUT ${node_bridge} ${dora_cxx_include_dir} ${operator_bridge} ${dora_c_include_dir}
        WORKING_DIRECTORY ${DORA_ROOT_DIR}
        DEPENDS Dora    # Dora 目标构建后此命令才会构建
        COMMAND
            mkdir ${dora_c_include_dir} -p  # -p 选项表示如果目录已存在，不会报错
            &&
            mkdir ${dora_cxx_include_dir} -p
            &&
            cp target/cxxbridge/dora-node-api-cxx/src/lib.rs.cc ${node_bridge}  # node bridge header file
            &&
            cp target/cxxbridge/dora-node-api-cxx/src/lib.rs.h ${dora_cxx_include_dir}/dora-node-api.h  # node bridge src file
            &&
            cp target/cxxbridge/dora-operator-api-cxx/src/lib.rs.cc ${operator_bridge}  # operator bridge header file
            &&
            cp target/cxxbridge/dora-operator-api-cxx/src/lib.rs.h ${dora_cxx_include_dir}/dora-operator-api.h  # operator bridge src file
            &&
            cp apis/c/node ${CMAKE_CURRENT_BINARY_DIR}/include/c -r # node file
            &&
            cp apis/c/operator ${CMAKE_CURRENT_BINARY_DIR}/include/c -r # operator file
    )
    
    # 指定 Dora_c 目标依赖于 ${dora_c_include_dir}。这意味着在构建 Dora_c 时，CMake 会确保 ${dora_c_include_dir} 已经存在或被生成
    add_custom_target(Dora_c DEPENDS ${dora_c_include_dir})
    add_custom_target(Dora_cxx DEPENDS ${node_bridge} ${operator_bridge} ${dora_cxx_include_dir})
    set(dora_link_dirs ${DORA_ROOT_DIR}/target/release)
else()
    include(ExternalProject)
    ExternalProject_Add(Dora
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/dora
        GIT_REPOSITORY https://github.com/dora-rs/dora.git
        GIT_TAG main
        BUILD_IN_SOURCE True
        CONFIGURE_COMMAND ""
        BUILD_COMMAND
            cargo build
            --package dora-node-api-c
            --target-dir ${CMAKE_CURRENT_BINARY_DIR}/dora/src/Dora/target
            &&
            cargo build
            --package dora-operator-api-c
            --target-dir ${CMAKE_CURRENT_BINARY_DIR}/dora/src/Dora/target
            &&
            cargo build
            --package dora-node-api-cxx
            --target-dir ${CMAKE_CURRENT_BINARY_DIR}/dora/src/Dora/target
            &&
            cargo build
            --package dora-operator-api-cxx
            --target-dir ${CMAKE_CURRENT_BINARY_DIR}/dora/src/Dora/target
        INSTALL_COMMAND ""
    )

    add_custom_command(OUTPUT ${node_bridge} ${dora_cxx_include_dir} ${operator_bridge} ${dora_c_include_dir}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dora/src/Dora/target
        DEPENDS Dora
        COMMAND
            mkdir ${CMAKE_CURRENT_BINARY_DIR}/include/c -p
            &&
            mkdir ${dora_cxx_include_dir} -p
            &&
            cp cxxbridge/dora-node-api-cxx/src/lib.rs.cc ${node_bridge}
            &&
            cp cxxbridge/dora-node-api-cxx/src/lib.rs.h ${dora_cxx_include_dir}/dora-node-api.h
            &&
            cp cxxbridge/dora-operator-api-cxx/src/lib.rs.cc ${operator_bridge}
            &&
            cp cxxbridge/dora-operator-api-cxx/src/lib.rs.h ${dora_cxx_include_dir}/dora-operator-api.h
            &&
            cp ../apis/c/node ${CMAKE_CURRENT_BINARY_DIR}/include/c -r
            &&
            cp ../apis/c/operator ${CMAKE_CURRENT_BINARY_DIR}/include/c -r
    )

    set(dora_link_dirs ${CMAKE_CURRENT_BINARY_DIR}/dora/src/Dora/target/debug)
    
    add_custom_target(Dora_c DEPENDS ${dora_c_include_dir})
    add_custom_target(Dora_cxx DEPENDS ${node_bridge} ${operator_bridge} ${dora_cxx_include_dir})
endif()


