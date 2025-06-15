/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         keyboard driver
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒6‒5 Q
*******************************************************************************/

//==============================================================================
_internal
void
E_keyboard_I_interrupt( void
){  E_font_I_print( "\nkeyboard interrupt" );
    N8 k = E_main_I_inb( 0x60 );
}
_private
void
E_keyboard_M( void
){  E_interrupt_P( 1, &E_keyboard_I_interrupt );
}
/******************************************************************************/
