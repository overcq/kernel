#*******************************************************************************
#   ___   public
#  ¦OUX¦  GNU “make”
#  ¦/C+¦  OUX/C+ OS
#   ---   kernel
#         makefile
# ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒5‒2 K
#*******************************************************************************
include ../boot/env.mk
#===============================================================================
CC := clang
CFLAGS := -Oz
#===============================================================================
H_make_I_block_root = $(if $(filter 0,$(shell id -u)),$(error root user not allowed. Run make as user first.))
#===============================================================================
all: build
build: kernel doc
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.PHONY: all build doc mostlyclean clean install-qemu install-vmware install-usb
.SECONDARY: $(patsubst %.S,%.o,interrupt.S) \
I_compile_S_0.h \
$(patsubst %.cx,I_compile_S_0_%.h,$(wildcard *.cx)) \
$(patsubst %.cx,I_compile_S_1_%.h,$(wildcard *.cx)) \
$(patsubst %.cx,I_compile_S_0_%.c,$(wildcard *.cx))
#===============================================================================
kernel: I_compile_S_0.h \
$(patsubst %.S,%.o,interrupt.S) \
$(patsubst %.cx,I_compile_S_0_%.h,$(wildcard *.cx)) \
$(patsubst %.cx,I_compile_S_1_%.h,$(wildcard *.cx)) \
simple.h \
$(patsubst %.cx,I_compile_S_0_%.c,$(wildcard *.cx)) \
main.ld \
Makefile
	$(CC) $(CFLAGS) -std=c23 -mcmodel=large -march=x86-64 -mno-red-zone -ffreestanding -fno-asynchronous-unwind-tables -fno-stack-protector -fno-unwind-tables -fno-zero-initialized-in-bss -fwrapv -Wall -Wextra -Wno-address-of-packed-member -Wno-dangling-else -Wno-incompatible-pointer-types-discards-qualifiers -Wno-missing-braces -Wno-sign-compare -Wno-switch -include stdarg.h -include I_compile_S_0.h -nostdlib -fPIC -shared -s -Wl,-T,main.ld -o $@.elf $(filter %.o,$^) $(filter %.c,$^) \
    && rm $@ ;\
    elf2oux $@.elf $@ \
    && rm $@.elf
doc: $(patsubst %.dot,%.svg,$(wildcard doc/*.dot))
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
        for header in $(patsubst %.cx,I_compile_S_1_%.h,$(filter-out main.cx,$(filter %.cx,$^))); do \
            echo "#include \"$${header}\"" ;\
        done ;\
        echo '#include "I_compile_S_0_main.h"' ;\
        echo '#include "I_compile_S_1_main.h"' ;\
        echo '#include "simple.h"' ;\
    } > $@
I_compile_S_0_%.h: %.cx \
I_compile_N_c_to_h.sh
	$(H_make_I_block_root)
	{   ./I_compile_N_c_to_h.sh -h1 $< \
        && ./I_compile_N_c_to_h.sh -h2 $< ;\
    } > $@
I_compile_S_1_%.h: %.cx \
I_compile_N_c_to_h.sh
	$(H_make_I_block_root)
	./I_compile_N_c_to_h.sh -h3 $< > $@
I_compile_S_0_%.c: %.cx \
I_compile_N_c_to_h.sh
	$(H_make_I_block_root)
	./I_compile_N_c_to_h.sh -c $< > $@
%.o: %.S
	$(CC) -c -o $@ $<
#-------------------------------------------------------------------------------
%.svg: %.dot
	dot -Tsvg \
      -Gbgcolor=\#dbdbdb -Gcolor=black \
      -Ncolor=\#8b8b90 -Nfontname='Calibri, Ubuntu Condensed, sans-serif' -Nfontsize=11 \
      -Ecolor=\#8b8b90 -Efontname='Calibri, Ubuntu Condensed, sans-serif' -Efontsize=11 \
      -o $@ $<
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
mostlyclean: $(wildcard *.cx)
	-rm I_compile_S_0.h $(patsubst %.cx,I_compile_S_0_%.h,$^) $(patsubst %.cx,I_compile_S_1_%.h,$^) $(patsubst %.cx,I_compile_S_0_%.c,$^) *.o
clean: mostlyclean
	-rm kernel $(patsubst %.dot,%.svg,$(wildcard doc/*.dot))
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
install-virtualbox:
	trap 'vmware-mount -d $(H_ocq_S_mnt)' EXIT \
    && vmware-mount -f $(H_ocq_S_virtualbox_disk) $(H_ocq_S_mnt) \
    && loopdev=$$( losetup -LPf --show $(H_ocq_S_mnt)/flat ) \
    && trap 'losetup -d $$loopdev && vmware-mount -d $(H_ocq_S_mnt)' EXIT \
    && install/a.out kernel $${loopdev}p3
install-vmware:
	trap 'vmware-mount -d $(H_ocq_S_mnt)' EXIT \
    && vmware-mount -f $(H_ocq_S_vmware_disk) $(H_ocq_S_mnt) \
    && loopdev=$$( losetup -LPf --show $(H_ocq_S_mnt)/flat ) \
    && trap 'losetup -d $$loopdev && vmware-mount -d $(H_ocq_S_mnt)' EXIT \
    && install/a.out kernel $${loopdev}p3
install-qemu:
	loopdev=$$( losetup -LPf --show ../boot/disk.img ) \
    && trap 'losetup -d $$loopdev' EXIT \
    && install/a.out kernel $${loopdev}p3
#-------------------------------------------------------------------------------
install-usb:
	loopdev=$$( losetup -Lf --show $(H_ocq_S_usb_dev)3 ); \
    trap 'losetup -d $$loopdev' EXIT \
    && install/a.out kernel $$loopdev
#*******************************************************************************
