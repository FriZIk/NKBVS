cd ~/NKB/coreboot/external/SeaBIOS/seabios
make
cd ~/NKB/coreboot
make menuconfig
make
qemu-system-x86_64 -bios build/coreboot.rom -serial stdio

https://www.coreboot.org/downloads.html
