/******************************************************************************
 * NAME:	    SerialBridge.c
 *
 * AUTHOR:	    Ethan D. Twardy
 *
 * DESCRIPTION:	    The module containing the main method for the project.
 *
 * CREATED:	    04/13/2019
 *
 * LAST EDITED:	    04/13/2019
 ***/

/******************************************************************************
 * PREAMBLE
 ***/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

/******************************************************************************
 * MAIN
 ***/

int main() {
  // NOTE: This works!


  //
  // Set the clocking to run directly from the crystal.
  //
  ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
		     SYSCTL_OSC_MAIN);

  //
  // Enable the GPIO Peripheral used by the UART.
  //
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

  //
  // Enable UART0
  //
  ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

  //
  // Configure GPIO Pins for UART mode.
  //
  ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
  ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
  ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

  UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 115200,
		      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE
		       | UART_CONFIG_PAR_NONE));

  // Put an infinite number of 'x's into the receive buffer
  while (1) {
    UARTCharPut(UART0_BASE, 'x');
  }
}

/*****************************************************************************/
