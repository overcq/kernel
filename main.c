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
#define E_cpu_Z_gdt_Z_type_S_tss        ( 9ULL << ( 32 + 8 ))
//==============================================================================
_private
struct E_mem_Z_memory_map
{ N physical_start;
  N virtual_start;
  N pages;
} *E_main_S_memory_map;
_private N E_main_S_memory_map_n;
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
_private
struct E_main_Z_framebuffer
{ volatile N32 *p;
  N32 width, height;
  N32 pixels_per_scan_line;
  enum E_uefi_Z_pixel_format pixel_format;
  struct E_main_Z_pixel_shifts pixel_shifts;
}E_main_S_framebuffer;
_private
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
}E_main_S_uefi_runtime_services;
_private
struct E_main_Z_kernel_Z_acpi
{ P dmar_content;
  N dmar_content_l;
  P facs;
  struct H_oux_Z_hpet hpet;
  struct H_acpi_Z_mcfg_entry *mcfg_content;
  N mcfg_content_n;
  unsigned virt_guest_rtc_good                :1;
  unsigned virt_guest_pm_good                 :1;
  unsigned smm_validate_fixed_comm_buffers    :1;
  unsigned smm_validate_nested_ptr            :1;
  unsigned smm_system_resource_protection     :1;
}E_main_S_acpi;
_private N *E_main_S_gdt;
_private N E_main_S_ldt[2];
_private P E_main_S_page_table;
_private P E_main_S_kernel;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct E_main_Z_kernel_args_Z_acpi
{ P dmar_content;
  N dmar_content_l;
  P dsdt_content;
  N dsdt_content_l;
  P facs;
  struct H_oux_Z_hpet hpet;
  struct
  { P address;
    N l;
  }ssdt_contents[4];
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
  N additional_pages;
  P kernel_stack;
  P pcie_base_address;
  struct E_main_Z_framebuffer framebuffer;
  struct E_main_Z_uefi_runtime_services uefi_runtime_services;
  struct E_main_Z_kernel_args_Z_acpi acpi;
  P local_apic_address;
  P io_apic_address;
  struct E_interrupt_Z_gsi *gsi;
  P *processor_proc;
  N32 processor_start_page;
  N32 processor_n;
  N8 gsi_n;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct __attribute__ (( __packed__ )) E_main_I_tss
{ N32 reserved_0;
  N rsp[3];
  N ist[8];
  N reserved_1;
  N16 reserved_2;
  N16 io_map_base_address;
};
struct __attribute__ ((packed)) E_main_Z_gd
{ N32 pad_1;
  N16 pad_2;
  N16 limit;
  N base;
};
_internal struct E_main_Z_gd E_main_S_gd;
_internal Pc *E_main_S_stack;
//==============================================================================
#define E_main_J_code_descriptor( base, limit ) (( (N)(limit) & (( 1 << 16 ) - 1 )) | (( (N)(base) & (( 1 << 24 ) - 1 )) << 16 ) | E_cpu_Z_gdt_Z_type_S_code | E_cpu_Z_gdt_S_code_data | E_cpu_Z_gdt_S_present | E_cpu_Z_gdt_Z_code_S_64bit | E_cpu_Z_gdt_S_granularity | ((( (N)(limit) >> 16 ) & (( 1 << 4 ) - 1 )) << ( 32 + 16 )) | (( (N)(base) >> 24 ) << ( 32 + 24 )))
#define E_main_J_data_descriptor( base, limit ) (( (N)(limit) & (( 1 << 16 ) - 1 )) | (( (N)(base) & (( 1 << 24 ) - 1 )) << 16 ) | E_cpu_Z_gdt_Z_data_S_write | E_cpu_Z_gdt_S_code_data | E_cpu_Z_gdt_S_present | E_cpu_Z_gdt_S_granularity | ((( (N)(limit) >> 16 ) & (( 1 << 4 ) - 1 )) << ( 32 + 16 )) | (( (N)(base) >> 24 ) << ( 32 + 24 )))
#define E_main_J_local_descriptor_low( base, limit ) (( (N)(limit) & (( 1 << 16 ) - 1 )) | (( (N)(base) & (( 1 << 24 ) - 1 )) << 16 ) | E_cpu_Z_gdt_Z_type_S_ldt | E_cpu_Z_gdt_S_present | ((( (N)(limit) >> 16 ) & (( 1 << 4 ) - 1 )) << ( 32 + 16 )) | (( (N)(base) >> 24 ) << ( 32 + 24 )))
#define E_main_J_task_descriptor_low( base, limit ) (( (N)(limit) & (( 1 << 16 ) - 1 )) | (( (N)(base) & (( 1 << 24 ) - 1 )) << 16 ) | E_cpu_Z_gdt_Z_type_S_tss | E_cpu_Z_gdt_S_present | ((( (N)(limit) >> 16 ) & (( 1 << 4 ) - 1 )) << ( 32 + 16 )) | (( (N)(base) >> 24 ) << ( 32 + 24 )))
//==============================================================================
_private
N8
E_main_I_in_8( N16 port
){  N8 v;
    __asm__ volatile (
    "\n"    "in     %1,%0"
    : "=a" (v)
    : "d" (port)
    );
    return v;
}
_private
void
E_main_I_out_8( N16 port
, N8 v
){  __asm__ volatile (
    "\n"    "out    %0,%1"
    :
    : "a" (v), "d" (port)
    );
}
_private
N32
E_main_I_in_32( N16 port
){  N32 v;
    __asm__ volatile (
    "\n"    "in     %1,%0"
    : "=a" (v)
    : "d" (port)
    );
    return v;
}
_private
void
E_main_I_out_32( N16 port
, N32 v
){  __asm__ volatile (
    "\n"    "out    %0,%1"
    :
    : "a" (v), "d" (port)
    );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_private
P
E_main_Z_p_I_to_virtual( P p
){  N min = 0;
    N max = E_main_S_memory_map_n - 1;
    N i = max / 2;
    O{  if( (N)p >= E_main_S_memory_map[i].physical_start
        && (N)p < E_main_S_memory_map[i].physical_start + E_main_S_memory_map[i].pages * E_mem_S_page_size
        )
            return (P)( E_main_S_memory_map[i].virtual_start + ( (N)p - E_main_S_memory_map[i].physical_start ));
        if( E_main_S_memory_map[i].physical_start > (N)p )
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
_private
P
E_main_Z_p_I_to_physical( P p
){  for_n( i, E_main_S_memory_map_n )
    {   if( (N)p >= E_main_S_memory_map[i].virtual_start
        && (N)p < E_main_S_memory_map[i].virtual_start + E_main_S_memory_map[i].pages * E_mem_S_page_size
        )
            return (P)( E_main_S_memory_map[i].physical_start + ( (N)p - E_main_S_memory_map[i].virtual_start ));
    }
    return 0;
}
_private
__attribute__ (( __noreturn__ ))
void
E_main_I_error_fatal( void
){  __asm__ volatile (
    "\n"    "cli"
    "\n0:"  "hlt"
    "\n"    "jmp    0b"
    );
    __builtin_unreachable();
}
_internal
__attribute__ (( __naked__ ))
void
E_main_I_processor_start( void
){  __asm__ volatile (
    "\n"    "mov    $0x802,%%ecx"
    "\n"    "rdmsr"
    "\n"    "dec    %%eax"
    "\n"    "mov    %%eax,%%ebx"
    "\n"    "mov    %0,%%rbp"
    "\n"    "mov    (%%rbp,%%rbx,8),%%rsp"
    "\n"    "lea    0x2000(%%rsp),%%rsp"
    "\n"    "lgdt   %1"
    "\n"    "mov    $3 << 3,%%ax"
    "\n"    "lldt   %%ax"
    "\n"    "mov    $2 << 3,%%ax"
    "\n"    "mov    %%ax,%%ds"
    "\n"    "mov    %%ax,%%es"
    "\n"    "mov    %%ax,%%ss"
    "\n"    "xor    %%ax,%%ax"
    "\n"    "mov    %%ax,%%fs"
    "\n"    "mov    %%ax,%%gs"
    "\n"    "movw   $1 << 3,-8(%%rsp)"
    "\n"    "lea    0f(%%rip),%%rax"
    "\n"    "mov    %%rax,-16(%%rsp)"
    "\n"    ".byte  0x48"
    "\n"    "ljmp   *-16(%%rsp)"
    "\n0:"  "lea    7(%%rbx,%%rbx),%%rax"
    "\n"    "shl    $3,%%rax"
    "\n"    "ltr    %%ax"
    "\n"    "lidt   %2"
    "\n"    "mov    $0x80f,%%ecx"
    "\n"    "rdmsr"
    "\n"    "or     $0x100,%%eax"
    "\n"    "mov    %3,%%al"
    "\n"    "lea    32(%%eax),%%eax"
    "\n"    "wrmsr"
    "\n"    "movzbq %4,%%rax"
    "\n"    "lea    32(%%rax),%%rax"
    "\n"    "mov    $0x832,%%ecx"
    "\n"    "wrmsr"
    "\n"    "mov    $0x83e,%%ecx"
    "\n"    "rdmsr"
    "\n"    "and    $~0xf,%%eax"
    "\n"    "or     $0xb,%%eax"
    "\n"    "wrmsr"
    "\n"    "sti"
    "\n"    "mov    (%%rbp,%%rbx,8),%%rdi"
    "\n"    "call   E_flow_M"
    "\n"    "test   %%rax,%%rax"
    "\n"    "jz     0f"
    "\n"    "jmp    E_main_I_error_fatal"
    "\n0:"  "mov    %%rax,(%%rbp,%%rbx,8)"
    "\n"    "jmp    E_flow_I_main_task"
    :
    : "m" ( E_main_S_stack ), "p" ( &E_main_S_gd.limit ), "p" ( &E_interrupt_S_id.limit ), "m" ( E_interrupt_S_gsi_n ), "m" ( E_interrupt_S_gsi_timer )
    : "rax", "rcx", "rdx", "rbx", "rbp", "rdi"
    );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
void
E_main_I_ipi_test( void
){  E_font_I_print( "\nipi" );
}
_private
__attribute__ (( __noreturn__ ))
void
main( struct E_main_Z_kernel_args *kernel_args
){  E_mem_blk_S = kernel_args->mem_blk;
    E_main_S_memory_map = kernel_args->memory_map;
    E_main_S_memory_map_n = kernel_args->memory_map_n;
    E_main_S_kernel = kernel_args->kernel;
    E_main_S_page_table = kernel_args->page_table;
    E_mem_stack_S_additional_pages = kernel_args->additional_pages;
    E_main_S_framebuffer = kernel_args->framebuffer;
    E_main_S_uefi_runtime_services = kernel_args->uefi_runtime_services;
    //E_main_S_acpi = kernel_args->acpi;
    E_interrupt_Q_local_apic_S_address = kernel_args->local_apic_address;
    E_interrupt_Q_io_apic_S_address = kernel_args->io_apic_address;
    E_interrupt_S_gsi = kernel_args->gsi;
    E_interrupt_S_gsi_n = kernel_args->gsi_n;
    E_mem_blk_S_mem_lock = 0;
    if( !~E_font_M() )
        goto End;
    E_vga_I_fill_rect( 0, 0, E_main_S_framebuffer.width, E_main_S_framebuffer.height, E_vga_R_video_color( E_vga_S_background_color ));
    E_vga_I_fill_rect( E_main_S_framebuffer.width / 2 - 50, E_main_S_framebuffer.height / 2 - 10 - 13, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_framebuffer.width / 2 - 50, E_main_S_framebuffer.height / 2 - 10, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_framebuffer.width / 2, E_main_S_framebuffer.height / 2 + 4, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_framebuffer.width / 2, E_main_S_framebuffer.height / 2 + 4 + 13, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_framebuffer.width / 2 - 38, E_main_S_framebuffer.height / 2 - 37, 38 + 34, 37 + 36, E_vga_R_video_color( 0x43864f ));
    E_vga_I_fill_rect( E_main_S_framebuffer.width / 2 - 50, E_main_S_framebuffer.height / 2 + 4, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_framebuffer.width / 2 - 50, E_main_S_framebuffer.height / 2 + 4 + 13, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_framebuffer.width / 2, E_main_S_framebuffer.height / 2 - 10 - 13, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_vga_I_fill_rect( E_main_S_framebuffer.width / 2, E_main_S_framebuffer.height / 2 - 10, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    E_font_I_print( "OUX/C+ OS. ©overcq <overcq@int.pl>. https:/""/github.com/overcq\n" );
    Mt_( E_main_S_gdt, 5 + kernel_args->processor_n * 2 );
    if( !E_main_S_gdt )
        goto End;
    E_main_S_gdt[1] = E_main_J_code_descriptor( 0, ~0ULL );
    E_main_S_gdt[2] = E_main_J_data_descriptor( 0, ~0ULL );
    E_main_S_gdt[3] = E_main_J_local_descriptor_low( (N)&E_main_S_ldt[0], sizeof( E_main_S_ldt ) - 1 );
    E_main_S_gdt[4] = (N)&E_main_S_ldt[0] >> 32;
    for_n( i, kernel_args->processor_n )
    {   struct E_main_I_tss *M_(tss);
        if( !tss )
            goto End;
        _0_(tss);
        P exception_stack = E_mem_Q_blk_M_align_tab( E_mem_S_page_size, 1, E_mem_S_page_size );
        if( !exception_stack )
            goto End;
        tss->ist[1] = (N)exception_stack + E_mem_S_page_size;
        tss->io_map_base_address = ~0;
        E_main_S_gdt[ 5 + i * 2 ] = E_main_J_task_descriptor_low( (N)tss, sizeof( *tss ));
        E_main_S_gdt[ 5 + i * 2 + 1 ] = (N)tss >> 32;
    }
    E_main_S_ldt[0] = 0;
    E_main_S_ldt[1] = 0;
    E_main_S_gd.base = (N)&E_main_S_gdt[0];
    E_main_S_gd.limit = ( 5 + kernel_args->processor_n * 2 ) * sizeof( *E_main_S_gdt ) - 1;
    __asm__ volatile (
    "\n"    "lgdt   %0"
    "\n"    "mov    $3 << 3,%%ax"
    "\n"    "lldt   %%ax"
    "\n"    "mov    $2 << 3,%%ax"
    "\n"    "mov    %%ax,%%ds"
    "\n"    "mov    %%ax,%%es"
    "\n"    "mov    %%ax,%%ss"
    "\n"    "xor    %%ax,%%ax"
    "\n"    "mov    %%ax,%%fs"
    "\n"    "mov    %%ax,%%gs"
    "\n"    "movw   $1 << 3,-8(%%rsp)"
    "\n"    "lea    0f(%%rip),%%rax"
    "\n"    "mov    %%rax,-16(%%rsp)"
    "\n"    ".byte  0x48"
    "\n"    "ljmp   *-16(%%rsp)"
    "\n0:"  "mov    $5 << 3,%%ax"
    "\n"    "ltr    %%ax"
    :
    : "p" ( &E_main_S_gd.limit )
    : "rax"
    );
    if( !~E_interrupt_M() )
        goto End;
    E_interrupt_S_external[ E_interrupt_S_gsi_ipi ] = &E_main_I_ipi_test;
    if( !~E_keyboard_M() )
        goto End;
    O{  for_n( i, kernel_args->processor_n - 1 )
            if( !~(N)kernel_args->processor_proc[i] )
                E_interrupt_I_ipi_startup( 1 + i, (P)(N)kernel_args->processor_start_page );
        E_flow_I_sleep(1000);
        for_n_( i, kernel_args->processor_n - 1 )
            if( !~(N)kernel_args->processor_proc[i] )
                break;
        if( i == kernel_args->processor_n - 1 )
            break;
    }
    E_flow_S_scheduler_n = kernel_args->processor_n;
    Mt_( E_flow_S_scheduler, E_flow_S_scheduler_n );
    if( !E_flow_S_scheduler )
        goto End;
    if( !~E_flow_M( kernel_args->kernel_stack ))
        goto End;
    E_interrupt_S_external[ E_interrupt_S_gsi_timer ] = &E_flow_I_apic_timer;
    Mt_( E_main_S_stack, kernel_args->processor_n - 1 );
    if( !E_main_S_stack )
        goto End;
    for_n_( i, kernel_args->processor_n - 1 )
    {   E_main_S_stack[i] = E_mem_Q_blk_M_align_tab( E_mem_S_page_size, 2, E_mem_S_page_size );
        if( !E_main_S_stack[i] )
            goto End;
    }
    for_n_( i, kernel_args->processor_n - 1 )
        kernel_args->processor_proc[i] = &E_main_I_processor_start;
    O{  for_n( i, kernel_args->processor_n - 1 )
            if( E_main_S_stack[i] )
                break;
        if( i == kernel_args->processor_n - 1 )
            break;
        __asm__ volatile (
        "\n"    "pause"
        );
    }
    W( E_main_S_stack );
    E_flow_I_lock( &E_mem_blk_S_mem_lock );
    struct E_mem_Q_blk_Z_free free_p_;
    if( !~E_mem_Q_blk_Q_sys_table_f_P_put( E_mem_blk_S.free_id, (Pc)&free_p_.p - (Pc)&free_p_, (Pc)&free_p_.l - (Pc)&free_p_, (P)(N)kernel_args->processor_start_page, E_mem_S_page_size ))
        goto End;
    E_flow_I_unlock( &E_mem_blk_S_mem_lock );
    W( kernel_args->processor_proc );
    if( !~E_acpi_aml_M( kernel_args->acpi.dsdt_content, kernel_args->acpi.dsdt_content_l, &kernel_args->acpi.ssdt_contents[0], kernel_args->acpi.ssdt_contents_n ))
        goto End;
    if( !~E_acpi_reader_M() )
        goto End;
    W( kernel_args->bootloader );

    if( !~E_pci_I_check_buses() )
        goto End;

    X_M( main, test );
    D_M( main, test )
        goto End;
    O{  X_B( main, test, 0 )
            break;
    }
    X_W( main, test );

    //S status = E_main_S_uefi_runtime_services.reset_system( H_uefi_Z_reset_Z_shutdown, 0, 0, 0 );
End:__asm__ volatile (
    "\n"    "cli"
    "\n0:"  "hlt"
    "\n"    "jmp    0b"
    );
    __builtin_unreachable();
}
D( main, test )
{   I timer = Y_M(1000);
    O{  Y_B( timer, 0 )
            break;
        E_font_I_print( "\ntest" );
        E_interrupt_I_ipi( 1, 32 + E_interrupt_S_gsi_ipi );
    }
    Y_W(timer);
}
/******************************************************************************/
