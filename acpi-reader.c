/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         ACPI reader
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒6‒3 N
*******************************************************************************/
_internal
N
E_acpi_reader_R_child( N parent_object_i
, struct E_acpi_aml_Z_pathname name
){  for( N object_i = parent_object_i + 1; object_i != E_acpi_aml_S_object_n; object_i++ )
    {   if( !( E_acpi_aml_S_object[ object_i ].name.n > E_acpi_aml_S_object[ parent_object_i ].name.n
          && E_text_Z_sl_T_eq( E_acpi_aml_S_object[ object_i ].name.s
          , E_acpi_aml_S_object[ parent_object_i ].name.s
          , J_min( E_acpi_aml_S_object[ object_i ].name.n, E_acpi_aml_S_object[ parent_object_i ].name.n ) * 4
        )))
            break;
        if( E_acpi_aml_S_object[ object_i ].name.n <= E_acpi_aml_S_object[ parent_object_i ].name.n )
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
//==============================================================================
_private
N
E_acpi_reader_M( void
){  for_n( i, E_acpi_aml_S_object_n )
        if( E_acpi_aml_S_object[i].type == E_acpi_aml_Z_object_Z_type_S_device )
        {   N object_i = E_acpi_reader_R_child( i, ( struct E_acpi_aml_Z_pathname ){ "_HID", 1 });
            if( ~object_i )
            {   Pc name_ = M( E_acpi_aml_S_object[i].name.n * 4 + 1 );
                E_text_Z_s_P_copy_sl_0( name_, E_acpi_aml_S_object[i].name.s, E_acpi_aml_S_object[i].name.n * 4 );
                E_font_I_print( "\n,device=" ); E_font_I_print( name_ );
                W( name_ );
                switch( E_acpi_aml_S_object[ object_i ].type )
                { case E_acpi_aml_Z_object_Z_type_S_number:
                        E_font_I_print( ",_HID=" ); E_font_I_print_hex( E_acpi_aml_S_object[ object_i ].n );
                        break;
                  case E_acpi_aml_Z_object_Z_type_S_string:
                        E_font_I_print( ",_HID=" ); E_font_I_print( E_acpi_aml_S_object[ object_i ].data );
                        break;
                  default:
                        E_font_I_print( ",type=" ); E_font_I_print_hex( E_acpi_aml_S_object[ object_i ].type );
                        break;
                }
            }
            object_i = E_acpi_reader_R_child( i, ( struct E_acpi_aml_Z_pathname ){ "_CID", 1 });
            if( ~object_i )
                switch( E_acpi_aml_S_object[ object_i ].type )
                { case E_acpi_aml_Z_object_Z_type_S_number:
                        E_font_I_print( ",_CID=" ); E_font_I_print_hex( E_acpi_aml_S_object[ object_i ].n );
                        break;
                  case E_acpi_aml_Z_object_Z_type_S_string:
                        E_font_I_print( ",_CID=" ); E_font_I_print( E_acpi_aml_S_object[ object_i ].data );
                        break;
                  case E_acpi_aml_Z_object_Z_type_S_package:
                    {   struct E_acpi_aml_Z_package *package = E_acpi_aml_S_object[ object_i ].data;
                        for_n( j, package->n )
                            switch( package->value[j].type )
                            { case E_acpi_aml_Z_value_Z_type_S_number:
                                    E_font_I_print( ",_CID=" ); E_font_I_print_hex( package->value[j].n );
                                    break;
                              case E_acpi_aml_Z_value_Z_type_S_string:
                                    E_font_I_print( ",_CID=" ); E_font_I_print( package->value[j].p );
                                    break;
                              default:
                                    E_font_I_print( ",type=" ); E_font_I_print_hex( package->value[j].type );
                                    break;
                            }
                        break;
                    }
                  default:
                        E_font_I_print( ",type=" ); E_font_I_print_hex( E_acpi_aml_S_object[ object_i ].type );
                        break;
                }
        }
    return 0;
}
/******************************************************************************/
