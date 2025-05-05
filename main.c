/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         main
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒5‒1 a
*******************************************************************************/
#include "main.h"
//==============================================================================
struct E_main_Z_kernel_args
{ struct E_mem_blk_Z mem_blk;
  struct H_oux_E_mem_Z_memory_map *memory_map;
  N memory_map_n;
  P bootloader;
  P kernel;
  P page_table;
  P kernel_stack;
  struct H_main_Z_uefi_runtime_services uefi_runtime_services;
  struct
  { P dsdt_content;
    N dsdt_content_l;
    P facs;
  }acpi;
};
//==============================================================================
__attribute__ (( __noreturn__ ))
_private
void
main( struct E_main_Z_kernel_args *kernel_args
){  S status = kernel_args->uefi_runtime_services.reset_system( H_uefi_Z_reset_Z_shutdown, 0, 0, 0 );
    __asm__ volatile (
    "\n0:"  "hlt"
    "\n"    "jmp    0b"
    );
    __builtin_unreachable();
}
/******************************************************************************/
