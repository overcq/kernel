OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(main)
SECTIONS
{
  . = SEGMENT_START("rodata-segment", ALIGN(CONSTANT (MAXPAGESIZE)) + SIZEOF_HEADERS);
  .got : { *(.got) *(.igot) }
  /*. = DATA_SEGMENT_RELRO_END (SIZEOF (.got.plt) >= 24 ? 24 : 0, .);*/
  .got.plt : { *(.got.plt) *(.igot.plt) }
  . = SEGMENT_START("text-segment", ALIGN(CONSTANT (MAXPAGESIZE)));
  .text :
  {
    *(.plt) *(.iplt)
    *(.plt.sec)
    *(.text.unlikely .text.*_unlikely .text.unlikely.*)
    *(.text.exit .text.exit.*)
    *(.text.startup .text.startup.*)
    *(.text.hot .text.hot.*)
    *(SORT(.text.sorted.*))
    *(.text .stub .text.* .gnu.linkonce.t.*)
  }
  . = DATA_SEGMENT_ALIGN (CONSTANT (MAXPAGESIZE), CONSTANT (COMMONPAGESIZE));
  .data :
  {
    *(.data .data.* .gnu.linkonce.d.*)
    *(.rodata .rodata.* .gnu.linkonce.r.*)
  }
  /*. = SEGMENT_START("rodata-segment", 0);*/
  .dynamic : { *(.dynamic) }
  .dynstr : { *(.dynstr) }
  .rela.dyn :
    {
      *(.rela.text .rela.text.* .rela.gnu.linkonce.t.*)
      *(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*)
      *(.rela.data .rela.data.* .rela.gnu.linkonce.d.*)
      *(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*)
      *(.rela.got)
      *(.rela.ldata .rela.ldata.* .rela.gnu.linkonce.l.*)
      *(.rela.lrodata .rela.lrodata.* .rela.gnu.linkonce.lr.*)
      *(.rela.ifunc)
    }
  .rela.plt :
    {
      *(.rela.plt)
      PROVIDE_HIDDEN (__rela_iplt_start = .);
      *(.rela.iplt)
      PROVIDE_HIDDEN (__rela_iplt_end = .);
    }
  .relr.dyn : { *(.relr.dyn) }
  .note.gnu-property : { *(.note.gnu-property) }
}
