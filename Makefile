###############################################################################
# NAME:		    Makefile
#
# AUTHOR:	    Ethan D. Twardy
#
# DESCRIPTION:	    Makefile for the project.
#
# CREATED:	    04/13/2019
#
# LAST EDITED:	    04/23/2021
###

TOP:=$(PWD)

PROJECT:=SerialBridge
SRCS += src/$(PROJECT).c
SRCS += src/startup_gcc.c
SRCS += driverlib/uart.c
SRCS += driverlib/interrupt.c
SRCS += driverlib/cpu.c

OBJS=$(patsubst %.c,%.o,$(SRCS))

CONFIG_UART_BAUDRATE?=1500000
CONFIG_UART_ECHO?=0
D?=0

# Make variables understood by the makedefs file
PART=TM4C123GH6PM
SCATTERgcc_$(PROJECT)=src/$(PROJECT).ld
ENTRY_$(PROJECT)=ResetISR
CFLAGSgcc=-Wall -Wextra -Werror -DTARGET_IS_TM4C123_RB1 -DUART_BUFFERED \
	-DCONFIG_UART_BAUDRATE=$(CONFIG_UART_BAUDRATE) \
	-I $(TOP)/include/ -I ./
ifeq (1,$(CONFIG_UART_ECHO))
	CFLAGSgcc += -DCONFIG_UART_ECHO
endif

ifeq (1,$(D))
	CFLAGSgcc += -g -O0
else
	CFLAGSgcc += -O3
endif

include $(TOP)/makedefs

# Rules
all: $(PROJECT).axf

program: $(PROJECT).axf
	openocd -f board/ek-lm4f120xl.cfg \
		-c "program $(PROJECT).axf verify reset exit"

$(PROJECT).axf: $(OBJS) src/$(PROJECT).ld

clean:
	rm -rf ./**/*.o
	rm -rf ./**/*.d
	rm -rf $(PROJECT).axf
	rm -rf $(PROJECT).bin

ifneq (${MAKECMDGOALS},clean)
-include ${wildcard src/*.d} ${wildcard driverlib/*.d} __dummy__
endif

###############################################################################
