/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         ACPI reader
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒6‒3 N
*******************************************************************************/
//TODO Konfiguracja z “_PRS” rozpoczęta i nie skończona; brak platformy testowej.
//==============================================================================
struct E_acpi_reader_Z_resources
{ N *irq;
  N irq_n;
  B bus_polarity_low;
  B bus_trigger_edge;
};
struct __attribute__ (( __packed__ )) E_acpi_reader_Z_crs_Z_irq
{ N8 type_name_length;
  N16 irq_mask;
  N8 info;
};
struct __attribute__ (( __packed__ )) E_acpi_reader_Z_crs_Z_end
{ N8 type_name_length;
  N8 checksum;
};
//==============================================================================
_internal
N
E_acpi_reader_R_child( N parent_object_i
, struct E_acpi_aml_Z_pathname name
){  for( N object_i = parent_object_i + 1; object_i != E_acpi_aml_S_object_n; object_i++ )
    {   if( !( E_acpi_aml_S_object[ object_i ].name.n > E_acpi_aml_S_object[ parent_object_i ].name.n
          && E_text_Z_sl_T_eq( E_acpi_aml_S_object[ object_i ].name.s
          , E_acpi_aml_S_object[ parent_object_i ].name.s
          , E_acpi_aml_S_object[ parent_object_i ].name.n * 4
        )))
            break;
        S cmp = E_text_Z_sl_T_cmp( E_acpi_aml_S_object[ object_i ].name.s + E_acpi_aml_S_object[ parent_object_i ].name.n * 4
        , name.s
        , J_min( E_acpi_aml_S_object[ object_i ].name.n - E_acpi_aml_S_object[ parent_object_i ].name.n, name.n ) * 4
        );
        if( cmp > 0 )
            break;
        if( cmp < 0 )
            continue;
        if( E_acpi_aml_S_object[ object_i ].name.n > E_acpi_aml_S_object[ parent_object_i ].name.n + name.n )
            break;
        if( E_acpi_aml_S_object[ object_i ].name.n == E_acpi_aml_S_object[ parent_object_i ].name.n + name.n )
            return object_i;
    }
    return ~0;
}
_internal
N
E_acpi_reader_R_parent( N child_object_i
){  if( E_acpi_aml_S_object[ child_object_i ].name.n > 1 )
        for( N object_i = child_object_i - 1; ~object_i; object_i-- )
        {   if( !( E_acpi_aml_S_object[ object_i ].name.n >= E_acpi_aml_S_object[ child_object_i ].name.n - 1
              && E_text_Z_sl_T_eq( E_acpi_aml_S_object[ object_i ].name.s
              , E_acpi_aml_S_object[ child_object_i ].name.s
              , ( E_acpi_aml_S_object[ child_object_i ].name.n - 1 ) * 4
            )))
                break;
            if( E_acpi_aml_S_object[ object_i ].name.n == E_acpi_aml_S_object[ child_object_i ].name.n - 1 )
                return object_i;
        }
    return ~0;
}
_internal
N
E_acpi_reader_I_next_sibling( N parent_object_i
){  for( N object_i = parent_object_i + 1; object_i != E_acpi_aml_S_object_n; object_i++ )
    {   if( !( E_acpi_aml_S_object[ object_i ].name.n >= E_acpi_aml_S_object[ parent_object_i ].name.n
          && E_text_Z_sl_T_eq( E_acpi_aml_S_object[ object_i ].name.s
          , E_acpi_aml_S_object[ parent_object_i ].name.s
          , ( E_acpi_aml_S_object[ parent_object_i ].name.n - 1 ) * 4
        )))
            break;
        if( E_acpi_aml_S_object[ object_i ].name.n == E_acpi_aml_S_object[ parent_object_i ].name.n )
            return object_i;
    }
    return ~0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
struct E_acpi_aml_Z_pathname
E_acpi_reader_I_child_build( N parent_object_i
, Pc name
){  Pc name_ = M(( E_acpi_aml_S_object[ parent_object_i ].name.n + 1 ) * 4 );
    E_mem_Q_blk_I_copy( name_, E_acpi_aml_S_object[ parent_object_i ].name.s, E_acpi_aml_S_object[ parent_object_i ].name.n * 4 );
    E_mem_Q_blk_I_copy( name_ + E_acpi_aml_S_object[ parent_object_i ].name.n * 4, name, 4 );
    return ( struct E_acpi_aml_Z_pathname ){ name_, E_acpi_aml_S_object[ parent_object_i ].name.n + 1 };
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
void
E_acpi_reader_I_child_print( N parent_object_i
){  for( N object_i = parent_object_i + 1; object_i != E_acpi_aml_S_object_n; object_i++ )
    {   if( !( E_acpi_aml_S_object[ object_i ].name.n > E_acpi_aml_S_object[ parent_object_i ].name.n
          && E_text_Z_sl_T_eq( E_acpi_aml_S_object[ object_i ].name.s
          , E_acpi_aml_S_object[ parent_object_i ].name.s
          , E_acpi_aml_S_object[ parent_object_i ].name.n * 4
        )))
            break;
        if( E_acpi_aml_S_object[ object_i ].name.n == E_acpi_aml_S_object[ parent_object_i ].name.n + 1 )
        {   Pc name = M( 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name, E_acpi_aml_S_object[ object_i ].name.s + ( E_acpi_aml_S_object[ object_i ].name.n - 1 ) * 4, 4 );
            E_font_I_print( "," ); E_font_I_print(name);
            W(name);
        }
    }
}
//==============================================================================
_internal
N
E_acpi_reader_M_prs( struct E_acpi_reader_Z_resources *resources
, struct E_acpi_aml_Z_value *prs_value
, struct E_acpi_aml_Z_value *crs_value
){  N prs_conf_n = 0;
    P *Mt_( prs_conf, prs_conf_n );
    if( !prs_conf )
        return ~0;
    N prs_dependent_conf_n = 0;
    P *Mt_( prs_dependent_conf, prs_dependent_conf_n );
    if( !prs_dependent_conf )
    {   W( prs_conf );
        return ~0;
    }
    N ret;
    Pc table = prs_value->buffer.p;
    N l = prs_value->buffer.n;
    B end_tag = no;
    B inside_dependent = no;
    N8 dependent_performance = ~0;
    while(l)
    {   N l_;
        if( (N8)table[0] & 0x80 )
        {   l_ = 1 + ( table[1] | ( (N)table[2] << 8 ));
            if( l < l_ )
            {   ret = ~1;
                goto Error;
            }
            switch( table[0] & 0x7f )
            { case 1: // 24‐bit memory range
                    break;
              case 2: // generic register
                    break;
              case 4: // vendor defined
                    break;
              case 5: // 32‐bit memory range
                    break;
              case 6: // 32‐bit fixed memory range
                    break;
              case 7: // address space resource
                    break;
              case 8: // word address space
                    break;
              case 9: // extended interrupt
                    break;
              case 0xa: // qword address space
                    break;
              case 0xb: // extended address space
                    break;
              case 0xc: // GPIO connection
                    break;
              case 0xd: // pin function
                    break;
              case 0xe: // GSB connection
                    break;
              case 0xf: // pin configuration
                    break;
              case 0x10: // pin group
                    break;
              case 0x11: // pin group function
                    break;
              case 0x12: // pin group configuration
                    break;
              case 0x13: // clock input resource
                    break;
              default:
                    break;
            }
        }else
        {   l_ = 1 + table[0] & 7;
            if( l < l_ )
            {   ret = ~1;
                goto Error;
            }
            switch(( table[0] >> 3 ) & 0xf )
            { case 4: // IRQ
                {   if( l_ != 2
                    && l_ != 3
                    )
                    {   ret = ~1;
                        goto Error;
                    }
                    struct E_acpi_reader_Z_crs_Z_irq *irq = (P)&table[0];
                    if( !~E_asm_I_bsf( irq->irq_mask ))
                    {   ret = ~1;
                        goto Error;
                    }
                    for_n( i, prs_conf_n )
                    {   struct E_acpi_reader_Z_crs_Z_irq *irq_ = prs_conf[i];
                        if((( irq_->type_name_length >> 3 ) & 0xf ) == 4 )
                        {   ret = ~1;
                            goto Error;
                        }
                    }
                    for_n_( i, prs_dependent_conf_n )
                    {   struct E_acpi_reader_Z_crs_Z_irq *irq_ = prs_dependent_conf[i];
                        if((( irq_->type_name_length >> 3 ) & 0xf ) == 4 )
                        {   ret = ~1;
                            goto Error;
                        }
                    }
                    P **prs_conf_ = inside_dependent ? &prs_dependent_conf : &prs_conf;
                    N *prs_conf_n_ = inside_dependent ? &prs_dependent_conf_n : &prs_conf_n;
                    N prepended;
                    if( !E_mem_Q_blk_I_add( prs_conf_, 1, &prepended, 0 ))
                    {   ret = ~0;
                        goto Error;
                    }
                    ( *prs_conf_n_ )++;
                    struct E_acpi_reader_Z_crs_Z_irq *irq_ = ( *prs_conf_ )[ prepended ? 0 : *prs_conf_n_ - 1 ];
                    *irq_ = *irq;
                    if( l_ == 2 )
                        irq_->info = 1;
                    break;
                }
              case 5: // DMA
                    break;
              case 6: // start dependent functions
                    if( inside_dependent )
                    {   ret = ~1;
                        goto Error;
                    }
                    inside_dependent = yes;
                    if(( l_ != 0
                      && l_ != 1
                    )
                    || ( table[1] & 0xf0 )
                    || ( l_ == 1
                      && (( table[1] & 3 ) == 3
                        || ( table[1] >> 2 ) == 3
                    )))
                    {   ret = ~1;
                        goto Error;
                    }
                    if( l_ == 1 )
                    {   if( dependent_performance > ( table[1] >> 2 ))
                        {   dependent_performance = table[1] >> 2;
                            for_n( i, prs_dependent_conf_n )
                                W( prs_dependent_conf[i] );
                            if( !E_mem_Q_blk_I_remove( &prs_dependent_conf, 0, prs_dependent_conf_n ))
                            {   prs_dependent_conf_n = 0;
                                ret = ~0;
                                goto Error;
                            }
                            prs_dependent_conf_n = 0;
                        }
                    }else if( dependent_performance > 1 )
                    {   dependent_performance = 1;
                        for_n( i, prs_dependent_conf_n )
                            W( prs_dependent_conf[i] );
                        if( !E_mem_Q_blk_I_remove( &prs_dependent_conf, 0, prs_dependent_conf_n ))
                        {   prs_dependent_conf_n = 0;
                            ret = ~0;
                            goto Error;
                        }
                        prs_dependent_conf_n = 0;
                    }
                    break;
              case 7: // end dependent functions
                    if( !inside_dependent )
                    {   ret = ~1;
                        goto Error;
                    }
                    inside_dependent = no;
                    break;
              case 8: // I/O port
                    break;
              case 9: // fixed location I/O port
                    break;
              case 0xa: // fixed DMA
                    break;
              case 0xe: // vendor defined
                    break;
              case 0xf: // end tag
                    if( inside_dependent
                    || l != l_
                    )
                    {   ret = ~1;
                        goto Error;
                    }
                    struct E_acpi_reader_Z_crs_Z_end *end = (P)&table[0];
                    if( end->checksum )
                    {   N8 checksum = end->checksum;
                        for_n( i, prs_value->buffer.n )
                            checksum += prs_value->buffer.p[i];
                        if(checksum)
                        {   ret = ~1;
                            goto Error;
                        }
                    }
                    end_tag = yes;
                    break;
              default:
                    ret = ~1;
                    goto Error;
            }
        }
        l -= l_;
        table += l_;
    }
    if( !end_tag )
    {   ret = ~1;
        goto Error;
    }
    table = crs_value->buffer.p;
    l = crs_value->buffer.n;
    end_tag = no;
    while(l)
    {   N l_;
        if( (N8)table[0] & 0x80 )
        {   l_ = 1 + ( table[1] | ( (N)table[2] << 8 ));
            if( l < l_ )
            {   ret = ~1;
                goto Error;
            }
            switch( table[0] & 0x7f )
            { case 1: // 24‐bit memory range
                    break;
              case 2: // generic register
                    break;
              case 4: // vendor defined
                    break;
              case 5: // 32‐bit memory range
                    break;
              case 6: // 32‐bit fixed memory range
                    break;
              case 7: // address space resource
                    break;
              case 8: // word address space
                    break;
              case 9: // extended interrupt
                    break;
              case 0xa: // qword address space
                    break;
              case 0xb: // extended address space
                    break;
              case 0xc: // GPIO connection
                    break;
              case 0xd: // pin function
                    break;
              case 0xe: // GSB connection
                    break;
              case 0xf: // pin configuration
                    break;
              case 0x10: // pin group
                    break;
              case 0x11: // pin group function
                    break;
              case 0x12: // pin group configuration
                    break;
              case 0x13: // clock input resource
                    break;
              default:
                    break;
            }
        }else
        {   l_ = 1 + table[0] & 7;
            if( l < l_ )
            {   ret = ~1;
                goto Error;
            }
            switch(( table[0] >> 3 ) & 0xf )
            { case 4: // IRQ
                {   if( l_ != 2
                    && l_ != 3
                    )
                    {   ret = ~1;
                        goto Error;
                    }
                    B dependent;
                    struct E_acpi_reader_Z_crs_Z_irq *irq = (P)&table[0];
                    struct E_acpi_reader_Z_crs_Z_irq *irq_;
                    for_n( i, prs_conf_n )
                    {   irq_ = prs_conf[i];
                        if((( irq_->type_name_length >> 3 ) & 0xf ) == 4 )
                        {   dependent = no;
                            break;
                        }
                    }
                    if( i == prs_conf_n )
                    {   for_n_( i, prs_dependent_conf_n )
                        {   irq_ = prs_dependent_conf[i];
                            if((( irq_->type_name_length >> 3 ) & 0xf ) == 4 )
                            {   dependent = yes;
                                break;
                            }
                        }
                        if( i == prs_dependent_conf_n )
                        {   ret = ~1;
                            goto Error;
                        }
                    }
                    N8 irq_i = E_asm_I_bsf( irq->irq_mask );
                    if( !( irq->info & 0x10 )) // not shared
                    {   for_n( j, resources->irq_n )
                            if( resources->irq[j] == irq_i )
                                break;
                        if( j != resources->irq_n )
                            continue;
                    }
                    for_n( j, E_interrupt_S_gsi_n )
                        if( E_interrupt_S_gsi[j].source == irq_i )
                            break;
                    if( j == E_interrupt_S_gsi_n )
                    {   ret = ~1;
                        goto Error;
                    }
                    if( E_interrupt_S_gsi[j].flags & 3 )
                    {   if( !!( irq->info & 8 ) != (( E_interrupt_S_gsi[j].flags & 3 ) == 3 ))
                        {   ret = ~1;
                            goto Error;
                        }
                    }else
                        if( !!( irq->info & 8 ) != resources->bus_polarity_low )
                        {   ret = ~1;
                            goto Error;
                        }
                    if(( E_interrupt_S_gsi[j].flags >> 2 ) & 3 )
                    {   if(( irq->info & 1 ) != ((( E_interrupt_S_gsi[j].flags >> 2 ) & 3 ) == 1 ))
                        {   ret = ~1;
                            goto Error;
                        }
                    }else
                        if(( irq->info & 1 ) != resources->bus_trigger_edge )
                        {   ret = ~1;
                            goto Error;
                        }
                    if( !E_mem_Q_blk_I_remove( dependent ? &prs_dependent_conf : &prs_conf, i, 1 ))
                    {   ret = ~0;
                        goto Error;
                    }
                    if(dependent)
                        prs_dependent_conf_n--;
                    else
                        prs_conf_n--;
                    irq_->irq_mask = 1 << irq_i;
                    break;
                }
              case 5: // DMA
                    break;
              case 8: // I/O port
                    break;
              case 9: // fixed location I/O port
                    break;
              case 0xa: // fixed DMA
                    break;
              case 0xe: // vendor defined
                    break;
              case 0xf: // end tag
                {   if( inside_dependent
                    || l != l_
                    )
                    {   ret = ~1;
                        goto Error;
                    }
                    struct E_acpi_reader_Z_crs_Z_end *end = (P)&table[0];
                    if( end->checksum )
                    {   N8 checksum = end->checksum;
                        for_n( i, prs_value->buffer.n )
                            checksum += prs_value->buffer.p[i];
                        if(checksum)
                        {   ret = ~1;
                            goto Error;
                        }
                    }
                    end_tag = yes;
                    break;
                }
              default:
                    ret = ~1;
                    goto Error;
            }
        }
        l -= l_;
        table += l_;
    }
    if( prs_dependent_conf_n
    || prs_conf_n
    )
    {   ret = ~1;
        goto Error;
    }
    W( prs_dependent_conf );
    W( prs_conf );
    return 0;
Error:
    for_n( i, prs_dependent_conf_n )
        W( prs_dependent_conf[i] );
    W( prs_dependent_conf );
    for_n_( i, prs_conf_n )
        W( prs_conf[i] );
    W( prs_conf );
    return ret;
}
_private
N
E_acpi_reader_M( void
){  // Inicjowanie urządzeń.
    for_n( i, E_acpi_aml_S_object_n )
        if( E_acpi_aml_S_object[i].type == E_acpi_aml_Z_object_Z_type_S_device )
        {   N ini_i = E_acpi_reader_R_child( i, ( struct E_acpi_aml_Z_pathname ){ "_INI", 1 });
            if( !~ini_i
            || E_acpi_aml_S_object[ ini_i ].type != E_acpi_aml_Z_object_Z_type_S_procedure
            )
                continue;
            struct E_acpi_aml_Z_pathname sta = E_acpi_reader_I_child_build( i, "_STA" );
            struct E_acpi_aml_Z_value result;
            N ret = E_acpi_aml_I_procedure( sta, &result, 0, 0 );
            W( sta.s );
            if( !~ret )
                return ~0;
            if( ret == ~1 )
                continue;
            if( ret != ~2 )
            {   if( result.type == E_acpi_aml_Z_value_Z_type_S_pathname )
                {   N object_i = E_acpi_aml_Q_object_R( result.pathname );
                    E_acpi_aml_Q_value_W( &result );
                    if( ~object_i )
                        if( E_acpi_aml_S_object[ object_i ].type == E_acpi_aml_Z_object_Z_type_S_number )
                            result = E_acpi_aml_Q_object_I_to_value( object_i );
                        else
                            result.type = E_acpi_aml_Z_value_Z_type_S_uninitialized;
                    else
                    {   result.n = 0;
                        result.type = E_acpi_aml_Z_value_Z_type_S_number;
                    }
                }else if( result.type != E_acpi_aml_Z_value_Z_type_S_number )
                {   E_acpi_aml_Q_value_W( &result );
                    result.type = E_acpi_aml_Z_value_Z_type_S_uninitialized;
                }
                if( result.type == E_acpi_aml_Z_value_Z_type_S_uninitialized
                || (( result.n & 1 ) == 0 // not present
                  && ( result.n & 8 ) == 0 // not functional
                ))
                {   i = E_acpi_reader_I_next_sibling(i) - 1;
                    continue;
                }
                if(( result.n & 1 ) == 0 // not present
                && ( result.n & 8 ) == 1 // functional
                )
                    continue;
            }
            ret = E_acpi_aml_I_procedure_( ini_i, &result, 0, 0 );
            if( !~ret )
                return ~0;
        }
    // Konfigurowanie urządzeń.
    struct E_acpi_reader_Z_resources resources;
    resources.irq_n = 0;
    Mt_( resources.irq, resources.irq_n );
    if( !resources.irq )
        return ~0;
    resources.bus_polarity_low = yes; //NDFN Tymczasowo dla ISA.
    resources.bus_trigger_edge = yes; //NDFN Tymczasowo dla ISA.
    for_n_( i, E_acpi_aml_S_object_n )
        if( E_acpi_aml_S_object[i].type == E_acpi_aml_Z_object_Z_type_S_device )
        {   struct E_acpi_aml_Z_value result;
            struct E_acpi_aml_Z_pathname sta = E_acpi_reader_I_child_build( i, "_STA" );
            N ret = E_acpi_aml_I_procedure( sta, &result, 0, 0 );
            W( sta.s );
            if( !~ret )
            {   W( resources.irq );
                return ~0;
            }
            if( ret == ~1 )
                continue;
            if( ret != ~2 )
            {   if( result.type == E_acpi_aml_Z_value_Z_type_S_pathname )
                {   N object_i = E_acpi_aml_Q_object_R( result.pathname );
                    E_acpi_aml_Q_value_W( &result );
                    if( ~object_i )
                        if( E_acpi_aml_S_object[ object_i ].type == E_acpi_aml_Z_object_Z_type_S_number )
                            result = E_acpi_aml_Q_object_I_to_value( object_i );
                        else
                            result.type = E_acpi_aml_Z_value_Z_type_S_uninitialized;
                    else
                    {   result.n = 0;
                        result.type = E_acpi_aml_Z_value_Z_type_S_number;
                    }
                }else if( result.type != E_acpi_aml_Z_value_Z_type_S_number )
                {   E_acpi_aml_Q_value_W( &result );
                    result.type = E_acpi_aml_Z_value_Z_type_S_uninitialized;
                }
                if( result.type == E_acpi_aml_Z_value_Z_type_S_uninitialized
                || (( result.n & 1 ) == 0 // not present
                  && ( result.n & 8 ) == 0 // not functional
                ))
                {   i = E_acpi_reader_I_next_sibling(i) - 1;
                    continue;
                }
                if(( result.n & 1 ) == 0 // not present
                || ( result.n & 8 ) == 0 // not functional
                )
                    continue;
            }else
            {   result.n = 0xb; // present, enabled, functional
                result.type = E_acpi_aml_Z_value_Z_type_S_number;
            }
            struct E_acpi_aml_Z_pathname crs = E_acpi_reader_I_child_build( i, "_CRS" );
            N object_i = E_acpi_aml_Q_object_R(crs);
            W( crs.s );
            struct E_acpi_aml_Z_value crs_value;
            if( ~object_i
            && E_acpi_aml_S_object[ object_i ].type == E_acpi_aml_Z_object_Z_type_S_buffer
            )
                crs_value = E_acpi_aml_Q_object_I_to_value( object_i );
            else
                crs_value.type = E_acpi_aml_Z_value_Z_type_S_uninitialized;
            if( crs_value.type == E_acpi_aml_Z_value_Z_type_S_uninitialized )
            {   E_acpi_aml_Q_value_W( &crs_value );
                continue;
            }
            struct E_acpi_aml_Z_pathname prs = E_acpi_reader_I_child_build( i, "_PRS" );
            object_i = E_acpi_aml_Q_object_R(prs);
            W( prs.s );
            struct E_acpi_aml_Z_value prs_value;
            if( ~object_i
            && E_acpi_aml_S_object[ object_i ].type == E_acpi_aml_Z_object_Z_type_S_buffer
            )
                prs_value = E_acpi_aml_Q_object_I_to_value( object_i );
            else
                prs_value.type = E_acpi_aml_Z_value_Z_type_S_uninitialized;
            if( prs_value.type != E_acpi_aml_Z_value_Z_type_S_uninitialized )
            {   N ret = E_acpi_reader_M_prs( &resources, &prs_value, &crs_value );
                E_acpi_aml_Q_value_W( &prs_value );
                if( !~ret )
                {   E_acpi_aml_Q_value_W( &crs_value );
                    W( resources.irq );
                    return ~0;
                }
                if( ret == ~1 )
                {   E_acpi_aml_Q_value_W( &crs_value );
                    continue;
                }
                struct E_acpi_aml_Z_pathname srs = E_acpi_reader_I_child_build( i, "_SRS" );
                ret = E_acpi_aml_I_procedure( srs, &result, 1, &crs_value );
                W( srs.s );
                if( !~ret )
                {   E_acpi_aml_Q_value_W( &crs_value );
                    W( resources.irq );
                    return ~0;
                }
                if( ret == ~1
                || ret == ~2
                )
                {   E_acpi_aml_Q_value_W( &crs_value );
                    continue;
                }
            }else
                if(( result.n & 2 ) == 0 ) // not enabled
                {   struct E_acpi_aml_Z_pathname srs = E_acpi_reader_I_child_build( i, "_SRS" );
                    N ret = E_acpi_aml_I_procedure( srs, &result, 1, &crs_value );
                    W( srs.s );
                    if( !~ret )
                    {   E_acpi_aml_Q_value_W( &crs_value );
                        W( resources.irq );
                        return ~0;
                    }
                    if( ret == ~1
                    || ret == ~2
                    )
                    {   E_acpi_aml_Q_value_W( &crs_value );
                        continue;
                    }
                }
            //TODO Przypisanie zasobów urządzenia.
            
            E_acpi_aml_Q_value_W( &crs_value );
        }
    W( resources.irq );
    // Diagnostyczne wypisanie urządzeń.
    for_n_( i, E_acpi_aml_S_object_n )
        if( E_acpi_aml_S_object[i].type == E_acpi_aml_Z_object_Z_type_S_device )
        {   N hid_i = E_acpi_reader_R_child( i, ( struct E_acpi_aml_Z_pathname ){ "_HID", 1 });
            if( ~hid_i )
            {   Pc name_ = M( E_acpi_aml_S_object[i].name.n * 4 + 1 );
                E_text_Z_s_P_copy_sl_0( name_, E_acpi_aml_S_object[i].name.s, E_acpi_aml_S_object[i].name.n * 4 );
                E_font_I_print( "\n,device=" ); E_font_I_print( name_ );
                W( name_ );
                E_font_I_print( ",_HID=" );
                switch( E_acpi_aml_S_object[ hid_i ].type )
                { case E_acpi_aml_Z_object_Z_type_S_number:
                        E_font_I_print_hex( E_acpi_aml_S_object[ hid_i ].n );
                        break;
                  case E_acpi_aml_Z_object_Z_type_S_string:
                        E_font_I_print( E_acpi_aml_S_object[ hid_i ].data );
                        break;
                  default:
                        E_font_I_print( ",type=" ); E_font_I_print_hex( E_acpi_aml_S_object[ hid_i ].type );
                        break;
                }
                N cid_i = E_acpi_reader_R_child( i, ( struct E_acpi_aml_Z_pathname ){ "_CID", 1 });
                if( ~cid_i )
                {   E_font_I_print( ",_CID=" );
                    switch( E_acpi_aml_S_object[ cid_i ].type )
                    { case E_acpi_aml_Z_object_Z_type_S_number:
                            E_font_I_print_hex( E_acpi_aml_S_object[ cid_i ].n );
                            break;
                      case E_acpi_aml_Z_object_Z_type_S_string:
                            E_font_I_print( E_acpi_aml_S_object[ cid_i ].data );
                            break;
                      case E_acpi_aml_Z_object_Z_type_S_package:
                        {   struct E_acpi_aml_Z_package *package = E_acpi_aml_S_object[ cid_i ].data;
                            for_n( j, package->n )
                                switch( package->value[j].type )
                                { case E_acpi_aml_Z_value_Z_type_S_number:
                                        E_font_I_print_hex( package->value[j].n );
                                        break;
                                  case E_acpi_aml_Z_value_Z_type_S_string:
                                        E_font_I_print( package->value[j].p );
                                        break;
                                  default:
                                        E_font_I_print( ",type=" ); E_font_I_print_hex( package->value[j].type );
                                        break;
                                }
                            break;
                        }
                      default:
                            E_font_I_print( ",type=" ); E_font_I_print_hex( E_acpi_aml_S_object[ cid_i ].type );
                            break;
                    }
                }
                E_acpi_reader_I_child_print(i);
            }
        }
    return 0;
}
/******************************************************************************/
