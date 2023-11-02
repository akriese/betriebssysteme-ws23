# betriebssysteme-ws23

We are building an operating system accompanying a uni lecture.

## Setup

You need to have the `gcc-arm-none-eabi` toolchain installed for `make` to work.

## Usage

Whatever code should be executed on the system, should be called in the `start.c` file
before the infinite loop.

- Execute `make all` to compile the kernel.
- Execute `make run` to execute it with `qemu`.

## Serial interface

If you wish to access the serial output of the kernel in `qemu` via `minicom`,
execute `minicom -D unix:/tmp/serial.socket` in another shell.
