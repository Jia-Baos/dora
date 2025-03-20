# 使用文档

## cargo

```
// 查看cargo安装的库
cargo install --list
```

## dora

```
# 启动 coordinator daemon
dora up

# 关闭
dora stop

# 启动数据流
# --name：命名数据流
dora start dataflow.yml --name first-dataflow

# 关闭数据流
dora stop --name first-dataflow

# 查看节点输出
dora logs first-dataflow op_1
dora logs first-dataflow op_2
dora logs first-dataflow custom-node_1
```
