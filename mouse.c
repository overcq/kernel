/*******************************************************************************
*   ___   public
*  ¦OUX¦  C+
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         keyboard driver
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒6‒5 Q
*******************************************************************************/
enum E_mouse_Q_button_Z_click_state
{ E_mouse_Q_button_Z_click_state_S_clean
, E_mouse_Q_button_Z_click_state_S_press
, E_mouse_Q_button_Z_click_state_S_release
, E_mouse_Q_button_Z_click_state_S_dirty
};
//==============================================================================
_private B E_mouse_S_initialized;
_internal N8 E_mouse_S_interrupt_state;
_internal N8 E_mouse_S_interrupt_data_0;
_private volatile N32 E_mouse_S_coordinate[2];
_internal N8 E_mouse_Q_button_S_buttons;
_internal volatile enum E_mouse_Q_button_Z_click_state E_mouse_Q_button_S_click_state;
_internal N32 E_mouse_Q_button_S_click_coordinate[2];
_private volatile N E_mouse_Q_button_S_time;
_internal N8 E_mouse_Q_button_S_click_dx, E_mouse_Q_button_S_click_dy;
_internal N16 E_mouse_Q_button_S_press_timeout, E_mouse_Q_button_S_release_timeout, E_mouse_Q_button_S_dirty_timeout;
_private volatile N8 E_mouse_Q_button_S_click_button, E_mouse_Q_button_S_click_count;
_internal B E_mouse_S_moved;
//==============================================================================
_internal
void
E_mouse_I_interrupt( void
){  N8 v = E_main_I_in_8( 0x60 );
    switch( E_mouse_S_interrupt_state )
    { default:
            if( v == 0xfa )
                E_mouse_S_interrupt_state++;
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
            Yi_A( mouse, click );
            if( E_mouse_S_moved )
            {   E_mouse_S_moved = no;
                E_gui_Q_pointer_I_move();
                if(( E_mouse_Q_button_S_click_state == E_mouse_Q_button_Z_click_state_S_press
                  || E_mouse_Q_button_S_click_state == E_mouse_Q_button_Z_click_state_S_release
                )
                && ( J_abs( E_mouse_Q_button_S_click_coordinate[0] - E_mouse_S_coordinate[0] ) > E_mouse_Q_button_S_click_dx
                  || J_abs( E_mouse_Q_button_S_click_coordinate[1] - E_mouse_S_coordinate[1] ) > E_mouse_Q_button_S_click_dy
                ))
                {   E_mouse_Q_button_S_click_state = E_mouse_Q_button_Z_click_state_S_dirty;
                    E_flow_Q_spin_time_M( &E_mouse_Q_button_S_time, E_mouse_Q_button_S_dirty_timeout * 1000 );
                    Yi_L( mouse, click );
                }
            }
            N8 buttons = E_mouse_S_interrupt_data_0 & 7;
            if( E_mouse_Q_button_S_buttons ^ buttons)
            {   if( E_mouse_Q_button_S_click_state == E_mouse_Q_button_Z_click_state_S_dirty )
                {   if( E_mouse_Q_button_S_buttons )
                        E_flow_Q_spin_time_M( &E_mouse_Q_button_S_time, E_mouse_Q_button_S_dirty_timeout * 1000 );
                    else if( E_flow_Q_spin_time_T( &E_mouse_Q_button_S_time ))
                        E_mouse_Q_button_S_click_state = E_mouse_Q_button_Z_click_state_S_clean;
                }
                if( E_mouse_Q_button_S_click_state != E_mouse_Q_button_Z_click_state_S_dirty )
                    if( !buttons
                    || buttons == 1 << 0
                    || buttons == 1 << 2
                    || buttons == 1 << 1
                    ) // Naciśnięto/zwolniono tylko ze zbioru dozwolonego: zwolniono wszystkie przyciski lub naciśnięto tylko jeden.
                    {   switch( E_mouse_Q_button_S_click_state )
                        { case E_mouse_Q_button_Z_click_state_S_clean:
                                E_mouse_Q_button_S_click_state = E_mouse_Q_button_Z_click_state_S_press;
                                E_mouse_Q_button_S_click_coordinate[0] = E_mouse_S_coordinate[0];
                                E_mouse_Q_button_S_click_coordinate[1] = E_mouse_S_coordinate[1];
                                E_mouse_Q_button_S_click_button = buttons;
                                E_mouse_Q_button_S_click_count = 0;
                                E_flow_Q_spin_time_M( &E_mouse_Q_button_S_time, E_mouse_Q_button_S_press_timeout * 1000 );
                                Yi_F( mouse, click, E_mouse_Q_button_S_release_timeout );
                                break;
                          case E_mouse_Q_button_Z_click_state_S_press:
                                if( !E_flow_Q_spin_time_T( &E_mouse_Q_button_S_time ))
                                {   E_mouse_Q_button_S_click_state = E_mouse_Q_button_Z_click_state_S_release;
                                    E_mouse_Q_button_S_click_count++;
                                    E_flow_Q_spin_time_M( &E_mouse_Q_button_S_time, E_mouse_Q_button_S_release_timeout * 1000 );
                                    Yi_F( mouse, click, E_mouse_Q_button_S_release_timeout );
                                }else
                                {   E_mouse_Q_button_S_click_state = E_mouse_Q_button_Z_click_state_S_dirty;
                                    E_flow_Q_spin_time_M( &E_mouse_Q_button_S_time, E_mouse_Q_button_S_dirty_timeout * 1000 );
                                    Yi_L( mouse, click );
                                }
                                break;
                          default: // E_mouse_Q_button_Z_click_state_S_release
                                if( !E_flow_Q_spin_time_T( &E_mouse_Q_button_S_time ))
                                    if( buttons == E_mouse_Q_button_S_click_button )
                                    {   E_mouse_Q_button_S_click_state = E_mouse_Q_button_Z_click_state_S_press;
                                        E_mouse_Q_button_S_click_count++;
                                        E_flow_Q_spin_time_M( &E_mouse_Q_button_S_time, E_mouse_Q_button_S_press_timeout * 1000 );
                                        Yi_F( mouse, click, E_mouse_Q_button_S_press_timeout );
                                    }else
                                    {   E_mouse_Q_button_S_click_state = E_mouse_Q_button_Z_click_state_S_dirty;
                                        E_flow_Q_spin_time_M( &E_mouse_Q_button_S_time, E_mouse_Q_button_S_dirty_timeout * 1000 );
                                        Yi_L( mouse, click );
                                    }
                                break;
                        }
                    }else
                    {   E_mouse_Q_button_S_click_state = E_mouse_Q_button_Z_click_state_S_dirty;
                        E_flow_Q_spin_time_M( &E_mouse_Q_button_S_time, E_mouse_Q_button_S_dirty_timeout * 1000 );
                        Yi_L( mouse, click );
                    }
                E_mouse_Q_button_S_buttons = buttons;
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
    E_mouse_Q_button_S_click_dx = 16; //CONF
    E_mouse_Q_button_S_click_dy = 16; //CONF
    E_mouse_Q_button_S_press_timeout = 360; //CONF
    E_mouse_Q_button_S_release_timeout = 360; //CONF
    E_mouse_Q_button_S_dirty_timeout = 360; //CONF
    D_M( mouse, 0, click )
        return ~0;
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
//==============================================================================
D( mouse, click )
{   Yi_M( mouse, click );
    O{  Yi_B( mouse, click )
            break;
        __asm__ volatile (
        "\n"    "cli"
        );
        if( E_flow_Q_spin_time_T( &E_mouse_Q_button_S_time )
        && ( E_mouse_Q_button_S_click_state == E_mouse_Q_button_Z_click_state_S_press
          || E_mouse_Q_button_S_click_state == E_mouse_Q_button_Z_click_state_S_release
        ))
        {   E_mouse_Q_button_S_click_state = E_mouse_Q_button_Z_click_state_S_dirty;
            E_flow_Q_spin_time_M( &E_mouse_Q_button_S_time, E_mouse_Q_button_S_dirty_timeout * 1000 );
            __asm__ volatile (
            "\n"    "sti"
            );
            G( "click: %x×%x", E_mouse_Q_button_S_click_count, E_mouse_Q_button_S_click_button );
        }else
            __asm__ volatile (
            "\n"    "sti"
            );
    }
    Yi_W( mouse, click );
}
/******************************************************************************/
