# Intel Simics fuzzing backend

This repository contains an example of how to transfer information from ASAN to
Intel Simics. In theory this method allows one to compile any binary with ASAN
instrumentation and intercept any error message the sanitizer prints directly in
Simics.

Linux and bare-metal targets should both be compatible with this approach,
although this demo only shows how to do it with Linux running on a RISC-V 64-bit
target.

## Requirements

- RISC-V 64 toolchain (`riscv64-unknown-linux-gnu-*` utilities)
- Intel Simics installed at `SIMICS_BASE` (tested on simics-7.21.0)
- buildroot
- make
- dtc

## Running the demo

TODO
