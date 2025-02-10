# Dora C++ Dataflow Example

This example shows how to create dora operators and custom nodes with C++.

Dora does not provide a C++ API yet, but we can create adapters for either the C or Rust API. The `operator-rust-api` and `node-rust-api` folders implement an example operator and node based on dora's Rust API, using the `cxx` crate for bridging. The `operator-c-api` and `node-c-api` show how to create operators and nodes based on dora's C API. Both approaches work, so you can choose the API that fits your application better.

## Compile and Run

To try it out, you can use the [`run.rs`](./run.rs) binary. It performs all required build steps and then starts the dataflow. Use the following command to run it: `cargo run --example cxx-dataflow`.

For a manual build, follow these steps:

- Create a `build` folder in this directory (i.e., next to the `node.c` file)
- Build the `cxx-dataflow-example-node-rust-api` and `cxx-dataflow-example-operator-rust-api` crates:
  ```
  cargo build -p cxx-dataflow-example-node-rust-api --release
  cargo build -p cxx-dataflow-example-operator-rust-api --release
  ```
- Compile the `dora-node-api-c` crate into a static library.
  - Run `cargo build -p dora-node-api-c --release`
  - The resulting staticlib is then available under `../../target/release/libdora-node-api-c.a`.
- Compile the `node-c-api/main.cc` (e.g. using `clang++`) and link the staticlib
  - For example, use the following command:
    ```
    clang++ node-c-api/main.cc <FLAGS> -std=c++14 -ldora_node_api_c -L ../../target/release --output build/node_c_api
    ```
  - The `<FLAGS>` depend on the operating system and the libraries that the C node uses. The following flags are required for each OS:
    - Linux: `-lm -lrt -ldl -pthread`
    - macOS: `-framework CoreServices -framework Security -l System -l resolv -l pthread -l c -l m`
    - Windows:
      ```
      -ladvapi32 -luserenv -lkernel32 -lws2_32 -lbcrypt -lncrypt -lschannel -lntdll -liphlpapi
      -lcfgmgr32 -lcredui -lcrypt32 -lcryptnet -lfwpuclnt -lgdi32 -lmsimg32 -lmswsock -lole32
      -lopengl32 -lsecur32 -lshell32 -lsynchronization -luser32 -lwinspool
      -Wl,-nodefaultlib:libcmt -D_DLL -lmsvcrt
      ```
      Also: On Windows, the output file should have an `.exe` extension: `--output build/c_node.exe`
- Compile the `operator-c-api/operator.cc` file into a shared library.
  - For example, use the following commands:
    ```
    clang++ -c operator-c-api/operator.cc -std=c++14 -o build/operator_c_api.o -fPIC
    clang++ -shared build/operator_c_api.o -o build/liboperator_c_api.so
    ```
    Omit the `-fPIC` argument on Windows. Replace the `liboperator_c_api.so` name with the shared library standard library prefix/extensions used on your OS, e.g. `.dll` on Windows.

**Build the dora coordinator and runtime:**

- Build the `dora-coordinator` executable using `cargo build -p dora-coordinator --release`
- Build the `dora-runtime` executable using `cargo build -p dora-runtime --release`

**Run the dataflow:**

- Start the `dora-coordinator`, passing the paths to the dataflow file and the `dora-runtime` as arguments:

  ```
  ../../target/release/dora-daemon --run-dataflow dataflow.yml ../../target/release/dora-runtime
  ```

## 简介

This example shows how to create dora operators and custom nodes with C++.

Dora does not provide a C++ API yet, but we can create adapters for either the C or Rust API. The `operator-rust-api` and `node-rust-api` folders implement an example operator and node based on dora's Rust API, using the `cxx` crate for bridging. The `operator-c-api` and `node-c-api` show how to create operators and nodes based on dora's C API. Both approaches work, so you can choose the API that fits your application better.

值得注意的是使用 `cxx` crate 作为桥接的 C++ 接口与原始的 C 接口是有较大差异的，使用时要格外注意

## c++-dataflow-test 编译、启动过程指令

```
mkdir build


// build dora-node-api-cxx
cargo build -p dora-node-api-cxx --release

cp ../../target/cxxbridge/dora-node-api-cxx/src/lib.rs.cc ./build/node-bridge.cc

cp ../../target/cxxbridge/dora-node-api-cxx/src/lib.rs.h ./build/dora-node-api.h

将 #include "../operator-rust-api/operator.h" 写入到 ./build/operator.h


// build dora-operator-api-cxx
cargo build -p dora-operator-api-cxx --release

cp ../../target/cxxbridge/dora-operator-api-cxx/src/lib.rs.cc ./build/operator-bridge.cc

cp ../../target/cxxbridge/dora-operator-api-cxx/src/lib.rs.h ./build/dora-operator-api.h


// build dora-node-api-c
cargo build -p dora-node-api-c --release


// build dora-operator-api-c
cargo build -p dora-operator-api-c --release


// build cxx node
clang++ ./node-rust-api/main.cc ./build/node-bridge.cc -std=c++17 -lm -lrt -ldl -pthread -ldora_node_api_cxx
 -L ../../target/release --output build/node_rust_api

clang++ ./node-c-api/main.cc -std=c++17 -lm -lrt -ldl -pthread -ldora_node_api_c -L ../../target/release --o
utput build/node_c_api


// build cxx operator
clang++ -c operator-rust-api/operator.cc -std=c++17 -o ./operator-rust-api/operator.o -fPIC

clang++ -c ./build/operator-bridge.cc -std=c++17 -o ./build/operator-bridge.o -fPIC

clang++ -shared operator-rust-api/operator.o build/operator-bridge.o -ldora_operator_api_cxx -L ../../target
/release -o build/liboperator_rust_api.so

clang++ -c operator-c-api/operator.cc -std=c++17 -o operator-c-api/operator.o -fPIC

clang++ -shared operator-c-api/operator.o -ldora_operator_api_c -L ../../target/release -o build/liboperator
_c_api.so


dora start dataflow.yml --name test // 运行使用

```