# Task 3: Bare-Metal Programming — Loading and Running a Custom Binary from U-Boot

> **Builds on Task 1 + Task 2** — uses the virtual SD card from Task 1 and U-Boot from Task 2 to load and execute a bare-metal AArch64 application on real Raspberry Pi 3B+ hardware.

---

## What This Task Does

Write a complete bare-metal AArch64 application from scratch that blinks an LED on GPIO 26, compile and link it correctly, deploy it to the RPi3 boot partition, load it via U-Boot, and execute it.

```
startup.s + main.c → compile → link (linker.ld) → blinky.img
                                                        ↓
                                              SD card /boot partition
                                                        ↓
                                              U-Boot: fatload → go
                                                        ↓
                                                  LED blinks on GPIO 26
```

---

## Project Structure

```
blinky/
├── startup.s     ← AArch64 assembly entry point
├── main.c        ← GPIO control in C
├── linker.ld     ← custom linker script
└── Makefile      ← automated build
```

---

## Source Files

### startup.s

```asm
.section .text.boot
.global _start

_start:
    ldr x30, =0x10000000
    mov sp, x30
    bl main

hang:
    b hang
```

**Why startup.s is needed:**
- No OS, no C runtime (crt0) exists at this stage
- Stack pointer must be set manually before any C code runs
- Without it, first function call crashes — no stack exists
- Hang loop prevents CPU executing random memory if `main()` returns

### main.c

```c
#define GPIO_BASE     0x3F200000

#define GPFSEL2  (*(volatile unsigned int *)(GPIO_BASE + 0x08))
#define GPSET0   (*(volatile unsigned int *)(GPIO_BASE + 0x1C))
#define GPCLR0   (*(volatile unsigned int *)(GPIO_BASE + 0x28))

void delay(unsigned long count)
{
    volatile unsigned long i;
    for(i = 0; i < count; i++);
}

void gpio_init(void)
{
    GPFSEL2 &= ~(7 << 18);   // clear bits 18,19,20 (pin 26)
    GPFSEL2 |=  (1 << 18);   // set 001 = output
}

void main(void)
{
    gpio_init();

    while(1)
    {
        GPSET0 = (1 << 26);    // LED ON
        delay(50000000);
        GPCLR0 = (1 << 26);    // LED OFF
        delay(50000000);
    }
}
```

**GPIO Register Map (BCM2837):**

| Register | Address | Purpose |
|----------|---------|---------|
| GPFSEL2 | 0x3F200008 | Function select pins 20-29 |
| GPSET0 | 0x3F20001C | Set pin HIGH (pins 0-31) |
| GPCLR0 | 0x3F200028 | Set pin LOW (pins 0-31) |

GPIO 26 → GPFSEL2 bits 18,19,20 → set `001` = output

### linker.ld

```ld
ENTRY(_start)

SECTIONS
{
    . = 0x10000000;

    .text :
    {
        *(.text.boot)    /* startup.s MUST be first */
        *(.text)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }
}
```

**Why load address is `0x10000000`:**

From RPi3 `bdinfo`:
```
DRAM start:  0x00000000
reserved[0]: 0x0000 - 0x0FFF        ← skip
reserved[3]: 0x39F46FB0 - 0x3B3FFFFF ← U-Boot lives here
Free area:   0x00001000 - 0x39BFFFFF ← safe
```
`0x10000000` sits in the middle of free RAM — safe from reserved regions and U-Boot.

### Makefile

```makefile
CROSS_COMPILE = aarch64-linux-gnu-
CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS  = -ffreestanding -nostdlib -nostartfiles
TARGET  = blinky

all: $(TARGET).img

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

startup.o: startup.s
	$(CC) $(CFLAGS) -c startup.s -o startup.o

$(TARGET).elf: startup.o main.o
	$(LD) -T linker.ld startup.o main.o -o $(TARGET).elf

$(TARGET).img: $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).img

clean:
	rm -f *.o *.elf *.img
```

**Why `.img` not `.elf`:**
U-Boot `go` jumps directly to a raw address. ELF has headers before the code — jumping to ELF start executes headers as instructions → crash. `objcopy -O binary` strips everything leaving pure machine code starting at `_start`.

---

## Build

```bash
make
ls -lh blinky.img
```

---

## Deployment — Boot Partition Files

All files required in `/boot`:

| File | Source | Purpose |
|------|--------|---------|
| `bootcode.bin` | RPi firmware repo | BootROM loads this, initializes RAM |
| `start.elf` | RPi firmware repo | GPU firmware, loads u-boot.bin |
| `fixup.dat` | RPi firmware repo | GPU/CPU RAM split |
| `bcm2710-rpi-3-b-plus.dtb` | RPi firmware repo | Hardware description for U-Boot |
| `u-boot.bin` | Built from source | First ARM CPU code |
| `config.txt` | Manual | Tells start.elf to load u-boot.bin |
| `blinky.img` | Built here | Our bare-metal application |

### Get RPi3 Firmware
```bash
git clone --depth 1 https://github.com/raspberrypi/firmware.git
```

### Build U-Boot for RPi3
```bash
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- rpi_3_defconfig
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc)
```

### config.txt
```
kernel=u-boot.bin
enable_uart=1
arm_64bit=1
```

---

## Development Workflow — TFTP (No SD card touching)

### PC Side
```bash
# Assign IP to ethernet interface connected to RPi3
sudo ip addr add 192.168.0.1/24 dev enp1s0f0

# TFTP config (/etc/default/tftpd-hpa)
TFTP_ADDRESS="0.0.0.0:69"   # listen on all interfaces

sudo systemctl restart tftpd-hpa

# Copy binary
sudo cp blinky.img /srv/tftp/
```

### U-Boot Side
```bash
setenv ipaddr 192.168.0.2
setenv serverip 192.168.0.1
saveenv
ping 192.168.0.1

tftp 0x10000000 blinky.img
go 0x10000000
```

### Iterate Without SD Card
```bash
# Change code → rebuild → redeploy instantly
make clean && make
sudo cp blinky.img /srv/tftp/

# In U-Boot
tftp 0x10000000 blinky.img
go 0x10000000
```

---

## Hardware Wiring

```
RPi3 GPIO Header:
Pin 1  (3.3V) ─────────────────────────────┐
                                            │
                                           [R 330Ω]
                                            │
Pin 37 (GPIO 26) ──────────────────────── [LED] ── Pin 39 (GND)
```

---

## Key Concepts

| Concept | Explanation |
|---------|-------------|
| `startup.s` needed | No OS to set up stack — must do it manually in assembly |
| Load address `0x10000000` | Free RAM on RPi3, away from reserved regions and U-Boot |
| `.img` not `.elf` | Raw binary — no headers, `go` jumps directly to `_start` |
| `volatile` keyword | Prevents compiler optimizing away hardware register writes |
| TFTP for development | Rebuild and reload without touching SD card |
| `0x3F000000` GPIO base | BCM2837 peripheral base — datasheet says `0x7E000000` but maps to `0x3F000000` on RPi3 |

---

*Lab: Embedded Linux — Task 3 | Bare-Metal AArch64 on Raspberry Pi 3B+*
