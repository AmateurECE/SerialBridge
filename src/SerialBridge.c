/******************************************************************************
 * NAME:	    SerialBridge.c
 *
 * AUTHOR:	    Ethan D. Twardy
 *
 * DESCRIPTION:	    The module containing the main method for the project.
 *
 * CREATED:	    04/13/2019
 *
 * LAST EDITED:	    04/15/2019
 ***/

/******************************************************************************
 * PREAMBLE
 ***/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"

typedef struct {

  uint32_t hostGpio;
  uint32_t hostUart;
  uint32_t rxPin;
  uint32_t txPin;
  uint32_t gpioBase;
  uint32_t gpioPins;
  uint32_t uartBase;
  uint32_t baudRate;
  uint32_t config;
  void (*intHandler)(void);

} uart_t;

/******************************************************************************
 * FUNCTIONS
 ***/

void GenericUARTIntHandler(uint32_t srcUart, uint32_t dstUart, bool echo)
{
  // Clear interrupt status
  UARTIntClear(srcUart, UARTIntStatus(srcUart, true));

  // Copy data from srcUart to destUart
  int32_t c = 0;
  while (UARTCharsAvail(srcUart)) {
    // Peripheral Driver Library Documentation, Section 30.2.2.8, Return:
    //   "The UARTCharsAvail() function should be called before attempting to
    //    call this function."
    if (-1 == (c = UARTCharGetNonBlocking(srcUart))) {
      return;
    }

    // TODO: GenericUARTIntHandler does not check success of write
    UARTCharPutNonBlocking(dstUart, c);
  }
  // TODO: Echo back to sender
}

void UARTZeroHandler(void) {
  GenericUARTIntHandler(UART0_BASE, UART1_BASE, true);
  // TODO: Debug flash Red LED
}

void UARTOneHandler(void) {
  GenericUARTIntHandler(UART1_BASE, UART0_BASE, false);
  // TODO: Debug flash Blue LED
}

static void ConfigureUART(uart_t* uart)
{
  // Enable the GPIO Peripheral used by the UART.
  ROM_SysCtlPeripheralEnable(uart->hostGpio);

  // Enable UART0
  ROM_SysCtlPeripheralEnable(uart->hostUart);

  // Configure GPIO Pins for UART mode.
  ROM_GPIOPinConfigure(uart->rxPin);
  ROM_GPIOPinConfigure(uart->txPin);
  ROM_GPIOPinTypeUART(uart->gpioBase, uart->gpioPins);

  // Configure the UART's mode
  UARTConfigSetExpClk(uart->uartBase, ROM_SysCtlClockGet(), uart->baudRate,
		      uart->config);

  // Enable interrupts
  // We could register the interrupt statically. But this is only done once,
  // at program startup, and it allows the entire application to be confined
  // to only this source file.
  UARTIntRegister(uart->uartBase, uart->intHandler);

  // TODO: Does not configure interrupts to trigger on error.
  UARTIntEnable(uart->uartBase, UART_INT_RX);

  // Configure RX int to trigger when the FIFO has seven bytes in it. Send 0
  // as ui32TxLevel because we do not trigger TX int.
  UARTFIFOLevelSet(uart->uartBase, 0, UART_FIFO_RX7_8);
}

/******************************************************************************
 * MAIN
 ***/

int main()
{
  // Set the clocking to run directly from the crystal.
  ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
		     | SYSCTL_XTAL_25MHZ);

  // Parameters for UART0
  uart_t uart0 = {
    .hostGpio = SYSCTL_PERIPH_GPIOA,
    .hostUart = SYSCTL_PERIPH_UART0,
    .rxPin = GPIO_PA0_U0RX,
    .txPin = GPIO_PA1_U0TX,
    .gpioBase = GPIO_PORTA_BASE,
    .gpioPins = GPIO_PIN_0 | GPIO_PIN_1,
    .uartBase = UART0_BASE,
    .baudRate = 115200,
    .config = (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE
	       | UART_CONFIG_PAR_NONE),
    .intHandler = UARTZeroHandler,
  };

  // Parameters for UART1
  uart_t uart1 = {
    .hostGpio = SYSCTL_PERIPH_GPIOB,
    .hostUart = SYSCTL_PERIPH_UART1,
    .rxPin = GPIO_PB0_U1RX,
    .txPin = GPIO_PB1_U1TX,
    .gpioBase = GPIO_PORTB_BASE,
    .gpioPins = GPIO_PIN_0 | GPIO_PIN_1,
    .uartBase = UART1_BASE,
    .baudRate = 115200,
    .config = (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE
	       | UART_CONFIG_PAR_NONE),
    .intHandler = UARTOneHandler,
  };

  ConfigureUART(&uart0);
  ConfigureUART(&uart1);

  // Global enable interrupts
  IntMasterEnable();

  /* asm volatile ("wfi"); */
  while (1);
}

/*****************************************************************************/
