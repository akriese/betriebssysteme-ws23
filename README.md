# betriebssysteme-ws23

We are building an operating system accompanying a uni lecture.

## Setup

You need to have the `gcc-arm-none-eabi` toolchain installed for `make` to work.

Following packages might have to be installed additionally:
`sudo apt install -y libglib2.0-dev libpixman-1-dev gcc-arm-none-eabi`

## Usage

Whatever code should be executed on the system, should be called in the `start.c` file
before the infinite loop.

- Execute `make all` to compile the kernel.
- Execute `make run` to execute it with `qemu`.
- Execute `make dump` to view the kernel dump.

## Serial interface

Using the `-nographic` flag for `qemu` by default, the debug serial interface is
forwarded by `qemu` to the executing console. Thus, you can send characters to the
virtual machine by typing on your keyboard. The console will print whatever is sent by
virtual machine.
