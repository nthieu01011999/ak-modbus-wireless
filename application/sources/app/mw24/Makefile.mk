include sources/app/mw24/eeprom/Makefile.mk
include sources/app/mw24/io_config/Makefile.mk
include sources/app/mw24/nrf24l01/Makefile.mk
include sources/app/mw24/nwk/Makefile.mk
include sources/app/mw24/timer/Makefile.mk

CFLAGS		+= -I./sources/app/mw24
CPPFLAGS	+= -I./sources/app/mw24

VPATH += sources/app/mw24