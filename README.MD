# Intel Simics fuzzing backend

This repository contains an example of how to transfer information from ASAN to
Intel Simics. In theory this method allows one to compile any binary with ASAN
instrumentation and intercept any error message the sanitizer prints directly in
Simics.

Linux and bare-metal targets should both be compatible with this approach,
although this demo only shows how to do it with Linux running on a RISC-V 64-bit
target.

For more information, see
[docs/dokumentacja.pdf](https://github.com/RoyalDonkey/put-intel-fuzzing/blob/master/docs/dokumentacja.pdf).

## Requirements

- RISC-V 64 toolchain (`riscv64-unknown-linux-gnu-*` utilities)
- Intel Simics installed at `SIMICS_BASE` (tested on simics-7.21.0)
- buildroot
- make
- dtc

## Running the demo

```sh
# 1. Build the "bad" binary, statically instrumented with ASAN
#    and injected with libsan-overlay:
make

# 2. Build the linux simics_shm kernel module and the image system.
#
# For this, use linux-driver/buildroot_external as BR2_EXTERNAL.
make BR2_EXTERNAL=<external> simics_riscv_defconfig
make BR2_EXTERNAL=<external> menuconfig  # enable BR2_PACKAGE_SIMICS_SHM
mkdir -p overlay/root/
cp bad overlay/root/
make BR2_ROOTFS_OVERLAY=overlay -j $(nproc)

# 3. Compile Linux device tree.
dtc -O dtb -o risc-v-simple.dtb simics-config/risc-v-simple.dts

# 4. Run the system in SIMICS. Your steps may vary here. See simics-config/README

# 5. Inside the emulated system:
modprobe simics_shm
./bad

# 6. You should see sanitizer output, both in the Linux console,
#    and in the Simics command line. If so, congratulations!
```
