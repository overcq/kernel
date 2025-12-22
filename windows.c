/*******************************************************************************
*   ___   public
*  ¦OUX¦  C_
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         windows
* (c)overcq              on WSL\Debian (Linux 6.6.87.2)             2025-12-21 J
*******************************************************************************/
struct E_window_Q_window_Z
{ Pc title, description;
  void ( *draw )( N32 x, N32 y, N32 width, N32 height );
  N32 x, y, width, height;
  B sized;
};
struct E_window_Q_desktop_Z
{ struct E_window_Q_window_Z *window;
  N8 *stack;
  N8 window_n;
};
//==============================================================================
_private struct E_window_Q_desktop_Z *E_windows_Q_desktop_S;
_private N8 E_windows_Q_desktop_S_n;
_private N8 E_window_Q_desktop_S_current;
//==============================================================================
_private
N
E_windows_M( void
){  E_windows_Q_desktop_S_n = 1;
    Mt_( E_windows_Q_desktop_S, E_windows_Q_desktop_S_n );
    if( !E_windows_Q_desktop_S )
        return ~0;
    E_window_Q_desktop_S_current = 0;
    E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n = 0;
    Mt_( E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window, E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n );
    if( !E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window )
        return ~2;
    Mt_( E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack, E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].window_n );
    if( !E_windows_Q_desktop_S[ E_window_Q_desktop_S_current ].stack )
        return ~2;
    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_export
N
E_windows_Q_window_M( N8 desktop_i
, Pc title
, Pc description
, void ( *draw_proc )( N32, N32, N32, N32 )
){  N8 window_i = E_windows_Q_desktop_S[ desktop_i ].window_n;
    if( (S8)window_i < 0 )
        return ~0;
    if( !E_mem_Q_blk_I_append( &E_windows_Q_desktop_S[ desktop_i ].window, 1 ))
        return ~0;
    if( !E_mem_Q_blk_I_append( &E_windows_Q_desktop_S[ desktop_i ].stack, 1 ))
        return E_mem_Q_blk_I_remove( &E_windows_Q_desktop_S[ desktop_i ].window, window_i, 1 ) ? ~0 : ~2;
    E_windows_Q_desktop_S[ desktop_i ].stack[ window_i ] = window_i;
    E_windows_Q_desktop_S[ desktop_i ].window[ window_i ].title = E_text_Z_s0_M_duplicate(title);
    if( !E_windows_Q_desktop_S[ desktop_i ].window[ window_i ].title )
    {   if( !E_mem_Q_blk_I_remove( &E_windows_Q_desktop_S[ desktop_i ].stack, window_i, 1 ))
            return ~2;
        return E_mem_Q_blk_I_remove( &E_windows_Q_desktop_S[ desktop_i ].window, window_i, 1 ) ? ~0 : ~2;
    }
    E_windows_Q_desktop_S[ desktop_i ].window[ window_i ].description = E_text_Z_s0_M_duplicate(description);
    if( !E_windows_Q_desktop_S[ desktop_i ].window[ window_i ].description )
    {   if( W( E_windows_Q_desktop_S[ desktop_i ].window[ window_i ].title ))
            return ~2;
        if( !E_mem_Q_blk_I_remove( &E_windows_Q_desktop_S[ desktop_i ].stack, window_i, 1 ))
            return ~2;
        return E_mem_Q_blk_I_remove( &E_windows_Q_desktop_S[ desktop_i ].window, window_i, 1 ) ? ~0 : ~2;
    }
    E_windows_Q_desktop_S[ desktop_i ].window_n++;
    E_windows_Q_desktop_S[ desktop_i ].window[ window_i ].sized = no;
    E_windows_Q_desktop_S[ desktop_i ].window[ window_i ].x = 0;
    E_windows_Q_desktop_S[ desktop_i ].window[ window_i ].y = 0;
    E_windows_Q_desktop_S[ desktop_i ].window[ window_i ].width = E_main_S_framebuffer.width - 2;
    N32 taskbar_height = E_gui_Q_taskbar_S_font_size + 1
    + ( E_gui_Q_taskbar_S_font_size + 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1
    + 2 * ( E_gui_Q_taskbar_S_font_size + 1 - 1 ) * E_font_S_font.height + E_gui_Q_taskbar_S_font_size + 1 - 1
    + E_gui_Q_taskbar_S_font_size + 1;
    E_windows_Q_desktop_S[ desktop_i ].window[ window_i ].height = E_main_S_framebuffer.height - taskbar_height - 2;
    E_windows_Q_desktop_S[ desktop_i ].window[ window_i ].draw = draw_proc;
    E_gui_Q_taskabar_S_redraw = yes;
    X_A( gui, draw );
    X_F( gui, draw );
    return window_i;
}
_export
N
E_windows_Q_window_W( N8 desktop_i
, N8 window_i
){  if( W( E_windows_Q_desktop_S[ desktop_i ].window[ window_i ].description ))
        return ~2;
    if( W( E_windows_Q_desktop_S[ desktop_i ].window[ window_i ].title ))
        return ~2;
    if( !E_mem_Q_blk_I_remove( &E_windows_Q_desktop_S[ desktop_i ].window, window_i, 1 ))
        return ~2;
    E_windows_Q_desktop_S[ desktop_i ].window_n--;
    E_gui_Q_taskabar_S_redraw = yes;
    X_A( gui, draw );
    X_F( gui, draw );
    return 0;
}
/******************************************************************************/
