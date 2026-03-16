# Lab 7 — Create Your Own Initramfs
### No More Kernel Panic · Intake 46 · March 2026
### Platform: Raspberry Pi 3B+ (AArch64) · Kernel 6.12.75-v8+ · BusyBox 1.36.1

---

## What This Lab Is About

In Lab 6 we compiled and booted a custom Linux kernel. It booted perfectly — but ended with this:

```
Kernel panic - not syncing: VFS: Unable to mount root fs on unknown-block(0,0)
```

The kernel had nothing to run. No rootfs, no init process, no shell. This lab fixes that forever by building an **initramfs** — a complete minimal filesystem packed into a single archive that the kernel unpacks into RAM and runs immediately.

**End result:** The moment the board powers on, you see a real shell prompt on the TTL serial cable with no HDMI required:

```
Welcome to Our Embedded Linux - Intake 46
Please press Enter to activate this console.
~ # ls
bin  dev  etc  init  proc  sbin  sys
~ # echo "Hello from Eng.Fady - Intake 46"
Hello from Eng.Fady - Intake 46
~ #
```

---

## Understanding Check — Questions and Answers

### Q1: What is initramfs? Why use it instead of mounting the real rootfs directly?

**initramfs** (initial RAM filesystem) is a gzip-compressed cpio archive that U-Boot loads into RAM alongside the kernel. The kernel decompresses it into a `tmpfs` (a filesystem that lives entirely in RAM) and runs `/init` from it as the first userspace process.

**Why use it instead of mounting rootfs directly?**

The kernel is a minimal core — it cannot always mount the real rootfs directly because:

- The driver needed to read the storage device might be a kernel **module** (`.ko` file) that lives *on* that storage device — a deadlock
- The disk might be **encrypted** (LUKS) — needs a passphrase before mounting
- The rootfs might be on **RAID or LVM** — needs assembly before it is visible
- The rootfs might be on **NFS** — needs the network stack configured first

initramfs solves this by giving the kernel a filesystem it can always access (it is already in RAM — no driver needed), where it can load modules, decrypt disks, configure network, and then mount the real rootfs.

In our lab specifically: we use initramfs because the kernel has nothing to run after booting. initramfs gives it BusyBox as an init process and a working shell.

```
Without initramfs:                    With initramfs:
─────────────────                     ────────────────
Kernel boots ✓                        Kernel boots ✓
Looks for /sbin/init → not found      Unpacks cpio.gz into RAM ✓
Looks for /etc/init  → not found      Finds /init → runs BusyBox ✓
Looks for /bin/init  → not found      Mounts proc/sys/dev ✓
Looks for /bin/sh    → not found      Shell prompt appears ✓
KERNEL PANIC ✗                        ~ # _  ✓
```

---

### Q2: Why cpio format? Why not tar or zip?

The Linux kernel has a **built-in cpio extractor** compiled directly into the kernel source (`init/initramfs.c`). This extractor understands only the `newc` (new portable) cpio format.

| Format | Kernel understands? | Why |
|--------|--------------------:|-----|
| `cpio -H newc` | ✓ Yes | Built-in extractor since Linux 2.6 |
| `tar` | ✗ No | Kernel has no tar parser |
| `zip` | ✗ No | Kernel has no zip decompressor |
| `cpio -H odc` | ✗ No | Only newc format is supported |

The `-H newc` flag is mandatory:
```bash
find . | cpio -H newc -o | gzip > rootramfs.cpio.gz
#              ^^^^
#              must be newc — kernel rejects everything else
```

---

### Q3: What does rdinit= do? What happens if the wrong path is given?

`rdinit=` is a kernel bootarg that overrides the default `/init` search path for initramfs.

```
Default (no rdinit=):   kernel looks for /init
rdinit=/sbin/init:      kernel looks for /sbin/init instead
rdinit=/bin/sh:         kernel runs shell directly as PID 1
```

