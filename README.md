# Serial Transceiver on the TivaBoard

This project implements a bidirectional serial bridge on the
[TM4C123GXL TivaBoard](http://www.ti.com/tool/EK-TM4C123GXL)
using the [TivaWare Peripheral Driver Library](http://www.ti.com/tool/SW-TM4C).

The application, when loaded, transmits characters between `UART0` and `UART1`
on the board. This can be used, for example, to control a `getty` session
running on a Linux device from a terminal on any other machine over a USB
connection.

# Building the Project

Each stable release contains a pre-built binary that's ready to be programmed
on the board, if you're lazy like me. Building from source requires the
`arm-none-eabi` toolchain. To install (e.g. on Debian):

```
apt-get install gcc-arm-none-eabi libnewlib-arm-none-eabi
```

The Makefile provides a number of useful functionalities for building and
testing the application:

* `D=1`: when set, the build system generates a debug build, with optimization
  disabled and debug symbols linked in.
* `CONFIG_UART_ECHO=1`: when set, all characters received on UART0 (the debug
  port) are echoed back to the host. This is useful for debugging.
* `CONFIG_UART_BAUDRATE`: sets the baud rate used by the device. The default is
  115200, but baud rates up to 1.5 Mbaud are supported. It's possible to get
  faster performance, see the TODO section for improvements.

OpenOCD or the Texas Instruments programming toolchain can be used to program
the device. With a distribution of OpenOCD configured with `--enable-ti-icdi`:

```
openocd -f board/ek-lm4f120xl.cfg \
    -c 'program SerialBridge.axf verify reset exit'
```

The `program` target in the Makefile executes this command for convenience.

The project is completely self-contained, so as long as your environment is
set up to cross compile for ARM, the project will build without requiring any
dependencies.

# Supported Baud Rates

Baud rates up to 115200 are supported without issue. At the time of this
writing, 1.5 Mbaud is supported entirely for a build generated with `D=0`, but
performance is poor for debug builds.

# Connecting to a Downstream Port

On Linux, the upstream port tends to appear as `/dev/ttyACM0`. On OSX, it may
appear as `tty.usbserial`. Both UARTs are configured to the baud rate specified
at build time with `CONFIG_UART_BAUDRATE`, or 115200 by default, with 8 data
bits, no parity, and 1 stop bit (8-N-1). Any serial terminal program such as
TeraTerm, minicom, or GNU screen can be used to connect to the device.

# TODO

The following is a list of opportunities to further optimize the software and
get even better performance out of the application, possibly even supporting
higher baudrates (5 Mbaud, or perhaps even 10 Mbaud).

2. Rewrite the interrupt handlers to perform direct register operations, or
   even in assembly.
4. Come up with a DMA scheme using the tm4c's uDMA controller and implement it.
5. Ensure we're maximizing compiler optimizations--as of this writing, using
   `-Os` will not compile. Perhaps there are even linker optimizations?
