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
	&& install/a.out kernel $${loopdev}p2
install-vmware:
	ocq_mnt=/mnt/oth; \
	mkdir -p $$ocq_mnt \
	&& trap '$(VMWARE_DIR)/bin/vmware-mount -d $$ocq_mnt' EXIT \
	&& $(VMWARE_DIR)/bin/vmware-mount -f ~inc/vmware/boot\ UEFI/boot\ UEFI.vmdk $$ocq_mnt \
	&& trap 'losetup -d $$loopdev && $(VMWARE_DIR)/bin/vmware-mount -d $$ocq_mnt' EXIT \
	&& loopdev=$$( losetup -LPf --show $$ocq_mnt/flat ) \
	&& install/a.out kernel $${loopdev}p2
#-------------------------------------------------------------------------------
install-usb:
	ocq_usb_dev=/dev/sdc; \
	ocq_usb_mnt=/mnt/usb; \
	loopdev=$$( losetup -Lf --show $${ocq_usb_dev}2 ); \
	trap 'losetup -d $$loopdev' EXIT \
	&& install/a.out kernel $$loopdev
#*******************************************************************************
