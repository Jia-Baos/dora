#!/usr/bin/bash

source /home/seer/Project-Rust/dora/.venv/bin/activate

# camera library path
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/3rdparty/hikang/lib/x86_64
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/3rdparty/orbbec/lib/x86_64
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/install/lib

# dora cli path
# export PATH=$PATH:/home/seer/Project-Rust/dora/target/release

# ./install/bin/camera_run

dora build dataflow.yml --uv
dora run dataflow.yml --uv

# dora up
# dora start dataflow.yml
