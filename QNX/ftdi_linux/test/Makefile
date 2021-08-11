TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/Rules.make

APP = ftdi

all: $(APP)

$(APP): ftdi_spi_example.cpp	
	$(CC) ftdi_spi_example.cpp ftdi_spi.c -o $(APP) $(CFLAGS)
clean:
	rm -f *.o ; rm $(APP)

# Флаги: https://linux.die.net/man/1/g++
# Для тестовых пусков отключил ряд ворнингов(-Wno-missing-field-initializers -Wno-narrowing -Wno-format) 
