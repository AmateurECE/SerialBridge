/******************************************************************************
 * NAME:	    SerialBridge.c
 *
 * AUTHOR:	    Ethan D. Twardy
 *
 * DESCRIPTION:	    The module containing the main method for the project.
 *
 * CREATED:	    04/13/2019
 *
 * LAST EDITED:	    04/22/2021
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

#ifndef CONFIG_UART_BAUDRATE
#define CONFIG_UART_BAUDRATE 115200
#endif

#define PIOSC_CLOCK_FREQ 16000000L

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
  uint32_t intMask;

} uart_t;

/******************************************************************************
 * FUNCTIONS
 ***/

/******************************************************************************
 * FUNCTION:        GenericUARTIntHandler
 *
 * DESCRIPTION:     Handle an interrupt from one of the UARTs. Copy chars from
 *                  the srcUart FIFO to the dstUart FIFO. If `echo' is set,
 *                  echo the characters back to the srcUart.
 *
 * ARGUMENTS:       srcUart: Base address of the UART to copy chars from
 *                  dstUart: Base address of the UART to copy chars to
 *                  echo: bool switch--echo chars back to src if true.
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

    UARTCharPutNonBlocking(dstUart, c);
    if (echo) { // Echo back to sender
      UARTCharPutNonBlocking(srcUart, c);
    }
  }
}

/******************************************************************************
 * FUNCTION:        UARTZeroHandler
 *
 * DESCRIPTION:     Handle interrupts from UART0.
 ***/
void UARTZeroHandler(void) {
#ifdef CONFIG_UART_ECHO
  GenericUARTIntHandler(UART0_BASE, UART1_BASE, true);
#else
  GenericUARTIntHandler(UART0_BASE, UART1_BASE, false);
#endif
}

/******************************************************************************
 * FUNCTION:        UARTOneHandler
 *
 * DESCRIPTION:     Handle interrupts from UART1.
 ***/
void UARTOneHandler(void) {
  GenericUARTIntHandler(UART1_BASE, UART0_BASE, false);
}

/******************************************************************************
 * FUNCTION:        ConfigureUART
 *
 * DESCRIPTION:     Configure a UART. All information about the UART is taken
 *                  from the uart_t structure passed in.
 *
 * ARGUMENTS:       uart: Configuration parameters and addresses of the UART.
 ***/
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

  // Run the clock from the PIOSC so the core can be run from the 25 MHz clock
  UARTClockSourceSet(uart->uartBase, UART_CLOCK_PIOSC);

  // Configure the UART's mode
  UARTConfigSetExpClk(uart->uartBase, PIOSC_CLOCK_FREQ, uart->baudRate,
		      uart->config);

  // Enable interrupts: Must be done before registering interrupt handler.
  UARTIntEnable(uart->uartBase, uart->intMask);

  // We *could* register the interrupt statically. But this allows the entire
  // application to be confined to only this source file.
  UARTIntRegister(uart->uartBase, uart->intHandler);
}

/******************************************************************************
 * MAIN
 ***/

int main()
{
  // TODO: Use lower clock rate if using lower baud rate
  // Set the clocking to run directly from the crystal.
  ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
                     | SYSCTL_XTAL_16MHZ);

  // Parameters for UART0
  uart_t uart0 = {
    .hostGpio = SYSCTL_PERIPH_GPIOA,
    .hostUart = SYSCTL_PERIPH_UART0,
    .rxPin = GPIO_PA0_U0RX,
    .txPin = GPIO_PA1_U0TX,
    .gpioBase = GPIO_PORTA_BASE,
    .gpioPins = GPIO_PIN_0 | GPIO_PIN_1,
    .uartBase = UART0_BASE,
    .baudRate = CONFIG_UART_BAUDRATE,
    .config = (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE
	       | UART_CONFIG_PAR_NONE),
    .intHandler = UARTZeroHandler,
    .intMask = (UART_INT_RX | UART_INT_RT),
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
    .baudRate = CONFIG_UART_BAUDRATE,
    .config = (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE
	       | UART_CONFIG_PAR_NONE),
    .intHandler = UARTOneHandler,
    .intMask = (UART_INT_RX | UART_INT_RT),
  };

  // Global enable interrupts: Must be done before configuring UART interrupts
  IntMasterEnable();

  ConfigureUART(&uart0);
  ConfigureUART(&uart1);

  // Sleep until an interrupt occurs
  while (1) {
    // TODO: Use SysCtlDeepSleep instead
    asm volatile ("wfi");
  }
}

/*****************************************************************************/
