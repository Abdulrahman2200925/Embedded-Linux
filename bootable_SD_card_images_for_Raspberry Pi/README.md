# Lab 2: Creating a Virtual SD Card for Embedded Linux Development

## Table of Contents
1. [Theory Questions](#theory-questions)
2. [Practical Workflow](#practical-workflow)
3. [Command Reference](#command-reference)
4. [Troubleshooting](#troubleshooting)

---

## Theory Questions

### Question 1: Create a 1 GiB Virtual Disk Image

**Command:**
```bash
dd if=/dev/zero of=sd_card.img bs=1M count=1024
```

**Explanation:**
- `dd` - Data duplicator command (low-level copy)
- `if=/dev/zero` - Input file: special device that produces infinite null bytes (0x00)
- `of=sd_card.img` - Output file: name of virtual disk image
- `bs=1M` - Block size: 1 megabyte (transfer 1MB at a time)
- `count=1024` - Number of blocks: 1024 blocks × 1MB = 1024MB = 1GB

**Expected Output:**
```
1023+0 records in
1023+0 records out
1072693248 bytes (1.1 GB, 1023 MiB) copied, 0.659766 s, 1.6 GB/s
```

**Alternative (Faster):**
```bash
fallocate -l 1G sd_card.img
```
- Instantly allocates 1GB file without writing zeros
- Much faster (no actual write to disk)
- Creates a "sparse file" (empty space not actually written)

---

### Question 2: DOS/MBR vs GPT Partition Scheme

#### DOS/MBR (Master Boot Record)
**Structure:**
- First 512 bytes of disk (sector 0)
- **Bytes 0-445:** Bootstrap code (not used on Raspberry Pi)
- **Bytes 446-509:** Partition table (64 bytes)
  - 4 partition entries × 16 bytes each
  - Maximum 4 primary partitions
- **Bytes 510-511:** Boot signature `0x55AA`

**Partition Table Entry (16 bytes):**
```
Offset  Size  Description
0       1     Boot flag (0x80 = bootable, 0x00 = non-bootable)
1-3     3     CHS start address (legacy)
4       1     Partition type (0x06=FAT16, 0x0B=FAT32, 0x83=Linux)
5-7     3     CHS end address (legacy)
8-11    4     LBA start sector (32-bit)
12-15   4     Number of sectors (32-bit)
```

**Limitations:**
- Maximum 4 primary partitions (or 3 primary + 1 extended)
- Maximum disk size: 2TB (2^32 sectors × 512 bytes)
- No redundancy (if MBR corrupted, all partitions lost)
- Limited partition type codes

#### GPT (GUID Partition Table)
**Structure:**
- **Sector 0:** Protective MBR (compatibility)
- **Sector 1:** GPT Header
  - Magic signature "EFI PART"
  - Disk GUID
  - Partition table location and size
  - CRC32 checksum
- **Sectors 2-33:** Partition entries (128 entries by default)
- **Last 33 sectors:** Backup GPT header and partition table

**Advantages:**
- Up to 128 partitions (no primary/extended distinction)
- Maximum disk size: 9.4 ZB (zettabytes)
- Redundant headers (primary and backup)
- CRC32 error detection
- Unique GUID for each partition
- Partition names (up to 36 Unicode characters)

**When to Use:**
- **MBR:** Legacy BIOS systems, Raspberry Pi (GPU firmware requires it), disks <2TB
- **GPT:** UEFI systems, modern PCs, disks >2TB, need >4 partitions

---

### Question 3: File Systems (FAT16, FAT32, EXT4)

#### FAT16 (File Allocation Table 16-bit)
**Characteristics:**
- Maximum partition size: 2GB (with 32KB clusters) to 4GB (with 64KB clusters)
- Maximum file size: 2GB
- Cluster size: 2KB to 64KB (depends on partition size)
- Maximum files in root directory: 512 entries (fixed)
- Compatibility: Excellent (all OS, embedded systems, Raspberry Pi GPU)

**Structure:**
```
Boot Sector (1 sector)
FAT 1 (copy 1)
FAT 2 (copy 2 - backup)
Root Directory (fixed size)
Data Area (files and subdirectories)
```

**Usage:**
- Small removable media (SD cards <2GB)
- Raspberry Pi boot partition (GPU firmware can only read FAT)
- Maximum cross-platform compatibility

#### FAT32 (File Allocation Table 32-bit)
**Characteristics:**
- Maximum partition size: 8TB (theoretical), 2TB (Windows format limit)
- Maximum file size: 4GB (hard limit - cannot store files ≥4GB)
- Cluster size: 4KB to 32KB
- No fixed root directory limit
- Compatibility: Excellent (all modern OS)

**Improvements over FAT16:**
- 28-bit cluster addressing (not true 32-bit)
- Smaller cluster sizes = less wasted space
- Root directory can be anywhere in data area
- Unlimited root directory entries

**Usage:**
- USB flash drives
- SD cards >2GB
- Cross-platform storage
- Raspberry Pi boot partition (alternative to FAT16)

#### EXT4 (Fourth Extended Filesystem)
**Characteristics:**
- Maximum partition size: 1 EB (exabyte) = 1,048,576 TB
- Maximum file size: 16 TB
- Extents-based allocation (contiguous blocks)
- Journaling (crash recovery)
- Delayed allocation (performance)
- Online defragmentation
- Backward compatible with ext2/ext3

**Advanced Features:**
- **Journaling:** Logs changes before committing (prevents corruption)
- **Extents:** Stores ranges of contiguous blocks (reduces fragmentation)
- **Delayed allocation:** Delays block allocation for better performance
- **Persistent pre-allocation:** Reserve space for files
- **Multiblock allocation:** Allocates multiple blocks at once
- **Journal checksumming:** Detects corruption in journal
- **Fast fsck:** Optimized filesystem checking
- **Nanosecond timestamps:** Precise file modification times
- **Unlimited subdirectories:** No fixed limit (ext3 had 32,000 limit)

**Structure:**
```
Block 0: Boot sector (unused by ext4)
Block 1: Superblock (filesystem metadata)
Block Group 0:
  - Group Descriptor
  - Block Bitmap
  - Inode Bitmap
  - Inode Table
  - Data Blocks
Block Group 1...
```

**Usage:**
- Linux root filesystem
- High-performance storage
- Large files (videos, databases)
- Raspberry Pi rootfs partition
- Servers and workstations

**Comparison Table:**

| Feature | FAT16 | FAT32 | EXT4 |
|---------|-------|-------|------|
| Max Partition | 4GB | 8TB | 1EB |
| Max File Size | 2GB | 4GB | 16TB |
| Journaling | No | No | Yes |
| Permissions | No | No | Yes (Unix) |
| Fragmentation | High | High | Low |
| Performance | Slow | Medium | Fast |
| Linux Native | No | No | Yes |
| GPU Compatible | Yes | Yes | No |

**Why Raspberry Pi Uses Both:**
- **FAT16/32 for /boot:** GPU firmware can only read FAT filesystems
- **EXT4 for rootfs:** Better performance, permissions, and reliability for Linux

---

### Question 4: Formatting and Partitioning

See [Practical Workflow](#practical-workflow) section below for complete step-by-step process.

---

### Question 5: Loop Devices

#### What are Loop Devices?

A **loop device** is a pseudo-device that makes a **regular file accessible as a block device**.

**Concept:**
```
Regular File (sd_card.img) → Loop Driver → Block Device (/dev/loop0)
```

**Why Linux Uses Them:**

1. **Mount disk images:** Mount ISO files, disk images without burning to physical media
2. **Test filesystems:** Create and test filesystems without physical disks
3. **Development:** Prepare bootable images for embedded systems
4. **Virtualization:** Backend for virtual machine disk images
5. **Encrypted containers:** Create encrypted file containers (LUKS)

**How It Works:**

```
Application writes to /dev/loop0
         ↓
Kernel loop driver intercepts
         ↓
Translates block I/O to file I/O
         ↓
Writes to sd_card.img at calculated offset
         ↓
File stored on actual disk
```

#### 5a. Command to Create a Loop Device

```bash
sudo losetup -fP sd_card.img
```

**Flags:**
- `-f` - Find first free loop device automatically
- `-P` - **Partition scan** (CRITICAL!) - tells kernel to detect and create partition devices
- `sd_card.img` - File to attach

**What happens:**
```
Before: sd_card.img (regular file)

After:
  /dev/loop0     → entire disk image
  /dev/loop0p1   → partition 1 (if partitioned)
  /dev/loop0p2   → partition 2 (if partitioned)
```

**Alternative (Manual):**
```bash
sudo losetup /dev/loop0 sd_card.img  # Specify loop device manually
sudo losetup -P /dev/loop0 sd_card.img  # Add partition scan to existing
```

**To attach with offset (access specific partition directly):**
```bash
# Get partition start offset
fdisk -l sd_card.img
# Partition 1 starts at sector 2048, sector size 512 bytes
# Offset = 2048 * 512 = 1048576

sudo losetup -o 1048576 /dev/loop0 sd_card.img
```

#### 5b. Command to List All Loop Devices

**Method 1: losetup**
```bash
losetup -l
```
Output shows: NAME, SIZELIMIT, OFFSET, AUTOCLEAR, RO, BACK-FILE, DIO, LOG-SEC

**Method 2: losetup -a (show attached only)**
```bash
losetup -a
```
Output: `/dev/loop0: [device]:inode (sd_card.img)`

**Method 3: lsblk**
```bash
lsblk | grep loop
```
Shows loop devices with size and mount points

**Method 4: ls**
```bash
ls -l /dev/loop*
```
Lists all loop device nodes (allocated or not)

#### 5c. Command to Detach a Loop Device

**Important:** Must unmount all partitions first!

```bash
# Step 1: Unmount all mounted partitions
sudo umount /dev/loop0p1
sudo umount /dev/loop0p2

# Step 2: Detach loop device
sudo losetup -d /dev/loop0
```

**Flags:**
- `-d` - Detach/delete loop device

**Detach all loop devices:**
```bash
sudo losetup -D
```

**Force unmount and detach:**
```bash
sudo umount -f /dev/loop0p1  # Force unmount
sudo losetup -d /dev/loop0
```

---

### Question 6: Check Current Loop Device Limit

```bash
cat /sys/module/loop/parameters/max_loop
```

**Default value:** Usually `8` or `0` (0 means dynamic/unlimited on modern kernels)

**Alternative:**
```bash
ls -l /dev/loop* | wc -l  # Count existing loop device nodes
```

---

### Question 7: Expand Number of Loop Devices

#### Temporary (Until Reboot):

```bash
# Remove loop module
sudo modprobe -r loop

# Reload with new limit
sudo modprobe loop max_loop=16
```

**Note:** Cannot unload if any loop device is in use!

#### Permanent (Survives Reboot):

**Method 1: Create modprobe configuration**
```bash
echo "options loop max_loop=16" | sudo tee /etc/modprobe.d/loop.conf
```

**Method 2: Edit GRUB (system-wide parameter)**
```bash
sudo nano /etc/default/grub
# Add to GRUB_CMDLINE_LINUX:
GRUB_CMDLINE_LINUX="... loop.max_loop=16"

sudo update-grub
sudo reboot
```

**Modern kernels:** Setting to `0` enables dynamic allocation (creates loop devices as needed)

---

### Question 8: Attach Virtual Disk Image as Loop Device

See answer to Question 5a and [Practical Workflow](#practical-workflow) section.

---

### Question 9: Format Virtual Disk Image Partitions

#### 9a. Format First Partition (FAT16, label "boot")

```bash
sudo mkfs.vfat -F 16 -n boot /dev/loop0p1
```

**Explanation:**
- `mkfs.vfat` - Make FAT filesystem
- `-F 16` - FAT type: 16-bit File Allocation Table
- `-n boot` - Volume label (name shown when mounted)
- `/dev/loop0p1` - Partition to format

**What happens:**
1. Writes boot sector with filesystem parameters
2. Creates two FAT tables (primary and backup)
3. Creates root directory area
4. Marks bad clusters (if any)
5. Stores volume label in boot sector

**Alternative for FAT32:**
```bash
sudo mkfs.vfat -F 32 -n boot /dev/loop0p1
```

#### 9b. Format Second Partition (EXT4, label "rootfs")

```bash
sudo mkfs.ext4 -L rootfs /dev/loop0p2
```

**Explanation:**
- `mkfs.ext4` - Make ext4 filesystem
- `-L rootfs` - Filesystem label (stored in superblock)
- `/dev/loop0p2` - Partition to format

**What happens:**
1. Writes superblock (filesystem metadata)
2. Creates block groups
3. Allocates inode table
4. Creates root directory (inode 2)
5. Initializes journal
6. Creates lost+found directory

**Advanced options:**
```bash
# With specific features
sudo mkfs.ext4 -L rootfs -O ^has_journal /dev/loop0p2  # Disable journaling

# With reserved blocks for root
sudo mkfs.ext4 -L rootfs -m 1 /dev/loop0p2  # Reserve 1% (default is 5%)

# With specific inode count
sudo mkfs.ext4 -L rootfs -N 1000000 /dev/loop0p2  # 1 million inodes
```

---

### Question 10: Mount and Unmount Commands

#### Mount Command

**Purpose:** Attach a filesystem to the directory tree at a specific point (mount point).

**Basic Syntax:**
```bash
sudo mount [OPTIONS] <device> <mount_point>
```

**Examples:**
```bash
# Mount with automatic filesystem detection
sudo mount /dev/loop0p1 /mnt/boot

# Mount with explicit filesystem type
sudo mount -t vfat /dev/loop0p1 /mnt/boot

# Mount read-only
sudo mount -o ro /dev/loop0p1 /mnt/boot

# Mount with specific options
sudo mount -o rw,uid=1000,gid=1000 /dev/loop0p1 /mnt/boot
```

**Common Options:**
- `-t <type>` - Filesystem type (vfat, ext4, ntfs, etc.)
- `-o <options>` - Mount options (comma-separated):
  - `rw` - Read-write (default)
  - `ro` - Read-only
  - `sync` - Synchronous I/O (safer, slower)
  - `async` - Asynchronous I/O (faster, default)
  - `noexec` - Prevent execution of binaries
  - `nosuid` - Ignore SUID/SGID bits
  - `nodev` - Don't interpret device files
  - `uid=<id>` - Set owner user ID (FAT only)
  - `gid=<id>` - Set owner group ID (FAT only)
  - `umask=<mask>` - Set permission mask (FAT only)

**What Mount Does:**
1. Reads filesystem superblock from device
2. Verifies filesystem integrity
3. Allocates kernel data structures
4. Links filesystem to mount point directory
5. Makes files accessible through mount point
6. Updates `/proc/mounts` and `/etc/mtab`

**View all mounted filesystems:**
```bash
mount                    # All mounts
mount | grep loop        # Only loop devices
df -h                    # Mounted filesystems with usage
findmnt                  # Tree view of mounts
cat /proc/mounts         # Kernel's view of mounts
```

#### Unmount Command

**Purpose:** Detach a filesystem from the directory tree.

**Basic Syntax:**
```bash
sudo umount <device_or_mount_point>
```

**Examples:**
```bash
# Unmount by device
sudo umount /dev/loop0p1

# Unmount by mount point
sudo umount /mnt/boot

# Force unmount (if busy)
sudo umount -f /mnt/boot

# Lazy unmount (detach now, cleanup when no longer busy)
sudo umount -l /mnt/boot
```

**Common Options:**
- `-f` - Force unmount (NFS, unresponsive devices)
- `-l` - Lazy unmount (detach immediately, cleanup later)
- `-r` - If unmount fails, remount read-only
- `-n` - Don't update `/etc/mtab`

**What Unmount Does:**
1. Flushes pending writes to disk
2. Closes all open file handles (or fails if busy)
3. Releases kernel data structures
4. Updates `/proc/mounts` and `/etc/mtab`

**Troubleshooting Busy Filesystems:**
```bash
# Find processes using the filesystem
sudo lsof +D /mnt/boot
sudo fuser -m /mnt/boot

# Kill processes using the filesystem
sudo fuser -km /mnt/boot

# Then unmount
sudo umount /mnt/boot
```

---

### Question 11: Block Device vs Character Device

#### Block Device

**Definition:** Device that stores and retrieves data in fixed-size blocks (usually 512 bytes or 4KB).

**Characteristics:**
- **Random access** - Can read/write any block directly
- **Buffered I/O** - Kernel buffers/caches data
- **Seekable** - Can jump to any position
- **Fixed block size** - Transfers in block units
- **Examples:** Hard disks, SSDs, SD cards, USB drives, loop devices

**Device Numbers:**
```bash
ls -l /dev/sda /dev/loop0
brw-rw---- 1 root disk 8, 0 Feb 13 /dev/sda
brw-rw---- 1 root disk 7, 0 Feb 13 /dev/loop0
^
└─ 'b' indicates block device
```

**Major/Minor Numbers:**
- **Major:** Device driver (8 = SCSI disk, 7 = loop device)
- **Minor:** Device instance (0 = first device, 1 = second, etc.)

**Operations:**
```c
// Block device operations
struct block_device_operations {
    int (*open) (struct block_device *, fmode_t);
    void (*release) (struct gendisk *, fmode_t);
    int (*ioctl) (struct block_device *, fmode_t, unsigned, unsigned long);
    int (*compat_ioctl) (struct block_device *, fmode_t, unsigned, unsigned long);
    // ...
};
```

**Use Cases:**
- Storage devices (disks, partitions)
- Filesystems must be on block devices
- Can be partitioned
- Support buffering and caching

#### Character Device

**Definition:** Device that transfers data as a stream of characters (bytes), one at a time.

**Characteristics:**
- **Sequential access** - Data flows in order (usually)
- **Unbuffered I/O** - Direct communication (usually)
- **Not seekable** - Cannot jump to arbitrary position (usually)
- **Variable size** - No fixed block size
- **Examples:** Serial ports, keyboards, mice, printers, terminals, sound cards, /dev/random

**Device Numbers:**
```bash
ls -l /dev/ttyUSB0 /dev/null
crw-rw---- 1 root dialout 188, 0 Feb 13 /dev/ttyUSB0
crw-rw-rw- 1 root root      1, 3 Feb 13 /dev/null
^
└─ 'c' indicates character device
```

**Operations:**
```c
// Character device operations
struct file_operations {
    ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);
    ssize_t (*write) (struct file *, const char __user *, size_t, loff_t *);
    int (*open) (struct inode *, struct file *);
    int (*release) (struct inode *, struct file *);
    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
    // ...
};
```

**Use Cases:**
- Serial communication (UART, USB-to-serial)
- Input devices (keyboard, mouse)
- Pseudo-devices (/dev/null, /dev/zero, /dev/random)
- Some special hardware (GPIO, I2C in userspace)

#### Comparison Table

| Feature | Block Device | Character Device |
|---------|--------------|------------------|
| **Access** | Random access | Sequential (usually) |
| **Buffer** | Kernel buffered | Direct (usually) |
| **Block size** | Fixed (512B, 4KB) | Variable (byte stream) |
| **Seekable** | Yes | Usually no |
| **Partition** | Yes | No |
| **Filesystem** | Yes | No |
| **Caching** | Yes (page cache) | Usually no |
| **Examples** | HDD, SSD, SD card | Serial port, keyboard |
| **Notation** | `brw-` | `crw-` |
| **Driver type** | `block_device_operations` | `file_operations` |

#### Special Cases

Some devices blur the line:

1. **Tape drives** - Block devices but sequential access
2. **/dev/random, /dev/zero** - Character devices but seekable
3. **Raw device nodes** - Allow unbuffered access to block devices
4. **Memory devices** (/dev/mem) - Character device but random access

#### Why It Matters

**For embedded systems:**
- **SD card** = Block device (`/dev/mmcblk0`)
  - Can partition, format, mount
  - Kernel buffers I/O for performance
  
- **Serial console** = Character device (`/dev/ttyS0`)
  - Direct communication with bootloader/kernel
  - No buffering needed for commands

**For your lab:**
- Virtual disk (loop device) = **Block device**
  - Can partition and format like real SD card
  - Supports filesystem operations
  - Can be mounted

---

### Question 12: Create Mount Points and Mount Partitions

See [Practical Workflow](#practical-workflow) section below.

---

## Practical Workflow

### Complete Process: Creating a Virtual SD Card

#### Step 1: Create Virtual Disk Image

**Option A: Using dd (writes zeros, slower but universal)**
```bash
dd if=/dev/zero of=sd_card.img bs=1M count=1024
# Creates 1GB file filled with zeros
# Time: ~1-5 seconds depending on disk speed
```

**Option B: Using fallocate (instant, preferred)**
```bash
fallocate -l 1G sd_card.img
# Creates 1GB sparse file instantly
# Time: <0.1 second
```

**Verification:**
```bash
ls -lh sd_card.img
# Output: -rw-r--r-- 1 user user 1.0G Feb 13 sd_card.img

file sd_card.img
# Output: sd_card.img: data
```

---

#### Step 2: Attach as Loop Device

```bash
sudo losetup -fP sd_card.img
```

**Verify attachment:**
```bash
losetup -l
# or
lsblk | grep loop
```

**Find which loop device was used:**
```bash
losetup -a | grep sd_card.img
# Output: /dev/loop0: [device]:inode (sd_card.img)
```

---

#### Step 3: Partition the Disk

**Option A: Using fdisk (traditional, interactive)**

```bash
sudo fdisk /dev/loop0
```

**Interactive commands:**
```
Command (m for help): o
Created a new DOS disklabel with disk identifier 0x12345678

Command (m for help): n
Partition type
   p   primary (0 primary, 0 extended, 4 free)
   e   extended (container for logical partitions)
Select (default p): p
Partition number (1-4, default 1): 1
First sector (2048-2097151, default 2048): [Press Enter]
Last sector, +/-sectors or +/-size{K,M,G,T,P} (2048-2097151, default 2097151): +200M

Created a new partition 1 of type 'Linux' and of size 200 MiB.

Command (m for help): t
Selected partition 1
Hex code or alias (type L to list all): 6
Changed type of partition 'Linux' to 'FAT16'.

Command (m for help): a
Selected partition 1
The bootable flag on partition 1 is enabled now.

Command (m for help): n
Partition type
   p   primary (1 primary, 0 extended, 3 free)
   e   extended (container for logical partitions)
Select (default p): p
Partition number (2-4, default 2): 2
First sector (411648-2097151, default 411648): [Press Enter]
Last sector, +/-sectors or +/-size{K,M,G,T,P} (411648-2097151, default 2097151): [Press Enter]

Created a new partition 2 of type 'Linux' and of size 824 MiB.

Command (m for help): p
Disk /dev/loop0: 1 GiB, 1073741824 bytes, 2097152 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x12345678

Device       Boot  Start     End Sectors  Size Id Type
/dev/loop0p1 *      2048  411647  409600  200M  6 FAT16
/dev/loop0p2      411648 2097151 1685504  824M 83 Linux

Command (m for help): w
The partition table has been altered.
Calling ioctl() to re-read partition table.
Syncing disks.
```

**Option B: Using cfdisk (menu-driven, easier)**

```bash
sudo cfdisk /dev/loop0
```

**Interactive steps:**
1. Select label type: `dos` (for MBR)
2. Select `[ New ]` → Enter size: `200M` → Select `primary`
3. Navigate to Type → Select `FAT16`
4. Navigate to Bootable → Press Enter (marks as bootable)
5. Select `[ New ]` → Press Enter (uses remaining space) → Select `primary`
6. Type defaults to `Linux (83)` - leave as is (ext4 will use this)
7. Navigate to `[ Write ]` → Type `yes` → Press Enter
8. Select `[ Quit ]`

**Option C: Using parted (scriptable, non-interactive)**

```bash
sudo parted /dev/loop0 --script \
    mklabel msdos \
    mkpart primary fat16 1MiB 200MiB \
    set 1 boot on \
    mkpart primary ext4 200MiB 100%
```

**Explanation:**
- `mklabel msdos` - Create MBR partition table
- `mkpart primary fat16 1MiB 200MiB` - Partition 1: 1MB-200MB, FAT16
- `set 1 boot on` - Set bootable flag on partition 1
- `mkpart primary ext4 200MiB 100%` - Partition 2: 200MB to end, ext4

**Verify partitions:**
```bash
sudo fdisk -l /dev/loop0
# or
lsblk | grep loop
```

**Expected output:**
```
Device       Boot  Start     End Sectors  Size Id Type
/dev/loop0p1 *      2048  411647  409600  200M  6 FAT16
/dev/loop0p2      411648 2097151 1685504  824M 83 Linux
```

---

#### Step 4: Format Partitions

**Format partition 1 (FAT16 boot partition):**
```bash
sudo mkfs.vfat -F 16 -n boot /dev/loop0p1
```

**Expected output:**
```
mkfs.fat 4.2 (2021-01-31)
```

**Format partition 2 (ext4 root partition):**
```bash
sudo mkfs.ext4 -L rootfs /dev/loop0p2
```

**Expected output:**
```
mke2fs 1.46.5 (30-Dec-2021)
Creating filesystem with 210688 4k blocks and 52736 inodes
Filesystem UUID: 12345678-1234-1234-1234-123456789abc
Superblock backups stored on blocks:
        32768, 98304, 163840

Allocating group tables: done
Writing inode tables: done
Creating journal (4096 blocks): done
Writing superblocks and filesystem accounting information: done
```

**Verify filesystems:**
```bash
sudo blkid /dev/loop0p1
sudo blkid /dev/loop0p2
```

**Expected output:**
```
/dev/loop0p1: LABEL="boot" UUID="1234-5678" TYPE="vfat"
/dev/loop0p2: LABEL="rootfs" UUID="12345678-1234-1234-1234-123456789abc" TYPE="ext4"
```

---

#### Step 5: Create Mount Points

```bash
sudo mkdir -p /mnt/boot /mnt/rootfs
```

**Alternative (user-specific mount points):**
```bash
mkdir -p ~/sdcard/boot ~/sdcard/rootfs
```

---

#### Step 6: Mount Partitions

```bash
sudo mount /dev/loop0p1 /mnt/boot
sudo mount /dev/loop0p2 /mnt/rootfs
```

**Verify mounts:**
```bash
mount | grep loop
# or
df -h | grep loop
# or
lsblk | grep loop
```

**Expected output:**
```
loop0       7:0    0  1023M  0 loop
├─loop0p1   259:8  0   200M  0 part /mnt/boot
└─loop0p2   259:9  0   823M  0 part /mnt/rootfs
```

---

#### Step 7: Verify Write Access

```bash
# Test boot partition
sudo touch /mnt/boot/test.txt
ls -l /mnt/boot/

# Test rootfs partition
sudo touch /mnt/rootfs/test.txt
ls -l /mnt/rootfs/

# Check disk usage
df -h /mnt/boot /mnt/rootfs
```

---

#### Step 8: Populate with Files (Example)

**Boot partition (copy kernel, bootloader, etc.):**
```bash
# Example: Copy Raspberry Pi boot files
sudo cp bootcode.bin /mnt/boot/
sudo cp start.elf /mnt/boot/
sudo cp kernel7.img /mnt/boot/
sudo cp config.txt /mnt/boot/
sudo cp cmdline.txt /mnt/boot/
```

**Rootfs partition (extract root filesystem):**
```bash
# Example: Extract rootfs archive
sudo tar -xzf rootfs.tar.gz -C /mnt/rootfs/
```

---

#### Step 9: Safely Unmount

```bash
# Sync to ensure all writes complete
sync

# Unmount partitions
sudo umount /mnt/boot
sudo umount /mnt/rootfs

# Detach loop device
sudo losetup -d /dev/loop0
```

**Verify clean unmount:**
```bash
mount | grep loop  # Should return nothing
losetup -l | grep sd_card.img  # Should return nothing
```

---

#### Step 10: Write to Physical SD Card (Optional)

```bash
# Find SD card device (BE CAREFUL!)
lsblk

# Write image to SD card (DESTROYS ALL DATA ON SD CARD!)
sudo dd if=sd_card.img of=/dev/mmcblk0 bs=4M status=progress

# Sync to ensure all writes complete
sync
```

**WARNING:** Double-check device name! Writing to wrong device destroys data!

---

## Command Reference

### Quick Reference Table

| Task | Command |
|------|---------|
| Create 1GB image (dd) | `dd if=/dev/zero of=sd_card.img bs=1M count=1024` |
| Create 1GB image (fallocate) | `fallocate -l 1G sd_card.img` |
| Attach as loop device | `sudo losetup -fP sd_card.img` |
| List loop devices | `losetup -l` or `lsblk \| grep loop` |
| Partition (fdisk) | `sudo fdisk /dev/loop0` |
| Partition (cfdisk) | `sudo cfdisk /dev/loop0` |
| Partition (parted) | `sudo parted /dev/loop0` |
| Format FAT16 | `sudo mkfs.vfat -F 16 -n boot /dev/loop0p1` |
| Format ext4 | `sudo mkfs.ext4 -L rootfs /dev/loop0p2` |
| Create mount points | `sudo mkdir -p /mnt/boot /mnt/rootfs` |
| Mount partitions | `sudo mount /dev/loop0p1 /mnt/boot` |
| Unmount partitions | `sudo umount /mnt/boot /mnt/rootfs` |
| Detach loop device | `sudo losetup -d /dev/loop0` |
| Check filesystem | `sudo blkid /dev/loop0p1` |
| View partition table | `sudo fdisk -l /dev/loop0` |

---

### Complete One-Liner Workflow

```bash
# Create, partition, format, and mount in one go
fallocate -l 1G sd.img && \
sudo losetup -fP sd.img && \
LOOP=$(losetup -a | grep sd.img | cut -d: -f1) && \
sudo parted $LOOP --script mklabel msdos mkpart primary fat16 1MiB 200MiB set 1 boot on mkpart primary ext4 200MiB 100% && \
sudo mkfs.vfat -F 16 -n boot ${LOOP}p1 && \
sudo mkfs.ext4 -L rootfs ${LOOP}p2 && \
sudo mkdir -p /mnt/boot /mnt/rootfs && \
sudo mount ${LOOP}p1 /mnt/boot && \
sudo mount ${LOOP}p2 /mnt/rootfs && \
echo "Virtual SD card ready at $LOOP"
```

---

## Troubleshooting

### Common Issues and Solutions

#### 1. "losetup: cannot find an unused loop device"

**Problem:** No free loop devices available

**Solution:**
```bash
# Check current limit
cat /sys/module/loop/parameters/max_loop

# Increase limit temporarily
sudo modprobe -r loop
sudo modprobe loop max_loop=16

# Or permanently
echo "options loop max_loop=16" | sudo tee /etc/modprobe.d/loop.conf
```

---

#### 2. "Partition devices not created (/dev/loop0p1 missing)"

**Problem:** Forgot `-P` flag when attaching loop device

**Solution:**
```bash
# Detach and reattach with -P
sudo losetup -d /dev/loop0
sudo losetup -fP sd_card.img

# Or force kernel to rescan
sudo partprobe /dev/loop0
# or
sudo blockdev --rereadpt /dev/loop0
```

---

#### 3. "Device or resource busy" when detaching

**Problem:** Partitions still mounted

**Solution:**
```bash
# Find what's using the device
lsof +D /mnt/boot
sudo fuser -m /dev/loop0p1

# Unmount all partitions
sudo umount /dev/loop0p1
sudo umount /dev/loop0p2

# Then detach
sudo losetup -d /dev/loop0
```

---

#### 4. "mount: wrong fs type, bad option" for FAT partition

**Problem:** Missing vfat kernel module or wrong partition type

**Solution:**
```bash
# Check if vfat module loaded
lsmod | grep vfat

# Load module if needed
sudo modprobe vfat

# Check partition type
sudo fdisk -l /dev/loop0
# Type should be '6' (FAT16) or 'b'/'c' (FAT32)
```

---

#### 5. "Invalid argument" when formatting FAT16

**Problem:** Partition too small (<4MB) or too large (>4GB for FAT16)

**Solution:**
```bash
# For FAT16: partition must be 16MB-4GB
# If partition is <16MB, use FAT12 (not recommended)
# If partition is >4GB, use FAT32
sudo mkfs.vfat -F 32 -n boot /dev/loop0p1
```

---

#### 6. Partition table changes not visible

**Problem:** Kernel hasn't updated partition table

**Solution:**
```bash
# Force kernel to re-read partition table
sudo partprobe /dev/loop0

# or
sudo blockdev --rereadpt /dev/loop0

# or detach and reattach
sudo losetup -d /dev/loop0
sudo losetup -fP sd_card.img
```

---

#### 7. "Permission denied" when mounting

**Problem:** Need root privileges

**Solution:**
```bash
# Always use sudo for mount operations
sudo mount /dev/loop0p1 /mnt/boot

# To allow user access after mounting
sudo mount -o uid=1000,gid=1000 /dev/loop0p1 /mnt/boot
# Replace 1000 with your user/group ID (check with 'id')
```

---

#### 8. How to reset and start over

```bash
# Unmount everything
sudo umount /dev/loop0p1 2>/dev/null
sudo umount /dev/loop0p2 2>/dev/null

# Detach loop device
sudo losetup -d /dev/loop0 2>/dev/null

# Delete image file
rm sd_card.img

# Start fresh
fallocate -l 1G sd_card.img
sudo losetup -fP sd_card.img
```

---

## Additional Tips

### Automated Cleanup Script

Save as `cleanup_loop.sh`:
```bash
#!/bin/bash
# Cleanup script for loop devices

LOOP_DEVICE=$1

if [ -z "$LOOP_DEVICE" ]; then
    echo "Usage: $0 /dev/loopX"
    exit 1
fi

echo "Cleaning up $LOOP_DEVICE..."

# Unmount all partitions
for part in ${LOOP_DEVICE}p*; do
    if mountpoint -q $part 2>/dev/null; then
        echo "Unmounting $part..."
        sudo umount $part
    fi
done

# Detach loop device
if losetup -a | grep -q $LOOP_DEVICE; then
    echo "Detaching $LOOP_DEVICE..."
    sudo losetup -d $LOOP_DEVICE
fi

echo "Cleanup complete!"
```

**Usage:**
```bash
chmod +x cleanup_loop.sh
./cleanup_loop.sh /dev/loop0
```

---

### Check Image Integrity

```bash
# Verify partition table
sudo fdisk -l sd_card.img

# Check filesystems
sudo fsck.vfat -n /dev/loop0p1  # -n = no modifications
sudo fsck.ext4 -n /dev/loop0p2

# or directly on image file (requires offset calculation)
# Get partition start and size from fdisk output
sudo fdisk -l sd_card.img
# Partition 1 starts at sector 2048, size 409600 sectors
OFFSET=$((2048 * 512))
sudo mount -o loop,offset=$OFFSET sd_card.img /mnt/boot
```

---

### Convert to Other Formats

```bash
# Convert to QEMU qcow2 format
qemu-img convert -f raw -O qcow2 sd_card.img sd_card.qcow2

# Convert to VirtualBox VDI
VBoxManage convertfromraw sd_card.img sd_card.vdi

# Convert to VMware VMDK
qemu-img convert -f raw -O vmdk sd_card.img sd_card.vmdk
```

---

### Resize Virtual Disk

```bash
# Expand image file
fallocate -l 2G sd_card.img  # Expand to 2GB

# Attach and resize partition
sudo losetup -fP sd_card.img
LOOP=$(losetup -a | grep sd_card.img | cut -d: -f1)

# Use parted to resize partition 2
sudo parted $LOOP resizepart 2 100%

# Resize filesystem
sudo e2fsck -f ${LOOP}p2  # Force check first
sudo resize2fs ${LOOP}p2  # Expand filesystem to fill partition
```

---

## Summary Flowchart

```
┌─────────────────────────────────────────────┐
│ 1. Create Virtual Disk Image               │
│    dd / fallocate                           │
└────────────────┬────────────────────────────┘
                 │
                 v
┌─────────────────────────────────────────────┐
│ 2. Attach as Loop Device                    │
│    losetup -fP                              │
└────────────────┬────────────────────────────┘
                 │
                 v
┌─────────────────────────────────────────────┐
│ 3. Partition Disk                           │
│    fdisk / cfdisk / parted                  │
│    - Partition 1: 200MB, FAT16, bootable    │
│    - Partition 2: Remaining, ext4           │
└────────────────┬────────────────────────────┘
                 │
                 v
┌─────────────────────────────────────────────┐
│ 4. Format Partitions                        │
│    mkfs.vfat -F 16 -n boot                  │
│    mkfs.ext4 -L rootfs                      │
└────────────────┬────────────────────────────┘
                 │
                 v
┌─────────────────────────────────────────────┐
│ 5. Create Mount Points                      │
│    mkdir /mnt/boot /mnt/rootfs              │
└────────────────┬────────────────────────────┘
                 │
                 v
┌─────────────────────────────────────────────┐
│ 6. Mount Partitions                         │
│    mount /dev/loop0p1 /mnt/boot             │
│    mount /dev/loop0p2 /mnt/rootfs           │
└────────────────┬────────────────────────────┘
                 │
                 v
┌─────────────────────────────────────────────┐
│ 7. Copy Files to Partitions                 │
│    - Boot files to /mnt/boot                │
│    - Root filesystem to /mnt/rootfs         │
└────────────────┬────────────────────────────┘
                 │
                 v
┌─────────────────────────────────────────────┐
│ 8. Unmount and Detach                       │
│    umount /mnt/boot /mnt/rootfs             │
│    losetup -d /dev/loop0                    │
└────────────────┬────────────────────────────┘
                 │
                 v
┌─────────────────────────────────────────────┐
│ 9. Write to Physical SD Card (Optional)     │
│    dd if=sd_card.img of=/dev/mmcblk0        │
└─────────────────────────────────────────────┘
```

---

## Key Concepts to Remember

1. **Loop devices** translate file I/O to block I/O, allowing files to act as disks
2. **`-P` flag** is critical for partition scanning when attaching loop devices
3. **FAT16/32** required for Raspberry Pi boot partition (GPU limitation)
4. **EXT4** provides better performance and features for Linux rootfs
5. **MBR** partition table lives in first 512 bytes with boot signature `0x55AA`
6. **Mount point** is just an empty directory that becomes the filesystem root
7. **Always unmount before detaching** loop devices to prevent data loss
8. **Block devices** support random access and filesystems
9. **Character devices** transfer data as sequential byte streams

---

**Created:** February 13, 2026  
**Lab:** Embedded Linux - Virtual SD Card Creation  
**Purpose:** Complete reference for creating bootable SD card images for Raspberry Pi