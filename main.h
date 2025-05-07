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
#define J_swap(type,a,b)                    { type J_autogen(c) = a; a = b; b = J_autogen(c); }
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
#include "simple.h"
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
P E_mem_Q_blk_I_append( P, N, N );
P E_mem_Q_blk_I_prepend( P, N );
P E_mem_Q_blk_I_insert( P, N, N );
P E_mem_Q_blk_I_remove( P, N, N );
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
struct E_main_Z_kernel
{ struct E_mem_blk_Z mem_blk;
  struct E_mem_Z_memory_map *memory_map;
  N memory_map_n;
  N gdt[5], ldt[2], idt[ 22 * 2 ];
  P kernel;
  P page_table;
  P *stack;
  struct H_main_Z_uefi_runtime_services uefi_runtime_services;
  struct
  { P dsdt_content;
    N dsdt_content_l;
    P facs;
  }acpi;
};
/******************************************************************************/
