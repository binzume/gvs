CC=avr-gcc
TARGET=atmega88
AVR_F_CPU=8000000
AVR_WRITER=hidspx
AVR_WRITER_OPT=-ph

# for atmega644p : make AVR_F_CPU=20000000 TARGET=atmega644p

all: da_test.hex

da_test.hex: da_test.c
	avr-gcc -mmcu=$(TARGET) -Wall -O2 -D F_CPU=$(AVR_F_CPU) -o da_test.elf da_test.c
	avr-objcopy -O ihex -R .eeprom da_test.elf $@
	rm da_test.elf

write: da_test.hex
	@echo writing $? ...
	$(AVR_WRITER) $(AVR_WRITER_OPT) $?

clean:
	rm *.elf *.hex

