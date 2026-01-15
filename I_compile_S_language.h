/*******************************************************************************
*   ___   public
*  ¦OUX¦  C+
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         base definitions
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒5‒2 Q
*******************************************************************************/
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
#define J_autogen_line(a)                   J_autogen( J_a_b( a, __LINE__ ))
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define _F_uid_v(v)                         ( (v) << ( sizeof(int) * 8 / 2 ))
#define _F_uid(file_identifier)             J_autogen(J_a_b(F,file_identifier))
#define _K_proc(module,event)               J_a_b(J_a_b(E,module),J_a_b(K,event))
#define _XhYi_F_uid(file_identifier)        _F_uid_v( _F_uid(file_identifier) )
#define _XhYi_uid(module,report_impulser)   J_autogen(J_a_b(J_a_b(E,module),report_impulser))
#define _X_uid(module,report)               J_autogen(J_a_b(J_a_b(E,module),J_a_b(X,report)))
#define _X_var(module,report)               J_autogen(J_a_b(J_a_b(E,module),J_a_b(J_a_b(X,report),S)))
#define _Yi_uid(module,impulser)            J_autogen(J_a_b(J_a_b(E,module),J_a_b(Yi,impulser)))
#define _Yi_var(module,impulser)            J_autogen(J_a_b(J_a_b(E,module),J_a_b(J_a_b(Yi,impulser),S)))
#define _D_proc(module,task)                J_a_b(J_a_b(E,module),J_a_b(D,task))
#define D_id(module,task)                   J_autogen(J_a_b(J_a_b(E,module),J_a_b(J_a_b(D,task),S)))
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define O                                   while(yes)
#define for_n_(i_var,n)                     for( i_var = 0; i_var != (n); i_var++ )
#define for_n(i_var,n)                      N i_var; for_n_(i_var,(n))
#define for_n_rev_(i_var,n)                 i_var = (n); while( i_var-- )
#define for_n_rev(i_var,n)                  N for_n_rev_(i_var,(n))
#define for_i_(i_var,n)                     for_n_(i_var,(n))
#define for_i(i_var,n)                      I i_var; for_i_(i_var,(n))
#define for_i_rev_(i_var,n)                 for_n_rev_(i_var,(n))
#define for_i_rev(i_var,n)                  I for_i_rev_(i_var,(n))
//------------------------------------------------------------------------------
#define for_each_out_(out,id_var,p,q)       id_var = (out); while( ~( id_var = J_a_b(q,Z_iter_R_next)( (p), id_var, (out) )))
#define for_each_out(out,id_var,p,q)        I for_each_out_(out,id_var,(p),q)
#define for_each_(id_var,p,q)               for_each_out_(~0,id_var,(p),q)
#define for_each(id_var,p,q)                for_each_out(~0,id_var,(p),q)
//------------------------------------------------------------------------------
#define for_each_rev_out_(out,id_var,p,q)   id_var = (out); while( ~( id_var = J_a_b(q,Z_iter_R_prev)( (p), id_var, (out) )))
#define for_each_rev_out(out,id_var,p,q)    I for_each_rev_out_(out,id_var,(p),q)
#define for_each_rev_(id_var,p,q)           for_each_rev_out_(~0,id_var,(p),q)
#define for_each_rev(id_var,p,q)            for_each_rev_out(~0,id_var,(p),q)
//------------------------------------------------------------------------------
#define for_each_pop_out_(out,id_var,p,q) \
  for( \
  ; ~( id_var = J_a_b(q,Z_iter_R_prev)( (p), ~0, (out) )) \
  ; E_mem_Q_tab_I_remove( (p), id_var ) \
  )
#define for_each_pop_out(out,id_var,p,q)    I id_var; for_each_pop_out_((out),id_var,(p),q)
#define for_each_pop_(id_var,p,q)           for_each_pop_out_(~0,id_var,(p),q)
#define for_each_pop(id_var,p,q)            I id_var; for_each_pop_(id_var,(p),q)
//------------------------------------------------------------------------------
#define for_each_q(id_var,p,iter,q) \
  I id_var; \
  I J_autogen_line(id_var) = ~0; \
  while( ~( id_var = J_a_b(q,Q_iter_R_next)( (p), (iter), ++J_autogen_line(id_var) )))
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define _0(pointer_variable,l)              E_mem_Q_blk_P_fill_c( (pointer_variable), l, 0 )
#define _0_(pointer_variable)               _0( (pointer_variable), sizeof( *(pointer_variable) ))
#define _0t_(pointer_variable,n)            _0( (pointer_variable), (n) * sizeof( *(pointer_variable) ))
//------------------------------------------------------------------------------
#define M(l)                                E_mem_Q_blk_M(l)
#define Mt(u,n)                             E_mem_Q_blk_M_tab((u),(n))
#define W(pointer_variable)                 E_mem_Q_blk_W( pointer_variable )
#define M_(pointer_variable)                pointer_variable = M( sizeof( *( pointer_variable )))
#define Mt_(pointer_variable,n)             pointer_variable = Mt( sizeof( *( pointer_variable )), (n) )
#define W_(pointer_variable)                ( W( pointer_variable ), pointer_variable = 0 )
#define W_tab_(pointer_variable)            ( E_mem_Q_tab_W( pointer_variable ), pointer_variable = 0 )
//------------------------------------------------------------------------------
// Instrukcja blokowa definicji ‹zadania›.
#define D(module,task)                      _private _unused void _D_proc(module,task)(void)
    #ifdef C_line_report
