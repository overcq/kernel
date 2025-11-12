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
//------------------------------------------------------------------------------
struct __attribute__ (( __packed__ )) E_sata_Z_command_header
{ N8 length         :5;
  N8 atapi          :1;
  N8 write          :1;
  N8 prefechable    :1;
  N8 reset          :1;
  N8 bist           :1;
  N8 clear_busy     :1;
  N8 reserved_1     :1;
  N8 multiplier_port:4;
  N16 prdt_length;
  volatile N32 prdt_transferred;
  N64 ctd_base;
  N32 reserved_2[4];
};
//------------------------------------------------------------------------------
enum E_sata_Z_fis_type
{ E_sata_Z_fis_type_S_register_h2d = 0x27
, E_sata_Z_fis_type_S_register_d2h = 0x34
, E_sata_Z_fis_type_S_dma_activate = 0x39
, E_sata_Z_fis_type_S_dma_setup = 0x41
, E_sata_Z_fis_type_S_data = 0x46
, E_sata_Z_fis_type_S_bist = 0x58
, E_sata_Z_fis_type_S_pio_setup = 0x5f
, E_sata_Z_fis_type_S_device_bits = 0xa1
};
struct __attribute__ (( __packed__ )) E_sata_Z_fis_register_h2d
{ N8 type;
  N8 multiplier     :4;
  N8 reserved_1     :3;
  N8 command_control:1;
  N8 command;
  N8 feature_low;
  N8 lba_0, lba_1, lba_2;
  N8 device;
  N8 lba_3, lba_4, lba_5;
  N8 feature_high;
  N16 count;
  N8 isochronous_completion;
  N8 control;
  N8 reserved_2[4];
};
struct __attribute__ (( __packed__ )) E_sata_Z_fis_register_d2h
{ N8 type;
  N8 multiplier :4;
  N8 reserved_1 :2;
  N8 interrupt  :1;
  N8 reserved_2 :1;
  N8 status;
  N8 error;
  N8 lba_0, lba_1, lba_2;
  N8 device;
  N8 lba_3, lba_4, lba_5;
  N8 reserved_3;
  N16 count;
  N8 reserved_4[6];
};
struct __attribute__ (( __packed__ )) E_sata_Z_fis_data
{ N8 type;
  N8 multiplier :4;
  N8 reserved_1 :4;
  N8 reserved_2[2];
  N32 data[];
};
struct __attribute__ (( __packed__ )) E_sata_Z_fis_pio_setup
{ N8 type;
  N8 multiplier :4;
  N8 reserved_1 :1;
  N8 direction  :1;
  N8 interrupt  :1;
  N8 reserved_2 :1;
  N8 status;
  N8 error;
  N8 lba_0, lba_1, lba_2;
  N8 device;
  N8 lba_3, lba_4, lba_5;
  N8 reserved_3;
  N16 count;
  N8 reserved_4;
  N8 status_new;
  N16 transfer_count;
  N8 reserved_5[2];
};
struct __attribute__ (( __packed__ )) E_sata_Z_fis_dma_setup
{ N8 type;
  N8 multiplier     :4;
  N8 reserved_1     :1;
  N8 direction      :1;
  N8 interrupt      :1;
  N8 auto_activate  :1;
  N8 reserved_2[2];
  N64 dma_buffer_id;
  N32 reserved_3;
  N32 dma_buffer_offset;
  N32 transfer_count;
  N32 reserved_4;
};
struct __attribute__ (( __packed__ )) E_sata_Z_fis_device_bits //NDFN
{ N8 type;
  N8 reserved[ 0x60 - 0x59 ];
};
struct __attribute__ (( __packed__ )) E_sata_Z_fis
{ struct E_sata_Z_fis_dma_setup dma_setup;
  N8 pad_1[4];
  struct E_sata_Z_fis_pio_setup pio_setup;
  N8 pad_2[12];
  struct E_sata_Z_fis_register_d2h register_d2h;
  N8 pad_3[4];
  struct E_sata_Z_fis_device_bits device_bits;
  N8 unknown_fis[64];
  N8 reserved[ 0x100 - 0xa0 ];
};
//==============================================================================
_private
void
E_sata_I_init( P memory
){  E_sata_S_memory = memory;
    if( E_sata_S_memory->global_host_control ^ ( 1 << 31 )) // AHCI enable
    {   E_sata_S_memory->global_host_control |= 1 << 31;
        if( E_sata_S_memory->global_host_control ^ ( 1 << 31 ))
            return;
    }
    N time;
    if( E_sata_S_memory->host_cap_2 & 1 ) // BIOS/OS handoff
    {   E_sata_S_memory->bios_os_handoff_control_status |= 1 << 1; // OS owned semaphore
        E_flow_Q_spin_time_M( &time, 25000 );
        while(( E_sata_S_memory->bios_os_handoff_control_status & 1 ) // BIOS owned semaphore
        && ( E_sata_S_memory->bios_os_handoff_control_status ^ ( 1 << 4 )) // BIOS busy
        && !E_flow_Q_spin_time_T( &time )
        )
            __asm__ volatile (
            "\n"    "pause"
            );
        if( E_sata_S_memory->bios_os_handoff_control_status & ( 1 << 4 )) // BIOS busy
        {   E_flow_Q_spin_time_M( &time, 2000000 );
            while(( E_sata_S_memory->bios_os_handoff_control_status & 1 ) // BIOS owned semaphore
            && !E_flow_Q_spin_time_T( &time )
            )
                __asm__ volatile (
                "\n"    "pause"
                );
        }
    }
    E_sata_S_memory->global_host_control |= 1; // reset
    E_flow_Q_spin_time_M( &time, 1000000 );
    while( E_sata_S_memory->global_host_control & 1 )
    {   if( E_flow_Q_spin_time_T( &time ))
            return;
        __asm__ volatile (
        "\n"    "pause"
        );
    }
    if( E_sata_S_memory->global_host_control ^ ( 1 << 31 ))
    {   E_sata_S_memory->global_host_control |= 1 << 31;
        if( E_sata_S_memory->global_host_control ^ ( 1 << 31 ))
            return;
    }
    N32 port_implemented = E_sata_S_memory->port_implemented;
    for_n( port, ( E_sata_S_memory->host_cap & 0x1f ) + 1 )
    {   if( port_implemented & 1 )
        {   E_sata_S_memory->port[port].sata_control |= 1;
            E_flow_I_sleep(1000);
            E_sata_S_memory->port[port].sata_control &= ~0xf;
            E_flow_Q_spin_time_M( &time, 100000 ); //NDFN Nie wiadomo, jaki czas oczekiwania.
            while(( E_sata_S_memory->port[port].sata_status & 0xf ) != 3 )
            {   if( E_flow_Q_spin_time_T( &time ))
                    goto Next_port;
                __asm__ volatile (
                "\n"    "pause"
                );
            }
            E_sata_S_memory->port[port].sata_error = ~0;
            switch( E_sata_S_memory->port[port].signature )
            { case 0x101:
                    E_font_I_print( "\nSATA" );
                    break;
              case 0xeb140101:
                    E_font_I_print( "\nSATAPI" );
                    E_sata_S_memory->port[port].command_status |= 1 << 24; // ATAPI
                    break;
              case 0xc33c0101:
                    E_font_I_print( "\nenclosure management bridge" );
                    break;
              case 0x96690101:
                    E_font_I_print( "\nport multiplier" );
                    break;
              default:
                    E_font_I_print( "\nother: " );
                    E_font_I_print_hex( E_sata_S_memory->port[port].signature );
                    break;
            }
            N command_list_n = ( E_sata_S_memory->host_cap >> 8 ) & 0x1f;
            P command_list_base = E_mem_Q_blk_M_align( command_list_n * sizeof( struct E_sata_Z_command_header ), 1024 );
            if( !command_list_base )
                E_main_I_error_fatal();
            P fis_base = E_mem_Q_blk_M_align( sizeof( struct E_sata_Z_fis ), 256 );
            if( !fis_base )
                E_main_I_error_fatal();
            N command_list_base_physical = (N)E_main_Z_p_I_to_physical( command_list_base );
            N fis_base_physical = (N)E_main_Z_p_I_to_physical( fis_base );
            if( E_sata_S_memory->host_cap & ( 1 << 31 ))
            {   E_sata_S_memory->port[port].command_list_base = command_list_base_physical;
                E_sata_S_memory->port[port].fis_base = fis_base_physical;
            }else
            {   if(( command_list_base_physical & 0xffff0000 )
                || ( fis_base_physical & 0xffff0000 )
                )
                    E_main_I_error_fatal();
                *( N32 * )&E_sata_S_memory->port[port].command_list_base = (N32)command_list_base_physical;
                *( N32 * )&E_sata_S_memory->port[port].fis_base = (N32)fis_base_physical;
            }
            *( N32 * )&E_sata_S_memory->port[port].command_status |= 1 << 4; // fis receive enable
            //TODO Konfiguracja przerwań.
            E_sata_S_memory->global_host_control |= 1 << 1; // interrupt enable
        }
Next_port:
        port_implemented >>= 1;
    }
}
/******************************************************************************/
