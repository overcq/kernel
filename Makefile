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
#===============================================================================
H_make_I_block_root = $(if $(filter 0,$(shell id -u)),$(error root user not allowed. Run make as user first.))
#===============================================================================
all: build
build: kernel
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PHONY: all build mostlyclean clean install-qemu install-vmware install-usb
.SECONDARY: $(patsubst %.S,%.o,interrupt.S) \
I_compile_S_0.h \
$(patsubst %.cx,I_compile_S_0_%.h,$(wildcard *.cx)) \
$(patsubst %.cx,I_compile_S_0_%.c,$(wildcard *.cx))
#===============================================================================
kernel: I_compile_S_0.h \
$(patsubst %.S,%.o,interrupt.S) \
$(patsubst %.cx,I_compile_S_0_%.h,$(wildcard *.cx)) \
simple.h \
$(patsubst %.cx,I_compile_S_0_%.c,$(wildcard *.cx)) \
main.ld \
Makefile
	$(CC) $(CFLAGS) -std=gnu23 -march=native -mno-sse -mno-red-zone -fno-zero-initialized-in-bss -ffreestanding -fno-stack-protector -fwrapv -Wall -Wextra -Wno-address-of-packed-member -Wno-dangling-else -Wno-incompatible-pointer-types-discards-qualifiers -Wno-parentheses -Wno-sign-compare -Wno-switch -include stdarg.h -include I_compile_S_0.h -nostdlib -shared -s -Wl,-T,main.ld -o $@.elf $(filter %.o,$^) $(filter %.c,$^)
	rm -f $@; elf2oux $@.elf
	rm $@.elf
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
I_compile_S_0.h: \
I_compile_N_c_to_h.sh \
$(wildcard *.cx)
	$(H_make_I_block_root)
	{   echo '#include "I_compile_S_machine.h"' ;\
        echo '#include "I_compile_S_language.h"' ;\
		./I_compile_N_c_to_h.sh -f $(patsubst %.cx,%,$(filter %.cx,$^)) ;\
        for header in $(patsubst %.cx,I_compile_S_0_%.h,$(filter-out main.cx,$(filter %.cx,$^))); do \
            echo "#include \"$${header}\"" ;\
        done ;\
        echo '#include "I_compile_S_0_main.h"' ;\
        echo '#include "simple.h"' ;\
    } > $@
I_compile_S_0_%.h: %.cx \
I_compile_N_c_to_h.sh
	$(H_make_I_block_root)
	{   ./I_compile_N_c_to_h.sh -h1 $< \
        && ./I_compile_N_c_to_h.sh -h2 $< \
        && ./I_compile_N_c_to_h.sh -h3 $< ;\
    } > $@
I_compile_S_0_%.c: %.cx \
I_compile_N_c_to_h.sh
	$(H_make_I_block_root)
	./I_compile_N_c_to_h.sh -c $< > $@
%.o: %.S
	$(CC) -c -o $@ $<
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
mostlyclean: $(wildcard *.cx)
	rm -f I_compile_S_0.h $(patsubst %.cx,I_compile_S_0_%.h,$^) $(patsubst %.cx,I_compile_S_0_%.c,$^) *.o
clean: mostlyclean
	rm -f kernel
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
	&& trap 'vmware-mount -d $$ocq_mnt' EXIT \
	&& vmware-mount -f /mnt/hgfs/kernel\ UEFI/kernel\ UEFI.vmdk $$ocq_mnt \
	&& trap 'vmware-mount -d $$ocq_mnt' EXIT \
	&& loopdev=$$( losetup -LPf --show $$ocq_mnt/flat ) \
	&& trap 'losetup -d $$loopdev && vmware-mount -d $$ocq_mnt' EXIT \
	&& install/a.out kernel $${loopdev}p2
install-virtualbox:
	ocq_mnt=/mnt/oth; \
	mkdir -p $$ocq_mnt \
	&& trap '$(VMWARE_DIR)/bin/vmware-mount -d $$ocq_mnt' EXIT \
	&& $(VMWARE_DIR)/bin/vmware-mount -f ~inc/.VirtualBox/Machines/boot\ UEFI/boot\ UEFI.vmdk $$ocq_mnt \
	&& trap '$(VMWARE_DIR)/bin/vmware-mount -d $$ocq_mnt' EXIT \
	&& loopdev=$$( losetup -LPf --show $$ocq_mnt/flat ) \
	&& trap 'losetup -d $$loopdev && $(VMWARE_DIR)/bin/vmware-mount -d $$ocq_mnt' EXIT \
	&& install/a.out kernel $${loopdev}p2
#-------------------------------------------------------------------------------
install-usb:
	ocq_usb_dev=/dev/sdb; \
	ocq_usb_mnt=/mnt/usb; \
	loopdev=$$( losetup -Lf --show $${ocq_usb_dev}2 ); \
	trap 'losetup -d $$loopdev' EXIT \
	&& install/a.out kernel $$loopdev
#*******************************************************************************
