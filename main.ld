OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(main)
SECTIONS
{ . = SEGMENT_START("rodata-segment", ALIGN(CONSTANT (MAXPAGESIZE)) + SIZEOF_HEADERS);
  .got : { *(.got) *(.igot) }
  /*. = DATA_SEGMENT_RELRO_END (SIZEOF (.got.plt) >= 24 ? 24 : 0, .);*/
  .got.plt : { *(.got.plt) *(.igot.plt) }
  . = SEGMENT_START("text-segment", ALIGN(CONSTANT (MAXPAGESIZE)));
  .text :
  { *(.ltext.hot.*)
    *(.text .ltext)
  }
  . = DATA_SEGMENT_ALIGN(CONSTANT (MAXPAGESIZE), CONSTANT (COMMONPAGESIZE));
  .data :
  { *(.data .ldata .ldata.*)
    *(.rodata .lrodata .rodata.* .lrodata.*)
  }
  /*. = SEGMENT_START("rodata-segment", 0);*/
  .dynamic : { *(.dynamic) }
  .dynstr : { *(.dynstr) }
  .rela.dyn :
  { *(.rela.text .rela.text.*)
    *(.rela.rodata .rela.rodata.*)
    *(.rela.data .rela.data.*)
    *(.rela.tdata .rela.tdata.*)
    *(.rela.got)
    *(.rela.ldata .rela.ldata.*)
    *(.rela.lrodata .rela.lrodata.*)
    *(.rela.ifunc)
  }
  .rela.plt :
  { *(.rela.plt)
    *(.rela.iplt)
  }
  .relr.dyn : { *(.relr.dyn) }
  .note.gnu.build-id : { *(.note.gnu.build-id) }
  .note.gnu.property : { *(.note.gnu-property) }
}