**What happens if wrong path:**

```
rdinit=/wrong/path → kernel cannot find it
                   → falls through to /sbin/init, /etc/init, /bin/init, /bin/sh
                   → if all fail → Kernel Panic: No working init found
```

**In our lab:** We do not use `rdinit=` at all — the kernel finds `/init` automatically since we created it at the root of the initramfs. This is the cleanest approach.

---

### Q4: Why must init be statically linked? What if dynamic?

When the kernel runs `/init`, only the initramfs has been unpacked into RAM. The `/lib` directory is completely empty — no shared libraries exist yet.

**Dynamic binary failure sequence:**
```
Kernel executes /init (dynamic binary)
    ↓
ELF loader maps binary into memory
    ↓
Dynamic linker (ld.so) tries to load libc.so, libm.so...
    ↓
Searches /lib → EMPTY → exits with error
    ↓
Process never actually starts
    ↓
Kernel tries /sbin/init, /etc/init, /bin/init, /bin/sh → all fail
    ↓
Kernel Panic: No working init found
```

**Static binary works because:**
```
Kernel executes /init (static binary)
    ↓
All library code is compiled INSIDE the binary
    ↓
No /lib lookup needed
    ↓
Process starts immediately ✓
```

**Verify static linking:**
```bash
file _install/bin/busybox
# Must show: statically linked ✓
# If shows: dynamically linked → rebuild with CONFIG_STATIC=y
```

---

### Q5: Difference between initramfs and initrd?

| | initramfs (modern) | initrd (legacy) |
|---|---|---|
| Type | `tmpfs` — pure RAM filesystem | loop-mounted `ext2` image |
| Format | `cpio.gz` archive | raw filesystem image |
| Kernel support | Built-in since Linux 2.6 | Requires block device emulation |
| Memory | Freed completely after `switch_root` | More complex cleanup |
| Used today | Yes — all modern distros | Rarely — old kernels only |
| Our lab | Yes — `rootramfs.cpio.gz` | No |

Both serve the same purpose — give the kernel an early userspace before the real rootfs is available. initramfs replaced initrd because it is simpler, faster, and requires no block device emulation.

---

### Q6: Where is initramfs loaded in memory? Who decompresses it?

**U-Boot loads it** at a safe RAM address that does not overlap with the kernel or DTB:

```
Address map on RPi 3B+:
0x00080000  ← kernel Image (loaded here by U-Boot)
0x05600000  ← DTB (loaded here by U-Boot)
0x02700000  ← rootramfs.cpio.gz (loaded here by U-Boot)
```

**The kernel decompresses it** automatically when it finds a cpio archive at the address passed via `booti`:

```bash
booti ${kernel_addr_r} 0x02700000:${filesize} ${fdt_addr_r}
#     ↑ kernel addr    ↑ initramfs addr:size   ↑ DTB addr
```

The kernel decompresses the `.gz`, extracts the `cpio` contents into a `tmpfs`, and the compressed archive is then freed from RAM. The unpacked tmpfs remains in RAM as the running filesystem.

**Serial log proof:**
```
Loading Ramdisk to 1fedf000, end 1ffff4ea ... OK  ← U-Boot loaded it
Trying to unpack rootfs image as initramfs...      ← Kernel decompressing
Freeing initrd memory: 1152K                       ← Compressed archive freed
```

---

### Q7: How does the kernel switch from initramfs to real rootfs?

Using the `switch_root` command — which is a BusyBox applet:

```bash
# Inside initramfs /init script:
mount /dev/sda1 /mnt/real_root          # mount real rootfs
exec switch_root /mnt/real_root /sbin/init  # pivot and exec real init
```

`switch_root` does three things:
1. Changes the root filesystem from tmpfs to the real rootfs
2. Frees all memory used by the initramfs tmpfs
3. Executes the real `/sbin/init` (systemd, OpenRC, etc.)

