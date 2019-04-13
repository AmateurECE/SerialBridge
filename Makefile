###############################################################################
# NAME:		    Makefile
#
# AUTHOR:	    Ethan D. Twardy
#
# DESCRIPTION:	    Makefile for the project.
#
# CREATED:	    04/13/2019
#
# LAST EDITED:	    04/13/2019
###

TOP:=$(PWD)

PROJECT:=SerialBridge
SRCS += src/$(PROJECT).c
SRCS += src/startup_gcc.c
SRCS += driverlib/uart.c

OBJS=$(patsubst %.c,%.o,$(SRCS))

# Make variables understood by the makedefs file
PART=TM4C123GH6PM
SCATTERgcc_$(PROJECT)=src/$(PROJECT).ld
ENTRY_$(PROJECT)=ResetISR
CFLAGSgcc=-DTARGET_IS_TM4C123_RB1 -DUART_BUFFERED -I $(TOP)/include/ 
# VERBOSE=1

include $(TOP)/makedefs

# Rules
all: force $(PROJECT).axf

$(PROJECT).axf: force $(OBJS) src/$(PROJECT).ld

$(OBJS): force

force:

clean:
	rm -rf src/*.o
	rm -rf src/*.d
	rm -rf $(PROJECT).axf
	rm -rf $(PROJECT).bin
	rm -rf `find . | grep \~`

ifneq (${MAKECMDGOALS},clean)
-include ${wildcard src/*.d} ${wildcard driverlib/*.d} __dummy__
endif

###############################################################################
