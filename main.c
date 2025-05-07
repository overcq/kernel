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
#define E_cpu_Z_page_entry_S_present    ( 1ULL << 0 )
#define E_cpu_Z_page_entry_S_write      ( 1ULL << 1 )
#define E_cpu_Z_gdt_Z_data_S_write      ( 1ULL << ( 32 + 9 ))
#define E_cpu_Z_gdt_Z_type_S_code       ( 1ULL << ( 32 + 11 ))
#define E_cpu_Z_gdt_S_code_data         ( 1ULL << ( 32 + 12 ))
#define E_cpu_Z_gdt_S_present           ( 1ULL << ( 32 + 15 ))
#define E_cpu_Z_gdt_Z_code_S_64bit      ( 1ULL << ( 32 + 21 ))
#define E_cpu_Z_gdt_S_granularity       ( 1ULL << ( 32 + 23 ))
#define E_cpu_Z_gdt_Z_type_S_ldt        ( 1ULL << ( 32 + 9 ))
//==============================================================================
struct E_main_Z_kernel_args
{ struct E_mem_blk_Z mem_blk;
  struct E_mem_Z_memory_map *memory_map;
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
_private struct E_main_Z_kernel E_main_S_kernel;
//==============================================================================
_private
__attribute__ (( __noreturn__ ))
void
main( struct E_main_Z_kernel_args *kernel_args
){  E_main_S_kernel.mem_blk = kernel_args->mem_blk;
    E_main_S_kernel.memory_map = kernel_args->memory_map;
    E_main_S_kernel.memory_map_n = kernel_args->memory_map_n;
    E_main_S_kernel.kernel = kernel_args->kernel;
    E_main_S_kernel.page_table = kernel_args->page_table;
    E_main_S_kernel.uefi_runtime_services = kernel_args->uefi_runtime_services;
    E_main_S_kernel.acpi.dsdt_content = kernel_args->acpi.dsdt_content;
    E_main_S_kernel.acpi.dsdt_content_l = kernel_args->acpi.dsdt_content_l;
    E_main_S_kernel.acpi.facs = kernel_args->acpi.facs;
    Mt_( E_main_S_kernel.stack, 1 );
    if( !E_main_S_kernel.stack )
        goto End;
    E_main_S_kernel.stack[0] = kernel_args->kernel_stack;
#define E_main_J_code_descriptor( base, limit ) (( (N)(limit) & (( 1 << 16 ) - 1 )) | (( (N)(base) & (( 1 << 24 ) - 1 )) << 16 ) | E_cpu_Z_gdt_Z_type_S_code | E_cpu_Z_gdt_S_code_data | E_cpu_Z_gdt_S_present | E_cpu_Z_gdt_Z_code_S_64bit | E_cpu_Z_gdt_S_granularity | ((( (N)(limit) >> 16 ) & (( 1 << 4 ) - 1 )) << ( 32 + 16 )) | (( (N)(base) >> 24 ) << ( 32 + 24 )))
#define E_main_J_data_descriptor( base, limit ) (( (N)(limit) & (( 1 << 16 ) - 1 )) | (( (N)(base) & (( 1 << 24 ) - 1 )) << 16 ) | E_cpu_Z_gdt_Z_data_S_write | E_cpu_Z_gdt_S_code_data | E_cpu_Z_gdt_S_present | E_cpu_Z_gdt_S_granularity | ((( (N)(limit) >> 16 ) & (( 1 << 4 ) - 1 )) << ( 32 + 16 )) | (( (N)(base) >> 24 ) << ( 32 + 24 )))
#define E_main_J_local_descriptor_1( base, limit ) (( (N)(limit) & (( 1 << 16 ) - 1 )) | (( (N)(base) & (( 1 << 24 ) - 1 )) << 16 ) | E_cpu_Z_gdt_Z_type_S_ldt | E_cpu_Z_gdt_S_present | ((( (N)(limit) >> 16 ) & (( 1 << 4 ) - 1 )) << ( 32 + 16 )) | (( (N)(base) >> 24 ) << ( 32 + 24 )))
    E_main_S_kernel.gdt[1] = E_main_J_code_descriptor( 0, ~0ULL );
    E_main_S_kernel.gdt[2] = E_main_J_data_descriptor( 0, ~0ULL );
    E_main_S_kernel.gdt[3] = E_main_J_local_descriptor_1( (N)&E_main_S_kernel.ldt[0], sizeof( E_main_S_kernel.ldt ) - 1 );
    E_main_S_kernel.gdt[4] = (N)&E_main_S_kernel.ldt[0] >> 32;
    E_main_S_kernel.ldt[0] = 0;
    E_main_S_kernel.ldt[1] = 0;
    _0( &E_main_S_kernel.idt[0], sizeof( E_main_S_kernel.idt ));
    struct __attribute__ ((packed))
    { N32 pad_1;
      N16 pad_2;
      N16 limit;
      N base;
    }gd, id;
    gd.base = (N)&E_main_S_kernel.gdt[0];
    gd.limit = sizeof( E_main_S_kernel.gdt ) - 1;
    id.base = (N)&E_main_S_kernel.idt[0];
    id.limit = sizeof( E_main_S_kernel.idt ) - 1;
    __asm__ volatile (
    "\n"    "lgdt   %0"
    "\n"    "mov    $3 << 3,%%ax"
    "\n"    "lldt   %%ax"
    "\n"    "lidt   %1"
    "\n"    "mov    $2 << 3,%%ax"
    "\n"    "mov    %%ax,%%ds"
    "\n"    "mov    %%ax,%%es"
    "\n"    "mov    %%ax,%%ss"
    "\n"    "mov    %%ax,%%fs"
    "\n"    "mov    %%ax,%%gs"
    "\n"    "movw   $1 << 3,-8(%%rsp)"
    "\n"    "leaq   0f(%%rip),%%rax"
    "\n"    "movq   %%rax,-16(%%rsp)"
    "\n"    ".byte  0x48"
    "\n"    "ljmp   *-16(%%rsp)"
    "\n0:"
    :
    : "p" ( &gd.limit ), "p" ( &id.limit )
    : "rax"
    );
    
    W( kernel_args->bootloader );
    
    S status = E_main_S_kernel.uefi_runtime_services.reset_system( H_uefi_Z_reset_Z_shutdown, 0, 0, 0 );
End:__asm__ volatile (
    "\n0:"  "hlt"
    "\n"    "jmp    0b"
    );
    __builtin_unreachable();
}
/******************************************************************************/
