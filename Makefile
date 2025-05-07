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
#NDFN Nie wiadomo, dlaczego SSE nie może być włączone.
kernel: simple.h main.h main.c mem-blk.c main.ld Makefile
	$(CC) $(CFLAGS) -std=gnu23 -mno-sse -fno-zero-initialized-in-bss -ffreestanding -fno-stack-protector -fwrapv -Wall -Wextra -Wno-address-of-packed-member -Wno-dangling-else -Wno-parentheses -Wno-pointer-sign -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-unused-variable -Werror -nostdlib -shared -s -Wl,-dT,main.ld,--section-start=.note.gnu.property=0x8000000000000000 -o $@.elf $(filter %.c,$^)
	rm -f $@; elf2oux $@.elf
	rm $@.elf
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
clean:
	rm -f kernel.elf kernel
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
install-qemu:
	ocq_mnt=/mnt/oth; \
	mkdir -p $$ocq_mnt \
	&& loopdev=$$( losetup -LPf --show ../boot/UEFI/disk.img ) \
	&& trap 'losetup -d $$loopdev' EXIT \
	&& install/a.out kernel $${loopdev}p2
install-vmware:
	ocq_mnt=/mnt/oth; \
	mkdir -p $$ocq_mnt \
	&& trap '$(VMWARE_DIR)/bin/vmware-mount -d $$ocq_mnt' EXIT \
	&& $(VMWARE_DIR)/bin/vmware-mount -f ~inc/vmware/boot\ UEFI/boot\ UEFI.vmdk $$ocq_mnt \
	&& trap '$(VMWARE_DIR)/bin/vmware-mount -d $$ocq_mnt' EXIT \
	&& loopdev=$$( losetup -LPf --show $$ocq_mnt/flat ) \
	&& trap 'losetup -d $$loopdev && $(VMWARE_DIR)/bin/vmware-mount -d $$ocq_mnt' EXIT \
	&& install/a.out kernel $${loopdev}p2
#-------------------------------------------------------------------------------
install-usb:
	ocq_usb_dev=/dev/sdc; \
	ocq_usb_mnt=/mnt/usb; \
	loopdev=$$( losetup -Lf --show $${ocq_usb_dev}2 ); \
	trap 'losetup -d $$loopdev' EXIT \
	&& install/a.out kernel $$loopdev
#*******************************************************************************
