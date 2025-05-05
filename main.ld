OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(main)
SECTIONS
{
  . = SEGMENT_START("text-segment", 0) + SIZEOF_HEADERS;
  . = ALIGN(CONSTANT (MAXPAGESIZE));
  .text :
  {
    *(.plt.sec)
    *(.text.unlikely .text.*_unlikely .text.unlikely.*)
    *(.text.exit .text.exit.*)
    *(.text.startup .text.startup.*)
    *(.text.hot .text.hot.*)
    *(SORT(.text.sorted.*))
    *(.text .stub .text.* .gnu.linkonce.t.*)
  }
  . = ALIGN(CONSTANT (MAXPAGESIZE));
  .data :
  {
    *(.data .data.* .gnu.linkonce.d.*)
  }
  . = DATA_SEGMENT_ALIGN (CONSTANT (MAXPAGESIZE), CONSTANT (COMMONPAGESIZE));
  .got : { *(.got) *(.igot) }
  . = DATA_SEGMENT_RELRO_END (SIZEOF (.got.plt) >= 24 ? 24 : 0, .);
  .got.plt : { *(.got.plt) *(.igot.plt) }
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