**In our lab:** We do NOT use `switch_root` — we stay in the initramfs permanently. The initramfs IS our rootfs. This is valid for embedded systems where you want a minimal self-contained system entirely in RAM.

---

## Step-by-Step Build Guide

### Prerequisites

```bash
# Verify toolchain
aarch64-linux-gnu-gcc --version   # cross-compiler
which cpio                         # archive tool
which mkimage                      # U-Boot script compiler

# Verify BusyBox is built and statically linked
file ~/workspace/.../busybox-1.36.1/_install/bin/busybox
# Must show: statically linked, ARM aarch64
```

---

### Step 1 — Create Directory Structure

```bash
mkdir -p ~/initramfs/{bin,sbin,etc/init.d,proc,sys,dev}
cd ~/initramfs
ls -la
```

**Why each directory:**

| Directory | Purpose |
|-----------|---------|
| `bin/` | BusyBox binary + all command symlinks |
| `sbin/` | System commands — `init`, `reboot`, `halt` |
| `etc/init.d/` | Startup scripts — `rcS` runs at boot |
| `proc/` | Mount point for procfs — needed by `ps`, `top` |
| `sys/` | Mount point for sysfs — hardware info |
| `dev/` | Device nodes — `console`, `null`, `tty` |

---

### Step 2 — Copy BusyBox Binary

```bash
BUSYBOX=~/workspace/.../busybox-1.36.1/_install/bin/busybox

cp $BUSYBOX bin/busybox
chmod +x bin/busybox

# Verify
file bin/busybox   # must show: statically linked, ARM aarch64
ls -lh bin/busybox # should be ~2.1MB
```

---

### Step 3 — Create All Symlinks

**Critical:** Symlinks must use absolute paths (`/bin/busybox`) NOT relative paths or PC paths.

```bash
cd ~/initramfs

# Create 300+ symlinks for all BusyBox applets
bin/busybox --list | while read app; do
    ln -sf /bin/busybox bin/$app
done

# Create sbin/init — for BusyBox inittab ::restart: line
ln -sf /bin/busybox sbin/init

# Create /init — THIS is what the kernel looks for first in initramfs
ln -sf /bin/busybox init

# Verify — must show /bin/busybox NOT /home/user/...
ls -la bin/sh      # bin/sh -> /bin/busybox  ✓
ls -la sbin/init   # sbin/init -> /bin/busybox  ✓
ls -la init        # init -> /bin/busybox  ✓
```

**Why the symlink trick works:**
```
bin/ls → /bin/busybox    kernel calls busybox, argv[0]="ls"   → ls logic
bin/sh → /bin/busybox    kernel calls busybox, argv[0]="sh"   → shell
init   → /bin/busybox    kernel calls busybox, argv[0]="init" → init logic
```

---

### Step 4 — Create inittab and rcS

```bash
# inittab — BusyBox init reads this at startup
cat > etc/inittab << 'EOF'
::sysinit:/etc/init.d/rcS
::askfirst:-/bin/sh
::restart:/sbin/init
EOF

# rcS — startup script
cat > etc/init.d/rcS << 'EOF'
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
mount -t devtmpfs none /dev
echo "Welcome to Our Embedded Linux - Intake 46"
EOF

# CRITICAL — rcS must be executable
chmod +x etc/init.d/rcS

# Verify
ls -la etc/init.d/rcS   # must show -rwxr-xr-x
```

**inittab lines explained:**

| Line | Action | When |
|------|--------|------|
| `::sysinit:/etc/init.d/rcS` | Run rcS once | Before anything else |
| `::askfirst:-/bin/sh` | Print "Press Enter", launch shell | After sysinit |
| `::restart:/sbin/init` | Restart init | If init exits |

**Why each mount in rcS:**

| Mount | Without it |
|-------|-----------|
| `/proc` | `ps`, `top`, `/proc/cpuinfo` all fail |
| `/sys` | Device management broken |
| `/dev` | `/dev/null`, `/dev/tty` missing → shell crashes |

