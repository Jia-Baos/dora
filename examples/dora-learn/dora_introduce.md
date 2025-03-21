
# 常用功能简介

## Node in Dora

**D**ataflow-**O**riented **R**obotic **A**rchitecture (`dora-rs`) is a framework that makes creation of robotic applications fast and simple.

`dora-rs` implements a declarative dataflow paradigm where tasks are split between nodes isolated as individual processes. <font color="red">So that the nodes can work relatively independently, which can prevent the entire program from crashing due to the failure of a single node.</font>

Each node defines its inputs and outputs to connect with other nodes.

## Interface about C and C++

Dora does not provide a C++ API yet, but we can create adapters for either the C or Rust API. The `operator-rust-api` and `node-rust-api` folders implement an example operator and node based on dora's Rust API, using the `cxx` crate for bridging. The `operator-c-api` and `node-c-api` show how to create operators and nodes based on dora's C API. Both approaches work, so you can choose the API that fits your application better.

## dataflow.yml 中字段解析

1. `envs` 表示环境变量，通过下面的方式获取

```python
# 从环境变量中获取值，如何其不存在，则返回后面的默认值
model_path = os.getenv("MODEL", args.model)
bbox_format = os.getenv("FORMAT", "xyxy")
```

2. `args` 表示启动可执行文件提供的命令行参数

3. 对于下面这种情况，存在 `dynamic` 关键字， 需要手动启动节点

```
- id: plot
    build: pip install ../../node-hub/opencv-plot
    path: dynamic
    inputs:
      image: camera/image
````

## 构建特定包

指定构建特定包：在包含多个包的工作区（workspace）中，cargo build -p 允许你仅构建指定的包，而不是构建整个工作区中的所有包；提高构建效率：通过指定特定包进行构建，可以节省时间和资源，避免不必要的编译操作

```
cargo build -p dora-node-api-c
```

## 启动 example

通过下面的命令可以执行 编译、启动 过程

```
dora start --example cxx-dataflow
```

这里的 cxx-dataflow 定义在 Cargo.toml 中，因此如果你希望添加自己的 demo，最好在 Cargo.toml 中添加自己的工程，如下所示：

```
[[example]]
name = "orbbec_camera"
path = "examples/orbbec_camera/run.rs"
```
