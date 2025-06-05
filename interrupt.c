/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         interrupt handlers
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒6‒4 K
*******************************************************************************/
_private void E_interrupt_I_exeception_de(void);
_private void E_interrupt_I_exeception_db(void);
_private void E_interrupt_I_exeception_nmi(void);
_private void E_interrupt_I_exeception_bp(void);
_private void E_interrupt_I_exeception_of(void);
_private void E_interrupt_I_exeception_br(void);
_private void E_interrupt_I_exeception_ud(void);
_private void E_interrupt_I_exeception_nm(void);
_private void E_interrupt_I_exeception_df(void);
_private void E_interrupt_I_exeception_mf(void);
_private void E_interrupt_I_exeception_ts(void);
_private void E_interrupt_I_exeception_np(void);
_private void E_interrupt_I_exeception_ss(void);
_private void E_interrupt_I_exeception_gp(void);
_private void E_interrupt_I_exeception_pf(void);
_private void E_interrupt_I_exeception_mf(void);
_private void E_interrupt_I_exeception_ac(void);
_private void E_interrupt_I_exeception_mc(void);
_private void E_interrupt_I_exeception_xm(void);
_private void E_interrupt_I_exeception_ve(void);
_private void E_interrupt_I_exeception_cp(void);
_private void E_interrupt_I_external(void);
_private void E_interrupt_I_external_spurious(void);
_private void E_interrupt_I_keyboard(void);
//==============================================================================
_private volatile P E_interrupt_Q_local_apic_S_address;
_private volatile P E_interrupt_Q_io_apic_S_address;
_private
struct E_interrupt_Z_gsi
{ N8 source;
  N8 flags;
} *E_interrupt_S_gsi;
_private N8 E_interrupt_S_gsi_n;
_private N *E_interrupt_S_idt;
_private N8 E_interrupt_S_idt_n;
//==============================================================================
_internal
N32
E_interrupt_Q_io_apic_R( N8 i
){  *( volatile N32 * )E_interrupt_Q_io_apic_S_address = i;
    return *( volatile N32 * )( (Pc)E_interrupt_Q_io_apic_S_address + 0x10 );
}
_internal
void
E_interrupt_Q_io_apic_P( N8 i
, N32 v
){  *( volatile N32 * )E_interrupt_Q_io_apic_S_address = i;
    *( volatile N32 * )( (Pc)E_interrupt_Q_io_apic_S_address + 0x10 ) = v;
}
_internal
void
E_interrupt_Q_io_apic_P_gsi( N8 i
, N64 v
){  E_interrupt_Q_io_apic_P( 0x10 + i * 2, v & 0xffffffff );
    E_interrupt_Q_io_apic_P( 0x10 + i * 2 + 1, v >> 32 );
}
//------------------------------------------------------------------------------
_internal
N32
E_interrupt_Q_local_apic_R( N i
){  return *( volatile N32 * )( E_interrupt_Q_local_apic_S_address + i * 0x10 );
}
_private
void
E_interrupt_Q_local_apic_P( N i
, N32 v
){  *( volatile N32 * )( E_interrupt_Q_local_apic_S_address + i * 0x10 ) = v;
}
//------------------------------------------------------------------------------
_internal
void
E_interrupt_I_ipi( N processor
, N8 i
){  E_interrupt_Q_local_apic_P( 0x31, ( processor & 0xff ) << 24 );
    E_interrupt_Q_local_apic_P( 0x30, i );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define E_interrupt_J_gsi_v( vector, delivery_mode, polarity, trigger_mode, mask, destination ) \
    ( (vector) | ( (N)( delivery_mode ) << 8 ) | ( (N)(polarity) << 13 ) | ( (N)( trigger_mode ) << 15 ) | ( (N)(mask) << 16 ) | ( (N)(destination) << 56 ))
_internal
void
E_interrupt_I_enable( N8 i
){  N polarity = ( E_interrupt_S_gsi[i].flags & 3 ) == 1 ? 0 : 1;
    N trigger_mode = (( E_interrupt_S_gsi[i].flags >> 2 ) & 3 ) == 1 ? 0 : 1;
    E_interrupt_Q_io_apic_P_gsi( i, E_interrupt_J_gsi_v( 32 + i, 0, polarity, trigger_mode, 0, 0 ));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define E_interrupt_J_interrupt_descriptor_low( selector, offset ) (( (N)(offset) & (( 1UL << 16 ) - 1 )) | ( (N)(selector) << 16 ) | ( 0xeUL << ( 32 + 8 )) | ( 1UL << ( 32 + 15 )) | ((( (N)(offset) >> 16 ) & (( 1UL << 16 ) - 1 )) << ( 32 + 16 )))
#define E_interrupt_J_interrupt_descriptor( i, procedure ) \
    {   E_interrupt_S_idt[ i * 2 ] = E_interrupt_J_interrupt_descriptor_low( 1 << 3, (N)&(procedure) ); \
        E_interrupt_S_idt[ i * 2 + 1 ] = (N)&(procedure) >> 32; \
    }
_private
N
E_interrupt_M( void
){  E_interrupt_S_idt_n = 32 + E_interrupt_S_gsi_n + 1;
    Mt_( E_interrupt_S_idt, E_interrupt_S_idt_n * 2 );
    if( !E_interrupt_S_idt )
        return ~0;
    E_interrupt_J_interrupt_descriptor( 0, E_interrupt_I_exeception_de );
    E_interrupt_J_interrupt_descriptor( 1, E_interrupt_I_exeception_db );
    E_interrupt_J_interrupt_descriptor( 2, E_interrupt_I_exeception_nmi );
    E_interrupt_J_interrupt_descriptor( 3, E_interrupt_I_exeception_bp );
    E_interrupt_J_interrupt_descriptor( 4, E_interrupt_I_exeception_of );
    E_interrupt_J_interrupt_descriptor( 5, E_interrupt_I_exeception_br );
    E_interrupt_J_interrupt_descriptor( 6, E_interrupt_I_exeception_ud );
    E_interrupt_J_interrupt_descriptor( 7, E_interrupt_I_exeception_nm );
    E_interrupt_J_interrupt_descriptor( 8, E_interrupt_I_exeception_df );
    E_interrupt_S_idt[ 9 * 2 ] = 0;
    E_interrupt_J_interrupt_descriptor( 10, E_interrupt_I_exeception_ts );
    E_interrupt_J_interrupt_descriptor( 11, E_interrupt_I_exeception_np );
    E_interrupt_J_interrupt_descriptor( 12, E_interrupt_I_exeception_ss );
    E_interrupt_J_interrupt_descriptor( 13, E_interrupt_I_exeception_gp );
    E_interrupt_J_interrupt_descriptor( 14, E_interrupt_I_exeception_pf );
    E_interrupt_S_idt[ 15 * 2 ] = 0;
    E_interrupt_J_interrupt_descriptor( 16, E_interrupt_I_exeception_mf );
    E_interrupt_J_interrupt_descriptor( 17, E_interrupt_I_exeception_ac );
    E_interrupt_J_interrupt_descriptor( 18, E_interrupt_I_exeception_mc );
    E_interrupt_J_interrupt_descriptor( 19, E_interrupt_I_exeception_xm );
    E_interrupt_J_interrupt_descriptor( 20, E_interrupt_I_exeception_ve );
    E_interrupt_J_interrupt_descriptor( 21, E_interrupt_I_exeception_cp );
    for( N i = 22; i != 32; i++ )
        E_interrupt_S_idt[ i * 2 ] = 0;
    for( N i = 32; i != 32 + E_interrupt_S_gsi_n; i++ )
        E_interrupt_J_interrupt_descriptor( i, E_interrupt_I_external );
    E_interrupt_J_interrupt_descriptor( 32 + E_interrupt_S_gsi_n, E_interrupt_I_external_spurious );
    struct __attribute__ ((packed))
    { N32 pad_1;
      N16 pad_2;
      N16 limit;
      N base;
    }id;
    id.base = (N)&E_interrupt_S_idt[0];
    id.limit = E_interrupt_S_idt_n * 2 * sizeof( *E_interrupt_S_idt ) - 1;
    __asm__ volatile (
    "\n"    "lidt   %0"
    :
    : "p" ( &id.limit )
    );
    E_interrupt_Q_local_apic_P( 0xf, E_interrupt_Q_local_apic_R( 0xf ) | 0x100 | ( E_interrupt_S_idt_n - 1 ));
    __asm__ volatile (
    "\n"    "sti"
    );
    return 0;
}
/******************************************************************************/
