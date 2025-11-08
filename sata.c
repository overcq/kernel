/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         SATA AHCI driver
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”             2025‒11‒7 d
*******************************************************************************/
struct __attribute__ (( __packed__ )) E_sata_Z_port
{ N64 command_list_base;
  N64 fis_base;
  N32 interrupt_status, interrupt_enable;
  N32 command_status;
  N32 reserved_1;
  N32 task_file_data;
  N32 signature;
  N32 sata_status;
  N32 sata_control;
  N32 sata_error;
  N32 sata_active;
  N32 command_issue;
  N32 sata_notification;
  N32 fis_based_switch_control;
  N32 reserved_2[11];
  N32 vendor[4];
};
_internal
volatile struct __attribute__ (( __packed__ )) E_sata_Z_memory
{ N32 host_cap;
  N32 global_host_control;
  N32 interrupt_status;
  N32 port_implemented;
  N32 version;
  N32 command_completion_control;
  N32 command_completion_ports;
  N32 enclosure_management_location;
  N32 enclosure_management_control;
  N32 host_cap_2;
  N32 bios_os_handoff_control_status;
  N8 reserved[ 0xa0 - 0x2c ];
  N8 vendor[ 0x100 - 0xa0 ];
  struct E_sata_Z_port port[];
} *E_sata_S_memory;
struct __attribute__ (( __packed__ )) E_sata_Z_command_header
{ N8 length         :5;
  N8 atapi          :1;
  N8 write          :1;
  N8 prefechable    :1;
  N8 reset          :1;
  N8 bist           :1;
  N8 clear_busy     :1;
  N8 reserved_1     :1;
  N8 multiplier_port:1;
  N16 prdt_length;
  volatile N32 prdt_transferred;
  N64 ctd_base;
  N32 reserved_2[4];
};
//==============================================================================
_private
void
E_sata_I_init( P memory
){  E_sata_S_memory = memory;
    N32 port_implemented = E_sata_S_memory->port_implemented;
    for_n( port, 32 )
    {   if( port_implemented & 1 )
        {   E_font_I_print_nl();
            E_font_I_print_hex(port);
            E_font_I_print( ": " );
            N32 sata_status = E_sata_S_memory->port[port].sata_status;
            E_font_I_print_hex( sata_status );
            if(( sata_status & 0xf ) == 3
            && (( sata_status >> 8 ) & 0xf ) == 1
            )
                switch( E_sata_S_memory->port[port].signature )
                { case 0x101:
                        E_font_I_print( "\nSATA" );
                        break;
                  case 0xeb140101:
                        E_font_I_print( "\nSATAPI" );
                        break;
                  case 0xc33c0101:
                        E_font_I_print( "\nenclosure management bridge" );
                        break;
                  case 0x96690101:
                        E_font_I_print( "\nport multiplier" );
                        break;
                }
        }
        port_implemented >>= 1;
    }
}
/******************************************************************************/
