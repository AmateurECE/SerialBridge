# Serial Transceiver on the TivaBoard #

This project implements a bidirectional serial bridge on the
[TM4C123GXL TivaBoard](http://www.ti.com/tool/EK-TM4C123GXL)
using the [TivaWare Peripheral Driver Library](http://www.ti.com/tool/SW-TM4C).

The application, when loaded, transmits characters between `UART0` and `UART1`
on the board. This can be used, for example, to control a `getty` session
running on a Linux device from a terminal on any other machine over a USB
connection.
