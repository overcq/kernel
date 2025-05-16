/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         include
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒5‒2 Q
*******************************************************************************/
typedef unsigned char       N8;
typedef signed char         S8;
typedef unsigned short      N16;
typedef short               S16;
typedef unsigned            N32;
typedef int                 S32;
typedef unsigned long long  N64;
typedef long long           S64;
typedef _Bool               B;
typedef char                C;
typedef N16                 C16;
typedef N64                 N;
typedef S64                 S;
typedef N32                 U;
typedef double              F;
typedef void                *P;
typedef C                   *Pc;
typedef C16                 *Pc16;
typedef N                   *Pn;
typedef N32                 I;
typedef unsigned            In;
    #ifdef __SSE__
typedef unsigned __int128   N128;
typedef __int128            S128;
    #endif
//------------------------------------------------------------------------------
#define false                               0
#define true                                1
#define no                                  false
#define yes                                 true
#define _v(a,v)                             (( (a) ^ (a) ) | (v) )
//------------------------------------------------------------------------------
#define _J_ab(a,b)                          a##b
#define J_ab(a,b)                           _J_ab(a,b)
#define J_a_b(a,b)                          J_ab(J_ab(a,_),b)
#define J_s0_R_l(s)                         ( sizeof(s) - 1 )
#define J_a_R_n(a)                          ( sizeof(a) / sizeof( (a)[0] ))
//------------------------------------------------------------------------------
#define J_swap(type,a,b)                    { type J_autogen(_) = a; a = b; b = J_autogen(_); }
#define J_min(a,b)                          ( (a) > (b) ? (b) : (a) )
#define J_max(a,b)                          ( (a) < (b) ? (b) : (a) )
#define J_min_max(a,b,c)                    ( J_min( (a), J_max( (b), (c) )))
#define J_abs(v)                            ( (S)(v) < 0 ? -(v) : (v) )
//------------------------------------------------------------------------------
#define J_autogen_S                         _autogen
#define J_autogen(a)                        J_a_b( a, J_autogen_S )
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define O                                   while(yes)
#define for_n_(i_var,n)                     for( i_var = 0; i_var != (n); i_var++ )
#define for_n(i_var,n)                      N i_var; for_n_(i_var,(n))
#define for_n_rev_(i_var,n)                 i_var = (n); while( i_var-- )
#define for_n_rev(i_var,n)                  N for_n_rev_(i_var,(n))
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define _0(pointer_variable,l)              E_mem_Q_blk_P_fill_c( (pointer_variable), l, 0 )
#define _0_(pointer_variable)               _0( (pointer_variable), sizeof( *(pointer_variable) ))
#define M(l)                                E_mem_Q_blk_M(l)
#define Mt(u,n)                             E_mem_Q_blk_M_tab((u),(n))
#define W(pointer_variable)                 E_mem_Q_blk_W( pointer_variable )
#define M_(pointer_variable)                pointer_variable = M( sizeof( *( pointer_variable )))
#define Mt_(pointer_variable,n)             pointer_variable = Mt( sizeof( *( pointer_variable )), (n) )
#define W_(pointer_variable)                ( W( pointer_variable ), pointer_variable = 0 )
#define W_tab_(pointer_variable)            ( E_mem_Q_tab_W( pointer_variable ), pointer_variable = 0 )
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define _inline                             static __attribute__ (( __always_inline__, __unused__ ))
#define _internal                           static
#define _private                            __attribute__ (( __visibility__( "hidden" ) ))
#define _export                             __attribute__ (( __visibility__( "protected" ) ))
//==============================================================================
#define S_eof                               ( ~1UL )
//==============================================================================
struct E_datetime_Z
{ N16 year;
  N8 month, day;
  N millisecond;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct E_flow_Z_args
{ N argc;
  Pc *argv;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct E_mem_Q_tab_Z
{ Pc *index; // Tablica mapowania indeksów do adresów w “data”.
  Pc data; // Dane ciągłe.
  N u; // Rozmiar elementu tablicy.
  struct E_mem_Q_tab_Z *iterator;
  I index_n;
  I data_n;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct E_mem_Q_tab_S_iterator_Z
{ I *index; // Dowolna sekwencja ‹identyfikatorów› (indeksów) danych, ale bez powtórzeń.
  I n;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define E_vga_S_background_color            0xcacaca
#define E_vga_S_text_color                  0
enum E_vga_Z_aa_pixel
{ E_vga_Z_aa_pixel_S_e = 1 << 0,
  E_vga_Z_aa_pixel_S_se = 1 << 1,
  E_vga_Z_aa_pixel_S_s = 1 << 2,
  E_vga_Z_aa_pixel_S_sw = 1 << 3,
  E_vga_Z_aa_pixel_S_w = 1 << 4,
  E_vga_Z_aa_pixel_S_nw = 1 << 5,
  E_vga_Z_aa_pixel_S_n = 1 << 6,
  E_vga_Z_aa_pixel_S_ne = 1 << 7
};
//==============================================================================
#include "simple.h"
//==============================================================================
N E_aml_M( Pc, N );
//==============================================================================
B E_mem_Q_blk_T_eq( P, P, N );
void E_mem_Q_blk_I_copy( P, P, N );
void E_mem_Q_blk_P_fill_c( P, N, C );
P E_mem_Q_blk_M(N);
P E_mem_Q_blk_M_tab( N, N );
P E_mem_Q_blk_M_align( N, N );
P E_mem_Q_blk_M_align_tab( N, N, N );
P E_mem_Q_blk_M_replace_tab( P, N, N );
P E_mem_Q_blk_M_replace( P, N );
P E_mem_Q_blk_M_split( P, N );
N E_mem_Q_blk_W(P);
P E_mem_Q_blk_I_add( P, N, N *, N * );
P E_mem_Q_blk_I_prepend_append( P, N, N );
P E_mem_Q_blk_I_append( P, N );
P E_mem_Q_blk_I_prepend( P, N );
P E_mem_Q_blk_I_insert( P, N, N );
P E_mem_Q_blk_I_remove( P, N, N );
//==============================================================================
N E_font_M(void);
N E_font_I_draw( N, N, N, U );
void E_font_I_print_nl(void);
void E_font_I_print_u(U);
N E_font_I_print(Pc);
void E_font_I_print_hex(N);
//==============================================================================
B E_text_Z_c_T_alpha(C);
B E_text_Z_c_T_quote(C);
C E_text_Z_c_I_lower(C);
C E_text_Z_c_I_upper(C);
Pc E_text_Z_s0_I_lower(Pc);
Pc E_text_Z_s0_I_upper(Pc);
Pc E_text_Z_s_I_lower( Pc, Pc );
Pc E_text_Z_s_I_upper( Pc, Pc );
B E_text_Z_s0_T_eq_s0( Pc, Pc );
B E_text_Z_s0_T_eq_case_s0( Pc, Pc );
N E_text_Z_s0_T_starts_s0( Pc, Pc );
N E_text_Z_s0_T_starts_case_s0( Pc, Pc );
B E_text_Z_s0_T_ends_s0( Pc, Pc );
B E_text_Z_s_T_ends_s0( Pc, Pc, Pc );
N E_text_Z_s0_I_cmp_s0( Pc, Pc );
N E_text_Z_sl_I_cmp( Pc, Pc, N );
N E_text_Z_s_T_starts_s0( Pc, Pc, Pc );
N E_text_Z_s_T_starts_case_s0( Pc, Pc, Pc );
B E_text_Z_s_T_eq_s0( Pc, Pc, Pc );
B E_text_Z_s_T_eq_case_s0( Pc, Pc, Pc );
B E_text_Z_s_T_eq_s( Pc, Pc, Pc, Pc );
B E_text_Z_s_T_eq_case_s( Pc, Pc, Pc, Pc );
B E_text_Z_sl_T_eq_case( Pc, Pc, N );
Pc E_text_Z_s0_R_end_0_le( Pc, N );
B E_text_Z_s0_T_l_0_le( Pc, N );
Pc E_text_Z_s_R_search_0( Pc, Pc );
Pc E_text_Z_s_R_search_c( Pc, Pc, C );
Pc E_text_Z_s_R_search_c_( Pc s, C );
Pc E_text_Z_s0_R_search_c( Pc, C );
Pc E_text_Z_s_R_search_last_c( Pc, Pc, C );
Pc E_text_Z_s0_R_search_last_c_( Pc, C );
Pc E_text_Z_s_R_search_s( Pc, Pc, Pc, Pc );
Pc E_text_Z_s_R_search_s0( Pc, Pc, Pc );
Pc E_text_Z_s0_R_search_s0( Pc, Pc );
N E_text_Z_s_Z_10_N_n( Pc, Pc, Pc * );
N E_text_Z_s0_Z_10_N_n( Pc, Pc * );
N E_text_Z_s_N_n( Pc, Pc, Pc *, N );
N E_text_Z_s0_N_n( Pc, Pc *, N );
S E_text_Z_s_N_sn( Pc, Pc, Pc *, N );
S E_text_Z_s0_N_sn( Pc, Pc *, N );
F E_text_Z_s_N_f( Pc, Pc, Pc *, N );
F E_text_Z_s0_N_f( Pc, Pc *, N );
struct E_math_Z_bignum *E_text_Z_s_N_bignum( Pc, Pc, Pc *, N, N );
struct E_math_Z_bignum *E_text_Z_s0_N_bignum( Pc, Pc *, N, N );
struct E_datetime_Z E_text_Z_s_N_datetime( Pc, Pc, Pc * );
struct E_datetime_Z E_text_Z_s0_N_datetime( Pc, Pc * );
Pc E_text_Z_n_N_s( Pc, N, N, N );
N E_text_Z_n_N_s_G( N, N, N );
Pc E_text_Z_ns_N_s( Pc, S, N, N );
N E_text_Z_ns_N_s_G( S, N, N );
Pc E_text_Z_s_P_c_fill( Pc, Pc, C );
void E_text_Z_s0_P_lower(Pc);
void E_text_Z_s_P_lower( Pc, Pc );
void E_text_Z_s0_P_upper(Pc);
void E_text_Z_s_P_upper( Pc, Pc );
Pc E_text_Z_s_P_copy_s( Pc, Pc, Pc );
Pc E_text_Z_s_P_copy_s_0( Pc, Pc, Pc );
Pc E_text_Z_s_P_copy_sl_0( Pc, Pc, N );
Pc E_text_Z_s_P_copy_s0( Pc, Pc );
Pc E_text_Z_s_P_copy_s0_0( Pc, Pc );
void E_text_Z_sl_P_rev( Pc, N );
N E_text_Z_s_I_append_s( Pc *, Pc, Pc );
N E_text_Z_s_I_s_append_0( Pc *, Pc, Pc );
N E_text_Z_s_I_append_s0( Pc *, Pc );
N E_text_Z_s_I_append_s0_0( Pc *, Pc );
Pc E_text_Z_s_I_prepend_s( Pc *, Pc, Pc );
Pc E_text_Z_s_I_prepend_s0( Pc *, Pc );
N E_text_Z_s0_I_clear( Pc * );
Pc E_text_Z_s0_I_append_c( Pc *, C );
Pc E_text_Z_s0_I_append_s0( Pc *, Pc );
Pc E_text_Z_s0_I_append_s( Pc *, Pc, Pc );
Pc E_text_Z_s_Z_cmdline_T_quote_n( Pc, Pc );
Pc E_text_Z_s0_Z_cmdline_T_quote(Pc);
N E_text_Z_s_Z_cmdline_N_quote( Pc, N, struct E_flow_Z_args *, N );
N E_text_Z_s_Z_cmdline_N_quote_G( N, struct E_flow_Z_args *, N );
N E_text_Z_su0_R_n(Pc);
N E_text_Z_u_R_su( U, Pc );
N E_text_Z_u_R_su_G(U);
Pc E_text_Z_su_R_u( Pc, U * );
Pc E_text_Z_su_R_u_rev( Pc, U * );
N E_text_Z_su0_R_l( Pc s );
N E_text_Z_getter_Z_c_R_u( N (*)(void), U * );
N E_text_Z_getter_Z_c_R_u_rev( N (*)(void), U * );
//==============================================================================
N E_vga_Z_color_M( N8, N8, N8 );
N8 E_vga_Z_color_R_red(N32);
N8 E_vga_Z_color_R_green(N32);
N8 E_vga_Z_color_R_blue(N32);
N E_vga_Z_color_M_gray(N8);
N E_vga_R_video_color(N);
N E_vga_R_pixel( N, N );
void E_vga_P_pixel( N, N, N );
void E_vga_I_set_pixel_aa( N, N, N, F, N );
void E_vga_I_draw_rect( N, N, N, N, N );
void E_vga_I_fill_rect( N, N, N, N, N );
//==============================================================================
#define H_uefi_Z_api __attribute__(( __ms_abi__ ))
#define H_uefi_S_error(e) ( (S)( 1LL << 63 ) | e )
//------------------------------------------------------------------------------
#define H_uefi_Z_error_S_success            0
#define H_uefi_Z_error_S_load_error         H_uefi_S_error(1)
#define H_uefi_Z_error_S_invalid_parameter  H_uefi_S_error(2)
#define H_uefi_Z_error_S_unsupported        H_uefi_S_error(3)
#define H_uefi_Z_error_S_bad_buffer_size    H_uefi_S_error(4)
#define H_uefi_Z_error_S_buffer_too_small   H_uefi_S_error(5)
#define H_uefi_Z_error_S_not_ready          H_uefi_S_error(6)
#define H_uefi_Z_error_S_device_error       H_uefi_S_error(7)
#define H_uefi_Z_error_S_write_protected    H_uefi_S_error(8)
#define H_uefi_Z_error_S_out_of_resoureces  H_uefi_S_error(9)
#define H_uefi_Z_error_S_volume_corrupted   H_uefi_S_error(10)
#define H_uefi_Z_error_S_volume_full        H_uefi_S_error(11)
#define H_uefi_Z_error_S_no_media           H_uefi_S_error(12)
#define H_uefi_Z_error_S_media_changed      H_uefi_S_error(13)
#define H_uefi_Z_error_S_not_found          H_uefi_S_error(14)
#define H_uefi_Z_error_S_access_denied      H_uefi_S_error(15)
#define H_uefi_Z_error_S_no_response        H_uefi_S_error(16)
#define H_uefi_Z_error_S_no_mapping         H_uefi_S_error(17)
#define H_uefi_Z_error_S_timeout            H_uefi_S_error(18)
#define H_uefi_Z_error_S_not_started        H_uefi_S_error(19)
#define H_uefi_Z_error_S_already_started    H_uefi_S_error(20)
#define H_uefi_Z_error_S_aborted            H_uefi_S_error(21)
#define H_uefi_Z_error_S_icmp_error         H_uefi_S_error(22)
#define H_uefi_Z_error_S_tftp_error         H_uefi_S_error(23)
#define H_uefi_Z_error_S_protocol_error     H_uefi_S_error(24)
#define H_uefi_Z_error_S_imcompatible_version H_uefi_S_error(25)
#define H_uefi_Z_error_S_security_violation H_uefi_S_error(26)
#define H_uefi_Z_error_S_crc_error          H_uefi_S_error(27)
#define H_uefi_Z_error_S_end_of_media       H_uefi_S_error(28)
#define H_uefi_Z_error_S_end_of_file        H_uefi_S_error(31)
#define H_uefi_Z_error_S_invalid_language   H_uefi_S_error(32)
#define H_uefi_Z_error_S_compromised_data   H_uefi_S_error(33)
#define H_uefi_Z_error_S_ip_address_confilct H_uefi_S_error(34)
#define H_uefi_Z_error_S_http_error         H_uefi_S_error(35)
//------------------------------------------------------------------------------
struct H_uefi_Z_guid
{ N32 data_1;
  N16 data_2;
  N16 data_3;
  N8 data_4[8];
};
struct H_uefi_Z_time
{ N16 year;
  N8 month;
  N8 day;
  N8 hour;
  N8 minute;
  N8 second;
  N8 pad_1;
  N32 nanosecond;
  S16 time_zone;
  N8 daylight;
  N8 pad_2;
};
struct H_uefi_Z_time_capabilities
{ N32 resolution;
  N32 accuracy;
  B sets_to_zero;
};
struct H_uefi_Z_capsule_header
{ struct H_uefi_Z_guid guid;
  N32 header_l;
  N32 flags;
  N32 image_size;
};
enum H_uefi_Z_reset
{ H_uefi_Z_reset_Z_cold
, H_uefi_Z_reset_Z_warm
, H_uefi_Z_reset_Z_shutdown
, H_uefi_Z_reset_Z_platform_specific
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct H_main_Z_uefi_runtime_services
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
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct E_mem_Q_blk_Z_free
{ Pc p;
  N l;
};
struct E_mem_Q_blk_Z_allocated
{ Pc p;
  N n;
  N u;
};
struct E_mem_blk_Z
{ struct E_mem_Q_blk_Z_allocated *allocated;
  N free_id, allocated_id;
  N *M_from_free_S_allocated_id[2];
  N M_from_free_S_table_id[2];
  N M_from_free_S_allocated_id_n;
  N memory_size;
  N reserved_size;
  B reserved_from_end;
};
struct E_mem_Z_memory_map
{ N64 physical_start;
  N64 virtual_start;
  N64 pages;
};
struct __attribute__ (( __packed__ )) H_acpi_Z_generic_address
{ N8 space;
  N8 width;
  N8 offset;
  N8 access_size;
  N64 address;
};
struct H_oux_Z_hpet
{ N8 comparator_count   :5;
  N8 counter_size       :1;
  N8 legacy_replacement :1;
  struct H_acpi_Z_generic_address address;
  N8 hpet_number;
  N16 minimum_tick;
  N8 page_protection;
};
struct __attribute__ (( __packed__ )) H_acpi_Z_mcfg_entry
{ N64 base_address;
  N16 pci_segment;
  N8 start_bus;
  N8 end_bus;
  N32 reserved;
};
struct H_main_Z_pixel_shifts
{ N8 red;
  N8 green;
  N8 blue;
};
enum H_uefi_Z_pixel_format
{ H_uefi_Z_pixel_format_S_rgb8
, H_uefi_Z_pixel_format_S_bgr8
, H_uefi_Z_pixel_format_S_bitmask
};
struct H_main_Z_framebuffer
{ volatile N32 *p;
  N32 width, height;
  N32 pixels_per_scan_line;
  enum H_uefi_Z_pixel_format pixel_format;
  struct H_main_Z_pixel_shifts pixel_shifts;
};
struct H_main_Z_kernel_Z_acpi
{ P apic_content;
  N apic_content_l;
  P dmar_content;
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
struct E_main_Z_kernel
{ struct E_mem_blk_Z mem_blk;
  struct E_mem_Z_memory_map *memory_map;
  N memory_map_n;
  N gdt[5], ldt[2], idt[ 22 * 2 ];
  P kernel;
  P page_table;
  P *stack;
  struct H_main_Z_framebuffer framebuffer;
  struct H_main_Z_uefi_runtime_services uefi_runtime_services;
  struct H_main_Z_kernel_Z_acpi acpi;
};
/******************************************************************************/