---

### Step 5 — Create Device Nodes

**This step is critical and often missed.** Without `/dev/console`, the kernel cannot print any output after launching `/init` — you get `Warning: unable to open an initial console` and a silent shell.

```bash
# Create essential device nodes
sudo mknod dev/console c 5 1   # kernel console output
sudo mknod dev/null    c 1 3   # discard output
sudo mknod dev/tty     c 5 0   # current terminal

# Verify — note the 'c' at start of permissions
ls -la dev/
# crw-r--r-- console  5, 1
# crw-r--r-- null     1, 3
# crw-r--r-- tty      5, 0
```

**Why statically created and not waiting for devtmpfs:**

When the kernel runs `/init`, devtmpfs is not mounted yet — `/dev` is empty. The kernel tries to open `/dev/console` immediately at launch. If it is not there → warning → no output even though the shell is running.

---

### Step 6 — Verify Complete Structure

```bash
# Check everything before packing
find . | sort

# Verify symlinks point to correct target
readlink init        # must show: /bin/busybox
readlink sbin/init   # must show: /bin/busybox
readlink bin/sh      # must show: /bin/busybox

# Check device nodes exist
find . -type c

# Check rcS is executable
ls -la etc/init.d/rcS
```

---

### Step 7 — Pack into cpio Archive

```bash
cd ~/initramfs

find . | cpio -H newc -o | gzip > ../rootramfs.cpio.gz
```

**Command breakdown:**

```
find .              → list every file/dir/symlink/device in current dir
    |
cpio -H newc -o     → pack into cpio archive
     -H newc           header format = newc (only format kernel accepts)
     -o                output mode (create archive)
    |
gzip                → compress the archive
    >
../rootramfs.cpio.gz → save one level up
```

**Verify the archive:**
```bash
ls -lh ../rootramfs.cpio.gz           # should be ~1.2MB

# List contents
zcat ../rootramfs.cpio.gz | cpio -t | grep -E "init|busybox|console"
# Should show:
# bin/busybox
# init
# sbin/init
# dev/console
```

---

### Step 8 — Update boot.cmd and Copy to SD Card

```bash
cat > boot.cmd << 'EOF'
echo "=== Embedded Linux Boot - Intake 46 ==="
fatload mmc 0:1 ${kernel_addr_r} Image
fatload mmc 0:1 ${fdt_addr_r} bcm2710-rpi-3-b-plus.dtb
fatload mmc 0:1 0x02700000 rootramfs.cpio.gz
setenv bootargs "console=ttyS0,115200 8250.nr_uarts=1 rdinit=/init loglevel=8"
booti ${kernel_addr_r} 0x02700000:${filesize} ${fdt_addr_r}
EOF

# Compile boot.cmd → boot.scr
mkimage -C none -A arm64 -T script -d boot.cmd boot.scr

# Copy to SD card
sudo cp boot.scr /media/$USER/BOOT/
sudo cp ../rootramfs.cpio.gz /media/$USER/BOOT/
sync
sudo umount /media/$USER/BOOT
sync
```

**bootargs explained:**

| Parameter | Meaning | Without it |
|-----------|---------|-----------|
| `console=ttyS0,115200` | Mini UART is `ttyS0`, 115200 baud | No shell output on TTL |
| `8250.nr_uarts=1` | Tell 8250 driver to register 1 Mini UART port | Driver fails with -EINVAL |
| `rdinit=/init` | Tell kernel which init to run | Kernel uses `/init` by default anyway |
| `loglevel=8` | Print all kernel messages | Less verbose output |

---

### Step 9 — Power On and Verify

Power on the RPi. You should see on TTL serial (minicom at 115200):

