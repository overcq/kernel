/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         keyboard driver
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒6‒5 Q
*******************************************************************************/
_internal const N E_keyboard_S_timeout = 100;
_internal B E_keyboard_S_mouse;
//==============================================================================
_internal
void
E_keyboard_I_interrupt( void
){  E_font_I_print( "\nkeyboard interrupt, key=" );
    N8 v = E_main_I_in_8( 0x60 );
    E_font_I_print_hex(v);
}
_internal
N
E_keyboard_I_wait_read( void
){  N time;
    E_flow_Q_spin_time_M( &time, 1000 );
    O{  N8 v = E_main_I_in_8( 0x64 );
        if( v & 1 )
            break;
        if( E_flow_Q_spin_time_T( &time ))
            return ~0;
        __asm__ volatile (
        "\n"    "pause"
        );
    }
    return 0;
}
_internal
N
E_keyboard_I_wait_write( void
){  N time;
    E_flow_Q_spin_time_M( &time, 1000 );
    O{  N8 v = E_main_I_in_8( 0x64 );
        if( !( v & 2 ))
            break;
        if( E_flow_Q_spin_time_T( &time ))
            return ~0;
        __asm__ volatile (
        "\n"    "pause"
        );
    }
    return 0;
}
_private
N
E_keyboard_M( void
){  E_main_I_out_8( 0x64, 0xad );
    E_main_I_out_8( 0x64, 0xa7 );
    O{  N8 v = E_main_I_in_8( 0x64 );
        if( !( v & 1 ))
            break;
        E_main_I_in_8( 0x60 );
    }
    E_main_I_out_8( 0x64, 0x20 );
    if( E_keyboard_I_wait_read() )
        return ~0;
    N8 start_conf = E_main_I_in_8( 0x60 );
    N8 v = start_conf;
    v &= ~(( 1 << 6 ) | ( 1 << 4 ) | 1 );
    E_main_I_out_8( 0x64, 0x60 );
    if( E_keyboard_I_wait_write() )
        return ~0;
    E_main_I_out_8( 0x60, v );
    E_main_I_out_8( 0x64, 0xaa );
    if( E_keyboard_I_wait_read() )
        return ~0;
    v = E_main_I_in_8( 0x60 );
    if( v != 0x55 )
        return ~0;
    v = start_conf;
    v &= ~(( 1 << 6 ) | ( 1 << 4 ) | 1 );
    E_main_I_out_8( 0x64, 0x60 );
    if( E_keyboard_I_wait_write() )
        return ~0;
    E_main_I_out_8( 0x60, v );
    E_main_I_out_8( 0x64, 0xa8 );
    if( E_keyboard_I_wait_read() )
        return ~0;
    v = E_main_I_in_8( 0x60 );
    E_keyboard_S_mouse = !( v & ( 1 << 5 ));
    if( E_keyboard_S_mouse )
    {   E_main_I_out_8( 0x64, 0xa7 );
        v = start_conf;
        v &= ~(( 1 << 6 ) | ( 1 << 4 ) | 1 );
        v &= ~(( 1 << 5 ) | ( 1 << 1 ));
        E_main_I_out_8( 0x64, 0x60 );
        if( E_keyboard_I_wait_write() )
            return ~0;
        E_main_I_out_8( 0x60, v );
    }
    E_main_I_out_8( 0x64, 0xab );
    if( E_keyboard_I_wait_read() )
        return ~0;
    v = E_main_I_in_8( 0x60 );
    if(v)
        return ~0;
    if( E_keyboard_S_mouse )
    {   E_main_I_out_8( 0x64, 0xa9 );
        if( E_keyboard_I_wait_read() )
            return ~0;
        v = E_main_I_in_8( 0x60 );
        if(v)
            E_keyboard_S_mouse = no;
    }
    E_main_I_out_8( 0x64, 0xae );
    if( E_keyboard_S_mouse )
        E_main_I_out_8( 0x64, 0xa8 );
    v = start_conf;
    v &= ~( 1 << 4 );
    v |= 1;
    if( E_keyboard_S_mouse )
    {   v &= ~( 1 << 5 );
        v |= 1 << 1;
    }
    E_main_I_out_8( 0x64, 0x60 );
    if( E_keyboard_I_wait_write() )
        return ~0;
    E_main_I_out_8( 0x60, v );
    if( E_keyboard_I_wait_write() )
        return ~0;
    E_main_I_out_8( 0x60, 0xff );
    if( E_keyboard_I_wait_read() )
        return ~0;
    v = E_main_I_in_8( 0x60 );
    if( v != 0xfa )
        return ~0;
    if( E_keyboard_I_wait_read() )
        return ~0;
    v = E_main_I_in_8( 0x60 );
    if( v != 0xaa )
        return ~0;
    O{  v = E_main_I_in_8( 0x64 );
        if( !( v & 1 ))
            break;
        E_main_I_in_8( 0x60 );
    }
    if( E_keyboard_S_mouse )
    {   E_main_I_out_8( 0x64, 0xd4 );
        if( E_keyboard_I_wait_write() )
            return ~0;
        E_main_I_out_8( 0x60, 0xff );
        if( E_keyboard_I_wait_read() )
            return ~0;
        v = E_main_I_in_8( 0x60 );
        if( v == 0xfa )
        {   if( E_keyboard_I_wait_read() )
                return ~0;
            v = E_main_I_in_8( 0x60 );
            if( v == 0xaa )
                O{  v = E_main_I_in_8( 0x64 );
                    if( !( v & 1 ))
                        break;
                    E_main_I_in_8( 0x60 );
                }
            else
                E_keyboard_S_mouse = no;
        }else
            E_keyboard_S_mouse = no;
    }
    E_interrupt_P( 1, &E_keyboard_I_interrupt );
    return 0;
}
/******************************************************************************/
