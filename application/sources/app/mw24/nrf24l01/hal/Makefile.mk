CFLAGS	+= -I./sources/app/mw24/nrf24l01/hal
CPPFLAGS	+= -I./sources/app/mw24/nrf24l01/hal

VPATH += sources/app/mw24/nrf24l01/hal

SOURCES += sources/app/mw24/nrf24l01/hal/hal_nrf_hw.c
SOURCES += sources/app/mw24/nrf24l01/hal/hal_nrf_l01.c