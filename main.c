/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         main
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒5‒1 a
*******************************************************************************/
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
struct E_mem_Z_memory_map
{ N physical_start;
  N virtual_start;
  N pages;
};
struct E_main_Z_pixel_shifts
{ N8 red;
  N8 green;
  N8 blue;
};
enum E_uefi_Z_pixel_format
{ H_uefi_Z_pixel_format_S_rgb8
, H_uefi_Z_pixel_format_S_bgr8
, H_uefi_Z_pixel_format_S_bitmask
};
struct E_main_Z_framebuffer
{ volatile N32 *p;
  N32 width, height;
  N32 pixels_per_scan_line;
  enum E_uefi_Z_pixel_format pixel_format;
  struct E_main_Z_pixel_shifts pixel_shifts;
};
struct E_main_Z_uefi_runtime_services
{ S ( H_uefi_Z_api __attribute__ (( __warn_unused_result__ )) *R_time )( struct H_uefi_Z_time *time, struct H_uefi_Z_time_capabilities *capabilities );
  S ( H_uefi_Z_api __attribute__ (( __warn_unused_result__ )) *P_time )( struct H_uefi_Z_time *time );
  S ( H_uefi_Z_api __attribute__ (( __warn_unused_result__ )) *R_wakeup_time )( B *enabled, B *pending, struct H_uefi_Z_time *time );
  S ( H_uefi_Z_api __attribute__ (( __warn_unused_result__ )) *P_wakeup_time )( B enable, struct H_uefi_Z_time *time );
  S ( H_uefi_Z_api __attribute__ (( __warn_unused_result__ )) *R_variable )( Pc16 name, struct H_uefi_Z_guid *vendor_guid, N32 attrbutes, N *data_l, P *data );
  S ( H_uefi_Z_api __attribute__ (( __warn_unused_result__ )) *R_next_variable_name )( N *name_l, Pc16 name, struct H_uefi_Z_guid *vendor_guid );
  S ( H_uefi_Z_api __attribute__ (( __warn_unused_result__ )) *P_variable )( Pc16 name, struct H_uefi_Z_guid *guid, N32 attributes, N data_l, P data );
  S ( H_uefi_Z_api __attribute__ (( __warn_unused_result__ )) *R_next_high_monotonic_count )( N32 *count );
  S ( H_uefi_Z_api __attribute__ (( __warn_unused_result__ )) *reset_system )( enum H_uefi_Z_reset type, N status, N data_l, Pc16 data );
  S ( H_uefi_Z_api __attribute__ (( __warn_unused_result__ )) *update_capsule )( struct H_uefi_Z_capsule_header **capsule_headers, N capsule_headers_n, N64 scatter_gather_list );
  S ( H_uefi_Z_api __attribute__ (( __warn_unused_result__ )) *R_capsule_capabilities )( struct H_uefi_Z_capsule_header **capsule_headers, N capsule_headers_n, N64 *maximum_capsule_size, enum H_uefi_Z_reset *reset_type );
  S ( H_uefi_Z_api __attribute__ (( __warn_unused_result__ )) *R_variable_info )( N32 attributes, N64 *maximum_variable_storage_size, N64 *remaining_variable_storage_size, N64 *maximum_variable_size );
};
struct E_main_Z_kernel_Z_acpi
{ P dmar_content;
  N dmar_content_l;
  P facs;
  struct H_oux_Z_hpet hpet;
  struct H_acpi_Z_mcfg_entry *mcfg_content;
  N mcfg_content_n;
  struct
  { P address;
    N l;
  }ssdt_contents[2];
  N ssdt_contents_n;
  unsigned virt_guest_rtc_good                :1;
  unsigned virt_guest_pm_good                 :1;
  unsigned smm_validate_fixed_comm_buffers    :1;
  unsigned smm_validate_nested_ptr            :1;
  unsigned smm_system_resource_protection     :1;
};
_private
struct E_main_Z_kernel
{ struct E_mem_blk_Z mem_blk;
  struct E_mem_Z_memory_map *memory_map;
  N memory_map_n;
  N gdt[5], ldt[2];
  P kernel;
  P page_table;
  P *stack;
  struct E_main_Z_framebuffer framebuffer;
  struct E_main_Z_uefi_runtime_services uefi_runtime_services;
  struct E_main_Z_kernel_Z_acpi acpi;
}E_main_S_kernel;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct E_main_Z_kernel_args_Z_acpi
{ P dmar_content;
  N dmar_content_l;
  P dsdt_content;
  N dsdt_content_l;
  P facs;
  struct H_oux_Z_hpet hpet;
  struct H_acpi_Z_mcfg_entry *mcfg_content;
  N mcfg_content_n;
  struct
  { P address;
    N l;
  }ssdt_contents[2];
  N ssdt_contents_n;
  unsigned virt_guest_rtc_good                :1;
  unsigned virt_guest_pm_good                 :1;
  unsigned smm_validate_fixed_comm_buffers    :1;
  unsigned smm_validate_nested_ptr            :1;
  unsigned smm_system_resource_protection     :1;
};
struct E_main_Z_kernel_args
{ struct E_mem_blk_Z mem_blk;
  struct E_mem_Z_memory_map *memory_map;
  N memory_map_n;
  P bootloader;
  P kernel;
  P page_table;
  P kernel_stack;
  struct E_main_Z_framebuffer framebuffer;
  struct E_main_Z_uefi_runtime_services uefi_runtime_services;
  struct E_main_Z_kernel_args_Z_acpi acpi;
  P local_apic_address;
  P io_apic_address;
  struct E_interrupt_Z_gsi *gsi;
  N8 gsi_n;
};
//==============================================================================
_private
P
E_main_Z_p_I_to_virtual( P p
){  N min = 0;
    N max = E_main_S_kernel.memory_map_n - 1;
    N i = max / 2;
    O{  if( (N)p >= E_main_S_kernel.memory_map[i].physical_start
        && (N)p < E_main_S_kernel.memory_map[i].physical_start + E_main_S_kernel.memory_map[i].pages * E_mem_S_page_size
        )
            return (P)( E_main_S_kernel.memory_map[i].virtual_start + ( (N)p - E_main_S_kernel.memory_map[i].physical_start ));
        if( E_main_S_kernel.memory_map[i].physical_start > (N)p )
        {   if( i == min )
                break;
            max = i - 1;
            i = max - ( i - min ) / 2;
        }else
        {   if( i == max )
                break;
            min = i + 1;
            i = min + ( max - i ) / 2;
        }
    }
    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_private
__attribute__ (( __noreturn__ ))
void
main( struct E_main_Z_kernel_args *kernel_args
){  E_main_S_kernel.mem_blk = kernel_args->mem_blk;
    E_main_S_kernel.memory_map = kernel_args->memory_map;
    E_main_S_kernel.memory_map_n = kernel_args->memory_map_n;
    E_main_S_kernel.kernel = kernel_args->kernel;
    E_main_S_kernel.page_table = kernel_args->page_table;
    E_main_S_kernel.framebuffer = kernel_args->framebuffer;
    E_main_S_kernel.uefi_runtime_services = kernel_args->uefi_runtime_services;
    //E_main_S_kernel.acpi = kernel_args->acpi;
    E_interrupt_Q_local_apic_S_address = kernel_args->local_apic_address;
    E_interrupt_Q_io_apic_S_address = kernel_args->io_apic_address;
    E_interrupt_S_gsi = kernel_args->gsi;
    E_interrupt_S_gsi_n = kernel_args->gsi_n;
    if( !~E_font_M() )
        goto End;
    E_vga_I_fill_rect( 0, 0, E_main_S_kernel.framebuffer.width, E_main_S_kernel.framebuffer.height, E_vga_R_video_color( E_vga_S_background_color ));
    E_vga_I_fill_rect( E_main_S_kernel.framebuffer.width / 2 - 50, E_main_S_kernel.framebuffer.height / 2 - 10 - 13, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_kernel.framebuffer.width / 2 - 50, E_main_S_kernel.framebuffer.height / 2 - 10, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_kernel.framebuffer.width / 2, E_main_S_kernel.framebuffer.height / 2 + 4, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_kernel.framebuffer.width / 2, E_main_S_kernel.framebuffer.height / 2 + 4 + 13, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_kernel.framebuffer.width / 2 - 38, E_main_S_kernel.framebuffer.height / 2 - 37, 38 + 34, 37 + 36, E_vga_R_video_color( 0x43864f ));
    E_vga_I_fill_rect( E_main_S_kernel.framebuffer.width / 2 - 50, E_main_S_kernel.framebuffer.height / 2 + 4, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_kernel.framebuffer.width / 2 - 50, E_main_S_kernel.framebuffer.height / 2 + 4 + 13, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_kernel.framebuffer.width / 2, E_main_S_kernel.framebuffer.height / 2 - 10 - 13, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_kernel.framebuffer.width / 2, E_main_S_kernel.framebuffer.height / 2 - 10, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_font_I_print( "OUX/C+ OS. ©overcq <overcq@int.pl>. https:/""/github.com/overcq\n" );
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
    struct __attribute__ ((packed))
    { N32 pad_1;
      N16 pad_2;
      N16 limit;
      N base;
    }gd;
    gd.base = (N)&E_main_S_kernel.gdt[0];
    gd.limit = sizeof( E_main_S_kernel.gdt ) - 1;
    __asm__ volatile (
    "\n"    "lgdt   %0"
    "\n"    "mov    $3 << 3,%%ax"
    "\n"    "lldt   %%ax"
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
    : "p" ( &gd.limit )
    : "rax"
    );
    if( !~E_interrupt_M() )
        goto End;
    if( !~E_acpi_aml_M( kernel_args->acpi.dsdt_content, kernel_args->acpi.dsdt_content_l ))
    {   E_font_I_print( "\nAML error" );
        goto End;
    }
    if( !~E_acpi_reader_M() )
        goto End;

    N allocated_i = E_mem_Q_blk_R( kernel_args->bootloader );
    if( !E_mem_Q_blk_I_remove( &kernel_args->bootloader, E_mem_S_page_size, E_main_S_kernel.mem_blk.allocated[ allocated_i ].n - E_mem_S_page_size )) // Pozostawienie jednej strony pamięci z ‘identity mapping’ na program ‘wakeup’ procesorów.
        goto End;

    W( kernel_args->bootloader );
    //S status = E_main_S_kernel.uefi_runtime_services.reset_system( H_uefi_Z_reset_Z_shutdown, 0, 0, 0 );
End:__asm__ volatile (
    "\n"    "cli"
    "\n0:"  "hlt"
    "\n"    "jmp    0b"
    );
    __builtin_unreachable();
}
/******************************************************************************/
