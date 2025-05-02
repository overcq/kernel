#*******************************************************************************
#   ___   public
#  ¦OUX¦  GNU “make”
#  ¦/C+¦  OUX/C+ OS
#   ---   kernel
#         makefile
# ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒5‒2 K
#*******************************************************************************
CC := clang
CFLAGS := -Os
VMWARE_DIR := /opt/vmware2
#===============================================================================
all: build
build: kernel
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PHONY: all build clean distclean install-qemu install-vmware install-usb
#===============================================================================
kernel: main.h main.c main.ld Makefile
	$(CC) $(CFLAGS) -fno-zero-initialized-in-bss -ffreestanding -fstack-clash-protection -fwrapv -Wall -Wextra -Wno-address-of-packed-member -Wno-dangling-else -Wno-parentheses -Wno-pointer-sign -Wno-uninitialized -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-unused-variable -nostdlib -Wl,-T,main.ld -o $@.elf $(filter %.c,$^)
	elf2oux $@.elf || rm -f $@
	rm $@.elf
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
clean:
	rm -f kernel.elf kernel
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
install-qemu:
	ocq_mnt=/mnt/oth; \
	mkdir -p $$ocq_mnt \
	|| exit 1; \
	loopdev=$$( losetup -LPf --show ../boot/disk.img ); \
	trap 'losetup -d $$loopdev' EXIT \
	&& mount $${loopdev}p2 $$ocq_mnt \
	&& trap 'losetup -d $$loopdev && umount $$ocq_mnt' EXIT \
	&& mkdir -p $$ocq_mnt/EFI/BOOT \
	&& install kernel $$ocq_mnt/system
install-vmware:
	ocq_mnt=/mnt/oth; \
	mkdir -p $$ocq_mnt \
	&& trap '$(VMWARE_DIR)/bin/vmware-mount -d $$ocq_mnt' EXIT \
	&& $(VMWARE_DIR)/bin/vmware-mount ~inc/vmware/boot\ UEFI/boot\ UEFI.vmdk 2 $$ocq_mnt \
	|| exit 1; \
	mkdir -p $$ocq_mnt/EFI/BOOT \
	&& install kernel $$ocq_mnt/system
#-------------------------------------------------------------------------------
install-usb:
	ocq_usb_dev=/dev/sdc; \
	ocq_usb_mnt=/mnt/usb; \
	mount $${ocq_usb_dev}2 $$ocq_usb_mnt \
	|| exit 1; \
	trap 'umount $$ocq_usb_mnt' EXIT; \
	mkdir -p $$ocq_usb_mnt/EFI/BOOT \
	&& install kernel $$ocq_usb_mnt/system
#*******************************************************************************
