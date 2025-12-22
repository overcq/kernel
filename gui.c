/*******************************************************************************
*   ___   public
*  ¦OUX¦  C+
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         GUI
* (c)overcq              on WSL\Debian (Linux 6.6.87.2)             2025-12-20 I
*******************************************************************************/
_internal N8 E_gui_Q_pointer_S_width, E_gui_Q_pointer_S_height;
_internal N32 E_gui_Q_pointer_S_screen_buffer_coordinate[2];
_private B E_gui_Q_taskabar_S_redraw;
_private N8 E_gui_Q_taskbar_S_font_size;
_internal N8 E_gui_Q_taskbar_S_thickness;
_private B E_gui_S_mouse_drawing_hold;
//==============================================================================
_private
N
E_gui_T_print_u(
  N32 x
, N32 width
, N8 thickness
, U *u
, N32 *font_i
){  thickness++;
    N dx = 0;
    for_n( i, E_font_S_font.bitmap_n )
        if( E_font_S_font.bitmap[i].u == *u )
        {   dx = E_font_S_font.bitmap[i].width;
            break;
        }
    if( !dx )
    {   i = 0;
        *u = E_font_S_font.bitmap[i].u;
        dx = E_font_S_font.bitmap[i].width;
    }
    *font_i = i;
    dx *= thickness;
    return x + dx + thickness <= width;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_private
N
E_gui_M( void
){  E_gui_Q_pointer_S_width = 4 * E_font_S_font.height / 3;
    E_gui_Q_pointer_S_height = 4 * E_font_S_font.height / 3;
    E_emerg_print_S_active = no;
    E_gui_Q_taskbar_S_font_size = 1; //DFN Musi być co najmniej 1 ze względu na zmniejszanie dla opisu przycisku.
    E_gui_Q_taskbar_S_thickness = 1;
    E_gui_Q_pointer_S_screen_buffer_coordinate[0] = E_mouse_S_coordinate[0] = E_main_S_framebuffer.width / 2;
    E_gui_Q_pointer_S_screen_buffer_coordinate[1] = E_mouse_S_coordinate[1] = E_main_S_framebuffer.height / 2;
    if( E_windows_M() )
        return ~0;
    //E_gui_S_mouse_drawing_hold = no; // Ponieważ jest tworzone okno, które wywołuje ‹zadanie› ‘draw’ jeszcze w tej procedurze, to nie potrzeba.
    N8 window_i = E_windows_Q_window_M( E_window_Q_desktop_S_current, "System log", "In memory system log window", E_window_log_I_draw );
    if( (S8)window_i < 0 )
        return (S8)window_i;
    E_window_log_S_active = yes;
    D_M( gui, 0, draw )
        return ~0;
    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
void
E_gui_Q_pointer_I_draw( N32 x
, N32 y
){  volatile N32 *video_address = E_main_S_framebuffer.p + y * E_main_S_framebuffer.pixels_per_scan_line + x;
    for_n( x_i, E_gui_Q_pointer_S_width )
    {   if( x + x_i == E_main_S_framebuffer.width )
            break;
        video_address[ x_i ] = 0;
    }
    video_address = E_main_S_framebuffer.p + ( y + 1 ) * E_main_S_framebuffer.pixels_per_scan_line + x;
    for_n( y_i, E_gui_Q_pointer_S_height - 1 )
    {   if( y + 1 + y_i == E_main_S_framebuffer.height )
            break;
        *video_address = 0;
        video_address += E_main_S_framebuffer.pixels_per_scan_line;
    }
}
_internal
void
E_gui_Q_pointer_I_move_0( void
){  E_vga_Q_buffer_I_draw( E_gui_Q_pointer_S_screen_buffer_coordinate[0], E_gui_Q_pointer_S_screen_buffer_coordinate[1]
    , E_gui_Q_pointer_S_width, E_gui_Q_pointer_S_height
    );
}
_internal
void
E_gui_Q_pointer_I_move_1( void
){  E_gui_Q_pointer_S_screen_buffer_coordinate[0] = E_mouse_S_coordinate[0];
    E_gui_Q_pointer_S_screen_buffer_coordinate[1] = E_mouse_S_coordinate[1];
    E_gui_Q_pointer_I_draw( E_gui_Q_pointer_S_screen_buffer_coordinate[0], E_gui_Q_pointer_S_screen_buffer_coordinate[1] );
}
_private
void
E_gui_Q_pointer_I_move( void
){  E_gui_Q_pointer_I_move_0();
    E_gui_Q_pointer_I_move_1();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
void
E_gui_Q_desktop_I_draw( void
){  N32 taskbar_height = E_gui_Q_taskbar_S_font_size + 1
    + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1
    + 2 * ( E_gui_Q_taskbar_S_font_size + 1 - 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1 - 1
    + E_gui_Q_taskbar_S_font_size + 1;
    if( !E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n
    || E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n - 1 ]].sized
    )
    {   E_vga_I_fill_rect( 0, taskbar_height, E_main_S_framebuffer.width, E_main_S_framebuffer.height - taskbar_height, E_vga_R_video_color( E_vga_S_background_color ));
        E_vga_I_fill_rect( E_main_S_framebuffer.width / 2 - 50, E_main_S_framebuffer.height / 2 - 10 - 13, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
        E_vga_I_fill_rect( E_main_S_framebuffer.width / 2 - 50, E_main_S_framebuffer.height / 2 - 10, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
        E_vga_I_fill_rect( E_main_S_framebuffer.width / 2, E_main_S_framebuffer.height / 2 + 4, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
        E_vga_I_fill_rect( E_main_S_framebuffer.width / 2, E_main_S_framebuffer.height / 2 + 4 + 13, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
        E_vga_I_fill_rect( E_main_S_framebuffer.width / 2 - 38, E_main_S_framebuffer.height / 2 - 37, 38 + 34, 37 + 36, E_vga_R_video_color( 0x43864f ));
        E_vga_I_fill_rect( E_main_S_framebuffer.width / 2 - 50, E_main_S_framebuffer.height / 2 + 4, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
        E_vga_I_fill_rect( E_main_S_framebuffer.width / 2 - 50, E_main_S_framebuffer.height / 2 + 4 + 13, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
        E_vga_I_fill_rect( E_main_S_framebuffer.width / 2, E_main_S_framebuffer.height / 2 - 10 - 13, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
        E_vga_I_fill_rect( E_main_S_framebuffer.width / 2, E_main_S_framebuffer.height / 2 - 10, 48, 5, E_vga_R_video_color( 0x2b2b2b ));
    }
    if( !E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n )
        return;
    N32 x, y, width, height;
    if( E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n - 1 ]].sized )
    {   for_n( window_i, E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n )
        {   if( E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ window_i ]].sized )
            {   x = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ window_i ]].x;
                y = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ window_i ]].y;
                width = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ window_i ]].width;
                height = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ window_i ]].height;
            }else
            {   x = 0;
                y = taskbar_height;
                width = E_main_S_framebuffer.width;
                height = E_main_S_framebuffer.height - taskbar_height;
            }
            E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ window_i ]].draw( x, y, width, height );
        }
    }else
    {   x = 0;
        y = taskbar_height;
        width = E_main_S_framebuffer.width;
        height = E_main_S_framebuffer.height - taskbar_height;
        E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n - 1 ]].draw( x, y, width, height );
    }
}
_internal
void
E_gui_Q_taskbar_I_draw( void
){  N32 height = E_gui_Q_taskbar_S_font_size + 1
    + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1
    + 2 * ( E_gui_Q_taskbar_S_font_size + 1 - 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1 - 1
    + E_gui_Q_taskbar_S_font_size + 1;
    E_vga_I_fill_rect( 0, 0, E_main_S_framebuffer.width, height, E_vga_R_video_color( 0xb3b3b3 ));
    if( E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n )
    {   N32 width = E_main_S_framebuffer.width / E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n;
        if( width < E_gui_Q_taskbar_S_thickness + 1 + E_gui_Q_taskbar_S_thickness * 5 + E_gui_Q_taskbar_S_thickness + 1 + 1 )
            width = E_gui_Q_taskbar_S_thickness + 1 + E_gui_Q_taskbar_S_thickness * 5 + E_gui_Q_taskbar_S_thickness + 1 + 1;
        if( E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n > 1 ) // Rysowanie linii rozdzielających.
        {   for_n( i, E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n - 1 )
                E_vga_I_draw_y_line(( i + 1 ) * width - 1, 0, height, E_vga_R_video_color( 0x908b8b ));
        }
        for_n( i, E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n ) // Rysowanie tekstu przycisków.
        {   N32 x = i * width + E_gui_Q_taskbar_S_thickness + 1;
            Pc s = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[i].title;
            while( *s )
            {   U u = ~0;
                Pc s_ = E_text_Z_su_R_u( s, &u );
                s = s_;
                N32 font_i;
                if( !E_gui_T_print_u( x, width, E_gui_Q_taskbar_S_thickness, &u, &font_i ))
                    break;
                E_font_I_draw_u( font_i
                , x, E_gui_Q_taskbar_S_font_size + 1
                , E_gui_Q_taskbar_S_font_size + 1, ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height
                , E_vga_S_text_color
                , E_gui_Q_taskbar_S_font_size, E_gui_Q_taskbar_S_thickness
                );
                x += ( E_gui_Q_taskbar_S_thickness + 1 ) * E_font_S_font.bitmap[ font_i ].width + E_gui_Q_taskbar_S_thickness + 1;
            }
            N8 line = 0;
            x = i * width + E_gui_Q_taskbar_S_thickness + 1;
            N32 y = E_gui_Q_taskbar_S_font_size + 1 + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1;
            s = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[i].description;
            while( *s )
            {   U u = ~0;
                Pc s_ = E_text_Z_su_R_u( s, &u );
                s = s_;
                N32 font_i;
                if( !E_gui_T_print_u( x, width, E_gui_Q_taskbar_S_thickness, &u, &font_i ))
                {   if(line)
                        break;
                    line++;
                    x = i * width + E_gui_Q_taskbar_S_thickness + 1;
                    y += ( E_gui_Q_taskbar_S_font_size + 1 - 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1 - 1;
                }
                E_font_I_draw_u( font_i
                , x, y
                , y, ( E_gui_Q_taskbar_S_font_size + 1 - 1 ) * E_font_S_font.height
                , E_vga_S_text_color
                , E_gui_Q_taskbar_S_font_size - 1, E_gui_Q_taskbar_S_thickness
                );
                x += ( E_gui_Q_taskbar_S_thickness + 1 ) * E_font_S_font.bitmap[ font_i ].width + E_gui_Q_taskbar_S_thickness + 1;
            }
        }
    }
    E_gui_Q_taskabar_S_redraw = no;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
D( gui, draw )
{   X_M( gui, draw );
    O{  X_B( gui, draw, 0 )
            break;
        E_gui_S_mouse_drawing_hold = yes;
        E_gui_Q_pointer_I_move_0();
        E_gui_Q_desktop_I_draw();
        if( E_gui_Q_taskabar_S_redraw )
            E_gui_Q_taskbar_I_draw();
        E_vga_Q_buffer_I_draw( 0, 0, E_main_S_framebuffer.width, E_main_S_framebuffer.height );
        E_gui_Q_pointer_I_move_1();
        E_gui_S_mouse_drawing_hold = no;
    }
}
/******************************************************************************/
