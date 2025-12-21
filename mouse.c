/*******************************************************************************
*   ___   public
*  ¦OUX¦  C+
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         keyboard driver
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒6‒5 Q
*******************************************************************************/
_private B E_mouse_S_initialized;
_internal volatile N8 E_mouse_S_interrupt_state;
_internal volatile N8 E_mouse_S_interrupt_data_0;
_private volatile N32 E_mouse_S_coordinate[2];
_internal volatile B E_mouse_S_moved;
//==============================================================================
_internal
void
E_mouse_I_interrupt( void
){  N8 v = E_main_I_in_8( 0x60 );
    switch( E_mouse_S_interrupt_state )
    { default:
            if( v == 0xfa )
                E_mouse_S_interrupt_state = 0;
            break;
      case 0:
            E_mouse_S_interrupt_data_0 = v;
            E_mouse_S_interrupt_state++;
            break;
      case 1:
            if(v)
            {   if( E_mouse_S_interrupt_data_0 & ( 1 << 4 ))
                {   N32 x = E_mouse_S_interrupt_data_0 & ( 1 << 6 ) ? 0x100 : 0x100 - v;
                    if( E_mouse_S_coordinate[0] > x )
                        E_mouse_S_coordinate[0] -= x;
                    else
                        E_mouse_S_coordinate[0] = 0;
                }else
                {   E_mouse_S_coordinate[0] += E_mouse_S_interrupt_data_0 & ( 1 << 6 ) ? 0x100 : v;
                    if( E_mouse_S_coordinate[0] >= E_main_S_framebuffer.width ) 
                        E_mouse_S_coordinate[0] = E_main_S_framebuffer.width - 1;
                }
                E_mouse_S_moved = yes;
            }else
                E_mouse_S_moved = no;
            E_mouse_S_interrupt_state++;
            break;
      case 2:
            if(v)
            {   if( E_mouse_S_interrupt_data_0 & ( 1 << 5 ))
                {   E_mouse_S_coordinate[1] += E_mouse_S_interrupt_data_0 & ( 1 << 7 ) ? 0x100 : 0x100 - v;
                    if( E_mouse_S_coordinate[1] >= E_main_S_framebuffer.height ) 
                        E_mouse_S_coordinate[1] = E_main_S_framebuffer.height - 1;
                }else
                {   N32 y = E_mouse_S_interrupt_data_0 & ( 1 << 7 ) ? 0x100 : v;
                    if( E_mouse_S_coordinate[1] > y )
                        E_mouse_S_coordinate[1] -= y;
                    else
                        E_mouse_S_coordinate[1] = 0;
                }
                E_mouse_S_moved = yes;
            }
            if( E_mouse_S_moved )
            {   E_mouse_S_moved = no;
                if( !E_gui_S_mouse_drawing_hold )
                    E_gui_Q_pointer_I_move();
            }
            E_mouse_S_interrupt_state = 0;
            break;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_private
N
E_mouse_M( void
){  if( !E_mouse_S_initialized )
        return 0;
    E_mouse_S_interrupt_state = ~0;
    E_interrupt_P( 12, &E_mouse_I_interrupt );
    if( E_keyboard_I_wait_write() )
        return ~0;
    E_main_I_out_8( 0x64, 0xd4 );
    if( E_keyboard_I_wait_write() )
        return ~0;
    E_main_I_out_8( 0x60, 0xf4 ); // Włącz raportowanie wskaźnika.
    G( "Mouse initialized." );
    return 0;
}
/******************************************************************************/
