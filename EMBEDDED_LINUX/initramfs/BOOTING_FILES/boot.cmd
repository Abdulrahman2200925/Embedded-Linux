echo "=== Embedded Linux Boot - Intake 46 ==="
fatload mmc 0:1 ${kernel_addr_r} Image
fatload mmc 0:1 ${fdt_addr_r} bcm2710-rpi-3-b-plus.dtb
fatload mmc 0:1 0x02700000 rootramfs.cpio.gz
setenv bootargs "console=ttyS0,115200 8250.nr_uarts=1 rdinit=/init loglevel=8"
booti ${kernel_addr_r} 0x02700000:${filesize} ${fdt_addr_r}
