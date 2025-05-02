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
int some_export_variable;
int some_export_bss = 0;
void
some_export_procedure( void
){
}
//==============================================================================
__attribute (( __noreturn__ ))
void
main( struct E_main_Z_kernel_args *kernel_args
){  S status = kernel_args->uefi_runtime_services.reset_system( H_uefi_Z_reset_Z_shutdown, 0, 0, 0 );
End:__asm__ volatile (
    "\n0:"  "hlt"
    "\n"    "jmp    0b"
    );
    __builtin_unreachable();
}
/******************************************************************************/
