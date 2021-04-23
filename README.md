# Serial Transceiver on the TivaBoard #

This project implements a bidirectional serial bridge on the
[TM4C123GXL TivaBoard](http://www.ti.com/tool/EK-TM4C123GXL)
using the [TivaWare Peripheral Driver Library](http://www.ti.com/tool/SW-TM4C).

The application, when loaded, transmits characters between `UART0` and `UART1`
on the board. This can be used, for example, to control a `getty` session
running on a Linux device from a terminal on any other machine over a USB
connection.

# Building the Project #

Each stable release contains a pre-built binary that's ready to be programmed
on the board, if you're lazy like me. Building from source requires the
`arm-none-eabi` toolchain. To install (e.g. on Debian):

```
apt-get install gcc-arm-none-eabi libnewlib-arm-none-eabi
```

OpenOCD or the Texas Instruments programming toolchain can be used to program
the device. With a distribution of OpenOCD configured with `--enable-ti-icdi`:

```
openocd -f board/ek-lm4f120xl.cfg \
    -c 'program SerialBridge.axf verify reset exit'
```

The project is completely self-contained, so as long as your environment is
set up to cross compile for ARM, the project will build without requiring any
dependencies.

# Connecting to a Downstream Port #

On Linux, the upstream port tends to appear as `/dev/ttyACM0`. On OSX, it may
appear as `tty.usbserial`. Both UARTs are configured to the baud rate specified
at build time with `CONFIG_UART_BAUDRATE`, or 115200 by default, with 8 data
bits, no parity, and 1 stop bit (8-N-1). Any serial terminal program such as
TeraTerm, minicom, or GNU screen can be used to connect to the device.
