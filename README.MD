# Intel Simics fuzzing backend

## Requirements

- make
- GCC (clang should also work, but mind that the sanitizer **must not be
    statically linked**)
- Intel Simics installed at `SIMICS_BASE` (tested on simics-7.21.0)

## Run demo

```sh
# Build the "bad" instrumented program and libsan-overlay.so
make

# Run "bad" with the overlay
./run.sh
```
