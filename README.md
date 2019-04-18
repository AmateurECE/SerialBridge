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
on the board. This is the recommended way to use the application. If you're
a developer or a masochist (what's the difference?), read on for instructions
on building from source.

The project requires a working Arm cross compile environment. On the current
version of Debian (as of this writing), this requires:

```
apt-get install gcc-arm-none-eabi libnewlib-arm-none-eabi
```

Of course, to program the image, you will likely also need OpenOCD or the
custom flashing application provided by Texas Instruments.

The project is completely self-contained, so as long as your environment is
set up to cross compile for Arm, the project will build without requiring any
dependencies.

# Connecting to a Downstream Port #

On Linux, the upstream port tends to appear as `/dev/ttyACM0`. On OSX, it may
appear as `tty.usbserial`. Both UARTs are configured to 115200-8-N-1. I have
successfully used both minicom and gnuscreen to connect to a getty session over
this application.