#define D_M(module,additional_stack_pages,task) if( ~E_flow_Q_task_M( &D_id(module,task), ( additional_stack_pages ), &_D_proc(module,task), J_s( _D_proc(module,task) ))){} else
    #else
#define D_M(module,additional_stack_pages,task) if( ~E_flow_Q_task_M( &D_id(module,task), ( additional_stack_pages ), &_D_proc(module,task) )){} else
    #endif
#define D_W(module,task)                    E_flow_Q_task_W( &D_id(module,task) )
//------------------------------------------------------------------------------
// Znacznik stanu — zwykle stanu pojedynczego obiektu sygnalizującego później kolekcję — umieszczony w strukturze tego ‹obiektu› dostępnej przez wyrażenie.
#define U_R(start_expr,state_name)          J_a_b(start_expr,J_autogen(J_a_b(U,state_name)))
// Wzbudzenie stanu.
#define U_F(start_expr,state_name)          U_R(start_expr,state_name) = yes
// I liniowe obsłużenie tego stanu.
#define U_L(start_expr,state_name)          U_R(start_expr,state_name) = no
// Albo blokowe.
#define U_E(start_expr,state_name)          ( U_R(start_expr,state_name) && ( U_L(start_expr,state_name), yes ))
//------------------------------------------------------------------------------
// Instrukcje “X_M”/“X_A”, “Yi_M”/“Yi_A” muszą występować w jednym z najwyższych bloków struktury programu ‹zadania›, w miejscu zapewniającym taką widoczność (do użycia ‹raportu› w tym ‹zadaniu›) jak deklaracja zmiennej lokalnej.
// Utworzenie i wyrzucenie ‹raportu›.
#define X_M_(module,report)                 _X_var(module,report) = E_flow_Q_report_M( _X_uid(module,report) )
#define X_M(module,report)                  I X_M_(module,report)
#define X_W(module,report)                  E_flow_Q_report_W( _X_var(module,report) )
// Deklaracja emisji ‹raportu› przez ‹zadanie›.
#define X_A(module,report)                  X_M(module,report); _unused B U_L(module,report)
// Sygnalizacja ‹zadania› obsługującego ‹raport› kolekcji.
#define X_F(module,report)                  E_flow_Q_report_I_signal( _X_var(module,report) )
// I warunkowa– gdy jest stan pojedynczego obiektu.
#define X_U(module,report)                  if( !U_E(module,report) ){} else X_F(module,report)
// Czekanie na ‹raport› kolekcji.
#define X_B(module,report,lost_count)       if( !E_flow_Q_report_I_wait( _X_var(module,report), (lost_count) )){} else
// Czyszczenie licznika raportów.
#define X_L(module,report)                  E_flow_Q_report_I_clear( _X_var(module,report) )
//------------------------------------------------------------------------------
// Utworzenie i wyrzucenie ‹cyklera›.
#define Y_M(period)                         E_flow_Q_timer_M(period)
#define Y_W(timer)                          E_flow_Q_timer_W(timer)
// Czekanie na pełny okres ‹cyklera›.
#define Y_B(timer,lost_count)               if( !E_flow_Q_timer_I_wait( (timer), (lost_count) )){} else
//------------------------------------------------------------------------------
// Utworzenie i wyrzucenie ‹impulsatora›.
#define Yi_M(module,impulser)               I _Yi_var(module,impulser) = E_flow_Q_impulser_M( _Yi_uid(module,impulser) )
#define Yi_W(module,impulser)               E_flow_Q_timer_W( _Yi_var(module,impulser) )
// Deklaracja aktywacji ‹impulsatora› przez ‹zadanie›.
#define Yi_A(module,impulser)               I _Yi_var(module,impulser) = E_flow_Q_impulser_M_srv( _Yi_uid(module,impulser) )
// Aktywacja ‹impulsatora›.
#define Yi_F(module,impulser,time)          E_flow_Q_impulser_I_activate( _Yi_var(module,impulser), (time) )
// Dezaktywacja ‹impulsatora›.
#define Yi_L(module,impulser)               E_flow_Q_impulser_I_deactivate( _Yi_var(module,impulser) )
// Czekanie na wzbudzenie przez ‹impulsator›.
#define Yi_B(module,impulser)               if( !E_flow_Q_impulser_I_wait( _Yi_var(module,impulser) )){} else
//------------------------------------------------------------------------------
// Czekanie na wznowienie w następnym obiegu czasu.
#define I_B()                               if( !E_flow_Q_task_I_schedule() ){} else
//------------------------------------------------------------------------------
// Wyjście z ‹zadania› po procedurze zawierającej instrukcję przełączenia.
#define I_V()                               if( !E_flow_Q_task_R_exit() ){} else
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define G(...)                              E_se_log_I_log( &__func__[0], __VA_ARGS__ )
#define G_(...)                             E_se_log_I_log_( __VA_ARGS__ )
//==============================================================================
#define _inline                             static __attribute__ (( __always_inline__, __unused__ ))
#define _internal                           static
#define _private                            __attribute__ (( __visibility__( "hidden" ) ))
#define _export                             __attribute__ (( __visibility__( "protected" ) ))
    #ifndef _unreachable
#define _unreachable                        __builtin_unreachable()
    #endif
#define _unused                             __attribute__ (( __unused__ ))
//==============================================================================
#define S_eof                               ( ~1UL )
//==============================================================================
//TODO Do uporządkowania.
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
#define E_vga_S_background_color            0xdbdbdb
#define E_vga_S_text_color                  0
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
#define H_uefi_Z_error_S_ip_address_conflict H_uefi_S_error(34)
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
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef void ( *E_interrupt_S_external_Z )(void);
/******************************************************************************/
