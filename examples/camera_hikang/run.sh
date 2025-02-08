#!/usr/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/seer/Project-Rust/dora/examples/camera_hikang/SDK/lib/x86_64

export PATH=$PATH:/home/seer/Project-Rust/dora/target/release

dora up

dora start dataflow.yml --name test