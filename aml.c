/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         AML interpreter
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”             2025‒5‒13 I
*******************************************************************************/
//TODO Przygotować maszynę stanów do interpretacji programów zawartych w AML.
//TODO Pamięć przydzielana dla “return” nie jest zwalniana.
#include "main.h"
//==============================================================================
struct E_aml_Z_pathname
{ Pc s;
  N8 n;
};
_internal struct E_aml_Z_pathname *E_aml_S_current_path;
_internal N E_aml_S_current_path_n;
_internal
struct E_aml_S_object_name_alias_Z
{ struct E_aml_Z_pathname alias; // Tablica posortowana według aliasu.
  struct E_aml_Z_pathname name;
} *E_aml_S_object_name_alias;
_internal
struct E_aml_S_object_Z
{ struct E_aml_Z_pathname name; // Tablica posortowana według nazwy.
  N8 arg_count;
} *E_aml_S_object; //TODO Obecnie przechwowywane są tylko metody.
_internal N E_aml_S_object_n;
_internal N E_aml_S_object_name_alias_n;
struct E_aml_Z_buffer
{ Pc buffer;
  N l;
};
//------------------------------------------------------------------------------
union E_aml_Z_stack_Z_result
{ struct E_aml_Z_pathname pathname;
  P p;
  N n;
};
enum E_aml_Z_stack_Z_entity
{ E_aml_Z_stack_Z_entity_S_result_to_n
, E_aml_Z_stack_Z_entity_S_restore_current_path
, E_aml_Z_stack_Z_entity_S_term
, E_aml_Z_stack_Z_entity_S_term_finish_1
, E_aml_Z_stack_Z_entity_S_term_finish_2
, E_aml_Z_stack_Z_entity_S_term_arg
, E_aml_Z_stack_Z_entity_S_term_arg_finish_1
, E_aml_Z_stack_Z_entity_S_object
, E_aml_Z_stack_Z_entity_S_expression
, E_aml_Z_stack_Z_entity_S_data_object
, E_aml_Z_stack_Z_entity_S_data_object_buffer_finish
, E_aml_Z_stack_Z_entity_S_package
, E_aml_Z_stack_Z_entity_S_package_finish
, E_aml_Z_stack_Z_entity_S_supername
, E_aml_Z_stack_Z_entity_S_supername_ref_finish
, E_aml_Z_stack_Z_entity_S_supername_deref_finish
, E_aml_Z_stack_Z_entity_S_supername_index_finish_1
, E_aml_Z_stack_Z_entity_S_supername_index_finish_2
, E_aml_Z_stack_Z_entity_S_supername_index_finish_3
, E_aml_Z_stack_Z_entity_S_bank_field_finish
, E_aml_Z_stack_Z_entity_S_bit_field_finish_1
, E_aml_Z_stack_Z_entity_S_bit_field_finish_2
, E_aml_Z_stack_Z_entity_S_byte_field_finish_1
, E_aml_Z_stack_Z_entity_S_byte_field_finish_2
, E_aml_Z_stack_Z_entity_S_dword_field_finish_1
, E_aml_Z_stack_Z_entity_S_dword_field_finish_2
, E_aml_Z_stack_Z_entity_S_field_finish_1
, E_aml_Z_stack_Z_entity_S_field_finish_2
, E_aml_Z_stack_Z_entity_S_field_finish_3
, E_aml_Z_stack_Z_entity_S_qword_field_finish_1
, E_aml_Z_stack_Z_entity_S_qword_field_finish_2
, E_aml_Z_stack_Z_entity_S_word_field_finish_1
, E_aml_Z_stack_Z_entity_S_word_field_finish_2
, E_aml_Z_stack_Z_entity_S_data_region_finish_1
, E_aml_Z_stack_Z_entity_S_data_region_finish_2
, E_aml_Z_stack_Z_entity_S_data_region_finish_3
, E_aml_Z_stack_Z_entity_S_op_region_finish_1
, E_aml_Z_stack_Z_entity_S_op_region_finish_2
, E_aml_Z_stack_Z_entity_S_power_res_finish
, E_aml_Z_stack_Z_entity_S_thermal_zone_finish
, E_aml_Z_stack_Z_entity_S_if_op_finish_1
, E_aml_Z_stack_Z_entity_S_if_op_finish_2
, E_aml_Z_stack_Z_entity_S_else_op_finish
, E_aml_Z_stack_Z_entity_S_release_finish
, E_aml_Z_stack_Z_entity_S_reset_finish
, E_aml_Z_stack_Z_entity_S_signal_finish
, E_aml_Z_stack_Z_entity_S_sleep_finish
, E_aml_Z_stack_Z_entity_S_stall_finish
, E_aml_Z_stack_Z_entity_S_notify_finish_1
, E_aml_Z_stack_Z_entity_S_notify_finish_2
, E_aml_Z_stack_Z_entity_S_return_finish
, E_aml_Z_stack_Z_entity_S_while_op_finish_1
, E_aml_Z_stack_Z_entity_S_while_op_finish_2
, E_aml_Z_stack_Z_entity_S_mutex_finish
, E_aml_Z_stack_Z_entity_S_add_op_finish_1
, E_aml_Z_stack_Z_entity_S_add_op_finish_2
, E_aml_Z_stack_Z_entity_S_add_op_finish_3
, E_aml_Z_stack_Z_entity_S_buffer_finish
, E_aml_Z_stack_Z_entity_S_and_op_finish_1
, E_aml_Z_stack_Z_entity_S_and_op_finish_2
, E_aml_Z_stack_Z_entity_S_and_op_finish_3
, E_aml_Z_stack_Z_entity_S_cond_refof_finish_1
, E_aml_Z_stack_Z_entity_S_cond_refof_finish_2
, E_aml_Z_stack_Z_entity_S_from_bcd_finish_1
, E_aml_Z_stack_Z_entity_S_from_bcd_finish_2
, E_aml_Z_stack_Z_entity_S_load_table_finish_1
, E_aml_Z_stack_Z_entity_S_load_table_finish_2
, E_aml_Z_stack_Z_entity_S_load_table_finish_3
, E_aml_Z_stack_Z_entity_S_load_table_finish_4
, E_aml_Z_stack_Z_entity_S_load_table_finish_5
, E_aml_Z_stack_Z_entity_S_load_table_finish_6
, E_aml_Z_stack_Z_entity_S_concat_finish_1
, E_aml_Z_stack_Z_entity_S_concat_finish_2
, E_aml_Z_stack_Z_entity_S_concat_finish_3
, E_aml_Z_stack_Z_entity_S_concat_res_finish_1
, E_aml_Z_stack_Z_entity_S_concat_res_finish_2
, E_aml_Z_stack_Z_entity_S_concat_res_finish_3
, E_aml_Z_stack_Z_entity_S_copy_object_finish
, E_aml_Z_stack_Z_entity_S_decrement_finish
, E_aml_Z_stack_Z_entity_S_derefof_finish
, E_aml_Z_stack_Z_entity_S_divide_finish_1
, E_aml_Z_stack_Z_entity_S_divide_finish_2
, E_aml_Z_stack_Z_entity_S_divide_finish_3
, E_aml_Z_stack_Z_entity_S_divide_finish_4
, E_aml_Z_stack_Z_entity_S_fslb_finish_1
, E_aml_Z_stack_Z_entity_S_fslb_finish_2
, E_aml_Z_stack_Z_entity_S_fsrb_finish_1
, E_aml_Z_stack_Z_entity_S_fsrb_finish_2
, E_aml_Z_stack_Z_entity_S_increment_finish
, E_aml_Z_stack_Z_entity_S_index_finish_1
, E_aml_Z_stack_Z_entity_S_index_finish_2
, E_aml_Z_stack_Z_entity_S_index_finish_3
, E_aml_Z_stack_Z_entity_S_land_finish_1
, E_aml_Z_stack_Z_entity_S_land_finish_2
, E_aml_Z_stack_Z_entity_S_lequal_finish_1
, E_aml_Z_stack_Z_entity_S_lequal_finish_2
, E_aml_Z_stack_Z_entity_S_lgreater_finish_1
, E_aml_Z_stack_Z_entity_S_lgreater_finish_2
, E_aml_Z_stack_Z_entity_S_lgreater_equal_finish_1
, E_aml_Z_stack_Z_entity_S_lgreater_equal_finish_2
, E_aml_Z_stack_Z_entity_S_lless_equal_finish_1
, E_aml_Z_stack_Z_entity_S_lless_equal_finish_2
, E_aml_Z_stack_Z_entity_S_lnot_equal_finish_1
, E_aml_Z_stack_Z_entity_S_lnot_equal_finish_2
, E_aml_Z_stack_Z_entity_S_lnot_finish
, E_aml_Z_stack_Z_entity_S_lless_finish_1
, E_aml_Z_stack_Z_entity_S_lless_finish_2
, E_aml_Z_stack_Z_entity_S_mid_finish_1
, E_aml_Z_stack_Z_entity_S_mid_finish_2
, E_aml_Z_stack_Z_entity_S_mid_finish_3
, E_aml_Z_stack_Z_entity_S_mid_finish_4
, E_aml_Z_stack_Z_entity_S_lor_finish_1
, E_aml_Z_stack_Z_entity_S_lor_finish_2
, E_aml_Z_stack_Z_entity_S_match_finish_1
, E_aml_Z_stack_Z_entity_S_match_finish_2
, E_aml_Z_stack_Z_entity_S_match_finish_3
, E_aml_Z_stack_Z_entity_S_match_finish_4
, E_aml_Z_stack_Z_entity_S_to_bcd_finish_1
, E_aml_Z_stack_Z_entity_S_to_bcd_finish_2
, E_aml_Z_stack_Z_entity_S_wait_finish_1
, E_aml_Z_stack_Z_entity_S_wait_finish_2
, E_aml_Z_stack_Z_entity_S_mod_finish_1
, E_aml_Z_stack_Z_entity_S_mod_finish_2
, E_aml_Z_stack_Z_entity_S_mod_finish_3
, E_aml_Z_stack_Z_entity_S_multiply_finish_1
, E_aml_Z_stack_Z_entity_S_multiply_finish_2
, E_aml_Z_stack_Z_entity_S_multiply_finish_3
, E_aml_Z_stack_Z_entity_S_nand_finish_1
, E_aml_Z_stack_Z_entity_S_nand_finish_2
, E_aml_Z_stack_Z_entity_S_nand_finish_3
, E_aml_Z_stack_Z_entity_S_nor_finish_1
, E_aml_Z_stack_Z_entity_S_nor_finish_2
, E_aml_Z_stack_Z_entity_S_nor_finish_3
, E_aml_Z_stack_Z_entity_S_not_finish_1
, E_aml_Z_stack_Z_entity_S_not_finish_2
, E_aml_Z_stack_Z_entity_S_object_type_finish
, E_aml_Z_stack_Z_entity_S_or_finish_1
, E_aml_Z_stack_Z_entity_S_or_finish_2
, E_aml_Z_stack_Z_entity_S_refof_finish
, E_aml_Z_stack_Z_entity_S_shift_left_finish_1
, E_aml_Z_stack_Z_entity_S_shift_left_finish_2
, E_aml_Z_stack_Z_entity_S_shift_left_finish_3
, E_aml_Z_stack_Z_entity_S_shift_right_finish_1
, E_aml_Z_stack_Z_entity_S_shift_right_finish_2
, E_aml_Z_stack_Z_entity_S_shift_right_finish_3
, E_aml_Z_stack_Z_entity_S_sizeof_finish
, E_aml_Z_stack_Z_entity_S_store_finish_1
, E_aml_Z_stack_Z_entity_S_store_finish_2
, E_aml_Z_stack_Z_entity_S_subtract_finish_1
, E_aml_Z_stack_Z_entity_S_subtract_finish_2
, E_aml_Z_stack_Z_entity_S_subtract_finish_3
, E_aml_Z_stack_Z_entity_S_to_buffer_finish_1
, E_aml_Z_stack_Z_entity_S_to_buffer_finish_2
, E_aml_Z_stack_Z_entity_S_to_decimal_string_finish_1
, E_aml_Z_stack_Z_entity_S_to_decimal_string_finish_2
, E_aml_Z_stack_Z_entity_S_to_hex_string_finish_1
, E_aml_Z_stack_Z_entity_S_to_hex_string_finish_2
, E_aml_Z_stack_Z_entity_S_to_integer_finish_1
, E_aml_Z_stack_Z_entity_S_to_integer_finish_2
, E_aml_Z_stack_Z_entity_S_to_string_finish_1
, E_aml_Z_stack_Z_entity_S_to_string_finish_2
, E_aml_Z_stack_Z_entity_S_to_string_finish_3
, E_aml_Z_stack_Z_entity_S_xor_finish_1
, E_aml_Z_stack_Z_entity_S_xor_finish_2
, E_aml_Z_stack_Z_entity_S_xor_finish_3
, E_aml_Z_stack_Z_entity_S_fatal_finish
, E_aml_Z_stack_Z_entity_S_load_finish
, E_aml_Z_stack_Z_entity_S_device_finish
, E_aml_Z_stack_Z_entity_S_processor_finish
};
_internal
struct
{ Pc data_end;
  N n;
  union E_aml_Z_stack_Z_result result;
  enum E_aml_Z_stack_Z_entity entity;
  B match;
} *E_aml_S_stack;
_internal N E_aml_S_stack_n;
_internal Pc E_aml_S_stack_data;
//==============================================================================
_internal
N
E_aml_Q_object_name_alias_M( void
){  E_aml_S_object_name_alias_n = 0;
    Mt_( E_aml_S_object_name_alias, E_aml_S_object_name_alias_n );
    if( !E_aml_S_object_name_alias )
        return ~0;
    return 0;
}
_internal
void
E_aml_Q_object_name_alias_W( void
){  for_n( i, E_aml_S_object_name_alias_n )
    {   W( E_aml_S_object_name_alias[i].alias.s );
        W( E_aml_S_object_name_alias[i].name.s );
    }
    W( E_aml_S_object_name_alias );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
N
E_aml_Q_object_name_alias_I_add( struct E_aml_Z_pathname alias
, struct E_aml_Z_pathname name
){  N middle;
    if( E_aml_S_object_name_alias_n )
    {   N min = 0;
        N max = E_aml_S_object_name_alias_n - 1;
        middle = max / 2;
        O{  N cmp = E_text_Z_sl_T_cmp( E_aml_S_object_name_alias[middle].alias.s, alias.s, J_min( E_aml_S_object_name_alias[middle].alias.n, alias.n ) * 4 );
            if( !cmp
            && E_aml_S_object_name_alias[middle].alias.n == alias.n
            )
                return ~0 - 1;
            if( cmp > 0
            || ( !cmp
              && E_aml_S_object_name_alias[middle].alias.n > alias.n
            ))
            {   if( middle == min )
                    break;
                max = middle - 1;
                middle = max - ( middle - min ) / 2;
            }else
            {   if( middle == max )
                {   middle++;
                    break;
                }
                min = middle + 1;
                middle = min + ( max - middle ) / 2;
            }
        }
    }else
        middle = 0;
    if( !E_mem_Q_blk_I_insert( &E_aml_S_object_name_alias, middle, 1 ))
        return ~0;
    E_aml_S_object_name_alias[middle].alias = alias;
    E_aml_S_object_name_alias[middle].name = name;
    return 0;
}
//==============================================================================
_internal
N
E_aml_Q_object_M( void
){  E_aml_S_object_n = 0;
    Mt_( E_aml_S_object, E_aml_S_object_n );
    if( !E_aml_S_object )
        return ~0;
    return 0;
}
_internal
void
E_aml_Q_object_W( void
){  for_n( i, E_aml_S_object_n )
        W( E_aml_S_object[i].name.s );
    W( E_aml_S_object );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
N
E_aml_S_object_I_add( struct E_aml_Z_pathname name
, N8 arg_count
){  N middle;
    if( E_aml_S_object_n )
    {   N min = 0;
        N max = E_aml_S_object_n - 1;
        middle = max / 2;
        O{  N cmp = E_text_Z_sl_T_cmp( E_aml_S_object[middle].name.s, name.s, J_min( E_aml_S_object[middle].name.n, name.n ) * 4 );
            if( !cmp
            && E_aml_S_object[middle].name.n == name.n
            )
                return ~0;
            if( cmp > 0
            || ( !cmp
              && E_aml_S_object[middle].name.n > name.n
            ))
            {   if( middle == min )
                    break;
                max = middle - 1;
                middle = max - ( middle - min ) / 2;
            }else
            {   if( middle == max )
                {   middle++;
                    break;
                }
                min = middle + 1;
                middle = min + ( max - middle ) / 2;
            }
        }
    }else
        middle = 0;
    if( !E_mem_Q_blk_I_insert( &E_aml_S_object, middle, 1 ))
        return ~0;
    E_aml_S_object_n++;
    E_aml_S_object[middle].name = name;
    E_aml_S_object[middle].arg_count = arg_count;
    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
N
E_aml_S_object_R( struct E_aml_Z_pathname name
){  if( !E_aml_S_object_n )
        return ~0;
    N min = 0;
    N max = E_aml_S_object_n - 1;
    N middle = max / 2;
    O{  N cmp = E_text_Z_sl_T_cmp( E_aml_S_object[middle].name.s, name.s, J_min( E_aml_S_object[middle].name.n, name.n ) * 4 );
        if( !cmp
        && E_aml_S_object[middle].name.n == name.n
        )
            return middle;
        if( cmp > 0
        || ( !cmp
          && E_aml_S_object[middle].name.n > name.n
        ))
        {   if( middle == min )
                break;
            max = middle - 1;
            middle = max - ( middle - min ) / 2;
        }else
        {   if( middle == max )
            {   middle++;
                break;
            }
            min = middle + 1;
            middle = min + ( max - middle ) / 2;
        }
    }
    return ~0;
}
_internal
N
E_aml_S_object_R_relative( struct E_aml_Z_pathname start_scope
, Pc name
){  if( !E_aml_S_object_n )
        return ~0;
    for_n_rev( i, start_scope.n + 1 )
    {   N min = 0;
        N max = E_aml_S_object_n - 1;
        N middle = max / 2;
        O{  N cmp = E_text_Z_sl_T_cmp( E_aml_S_object[middle].name.s, start_scope.s, J_min( E_aml_S_object[middle].name.n, i ) * 4 );
            if( !cmp
            && E_aml_S_object[middle].name.n == i + 1
            && !( cmp = E_text_Z_sl_T_cmp( E_aml_S_object[middle].name.s + i * 4, name, 4 ))
            )
                return middle;
            if( cmp > 0
            || ( !cmp
              && E_aml_S_object[middle].name.n > i + 1
            ))
            {   if( middle == min )
                    break;
                max = middle - 1;
                middle = max - ( middle - min ) / 2;
            }else
            {   if( middle == max )
                {   middle++;
                    break;
                }
                min = middle + 1;
                middle = min + ( max - middle ) / 2;
            }
        }
    }
    return ~0;
}
//==============================================================================
_internal
N
E_aml_Q_path_R_pathname( void
){  if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
        return ~0;
    N n;
    switch( (N8)*E_aml_S_stack_data++ )
    { case 0: // null
            return 0;
      case 0x2e: // dual
            n = 2;
            break;
      case 0x2f: // multi
            if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0;
            n = (N8)*E_aml_S_stack_data;
            if( !n )
                return ~0;
            E_aml_S_stack_data++;
            break;
      default:
            E_aml_S_stack_data--;
            n = 1;
            break;
    }
    if( E_aml_S_stack_data + n * 4 > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
        return ~0;
    for_n( i, n )
    {   if( !( E_aml_S_stack_data[ i * 4 ] == '_'
        || ( E_aml_S_stack_data[ i * 4 ] >= 'A'
          && E_aml_S_stack_data[ i * 4 ] <= 'Z'
        )))
            return ~0;
        for_n( j, 3 )
            if( !( E_aml_S_stack_data[ i * 4 + 1 + j ] == '_'
            || ( E_aml_S_stack_data[ i * 4 + 1 + j ] >= '0'
              && E_aml_S_stack_data[ i * 4 + 1 + j ] <= '9'
            )
            || ( E_aml_S_stack_data[ i * 4 + 1 + j ] >= 'A'
              && E_aml_S_stack_data[ i * 4 + 1 + j ] <= 'Z'
            )))
                return ~0;
    }
    Pc name_ = M( n * 4 + 1 );
    E_text_Z_s_P_copy_sl_0( name_, E_aml_S_stack_data, n * 4 );
    E_font_I_print( ",s=" ); E_font_I_print( name_ );
    W( name_ );
    E_aml_S_stack_data += n * 4;
    return n;
}
_internal
Pc
E_aml_Q_path_R_relative( N *n
, N *object_i
){  if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
        return 0;
    N parent_n = 0;
    switch( *E_aml_S_stack_data )
    { case '\\':
        {   E_aml_S_stack_data++;
            *n = E_aml_Q_path_R_pathname();
            if( !~*n
            || !*n
            )
            {   *n = ~0 - 1;
                return 0;
            }
            Pc s = M( *n * 4 );
            if( !s )
            {   *n = ~0;
                return 0;
            }
            E_mem_Q_blk_I_copy( s, E_aml_S_stack_data - *n * 4, *n * 4 );
            *object_i = E_aml_S_object_R(( struct E_aml_Z_pathname ){ E_aml_S_stack_data - *n * 4, *n });
            return s;
        }
      case '^':
        {   E_aml_S_stack_data++;
            do
            {   if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end
                || ++parent_n > E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n
                )
                {   *n = ~0 - 1;
                    return 0;
                }
            }while( *E_aml_S_stack_data++ == '^' );
            *n = E_aml_Q_path_R_pathname();
            if( !~*n
            || !*n
            )
            {   *n = ~0 - 1;
                return 0;
            }
            N depth = E_aml_S_current_path_n ? E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n - parent_n : 0;
            Pc s = M(( depth + *n ) * 4 );
            if( !s )
            {   *n = ~0;
                return 0;
            }
            if( E_aml_S_current_path_n )
                E_mem_Q_blk_I_copy( s, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].s, depth * 4 );
            E_mem_Q_blk_I_copy( s + depth * 4, E_aml_S_stack_data - *n * 4, *n * 4 );
            *n += depth;
            *object_i = E_aml_S_object_R(( struct E_aml_Z_pathname ){ s, *n });
            return s;
        }
      default:
        {   *n = E_aml_Q_path_R_pathname();
            if( !~*n
            || !*n
            )
            {   *n = ~0 - 1;
                return 0;
            }
            Pc s;
            if( E_aml_S_current_path_n )
            {   N i;
                if( *n == 1
                && ~( i = E_aml_S_object_R_relative( E_aml_S_current_path[ E_aml_S_current_path_n - 1 ], E_aml_S_stack_data - 4 ))
                )
                {   s = M( E_aml_S_object[i].name.n * 4 );
                    if( !s )
                    {   *n = ~0;
                        return 0;
                    }
                    E_mem_Q_blk_I_copy( s, E_aml_S_object[i].name.s, E_aml_S_object[i].name.n * 4 );
                    *n = E_aml_S_object[i].name.n;
                    *object_i = i;
                }else
                {   N depth = E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n - parent_n;
                    s = M(( depth + *n ) * 4 );
                    if( !s )
                        return 0;
                    E_mem_Q_blk_I_copy( s, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].s, depth * 4 );
                    E_mem_Q_blk_I_copy( s + depth * 4, E_aml_S_stack_data - *n * 4, *n * 4 );
                    *n += depth;
                    *object_i = E_aml_S_object_R(( struct E_aml_Z_pathname ){ s, *n });
                }
            }else
            {   s = M( *n * 4 );
                if( !s )
                {   *n = ~0;
                    return 0;
                }
                E_mem_Q_blk_I_copy( s, E_aml_S_stack_data - *n * 4, *n * 4 );
                *object_i = E_aml_S_object_R(( struct E_aml_Z_pathname ){ E_aml_S_stack_data - *n * 4, *n });
            }
            return s;
        }
    }
}
_internal
Pc
E_aml_Q_path_R_root( N *n
){  if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
        return 0;
    N parent_n = 0;
    switch( *E_aml_S_stack_data )
    { case '\\':
        {   E_aml_S_stack_data++;
            *n = E_aml_Q_path_R_pathname();
            if( !~*n
            || !*n
            )
            {   *n = ~0 - 1;
                return 0;
            }
            Pc s = M( *n * 4 );
            if( !s )
            {   *n = ~0;
                return 0;
            }
            E_mem_Q_blk_I_copy( s, E_aml_S_stack_data - *n * 4, *n * 4 );
            return s;
        }
      case '^':
            do
            {   if( ++E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end
                || ++parent_n > E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n
                )
                {   *n = ~0 - 1;
                    return 0;
                }
            }while( *E_aml_S_stack_data == '^' );
      default:
        {   *n = E_aml_Q_path_R_pathname();
            if( !~*n )
            {   *n = ~0 - 1;
                return 0;
            }
            N depth = E_aml_S_current_path_n ? E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n - parent_n : 0;
            Pc s = M(( depth + *n ) * 4 );
            if( !s )
            {   *n = ~0;
                return 0;
            }
            if( E_aml_S_current_path_n )
                E_mem_Q_blk_I_copy( s, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].s, depth * 4 );
            if( *n )
                E_mem_Q_blk_I_copy( s + depth * 4, E_aml_S_stack_data - *n * 4, *n * 4 );
            else if( !depth )
            {   W(s);
                return 0;
            }
            *n += depth;
            return s;
        }
    }
}
_internal
N
E_aml_R_pkg_length( void
){  if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
        return ~0;
    N n = (N8)*E_aml_S_stack_data >> 6;
    if( !n )
    {   //E_font_I_print( "\npkg_length=" ); E_font_I_print_hex( *E_aml_S_stack_data & 0x3f );
        return *E_aml_S_stack_data++ & 0x3f;
    }
    if( *E_aml_S_stack_data & 0x30 )
        return ~0;
    N l = *E_aml_S_stack_data & 0xf;
    N i = 0;
    while( ++E_aml_S_stack_data != E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end
    && i != n
    )
    {   l |= (N)(N8)*E_aml_S_stack_data << ( 4 + i * 8 );
        i++;
    }
    //E_font_I_print( "\npkg_length=" ); E_font_I_print_hex(l);
    return l;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define E_aml_I_delegate( finish, entity_ ) \
{  if( !E_mem_Q_blk_I_append( &E_aml_S_stack, 2 )) \
        return ~0; \
    Pc data_end = E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end; \
    E_aml_S_stack[ E_aml_S_stack_n ].entity = finish; \
    E_aml_S_stack[ E_aml_S_stack_n ].n = 0; \
    E_aml_S_stack[ E_aml_S_stack_n ].data_end = 0; \
    E_aml_S_stack_n++; \
    E_aml_S_stack[ E_aml_S_stack_n ].entity = entity_; \
    E_aml_S_stack[ E_aml_S_stack_n ].n = 1; \
    E_aml_S_stack[ E_aml_S_stack_n ].data_end = data_end; \
    E_aml_S_stack_n++; \
}
#define E_aml_I_delegate_pkg(  finish, entity_ ) \
{   if( !E_mem_Q_blk_I_append( &E_aml_S_stack, 2 )) \
        return ~0; \
    E_aml_S_stack[ E_aml_S_stack_n ].entity = finish; \
    E_aml_S_stack[ E_aml_S_stack_n ].n = 0; \
    E_aml_S_stack[ E_aml_S_stack_n ].data_end = 0; \
    E_aml_S_stack_n++; \
    E_aml_S_stack[ E_aml_S_stack_n ].entity = entity_; \
    E_aml_S_stack[ E_aml_S_stack_n ].n = 1; \
    E_aml_S_stack[ E_aml_S_stack_n ].data_end = pkg_end; \
    E_aml_S_stack_n++; \
}
_internal
N
E_aml_I_data_object( void
){  if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
        return ~0 - 1;
    switch( (N8)*E_aml_S_stack_data++ )
    { case 0:
            E_font_I_print( ",0" );
            //TODO Zapisać 0.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            break;
      case 1:
            E_font_I_print( ",1" );
            //TODO Zapisać 1.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            break;
      case 0xff:
            E_font_I_print( ",~0" );
            //TODO Zapisać ~0.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            break;
      case 0xa: // byte const
            if( E_aml_S_stack_data + 1 > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",byte" );
            E_aml_S_stack_data++;
            //TODO Zapisać ‘byte’.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            break;
      case 0xb: // word const
            if( E_aml_S_stack_data + 2 > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",word" );
            E_aml_S_stack_data += 2;
            //TODO Zapisać ‘word’.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            break;
      case 0xc: // dword const
            if( E_aml_S_stack_data + 4 > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",dword" );
            E_aml_S_stack_data += 4;
            //TODO Zapisać ‘dword’.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            break;
      case 0xd: // string const
            E_font_I_print( ",string=" );
            Pc s = E_aml_S_stack_data;
            while( E_aml_S_stack_data != E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end
            && *E_aml_S_stack_data
            )
                E_aml_S_stack_data++;
            if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_aml_S_stack_data++;
            E_font_I_print(s);
            //TODO Zapisać ‘string’.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            break;
      case 0xe: // qword const
            if( E_aml_S_stack_data + 8 > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",qword" );
            E_aml_S_stack_data += 8;
            //TODO Zapisać ‘qword’.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            break;
      case 0x5b:
            if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            switch( (N8)*E_aml_S_stack_data++ )
            { case 0x30: // revision
                    if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    E_font_I_print( ",revision" );
                    //TODO Zapisać ‘revision’ (0x30).
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    break;
              default:
                    E_aml_S_stack_data -= 2;
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = no;
                    return 0;
            }
            break;
      case 0x11: // buffer
        {   Pc data_start = E_aml_S_stack_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
            if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",buffer" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_data_object_buffer_finish, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
        }
      case 0x12: // package
        {   Pc data_start = E_aml_S_stack_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
            if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            if( E_aml_S_stack_data == pkg_end )
                return ~0 - 1;
            N n = (N8)*E_aml_S_stack_data;
            E_aml_S_stack_data++;
            E_font_I_print( ",package," ); E_font_I_print_hex(n);
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_package_finish, E_aml_Z_stack_Z_entity_S_package );
            E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = n;
            break;
        }
      case 0x13: // var package
        {   Pc data_start = E_aml_S_stack_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
            if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",var package" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            if( !E_mem_Q_blk_I_append( &E_aml_S_stack, 4 ))
                return ~0;
            E_aml_S_stack[ E_aml_S_stack_n ].entity = E_aml_Z_stack_Z_entity_S_package_finish;
            E_aml_S_stack[ E_aml_S_stack_n ].n = 0;
            E_aml_S_stack_n++;
            E_aml_S_stack[ E_aml_S_stack_n ].entity = E_aml_Z_stack_Z_entity_S_package;
            E_aml_S_stack[ E_aml_S_stack_n ].n = 0;
            E_aml_S_stack_n++;
            E_aml_S_stack[ E_aml_S_stack_n ].entity = E_aml_Z_stack_Z_entity_S_result_to_n;
            E_aml_S_stack[ E_aml_S_stack_n ].n = 0;
            E_aml_S_stack_n++;
            E_aml_S_stack[ E_aml_S_stack_n ].entity = E_aml_Z_stack_Z_entity_S_term_arg;
            E_aml_S_stack[ E_aml_S_stack_n ].n = 1;
            E_aml_S_stack[ E_aml_S_stack_n ].data_end = pkg_end;
            E_aml_S_stack_n++;
            break;
        }
      default:
            E_aml_S_stack_data--;
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = no;
            return 0;
    }
    return 0;
}
_internal
N
E_aml_I_term( void
){  if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
        return ~0 - 1;
    switch( (N8)*E_aml_S_stack_data++ )
    { case 0x5b: // ext
            if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            switch( (N8)*E_aml_S_stack_data++ )
            { case 0x32: // fatal
                    E_font_I_print( ",fatal" );
                    if( ++E_aml_S_stack_data > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    if( E_aml_S_stack_data + 4 == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    E_aml_S_stack_data += 4;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_fatal_finish, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
              case 0x27: // release
                    E_font_I_print( ",release" );
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_release_finish, E_aml_Z_stack_Z_entity_S_supername );
                    break;
              case 0x26: // reset
                    E_font_I_print( ",reset" );
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_reset_finish, E_aml_Z_stack_Z_entity_S_supername );
                    break;
              case 0x24: // signal
                    E_font_I_print( ",signal" );
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_signal_finish, E_aml_Z_stack_Z_entity_S_supername );
                    break;
              case 0x22: // sleep
                    E_font_I_print( ",sleep" );
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_sleep_finish, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
              case 0x21: // stall
                    E_font_I_print( ",stall" );
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_stall_finish, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
              case 0x20: // load
                    E_font_I_print( ",load" );
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_load_finish, E_aml_Z_stack_Z_entity_S_supername );
                    break;
              case 0x2a: // unload
                    E_font_I_print( ",unload" );
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_load_finish, E_aml_Z_stack_Z_entity_S_supername );
                    break;
              default:
                {   E_aml_S_stack_data -= 2;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_term_finish_1, E_aml_Z_stack_Z_entity_S_object );
                    break;
                }
            }
            break;
      case 0x15: // external
        {   N n;
            Pc name = E_aml_Q_path_R_root( &n );
            if( !name )
                return n;
            W(name);
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( ",external=" ); E_font_I_print( name_ );
            W( name_ );
            if( E_aml_S_stack_data + 2 > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            //TODO Dodać ‘external’ do drzewa zinterpretowanej przestrzeni ACPI.
            break;
        }
      case 0xa5: // break
            E_font_I_print( ",break" );
            break;
      case 0xcc: // breakpoint
            E_font_I_print( ",breakpoint" );
            break;
      case 0x9f: // continue
            E_font_I_print( ",continue" );
            break;
      case 0xa0: // if
        {   Pc data_start = E_aml_S_stack_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
            if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",if" );
            if( E_aml_S_stack_data != pkg_end )
                E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_if_op_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
        }
      case 0xa1: // else
        {   Pc data_start = E_aml_S_stack_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
            if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",else" );
            if( E_aml_S_stack_data != pkg_end )
            {   E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_else_op_finish, E_aml_Z_stack_Z_entity_S_term );
                E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = ~0;
            }
        }
      case 0xa3: // noop
            break;
      case 0x86: // notify
            E_font_I_print( ",notify" );
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_notify_finish_1, E_aml_Z_stack_Z_entity_S_supername );
            break;
      case 0xa4: // return
            E_font_I_print( ",return" );
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_return_finish, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0xa2: // while
        {   Pc data_start = E_aml_S_stack_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
            if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",while" );
            if( E_aml_S_stack_data != pkg_end )
                E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_while_op_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
        }
      default:
            E_aml_S_stack_data--;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_term_finish_1, E_aml_Z_stack_Z_entity_S_object );
            break;
    }
    return 0;
}
_internal
N
E_aml_I_object( void
){  if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
        return ~0 - 1;
    switch( (N8)*E_aml_S_stack_data++ )
    { case 6: // alias
        {   N name_n;
            Pc name = E_aml_Q_path_R_root( &name_n );
            if( !name )
                return name_n;
            //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje.
            Pc name_ = M( name_n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, name_n * 4 );
            E_font_I_print( ",alias=" ); E_font_I_print( name_ );
            W( name_ );
            N alias_n;
            Pc alias = E_aml_Q_path_R_root( &alias_n );
            if( !alias )
            {   W(name);
                return alias_n;
            }
            name_ = M( alias_n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, alias, alias_n * 4 );
            E_font_I_print( ":" ); E_font_I_print( name_ );
            W( name_ );
            N ret = E_aml_Q_object_name_alias_I_add(( struct E_aml_Z_pathname ){ alias, alias_n }, ( struct E_aml_Z_pathname ){ name, name_n });
            if( (S)ret < 0 )
            {   W(alias);
                W(name);
                return ret;
            }
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            break;
        }
      case 8: // name
        {   N n;
            Pc name = E_aml_Q_path_R_root( &n );
            if( !name )
                return n;
            //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje (oprócz ostatniego obiektu, który teraz jest definiowany).
            //TODO Dodać ‘name’ do drzewa zinterpretowanej przestrzeni ACPI.
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( "\n, object name=" ); E_font_I_print( name_ );
            W( name_ );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            if( !E_mem_Q_blk_I_append( &E_aml_S_current_path, 1 ))
                return ~0;
            E_aml_S_current_path[ E_aml_S_current_path_n ].s = name;
            E_aml_S_current_path[ E_aml_S_current_path_n ].n = n;
            E_aml_S_current_path_n++;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_restore_current_path, E_aml_Z_stack_Z_entity_S_data_object );
            break;
        }
      case 0x10: // scope
        {   Pc data_start = E_aml_S_stack_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
            if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            N n;
            Pc name = E_aml_Q_path_R_root( &n );
            if( !name )
                return n;
            if( E_aml_S_stack_data == pkg_end )
                return ~0 - 1;
            //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje (oprócz ostatniego obiektu, który teraz jest definiowany).
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( "\n,scope name=" ); E_font_I_print( name_ );
            W( name_ );
            //TODO Dodać ‘scope’ do drzewa zinterpretowanej przestrzeni ACPI.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            if( E_aml_S_stack_data != pkg_end )
            {   if( !E_mem_Q_blk_I_append( &E_aml_S_current_path, 1 ))
                    return ~0;
                E_aml_S_current_path[ E_aml_S_current_path_n ].s = name;
                E_aml_S_current_path[ E_aml_S_current_path_n ].n = n;
                E_aml_S_current_path_n++;
                E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_restore_current_path, E_aml_Z_stack_Z_entity_S_term );
                E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = ~0;
            }
            break;
        }
      case 0x5b: // ext
            if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            switch( (N8)*E_aml_S_stack_data++ )
            { case 0x87: // bank field
                {   Pc data_start = E_aml_S_stack_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
                    if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N n;
                    Pc region_name = E_aml_Q_path_R_root( &n );
                    if( !region_name )
                        return n;
                    if( E_aml_S_stack_data == pkg_end )
                        return ~0 - 1;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, region_name, n * 4 );
                    E_font_I_print( ",region name=" ); E_font_I_print( name_ );
                    W( name_ );
                    //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje.
                    W( region_name );
                    Pc bank_name = E_aml_Q_path_R_root( &n );
                    if( !bank_name )
                        return n;
                    if( E_aml_S_stack_data == pkg_end )
                        return ~0 - 1;
                    name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, bank_name, n * 4 );
                    E_font_I_print( ",bank name=" ); E_font_I_print( name_ );
                    W( name_ );
                    //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje.
                    W( bank_name );
                    //TODO Dodać ‘bank field’ do drzewa zinterpretowanej przestrzeni ACPI.
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_bank_field_finish, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
                }
              case 1: // mutex
                {   N n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",mutex=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    E_aml_S_stack_data++;
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    break;
                }
              case 0x13: // field
                    //TODO Dodać ‘field’ do drzewa zinterpretowanej przestrzeni ACPI.
                    E_font_I_print( ",field" );
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_field_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
              case 0x88: // data region
                {   N n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",data region=" ); E_font_I_print( name_ );
                    W( name_ );
                    //TODO Dodać ‘data region’ do drzewa zinterpretowanej przestrzeni ACPI.
                    W(name);
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_data_region_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
                }
             case 0x80: // op region
                {   N n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",op region=" ); E_font_I_print( name_ );
                    W( name_ );
                    W(name);
                    if( ++E_aml_S_stack_data > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    //TODO Dodać ‘op region’ do drzewa zinterpretowanej przestrzeni ACPI.
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_op_region_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
                }
              case 0x81: // field op
                {   Pc data_start = E_aml_S_stack_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
                    if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return n;
                    if( ++E_aml_S_stack_data > pkg_end )
                        return ~0 - 1;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",field op=" ); E_font_I_print( name_ );
                    W( name_ );
                    W(name);
                    while( E_aml_S_stack_data != pkg_end )
                    {   switch( *E_aml_S_stack_data++ )
                        { case 0: // reserved field
                            {   if( E_aml_S_stack_data == pkg_end )
                                    return ~0 - 1;
                                E_font_I_print( ",reserved field" );
                                N n = E_aml_R_pkg_length();
                                if( !~n )
                                    return ~0 - 1;
                                //TODO Dodać ‘field pkg length’ do drzewa zinterpretowanej przestrzeni ACPI.
                                break;
                            }
                          case 1: // access field
                                E_font_I_print( ",access field" );
                                if( E_aml_S_stack_data + 2 > pkg_end )
                                    return ~0 - 1;
                                //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_stack_data++;
                                //TODO Dodać ‘access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_stack_data++;
                                break;
                          case 2: // connect field
                                if( E_aml_S_stack_data == pkg_end )
                                    return ~0 - 1;
                                E_font_I_print( ",connect field" );
                                return ~0; //NDFN
                                break;
                          case 3: // extended access field
                                E_font_I_print( ",ext access field" );
                                if( E_aml_S_stack_data + 3 > pkg_end )
                                    return ~0 - 1;
                                //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_stack_data++;
                                //TODO Dodać ‘extended access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_stack_data++;
                                //TODO Dodać ‘access length’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_stack_data++; //NDFN Jaki rozmiar.
                                break;
                          default: // named field
                            {   E_aml_S_stack_data--;
                                if( E_aml_S_stack_data + 4 > pkg_end )
                                    return ~0 - 1;
                                if( !( E_aml_S_stack_data[0] == '_'
                                || ( E_aml_S_stack_data[0] >= 'A'
                                  && E_aml_S_stack_data[0] <= 'Z'
                                )))
                                    return ~0 - 1;
                                for_n( j, 3 )
                                    if( !( E_aml_S_stack_data[ 1 + j ] == '_'
                                    || ( E_aml_S_stack_data[ 1 + j ] >= '0'
                                      && E_aml_S_stack_data[ 1 + j ] <= '9'
                                    )
                                    || ( E_aml_S_stack_data[ 1 + j ] >= 'A'
                                      && E_aml_S_stack_data[ 1 + j ] <= 'Z'
                                    )))
                                        return ~0 - 1;
                                Pc name_ = M( 4 + 1 );
                                E_text_Z_s_P_copy_sl_0( name_, E_aml_S_stack_data, 4 );
                                E_font_I_print( ",field=" ); E_font_I_print( name_ );
                                W( name_ );
                                //TODO Dodać ‘field name’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_stack_data += 4;
                                N n = E_aml_R_pkg_length();
                                if( !~n )
                                    return n;
                                E_font_I_print( ",field" );
                                //TODO Dodać ‘field pkg length’ do drzewa zinterpretowanej przestrzeni ACPI.
                                break;
                            }
                        }
                    }
                    //TODO Dodać ‘field op’ do drzewa zinterpretowanej przestrzeni ACPI.
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    break;
                }
              case 0x86: // index field
                {   Pc data_start = E_aml_S_stack_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
                    if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",field op=" ); E_font_I_print( name_ );
                    W( name_ );
                    W(name);
                    name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return n;
                    name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",=" ); E_font_I_print( name_ );
                    W( name_ );
                    W(name);
                    if( ++E_aml_S_stack_data > pkg_end )
                        return ~0 - 1;
                    while( E_aml_S_stack_data != pkg_end )
                    {   switch( *E_aml_S_stack_data++ )
                        { case 0: // reserved field
                            {   if( E_aml_S_stack_data == pkg_end )
                                    return ~0 - 1;
                                E_font_I_print( ",reserved field" );
                                N n = E_aml_R_pkg_length();
                                if( !~n )
                                    return ~0 - 1;
                                //TODO Dodać ‘field pkg length’ do drzewa zinterpretowanej przestrzeni ACPI.
                                break;
                            }
                          case 1: // access field
                                E_font_I_print( ",access field" );
                                if( E_aml_S_stack_data + 2 > pkg_end )
                                    return ~0 - 1;
                                //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_stack_data++;
                                //TODO Dodać ‘access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_stack_data++;
                                break;
                          case 2: // connect field
                                if( E_aml_S_stack_data == pkg_end )
                                    return ~0 - 1;
                                E_font_I_print( ",connect field" );
                                return ~0; //NDFN
                                break;
                          case 3: // extended access field
                                E_font_I_print( ",ext access field" );
                                if( E_aml_S_stack_data + 3 > pkg_end )
                                    return ~0 - 1;
                                //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_stack_data++;
                                //TODO Dodać ‘extended access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_stack_data++;
                                //TODO Dodać ‘access length’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_stack_data++; //NDFN Jaki rozmiar.
                                break;
                          default: // named field
                            {   E_aml_S_stack_data--;
                                if( E_aml_S_stack_data + 4 > pkg_end )
                                    return ~0 - 1;
                                if( !( E_aml_S_stack_data[0] == '_'
                                || ( E_aml_S_stack_data[0] >= 'A'
                                  && E_aml_S_stack_data[0] <= 'Z'
                                )))
                                    return ~0 - 1;
                                for_n( j, 3 )
                                    if( !( E_aml_S_stack_data[ 1 + j ] == '_'
                                    || ( E_aml_S_stack_data[ 1 + j ] >= '0'
                                      && E_aml_S_stack_data[ 1 + j ] <= '9'
                                    )
                                    || ( E_aml_S_stack_data[ 1 + j ] >= 'A'
                                      && E_aml_S_stack_data[ 1 + j ] <= 'Z'
                                    )))
                                        return ~0 - 1;
                                Pc name_ = M( 4 + 1 );
                                E_text_Z_s_P_copy_sl_0( name_, E_aml_S_stack_data, 4 );
                                E_font_I_print( ",field=" ); E_font_I_print( name_ );
                                W( name_ );
                                //TODO Dodać ‘field name’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_stack_data += 4;
                                N n = E_aml_R_pkg_length();
                                if( !~n )
                                    return ~0 - 1;
                                E_font_I_print( ",field" );
                                //TODO Dodać ‘field pkg length’ do drzewa zinterpretowanej przestrzeni ACPI.
                                break;
                            }
                        }
                    }
                    //TODO Dodać ‘field op’ do drzewa zinterpretowanej przestrzeni ACPI.
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    break;
                }
              case 0x82: // device
                {   Pc data_start = E_aml_S_stack_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
                    if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",device=" ); E_font_I_print( name_ );
                    W( name_ );
                    //TODO Dodać ‘device’ do drzewa zinterpretowanej przestrzeni ACPI.
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    if( E_aml_S_stack_data != pkg_end )
                    {   if( !E_mem_Q_blk_I_append( &E_aml_S_current_path, 1 ))
                            return ~0;
                        E_aml_S_current_path[ E_aml_S_current_path_n ].s = name;
                        E_aml_S_current_path[ E_aml_S_current_path_n ].n = n;
                        E_aml_S_current_path_n++;
                        E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_device_finish, E_aml_Z_stack_Z_entity_S_object );
                        E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = ~0;
                    }
                    break;
                }
              case 2: // event
                {   N n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",event=" ); E_font_I_print( name_ );
                    W( name_ );
                    //TODO Dodać ‘event’ do drzewa zinterpretowanej przestrzeni ACPI.
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    break;
                }
              case 0x84: // power res
                {   Pc data_start = E_aml_S_stack_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
                    if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",power res name=" ); E_font_I_print( name_ );
                    W( name_ );
                    //TODO Dodać ‘power res’ do drzewa zinterpretowanej przestrzeni ACPI.
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    if( E_aml_S_stack_data != pkg_end )
                    {   if( ++E_aml_S_stack_data > pkg_end )
                            return ~0 - 1;
                        E_aml_S_stack_data += 2;
                        if( E_aml_S_stack_data > pkg_end )
                            return ~0 - 1;
                        if( !E_mem_Q_blk_I_append( &E_aml_S_current_path, 1 ))
                            return ~0;
                        E_aml_S_current_path[ E_aml_S_current_path_n ].s = name;
                        E_aml_S_current_path[ E_aml_S_current_path_n ].n = n;
                        E_aml_S_current_path_n++;
                        E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_power_res_finish, E_aml_Z_stack_Z_entity_S_object );
                        E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = ~0;
                    }
                    break;
                }
              case 0x85: // thermal zone
                {   Pc data_start = E_aml_S_stack_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
                    if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",thermal zone name=" ); E_font_I_print( name_ );
                    W( name_ );
                    //TODO Dodać ‘thermal zone’ do drzewa zinterpretowanej przestrzeni ACPI.
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    if( E_aml_S_stack_data != pkg_end )
                    {   if( !E_mem_Q_blk_I_append( &E_aml_S_current_path, 1 ))
                            return ~0;
                        E_aml_S_current_path[ E_aml_S_current_path_n ].s = name;
                        E_aml_S_current_path[ E_aml_S_current_path_n ].n = n;
                        E_aml_S_current_path_n++;
                        E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_thermal_zone_finish, E_aml_Z_stack_Z_entity_S_object );
                        E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = ~0;
                    }
                    break;
                }
              case 0x83: // processor
                {   Pc data_start = E_aml_S_stack_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
                    if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",processor=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( ++E_aml_S_stack_data > pkg_end )
                        return ~0 - 1;
                    if( E_aml_S_stack_data + 4 > pkg_end )
                        return ~0 - 1;
                    E_aml_S_stack_data += 4;
                    if( ++E_aml_S_stack_data > pkg_end )
                        return ~0 - 1;
                    //TODO Dodać ‘processor’ do drzewa zinterpretowanej przestrzeni ACPI.
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    if( E_aml_S_stack_data != pkg_end )
                    {   if( !E_mem_Q_blk_I_append( &E_aml_S_current_path, 1 ))
                            return ~0;
                        E_aml_S_current_path[ E_aml_S_current_path_n ].s = name;
                        E_aml_S_current_path[ E_aml_S_current_path_n ].n = n;
                        E_aml_S_current_path_n++;
                        E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_processor_finish, E_aml_Z_stack_Z_entity_S_object );
                        E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = ~0;
                    }
                    break;
                }
              default:
                {   E_aml_S_stack_data -= 2;
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = no;
                    if( !E_mem_Q_blk_I_append( &E_aml_S_stack, 1 ))
                        return ~0;
                    Pc data_end = E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end;
                    E_aml_S_stack[ E_aml_S_stack_n ].entity = E_aml_Z_stack_Z_entity_S_expression;
                    E_aml_S_stack[ E_aml_S_stack_n ].n = 1;
                    E_aml_S_stack[ E_aml_S_stack_n ].data_end = data_end;
                    E_aml_S_stack_n++;
                    break;
                }
            }
            break;
      case 0x8d: // bit field
            E_font_I_print( ",bit field" );
            //TODO Dodać ‘bit field’ do drzewa zinterpretowanej przestrzeni ACPI.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_bit_field_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x8c: // byte field
            E_font_I_print( ",byte field" );
            //TODO Dodać ‘byte field’ do drzewa zinterpretowanej przestrzeni ACPI.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_byte_field_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x8a: // dword field
            E_font_I_print( ",dword field" );
            //TODO Dodać ‘dword field’ do drzewa zinterpretowanej przestrzeni ACPI.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_dword_field_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x8f: // qword field
            E_font_I_print( ",qword field" );
            //TODO Dodać ‘qword field’ do drzewa zinterpretowanej przestrzeni ACPI.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_qword_field_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x8b: // word field
            E_font_I_print( ",word field" );
            //TODO Dodać ‘word field’ do drzewa zinterpretowanej przestrzeni ACPI.
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_word_field_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x14: // method
        {   Pc data_start = E_aml_S_stack_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
            if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            N n;
            Pc name = E_aml_Q_path_R_root( &n );
            if( !name )
                return n;
            if( E_aml_S_stack_data == pkg_end )
                return ~0 - 1;
            //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje (oprócz ostatniego obiektu, który teraz jest definiowany).
            N8 arg_count = *E_aml_S_stack_data & 3;
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( "\n,method=" ); E_font_I_print( name_ ); E_font_I_print( "," ); E_font_I_print_hex( arg_count );
            W( name_ );
            if( ++E_aml_S_stack_data > pkg_end )
            {   W(name);
                return ~0 - 1;
            }
            N ret = E_aml_S_object_I_add(( struct E_aml_Z_pathname ){ name, n }, arg_count );
            if( (S)ret < 0 )
            {   W(name);
                return ret;
            }
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            if( E_aml_S_stack_data != pkg_end )
            {   if( !E_mem_Q_blk_I_append( &E_aml_S_current_path, 1 ))
                    return ~0;
                name_ = E_text_Z_s_M_duplicate( name, n * 4 );
                if( !name_ )
                    return ~0;
                E_aml_S_current_path[ E_aml_S_current_path_n ].s = name_;
                E_aml_S_current_path[ E_aml_S_current_path_n ].n = n;
                E_aml_S_current_path_n++;
                E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_restore_current_path, E_aml_Z_stack_Z_entity_S_term );
                E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = ~0;
            }
            return 0;
        }
      default:
            E_aml_S_stack_data--;
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = no;
            return 0;
    }
    return 0;
}
_internal
N
E_aml_I_expression( void
){  if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
        return ~0 - 1;
    switch( (N8)*E_aml_S_stack_data++ )
    { case 0x5b: // ext
            if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            switch( (N8)*E_aml_S_stack_data++ )
            { case 0x23: // acquire
                    E_font_I_print( ",acquire" );
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_mutex_finish, E_aml_Z_stack_Z_entity_S_supername );
                    break;
              case 0x12: // cond refof
                    E_font_I_print( ",cond refof" );
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_cond_refof_finish_1, E_aml_Z_stack_Z_entity_S_supername );
                    break;
              case 0x28: // from bcd
                    E_font_I_print( ",from bcd" );
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_from_bcd_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
              case 0x1f: // load table
                    E_font_I_print( ",load table" );
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_load_table_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
              case 0x33: // timer
                    E_font_I_print( ",timer" );
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    break;
              case 0x29: // to bcd
                    E_font_I_print( ",to bcd" );
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_to_bcd_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
              case 0x25: // wait
                    E_font_I_print( ",wait" );
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_wait_finish_1, E_aml_Z_stack_Z_entity_S_supername );
                    break;
              default:
                    E_aml_S_stack_data -= 2;
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = no;
                    return 0;
            }
            break;
      case 0x72: // add
            E_font_I_print( ",add" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_add_op_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x7b: // and
            E_font_I_print( ",and" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_and_op_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x11: // buffer
        {   Pc data_start = E_aml_S_stack_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
            if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",buffer" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_buffer_finish, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
        }
      case 0x73: // concat
            E_font_I_print( ",concat" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_concat_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x84: // concat res
            E_font_I_print( ",concat res" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_concat_res_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x9d: // copy object
            E_font_I_print( ",copy object" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_copy_object_finish, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x76: // decrement
            E_font_I_print( ",decrement" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_decrement_finish, E_aml_Z_stack_Z_entity_S_supername );
            break;
      case 0x83: // derefof
            E_font_I_print( ",derefof" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_derefof_finish, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x78: // divide
            E_font_I_print( ",divide" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_divide_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x81: // find set left bit
            E_font_I_print( ",fslb" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_fslb_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x82: // find set right bit
            E_font_I_print( ",fsrb" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_fsrb_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x75: // increment
            E_font_I_print( ",increment" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_increment_finish, E_aml_Z_stack_Z_entity_S_supername );
            break;
      case 0x88: // index
            E_font_I_print( ",index" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_index_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x90: // land
            E_font_I_print( ",land" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_land_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x93: // lequal
            E_font_I_print( ",lequal" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lequal_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x94: // lgreater
            E_font_I_print( ",lgreater" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lgreater_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x92: // lnot
            if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            switch( (N8)*E_aml_S_stack_data++ )
            { case 0x95: // lgreater equal
                    E_font_I_print( ",lgreater equal" );
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lgreater_equal_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
              case 0x94: // lless equal
                    E_font_I_print( ",lless equal" );
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lless_equal_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
              case 0x93: // lnot equal
                    E_font_I_print( ",lnot equal" );
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lnot_equal_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
              default: // lnot
                    E_font_I_print( ",lnot" );
                    E_aml_S_stack_data--;
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lnot_finish, E_aml_Z_stack_Z_entity_S_term_arg );
                    break;
            }
            break;
      case 0x95: // lless
            E_font_I_print( ",lless" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lless_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x9e: // mid
            E_font_I_print( ",mid" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_mid_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x91: // lor
            E_font_I_print( ",lor" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lor_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x89: // match
            E_font_I_print( ",match" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_match_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x85: // mod
            E_font_I_print( ",mod" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_mod_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x77: // multiply
            E_font_I_print( ",multiply" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_multiply_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x7c: // nand
            E_font_I_print( ",nand" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_nand_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x7e: // nor
            E_font_I_print( ",nor" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_nor_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x80: // not
            E_font_I_print( ",not" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_not_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x8e: // object type
            E_font_I_print( ",object type" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_object_type_finish, E_aml_Z_stack_Z_entity_S_supername );
            break;
      case 0x7d: // or
            E_font_I_print( ",or" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_or_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x12: // package
        {   Pc data_start = E_aml_S_stack_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
            if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            if( E_aml_S_stack_data == pkg_end )
                return ~0 - 1;
            N n = (N8)*E_aml_S_stack_data;
            E_aml_S_stack_data++;
            E_font_I_print( ",package," ); E_font_I_print_hex(n);
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate_pkg( E_aml_Z_stack_Z_entity_S_package_finish, E_aml_Z_stack_Z_entity_S_package );
            E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = n;
            break;
        }
      case 0x13: // var package
        {   Pc data_start = E_aml_S_stack_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_stack_data + pkg_length - ( E_aml_S_stack_data - data_start );
            if( pkg_end > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            if( !E_mem_Q_blk_I_append( &E_aml_S_stack, 4 ))
                return ~0;
            E_font_I_print( ",var package" );
            E_aml_S_stack[ E_aml_S_stack_n ].entity = E_aml_Z_stack_Z_entity_S_package_finish;
            E_aml_S_stack[ E_aml_S_stack_n ].n = 0;
            E_aml_S_stack_n++;
            E_aml_S_stack[ E_aml_S_stack_n ].entity = E_aml_Z_stack_Z_entity_S_package;
            E_aml_S_stack[ E_aml_S_stack_n ].n = 0;
            E_aml_S_stack_n++;
            E_aml_S_stack[ E_aml_S_stack_n ].entity = E_aml_Z_stack_Z_entity_S_result_to_n;
            E_aml_S_stack[ E_aml_S_stack_n ].n = 0;
            E_aml_S_stack_n++;
            E_aml_S_stack[ E_aml_S_stack_n ].entity = E_aml_Z_stack_Z_entity_S_term_arg;
            E_aml_S_stack[ E_aml_S_stack_n ].n = 1;
            E_aml_S_stack[ E_aml_S_stack_n ].data_end = pkg_end;
            E_aml_S_stack_n++;
            break;
        }
      case 0x71: // refof
            E_font_I_print( ",refof" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_refof_finish, E_aml_Z_stack_Z_entity_S_supername );
            break;
      case 0x79: // shift left
            E_font_I_print( ",shift left" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_shift_left_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x7a: // shift right
            E_font_I_print( ",shift right" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_shift_right_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x87: // sizeof
            E_font_I_print( ",sizeof" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_sizeof_finish, E_aml_Z_stack_Z_entity_S_supername );
            break;
      case 0x70: // store
            E_font_I_print( ",store" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_store_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x74: // subtract
            E_font_I_print( ",subtract" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_subtract_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x96: // to buffer
            E_font_I_print( ",to buffer" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_to_buffer_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x97: // to decimal string
            E_font_I_print( ",to decimal string" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_to_decimal_string_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x98: // to hex string
            E_font_I_print( ",to hex string" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_to_hex_string_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x99: // to integer
            E_font_I_print( ",to integer" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_to_integer_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x9c: // to string
            E_font_I_print( ",to string" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_to_string_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x7f: // xor
            E_font_I_print( ",xor" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_xor_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      default:
        {   E_aml_S_stack_data--;
            N n;
            N object_i;
            Pc name = E_aml_Q_path_R_relative( &n, &object_i );
            if( !name )
                return n;
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( ",expr string=" ); E_font_I_print( name_ );
            W( name_ );
            //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje.
            // method invocation
            if( !~object_i )
            {   name_ = name + ( n - 1 ) * 4;
                if( !( E_text_Z_sl_T_eq( name_, "_OS_", 4 )
                || E_text_Z_sl_T_eq( name_, "_OSI", 4 )
                || E_text_Z_sl_T_eq( name_, "_REV", 4 )
                ))
                {   W(name);
                    E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = no;
                    break;
                }
            }
            E_font_I_print( ",invocation" );
            E_aml_S_stack[ E_aml_S_stack_n - 2 ].match = yes;
            if(( ~object_i
              && E_aml_S_object[ object_i ].arg_count
            )
            || ( !~object_i
              && E_text_Z_sl_T_eq( name_, "_OSI", 4 )
            ))
            {   if( ~object_i )
                    n = E_aml_S_object[ object_i ].arg_count;
                else if( E_text_Z_sl_T_eq( name_, "_OSI", 4 ))
                    n = 1;
                E_font_I_print( "," ); E_font_I_print_hex(n);
                if( !E_mem_Q_blk_I_append( &E_aml_S_stack, 1 ))
                {   W(name);
                    return ~0;
                }
                Pc data_end = E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end;
                E_aml_S_stack[ E_aml_S_stack_n ].entity = E_aml_Z_stack_Z_entity_S_term_arg;
                E_aml_S_stack[ E_aml_S_stack_n ].n = n;
                E_aml_S_stack[ E_aml_S_stack_n ].data_end = data_end;
                E_aml_S_stack_n++;
            }
            W(name);
            break;
        }
    }
    return 0;
}
_internal
N
E_aml_I_supername( void
){  if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
        return ~0 - 1;
    switch( (N8)*E_aml_S_stack_data++ )
    { case 0x5b:
            if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                return ~0 - 1;
            switch( (N8)*E_aml_S_stack_data++ )
            { case 0x31: // debug
                    E_font_I_print( ",debug" );
                    break;
              default:
                    E_aml_S_stack_data -= 2;
                    return ~0 - 1;
            }
            break;
      case 0x71: // ref
            E_font_I_print( ",ref" );
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_supername_ref_finish, E_aml_Z_stack_Z_entity_S_supername );
            break;
      case 0x83: // deref
            E_font_I_print( ",deref" );
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_supername_deref_finish, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x88: // index
            E_font_I_print( ",index" );
            E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_supername_index_finish_1, E_aml_Z_stack_Z_entity_S_term_arg );
            break;
      case 0x68: // arg
      case 0x69:
      case 0x6a:
      case 0x6b:
      case 0x6c:
      case 0x6d:
      case 0x6e:
            E_font_I_print( ",arg" );
            //E_aml_S_stack[ E_aml_S_stack_n - 1 ].result =
            break;
      case 0x60: // local
      case 0x61:
      case 0x62:
      case 0x63:
      case 0x64:
      case 0x65:
      case 0x66:
      case 0x67:
            E_font_I_print( ",local" );
            //E_aml_S_stack[ E_aml_S_stack_n - 1 ].result =
            break;
      default: // simple name
        {   E_aml_S_stack_data--;
            N n;
            Pc name = E_aml_Q_path_R_root( &n );
            if( !name )
                return n;
            Pc name_ = M( 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, 4 );
            E_font_I_print( ",simple name=" ); E_font_I_print( name_ );
            W( name_ );
            //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje.
            E_aml_S_stack[ E_aml_S_stack_n - 1 ].result.pathname.s = name;
            E_aml_S_stack[ E_aml_S_stack_n - 1 ].result.pathname.n = n;
            break;
        }
    }
    return 0;
}
#undef E_aml_I_delegate_pkg
#undef E_aml_I_delegate
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define E_aml_I_delegate( finish, entity_ ) \
{   if( !E_mem_Q_blk_I_append( &E_aml_S_stack, 1 )) \
        goto Error; \
    Pc data_end = E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end; \
    E_aml_S_stack[ E_aml_S_stack_n - 1 ].entity = finish; \
    E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = 0; \
    E_aml_S_stack[ E_aml_S_stack_n ].data_end = 0; \
    E_aml_S_stack[ E_aml_S_stack_n ].entity = entity_; \
    E_aml_S_stack[ E_aml_S_stack_n ].n = 1; \
    E_aml_S_stack[ E_aml_S_stack_n ].data_end = data_end; \
    E_aml_S_stack_n++; \
    stack_n_last = E_aml_S_stack_n; \
}
_private
N
E_aml_M( Pc table
, N l
){  E_aml_S_current_path_n = 0;
    Mt_( E_aml_S_current_path, E_aml_S_current_path_n );
    if( !E_aml_S_current_path )
        return ~0;
    E_aml_S_stack_n = 1;
    Mt_( E_aml_S_stack, E_aml_S_stack_n );
    if( !E_aml_S_stack )
    {   W( E_aml_S_current_path );
        return ~0;
    }
    if( !~E_aml_Q_object_name_alias_M() )
    {   W( E_aml_S_stack );
        W( E_aml_S_current_path );
        return ~0;
    }
    if( !~E_aml_Q_object_M() )
    {   E_aml_Q_object_name_alias_W();
        W( E_aml_S_stack );
        W( E_aml_S_current_path );
        return ~0;
    }
    E_aml_S_stack_data = table;
    E_aml_S_stack[ E_aml_S_stack_n - 1 ].entity = E_aml_Z_stack_Z_entity_S_term;
    E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = ~0;
    E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end = table + l;
    N stack_n_last;
    union E_aml_Z_stack_Z_result result;
    Pc data = 0;
    N ret = 0;
Loop:
    O{  stack_n_last = E_aml_S_stack_n;
        enum E_aml_Z_stack_Z_entity entity = E_aml_S_stack[ E_aml_S_stack_n - 1 ].entity;
        //E_font_I_print( "\nentity=" ); E_font_I_print_hex(entity); E_font_I_print( ",data=" ); E_font_I_print_hex( (N)E_aml_S_stack_data ); E_font_I_print( ",data_end=" ); E_font_I_print_hex( (N)E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end ); E_font_I_print( ",value=" ); E_font_I_print_hex( (( N * )E_aml_S_stack_data )[0] ); //E_font_I_print( "," ); E_font_I_print_hex( (( N * )E_aml_S_stack_data )[1] );
        switch(entity)
        { case E_aml_Z_stack_Z_entity_S_result_to_n:
                E_aml_S_stack[ E_aml_S_stack_n - 2 ].n = result.n;
                break;
          case E_aml_Z_stack_Z_entity_S_restore_current_path:
                W( E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].s );
                if( !E_mem_Q_blk_I_remove( &E_aml_S_current_path, --E_aml_S_current_path_n, 1 ))
                    goto Error;
                break;
          case E_aml_Z_stack_Z_entity_S_term:
                ret = E_aml_I_term();
                break;
          case E_aml_Z_stack_Z_entity_S_term_finish_1:
                if( E_aml_S_stack[ E_aml_S_stack_n - 1 ].match )
                    break;
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_term_finish_2, E_aml_Z_stack_Z_entity_S_expression );
                break;
          case E_aml_Z_stack_Z_entity_S_term_finish_2:
                break;
          case E_aml_Z_stack_Z_entity_S_term_arg:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                switch( (N8)*E_aml_S_stack_data++ )
                { case 0x68: // arg
                  case 0x69:
                  case 0x6a:
                  case 0x6b:
                  case 0x6c:
                  case 0x6d:
                  case 0x6e:
                        E_font_I_print( ",arg" );
                        //result =
                        break;
                  case 0x60: // local
                  case 0x61:
                  case 0x62:
                  case 0x63:
                  case 0x64:
                  case 0x65:
                  case 0x66:
                  case 0x67:
                        E_font_I_print( ",local" );
                        //result =
                        break;
                  default:
                    {   E_aml_S_stack_data--;
                        if( !E_mem_Q_blk_I_append( &E_aml_S_stack, 2 ))
                            goto Error;
                        Pc data_end = E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end;
                        E_aml_S_stack[ E_aml_S_stack_n ].entity = E_aml_Z_stack_Z_entity_S_term_arg_finish_1;
                        E_aml_S_stack[ E_aml_S_stack_n ].n = 0;
                        E_aml_S_stack[ E_aml_S_stack_n ].data_end = 0;
                        E_aml_S_stack_n++;
                        E_aml_S_stack[ E_aml_S_stack_n ].entity = E_aml_Z_stack_Z_entity_S_data_object;
                        E_aml_S_stack[ E_aml_S_stack_n ].n = 1;
                        E_aml_S_stack[ E_aml_S_stack_n ].data_end = data_end;
                        E_aml_S_stack_n++;
                        break;
                    }
                }
                break;
          case E_aml_Z_stack_Z_entity_S_data_object:
                ret = E_aml_I_data_object();
                if( ret == ~0 - 1
                && E_aml_S_stack[ E_aml_S_stack_n - 2 ].entity == E_aml_Z_stack_Z_entity_S_term_arg_finish_1
                )
                    ret = 0;
                result = E_aml_S_stack[ E_aml_S_stack_n - 1 ].result;
                break;
          case E_aml_Z_stack_Z_entity_S_term_arg_finish_1:
                if( E_aml_S_stack[ E_aml_S_stack_n - 1 ].match )
                    break;
                E_aml_S_stack[ E_aml_S_stack_n - 1 ].entity = E_aml_Z_stack_Z_entity_S_expression;
                E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = 1;
                break;
          case E_aml_Z_stack_Z_entity_S_object:
                ret = E_aml_I_object();
                break;
          case E_aml_Z_stack_Z_entity_S_expression:
                ret = E_aml_I_expression();
                result = E_aml_S_stack[ E_aml_S_stack_n - 1 ].result;
                break;
          case E_aml_Z_stack_Z_entity_S_supername:
                ret = E_aml_I_supername();
                result = E_aml_S_stack[ E_aml_S_stack_n - 1 ].result;
                break;
          case E_aml_Z_stack_Z_entity_S_data_object_buffer_finish:
                E_aml_S_stack_data = E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end; //TODO Dopóki nie interpretuję wartości z programu.
                //if( E_aml_S_stack_data + result.n > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                //{   ret = ~0 - 1;
                    //break;
                //}
                //TODO Dodać ‘buffer’ do drzewa zinterpretowanej przestrzeni ACPI.
                //E_aml_S_stack_data += result.n;
                break;
          case E_aml_Z_stack_Z_entity_S_package:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end ) // Na przypadek startu z “n == 0”.
                    break;
                ret = E_aml_I_data_object();
                if( (S)ret < 0 )
                    break;
                if( !E_aml_S_stack[ stack_n_last - 2 ].match )
                {   N n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                    {   ret = n;
                        break;
                    }
                    //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje.
                }
                break;
          case E_aml_Z_stack_Z_entity_S_package_finish:
                break;
          case E_aml_Z_stack_Z_entity_S_supername_ref_finish:
                //TODO Dodać ‘ref object’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_supername_deref_finish:
                //TODO Dodać ‘object ref’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_supername_index_finish_1:
                //TODO Dodać ‘buffer’, ‘pkg’ lub ‘string’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_supername_index_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_supername_index_finish_2:
                //TODO Dodać ‘index’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_supername_index_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                break;
          case E_aml_Z_stack_Z_entity_S_supername_index_finish_3:
                //TODO Dodać ‘target’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_bank_field_finish:
            {   if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                E_aml_S_stack_data++;
                while( E_aml_S_stack_data != E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   switch( *E_aml_S_stack_data++ )
                    { case 0: // reserved field
                        {   if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                            {   ret = ~0 - 1;
                                break;
                            }
                            N n = E_aml_R_pkg_length();
                            if( !~n )
                            {   ret = ~0 - 1;
                                break;
                            }
                            E_font_I_print( ", reserved field" );
                            //TODO Dodać ‘field pkg length’ do drzewa zinterpretowanej przestrzeni ACPI.
                            break;
                        }
                      case 1: // access field
                            if( E_aml_S_stack_data + 2 > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                            {   ret = ~0 - 1;
                                break;
                            }
                            //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                            E_aml_S_stack_data++;
                            //TODO Dodać ‘access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                            E_aml_S_stack_data++;
                            break;
                      case 2: // connect field
                            E_font_I_print( ", connect field" );
                            goto Error; //NDFN
                            break;
                      case 3: // extended access field
                            if( E_aml_S_stack_data + 3 > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                            {   ret = ~0 - 1;
                                break;
                            }
                            //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                            E_aml_S_stack_data++;
                            //TODO Dodać ‘extended access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                            E_aml_S_stack_data++;
                            //TODO Dodać ‘access length’ do drzewa zinterpretowanej przestrzeni ACPI.
                            E_aml_S_stack_data++; //NDFN Jaki rozmiar.
                            break;
                      default: // named field
                        {   if( E_aml_S_stack_data + 4 > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                            {   ret = ~0 - 1;
                                break;
                            }
                            Pc name_ = M( 4 + 1 );
                            E_text_Z_s_P_copy_sl_0( name_, E_aml_S_stack_data, 4 );
                            E_font_I_print( ",field=" ); E_font_I_print( name_ );
                            W( name_ );
                            //TODO Dodać ‘field name’ do drzewa zinterpretowanej przestrzeni ACPI.
                            E_aml_S_stack_data += 4;
                            N n = E_aml_R_pkg_length();
                            if( !~n )
                            {   ret = ~0 - 1;
                                break;
                            }
                            E_font_I_print( ", field" );
                            //TODO Dodać ‘field pkg length’ do drzewa zinterpretowanej przestrzeni ACPI.
                            break;
                        }
                    }
                }
                break;
            }
          case E_aml_Z_stack_Z_entity_S_bit_field_finish_1:
                //TODO Dodać ‘buffer’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_bit_field_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_bit_field_finish_2:
            {   //TODO Dodać ‘bit index’ do drzewa zinterpretowanej przestrzeni ACPI.
                N n;
                Pc name = E_aml_Q_path_R_root( &n );
                if( !name )
                {   ret = n;
                    break;
                }
                Pc name_ = M( n * 4 + 1 );
                E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                E_font_I_print( ",bit field=" ); E_font_I_print( name_ );
                W( name_ );
                //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje (oprócz ostatniego obiektu, który teraz jest definiowany).
                //TODO Dodać ‘name’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
            }
          case E_aml_Z_stack_Z_entity_S_byte_field_finish_1:
                //TODO Dodać ‘buffer’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_byte_field_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_byte_field_finish_2:
            {   //TODO Dodać ‘byte index’ do drzewa zinterpretowanej przestrzeni ACPI.
                N n;
                Pc name = E_aml_Q_path_R_root( &n );
                if( !name )
                {   ret = n;
                    break;
                }
                Pc name_ = M( n * 4 + 1 );
                E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                E_font_I_print( ",qword field=" ); E_font_I_print( name_ );
                W( name_ );
                //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje (oprócz ostatniego obiektu, który teraz jest definiowany).
                //TODO Dodać ‘name’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
            }
          case E_aml_Z_stack_Z_entity_S_dword_field_finish_1:
                //TODO Dodać ‘buffer’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_dword_field_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_dword_field_finish_2:
            {   //TODO Dodać ‘byte index’ do drzewa zinterpretowanej przestrzeni ACPI.
                N n;
                Pc name = E_aml_Q_path_R_root( &n );
                if( !name )
                {   ret = n;
                    break;
                }
                Pc name_ = M( n * 4 + 1 );
                E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                E_font_I_print( ",dword field=" ); E_font_I_print( name_ );
                W( name_ );
                //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje (oprócz ostatniego obiektu, który teraz jest definiowany).
                //TODO Dodać ‘name’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
            }
          case E_aml_Z_stack_Z_entity_S_field_finish_1:
                //TODO Dodać ‘buffer’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_field_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_field_finish_2:
                //TODO Dodać ‘bit index’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_field_finish_3, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_field_finish_3:
            {   //TODO Dodać ‘num bits’ do drzewa zinterpretowanej przestrzeni ACPI.
                N n;
                Pc name = E_aml_Q_path_R_root( &n );
                if( !name )
                {   ret = n;
                    break;
                }
                Pc name_ = M( n * 4 + 1 );
                E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                E_font_I_print( ",field=" ); E_font_I_print( name_ );
                W( name_ );
                //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje (oprócz ostatniego obiektu, który teraz jest definiowany).
                //TODO Dodać ‘name’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
            }
          case E_aml_Z_stack_Z_entity_S_qword_field_finish_1:
                //TODO Dodać ‘buffer’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_qword_field_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_qword_field_finish_2:
            {   //TODO Dodać ‘byte index’ do drzewa zinterpretowanej przestrzeni ACPI.
                N n;
                Pc name = E_aml_Q_path_R_root( &n );
                if( !name )
                {   ret = n;
                    break;
                }
                Pc name_ = M( n * 4 + 1 );
                E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                E_font_I_print( ",qword field=" ); E_font_I_print( name_ );
                W( name_ );
                //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje (oprócz ostatniego obiektu, który teraz jest definiowany).
                //TODO Dodać ‘name’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
            }
          case E_aml_Z_stack_Z_entity_S_word_field_finish_1:
                //TODO Dodać ‘buffer’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_word_field_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_word_field_finish_2:
            {   //TODO Dodać ‘byte index’ do drzewa zinterpretowanej przestrzeni ACPI.
                N n;
                Pc name = E_aml_Q_path_R_root( &n );
                if( !name )
                {   ret = n;
                    break;
                }
                Pc name_ = M( n * 4 + 1 );
                E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                E_font_I_print( ",word field=" ); E_font_I_print( name_ );
                W( name_ );
                //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje (oprócz ostatniego obiektu, który teraz jest definiowany).
                //TODO Dodać ‘name’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
            }
          case E_aml_Z_stack_Z_entity_S_data_region_finish_1:
                //TODO Dodać ‘signature’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_data_region_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_data_region_finish_2:
                //TODO Dodać ‘oem id’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_data_region_finish_3, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_data_region_finish_3:
                //TODO Dodać ‘oem table id’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_op_region_finish_1:
                //TODO Dodać ‘region offset’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_op_region_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_op_region_finish_2:
                //TODO Dodać ‘region len’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_power_res_finish:
                //TODO Dodać ‘power res’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_S_stack[ E_aml_S_stack_n - 1 ].entity = E_aml_Z_stack_Z_entity_S_restore_current_path;
                goto Loop;
          case E_aml_Z_stack_Z_entity_S_thermal_zone_finish:
                //TODO Dodać ‘thermal zone’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_S_stack[ E_aml_S_stack_n - 1 ].entity = E_aml_Z_stack_Z_entity_S_restore_current_path;
                goto Loop;
          case E_aml_Z_stack_Z_entity_S_device_finish:
                //TODO Dodać ‘device’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_S_stack[ E_aml_S_stack_n - 1 ].entity = E_aml_Z_stack_Z_entity_S_restore_current_path;
                goto Loop;
          case E_aml_Z_stack_Z_entity_S_processor_finish:
                //TODO Dodać ‘processor’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_S_stack[ E_aml_S_stack_n - 1 ].entity = E_aml_Z_stack_Z_entity_S_restore_current_path;
                goto Loop;
          case E_aml_Z_stack_Z_entity_S_if_op_finish_1:
                if( E_aml_S_stack_data != E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_if_op_finish_2, E_aml_Z_stack_Z_entity_S_term );
                    E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = ~0;
                }
                break;
          case E_aml_Z_stack_Z_entity_S_if_op_finish_2:
                //TODO Dodać ‘if’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_else_op_finish:
                //TODO Dodać ‘else’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_notify_finish_1:
                //TODO Dodać ‘notify object’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_notify_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_notify_finish_2:
                //TODO Dodać ‘notify value’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_release_finish:
                //TODO Zwolnić ‘mutex’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_reset_finish:
                //TODO ‘Zresetować’ ‘event’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_return_finish:
                //TODO Wykonać ‘return’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_signal_finish:
                //TODO Sygnalizować ‘event’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_sleep_finish:
                //TODO Wstrzymać wykonywanie w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_stall_finish:
                //TODO Wstrzymać wykonywanie w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_while_op_finish_1:
                if( E_aml_S_stack_data != E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_while_op_finish_2, E_aml_Z_stack_Z_entity_S_term );
                    E_aml_S_stack[ E_aml_S_stack_n - 1 ].n = ~0;
                }
                break;
          case E_aml_Z_stack_Z_entity_S_while_op_finish_2:
                //TODO Dodać ‘while loop’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_mutex_finish:
                //TODO Dodać ‘mutex’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_S_stack_data += 2;
                break;
          case E_aml_Z_stack_Z_entity_S_add_op_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_add_op_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_add_op_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_add_op_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_add_op_finish_3:
                //TODO Umieścić operację dodawania w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_and_op_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_and_op_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_and_op_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_and_op_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_and_op_finish_3:
                //TODO Umieścić operację ‘and’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_buffer_finish:
                //TODO Umieścić operację ‘and’ w drzewie zinterpretowanej przestrzeni ACPI.
                E_aml_S_stack_data += result.n;
                break;
          case E_aml_Z_stack_Z_entity_S_concat_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_concat_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_concat_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_concat_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_concat_finish_3:
                //TODO Umieścić operację ‘concat’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_concat_res_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_concat_res_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_concat_res_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_concat_res_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_concat_res_finish_3:
                //TODO Umieścić operację ‘concat res’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_cond_refof_finish_1:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_cond_refof_finish_2, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_cond_refof_finish_2:
                //TODO Umieścić operację ‘concat refof’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_copy_object_finish:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                switch( (N8)*E_aml_S_stack_data )
                { case 0x68: // arg
                  case 0x69:
                  case 0x6a:
                  case 0x6b:
                  case 0x6c:
                  case 0x6d:
                  case 0x6e:
                        E_font_I_print( ",arg" );
                        break;
                  case 0x60: // local
                  case 0x61:
                  case 0x62:
                  case 0x63:
                  case 0x64:
                  case 0x65:
                  case 0x66:
                  case 0x67:
                        E_font_I_print( ",local" );
                        break;
                  default:
                    {   N n;
                        Pc name = E_aml_Q_path_R_root( &n );
                        if( !name )
                        {   ret = n;
                            break;
                        }
                        Pc name_ = M( n * 4 + 1 );
                        E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                        E_font_I_print( ", object name=" ); E_font_I_print( name_ );
                        W( name_ );
                        //TODO Sprawdzić w drzewie zinterpretowanej przestrzeni ACPI, czy ścieżka istnieje.
                        W(name);
                    }
                }
                break;
          case E_aml_Z_stack_Z_entity_S_decrement_finish:
                //TODO Umieścić operację ‘decrement’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_derefof_finish:
                //TODO Umieścić operację ‘derefof’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_divide_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_divide_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_divide_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_divide_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_divide_finish_3:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_divide_finish_4, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_divide_finish_4:
                //TODO Umieścić operację ‘divide’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_fslb_finish_1:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_fslb_finish_2, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_fslb_finish_2:
                //TODO Umieścić operację ‘find set left bit’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_fsrb_finish_1:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_fsrb_finish_2, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_fsrb_finish_2:
                //TODO Umieścić operację ‘find set right bit’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_from_bcd_finish_1:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_from_bcd_finish_2, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_from_bcd_finish_2:
                //TODO Umieścić operację ‘from bcd’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_increment_finish:
                //TODO Umieścić operację ‘increment’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_index_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_index_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_index_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_index_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_index_finish_3:
                //TODO Umieścić operację ‘index’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_land_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_land_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_land_finish_2:
                //TODO Umieścić operację ‘land’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_lequal_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lequal_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_lequal_finish_2:
                //TODO Umieścić operację ‘lequal’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_lgreater_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lgreater_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_lgreater_finish_2:
                //TODO Umieścić operację ‘lgreater’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_lgreater_equal_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lgreater_equal_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_lgreater_equal_finish_2:
                //TODO Umieścić operację ‘lgreater equal’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_lless_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lless_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_lless_finish_2:
                //TODO Umieścić operację ‘lless’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_lless_equal_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lless_equal_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_lless_equal_finish_2:
                //TODO Umieścić operację ‘lless equal’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_mid_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_mid_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_mid_finish_2:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_mid_finish_3, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_mid_finish_3:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_mid_finish_4, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_mid_finish_4:
                //TODO Umieścić operację ‘mid’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_lnot_finish:
                //TODO Umieścić operację ‘lnot’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_lnot_equal_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lnot_equal_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_lnot_equal_finish_2:
                //TODO Umieścić operację ‘lnot equal’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_load_table_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_load_table_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_load_table_finish_2:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_load_table_finish_3, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_load_table_finish_3:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_load_table_finish_4, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_load_table_finish_4:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_load_table_finish_5, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_load_table_finish_5:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_load_table_finish_6, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_load_table_finish_6:
                //TODO Umieścić operację ‘load table’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_lor_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_lor_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_lor_finish_2:
                //TODO Umieścić operację ‘lor’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_match_finish_1:
                if( ++E_aml_S_stack_data > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_match_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_match_finish_2:
                if( ++E_aml_S_stack_data > E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_match_finish_3, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_match_finish_3:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_match_finish_4, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_match_finish_4:
                //TODO Umieścić operację ‘match’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_mod_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_mod_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_mod_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_divide_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_mod_finish_3:
                //TODO Umieścić operację ‘mod’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_multiply_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_multiply_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_multiply_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_multiply_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_multiply_finish_3:
                //TODO Umieścić operację ‘multiply’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_nand_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_nand_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_nand_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_nand_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_nand_finish_3:
                //TODO Umieścić operację ‘nand’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_nor_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_nor_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_nor_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_nor_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_nor_finish_3:
                //TODO Umieścić operację ‘nor’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_not_finish_1:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_not_finish_2, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_not_finish_2:
                //TODO Umieścić operację ‘not’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_object_type_finish:
                //TODO Umieścić operację ‘object type’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_or_finish_1:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_or_finish_2, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_or_finish_2:
                //TODO Umieścić operację ‘or’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_refof_finish:
                //TODO Umieścić operację ‘refof’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_shift_left_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_shift_left_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_shift_left_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_shift_left_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_shift_left_finish_3:
                //TODO Umieścić operację ‘shift left’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_shift_right_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_shift_left_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_shift_right_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_shift_right_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_shift_right_finish_3:
                //TODO Umieścić operację ‘shift right’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_sizeof_finish:
                //TODO Umieścić operację ‘sizeof’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_store_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_store_finish_2, E_aml_Z_stack_Z_entity_S_supername );
                break;
          case E_aml_Z_stack_Z_entity_S_store_finish_2:
                //TODO Umieścić operację ‘store’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_subtract_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_subtract_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_subtract_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_subtract_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_subtract_finish_3:
                //TODO Umieścić operację ‘subtract’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_to_bcd_finish_1:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_to_bcd_finish_2, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_to_bcd_finish_2:
                //TODO Umieścić operację ‘to bcd’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_to_buffer_finish_1:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_to_buffer_finish_2, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_to_buffer_finish_2:
                //TODO Umieścić operację ‘to buffer’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_to_decimal_string_finish_1:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_to_decimal_string_finish_2, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_to_decimal_string_finish_2:
                //TODO Umieścić operację ‘to decimal string’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_to_hex_string_finish_1:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_to_hex_string_finish_2, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_to_hex_string_finish_2:
                //TODO Umieścić operację ‘to decimal string’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_to_integer_finish_1:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_to_integer_finish_2, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_to_integer_finish_2:
                //TODO Umieścić operację ‘to integer’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_to_string_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_to_string_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_to_string_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_to_string_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_to_string_finish_3:
                //TODO Umieścić operację ‘to integer’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_wait_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_wait_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_wait_finish_2:
                //TODO Umieścić operację ‘to integer’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_xor_finish_1:
                E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_xor_finish_2, E_aml_Z_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_stack_Z_entity_S_xor_finish_2:
                if( E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_stack_data )
                {   E_aml_I_delegate( E_aml_Z_stack_Z_entity_S_xor_finish_3, E_aml_Z_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_stack_data++;
          case E_aml_Z_stack_Z_entity_S_xor_finish_3:
                //TODO Umieścić operację ‘nor’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_fatal_finish:
                //TODO Umieścić operację ‘fatal’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_stack_Z_entity_S_load_finish:
                //TODO Umieścić operację ‘load’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
        }
        if( !~ret )
            goto Error;
        if( ret == ~0 - 1 )
        {   if( E_aml_S_stack_n >= 2
            && E_aml_S_current_path_n
            )
            {   N current_path_i = E_aml_S_current_path_n - 1;
                for_n_rev( i, E_aml_S_stack_n ) // Wyjście do nadrzędnego bloku metody po błędzie składni.
                {   if(( E_aml_S_stack[i].entity == E_aml_Z_stack_Z_entity_S_restore_current_path
                      || E_aml_S_stack[i].entity == E_aml_Z_stack_Z_entity_S_power_res_finish
                      || E_aml_S_stack[i].entity == E_aml_Z_stack_Z_entity_S_thermal_zone_finish
                    )
                    && ~E_aml_S_object_R( E_aml_S_current_path[ current_path_i-- ] )
                    )
                    {   for_n( j, E_aml_S_current_path_n - ( current_path_i + 1 ))
                            W( E_aml_S_current_path[ current_path_i + 1 + j ].s );
                        if( !E_mem_Q_blk_I_remove( &E_aml_S_current_path, current_path_i + 1, E_aml_S_current_path_n - ( current_path_i + 1 )))
                            goto Error;
                        E_aml_S_current_path_n -= E_aml_S_current_path_n - ( current_path_i + 1 );
                        if( E_aml_S_current_path_n )
                        {   Pc name_ = M( E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n * 4 + 1 );
                            E_text_Z_s_P_copy_sl_0( name_, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].s, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n * 4 );
                            E_font_I_print( "\n,go over to=" ); E_font_I_print( name_ ); E_font_I_print_hex(i);
                            W( name_ );
                        }else
                        {   E_font_I_print( "\n,go over to=\\" ); E_font_I_print_hex(i);
                        }
                        if( !~E_aml_S_stack[ i - 1 ].n ) // Sprawdzanie dla listy wyliczanej w nieskończoność, czy interpretacja zakończyła się.
                            if( !--i )
                                break;
                        E_aml_S_stack_data = E_aml_S_stack[i].data_end;
                        if( !E_mem_Q_blk_I_remove( &E_aml_S_stack, i, E_aml_S_stack_n - i ))
                            goto Error;
                        E_aml_S_stack_n = i;
                        ret = 0;
                        goto Loop;
                    }
                    if( !~current_path_i )
                        break;
                }
            }
            goto Error;
        }
        //E_font_I_print( "\nstack_n=" ); E_font_I_print_hex( E_aml_S_stack_n );
        //E_font_I_print( ",stack_n_last=" ); E_font_I_print_hex( stack_n_last );
        //for_n( j, E_aml_S_stack_n )
        //{   E_font_I_print( "\n-entity=" ); E_font_I_print_hex( E_aml_S_stack[j].entity );
            //E_font_I_print( ",n=" ); E_font_I_print_hex( E_aml_S_stack[j].n );
            //E_font_I_print( ",data_end=" ); E_font_I_print_hex( (N)E_aml_S_stack[j].data_end );
            //E_font_I_print( ",result=" ); E_font_I_print_hex( E_aml_S_stack[j].result.n );
        //}
        // Usunięcie wykananego “entity” po ‘push’.
        if( stack_n_last != E_aml_S_stack_n
        && ~E_aml_S_stack[ stack_n_last - 1 ].n
        && !E_aml_S_stack[ stack_n_last - 1 ].n--
        )
        {   if( !E_mem_Q_blk_I_remove( &E_aml_S_stack, stack_n_last - 1, 1 ))
                goto Error;
            E_aml_S_stack_n--;
        }
        //for_n_( j, E_aml_S_stack_n )
        //{   E_font_I_print( "\n-entity=" ); E_font_I_print_hex( E_aml_S_stack[j].entity );
            //E_font_I_print( ",n=" ); E_font_I_print_hex( E_aml_S_stack[j].n );
            //E_font_I_print( ",data_end=" ); E_font_I_print_hex( (N)E_aml_S_stack[j].data_end );
            //E_font_I_print( ",result=" ); E_font_I_print_hex( E_aml_S_stack[j].result.n );
        //}
        if( E_aml_S_stack_n >= 2 )
        {   if( ~E_aml_S_stack[ E_aml_S_stack_n - 1 ].n // Czy bieżący element nie jest listą wyliczaną w nieskończoność.
            && E_aml_S_stack[ E_aml_S_stack_n - 2 ].data_end // Czy poprzedni element na stosie nie jest nie zainicjowanym którymś “finish”.
            && E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end != E_aml_S_stack[ E_aml_S_stack_n - 2 ].data_end // Czy było ustawione “pkg_length”.
            && E_aml_S_stack_data != E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end 
            ) // Sprawdzanie dla konkretnych “n”, czy interpretacja zakończyła się na końcu wcześniej odczytanego rozmiaru “pkg_length”.
            {   if( E_aml_S_stack_n >= 2
                && E_aml_S_current_path_n
                )
                {   N current_path_i = E_aml_S_current_path_n - 1;
                    for_n_rev( i, E_aml_S_stack_n ) // Wyjście do nadrzędnego bloku metody po błędzie składni.
                    {   if(( E_aml_S_stack[i].entity == E_aml_Z_stack_Z_entity_S_restore_current_path
                          || E_aml_S_stack[i].entity == E_aml_Z_stack_Z_entity_S_power_res_finish
                          || E_aml_S_stack[i].entity == E_aml_Z_stack_Z_entity_S_thermal_zone_finish
                        )
                        && ~E_aml_S_object_R( E_aml_S_current_path[ current_path_i-- ] )
                        )
                        {   for_n( j, E_aml_S_current_path_n - ( current_path_i + 1 ))
                                W( E_aml_S_current_path[ current_path_i + 1 + j ].s );
                            if( !E_mem_Q_blk_I_remove( &E_aml_S_current_path, current_path_i + 1, E_aml_S_current_path_n - ( current_path_i + 1 )))
                                goto Error;
                            E_aml_S_current_path_n -= E_aml_S_current_path_n - ( current_path_i + 1 );
                            if( E_aml_S_current_path_n )
                            {   Pc name_ = M( E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n * 4 + 1 );
                                E_text_Z_s_P_copy_sl_0( name_, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].s, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n * 4 );
                                E_font_I_print( "\n,go over to=" ); E_font_I_print( name_ ); E_font_I_print_hex(i);
                                W( name_ );
                            }else
                            {   E_font_I_print( "\n,go over to=\\" ); E_font_I_print_hex(i);
                            }
                            if( !~E_aml_S_stack[ i - 1 ].n ) // Sprawdzanie dla listy wyliczanej w nieskończoność, czy interpretacja zakończyła się.
                                if( !--i )
                                    break;
                            E_aml_S_stack_data = E_aml_S_stack[i].data_end;
                            if( !E_mem_Q_blk_I_remove( &E_aml_S_stack, i, E_aml_S_stack_n - i ))
                                goto Error;
                            E_aml_S_stack_n = i;
                            goto Loop;
                        }
                        if( !~current_path_i )
                            break;
                    }
                }
                goto Error;
            }
            if( !E_aml_S_stack[ E_aml_S_stack_n - 2 ].data_end ) // Czy poprzedni element na stosie jest nie zainicjowanym którymś “finish”.
                E_aml_S_stack[ E_aml_S_stack_n - 2 ].data_end = E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end; // Zachowaj “data_end” do sprawdzenia podczas któregoś “finish”.
        }
        if( ~E_aml_S_stack[ E_aml_S_stack_n - 1 ].n
        && !E_aml_S_stack[ E_aml_S_stack_n - 1 ].n--
        )
            if( !E_mem_Q_blk_I_remove( &E_aml_S_stack, --E_aml_S_stack_n, 1 ))
                goto Error;
        if( !~E_aml_S_stack[ E_aml_S_stack_n - 1 ].n
        && E_aml_S_stack_data == E_aml_S_stack[ E_aml_S_stack_n - 1 ].data_end
        ) // Sprawdzanie dla listy wyliczanej w nieskończoność, czy interpretacja zakończyła się.
        {   if( !--E_aml_S_stack_n )
                break;
            if( !E_mem_Q_blk_I_remove( &E_aml_S_stack, E_aml_S_stack_n, 1 ))
                goto Error;
        }
        //for_n_( j, E_aml_S_stack_n )
        //{   E_font_I_print( "\n-entity=" ); E_font_I_print_hex( E_aml_S_stack[j].entity );
            //E_font_I_print( ",n=" ); E_font_I_print_hex( E_aml_S_stack[j].n );
            //E_font_I_print( ",data_end=" ); E_font_I_print_hex( (N)E_aml_S_stack[j].data_end );
            //E_font_I_print( ",result=" ); E_font_I_print_hex( E_aml_S_stack[j].result.n );
        //}
        if( E_aml_S_stack[ E_aml_S_stack_n - 1 ].n > 1 )
        {   if( data == E_aml_S_stack_data ) // Czy kolejne “n” nic nie wykonują.
            {   if( E_aml_S_stack_n >= 2
                && E_aml_S_current_path_n
                )
                {   N current_path_i = E_aml_S_current_path_n - 1;
                    for_n_rev( i, E_aml_S_stack_n ) // Wyjście do nadrzędnego bloku metody po błędzie składni.
                    {   if(( E_aml_S_stack[i].entity == E_aml_Z_stack_Z_entity_S_restore_current_path
                          || E_aml_S_stack[i].entity == E_aml_Z_stack_Z_entity_S_power_res_finish
                          || E_aml_S_stack[i].entity == E_aml_Z_stack_Z_entity_S_thermal_zone_finish
                        )
                        && ~E_aml_S_object_R( E_aml_S_current_path[ current_path_i-- ] )
                        )
                        {   for_n( j, E_aml_S_current_path_n - ( current_path_i + 1 ))
                                W( E_aml_S_current_path[ current_path_i + 1 + j ].s );
                            if( !E_mem_Q_blk_I_remove( &E_aml_S_current_path, current_path_i + 1, E_aml_S_current_path_n - ( current_path_i + 1 )))
                                goto Error;
                            E_aml_S_current_path_n -= E_aml_S_current_path_n - ( current_path_i + 1 );
                            if( E_aml_S_current_path_n )
                            {   Pc name_ = M( E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n * 4 + 1 );
                                E_text_Z_s_P_copy_sl_0( name_, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].s, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n * 4 );
                                E_font_I_print( "\n,go over to=" ); E_font_I_print( name_ ); E_font_I_print_hex(i);
                                W( name_ );
                            }else
                            {   E_font_I_print( "\n,go over to=\\" ); E_font_I_print_hex(i);
                            }
                            if( !~E_aml_S_stack[ i - 1 ].n ) // Sprawdzanie dla listy wyliczanej w nieskończoność, czy interpretacja zakończyła się.
                                if( !--i )
                                    break;
                            E_aml_S_stack_data = E_aml_S_stack[i].data_end;
                            if( !E_mem_Q_blk_I_remove( &E_aml_S_stack, i, E_aml_S_stack_n - i ))
                                goto Error;
                            E_aml_S_stack_n = i;
                            ret = 0;
                            goto Loop;
                        }
                        if( !~current_path_i )
                            break;
                    }
                }
                goto Error;
            }else
                data = E_aml_S_stack_data;
        }
    }
    W( E_aml_S_stack );
    for_n( i, E_aml_S_current_path_n )
        W( E_aml_S_current_path[i].s );
    W( E_aml_S_current_path );
    return 0;
Error:
    E_aml_Q_object_W();
    E_aml_Q_object_name_alias_W();
    W( E_aml_S_stack );
    for_n_( i, E_aml_S_current_path_n )
        W( E_aml_S_current_path[i].s );
    W( E_aml_S_current_path );
    E_font_I_print( "\nerror" );
    return ~0;
}
/******************************************************************************/
