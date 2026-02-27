# Task 2: Building and Customizing U-Boot for QEMU & Raspberry Pi 3B+

> **Builds on Task 1** — the virtual SD card partitions created in Task 1 are populated here with U-Boot and the files needed to boot Linux.

---

## Boot Chain — Raspberry Pi

```
BootROM → bootcode.bin → start.elf → u-boot.bin → kernel + DTB
  GPU        GPU           GPU           CPU           CPU
```

| Stage | Job |
|-------|-----|
| **BootROM** | Baked in SoC. Loads `bootcode.bin` into L2 cache |
| **bootcode.bin** | Initializes RAM, loads `start.elf` |
| **start.elf** | GPU firmware. Reads `config.txt` + `fixup.dat`, loads `u-boot.bin`, releases CPU |
| **fixup.dat** | Defines GPU/CPU RAM split |
| **u-boot.bin** | First ARM CPU code. Loads kernel + DTB |
| **kernel + .dtb** | Linux takes over |

> **Key insight:** The first 3 stages run on the **GPU**. The CPU is held in reset until `start.elf` releases it.

---

## Files Required in `/boot` Partition

| File | Purpose |
|------|---------|
| `bootcode.bin` | Loaded by BootROM, initializes RAM |
| `start.elf` | GPU firmware, orchestrates the boot |
| `config.txt` | Boot configuration read by `start.elf` |
| `fixup.dat` | GPU/CPU RAM split definition |
| `u-boot.bin` | First ARM CPU executable |
| `kernel.img` + `.dtb` | Linux kernel + hardware description |

---

## Part A — Build U-Boot

### For QEMU (Cortex-A9 / vexpress)

```bash
sudo apt install gcc-arm-linux-gnueabi

make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- vexpress_ca9x4_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- -j$(nproc)

# Run
qemu-system-arm -M vexpress-a9 -kernel u-boot \
    -dtb arch/arm/dts/vexpress-v2p-ca9.dtb \
    -nographic -m 512M \
    -net tap -net nic
```

### For Raspberry Pi 3B+ (AArch64)

```bash
sudo apt install gcc-aarch64-linux-gnu

# Note: ARCH=arm not arm64 — U-Boot handles 64-bit internally
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- rpi_3_defconfig
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- menuconfig
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- -j$(nproc)

# Deploy to SD card
sudo cp u-boot.bin /mnt/boot/
sudo cp bcm2710-rpi-3-b-plus.dtb /mnt/boot/
```

`config.txt`:
```
kernel=u-boot.bin
enable_uart=1
arm_64bit=1
```

### Build Comparison

| | QEMU vexpress-a9 | RPi 3B+ |
|--|-----------------|---------|
| ARCH | `arm` | `arm` |
| CROSS_COMPILE | `arm-linux-gnueabi-` | `aarch64-linux-gnu-` |
| defconfig | `vexpress_ca9x4_defconfig` | `rpi_3_defconfig` |
| Kernel format | zImage → `bootz` | Image → `booti` |

---

## Part B — U-Boot Commands

### Key Environment Variables

| Variable | Meaning |
|----------|---------|
| `kernel_addr_r` | Address to load kernel into RAM |
| `fdt_addr_r` | Address to load DTB into RAM |
| `bootargs` | Arguments passed to Linux kernel |
| `bootcmd` | Auto-runs after `bootdelay` seconds |
| `bootdelay` | Seconds before autoboot |

### Essential Commands

```bash
# Info
bdinfo                              # board info, RAM map, MAC address
printenv                            # all environment variables

# Memory
md 0x62000000                       # display memory at address
fatload mmc 0:1 0x62000000 zImage   # load file from FAT into RAM
fatwrite mmc 0:1 0x62000000 f.txt ${filesize}  # write RAM to FAT

# Network
setenv ipaddr 192.168.0.2
setenv serverip 192.168.0.1
ping 192.168.0.1
tftp 0x62000000 zImage              # download file into RAM

# Boot
setenv bootargs console=ttyAMA0 root=/dev/mmcblk0p2 rw
bootz ${kernel_addr_r} - ${fdt_addr_r}   # 32-bit
booti ${kernel_addr_r} - ${fdt_addr_r}   # 64-bit
```

### DRAM Memory Map (vexpress, 128MB)

```
0x60000000  ← DTB              (fdt_addr_r)
0x60100000  ← Kernel zImage    (kernel_addr_r)
0x67F5D000  ← U-Boot itself    (top of RAM — do not touch)
```

> Why not load kernel at `0x60000000`? DTB lives there. Loading kernel there overwrites DTB before kernel starts.

---

## Custom Banner via menuconfig

```
menuconfig → Console → Board specific string to be added to uboot version string
Value: "Welcome to Our-Boot - Intake 46"
```

---

## Custom Command (hello)

**`cmd/hello.c`:**
```c
#include <command.h>

static int do_hello(struct cmd_tbl *cmdtp, int flag,
                    int argc, char *const argv[])
{
    printf("Hello, My Name is Abdo\n");
    return 0;
}

U_BOOT_CMD(hello, 1, 1, do_hello, "print my name", "");
```

**`cmd/Makefile`** — add after `obj-y += boot.o`:
```makefile
obj-y += hello.o
```

---

## TFTP Network Boot Setup

### PC Side
```bash
sudo apt install tftpd-hpa
sudo mkdir -p /srv/tftp && sudo chmod 777 /srv/tftp
sudo systemctl restart tftpd-hpa

# Place files
cp zImage /srv/tftp/
cp board.dtb /srv/tftp/
```

### QEMU with TAP Network
```bash
# Run QEMU — let it create TAP automatically
sudo qemu-system-arm -M vexpress-a9 -kernel u-boot \
    -dtb arch/arm/dts/vexpress-v2p-ca9.dtb \
    -nographic -m 512M -net tap -net nic

# In another terminal — assign IP to the tap QEMU created
sudo ip addr add 192.168.0.1/24 dev tap1
```

### U-Boot Side
```bash
setenv ipaddr 192.168.0.2
setenv serverip 192.168.0.1
ping 192.168.0.1

tftp ${kernel_addr_r} zImage
tftp ${fdt_addr_r} board.dtb
setenv bootargs console=ttyAMA0 root=/dev/mmcblk0p2 rw
bootz ${kernel_addr_r} - ${fdt_addr_r}
```

---

## Key Concepts

| Concept | One Line |
|---------|----------|
| DTB | Hardware description map — not code. Kernel reads it to init drivers |
| bootargs | U-Boot sets it, Linux kernel reads it to find rootfs and console |
| TAP network | Virtual wire between PC and QEMU — full two-way communication |
| `run` command | Executes U-Boot commands stored in an env variable |
| `go` command | Jumps to address and executes as bare-metal (no OS) |
| Loop device `-P` flag | Makes kernel read MBR and create partition sub-devices |

---

*Lab: Embedded Linux — Task 2 | U-Boot Build and Deployment*
