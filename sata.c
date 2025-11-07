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
  N32 fis_base_switch_control;
  N32 vendor[4];
};
struct __attribute__ (( __packed__ )) E_sata_Z_memory
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
};
//==============================================================================

/******************************************************************************/
