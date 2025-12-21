/*******************************************************************************
*   ___   public
*  ¦OUX¦  C+
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         log window
* (c)overcq              on WSL\Debian (Linux 6.6.87.2)             2025-12-21 S
*******************************************************************************/
_private B E_window_log_S_active;
//==============================================================================
_private
void
E_window_log_I_draw( N32 x
, N32 y
, N32 width
, N32 height
){  E_vga_I_fill_rect( x, y, width, height, E_vga_S_background_color );
    E_flow_I_lock( &E_se_log_S_lock );
    if( !E_se_log_S_buffer_l )
    {   E_flow_I_unlock( &E_se_log_S_lock );
        return;
    }
    N8 font_size = 0;
    N8 thickness = 0;
    // Sprawdź, ile byłoby linii wyświetlanych dla szerokości okna od ostatnich akapitów/znaków nowej linii/początków linii w tekście.
    N nl = 0;
    Pc s_begin = E_se_log_S_buffer + E_se_log_S_buffer_l;
    Pc s_end;
    N32 x_;
    do
    {   nl++;
        s_end = s_begin;
        Pc s = s_begin = E_text_Z_s_R_search_last_c( E_se_log_S_buffer, s_end, '\n' );
        if( *s == '\n' )
            s++;
        x_ = thickness + 1;
        while( s != s_end )
        {   U u = ~0;
            Pc s_ = E_text_Z_su_R_u( s, &u );
            s = s_;
            N32 font_i;
            if( !E_gui_T_print_u( x_, width, thickness, &u, &font_i ))
            {   x_ = thickness + 1;
                nl++;
            }
            x_ += ( thickness + 1 ) * E_font_S_font.bitmap[ font_i ].width + thickness + 1;
        }
    }while( s_begin != E_se_log_S_buffer
    && height > font_size + 1 + nl * (( font_size + 1 ) * E_font_S_font.height + font_size + 1 )
    );
    if( *s_begin == '\n' )
        s_begin++;
    // Przesuń wskaźnik tekstu do przodu, tak by pozostały tylko linie, które się zmieszczą w wysokości okna.
    x_ = thickness + 1;
    if( height < font_size + 1 + ( nl - 1 ) * (( font_size + 1 ) * E_font_S_font.height + font_size + 1 ) + 1 )
        while( s_begin != s_end )
        {   U u = ~0;
            Pc s = E_text_Z_su_R_u( s_begin, &u );
            N32 font_i;
            if( !E_gui_T_print_u( x_, width, thickness, &u, &font_i ))
            {   x_ = thickness + 1;
                nl--;
                if( height >= font_size + 1 + ( nl - 1 ) * (( font_size + 1 ) * E_font_S_font.height + font_size + 1 ) + 1 )
                    break;
            }
            x_ += ( thickness + 1 ) * E_font_S_font.bitmap[ font_i ].width + thickness + 1;
            s_begin = s;
        }
    // Narysuj tekst.
    N32 y_ = height > font_size + 1 + nl * (( font_size + 1 ) * E_font_S_font.height + font_size + 1 )
    ? 0
    : height - ( font_size + 1 + nl * (( font_size + 1 ) * E_font_S_font.height + font_size + 1 ));
    x_ += x;
    y_ += y + font_size + 1;
    while( s_begin != E_se_log_S_buffer + E_se_log_S_buffer_l )
    {   U u = ~0;
        Pc s = E_text_Z_su_R_u( s_begin, &u );
        s_begin = s;
        if( u == '\n' )
        {   x_ = thickness + 1;
            y_ += ( font_size + 1 ) * E_font_S_font.height + font_size + 1;
            continue;
        }
        N32 font_i;
        if( !E_gui_T_print_u( x_, width, thickness, &u, &font_i ))
        {   x_ = thickness + 1;
            y_ += ( font_size + 1 ) * E_font_S_font.height + font_size + 1;
        }
        E_font_I_draw_u( font_i, x_, y_, y, height, E_vga_S_text_color, font_size, thickness );
        x_ += ( thickness + 1 ) * E_font_S_font.bitmap[ font_i ].width + thickness + 1;
    }
    E_flow_I_unlock( &E_se_log_S_lock );
}
/******************************************************************************/
