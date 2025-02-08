# C 数据流示例

本示例展示了如何在 C 中创建和连接 dora 操作符和自定义节点。

## 概述

数据流图 定义了一个简单的数据流图，包含以下三个节点：

- node.c 是一个自定义节点，即它有自己的主函数，并作为一个独立的进程运行。它使用 dora-node-api-c crate 与 dora 数据流进行交互。
    - 该节点有一个名为 timer 的输入，它被映射到 dora 提供的周期性定时器 (dora/timer/secs/1)。
    - 每当节点接收到定时器的触发信号时，它会发送一条带有 ID tick 和一个计数值作为数据（只是一个字节）的消息。
    - 在接收到 10 个定时器输入后，节点退出。
    
- operator.c 文件定义了一个 dora 操作符，它作为一个共享库被插入到 dora 运行时中。它不是定义一个 main 函数，而是实现了 dora_* 函数模板，这些函数由 dora 运行时调用，例如当有新输入可用时。
    - 操作符将 node.c 节点创建的 tick 消息作为输入。对于每个输入值，它会检查 ID，然后将接收到的消息打印到 stdout。
    - 它会统计接收到的值，并输出一个格式为 "当前计数值是 ..." 的字符串。
    
- sink.c 文件定义了另一个自定义节点，它将操作符的输出字符串作为输入。它将每个接收到的输入打印到 stdout，并在输入流关闭时退出。

## c-dataflow-test 编译、启动过程指令

```
cargo build -p dora-node-api-c --release

clang node.c -lm -lrt -ldl -pthread -ldora_node_api_c -L ../../target/release --output build/c_node

clang sink.c -lm -lrt -ldl -pthread -ldora_node_api_c -L ../../target/release --output build/c_sink

cargo build -p dora-operator-api-c --release

clang -c operator.c -o build/operator.o -fdeclspec -fPIC

clang -shared build/operator.o -ldora_operator_api_c -L ../../target/release -o build/liboperator.so

dora start dataflow.yml --name test // 运行使用

```