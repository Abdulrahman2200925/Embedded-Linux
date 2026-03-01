# Cross compiler prefix
CROSS_COMPILE = aarch64-linux-gnu-
CC      = $(CROSS_COMPILE)gcc
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

# Flags
CFLAGS  = -ffreestanding -nostdlib -nostartfiles
TARGET  = blinky

# Build final binary
all: $(TARGET).img

# Compile C file
main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

# Assemble startup file
startup.o: startup.s
	$(CC) $(CFLAGS) -c startup.s -o startup.o

# Link everything
$(TARGET).elf: startup.o main.o
	$(LD) -T linker.ld startup.o main.o -o $(TARGET).elf

# Convert to raw binary
$(TARGET).img: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).img

# Clean
clean:
	rm -f *.o *.elf *.img