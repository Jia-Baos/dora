# C++ 数据流示例

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