```
Starting kernel ...

[    0.000000] Booting Linux on physical CPU 0x0000000000
...
[    2.284823] Run /init as init process
Welcome to Our Embedded Linux - Intake 46

Please press Enter to activate this console.
~ # ls
bin  dev  etc  init  proc  sbin  sys
~ # echo "Hello from Eng.Fady - Intake 46"
Hello from Eng.Fady - Intake 46
~ # cat /proc/cpuinfo | grep Hardware
Hardware        : BCM2835
~ #
```

---

## Key Technical Discoveries

### Discovery 1 — The Symlink Path Bug

Using `busybox --install -s` from outside the initramfs directory creates symlinks with the full PC path:

```
bin/sh → /home/mac/workspace/.../bin/busybox  ✗ BREAKS on RPi
```

**Fix:** Create symlinks manually with absolute target path:

```bash
ln -sf /bin/busybox bin/sh   # /bin/busybox = path on RPi ✓
```

---

### Discovery 2 — start.elf Patches DTB bootargs

The GPU firmware (`start.elf`) reads `config.txt` and writes into the DTB `chosen` node:

```
enable_uart=1 in config.txt
        ↓
start.elf writes into DTB:
chosen {
    bootargs = "coherent_pool=1M 8250.nr_uarts=1 ...";
    stdout-path = "serial0:115200n8";
};
```

**Proof:**
```bash
strings start.elf | grep "8250.nr_uarts"
# Output: 8250.nr_uarts=1
#         8250.nr_uarts=0
```

When `setenv bootargs` is used in U-Boot → it **overrides** the DTB bootargs completely. When it is NOT used → kernel reads the GPU's pre-configured bootargs from DTB.

---

### Discovery 3 — fdt_addr vs fdt_addr_r

| Variable | What it contains |
|----------|-----------------|
| `${fdt_addr}` | GPU's DTB — overlays already applied by firmware |
| `${fdt_addr_r}` | Address where U-Boot loads its own copy of DTB |

Using `${fdt_addr}` → GPU overlay applied → correct UART config  
Using `${fdt_addr_r}` with `fatload` → must set all bootargs manually

---

### Discovery 4 — Mini UART Registers as ttyS0

After `enable_uart=1` and the `pi3-miniuart-bt` overlay:
- Mini UART → GPIO14/15 → TTL cable → registers as **`ttyS0`**
- PL011 → Bluetooth → registers as **`ttyAMA1`**

**Wrong:** `console=ttyAMA0`  
**Correct:** `console=ttyS0,115200`

---

## Final SD Card Contents

```
/media/mac/BOOT/
├── Image                        ← kernel (27MB)
├── bcm2710-rpi-3-b-plus.dtb    ← device tree blob
├── rootramfs.cpio.gz            ← initramfs archive (1.2MB)
├── boot.cmd                     ← human readable boot script
├── boot.scr                     ← compiled boot script (mkimage output)
├── config.txt                   ← GPU configuration
├── bootcode.bin                 ← GPU stage 1 bootloader
├── start.elf                    ← GPU firmware
├── fixup.dat                    ← GPU/ARM memory split
└── u-boot.bin                   ← U-Boot bootloader (640KB)
```

---

## RAM Layout at Boot

```
Address          Contents                    Status after boot
───────────────────────────────────────────────────────────────
0x00080000       Kernel Image (27MB)         Stays — runs forever
0x05600000       DTB (35KB)                  Stays — kernel uses it
0x02700000       rootramfs.cpio.gz (1.2MB)   Freed after decompression
0x1fedf000       tmpfs (unpacked initramfs)  Running filesystem in RAM
U-Boot area      U-Boot binary               Freed after booti
───────────────────────────────────────────────────────────────
Total RAM: 948MB   Used at shell prompt: ~30MB   Free: ~918MB
```

---

*Embedded Linux · Lab 7 · Intake 46 · March 2026*  
*Raspberry Pi 3B+ · AArch64 · Kernel 6.12.75-v8+ · BusyBox 1.36.1*
