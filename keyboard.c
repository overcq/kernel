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
){  E_font_I_print( "\nkeyboard interrupt" );
    N8 v = E_main_I_inb( 0x60 );
    E_font_I_print_hex(v);
}
_internal
N
E_keyboard_I_wait_read( void
){  N time = E_flow_I_current_time();
    N time_end = time + E_interrupt_S_cpu_freq * E_keyboard_S_timeout / 1000;
    O{  N8 v = E_main_I_inb( 0x64 );
        if( v & 1 )
            break;
        __asm__ volatile (
        "\n"    "pause"
        );
        if( E_flow_I_current_time() > time_end )
            return ~0;
    }
    return 0;
}
_internal
N
E_keyboard_I_wait_write( void
){  N time = E_flow_I_current_time();
    N time_end = time + E_interrupt_S_cpu_freq * E_keyboard_S_timeout / 1000;
    O{  N8 v = E_main_I_inb( 0x64 );
        if( v ^ 2 )
            break;
        __asm__ volatile (
        "\n"    "pause"
        );
        if( E_flow_I_current_time() > time_end )
            return ~0;
    }
    return 0;
}
_private
N
E_keyboard_M( void
){  E_main_I_outb( 0x64, 0xad );
    E_main_I_outb( 0x64, 0xa7 );
    E_main_I_inb( 0x60 );
    E_main_I_outb( 0x64, 0x20 );
    if( !~E_keyboard_I_wait_read() )
        return ~0;
    N8 v = E_main_I_inb( 0x60 );
    v &= ~(( 1 << 6 ) | ( 1 << 4 ) | 1 );
    E_main_I_outb( 0x64, 0x60 );
    E_main_I_outb( 0x60, v );
    E_main_I_outb( 0x64, 0xaa );
    if( !~E_keyboard_I_wait_read() )
        return ~0;
    v = E_main_I_inb( 0x60 );
    if( v != 0x55 )
        return ~0;
    E_main_I_outb( 0x64, 0x20 );
    if( !~E_keyboard_I_wait_read() )
        return ~0;
    v = E_main_I_inb( 0x60 );
    v &= ~(( 1 << 6 ) | ( 1 << 4 ) | 1 );
    E_main_I_outb( 0x64, 0x60 );
    E_main_I_outb( 0x60, v );
    E_main_I_outb( 0x64, 0xa8 );
    if( !~E_keyboard_I_wait_read() )
        return ~0;
    v = E_main_I_inb( 0x60 );
    E_keyboard_S_mouse = !( v & ( 1 << 5 ));
    if( E_keyboard_S_mouse )
    {   E_main_I_outb( 0x64, 0xa7 );
        E_main_I_outb( 0x64, 0x20 );
        if( !~E_keyboard_I_wait_read() )
            return ~0;
        v = E_main_I_inb( 0x60 );
        v &= ~(( 1 << 5 ) | ( 1 << 1 ));
        E_main_I_outb( 0x64, 0x60 );
        E_main_I_outb( 0x60, v );
    }
    E_main_I_outb( 0x64, 0xab );
    if( !~E_keyboard_I_wait_read() )
        return ~0;
    v = E_main_I_inb( 0x60 );
    if(v)
        return ~0;
    if( E_keyboard_S_mouse )
    {   E_main_I_outb( 0x64, 0xa9 );
        if( !~E_keyboard_I_wait_read() )
            return ~0;
        v = E_main_I_inb( 0x60 );
        if(v)
            E_keyboard_S_mouse = no;
    }
    E_main_I_outb( 0x64, 0xae );
    if( E_keyboard_S_mouse )
        E_main_I_outb( 0x64, 0xa8 );
    E_main_I_outb( 0x64, 0x20 );
    if( !~E_keyboard_I_wait_read() )
        return ~0;
    v = E_main_I_inb( 0x60 );
    v |= 1;
    if( E_keyboard_S_mouse )
        v |= 1 << 1;
    E_main_I_outb( 0x64, 0x60 );
    E_main_I_outb( 0x60, v );
    E_main_I_outb( 0x60, 0xff );
    if( !~E_keyboard_I_wait_read() )
        return ~0;
    v = E_main_I_inb( 0x60 );
    if( v != 0xfa )
        return ~0;
    if( !~E_keyboard_I_wait_read() )
        return ~0;
    v = E_main_I_inb( 0x60 );
    if( v != 0xaa )
        return ~0;
    if( E_keyboard_S_mouse )
    {   E_main_I_outb( 0x64, 0xd4 );
        E_main_I_outb( 0x60, 0xff );
        if( !~E_keyboard_I_wait_read() )
            return ~0;
        v = E_main_I_inb( 0x60 );
        if( v != 0xfa )
            E_keyboard_S_mouse = no;
        if( E_keyboard_S_mouse )
        {   if( !~E_keyboard_I_wait_read() )
                return ~0;
            v = E_main_I_inb( 0x60 );
            if( v != 0xaa )
                E_keyboard_S_mouse = no;
        }
    }
    E_interrupt_P( 1, &E_keyboard_I_interrupt );
    return 0;
}
/******************************************************************************/
