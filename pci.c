/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         PCI driver
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”             2025‒6‒25 T
*******************************************************************************/
_private
N32
E_pci_I_read( N8 bus
, N8 device
, N8 function
, N8 offset
){  E_main_I_out_32( 0xcf8, 0x80000000 | ( bus << 16 ) | ( device << 11 ) | ( function << 8 ) | offset );
    return E_main_I_in_32( 0xcfc );
}
_private
void
E_pci_I_write( N8 bus
, N8 device
, N8 function
, N8 offset
, N32 value
){  E_main_I_out_32( 0xcf8, 0x80000000 | ( bus << 16 ) | ( device << 11 ) | ( function << 8 ) | offset );
    E_main_I_out_32( 0xcfc, value );
}
/******************************************************************************/
