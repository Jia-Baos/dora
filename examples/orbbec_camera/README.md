# Dora CMake Dataflow Example

This example shows how to create dora operators and custom nodes in CMake build system.

See also [c++-example](https://github.com/dora-rs/dora/blob/main/examples/c%2B%2B-dataflow/README.md) for the implementation details of operator and node.

## Compile and Run

To try it out, you can use the [`run.rs`](./run.rs) binary. It performs all required build steps and then starts the dataflow. Use the following command to run it: `cargo run --example cmake-dataflow`.

## Out-of-tree compile

This example also can be ran in a separate root directory.
```
cd <path-to-cmake-dataflow>
mkdir build
cd build && cmake ..
make install
cd ..
dora up
dora start dataflow.yml
```

为用户添加打开 usb 设备的权限，设置后需要重启电脑

```
sudo usermod -a -G dialout username
```

我们已经使得代码顺利编译通过，但是通过 dora 启动的过程中，程序似乎无法正确打开 usb 设备，这这有待进一步探索