/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         ACPI reader
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒6‒3 N
*******************************************************************************/
struct __attribute__ (( __packed__ )) E_acpi_reader_Z_local_apic
{ N8 type;
  N8 l;
  N8 processor;
  N8 id;
  N32 flags;
};
struct __attribute__ (( __packed__ )) E_acpi_reader_Z_io_apic
{ N8 type;
  N8 l;
  N8 id;
  N8 reserved;
  N32 address;
  N32 gsi_base;
};
struct __attribute__ (( __packed__ )) E_acpi_reader_Z_source_override
{ N8 type;
  N8 l;
  N8 bus;
  N8 source;
  N32 gsi;
  N16 flags;
};
struct __attribute__ (( __packed__ )) E_acpi_reader_Z_local_apic_nmi
{ N8 type;
  N8 l;
  N8 processor;
  N16 flags;
  N8 int_;
};
//==============================================================================
_private
P
E_acpi_reader_Z_p_I_to_virtual( P p
){  N min = 0;
    N max = E_main_S_kernel.memory_map_n - 1;
    N i = max / 2;
    O{  if( (N)p >= E_main_S_kernel.memory_map[i].physical_start
        && (N)p < E_main_S_kernel.memory_map[i].physical_start + E_main_S_kernel.memory_map[i].pages * E_mem_S_page_size
        )
            return (P)( E_main_S_kernel.memory_map[i].virtual_start + ( (N)p - E_main_S_kernel.memory_map[i].physical_start ));
        if( E_main_S_kernel.memory_map[i].physical_start > (N)p )
        {   if( i == min )
                break;
            max = i - 1;
            i = max - ( i - min ) / 2;
        }else
        {   if( i == max )
                break;
            min = i + 1;
            i = min + ( max - i ) / 2;
        }
    }
    return 0;
}
_internal
void
E_acpi_reader_M_madt_I_source_override_sort( void
){  N n = E_main_S_kernel.apic_source_override_n;
    while( n > 1 )
    {   struct E_apic_Z_source_override *entry_prev = &E_main_S_kernel.apic_source_override[0];
        N new_n = 0;
        for( N i = 1; i != n; i++ )
        {   if( entry_prev->source > E_main_S_kernel.apic_source_override[i].source )
            {   J_swap( struct E_apic_Z_source_override, *entry_prev, E_main_S_kernel.apic_source_override[i] );
                new_n = i;
            }
            entry_prev = &E_main_S_kernel.apic_source_override[i];
        }
        n = new_n;
    }
}
_private
N
E_acpi_reader_M_madt( Pc table
, N l
){  E_main_S_kernel.local_apic_address = 0;
    while(l)
    {   if( l < (N8)table[1] )
            return ~0;
        switch( (N8)table[0] )
        { case 0: // local APIC
            {   struct E_acpi_reader_Z_local_apic *local_apic = (P)&table[0];
                if( local_apic->l != sizeof( *local_apic ))
                    return ~0;
                if( !(( local_apic->flags & 3 ) == 1
                  || ( local_apic->flags & 3 ) == 2
                )
                || local_apic->flags & ~3
                )
                    return ~0;
                break;
            }
          case 1: // I/O APIC
            {   if( E_main_S_kernel.local_apic_address ) // Obsługiwany tylko jeden kontroler I/O APIC.
                    return ~0;
                struct E_acpi_reader_Z_io_apic *io_apic = (P)&table[0];
                if( io_apic->l != sizeof( *io_apic ))
                    return ~0;
                E_main_S_kernel.local_apic_address = (P)(N)io_apic->address;
                if( io_apic->gsi_base > 255 )
                    return ~0;
                E_main_S_kernel.io_apic_base = io_apic->gsi_base;
                break;
            }
          case 2: // source override
            {   struct E_acpi_reader_Z_source_override *source_override = (P)&table[0];
                if( source_override->l != sizeof( *source_override ))
                    return ~0;
                N n_prepended;
                if( !E_mem_Q_blk_I_add( &E_main_S_kernel.apic_source_override, 1, &n_prepended, 0 ))
                    return ~0;
                N i = n_prepended ? 0 : E_main_S_kernel.apic_source_override_n;
                E_main_S_kernel.apic_source_override[i].source = source_override->source;
                E_main_S_kernel.apic_source_override[i].gsi = source_override->gsi;
                E_main_S_kernel.apic_source_override_n++;
                break;
            }
          case 4: // local APIC NMI
            {   struct E_acpi_reader_Z_local_apic_nmi *local_apic_nmi = (P)&table[0];
                if( local_apic_nmi->l != sizeof( *local_apic_nmi ))
                    return ~0;
                break;
            }
          default:
                return ~0;
        }
        l -= (N8)table[1];
        table += (N8)table[1];
    }
    // Sprawdzenie, czy nie ma duplikatów ‘source override’.
    if( E_main_S_kernel.apic_source_override_n > 1 )
    {   E_acpi_reader_M_madt_I_source_override_sort();
        N source = E_main_S_kernel.apic_source_override[0].source;
        for_n( i, E_main_S_kernel.apic_source_override_n - 1 )
        {   if( E_main_S_kernel.apic_source_override[ 1 + i ].source == source )
                return ~0;
            source = E_main_S_kernel.apic_source_override[ 1 + i ].source;
        }
    }
    return 0;
}
/******************************************************************************/
