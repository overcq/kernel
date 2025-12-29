/*******************************************************************************
*   ___   public
*  ¦OUX¦  C+
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         GUI
* (c)overcq              on WSL\Debian (Linux 6.6.87.2)             2025-12-20 I
*******************************************************************************/
_internal N8 E_gui_Q_pointer_S_width, E_gui_Q_pointer_S_height;
_internal N32 E_gui_Q_pointer_S_coordinate[2];
_private B E_gui_Q_taskabar_S_redraw;
_private N8 E_gui_Q_taskbar_S_font_size;
_internal N8 E_gui_Q_taskbar_S_thickness;
_private volatile N8 E_gui_Q_taskbar_S_mouse_over_panel;
_private N32 E_gui_Q_taskbar_S_height;
_internal N32 E_gui_Q_taskbar_S_panel_width, E_gui_Q_taskbar_S_panel_height;
//==============================================================================
_private
N
E_gui_T_print_u(
  N32 width
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
    return dx + thickness <= width;
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
    E_gui_Q_taskbar_S_height = E_gui_Q_taskbar_S_font_size + 1
    + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1
    + 2 * ( E_gui_Q_taskbar_S_font_size + 1 - 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1 - 1
    + E_gui_Q_taskbar_S_font_size + 1;
    E_gui_Q_taskbar_S_panel_width = E_gui_Q_taskbar_S_font_size + 1
    + 2 * (( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1 );
    E_gui_Q_taskbar_S_panel_height = E_gui_Q_taskbar_S_font_size + 1 + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1;
    E_gui_Q_pointer_S_coordinate[0] = E_mouse_S_coordinate[0] = E_main_S_framebuffer.width / 2;
    E_gui_Q_pointer_S_coordinate[1] = E_mouse_S_coordinate[1] = E_main_S_framebuffer.height / 2;
    if( E_windows_M() )
        return ~0;
    //E_gui_S_mouse_moving_hold = no; // Ponieważ jest tworzone okno, które wywołuje ‹zadanie› “draw” jeszcze w tej procedurze, to nie potrzeba.
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
){  N32 width = E_gui_Q_pointer_S_coordinate[0] + E_gui_Q_pointer_S_width > E_main_S_framebuffer.width
    ? E_main_S_framebuffer.width - E_gui_Q_pointer_S_coordinate[0]
    : E_gui_Q_pointer_S_width;
    N32 height = E_gui_Q_pointer_S_coordinate[1] + E_gui_Q_pointer_S_height > E_main_S_framebuffer.height
    ? E_main_S_framebuffer.height - E_gui_Q_pointer_S_coordinate[1]
    : E_gui_Q_pointer_S_height;
    E_vga_Q_buffer_I_draw( E_gui_Q_pointer_S_coordinate[0], E_gui_Q_pointer_S_coordinate[1], width, height );
}
_internal
void
E_gui_Q_pointer_I_move_1( void
){  E_gui_Q_pointer_S_coordinate[0] = E_mouse_S_coordinate[0];
    E_gui_Q_pointer_S_coordinate[1] = E_mouse_S_coordinate[1];
    if( E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n )
    {   N32 taskbar_button_width_with_border = E_main_S_framebuffer.width / E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n;
        if( taskbar_button_width_with_border < E_gui_Q_taskbar_S_thickness + 1 + E_gui_Q_taskbar_S_thickness * 5 + E_gui_Q_taskbar_S_thickness + 1 + 1 )
            taskbar_button_width_with_border = E_gui_Q_taskbar_S_thickness + 1 + E_gui_Q_taskbar_S_thickness * 5 + E_gui_Q_taskbar_S_thickness + 1 + 1;
        if( E_gui_Q_taskbar_S_mouse_over_panel )
        {   N32 panel_x_0 = ( E_gui_Q_taskbar_S_mouse_over_panel - 1 ) * taskbar_button_width_with_border;
            N32 panel_x_1 = panel_x_0 + E_gui_Q_taskbar_S_panel_width;
            if( panel_x_1 > E_main_S_framebuffer.width ) //DFN Szerokość wyświetlacza co najmniej równa “E_gui_Q_taskbar_S_panel_width”.
            {   panel_x_0 -= panel_x_1 - E_main_S_framebuffer.width;
                panel_x_1 -= panel_x_1 - E_main_S_framebuffer.width;
            }
            if( E_gui_Q_pointer_S_coordinate[0] < panel_x_0
            || E_gui_Q_pointer_S_coordinate[0] >= panel_x_1
            )
            {   if( E_gui_Q_pointer_S_coordinate[1] < E_gui_Q_taskbar_S_height )
                {   N8 button = E_gui_Q_pointer_S_coordinate[0] / taskbar_button_width_with_border + 1;
                    if( E_gui_Q_pointer_S_coordinate[0] >= ( button - 1 ) * taskbar_button_width_with_border
                    && E_gui_Q_pointer_S_coordinate[0] < ( button - 1 ) * taskbar_button_width_with_border + E_gui_Q_taskbar_S_panel_width
                    )
                        E_gui_Q_taskbar_S_mouse_over_panel = button;
                    else
                        E_gui_Q_taskbar_S_mouse_over_panel = 0;
                }else
                    E_gui_Q_taskbar_S_mouse_over_panel = 0;
                X_A( gui, draw );
                X_F( gui, draw );
            }else if( E_gui_Q_pointer_S_coordinate[1] >= E_gui_Q_taskbar_S_height + E_gui_Q_taskbar_S_panel_height )
            {   E_gui_Q_taskbar_S_mouse_over_panel = 0;
                X_A( gui, draw );
                X_F( gui, draw );
            }
        }else if( E_gui_Q_pointer_S_coordinate[1] < E_gui_Q_taskbar_S_height )
        {   N8 button = E_gui_Q_pointer_S_coordinate[0] / taskbar_button_width_with_border + 1;
            if( E_gui_Q_pointer_S_coordinate[0] >= ( button - 1 ) * taskbar_button_width_with_border
            && E_gui_Q_pointer_S_coordinate[0] < ( button - 1 ) * taskbar_button_width_with_border + E_gui_Q_taskbar_S_panel_width
            )
            {   E_gui_Q_taskbar_S_mouse_over_panel = button;
                X_A( gui, draw );
                X_F( gui, draw );
            }
        }
    }
    E_gui_Q_pointer_I_draw( E_gui_Q_pointer_S_coordinate[0], E_gui_Q_pointer_S_coordinate[1] );
}
_private
void
E_gui_Q_pointer_I_move( void
){  E_gui_Q_pointer_I_move_0();
    E_gui_Q_pointer_I_move_1();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_private
void
E_gui_Q_pointer_I_click( N8 button
, N8 click_count
, N32 x
, N32 y
){  N8 visible_buttons = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n;
    N32 taskbar_button_width_with_border = E_main_S_framebuffer.width / visible_buttons;
    if( taskbar_button_width_with_border < E_gui_Q_taskbar_S_thickness + 1 + E_gui_Q_taskbar_S_thickness * 5 + E_gui_Q_taskbar_S_thickness + 1 + 1 )
        taskbar_button_width_with_border = E_gui_Q_taskbar_S_thickness + 1 + E_gui_Q_taskbar_S_thickness * 5 + E_gui_Q_taskbar_S_thickness + 1 + 1;
    if( y < E_gui_Q_taskbar_S_panel_height )
    {   if( E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n
        && button == 1
        && click_count == 1
        )
        {   N8 window_i = x / taskbar_button_width_with_border;
            E_windows_Q_window_I_stack_on_top( window_i );
        }else if( button == 2
        && click_count == 1
        )
        {   //TODO Otwórz menu główne.
        }
    }else if( E_gui_Q_taskbar_S_mouse_over_panel
    && button == 1
    && click_count == 1
    )
    {   N32 panel_x_0 = ( E_gui_Q_taskbar_S_mouse_over_panel - 1 ) * taskbar_button_width_with_border;
        N32 panel_x_1 = panel_x_0 + E_gui_Q_taskbar_S_panel_width - 1;
        if( panel_x_1 > E_main_S_framebuffer.width )
            panel_x_0 -= panel_x_1 - E_main_S_framebuffer.width;
        if( x >= panel_x_0 + E_gui_Q_taskbar_S_font_size + 1
        && x < panel_x_0 + E_gui_Q_taskbar_S_font_size + 1 + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height
        ) // Kliknięto przycisk ‛zamknij’.
        {   N8 window_i = x / taskbar_button_width_with_border;
            if( E_windows_Q_window_W( E_window_Q_desktop_S_current, window_i ))
                E_main_I_error_fatal();
        }else if( x >= panel_x_0 + E_gui_Q_taskbar_S_font_size + 1 + (( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1 )
        && x < panel_x_0 + E_gui_Q_taskbar_S_font_size + 1 + (( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1 ) + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height
        )
        {   N8 window_i = x / taskbar_button_width_with_border;
            E_windows_Q_window_I_toggle_size( window_i );
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
void
E_gui_Q_desktop_I_draw( void
){  if( !E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n
    || E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n - 1 ]].sized
    )
    {   E_vga_I_fill_rect( 0, E_gui_Q_taskbar_S_height, E_main_S_framebuffer.width, E_main_S_framebuffer.height - E_gui_Q_taskbar_S_height, E_vga_R_video_color( E_vga_S_background_color ));
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
    {    //TODO Przerysowywać tylko okna, które zawierają co najmniej jeden obszar widoczny.
        for_n( window_i, E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n )
        {   if( E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ window_i ]].sized )
            {   x = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ window_i ]].x + 2;
                y = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ window_i ]].y + 2;
                width = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ window_i ]].width - 4;
                height = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ window_i ]].height - 4;
                E_vga_I_draw_rect( x - 2, y - 2, width + 4, height + 4, 1, E_vga_R_video_color( 0x908b8b ));
                //DFN Do przyszłego rozmiarowania: minimalny rozmiar okna to ramka plus minimalna zawartość.
            }else
            {   x = 0;
                y = E_gui_Q_taskbar_S_height;
                width = E_main_S_framebuffer.width;
                height = E_main_S_framebuffer.height - E_gui_Q_taskbar_S_height;
            }
            E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ window_i ]].draw( x, y, width, height );
        }
    }else
    {   x = 0;
        y = E_gui_Q_taskbar_S_height;
        width = E_main_S_framebuffer.width;
        height = E_main_S_framebuffer.height - E_gui_Q_taskbar_S_height;
        E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack[ E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n - 1 ]].draw( x, y, width, height );
    }
}
_internal
void
E_gui_Q_taskbar_I_draw( void
){  if( !E_gui_Q_taskabar_S_redraw )
        return;
    E_vga_I_fill_rect( 0, 0, E_main_S_framebuffer.width, E_gui_Q_taskbar_S_height, E_vga_R_video_color( 0xb3b3b3 ));
    N8 visible_buttons = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n;
    if( visible_buttons )
    {   N32 taskbar_button_width_with_border = E_main_S_framebuffer.width / E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n;
        if( taskbar_button_width_with_border < E_gui_Q_taskbar_S_thickness + 1 + E_gui_Q_taskbar_S_thickness * 5 + E_gui_Q_taskbar_S_thickness + 1 + 1 )
        {   taskbar_button_width_with_border = E_gui_Q_taskbar_S_thickness + 1 + E_gui_Q_taskbar_S_thickness * 5 + E_gui_Q_taskbar_S_thickness + 1 + 1;
            visible_buttons = E_main_S_framebuffer.width / taskbar_button_width_with_border;
        }
        if( visible_buttons > 1 ) // Rysowanie linii rozdzielających.
        {   for_n( i, visible_buttons - 1 )
            {   if(( i + 1 ) * taskbar_button_width_with_border - 1 >= E_main_S_framebuffer.width )
                    break;
                E_vga_I_draw_y_line(( i + 1 ) * taskbar_button_width_with_border - 1, 0, E_gui_Q_taskbar_S_height, E_vga_R_video_color( 0x908b8b ));
            }
        }
        for_n( i, visible_buttons ) // Rysowanie tekstu przycisków.
        {   N32 x_0 = i * taskbar_button_width_with_border + ( i ? 1 : 0 ) + E_gui_Q_taskbar_S_thickness + 1;
            N32 x = x_0;
            N32 button_real_width = ( i != visible_buttons - 1 ? taskbar_button_width_with_border - 1 : E_main_S_framebuffer.width - x ) - 2 * ( E_gui_Q_taskbar_S_thickness + 1 );
            Pc s = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[i].title;
            while( *s )
            {   U u = ~0;
                Pc s_ = E_text_Z_su_R_u( s, &u );
                s = s_;
                N32 font_i;
                if( !E_gui_T_print_u( button_real_width - ( x - x_0 ), E_gui_Q_taskbar_S_thickness, &u, &font_i ))
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
            x = x_0;
            N32 y = E_gui_Q_taskbar_S_font_size + 1 + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1;
            s = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[i].description;
            while( *s )
            {   U u = ~0;
                Pc s_ = E_text_Z_su_R_u( s, &u );
                s = s_;
                N32 font_i;
                if( !E_gui_T_print_u( button_real_width - ( x - x_0 ), E_gui_Q_taskbar_S_thickness, &u, &font_i ))
                {   if(line)
                        break;
                    line++;
                    x = x_0;
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
}
_internal
void
E_gui_Q_taskbar_I_draw_expander( void
){  if( !E_gui_Q_taskbar_S_mouse_over_panel )
        return;
    N8 visible_buttons = E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n;
    N32 taskbar_button_width_with_border = E_main_S_framebuffer.width / visible_buttons;
    if( taskbar_button_width_with_border < E_gui_Q_taskbar_S_thickness + 1 + E_gui_Q_taskbar_S_thickness * 5 + E_gui_Q_taskbar_S_thickness + 1 + 1 )
        taskbar_button_width_with_border = E_gui_Q_taskbar_S_thickness + 1 + E_gui_Q_taskbar_S_thickness * 5 + E_gui_Q_taskbar_S_thickness + 1 + 1;
    N32 panel_x_0 = ( E_gui_Q_taskbar_S_mouse_over_panel - 1 ) * taskbar_button_width_with_border;
    N32 panel_x_1 = panel_x_0 + E_gui_Q_taskbar_S_panel_width - 1;
    if( panel_x_1 > E_main_S_framebuffer.width )
        panel_x_0 -= panel_x_1 - E_main_S_framebuffer.width;
    E_vga_I_fill_rect( panel_x_0, E_gui_Q_taskbar_S_height, E_gui_Q_taskbar_S_panel_width, E_gui_Q_taskbar_S_panel_height, E_vga_R_video_color( 0xb3b3b3 ));
    // Przycisk ‛zamknij’.
    E_vga_I_draw_rect( panel_x_0 + E_gui_Q_taskbar_S_font_size + 1
    , E_gui_Q_taskbar_S_height + E_gui_Q_taskbar_S_font_size + 1
    , ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height
    , ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height
    , 0
    , E_vga_R_video_color( 0x908b8b )
    );
    E_vga_I_draw_line( panel_x_0 + E_gui_Q_taskbar_S_font_size + 1 + 1
      + 1
    , E_gui_Q_taskbar_S_height + E_gui_Q_taskbar_S_font_size + 1 + 1
      + 1
    , panel_x_0 + E_gui_Q_taskbar_S_font_size + 1
      + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height - 1 - 1
      - 1
    , E_gui_Q_taskbar_S_height + E_gui_Q_taskbar_S_font_size + 1
      + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height - 1 - 1
      - 1
    , 0
    );
    E_vga_I_draw_line( panel_x_0 + E_gui_Q_taskbar_S_font_size + 1 + 1
      + 1
    , E_gui_Q_taskbar_S_height + E_gui_Q_taskbar_S_font_size + 1
      + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height - 1 - 1
      - 1
    , panel_x_0 + E_gui_Q_taskbar_S_font_size + 1
      + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height - 1 - 1
      - 1
    , E_gui_Q_taskbar_S_height + E_gui_Q_taskbar_S_font_size + 1 + 1
      + 1
    , 0
    );
    E_vga_I_draw_rect( panel_x_0 + E_gui_Q_taskbar_S_font_size + 1
      + (( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1 )
    , E_gui_Q_taskbar_S_height + E_gui_Q_taskbar_S_font_size + 1
    , ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height
    , ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height
    , 0
    , E_vga_R_video_color( 0x908b8b )
    );
    if( E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window[ E_gui_Q_taskbar_S_mouse_over_panel - 1 ].sized )
        // Przycisk ‛maksymalizuj’.
        E_vga_I_draw_rect( panel_x_0 + E_gui_Q_taskbar_S_font_size + 1
          + (( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1 ) + 1
          + 1
        , E_gui_Q_taskbar_S_height + E_gui_Q_taskbar_S_font_size + 1 + 1
          + 1
        , ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height
          - 2 * ( 1 + 1 )
        , E_gui_Q_taskbar_S_font_size + 1 + 2
        , 0
        , 0
        );
    else
    {   // Przycisk ‛rozmiaruj’.
        E_vga_I_draw_rect( panel_x_0 + E_gui_Q_taskbar_S_font_size + 1
          + (( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1 ) + 1
          + 1
        , E_gui_Q_taskbar_S_height + E_gui_Q_taskbar_S_font_size + 1 + 1
          + 1
        , (( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height - 2 * ( 1 + 1 )) / 2 + 1 + E_gui_Q_taskbar_S_font_size + 1
        , (( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height - 2 * ( 1 + 1 )) / 2 + 1 + E_gui_Q_taskbar_S_font_size + 1
        , 0
        , 0
        );
        E_vga_I_draw_rect( panel_x_0 + E_gui_Q_taskbar_S_font_size + 1
          + (( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1 ) + 1
          + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height - 2
          - ((( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height - 2 ) / 2 + 1 + E_gui_Q_taskbar_S_font_size + 1 )
        , E_gui_Q_taskbar_S_height + E_gui_Q_taskbar_S_font_size + 1 + 1
          + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height - 2
          - ((( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height - 2 ) / 2 + 1 + E_gui_Q_taskbar_S_font_size + 1 )
        , (( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height - 2 * ( 1 + 1 )) / 2 + 1 + E_gui_Q_taskbar_S_font_size + 1
        , (( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height - 2 * ( 1 + 1 )) / 2 + 1 + E_gui_Q_taskbar_S_font_size + 1
        , 0
        , 0
        );
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
D( gui, draw )
{   X_M( gui, draw );
    O{  X_B( gui, draw, 0 )
            break;
        E_flow_I_cli();
        E_gui_Q_pointer_I_move_0();
        E_gui_Q_desktop_I_draw();
        E_gui_Q_taskbar_I_draw();
        E_gui_Q_taskbar_I_draw_expander();
        E_vga_Q_buffer_I_draw( 0, 0, E_main_S_framebuffer.width, E_main_S_framebuffer.height ); //TODO Kopiować tylko obszary zmienione?
        E_gui_Q_pointer_I_move_1();
        E_flow_I_sti();
    }
}
/******************************************************************************/
