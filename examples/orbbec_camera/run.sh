#!/usr/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/seer/Project-Rust/dora/examples/orbbec_camera/SDK1.10.18/lib/x86_64

export PATH=$PATH:/home/seer/Project-Rust/dora/target/release

dora up

dora start dataflow.yml --name test