# Dora CMake Dataflow Rerun Example

This example shows how to create dora operators and custom nodes in CMake build system and communicate between them.

See also [c++-example](https://github.com/dora-rs/dora/blob/main/examples/c%2B%2B-dataflow/README.md) for the implementation details of operator and node.

## Compile and Run

To try it out, you can use the [`run.rs`](./run.rs) binary. It performs all required build steps and then starts the dataflow. Use the following command to run it: `cargo run --example cmake-dataflow-rerun`.

## Out-of-tree compile

This example also can be run in a separate root directory.
```
cd <path-to-cmake-dataflow-rerun>
mkdir build
cd build && cmake ..
make install
cd ..
dora up
dora start dataflow.yml
```

## run the dataflow using uv

```
# build node-cpp-api
cargo run --example cmake-dataflow-rerun

# build node-rust-api
# add '"examples/cmake-dataflow-rerun/node-rust-api",' in Cargo.toml
cargo build -p node-rust-api

uv venv --seed -p 3.11

source .venv/bin/activate  # Linux/macOS
.venv\Scripts\activate     # Windows

uv pip install dora-rs-cli

dora build dataflow.yml --uv

dora run dataflow.yml --uv
```
