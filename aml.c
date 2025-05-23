/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         AML interpreter
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”             2025‒5‒13 I
*******************************************************************************/
//DFN W tym interpretatorze AML wartości albo są liczbowe, albo są referencjami do obiektów. Natomiast wartość może być kopią, jeśli powstała z algorytmu przetwarzania. Dopiero podczas przypisywania do obiektu wartości są kopiowane, jeśli nie były kopiami. Tzn. operator ASL “DerefOf” działa z opóźnieniem, a “RefOf” kopiuje wartość.
//TODO Czy da się nie obliczać drugiego argumentu dla instrukcji “LOr”, jeśli nie potrzeba?
#include "main.h"
//==============================================================================
struct E_aml_Z_pathname
{ Pc s;
  N8 n;
};
_internal struct E_aml_Z_pathname *E_aml_S_current_path;
_internal N E_aml_S_current_path_n;
_internal N E_aml_Q_current_path_S_precompilation_i;
//------------------------------------------------------------------------------
struct E_aml_Z_buffer
{ Pc p;
  N n;
};
enum E_aml_Z_value_Z_type
{ E_aml_Z_value_Z_type_S_uninitialized
, E_aml_Z_value_Z_type_S_number
, E_aml_Z_value_Z_type_S_string
, E_aml_Z_value_Z_type_S_buffer
, E_aml_Z_value_Z_type_S_package
, E_aml_Z_value_Z_type_S_pathname
, E_aml_Z_value_Z_type_S_arg_ref
, E_aml_Z_value_Z_type_S_local_ref
, E_aml_Z_value_Z_type_S_debug_ref
};
struct E_aml_Z_value
{ union
  { struct E_aml_Z_buffer buffer;
    struct E_aml_Z_pathname pathname;
    struct E_aml_Z_package *package;
    P p;
    N n;
  };
  N i; // Jeśli “type == E_aml_Z_value_Z_type_S_local_ref”, to przechowuje numer zmiennej lokalnej, w przeciwnym przypadku jeśli wartość “!~0”, to jest indeksem obiektu.
  enum E_aml_Z_value_Z_type type;
  B copy;
};
struct E_aml_Z_package
{ struct E_aml_Z_value *value;
  N8 n;
};
//------------------------------------------------------------------------------
_internal
struct
{ struct E_aml_Z_pathname procedure;
  struct E_aml_Z_value arg[7], local[8];
} *E_aml_S_procedure_invocation_stack;
_internal
N E_aml_S_procedure_invocation_stack_n;
//------------------------------------------------------------------------------
struct E_aml_Z_object_data_Z_procedure
{ N8 arg_n;
};
enum E_aml_S_object_Z_type
{ E_aml_Z_object_Z_type_S_uninitialized
, E_aml_Z_object_Z_type_S_number
, E_aml_Z_object_Z_type_S_string
, E_aml_Z_object_Z_type_S_buffer
, E_aml_Z_object_Z_type_S_package
, E_aml_Z_object_Z_type_S_field_unit
, E_aml_Z_object_Z_type_S_device
, E_aml_Z_object_Z_type_S_event
, E_aml_Z_object_Z_type_S_procedure
, E_aml_Z_object_Z_type_S_mutex
, E_aml_Z_object_Z_type_S_op_region
, E_aml_Z_object_Z_type_S_power_res
, E_aml_Z_object_Z_type_S_reserved_1
, E_aml_Z_object_Z_type_S_thermal_zone
, E_aml_Z_object_Z_type_S_buffer_field
, E_aml_Z_object_Z_type_S_reserved_2
, E_aml_Z_object_Z_type_S_debug
, E_aml_Z_object_Z_type_S_alias
//TODO Poniższe typy nie są obiektami?
, E_aml_Z_object_Z_type_S_processor
, E_aml_Z_object_Z_type_S_bank_field
, E_aml_Z_object_Z_type_S_data_table_region
, E_aml_Z_object_Z_type_S_index_field
};
_internal
struct
{ struct E_aml_Z_pathname name; // Tablica posortowana według nazwy.
  P data;
  enum E_aml_S_object_Z_type type;
  N8 n; // Jeśli “type” równe “E_aml_Z_value_Z_type_S_string”, “E_aml_Z_value_Z_type_S_buffer” lub “E_aml_Z_value_Z_type_S_package”, to jest indeksem elementu obiektu; “~0” oznacza brak referencji do elementu.
} *E_aml_S_object;
_internal N E_aml_S_object_n;
//------------------------------------------------------------------------------
enum E_aml_Z_parse_stack_Z_entity
{ E_aml_Z_parse_stack_Z_entity_S_result_to_n
, E_aml_Z_parse_stack_Z_entity_S_restore_current_path
, E_aml_Z_parse_stack_Z_entity_S_term
, E_aml_Z_parse_stack_Z_entity_S_term_finish_1
, E_aml_Z_parse_stack_Z_entity_S_term_finish_2
, E_aml_Z_parse_stack_Z_entity_S_term_arg
, E_aml_Z_parse_stack_Z_entity_S_term_arg_finish_1
, E_aml_Z_parse_stack_Z_entity_S_object
, E_aml_Z_parse_stack_Z_entity_S_expression
, E_aml_Z_parse_stack_Z_entity_S_data_object
, E_aml_Z_parse_stack_Z_entity_S_package
, E_aml_Z_parse_stack_Z_entity_S_package_finish
, E_aml_Z_parse_stack_Z_entity_S_supername
, E_aml_Z_parse_stack_Z_entity_S_bank_field_finish
, E_aml_Z_parse_stack_Z_entity_S_bit_field_finish_1
, E_aml_Z_parse_stack_Z_entity_S_bit_field_finish_2
, E_aml_Z_parse_stack_Z_entity_S_byte_field_finish_1
, E_aml_Z_parse_stack_Z_entity_S_byte_field_finish_2
, E_aml_Z_parse_stack_Z_entity_S_dword_field_finish_1
, E_aml_Z_parse_stack_Z_entity_S_dword_field_finish_2
, E_aml_Z_parse_stack_Z_entity_S_field_finish_1
, E_aml_Z_parse_stack_Z_entity_S_field_finish_2
, E_aml_Z_parse_stack_Z_entity_S_field_finish_3
, E_aml_Z_parse_stack_Z_entity_S_qword_field_finish_1
, E_aml_Z_parse_stack_Z_entity_S_qword_field_finish_2
, E_aml_Z_parse_stack_Z_entity_S_word_field_finish_1
, E_aml_Z_parse_stack_Z_entity_S_word_field_finish_2
, E_aml_Z_parse_stack_Z_entity_S_data_region_finish_1
, E_aml_Z_parse_stack_Z_entity_S_data_region_finish_2
, E_aml_Z_parse_stack_Z_entity_S_data_region_finish_3
, E_aml_Z_parse_stack_Z_entity_S_op_region_finish_1
, E_aml_Z_parse_stack_Z_entity_S_op_region_finish_2
, E_aml_Z_parse_stack_Z_entity_S_power_res_finish
, E_aml_Z_parse_stack_Z_entity_S_thermal_zone_finish
, E_aml_Z_parse_stack_Z_entity_S_if_op_finish_1
, E_aml_Z_parse_stack_Z_entity_S_if_op_finish_2
, E_aml_Z_parse_stack_Z_entity_S_else_op_finish
, E_aml_Z_parse_stack_Z_entity_S_release_finish
, E_aml_Z_parse_stack_Z_entity_S_reset_finish
, E_aml_Z_parse_stack_Z_entity_S_signal_finish
, E_aml_Z_parse_stack_Z_entity_S_sleep_finish
, E_aml_Z_parse_stack_Z_entity_S_stall_finish
, E_aml_Z_parse_stack_Z_entity_S_notify_finish_1
, E_aml_Z_parse_stack_Z_entity_S_notify_finish_2
, E_aml_Z_parse_stack_Z_entity_S_return_finish
, E_aml_Z_parse_stack_Z_entity_S_while_op_finish_1
, E_aml_Z_parse_stack_Z_entity_S_while_op_finish_2
, E_aml_Z_parse_stack_Z_entity_S_acquire_finish
, E_aml_Z_parse_stack_Z_entity_S_add_op_finish_1
, E_aml_Z_parse_stack_Z_entity_S_add_op_finish_2
, E_aml_Z_parse_stack_Z_entity_S_add_op_finish_3
, E_aml_Z_parse_stack_Z_entity_S_buffer_finish
, E_aml_Z_parse_stack_Z_entity_S_and_op_finish_1
, E_aml_Z_parse_stack_Z_entity_S_and_op_finish_2
, E_aml_Z_parse_stack_Z_entity_S_and_op_finish_3
, E_aml_Z_parse_stack_Z_entity_S_cond_refof_finish_1
, E_aml_Z_parse_stack_Z_entity_S_cond_refof_finish_2
, E_aml_Z_parse_stack_Z_entity_S_from_bcd_finish_1
, E_aml_Z_parse_stack_Z_entity_S_from_bcd_finish_2
, E_aml_Z_parse_stack_Z_entity_S_load_table_finish_1
, E_aml_Z_parse_stack_Z_entity_S_load_table_finish_2
, E_aml_Z_parse_stack_Z_entity_S_load_table_finish_3
, E_aml_Z_parse_stack_Z_entity_S_load_table_finish_4
, E_aml_Z_parse_stack_Z_entity_S_load_table_finish_5
, E_aml_Z_parse_stack_Z_entity_S_load_table_finish_6
, E_aml_Z_parse_stack_Z_entity_S_concat_finish_1
, E_aml_Z_parse_stack_Z_entity_S_concat_finish_2
, E_aml_Z_parse_stack_Z_entity_S_concat_finish_3
, E_aml_Z_parse_stack_Z_entity_S_concat_res_finish_1
, E_aml_Z_parse_stack_Z_entity_S_concat_res_finish_2
, E_aml_Z_parse_stack_Z_entity_S_concat_res_finish_3
, E_aml_Z_parse_stack_Z_entity_S_copy_object_finish_1
, E_aml_Z_parse_stack_Z_entity_S_copy_object_finish_2
, E_aml_Z_parse_stack_Z_entity_S_decrement_finish
, E_aml_Z_parse_stack_Z_entity_S_derefof_finish
, E_aml_Z_parse_stack_Z_entity_S_divide_finish_1
, E_aml_Z_parse_stack_Z_entity_S_divide_finish_2
, E_aml_Z_parse_stack_Z_entity_S_divide_finish_3
, E_aml_Z_parse_stack_Z_entity_S_divide_finish_4
, E_aml_Z_parse_stack_Z_entity_S_fslb_finish_1
, E_aml_Z_parse_stack_Z_entity_S_fslb_finish_2
, E_aml_Z_parse_stack_Z_entity_S_fsrb_finish_1
, E_aml_Z_parse_stack_Z_entity_S_fsrb_finish_2
, E_aml_Z_parse_stack_Z_entity_S_increment_finish
, E_aml_Z_parse_stack_Z_entity_S_index_finish_1
, E_aml_Z_parse_stack_Z_entity_S_index_finish_2
, E_aml_Z_parse_stack_Z_entity_S_index_finish_3
, E_aml_Z_parse_stack_Z_entity_S_land_finish_1
, E_aml_Z_parse_stack_Z_entity_S_land_finish_2
, E_aml_Z_parse_stack_Z_entity_S_lequal_finish_1
, E_aml_Z_parse_stack_Z_entity_S_lequal_finish_2
, E_aml_Z_parse_stack_Z_entity_S_lgreater_finish_1
, E_aml_Z_parse_stack_Z_entity_S_lgreater_finish_2
, E_aml_Z_parse_stack_Z_entity_S_lnot_finish
, E_aml_Z_parse_stack_Z_entity_S_lless_finish_1
, E_aml_Z_parse_stack_Z_entity_S_lless_finish_2
, E_aml_Z_parse_stack_Z_entity_S_mid_finish_1
, E_aml_Z_parse_stack_Z_entity_S_mid_finish_2
, E_aml_Z_parse_stack_Z_entity_S_mid_finish_3
, E_aml_Z_parse_stack_Z_entity_S_mid_finish_4
, E_aml_Z_parse_stack_Z_entity_S_lor_finish_1
, E_aml_Z_parse_stack_Z_entity_S_lor_finish_2
, E_aml_Z_parse_stack_Z_entity_S_match_finish_1
, E_aml_Z_parse_stack_Z_entity_S_match_finish_2
, E_aml_Z_parse_stack_Z_entity_S_match_finish_3
, E_aml_Z_parse_stack_Z_entity_S_match_finish_4
, E_aml_Z_parse_stack_Z_entity_S_to_bcd_finish_1
, E_aml_Z_parse_stack_Z_entity_S_to_bcd_finish_2
, E_aml_Z_parse_stack_Z_entity_S_wait_finish_1
, E_aml_Z_parse_stack_Z_entity_S_wait_finish_2
, E_aml_Z_parse_stack_Z_entity_S_mod_finish_1
, E_aml_Z_parse_stack_Z_entity_S_mod_finish_2
, E_aml_Z_parse_stack_Z_entity_S_mod_finish_3
, E_aml_Z_parse_stack_Z_entity_S_multiply_finish_1
, E_aml_Z_parse_stack_Z_entity_S_multiply_finish_2
, E_aml_Z_parse_stack_Z_entity_S_multiply_finish_3
, E_aml_Z_parse_stack_Z_entity_S_nand_finish_1
, E_aml_Z_parse_stack_Z_entity_S_nand_finish_2
, E_aml_Z_parse_stack_Z_entity_S_nand_finish_3
, E_aml_Z_parse_stack_Z_entity_S_nor_finish_1
, E_aml_Z_parse_stack_Z_entity_S_nor_finish_2
, E_aml_Z_parse_stack_Z_entity_S_nor_finish_3
, E_aml_Z_parse_stack_Z_entity_S_not_finish_1
, E_aml_Z_parse_stack_Z_entity_S_not_finish_2
, E_aml_Z_parse_stack_Z_entity_S_object_type_finish
, E_aml_Z_parse_stack_Z_entity_S_or_finish_1
, E_aml_Z_parse_stack_Z_entity_S_or_finish_2
, E_aml_Z_parse_stack_Z_entity_S_or_finish_3
, E_aml_Z_parse_stack_Z_entity_S_refof_finish
, E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_1
, E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_2
, E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_3
, E_aml_Z_parse_stack_Z_entity_S_shift_right_finish_1
, E_aml_Z_parse_stack_Z_entity_S_shift_right_finish_2
, E_aml_Z_parse_stack_Z_entity_S_shift_right_finish_3
, E_aml_Z_parse_stack_Z_entity_S_sizeof_finish
, E_aml_Z_parse_stack_Z_entity_S_store_finish_1
, E_aml_Z_parse_stack_Z_entity_S_store_finish_2
, E_aml_Z_parse_stack_Z_entity_S_subtract_finish_1
, E_aml_Z_parse_stack_Z_entity_S_subtract_finish_2
, E_aml_Z_parse_stack_Z_entity_S_subtract_finish_3
, E_aml_Z_parse_stack_Z_entity_S_to_buffer_finish_1
, E_aml_Z_parse_stack_Z_entity_S_to_buffer_finish_2
, E_aml_Z_parse_stack_Z_entity_S_to_decimal_string_finish_1
, E_aml_Z_parse_stack_Z_entity_S_to_decimal_string_finish_2
, E_aml_Z_parse_stack_Z_entity_S_to_hex_string_finish_1
, E_aml_Z_parse_stack_Z_entity_S_to_hex_string_finish_2
, E_aml_Z_parse_stack_Z_entity_S_to_integer_finish_1
, E_aml_Z_parse_stack_Z_entity_S_to_integer_finish_2
, E_aml_Z_parse_stack_Z_entity_S_to_string_finish_1
, E_aml_Z_parse_stack_Z_entity_S_to_string_finish_2
, E_aml_Z_parse_stack_Z_entity_S_to_string_finish_3
, E_aml_Z_parse_stack_Z_entity_S_xor_finish_1
, E_aml_Z_parse_stack_Z_entity_S_xor_finish_2
, E_aml_Z_parse_stack_Z_entity_S_xor_finish_3
, E_aml_Z_parse_stack_Z_entity_S_fatal_finish
, E_aml_Z_parse_stack_Z_entity_S_load_finish
, E_aml_Z_parse_stack_Z_entity_S_device_finish
, E_aml_Z_parse_stack_Z_entity_S_processor_finish
, E_aml_Z_parse_stack_Z_entity_S_procedure_finish
, E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish
};
_internal
struct
{ Pc data_end;
  N n;
  struct
  { struct E_aml_Z_value result;
    Pc continue_, break_;
    union
    { N tmp_n;
      P tmp_p;
    };
    N8 last_op_if;
    B procedure_osi;
  }execution_context;
  enum E_aml_Z_parse_stack_Z_entity entity;
  B match;
} *E_aml_S_parse_stack;
_internal N E_aml_S_parse_stack_n;
_internal const N E_aml_S_parse_stack_S_tmp_p_n = 5;
_internal Pc E_aml_S_parse_data;
//==============================================================================
_internal
void
E_aml_Q_value_W_package( struct E_aml_Z_package *package
){  for_n( i, package->n )
        switch( package->value[i].type )
        { case E_aml_Z_value_Z_type_S_string:
                W( package->value[i].p );
                break;
          case E_aml_Z_value_Z_type_S_buffer:
                W( package->value[i].buffer.p );
                break;
          case E_aml_Z_value_Z_type_S_package:
                E_aml_Q_value_W_package( package->value[i].package );
                break;
          case E_aml_Z_value_Z_type_S_pathname:
                W( package->value[i].pathname.s );
                break;
        }
    W(package);
}
_internal
void
E_aml_Q_value_W( struct E_aml_Z_value *value
){  switch( value->type )
    { case E_aml_Z_value_Z_type_S_string:
            W( value->p );
            break;
      case E_aml_Z_value_Z_type_S_buffer:
            W( value->buffer.p );
            break;
      case E_aml_Z_value_Z_type_S_package:
            E_aml_Q_value_W_package( value->package );
            break;
      case E_aml_Z_value_Z_type_S_pathname:
            W( value->pathname.s );
            break;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
N
E_aml_Q_value_R_l_package( struct E_aml_Z_package *package
){  N l = 0;
    for_n( i, package->n )
        switch( package->value[i].type )
        { case E_aml_Z_value_Z_type_S_string:
                l += E_text_Z_s0_R_l( package->value[i].p );
                break;
          case E_aml_Z_value_Z_type_S_buffer:
                l += package->value[i].buffer.n;
                break;
          case E_aml_Z_value_Z_type_S_package:
                l += E_aml_Q_value_R_l_package( package->value[i].package );
                break;
        }
    return l;
}
_internal
S
E_aml_Q_value_N_convert( enum E_aml_Z_value_Z_type type
, struct E_aml_Z_value *value
){  if( value->type == type )
        return 0;
    switch( value->type )
    { case E_aml_Z_value_Z_type_S_number:
            switch(type)
            { case E_aml_Z_value_Z_type_S_string:
                {   N l = E_text_Z_n_N_s_G( value->n, sizeof( value->n ), 10 );
                    Pc s = M( l + 1 );
                    if( !s )
                        return ~0;
                    s[l] = '\0';
                    E_text_Z_n_N_s( s + l, value->n, sizeof( value->n ), 10 );
                    value->p = s;
                    value->i = ~0;
                    value->type = type;
                    value->copy = yes;
                    return 0;
                }
              case E_aml_Z_value_Z_type_S_buffer:
                {   N l = sizeof( value->n );
                    P p = M(l);
                    if( !p )
                        return ~0;
                    *( N * )p = value->n;
                    value->buffer.p = p;
                    value->buffer.n = l;
                    value->i = ~0;
                    value->type = type;
                    value->copy = yes;
                    return 0;
                }
            }
            break;
      case E_aml_Z_value_Z_type_S_string:
            switch(type)
            { case E_aml_Z_value_Z_type_S_number:
                {   Pc s;
                    N n = E_text_Z_s0_N_n( value->p, &s, 10 );
                    if( *s )
                    {   if( s == value->p + 1
                        && *( s - 1 ) == '0'
                        && ( *s == 'x'
                          || *s == 'X'
                        ))
                        {   n = E_text_Z_s0_N_n( s + 1, &s, 16 );
                            if( *s )
                                return ~0 - 1;
                        }else
                            return ~0 - 1;
                    }
                    if( value->copy )
                        W( value->p );
                    value->n = n;
                    value->type = type;
                    return 0;
                }
              case E_aml_Z_value_Z_type_S_buffer:
                {   P p = value->p;
                    N l = E_text_Z_s0_R_l_0( value->p );
                    value->buffer.p = p;
                    value->buffer.n = l;
                    value->type = type;
                    return 0;
                }
            }
            break;
      case E_aml_Z_value_Z_type_S_buffer:
            switch(type)
            { case E_aml_Z_value_Z_type_S_number:
                {   if( value->buffer.n < sizeof( value->n ))
                    {   N n = 0;
                        for_n( i, sizeof( value->n ))
                            n |= value->buffer.p[i];
                        value->n = n;
                    }else
                        value->n = *( N * )value->buffer.p;
                    if( value->copy )
                        W( value->buffer.p );
                    return 0;
                }
              case E_aml_Z_value_Z_type_S_string:
                {   Pc p = value->buffer.p;
                    N l = value->buffer.n;
                    Pc s = E_text_Z_sl_R_search_0( p, l );
                    if( s != p + l )
                    {   if( value->copy )
                        {   if( !E_mem_Q_blk_I_remove( &p, s + 1 - p, l - ( s + 1 - p )))
                                return ~0;
                        }else
                        {   p = E_text_Z_sl_M_duplicate( p, s + 1 - p );
                            if( !p )
                                return ~0;
                            value->copy = yes;
                        }
                    }else
                    {   if( value->copy )
                        {   if( !E_mem_Q_blk_I_append( &p, 1 ))
                                return ~0;
                        }else
                        {   Pc p_ = M( l + 1 );
                            if( !p_ )
                                return ~0;
                            E_mem_Q_blk_I_copy( p_, p, l );
                            value->copy = yes;
                        }
                        p[l] = '\0';
                    }
                    value->p = p;
                    value->type = type;
                    return 0;
                }
            }
            break;
    }
    return ~0 - 1;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
B
E_aml_Q_value_T_cmp_eq( struct E_aml_Z_value *v_1
, struct E_aml_Z_value *v_2
){  B ret;
    switch( v_1->type )
    { case E_aml_Z_value_Z_type_S_number:
            ret = v_1->n == v_2->n;
            break;
      case E_aml_Z_value_Z_type_S_string:
            ret = !E_text_Z_s0_I_cmp_s0( v_1->p, v_2->p );
            break;
      case E_aml_Z_value_Z_type_S_buffer:
            ret = v_1->buffer.n == v_2->buffer.n && !E_text_Z_sl_T_cmp( v_1->buffer.p, v_2->buffer.p, v_1->buffer.n );
            break;
    }
    return ret;
}
_internal
B
E_aml_Q_value_T_cmp_gt( struct E_aml_Z_value *v_1
, struct E_aml_Z_value *v_2
){  B ret;
    switch( v_1->type )
    { case E_aml_Z_value_Z_type_S_number:
            ret = v_1->n > v_2->n;
            break;
      case E_aml_Z_value_Z_type_S_string:
            ret = E_text_Z_s0_I_cmp_s0( v_1->p, v_2->p ) > 0;
            break;
      case E_aml_Z_value_Z_type_S_buffer:
            if( v_1->buffer.n != v_2->buffer.n )
                ret = v_1->buffer.n > v_2->buffer.n;
            else
                ret = E_text_Z_sl_T_cmp( v_1->buffer.p, v_2->buffer.p, v_1->buffer.n ) < 0;
            break;
    }
    return ret;
}
_internal
B
E_aml_Q_value_T_cmp_lt( struct E_aml_Z_value *v_1
, struct E_aml_Z_value *v_2
){  B ret;
    switch( v_1->type )
    { case E_aml_Z_value_Z_type_S_number:
            ret = v_1->n < v_2->n;
            break;
      case E_aml_Z_value_Z_type_S_string:
            ret = E_text_Z_s0_I_cmp_s0( v_1->p, v_2->p ) < 0;
            break;
      case E_aml_Z_value_Z_type_S_buffer:
            if( v_1->buffer.n != v_2->buffer.n )
                ret = v_1->buffer.n < v_2->buffer.n;
            else
                ret = E_text_Z_sl_T_cmp( v_1->buffer.p, v_2->buffer.p, v_1->buffer.n ) < 0;
            break;
    }
    return ret;
}
//==============================================================================
_internal
N
E_aml_Q_current_path_M( void
){  E_aml_S_current_path_n = 0;
    Mt_( E_aml_S_current_path, E_aml_S_current_path_n );
    if( !E_aml_S_current_path )
        return ~0;
    E_aml_Q_current_path_S_precompilation_i = 0;
    return 0;
}
_internal
void
E_aml_Q_current_path_W( void
){  for_n( i, E_aml_S_current_path_n )
        W( E_aml_S_current_path[i].s );
    W( E_aml_S_current_path );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
void
E_aml_Q_current_path_P_precompilation( B b
){  if(b)
    {   if( E_aml_Q_current_path_S_precompilation_i )
            return;
        E_aml_Q_current_path_S_precompilation_i = E_aml_S_current_path_n;
    }else if( E_aml_Q_current_path_S_precompilation_i == E_aml_S_current_path_n )
        E_aml_Q_current_path_S_precompilation_i = 0;
}
//------------------------------------------------------------------------------
_internal
N
E_aml_Q_current_path_I_push( struct E_aml_Z_pathname pathname
){  if( !E_mem_Q_blk_I_append( &E_aml_S_current_path, 1 ))
        return ~0;
    E_aml_S_current_path[ E_aml_S_current_path_n ] = pathname;
    E_aml_S_current_path_n++;
    //Pc name_ = M( pathname.n * 4 + 1 );
    //E_text_Z_s_P_copy_sl_0( name_, pathname.s, pathname.n * 4 );
    //E_font_I_print( "\n,push=" ); E_font_I_print( name_ );
    //W( name_ );
    return 0;
}
_internal
N
E_aml_Q_current_path_I_pop( void
){  W( E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].s );
    if( !E_mem_Q_blk_I_remove( &E_aml_S_current_path, --E_aml_S_current_path_n, 1 ))
        return ~0;
    //E_font_I_print( "\n,pop=" ); 
    //if( E_aml_S_current_path_n )
    //{   Pc name_ = M( E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n * 4 + 1 );
        //E_text_Z_s_P_copy_sl_0( name_, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].s, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n * 4 );
        //E_font_I_print( name_ );
        //W( name_ );
    //}else
        //E_font_I_print( "\\" ); 
    return 0;
}
//==============================================================================
_internal
N
E_aml_Q_object_M( void
){  E_aml_S_object_n = 2;
    Mt_( E_aml_S_object, E_aml_S_object_n );
    if( !E_aml_S_object )
        return ~0;
    Pc name = E_text_Z_sl_M_duplicate( "_GPE", 4 );
    if( !name )
    {   W( E_aml_S_object );
        return ~0;
    }
    E_aml_S_object[0].name.s = name;
    E_aml_S_object[0].name.n = 1;
    E_aml_S_object[0].type = E_aml_Z_object_Z_type_S_uninitialized;
    E_aml_S_object[0].data = 0;
    name = E_text_Z_sl_M_duplicate( "_SB_", 4 );
    if( !name )
    {   W( E_aml_S_object[0].name.s );
        W( E_aml_S_object );
        return ~0;
    }
    E_aml_S_object[1].name.s = name;
    E_aml_S_object[1].name.n = 1;
    E_aml_S_object[1].type = E_aml_Z_object_Z_type_S_uninitialized;
    E_aml_S_object[1].data = 0;
    return 0;
}
_internal
void
E_aml_Q_object_W_package( struct E_aml_Z_package *package
){  for_n( i, package->n )
        switch( package->value[i].type )
        { case E_aml_Z_value_Z_type_S_string:
                if( package->value[i].p )
                    W( package->value[i].p );
                break;
          case E_aml_Z_value_Z_type_S_buffer:
                W( package->value[i].buffer.p );
                break;
          case E_aml_Z_value_Z_type_S_package:
                E_aml_Q_object_W_package( package->value[i].package );
                break;
          case E_aml_Z_value_Z_type_S_pathname:
                W( package->value[i].pathname.s );
                break;
        }
    W(package);
}
_internal
void
E_aml_Q_object_W_data( N object_i
){  if( E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_number
    && E_aml_S_object[ object_i ].data
    )
    {   switch( E_aml_S_object[ object_i ].type )
        { case E_aml_Z_object_Z_type_S_buffer:
            {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                W( buffer->p );
                break;
            }
          case E_aml_Z_object_Z_type_S_alias:
            {   struct E_aml_Z_pathname *pathname = E_aml_S_object[ object_i ].data;
                W( pathname->s );
                break;
            }
          case E_aml_Z_object_Z_type_S_package:
                E_aml_Q_object_W_package( E_aml_S_object[ object_i ].data );
                break;
        }
        W( E_aml_S_object[ object_i ].data );
    }
}
_internal
void
E_aml_Q_object_W( void
){  for_n( i, E_aml_S_object_n )
    {   E_aml_Q_object_W_data(i);
        W( E_aml_S_object[i].name.s );
    }
    W( E_aml_S_object );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
N
E_aml_Q_object_R_l( N object_i
){  N l = 0;
    switch( E_aml_S_object[ object_i ].type )
    { case E_aml_Z_value_Z_type_S_string:
            l += E_text_Z_s0_R_l( E_aml_S_object[ object_i ].data );
            break;
      case E_aml_Z_value_Z_type_S_buffer:
        {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
            l += buffer->n;
            break;
        }
      case E_aml_Z_value_Z_type_S_package:
            l += E_aml_Q_value_R_l_package( E_aml_S_object[ object_i ].data );
            break;
    }
    return l;
}
_internal
struct E_aml_Z_package *
E_aml_Q_object_I_package_duplicate( struct E_aml_Z_package *package
){  struct E_aml_Z_package *M_(ret);
    if( !ret )
        return 0;
    Mt_( ret->value, package->n );
    if( !ret->value )
    {   W(ret);
        return 0;
    }
    for_n( i, package->n )
        switch( package->value[i].type )
        { case E_aml_Z_value_Z_type_S_number:
                ret->value[i].type = E_aml_Z_value_Z_type_S_number;
                ret->value[i].n = package->value[i].n;
                break;
          case E_aml_Z_value_Z_type_S_string:
            {   Pc s = E_text_Z_s0_M_duplicate( package->value[i].p );
                if( !s )
                {   W( ret->value );
                    W(ret);
                    return 0;
                }
                ret->value[i].type = E_aml_Z_value_Z_type_S_string;
                ret->value[i].p = s;
                break;
            }
          case E_aml_Z_value_Z_type_S_buffer:
            {   ret->value[i].buffer.n = package->value[i].buffer.n;
                ret->value[i].buffer.p = E_text_Z_sl_M_duplicate( package->value[i].p, ret->value[i].buffer.n );
                if( !ret->value[i].buffer.p )
                {   W( ret->value );
                    W(ret);
                    return 0;
                }
                ret->value[i].type = E_aml_Z_value_Z_type_S_buffer;
                break;
            }
          case E_aml_Z_value_Z_type_S_package:
            {   struct E_aml_Z_package *package_ = E_aml_Q_object_I_package_duplicate( package->value[i].package );
                if( !package_ )
                {   W( ret->value );
                    W(ret);
                    return 0;
                }
                ret->value[i].type = E_aml_Z_value_Z_type_S_package;
                ret->value[i].package = package_;
                break;
            }
          default:
                W( ret->value );
                W(ret);
                return 0;
        }
    return ret;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
N
E_aml_Q_object_I_add( enum E_aml_S_object_Z_type type
, struct E_aml_Z_pathname name
){  N middle;
    if( E_aml_S_object_n )
    {   N min = 0;
        N max = E_aml_S_object_n - 1;
        middle = max / 2;
        O{  S cmp = E_text_Z_sl_T_cmp( E_aml_S_object[middle].name.s, name.s, J_min( E_aml_S_object[middle].name.n, name.n ) * 4 );
            if( !cmp
            && E_aml_S_object[middle].name.n == name.n
            )
                return ~0 - 1;
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
    E_aml_S_object[middle].type = type;
    E_aml_S_object[middle].data = 0;
    return middle;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
N
E_aml_Q_object_R( struct E_aml_Z_pathname name
){  if( !E_aml_S_object_n )
        return ~0;
    N middle;
    do
    {   N min = 0;
        N max = E_aml_S_object_n - 1;
        middle = max / 2;
        O{  S cmp = E_text_Z_sl_T_cmp( E_aml_S_object[middle].name.s, name.s, J_min( E_aml_S_object[middle].name.n, name.n ) * 4 );
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
        if( E_aml_S_object[middle].type == E_aml_Z_object_Z_type_S_alias )
            name = *( struct E_aml_Z_pathname * )E_aml_S_object[middle].data;
    }while( E_aml_S_object[middle].type == E_aml_Z_object_Z_type_S_alias );
    return ~0;
}
_internal
N
E_aml_Q_object_R_relative( struct E_aml_Z_pathname start_scope
, Pc name
){  if( !E_aml_S_object_n )
        return ~0;
    for_n_rev( i, start_scope.n + 1 )
    {   N min = 0;
        N max = E_aml_S_object_n - 1;
        N middle = max / 2;
        O{  S cmp = E_text_Z_sl_T_cmp( E_aml_S_object[middle].name.s, start_scope.s, J_min( E_aml_S_object[middle].name.n, i ) * 4 );
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
N8
E_aml_Q_path_R_pathname( void
){  if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
        return ~0;
    N8 n;
    switch( (N8)*E_aml_S_parse_data++ )
    { case 0: // null
            return 0;
      case 0x2e: // dual
            n = 2;
            break;
      case 0x2f: // multi
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0;
            n = (N8)*E_aml_S_parse_data;
            if( !n )
                return ~0;
            E_aml_S_parse_data++;
            break;
      default:
            E_aml_S_parse_data--;
            n = 1;
            break;
    }
    if( E_aml_S_parse_data + n * 4 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
        return ~0;
    for_n( i, n )
    {   if( !( E_aml_S_parse_data[ i * 4 ] == '_'
        || ( E_aml_S_parse_data[ i * 4 ] >= 'A'
          && E_aml_S_parse_data[ i * 4 ] <= 'Z'
        )))
            return ~0;
        for_n( j, 3 )
            if( !( E_aml_S_parse_data[ i * 4 + 1 + j ] == '_'
            || ( E_aml_S_parse_data[ i * 4 + 1 + j ] >= '0'
              && E_aml_S_parse_data[ i * 4 + 1 + j ] <= '9'
            )
            || ( E_aml_S_parse_data[ i * 4 + 1 + j ] >= 'A'
              && E_aml_S_parse_data[ i * 4 + 1 + j ] <= 'Z'
            )))
                return ~0;
    }
    E_aml_S_parse_data += n * 4;
    return n;
}
_internal
Pc
E_aml_Q_path_R_root( N8 *n
){  if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
        return 0;
    N parent_n = 0;
    switch( *E_aml_S_parse_data )
    { case '\\':
        {   E_aml_S_parse_data++;
            *n = E_aml_Q_path_R_pathname();
            if( !~(S8)*n
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
            E_mem_Q_blk_I_copy( s, E_aml_S_parse_data - *n * 4, *n * 4 );
            return s;
        }
      case '^':
            do
            {   if( ++E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end
                || ++parent_n > E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n
                )
                {   *n = ~0 - 1;
                    return 0;
                }
            }while( *E_aml_S_parse_data == '^' );
      default:
        {   *n = E_aml_Q_path_R_pathname();
            if( !~(S8)*n )
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
                E_mem_Q_blk_I_copy( s + depth * 4, E_aml_S_parse_data - *n * 4, *n * 4 );
            else if( !depth )
            {   W(s);
                return 0;
            }
            *n += depth;
            return s;
        }
    }
}
// Obecnie wyszukiwanie obiektów nie zamienia aliasów na docelowe nazwy. Być może to nie ma znaczenia.
_internal
Pc
E_aml_Q_path_R_relative( N8 *n
, N *object_i
){  if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
        return 0;
    N parent_n = 0;
    switch( *E_aml_S_parse_data )
    { case '\\':
        {   E_aml_S_parse_data++;
            *n = E_aml_Q_path_R_pathname();
            if( !~(S8)*n
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
            E_mem_Q_blk_I_copy( s, E_aml_S_parse_data - *n * 4, *n * 4 );
            *object_i = E_aml_Q_object_R(( struct E_aml_Z_pathname ){ E_aml_S_parse_data - *n * 4, *n });
            return s;
        }
      case '^':
        {   E_aml_S_parse_data++;
            do
            {   if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end
                || ++parent_n > E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n
                )
                {   *n = ~0 - 1;
                    return 0;
                }
            }while( *E_aml_S_parse_data++ == '^' );
            *n = E_aml_Q_path_R_pathname();
            if( !~(S8)*n
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
            E_mem_Q_blk_I_copy( s + depth * 4, E_aml_S_parse_data - *n * 4, *n * 4 );
            *n += depth;
            *object_i = E_aml_Q_object_R(( struct E_aml_Z_pathname ){ s, *n });
            return s;
        }
      default:
        {   *n = E_aml_Q_path_R_pathname();
            if( !~(S8)*n
            || !*n
            )
            {   *n = ~0 - 1;
                return 0;
            }
            Pc s;
            if( E_aml_S_current_path_n )
            {   N i;
                if( *n == 1
                && ~( i = E_aml_Q_object_R_relative( E_aml_S_current_path[ E_aml_S_current_path_n - 1 ], E_aml_S_parse_data - 4 ))
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
                    {   *n = ~0;
                        return 0;
                    }
                    E_mem_Q_blk_I_copy( s, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].s, depth * 4 );
                    E_mem_Q_blk_I_copy( s + depth * 4, E_aml_S_parse_data - *n * 4, *n * 4 );
                    *n += depth;
                    *object_i = E_aml_Q_object_R(( struct E_aml_Z_pathname ){ s, *n });
                }
            }else
            {   s = M( *n * 4 );
                if( !s )
                {   *n = ~0;
                    return 0;
                }
                E_mem_Q_blk_I_copy( s, E_aml_S_parse_data - *n * 4, *n * 4 );
                *object_i = E_aml_Q_object_R(( struct E_aml_Z_pathname ){ E_aml_S_parse_data - *n * 4, *n });
            }
            return s;
        }
    }
}
_internal
N
E_aml_R_pkg_length( void
){  if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
        return ~0;
    N n = (N8)*E_aml_S_parse_data >> 6;
    if( !n )
    {   //E_font_I_print( "\npkg_length=" ); E_font_I_print_hex( *E_aml_S_parse_data & 0x3f );
        return *E_aml_S_parse_data++ & 0x3f;
    }
    if( *E_aml_S_parse_data & 0x30 )
        return ~0;
    N l = *E_aml_S_parse_data & 0xf;
    N i = 0;
    while( ++E_aml_S_parse_data != E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end
    && i != n
    )
    {   l |= (N)(N8)*E_aml_S_parse_data << ( 4 + i * 8 );
        i++;
    }
    //E_font_I_print( "\npkg_length=" ); E_font_I_print_hex(l);
    return l;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define E_aml_I_delegate( finish, entity_ ) \
{  if( !E_mem_Q_blk_I_append( &E_aml_S_parse_stack, 2 )) \
        return ~0; \
    Pc data_end = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = finish; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0; \
    E_aml_S_parse_stack_n++; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = entity_; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 1; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = data_end; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0; \
    E_aml_S_parse_stack_n++; \
}
#define E_aml_I_delegate_pkg(  finish, entity_ ) \
{   if( !E_mem_Q_blk_I_append( &E_aml_S_parse_stack, 2 )) \
        return ~0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = finish; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0; \
    E_aml_S_parse_stack_n++; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = entity_; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 1; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = pkg_end; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0; \
    E_aml_S_parse_stack_n++; \
}
_internal
N
E_aml_I_data_object( void
){  if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
        return ~0 - 1;
    switch( (N8)*E_aml_S_parse_data++ )
    { case 0:
            E_font_I_print( ",0" );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = 0;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 1:
            E_font_I_print( ",1" );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = 1;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 0xff:
            E_font_I_print( ",~0" );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = ~0;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 0xa: // byte const
            if( E_aml_S_parse_data + 1 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",byte" );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = (N8)*E_aml_S_parse_data;
            E_aml_S_parse_data++;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 0xb: // word const
            if( E_aml_S_parse_data + 2 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",word" );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = *( N16 * )E_aml_S_parse_data;
            E_aml_S_parse_data += 2;
            //TODO Zapisać ‘word’.
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 0xc: // dword const
            if( E_aml_S_parse_data + 4 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",dword" );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = *( N32 * )E_aml_S_parse_data;
            E_aml_S_parse_data += 4;
            //TODO Zapisać ‘dword’.
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 0xd: // string const
            E_font_I_print( ",string=" );
            Pc s = E_aml_S_parse_data;
            while( E_aml_S_parse_data != E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end
            && *E_aml_S_parse_data
            )
                E_aml_S_parse_data++;
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_aml_S_parse_data++;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_string;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p = s;
            E_font_I_print(s);
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 0xe: // qword const
            if( E_aml_S_parse_data + 8 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",qword" );
            E_aml_S_parse_data += 8;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = *( N * )E_aml_S_parse_data;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 0x5b:
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            switch( (N8)*E_aml_S_parse_data++ )
            { case 0x30: // revision
                    if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    E_font_I_print( ",revision" );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = 0x30;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    break;
              default:
                    E_aml_S_parse_data -= 2;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = no;
                    return 0;
            }
            break;
      case 0x11: // buffer
        {   Pc data_start = E_aml_S_parse_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",buffer" );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_buffer_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
        }
      case 0x12: // package
        {   Pc data_start = E_aml_S_parse_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            if( E_aml_S_parse_data == pkg_end )
                return ~0 - 1;
            N n = (N8)*E_aml_S_parse_data;
            E_aml_S_parse_data++;
            E_font_I_print( ",package," ); E_font_I_print_hex(n);
            if( E_aml_Q_current_path_S_precompilation_i )
            {
            }else
            {   struct E_aml_Z_package *M_(package);
                if( !package )
                    return ~0;
                Mt_( package->value, 0 );
                if( !package->value )
                {   W(package);
                    return ~0;
                }
                package->n = 0;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package = package;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_package;
            }
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_package_finish, E_aml_Z_parse_stack_Z_entity_S_package );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = n;
            break;
        }
      case 0x13: // var package
        {   Pc data_start = E_aml_S_parse_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",var package" );
            if( E_aml_Q_current_path_S_precompilation_i )
            {
            }else
            {   struct E_aml_Z_package *M_(package);
                if( !package )
                    return ~0;
                Mt_( package->value, 0 );
                if( !package->value )
                {   W(package);
                    return ~0;
                }
                package->n = 0;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package = package;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_package;
            }
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            if( !E_mem_Q_blk_I_append( &E_aml_S_parse_stack, 4 ))
                return ~0;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = E_aml_Z_parse_stack_Z_entity_S_package_finish;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 0;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0;
            E_aml_S_parse_stack_n++;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = E_aml_Z_parse_stack_Z_entity_S_package;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 0;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0;
            E_aml_S_parse_stack_n++;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = E_aml_Z_parse_stack_Z_entity_S_result_to_n;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 0;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0;
            E_aml_S_parse_stack_n++;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = E_aml_Z_parse_stack_Z_entity_S_term_arg;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 1;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = pkg_end;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0;
            E_aml_S_parse_stack_n++;
            break;
        }
      default:
            E_aml_S_parse_data--;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = no;
            return 0;
    }
    return 0;
}
#undef E_aml_I_delegate_pkg
#undef E_aml_I_delegate
//------------------------------------------------------------------------------
#define E_aml_I_delegate( finish, entity_ ) \
{  if( !E_mem_Q_blk_I_append( &E_aml_S_parse_stack, 2 )) \
        return ~0; \
    Pc data_end = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = finish; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.break_ = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0; \
    E_aml_S_parse_stack_n++; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = entity_; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 1; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = data_end; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.break_ = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0; \
    E_aml_S_parse_stack_n++; \
}
#define E_aml_I_delegate_pkg(  finish, entity_ ) \
{   if( !E_mem_Q_blk_I_append( &E_aml_S_parse_stack, 2 )) \
        return ~0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = finish; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.break_ = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0; \
    E_aml_S_parse_stack_n++; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = entity_; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 1; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = pkg_end; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.break_ = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0; \
    E_aml_S_parse_stack_n++; \
}
_internal
N
E_aml_I_term( void
){  if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
        return ~0 - 1;
    switch( (N8)*E_aml_S_parse_data++ )
    { case 0x5b: // ext
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            switch( (N8)*E_aml_S_parse_data++ )
            { case 0x32: // fatal
                    E_font_I_print( ",fatal" );
                    if( ++E_aml_S_parse_data > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    if( E_aml_S_parse_data + 4 == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    E_aml_S_parse_data += 4;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_fatal_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
              case 0x27: // release
                    E_font_I_print( ",release" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_release_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
              case 0x26: // reset
                    E_font_I_print( ",reset" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_reset_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
              case 0x24: // signal
                    E_font_I_print( ",signal" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_signal_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
              case 0x22: // sleep
                    E_font_I_print( ",sleep" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_sleep_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
              case 0x21: // stall
                    E_font_I_print( ",stall" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_stall_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
              case 0x20: // load
                    E_font_I_print( ",load" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_load_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
              case 0x2a: // unload
                    E_font_I_print( ",unload" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_load_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
              default:
                {   E_aml_S_parse_data -= 2;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_term_finish_1, E_aml_Z_parse_stack_Z_entity_S_object );
                    break;
                }
            }
            break;
      case 0x15: // external
        {   N8 n;
            Pc name = E_aml_Q_path_R_root( &n );
            if( !name )
                return (S8)n;
            W(name);
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( ",external=" ); E_font_I_print( name_ );
            W( name_ );
            if( E_aml_S_parse_data + 2 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_aml_S_parse_data += 2;
            break;
        }
      case 0xa5: // break
        {   E_font_I_print( ",break" );
            for_n_rev( i, E_aml_S_parse_stack_n )
                if( E_aml_S_parse_stack[i].execution_context.break_ )
                {   E_aml_S_parse_data = E_aml_S_parse_stack[i].execution_context.break_;
                    break;
                }
            if( !~i )
                return ~0 - 1;
            break;
        }
      case 0xcc: // breakpoint
        {   E_font_I_print( ",breakpoint" );
            for_n_rev( i, E_aml_S_parse_stack_n )
                if( E_aml_S_parse_stack[i].execution_context.break_ )
                {   E_aml_S_parse_data = E_aml_S_parse_stack[i].execution_context.break_;
                    break;
                }
            if( !~i )
                return ~0 - 1;
            break;
        }
      case 0x9f: // continue
        {   E_font_I_print( ",continue" );
            for_n_rev( i, E_aml_S_parse_stack_n )
                if( E_aml_S_parse_stack[i].execution_context.continue_ )
                {   E_aml_S_parse_data = E_aml_S_parse_stack[i].execution_context.continue_;
                    break;
                }
            if( !~i )
                return ~0 - 1;
            break;
        }
      case 0xa0: // if
        {   Pc data_start = E_aml_S_parse_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",if" );
            if( E_aml_S_parse_data != pkg_end )
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.last_op_if = 2;
                E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_if_op_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            }
            break;
        }
      case 0xa1: // else
        {   if( !E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.last_op_if )
                return ~0 - 1;
            Pc data_start = E_aml_S_parse_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",else" );
            if( E_aml_S_parse_data != pkg_end )
            {   if( !E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n )
                {   E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_else_op_finish, E_aml_Z_parse_stack_Z_entity_S_term );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                }else
                    E_aml_S_parse_data = pkg_end;
            }
        }
      case 0xa3: // noop
            break;
      case 0x86: // notify
            E_font_I_print( ",notify" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_notify_finish_1, E_aml_Z_parse_stack_Z_entity_S_supername );
            break;
      case 0xa4: // return
            E_font_I_print( ",return" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_return_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0xa2: // while
        {   Pc data_start = E_aml_S_parse_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",while" );
            if( E_aml_S_parse_data != pkg_end )
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.continue_ = E_aml_S_parse_data;
                E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_while_op_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            }
            break;
        }
      default:
            E_aml_S_parse_data--;
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_term_finish_1, E_aml_Z_parse_stack_Z_entity_S_object );
            break;
    }
    return 0;
}
_internal
N
E_aml_I_object( void
){  if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
        return ~0 - 1;
    switch( (N8)*E_aml_S_parse_data++ )
    { case 6: // alias
        {   N8 name_n;
            Pc name = E_aml_Q_path_R_root( &name_n );
            if( !name )
                return (S8)name_n;
            if( !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, name_n }))
            {   W(name);
                return ~0 - 1;
            }
            Pc name_ = M( name_n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, name_n * 4 );
            E_font_I_print( ",alias=" ); E_font_I_print( name_ );
            W( name_ );
            N8 alias_n;
            Pc alias = E_aml_Q_path_R_root( &alias_n );
            if( !alias )
            {   W(name);
                return (S8)alias_n;
            }
            name_ = M( alias_n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, alias, alias_n * 4 );
            E_font_I_print( ":" ); E_font_I_print( name_ );
            W( name_ );
            N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_alias, ( struct E_aml_Z_pathname ){ alias, alias_n });
            if( (S)object_i < 0 )
            {   W(alias);
                W(name);
                return object_i;
            }
            struct E_aml_Z_pathname *M_(pathname);
            if( !pathname )
                return ~0;
            pathname->s = name;
            pathname->n = name_n;
            E_aml_S_object[ object_i ].data = pathname;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
        }
      case 8: // object
        {   N8 n;
            Pc name = E_aml_Q_path_R_root( &n );
            if( !name )
                return (S8)n;
            if( ~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n })
            || ( n > 1
              && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
            ))
            {   W(name);
                return ~0 - 1;
            }
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( "\n,object=" ); E_font_I_print( name_ );
            W( name_ );
            N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_uninitialized, ( struct E_aml_Z_pathname ){ name, n });
            if( (S)object_i < 0 )
            {   W(name);
                return object_i;
            }
            name_ = E_text_Z_sl_M_duplicate( name, n * 4 );
            if( !name_ )
                return ~0;
            if( !~E_aml_Q_current_path_I_push(( struct E_aml_Z_pathname ){ name_, n }))
            {   W( name_ );
                return ~0;
            }
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_restore_current_path, E_aml_Z_parse_stack_Z_entity_S_data_object );
            break;
        }
      case 0x10: // scope
        {   Pc data_start = E_aml_S_parse_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            N8 n;
            Pc name = E_aml_Q_path_R_root( &n );
            if( !name )
                return (S8)n;
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( "\n,scope=" ); E_font_I_print( name_ );
            W( name_ );
            N object_i = E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n });
            if( !~object_i
            || !( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_uninitialized
              || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_device
              || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_power_res
              || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_thermal_zone
              || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_processor
            ))
            {   W(name);
                return ~0 - 1;
            }
            //TODO Dodać ‘scope’ do drzewa zinterpretowanej przestrzeni ACPI.
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            if( E_aml_S_parse_data != pkg_end )
            {   if( !~E_aml_Q_current_path_I_push(( struct E_aml_Z_pathname ){ name, n }))
                    return ~0;
                E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_restore_current_path, E_aml_Z_parse_stack_Z_entity_S_term );
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
            }else
                W(name);
            break;
        }
      case 0x5b:
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            switch( (N8)*E_aml_S_parse_data++ )
            { case 0x87: // bank field
                {   Pc data_start = E_aml_S_parse_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N8 n;
                    Pc region_name = E_aml_Q_path_R_root( &n );
                    if( !region_name )
                        return (S8)n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, region_name, n * 4 );
                    E_font_I_print( ",region=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( ~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ region_name, n })
                    || ( n > 1
                      && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ region_name, n - 1 })
                    ))
                    {   W( region_name );
                        return ~0 - 1;
                    }
                    N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_bank_field, ( struct E_aml_Z_pathname ){ region_name, n });
                    if( (S)object_i < 0 )
                    {   W( region_name );
                        return object_i;
                    }
                    Pc bank_name = E_aml_Q_path_R_root( &n );
                    if( !bank_name )
                        return (S8)n;
                    name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, bank_name, n * 4 );
                    E_font_I_print( ",bank=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( ~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ bank_name, n })
                    || ( n > 1
                      && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ bank_name, n - 1 })
                    ))
                    {   W( bank_name );
                        return ~0 - 1;
                    }
                    W( bank_name );
                    //TODO Dodać ‘bank field’ do drzewa zinterpretowanej przestrzeni ACPI.
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_bank_field_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
                }
              case 1: // mutex
                {   N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( ~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n })
                    || ( n > 1
                      && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    ))
                    {   W(name);
                        return ~0 - 1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",mutex=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    E_aml_S_parse_data++;
                    N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_mutex, ( struct E_aml_Z_pathname ){ name, n });
                    if( (S)object_i < 0 )
                    {   W(name);
                        return object_i;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    break;
                }
              case 0x13: // field
                    E_font_I_print( ",field" );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_field_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
              case 0x88: // data table region
                {   N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( ~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n })
                    || ( n > 1
                      && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    ))
                    {   W(name);
                        return ~0 - 1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",data table region=" ); E_font_I_print( name_ );
                    W( name_ );
                    N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_data_table_region, ( struct E_aml_Z_pathname ){ name, n });
                    if( (S)object_i < 0 )
                    {   W(name);
                        return object_i;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_data_region_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
                }
             case 0x80: // op region
                {   N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( ~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n })
                    || ( n > 1
                      && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    ))
                    {   W(name);
                        return ~0 - 1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",op region=" ); E_font_I_print( name_ );
                    W( name_ );
                    N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_op_region, ( struct E_aml_Z_pathname ){ name, n });
                    if( (S)object_i < 0 )
                    {   W(name);
                        return object_i;
                    }
                    if( ++E_aml_S_parse_data > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_op_region_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
                }
              case 0x81: // field op
                {   Pc data_start = E_aml_S_parse_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",field op=" ); E_font_I_print( name_ );
                    W( name_ );
                    N object_i = E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n });
                    if( !~object_i
                    || E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_op_region
                    )
                    {   W(name);
                        return ~0 - 1;
                    }
                    if( ++E_aml_S_parse_data > pkg_end )
                        return ~0 - 1;
                    while( E_aml_S_parse_data != pkg_end )
                        switch( *E_aml_S_parse_data++ )
                        { case 0: // reserved field
                            {   if( E_aml_S_parse_data == pkg_end )
                                    return ~0 - 1;
                                E_font_I_print( ",reserved field" );
                                N n = E_aml_R_pkg_length();
                                if( !~n )
                                    return ~0 - 1;
                                //TODO Dodać ‘field pkg length’ do drzewa zinterpretowanej przestrzeni ACPI.
                                break;
                            }
                          case 1: // access field
                                if( E_aml_S_parse_data + 2 > pkg_end )
                                    return ~0 - 1;
                                E_font_I_print( ",access field" );
                                //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_parse_data++;
                                //TODO Dodać ‘access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_parse_data++;
                                break;
                          case 2: // connect field
                                if( E_aml_S_parse_data == pkg_end )
                                    return ~0 - 1;
                                E_font_I_print( ",connect field" );
                                return ~0; //NDFN
                                break;
                          case 3: // extended access field
                                if( E_aml_S_parse_data + 3 > pkg_end )
                                    return ~0 - 1;
                                E_font_I_print( ",ext access field" );
                                //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_parse_data++;
                                //TODO Dodać ‘extended access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_parse_data++;
                                //TODO Dodać ‘access length’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_parse_data++; //NDFN Jaki rozmiar.
                                break;
                          default: // named field
                            {   E_aml_S_parse_data--;
                                if( E_aml_S_parse_data + 4 > pkg_end )
                                    return ~0 - 1;
                                if( !( E_aml_S_parse_data[0] == '_'
                                || ( E_aml_S_parse_data[0] >= 'A'
                                  && E_aml_S_parse_data[0] <= 'Z'
                                )))
                                    return ~0 - 1;
                                for_n( j, 3 )
                                    if( !( E_aml_S_parse_data[ 1 + j ] == '_'
                                    || ( E_aml_S_parse_data[ 1 + j ] >= '0'
                                      && E_aml_S_parse_data[ 1 + j ] <= '9'
                                    )
                                    || ( E_aml_S_parse_data[ 1 + j ] >= 'A'
                                      && E_aml_S_parse_data[ 1 + j ] <= 'Z'
                                    )))
                                        return ~0 - 1;
                                Pc name_ = M( 4 + 1 );
                                E_text_Z_s_P_copy_sl_0( name_, E_aml_S_parse_data, 4 );
                                E_font_I_print( ",name=" ); E_font_I_print( name_ );
                                W( name_ );
                                //TODO Dodać ‘field name’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_parse_data += 4;
                                N n = E_aml_R_pkg_length();
                                if( !~n )
                                    return n;
                                //TODO Dodać ‘field pkg length’ do drzewa zinterpretowanej przestrzeni ACPI.
                                break;
                            }
                        }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    break;
                }
              case 0x86: // index field
                {   Pc data_start = E_aml_S_parse_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( ~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n })
                    || ( n > 1
                      && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    ))
                    {   W(name);
                        return ~0 - 1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",index field=" ); E_font_I_print( name_ );
                    W( name_ );
                    N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_index_field, ( struct E_aml_Z_pathname ){ name, n });
                    if( (S)object_i < 0 )
                    {   W(name);
                        return object_i;
                    }
                    name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",=" ); E_font_I_print( name_ );
                    W( name_ );
                    W(name);
                    if( ++E_aml_S_parse_data > pkg_end )
                        return ~0 - 1;
                    while( E_aml_S_parse_data != pkg_end )
                    {   switch( *E_aml_S_parse_data++ )
                        { case 0: // reserved field
                            {   if( E_aml_S_parse_data == pkg_end )
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
                                if( E_aml_S_parse_data + 2 > pkg_end )
                                    return ~0 - 1;
                                //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_parse_data++;
                                //TODO Dodać ‘access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_parse_data++;
                                break;
                          case 2: // connect field
                                if( E_aml_S_parse_data == pkg_end )
                                    return ~0 - 1;
                                E_font_I_print( ",connect field" );
                                return ~0; //NDFN
                                break;
                          case 3: // extended access field
                                E_font_I_print( ",ext access field" );
                                if( E_aml_S_parse_data + 3 > pkg_end )
                                    return ~0 - 1;
                                //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_parse_data++;
                                //TODO Dodać ‘extended access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_parse_data++;
                                //TODO Dodać ‘access length’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_parse_data++; //NDFN Jaki rozmiar.
                                break;
                          default: // named field
                            {   E_aml_S_parse_data--;
                                if( E_aml_S_parse_data + 4 > pkg_end )
                                    return ~0 - 1;
                                if( !( E_aml_S_parse_data[0] == '_'
                                || ( E_aml_S_parse_data[0] >= 'A'
                                  && E_aml_S_parse_data[0] <= 'Z'
                                )))
                                    return ~0 - 1;
                                for_n( j, 3 )
                                    if( !( E_aml_S_parse_data[ 1 + j ] == '_'
                                    || ( E_aml_S_parse_data[ 1 + j ] >= '0'
                                      && E_aml_S_parse_data[ 1 + j ] <= '9'
                                    )
                                    || ( E_aml_S_parse_data[ 1 + j ] >= 'A'
                                      && E_aml_S_parse_data[ 1 + j ] <= 'Z'
                                    )))
                                        return ~0 - 1;
                                Pc name_ = M( 4 + 1 );
                                E_text_Z_s_P_copy_sl_0( name_, E_aml_S_parse_data, 4 );
                                E_font_I_print( ",field=" ); E_font_I_print( name_ );
                                W( name_ );
                                //TODO Dodać ‘field name’ do drzewa zinterpretowanej przestrzeni ACPI.
                                E_aml_S_parse_data += 4;
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
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    break;
                }
              case 0x82: // device
                {   Pc data_start = E_aml_S_parse_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( ~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n })
                    || ( n > 1
                      && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    ))
                    {   W(name);
                        return ~0 - 1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",device=" ); E_font_I_print( name_ );
                    W( name_ );
                    N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_device, ( struct E_aml_Z_pathname ){ name, n });
                    if( (S)object_i < 0 )
                    {   W(name);
                        return object_i;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    if( E_aml_S_parse_data != pkg_end )
                    {   name_ = E_text_Z_sl_M_duplicate( name, n * 4 );
                        if( !name_ )
                            return ~0;
                        if( !~E_aml_Q_current_path_I_push(( struct E_aml_Z_pathname ){ name_, n }))
                        {   W( name_ );
                            return ~0;
                        }
                        E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_device_finish, E_aml_Z_parse_stack_Z_entity_S_object );
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                    }
                    break;
                }
              case 2: // event
                {   N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( ~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n })
                    || ( n > 1
                      && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    ))
                    {   W(name);
                        return ~0 - 1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",event=" ); E_font_I_print( name_ );
                    W( name_ );
                    N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_event, ( struct E_aml_Z_pathname ){ name, n });
                    if( (S)object_i < 0 )
                    {   W(name);
                        return object_i;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    break;
                }
              case 0x84: // power res
                {   Pc data_start = E_aml_S_parse_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( ~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n })
                    || ( n > 1
                      && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    ))
                    {   W(name);
                        return ~0 - 1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",power res=" ); E_font_I_print( name_ );
                    W( name_ );
                    N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_power_res, ( struct E_aml_Z_pathname ){ name, n });
                    if( (S)object_i < 0 )
                    {   W(name);
                        return object_i;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    if( E_aml_S_parse_data != pkg_end )
                    {   if( ++E_aml_S_parse_data > pkg_end )
                            return ~0 - 1;
                        E_aml_S_parse_data += 2;
                        if( E_aml_S_parse_data > pkg_end )
                            return ~0 - 1;
                        name_ = E_text_Z_sl_M_duplicate( name, n * 4 );
                        if( !name_ )
                            return ~0;
                        if( !~E_aml_Q_current_path_I_push(( struct E_aml_Z_pathname ){ name_, n }))
                        {   W( name_ );
                            return ~0;
                        }
                        E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_power_res_finish, E_aml_Z_parse_stack_Z_entity_S_object );
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                    }
                    break;
                }
              case 0x85: // thermal zone
                {   Pc data_start = E_aml_S_parse_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( ~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n })
                    || ( n > 1
                      && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    ))
                    {   W(name);
                        return ~0 - 1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",thermal zone=" ); E_font_I_print( name_ );
                    W( name_ );
                    N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_thermal_zone, ( struct E_aml_Z_pathname ){ name, n });
                    if( (S)object_i < 0 )
                    {   W(name);
                        return object_i;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    if( E_aml_S_parse_data != pkg_end )
                    {   name_ = E_text_Z_sl_M_duplicate( name, n * 4 );
                        if( !name_ )
                            return ~0;
                        if( !~E_aml_Q_current_path_I_push(( struct E_aml_Z_pathname ){ name_, n }))
                        {   W( name_ );
                            return ~0;
                        }
                        E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_thermal_zone_finish, E_aml_Z_parse_stack_Z_entity_S_object );
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                    }
                    break;
                }
              case 0x83: // processor
                {   Pc data_start = E_aml_S_parse_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~0 - 1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~0 - 1;
                    N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( ~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n })
                    || ( n > 1
                      && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    ))
                    {   W(name);
                        return ~0 - 1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",processor=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( ++E_aml_S_parse_data > pkg_end )
                        return ~0 - 1;
                    if( E_aml_S_parse_data + 4 > pkg_end )
                        return ~0 - 1;
                    E_aml_S_parse_data += 4;
                    if( ++E_aml_S_parse_data > pkg_end )
                        return ~0 - 1;
                    N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_processor, ( struct E_aml_Z_pathname ){ name, n });
                    if( (S)object_i < 0 )
                    {   W(name);
                        return object_i;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    if( E_aml_S_parse_data != pkg_end )
                    {   name_ = E_text_Z_sl_M_duplicate( name, n * 4 );
                        if( !name_ )
                            return ~0;
                        if( !~E_aml_Q_current_path_I_push(( struct E_aml_Z_pathname ){ name_, n }))
                        {   W( name_ );
                            return ~0;
                        }
                        E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_processor_finish, E_aml_Z_parse_stack_Z_entity_S_object );
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                    }
                    break;
                }
              default:
                {   E_aml_S_parse_data -= 2;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = no;
                    break;
                }
            }
            break;
      case 0x8d: // bit field
            E_font_I_print( ",bit field" );
            //TODO Dodać ‘bit field’ do drzewa zinterpretowanej przestrzeni ACPI.
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_bit_field_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x8c: // byte field
            E_font_I_print( ",byte field" );
            //TODO Dodać ‘byte field’ do drzewa zinterpretowanej przestrzeni ACPI.
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_byte_field_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x8a: // dword field
            E_font_I_print( ",dword field" );
            //TODO Dodać ‘dword field’ do drzewa zinterpretowanej przestrzeni ACPI.
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_dword_field_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x8f: // qword field
            E_font_I_print( ",qword field" );
            //TODO Dodać ‘qword field’ do drzewa zinterpretowanej przestrzeni ACPI.
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_qword_field_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x8b: // word field
            E_font_I_print( ",word field" );
            //TODO Dodać ‘word field’ do drzewa zinterpretowanej przestrzeni ACPI.
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_word_field_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x14: // method
        {   Pc data_start = E_aml_S_parse_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            N8 n;
            Pc name = E_aml_Q_path_R_root( &n );
            if( !name )
                return (S8)n;
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( "\n,procedure=" ); E_font_I_print( name_ );
            W( name_ );
            if( ~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n })
            || ( n > 1
              && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
            ))
            {   W(name);
                return ~0 - 1;
            }
            if( E_aml_S_parse_data == pkg_end )
                return ~0 - 1;
            struct E_aml_Z_object_data_Z_procedure *M_(procedure);
            if( !procedure )
                return ~0;
            procedure->arg_n = *E_aml_S_parse_data & 3;
            E_font_I_print( "," ); E_font_I_print_hex( procedure->arg_n );
            if( ++E_aml_S_parse_data > pkg_end )
            {   W(procedure);
                W(name);
                return ~0 - 1;
            }
            N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_procedure, ( struct E_aml_Z_pathname ){ name, n });
            if( (S)object_i < 0 )
            {   W(procedure);
                W(name);
                return object_i;
            }
            E_aml_S_object[ object_i ].data = procedure;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            if( E_aml_S_parse_data != pkg_end )
            {   name_ = E_text_Z_sl_M_duplicate( name, n * 4 );
                if( !name_ )
                    return ~0;
                if( !~E_aml_Q_current_path_I_push(( struct E_aml_Z_pathname ){ name_, n }))
                {   W( name_ );
                    return ~0;
                }
                E_aml_Q_current_path_P_precompilation(yes);
                E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_procedure_finish, E_aml_Z_parse_stack_Z_entity_S_term );
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
            }
            break;
        }
      default:
            E_aml_S_parse_data--;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = no;
            break;
    }
    return 0;
}
#undef E_aml_I_delegate_pkg
#undef E_aml_I_delegate
#define E_aml_I_delegate( finish, entity_ ) \
{  if( !E_mem_Q_blk_I_append( &E_aml_S_parse_stack, 1 )) \
        return ~0; \
    Pc data_end = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].entity = finish; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.break_ = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = entity_; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 1; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = data_end; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.break_ = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0; \
    E_aml_S_parse_stack_n++; \
}
#define E_aml_I_delegate_pkg(  finish, entity_ ) \
{   if( !E_mem_Q_blk_I_append( &E_aml_S_parse_stack, 1 )) \
        return ~0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].entity = finish; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.break_ = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = entity_; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 1; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = pkg_end; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.break_ = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0; \
    E_aml_S_parse_stack_n++; \
}
_internal
N
E_aml_I_expression( void
){  if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
        return ~0 - 1;
    switch( (N8)*E_aml_S_parse_data++ )
    { case 0x5b: // ext
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            switch( (N8)*E_aml_S_parse_data++ )
            { case 0x23: // acquire
                    E_font_I_print( ",acquire" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_acquire_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
              case 0x12: // cond refof
                    E_font_I_print( ",cond refof" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_cond_refof_finish_1, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
              case 0x28: // from bcd
                    E_font_I_print( ",from bcd" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_from_bcd_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
              case 0x1f: // load table
                    E_font_I_print( ",load table" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_load_table_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
              case 0x33: // timer
                    E_font_I_print( ",timer" );
                    break;
              case 0x29: // to bcd
                    E_font_I_print( ",to bcd" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_bcd_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
              case 0x25: // wait
                    E_font_I_print( ",wait" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_wait_finish_1, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
              default:
                    E_aml_S_parse_data -= 2;
                    return ~0 - 1;
            }
            break;
      case 0x72: // add
            E_font_I_print( ",add" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_add_op_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x7b: // and
            E_font_I_print( ",and" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_and_op_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x73: // concat
            E_font_I_print( ",concat" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_concat_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x84: // concat res
            E_font_I_print( ",concat res" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_concat_res_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x9d: // copy object
            E_font_I_print( ",copy object" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_copy_object_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x76: // decrement
            E_font_I_print( ",decrement" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_decrement_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
            break;
      case 0x83: // derefof
            E_font_I_print( ",derefof" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_derefof_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x78: // divide
            E_font_I_print( ",divide" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_divide_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x81: // find set left bit
            E_font_I_print( ",fslb" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_fslb_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x82: // find set right bit
            E_font_I_print( ",fsrb" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_fsrb_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x75: // increment
            E_font_I_print( ",increment" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_increment_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
            break;
      case 0x88: // index
            E_font_I_print( ",index" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_index_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x90: // land
            E_font_I_print( ",land" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_land_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x93: // lequal
            E_font_I_print( ",lequal" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lequal_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x94: // lgreater
            E_font_I_print( ",lgreater" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lgreater_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x92: // lnot
            E_font_I_print( ",lnot" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lnot_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x95: // lless
            E_font_I_print( ",lless" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lless_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x9e: // mid
            E_font_I_print( ",mid" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_mid_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x91: // lor
            E_font_I_print( ",lor" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lor_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x89: // match
            E_font_I_print( ",match" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_match_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x85: // mod
            E_font_I_print( ",mod" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_mod_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x77: // multiply
            E_font_I_print( ",multiply" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_multiply_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x7c: // nand
            E_font_I_print( ",nand" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_nand_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x7e: // nor
            E_font_I_print( ",nor" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_nor_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x80: // not
            E_font_I_print( ",not" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_not_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x8e: // object type
            E_font_I_print( ",object type" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_object_type_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
            break;
      case 0x7d: // or
            E_font_I_print( ",or" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_or_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x12: // package
        {   Pc data_start = E_aml_S_parse_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            if( E_aml_S_parse_data == pkg_end )
                return ~0 - 1;
            N n = (N8)*E_aml_S_parse_data;
            E_aml_S_parse_data++;
            E_font_I_print( ",package," ); E_font_I_print_hex(n);
            if( E_aml_Q_current_path_S_precompilation_i )
            {
            }else
            {   struct E_aml_Z_package *M_(package);
                if( !package )
                    return ~0;
                Mt_( package->value, 0 );
                if( !package->value )
                {   W(package);
                    return ~0;
                }
                package->n = 0;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package = package;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_package;
            }
            E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_package_finish, E_aml_Z_parse_stack_Z_entity_S_package );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = n;
            break;
        }
      case 0x13: // var package
        {   Pc data_start = E_aml_S_parse_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~0 - 1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            E_font_I_print( ",var package" );
            struct E_aml_Z_package *package;
            if( E_aml_Q_current_path_S_precompilation_i )
            {
            }else
            {   M_(package);
                if( !package )
                    return ~0;
                Mt_( package->value, 0 );
                if( !package->value )
                {   W(package);
                    return ~0;
                }
                package->n = 0;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package = package;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_package;
            }
            if( !E_mem_Q_blk_I_append( &E_aml_S_parse_stack, 4 ))
                return ~0;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = E_aml_Z_parse_stack_Z_entity_S_package_finish;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 0;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0;
            E_aml_S_parse_stack_n++;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = E_aml_Z_parse_stack_Z_entity_S_package;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 0;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0;
            E_aml_S_parse_stack_n++;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = E_aml_Z_parse_stack_Z_entity_S_result_to_n;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 0;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0;
            E_aml_S_parse_stack_n++;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = E_aml_Z_parse_stack_Z_entity_S_term_arg;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 1;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = pkg_end;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0;
            E_aml_S_parse_stack_n++;
            break;
        }
      case 0x71: // refof
            E_font_I_print( ",refof" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_refof_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
            break;
      case 0x79: // shift left
            E_font_I_print( ",shift left" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x7a: // shift right
            E_font_I_print( ",shift right" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_shift_right_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x87: // sizeof
            E_font_I_print( ",sizeof" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_sizeof_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
            break;
      case 0x70: // store
            E_font_I_print( ",store" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_store_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x74: // subtract
            E_font_I_print( ",subtract" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_subtract_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x96: // to buffer
            E_font_I_print( ",to buffer" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_buffer_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x97: // to decimal string
            E_font_I_print( ",to decimal string" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_decimal_string_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x98: // to hex string
            E_font_I_print( ",to hex string" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_hex_string_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x99: // to integer
            E_font_I_print( ",to integer" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_integer_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x9c: // to string
            E_font_I_print( ",to string" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_string_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x7f: // xor
            E_font_I_print( ",xor" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_xor_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      default:
        {   E_aml_S_parse_data--;
            N8 n;
            N object_i;
            Pc name = E_aml_Q_path_R_relative( &n, &object_i );
            if( !name )
                return (S8)n;
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( ",expr string=" ); E_font_I_print( name_ );
            W( name_ );
            // method invocation
            if( !~object_i )
            {   name_ = name + ( n - 1 ) * 4;
                if( !( E_text_Z_sl_T_eq( name_, "_OS_", 4 )
                || E_text_Z_sl_T_eq( name_, "_OSI", 4 )
                || E_text_Z_sl_T_eq( name_, "_REV", 4 )
                ))
                {   W(name);
                    if( n > 1
                    && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    )
                        return ~0 - 1;
                    break;
                }
                if( E_text_Z_sl_T_eq( name_, "_OS_", 4 ))
                {   if( E_aml_Q_current_path_S_precompilation_i )
                    {
                    }else
                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_string;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p = "OUXOS";
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                    }
                }else if( E_text_Z_sl_T_eq( name_, "_REV", 4 ))
                    if( E_aml_Q_current_path_S_precompilation_i )
                    {
                    }else
                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = 2;
                    }
            }else if( E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_procedure )
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_pathname;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname = ( struct E_aml_Z_pathname ){ name, n };
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                break;
            }
            E_font_I_print( ",invocation" );
            struct E_aml_Z_object_data_Z_procedure *procedure;
            if( ~object_i
            && ( procedure = E_aml_S_object[ object_i ].data, procedure->arg_n )
            )
            {   E_font_I_print( "," ); E_font_I_print_hex( procedure->arg_n );
                if( !E_aml_Q_current_path_S_precompilation_i )
                {   if( !E_mem_Q_blk_I_append( &E_aml_S_procedure_invocation_stack, 1 ))
                        return ~0;
                    E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].procedure = E_aml_S_object[ object_i ].name;
                    for_n( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].arg ))
                        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].arg[i].type = E_aml_Z_value_Z_type_S_uninitialized;
                    for_n_( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].local ))
                        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].local[i].type = E_aml_Z_value_Z_type_S_uninitialized;
                    E_aml_S_procedure_invocation_stack_n++;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = procedure->arg_n;
                if( !E_aml_Q_current_path_S_precompilation_i )
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.procedure_osi = no;
            }else if( !~object_i
            && E_text_Z_sl_T_eq( name_, "_OSI", 4 )
            )
            {   if( !E_aml_Q_current_path_S_precompilation_i )
                {   if( !E_mem_Q_blk_I_append( &E_aml_S_procedure_invocation_stack, 1 ))
                        return ~0;
                    E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].procedure = ( struct E_aml_Z_pathname ){ 0, 0 };
                    for_n( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].arg ))
                        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].arg[i].type = E_aml_Z_value_Z_type_S_uninitialized;
                    for_n_( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].local ))
                        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].local[i].type = E_aml_Z_value_Z_type_S_uninitialized;
                    E_aml_S_procedure_invocation_stack_n++;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = 1;
                if( !E_aml_Q_current_path_S_precompilation_i )
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.procedure_osi = yes;
            }
            W(name);
            break;
        }
    }
    return 0;
}
//------------------------------------------------------------------------------
_internal
N
E_aml_I_supername( void
){  if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
        return ~0 - 1;
    switch( (N8)*E_aml_S_parse_data++ )
    { case 0x5b:
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~0 - 1;
            switch( (N8)*E_aml_S_parse_data++ )
            { case 0x31: // debug
                    E_font_I_print( ",debug" );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_debug_ref;
                    break;
              default:
                    E_aml_S_parse_data -= 2;
                    return ~0 - 1;
            }
            break;
      case 0x71: // ref
            E_font_I_print( ",ref" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_refof_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
            break;
      case 0x83: // deref
            E_font_I_print( ",deref" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_derefof_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x88: // index
            E_font_I_print( ",index" );
            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_index_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
      case 0x68: // arg
      case 0x69:
      case 0x6a:
      case 0x6b:
      case 0x6c:
      case 0x6d:
      case 0x6e:
        {   E_font_I_print( ",arg" );
            if( E_aml_Q_current_path_S_precompilation_i )
            {
            }else
            {   if( !E_aml_S_procedure_invocation_stack_n )
                    return ~0 - 1;
                N arg_i = (N8)*( E_aml_S_parse_data - 1 ) - 0x68;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = arg_i;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_arg_ref;
            }
            break;
        }
      case 0x60: // local
      case 0x61:
      case 0x62:
      case 0x63:
      case 0x64:
      case 0x65:
      case 0x66:
      case 0x67:
        {   E_font_I_print( ",local" );
            if( E_aml_Q_current_path_S_precompilation_i )
            {
            }else
            {   if( !E_aml_S_procedure_invocation_stack_n )
                    return ~0 - 1;
                N local_i = (N8)*( E_aml_S_parse_data - 1 ) - 0x60;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = local_i;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_local_ref;
            }
            break;
        }
      default: // simple name
        {   E_aml_S_parse_data--;
            N8 n;
            Pc name = E_aml_Q_path_R_root( &n );
            if( !name )
                return (S8)n;
            if( E_aml_Q_current_path_S_precompilation_i )
            {   
                W(name);
            }else
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_pathname;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname = ( struct E_aml_Z_pathname ){ name, n };
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
            }
            break;
        }
    }
    return 0;
}
#undef E_aml_I_delegate_pkg
#undef E_aml_I_delegate
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define E_aml_I_delegate( finish, entity_ ) \
{   if( !E_mem_Q_blk_I_append( &E_aml_S_parse_stack, 1 )) \
        goto Error; \
    Pc data_end = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].entity = finish; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = entity_; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 1; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = data_end; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.break_ = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0; \
    E_aml_S_parse_stack_n++; \
    stack_n_last = E_aml_S_parse_stack_n; \
}
_internal
N
E_aml_M_arg_local_debug( void
){  if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_arg_ref )
    {   if( !E_aml_S_procedure_invocation_stack_n )
            return ~0;
        N arg_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
        if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].type != E_aml_Z_value_Z_type_S_uninitialized
        && E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].copy
        )
            E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ] );
        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result;
        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
        return yes;
    }
    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_local_ref )
    {   if( !E_aml_S_procedure_invocation_stack_n )
            return ~0;
        N local_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
        if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].type != E_aml_Z_value_Z_type_S_uninitialized
        && E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].copy
        )
            E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ] );
        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result;
        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
        return yes;
    }
    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_debug_ref )
        return yes;
    return no;
}
_internal
N
E_aml_M_clear_object( void
){  N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
    if( !~object_i )
        return ~0;
    if( E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_number
    && E_aml_S_object[ object_i ].data
    )
        E_aml_Q_object_W_data( object_i );
    return object_i;
}
_internal
N
E_aml_M_convert( struct E_aml_Z_value **e
, B copy
, struct E_aml_Z_value *v_0
, struct E_aml_Z_value *v_1
){  if( v_0->type != v_1->type )
    {   if( !copy )
        {   M_( *e );
            if( !*e )
                return ~0;
            switch( v_1->type )
            { case E_aml_Z_value_Z_type_S_number:
                    **e = *v_1;
                    break;
              case E_aml_Z_value_Z_type_S_string:
                    (*e)->p = E_text_Z_s0_M_duplicate( v_0->p );
                    if( !(*e)->p )
                    {   W( *e );
                        return ~0;
                    }
                    (*e)->i = ~0;
                    (*e)->type = E_aml_Z_value_Z_type_S_string;
                    break;
              case E_aml_Z_value_Z_type_S_buffer:
                    (*e)->buffer.p = E_text_Z_sl_M_duplicate( v_0->buffer.p, v_0->buffer.n );
                    if( !(*e)->buffer.p )
                    {   W( *e );
                        return ~0;
                    }
                    (*e)->i = ~0;
                    (*e)->type = E_aml_Z_value_Z_type_S_buffer;
                    break;
            }
            (*e)->copy = yes;
        }else
            *e = v_0;
        N ret = E_aml_Q_value_N_convert( v_1->type, *e );
        if( ret < 0 )
        {   if( !copy )
                W( *e );
            return ret;
        }
    }else
        *e = v_0;
    return 0;
}
_internal
B
E_aml_M_cmp( N8 op
, struct E_aml_Z_value *e
, struct E_aml_Z_value *v
){  B ret;
    switch(op)
    { case 0:
            ret = yes;
            break;
      case 1:
            ret = E_aml_Q_value_T_cmp_eq( e, v );
            break;
      case 2:
            ret = !E_aml_Q_value_T_cmp_gt( e, v );
            break;
      case 3:
            ret = E_aml_Q_value_T_cmp_lt( e, v );
            break;
      case 4:
            ret = !E_aml_Q_value_T_cmp_lt( e, v );
            break;
      case 5:
            ret = E_aml_Q_value_T_cmp_gt( e, v );
            break;
    }
    return ret;
}

_internal
N
E_aml_M_res1( void
){  if( E_aml_Q_current_path_S_precompilation_i )
    {   N current_path_i = E_aml_S_current_path_n;
        for_n_rev( i, E_aml_S_parse_stack_n ) // Wyjście do nadrzędnego bloku metody po błędzie składni.
        {   if(( E_aml_S_parse_stack[i].entity == E_aml_Z_parse_stack_Z_entity_S_restore_current_path
              || E_aml_S_parse_stack[i].entity == E_aml_Z_parse_stack_Z_entity_S_power_res_finish
              || E_aml_S_parse_stack[i].entity == E_aml_Z_parse_stack_Z_entity_S_thermal_zone_finish
              || E_aml_S_parse_stack[i].entity == E_aml_Z_parse_stack_Z_entity_S_device_finish
              || E_aml_S_parse_stack[i].entity == E_aml_Z_parse_stack_Z_entity_S_processor_finish
              || E_aml_S_parse_stack[i].entity == E_aml_Z_parse_stack_Z_entity_S_procedure_finish
            )
            && E_aml_Q_current_path_S_precompilation_i != current_path_i--
            )
            {   for_n( j, E_aml_S_current_path_n - current_path_i )
                    W( E_aml_S_current_path[ current_path_i + j ].s );
                if( !E_mem_Q_blk_I_remove( &E_aml_S_current_path, current_path_i, E_aml_S_current_path_n - current_path_i ))
                    return ~0;
                E_aml_S_current_path_n -= E_aml_S_current_path_n - current_path_i;
                if( E_aml_S_current_path_n )
                {   Pc name_ = M( E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].s, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n * 4 );
                    E_font_I_print( "\n,go over to=" ); E_font_I_print( name_ );
                    W( name_ );
                }else
                    E_font_I_print( "\n,go over to=\\" );
                if( !~E_aml_S_parse_stack[ i - 1 ].n )
                    if( !--i ) // Sprawdzanie dla listy wyliczanej w nieskończoność, czy interpretacja zakończyła się.
                        break;
                E_aml_S_parse_data = E_aml_S_parse_stack[i].data_end;
                if( !E_mem_Q_blk_I_remove( &E_aml_S_parse_stack, i, E_aml_S_parse_stack_n - i ))
                    return ~0;
                E_aml_S_parse_stack_n = i;
                E_aml_Q_current_path_S_precompilation_i = 0;
                return 0;
            }
            if( !current_path_i )
                break;
        }
    }
    return ~0;
}
_private
N
E_aml_M( Pc table
, N l
){  if( !~E_aml_Q_current_path_M() )
        return ~0;
    E_aml_S_parse_stack_n = 1;
    Mt_( E_aml_S_parse_stack, E_aml_S_parse_stack_n );
    if( !E_aml_S_parse_stack )
    {   E_aml_Q_current_path_W();
        return ~0;
    }
    if( !~E_aml_Q_object_M() )
    {   W( E_aml_S_parse_stack );
        E_aml_Q_current_path_W();
        return ~0;
    }
    E_aml_S_parse_data = table;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].entity = E_aml_Z_parse_stack_Z_entity_S_term;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end = table + l;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p = 0;
    E_aml_S_procedure_invocation_stack_n = 0;
    Mt_( E_aml_S_procedure_invocation_stack, E_aml_S_procedure_invocation_stack_n );
    if( !E_aml_S_procedure_invocation_stack )
    {   E_aml_Q_object_W();
        W( E_aml_S_parse_stack );
        E_aml_Q_current_path_W();
        return ~0;
    }
    N stack_n_last;
    Pc data = 0;
    N ret = 0;
Loop:
    O{  stack_n_last = E_aml_S_parse_stack_n;
        enum E_aml_Z_parse_stack_Z_entity entity = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].entity;
        //E_font_I_print( "\nentity=" ); E_font_I_print_hex(entity);
        //E_font_I_print( ",data=" ); E_font_I_print_hex( (N)E_aml_S_parse_data );
        //E_font_I_print( ",data_end=" ); E_font_I_print_hex( (N)E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end );
        //E_font_I_print( ",value=" ); E_font_I_print_hex( (( N * )E_aml_S_parse_data )[0] ); E_font_I_print( "," ); E_font_I_print_hex( (( N * )E_aml_S_parse_data )[1] );
        switch(entity)
        { case E_aml_Z_parse_stack_Z_entity_S_result_to_n:
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_restore_current_path:
                if( !~E_aml_Q_current_path_I_pop() )
                    goto Error;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_term:
                ret = E_aml_I_term();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_term_finish_1:
                if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].match )
                    break;
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_term_finish_2, E_aml_Z_parse_stack_Z_entity_S_expression );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_term_finish_2:
                break;
          case E_aml_Z_parse_stack_Z_entity_S_term_arg:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                switch( (N8)*E_aml_S_parse_data++ )
                { case 0x68: // arg
                  case 0x69:
                  case 0x6a:
                  case 0x6b:
                  case 0x6c:
                  case 0x6d:
                  case 0x6e:
                        E_font_I_print( ",arg" );
                        if( E_aml_Q_current_path_S_precompilation_i )
                        {
                        }else
                        {   if( !E_aml_S_procedure_invocation_stack_n )
                            {   ret = ~0 - 1;
                                break;
                            }
                            N arg_i = (N8)*( E_aml_S_parse_data - 1 ) - 0x68;
                            if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].procedure.s == E_aml_S_object[ E_aml_S_object_n - 1 ].name.s // ‘method invocation’ z argumentami
                            || E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.procedure_osi
                            )
                            {   if( E_aml_S_procedure_invocation_stack_n < 2
                                || E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 2 ].arg[ arg_i ].type == E_aml_Z_value_Z_type_S_uninitialized
                                )
                                {   ret = ~0 - 1;
                                    break;
                                }
                                for_n_rev( arg_n, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].arg ))
                                    if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_n ].type != E_aml_Z_value_Z_type_S_uninitialized )
                                        break;
                                E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_n - E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n ] = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 2 ].arg[ arg_i ];
                            }else
                            {   if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].type == E_aml_Z_value_Z_type_S_uninitialized )
                                {   ret = ~0 - 1;
                                    break;
                                }
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ];
                            }
                        }
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
                        if( E_aml_Q_current_path_S_precompilation_i )
                        {
                        }else
                        {   if( !E_aml_S_procedure_invocation_stack_n )
                            {   ret = ~0 - 1;
                                break;
                            }
                            N local_i = (N8)*( E_aml_S_parse_data - 1 ) - 0x60;
                            if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].type == E_aml_Z_value_Z_type_S_uninitialized )
                            {   ret = ~0 - 1;
                                break;
                            }
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ];
                        }
                        break;
                  default:
                    {   E_aml_S_parse_data--;
                        if( !E_mem_Q_blk_I_append( &E_aml_S_parse_stack, 2 ))
                            goto Error;
                        Pc data_end = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = E_aml_Z_parse_stack_Z_entity_S_term_arg_finish_1;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = 0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0;
                        E_aml_S_parse_stack_n++;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = E_aml_Z_parse_stack_Z_entity_S_data_object;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 1;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = data_end;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0;
                        E_aml_S_parse_stack_n++;
                        break;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_data_object:
                ret = E_aml_I_data_object();
                if( ret == ~0 - 1
                && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].entity == E_aml_Z_parse_stack_Z_entity_S_term_arg_finish_1
                )
                    ret = 0;
                else if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].entity == E_aml_Z_parse_stack_Z_entity_S_package_finish )
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result;
                    E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.copy = no;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = E_aml_S_parse_stack[ stack_n_last - 2 ].match;
                }else if( E_aml_S_parse_stack_n >= 4
                && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 4 ].entity == E_aml_Z_parse_stack_Z_entity_S_package_finish
                )
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 4 ].execution_context.result = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result;
                    E_aml_S_parse_stack[ stack_n_last - 4 ].execution_context.result.copy = no;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 4 ].match = E_aml_S_parse_stack[ stack_n_last - 2 ].match;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_term_arg_finish_1:
                if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].match )
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    break;
                }
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].entity = E_aml_Z_parse_stack_Z_entity_S_expression;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = 1;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_object:
                ret = E_aml_I_object();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_expression:
                ret = E_aml_I_expression();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_supername:
                ret = E_aml_I_supername();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_buffer_finish:
                if( E_aml_Q_current_path_S_precompilation_i )
                {   
                    E_aml_S_parse_data = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end;
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    N n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                    if( E_aml_S_parse_data + n > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p = E_aml_S_parse_data;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n = n;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_buffer;
                    E_aml_S_parse_data += n;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_package:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end ) // Na przypadek startu z “n == 0”.
                    break;
                struct E_aml_Z_value value_package = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result;
                ret = E_aml_I_data_object();
                if( (S)ret < 0 )
                    break;
                if( !E_aml_S_parse_stack[ stack_n_last - 2 ].match )
                {   N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                    {   ret = (S8)n;
                        E_aml_Q_value_W( &value_package );
                        break;
                    }
                    if( E_aml_Q_current_path_S_precompilation_i )
                    {
                    }else
                    {   if( !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n }))
                        {   W(name);
                            E_aml_Q_value_W( &value_package );
                            goto Error;
                        }
                        struct E_aml_Z_value *value = E_mem_Q_blk_I_append( &value_package.package->value, 1 );
                        if( !value )
                        {   W(name);
                            E_aml_Q_value_W( &value_package );
                            goto Error;
                        }
                        value_package.package->n++;
                        value->pathname = ( struct E_aml_Z_pathname ){ name, n };
                        value->type = E_aml_Z_value_Z_type_S_pathname;
                        E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result = value_package;
                    }
                }else
                    if( E_aml_Q_current_path_S_precompilation_i )
                    {
                    }else
                    {   struct E_aml_Z_value *value = E_mem_Q_blk_I_append( &value_package.package->value, 1 );
                        if( !value )
                        {   E_aml_Q_value_W( &E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result );
                            goto Error;
                        }
                        switch( E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.type )
                        { case E_aml_Z_value_Z_type_S_number:
                                value->n = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.n;
                                break;
                          case E_aml_Z_value_Z_type_S_string:
                            {   Pc s;
                                if( E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.copy )
                                {   E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.copy = no;
                                    s = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.p;
                                }else
                                {   s = E_text_Z_s0_M_duplicate( E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.p );
                                    if( !s )
                                    {   E_aml_Q_value_W( &value_package );
                                        goto Error;
                                    }
                                }
                                value->p = s;
                                value->i = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.i;
                                break;
                            }
                          case E_aml_Z_value_Z_type_S_buffer:
                            {   if( E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.copy )
                                {   E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.copy = no;
                                    value->buffer.p = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.buffer.p;
                                }else
                                {   value->buffer.p = E_text_Z_sl_M_duplicate( E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.buffer.p
                                    , E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.buffer.n
                                    );
                                    if( !value->buffer.p )
                                    {   E_aml_Q_value_W( &value_package );
                                        goto Error;
                                    }
                                }
                                value->buffer.n = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.buffer.n;
                                value->i = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.i;
                                break;
                            }
                          case E_aml_Z_value_Z_type_S_package:
                            {   E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.copy = no;
                                *value = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result;
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result;
                                break;
                            }
                          default:
                                E_aml_Q_value_W( &value_package );
                                goto Error;
                        }
                        value->type = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.type;
                        value_package.package->n++;
                        E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result = value_package;
                    }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_package_finish:
                break;
          case E_aml_Z_parse_stack_Z_entity_S_bank_field_finish:
            {   if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                E_aml_S_parse_data++;
                while( E_aml_S_parse_data != E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   switch( *E_aml_S_parse_data++ )
                    { case 0: // reserved field
                        {   if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
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
                            if( E_aml_S_parse_data + 2 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                            {   ret = ~0 - 1;
                                break;
                            }
                            //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                            E_aml_S_parse_data++;
                            //TODO Dodać ‘access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                            E_aml_S_parse_data++;
                            break;
                      case 2: // connect field
                            E_font_I_print( ", connect field" );
                            ret = ~0; //NDFN
                            break;
                      case 3: // extended access field
                            if( E_aml_S_parse_data + 3 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                            {   ret = ~0 - 1;
                                break;
                            }
                            //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                            E_aml_S_parse_data++;
                            //TODO Dodać ‘extended access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                            E_aml_S_parse_data++;
                            //TODO Dodać ‘access length’ do drzewa zinterpretowanej przestrzeni ACPI.
                            E_aml_S_parse_data++; //NDFN Jaki rozmiar.
                            break;
                      default: // named field
                        {   if( E_aml_S_parse_data + 4 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                            {   ret = ~0 - 1;
                                break;
                            }
                            Pc name_ = M( 4 + 1 );
                            E_text_Z_s_P_copy_sl_0( name_, E_aml_S_parse_data, 4 );
                            E_font_I_print( ",field=" ); E_font_I_print( name_ );
                            W( name_ );
                            //TODO Dodać ‘field name’ do drzewa zinterpretowanej przestrzeni ACPI.
                            E_aml_S_parse_data += 4;
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
          case E_aml_Z_parse_stack_Z_entity_S_bit_field_finish_1:
                //TODO Dodać ‘buffer’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_bit_field_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_bit_field_finish_2:
            {   //TODO Dodać ‘bit index’ do drzewa zinterpretowanej przestrzeni ACPI.
                N8 n;
                Pc name = E_aml_Q_path_R_root( &n );
                if( !name )
                {   ret = (S8)n;
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
          case E_aml_Z_parse_stack_Z_entity_S_byte_field_finish_1:
                //TODO Dodać ‘buffer’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_byte_field_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_byte_field_finish_2:
            {   //TODO Dodać ‘byte index’ do drzewa zinterpretowanej przestrzeni ACPI.
                N8 n;
                Pc name = E_aml_Q_path_R_root( &n );
                if( !name )
                {   ret = (S8)n;
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
          case E_aml_Z_parse_stack_Z_entity_S_dword_field_finish_1:
                //TODO Dodać ‘buffer’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_dword_field_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_dword_field_finish_2:
            {   //TODO Dodać ‘byte index’ do drzewa zinterpretowanej przestrzeni ACPI.
                N8 n;
                Pc name = E_aml_Q_path_R_root( &n );
                if( !name )
                {   ret = (S8)n;
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
          case E_aml_Z_parse_stack_Z_entity_S_field_finish_1:
                //TODO Dodać ‘buffer’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_field_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_field_finish_2:
                //TODO Dodać ‘bit index’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_field_finish_3, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_field_finish_3:
            {   //TODO Dodać ‘num bits’ do drzewa zinterpretowanej przestrzeni ACPI.
                N8 n;
                Pc name = E_aml_Q_path_R_root( &n );
                if( !name )
                {   ret = (S8)n;
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
          case E_aml_Z_parse_stack_Z_entity_S_qword_field_finish_1:
                //TODO Dodać ‘buffer’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_qword_field_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_qword_field_finish_2:
            {   //TODO Dodać ‘byte index’ do drzewa zinterpretowanej przestrzeni ACPI.
                N8 n;
                Pc name = E_aml_Q_path_R_root( &n );
                if( !name )
                {   ret = (S8)n;
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
          case E_aml_Z_parse_stack_Z_entity_S_word_field_finish_1:
                //TODO Dodać ‘buffer’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_word_field_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_word_field_finish_2:
            {   //TODO Dodać ‘byte index’ do drzewa zinterpretowanej przestrzeni ACPI.
                N8 n;
                Pc name = E_aml_Q_path_R_root( &n );
                if( !name )
                {   ret = (S8)n;
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
          case E_aml_Z_parse_stack_Z_entity_S_data_region_finish_1:
                //TODO Dodać ‘signature’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_data_region_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_data_region_finish_2:
                //TODO Dodać ‘oem id’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_data_region_finish_3, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_data_region_finish_3:
                //TODO Dodać ‘oem table id’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_op_region_finish_1:
                //TODO Dodać ‘region offset’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_op_region_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_op_region_finish_2:
                //TODO Dodać ‘region len’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_power_res_finish:
                //TODO Dodać ‘power res’ do drzewa zinterpretowanej przestrzeni ACPI.
                if( !~E_aml_Q_current_path_I_pop() )
                    goto Error;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_thermal_zone_finish:
                //TODO Dodać ‘thermal zone’ do drzewa zinterpretowanej przestrzeni ACPI.
                if( !~E_aml_Q_current_path_I_pop() )
                    goto Error;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_device_finish:
                //TODO Dodać ‘device’ do drzewa zinterpretowanej przestrzeni ACPI.
                if( !~E_aml_Q_current_path_I_pop() )
                    goto Error;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_processor_finish:
                //TODO Dodać ‘processor’ do drzewa zinterpretowanej przestrzeni ACPI.
                if( !~E_aml_Q_current_path_I_pop() )
                    goto Error;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_if_op_finish_1:
                if( E_aml_S_parse_data != E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   if( E_aml_Q_current_path_S_precompilation_i )
                    {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_if_op_finish_2, E_aml_Z_parse_stack_Z_entity_S_term );
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                    }else
                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                            goto Error;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n )
                        {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_if_op_finish_2, E_aml_Z_parse_stack_Z_entity_S_term );
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                        }else
                            E_aml_S_parse_data = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_if_op_finish_2:
                //TODO Dodać ‘if’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_else_op_finish:
                //TODO Dodać ‘else’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_notify_finish_1:
                //TODO Dodać ‘notify object’ do drzewa zinterpretowanej przestrzeni ACPI.
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_notify_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_notify_finish_2:
                //TODO Dodać ‘notify value’ do drzewa zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_release_finish:
                //TODO Zwolnić ‘mutex’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_reset_finish:
                //TODO ‘Zresetować’ ‘event’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_return_finish:
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_signal_finish:
                //TODO Sygnalizować ‘event’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_sleep_finish:
                //TODO Wstrzymać wykonywanie w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_stall_finish:
                //TODO Wstrzymać wykonywanie w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_while_op_finish_1:
                if( E_aml_S_parse_data != E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   if( E_aml_Q_current_path_S_precompilation_i )
                    {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_while_op_finish_2, E_aml_Z_parse_stack_Z_entity_S_term );
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                    }else
                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                            goto Error;
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n )
                        {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.break_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end;
                            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_while_op_finish_2, E_aml_Z_parse_stack_Z_entity_S_term );
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                        }
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_while_op_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   E_aml_S_parse_data = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.continue_;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_while_op_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_acquire_finish:
                //TODO Dodać ‘mutex’ do drzewa zinterpretowanej przestrzeni ACPI.
                if( E_aml_S_parse_data + 2 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                E_aml_S_parse_data += 2;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_add_op_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_add_op_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_add_op_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n += E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_add_op_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_add_op_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_and_op_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_and_op_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_and_op_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n &= E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_and_op_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_and_op_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_concat_finish_1:
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_concat_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_concat_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                //TODO Utworzyć operację ‘concat’ w drzewie zinterpretowanej przestrzeni ACPI.
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_concat_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_concat_finish_3:
                break;
          case E_aml_Z_parse_stack_Z_entity_S_concat_res_finish_1:
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_concat_res_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_concat_res_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                //TODO Utworzyć operację ‘concat res’ w drzewie zinterpretowanej przestrzeni ACPI.
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_concat_res_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_concat_res_finish_3:
                break;
          case E_aml_Z_parse_stack_Z_entity_S_cond_refof_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   B ret = yes;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( !~object_i )
                            ret = no;
                    }
                    struct E_aml_Z_value *Mt_( value, E_aml_S_parse_stack_S_tmp_p_n );
                    if( !value )
                        goto Error;
                    value[0] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p = value;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = ret;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                    for_n( i, E_aml_S_parse_stack_S_tmp_p_n )
                        value[i].copy = no;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_cond_refof_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_cond_refof_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_copy_object_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname
                    && !~E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname )
                    )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_copy_object_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_copy_object_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                        switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type )
                        { case E_aml_Z_value_Z_type_S_number:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    goto Error;
                                E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                                break;
                            }
                          case E_aml_Z_value_Z_type_S_string:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    goto Error;
                                Pc s;
                                if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                    s = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p;
                                }else
                                {   s = E_text_Z_s0_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p );
                                    if( !s )
                                        goto Error;
                                }
                                E_aml_S_object[ object_i ].data = s;
                                E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                                break;
                            }
                          case E_aml_Z_value_Z_type_S_buffer:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    goto Error;
                                struct E_aml_Z_buffer *M_(buffer);
                                if( !buffer )
                                    goto Error;
                                if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                    buffer->p = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p;
                                }else
                                {   buffer->p = E_text_Z_sl_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p
                                    , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n
                                    );
                                    if( !buffer->p )
                                    {   W(buffer);
                                        goto Error;
                                    }
                                }
                                buffer->n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n;
                                E_aml_S_object[ object_i ].data = buffer;
                                E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                                break;
                            }
                          case E_aml_Z_value_Z_type_S_package:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    goto Error;
                                struct E_aml_Z_package *package;
                                if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                    package = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package;
                                }else
                                {   package = E_aml_Q_object_I_package_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package );
                                    if( !package )
                                        goto Error;
                                }
                                E_aml_S_object[ object_i ].data = package;
                                E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_package;
                                break;
                            }
                          default:
                                goto Error;
                        }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_decrement_finish:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {  if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_arg_ref )
                    {   if( !E_aml_S_procedure_invocation_stack_n )
                            goto Error;
                        N arg_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
                        if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].type != E_aml_Z_value_Z_type_S_number )
                            goto Error;
                        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].n--;
                        break;
                    }
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_local_ref )
                    {   if( !E_aml_S_procedure_invocation_stack_n )
                            goto Error;
                        N local_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
                        if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].type != E_aml_Z_value_Z_type_S_number )
                            goto Error;
                        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].n--;
                        break;
                    }
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_debug_ref )
                        break;
                    N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                    if( !~object_i
                    || E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_number
                    )
                        goto Error;
                    E_aml_S_object[ object_i ].n--;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_derefof_finish:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if(ret)
                        break;
                    N object_i;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_string )
                    {   if( E_text_Z_s0_R_l( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.p ) != 4 )
                            goto Error;
                        if( E_aml_S_current_path_n )
                            object_i = E_aml_Q_object_R_relative( E_aml_S_current_path[ E_aml_S_current_path_n - 1 ]
                            , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.p
                            );
                        else
                            object_i = E_aml_Q_object_R(( struct E_aml_Z_pathname ){ E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.p, 1 });
                    }else
                        object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                    if( !~object_i )
                        goto Error;
                    switch( E_aml_S_object[ object_i ].type )
                    { case E_aml_Z_object_Z_type_S_number:
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_S_object[ object_i ].n;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                            break;
                      case E_aml_Z_object_Z_type_S_string:
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p = E_aml_S_object[ object_i ].data;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = E_aml_S_object[ object_i ].n;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_string;
                            break;
                      case E_aml_Z_object_Z_type_S_buffer:
                        {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer = *buffer;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = E_aml_S_object[ object_i ].n;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_buffer;
                            break;
                        }
                      case E_aml_Z_object_Z_type_S_package:
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package = E_aml_S_object[ object_i ].data;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = E_aml_S_object[ object_i ].n;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_package;
                            break;
                      default:
                            goto Error;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_divide_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_divide_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_divide_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    || !E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n
                    )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n % E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n /= E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_divide_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_divide_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_divide_finish_4, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_divide_finish_4:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_fslb_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_asm_I_bsr( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n ) + 1;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_fslb_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_fslb_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_fsrb_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_asm_I_bsf( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n ) + 1;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_fsrb_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_fsrb_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_from_bcd_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    N n = 0;
                    for_n_rev( i, sizeof( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n ))
                    {   n *= 10;
                        n += E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n >> 4;
                        n *= 10;
                        n += E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n & 0xf;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = n;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_from_bcd_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_from_bcd_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( !~object_i
                        || E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_number
                        )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_increment_finish:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {  if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_arg_ref )
                    {   if( !E_aml_S_procedure_invocation_stack_n )
                            goto Error;
                        N arg_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
                        if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].type != E_aml_Z_value_Z_type_S_number )
                            goto Error;
                        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].n++;
                        break;
                    }
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_local_ref )
                    {   if( !E_aml_S_procedure_invocation_stack_n )
                            goto Error;
                        N local_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
                        if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].type != E_aml_Z_value_Z_type_S_number )
                            goto Error;
                        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].n++;
                        break;
                    }
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_debug_ref )
                        break;
                    N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                    if( !~object_i
                    || E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_number
                    )
                        goto Error;
                    E_aml_S_object[ object_i ].n++;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_index_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_string
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_buffer
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_package
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname
                    )
                        goto Error;
                    N object_i;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( !~object_i
                        || ( E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_string
                          && E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_buffer
                          && E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_package
                        ))
                            goto Error;
                    }
                    struct E_aml_Z_value *Mt_( value, E_aml_S_parse_stack_S_tmp_p_n );
                    if( !value )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p = value;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   switch( E_aml_S_object[ object_i ].type )
                        { case E_aml_Z_object_Z_type_S_string:
                                value[0].p = E_aml_S_object[ object_i ].data;
                                value[0].i = E_aml_S_object[ object_i ].n;
                                value[0].type = E_aml_Z_value_Z_type_S_string;
                                break;
                          case E_aml_Z_object_Z_type_S_buffer:
                            {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                                value[0].buffer = *buffer;
                                value[0].i = E_aml_S_object[ object_i ].n;
                                value[0].type = E_aml_Z_value_Z_type_S_buffer;
                                break;
                            }
                          default:
                                value[0].package = E_aml_S_object[ object_i ].data;
                                value[0].i = E_aml_S_object[ object_i ].n;
                                value[0].type = E_aml_Z_value_Z_type_S_package;
                                break;
                        }
                        value[0].copy = no;
                    }else
                    {   value[0] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                    }
                    for_n( i, E_aml_S_parse_stack_S_tmp_p_n - 1 )
                        value[ 1 + i ].copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_index_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_index_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n <= 255
                    )
                        goto Error;
                    struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    N i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                    struct E_aml_Z_value *value_;
                    N i_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i;
                    if( ~i_
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_package
                    )
                    {   value_ = &value[0].package->value[ i_ ];
                        i_ = ~0;
                    }else
                        value_ = &value[0];
                    if( !~i )
                        switch( value_->type )
                        { case E_aml_Z_value_Z_type_S_string:
                                if( i >= E_text_Z_s0_R_l( value_->p ))
                                    goto Error;
                                break;
                          case E_aml_Z_value_Z_type_S_buffer:
                                if( i >= value_->buffer.n )
                                    goto Error;
                                break;
                          default:
                                if( i >= value_->package->n )
                                    goto Error;
                                break;
                        }
                    else
                    {   i_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i;
                        switch( value_->type )
                        { case E_aml_Z_value_Z_type_S_string:
                                if( i >= E_text_Z_s0_R_l( value_->p )
                                || i_ >= 1
                                )
                                    goto Error;
                                break;
                          default:
                                if( i >= value_->buffer.n
                                || i_ >= 1
                                )
                                    goto Error;
                                break;
                        }
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = i;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_index_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_index_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                        switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type )
                        { case E_aml_Z_value_Z_type_S_string:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    goto Error;
                                Pc s;
                                if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                    s = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p;
                                }else
                                {   s = E_text_Z_s0_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p );
                                    if( !s )
                                        goto Error;
                                }
                                E_aml_S_object[ object_i ].data = s;
                                E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                                break;
                            }
                          case E_aml_Z_value_Z_type_S_buffer:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    goto Error;
                                struct E_aml_Z_buffer *M_(buffer);
                                if( !buffer )
                                    goto Error;
                                if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                    buffer->p = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p;
                                }else
                                {   buffer->p = E_text_Z_sl_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p
                                    , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n
                                    );
                                    if( !buffer->p )
                                    {   W(buffer);
                                        goto Error;
                                    }
                                }
                                buffer->n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n;
                                E_aml_S_object[ object_i ].data = buffer;
                                E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                                break;
                            }
                          default:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    goto Error;
                                struct E_aml_Z_package *package;
                                if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                    package = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package;
                                }else
                                {   package = E_aml_Q_object_I_package_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package );
                                    if( !package )
                                        goto Error;
                                }
                                E_aml_S_object[ object_i ].data = package;
                                E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_package;
                                break;
                            }
                        }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_land_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_land_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_land_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lequal_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_string
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_buffer
                    )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lequal_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lequal_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_string
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_buffer
                    )
                        goto Error;
                    struct E_aml_Z_value *e;
                    if( E_aml_M_convert( &e
                    , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy
                    , &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result
                    , &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result
                    ) < 0 )
                    {   W(e);
                        goto Error;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_Q_value_T_cmp_eq( e
                    , &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result
                    );
                    W(e);
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lgreater_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_string
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_buffer
                    )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lgreater_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lgreater_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_string
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_buffer
                    )
                        goto Error;
                    struct E_aml_Z_value *e;
                    if( E_aml_M_convert( &e
                    , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy
                    , &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result
                    , &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result
                    ) < 0 )
                    {   W(e);
                        goto Error;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_Q_value_T_cmp_gt( e
                    , &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result
                    );
                    W(e);
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lless_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_string
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_buffer
                    )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lless_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lless_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_string
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_buffer
                    )
                        goto Error;
                    struct E_aml_Z_value *e;
                    if( E_aml_M_convert( &e
                    , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy
                    , &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result
                    , &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result
                    ) < 0 )
                    {   W(e);
                        goto Error;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_Q_value_T_cmp_lt( e
                    , &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result
                    );
                    W(e);
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mid_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_string
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_buffer
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname
                    )
                        goto Error;
                    N object_i;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( !~object_i
                        || ( E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_string
                          && E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_buffer
                          && E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_package
                        ))
                            goto Error;
                    }
                    struct E_aml_Z_value *Mt_( value, E_aml_S_parse_stack_S_tmp_p_n );
                    if( !value )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p = value;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   switch( E_aml_S_object[ object_i ].type )
                        { case E_aml_Z_object_Z_type_S_string:
                                value[0].p = E_aml_S_object[ object_i ].data;
                                value[0].i = E_aml_S_object[ object_i ].n;
                                value[0].type = E_aml_Z_value_Z_type_S_string;
                                break;
                          default:
                            {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                                value[0].buffer = *buffer;
                                value[0].i = E_aml_S_object[ object_i ].n;
                                value[0].type = E_aml_Z_value_Z_type_S_buffer;
                                break;
                            }
                        }
                        value[0].copy = no;
                    }else
                    {   value[0] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                    }
                    for_n( i, E_aml_S_parse_stack_S_tmp_p_n - 1 )
                        value[ 1 + i ].copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_mid_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mid_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n <= 255
                    )
                        goto Error;
                    struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    N i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                    if( ~i )
                    {   N i_ = value[0].i;
                        switch( value[0].type )
                        { case E_aml_Z_value_Z_type_S_string:
                                if( i_ >= 1 )
                                    goto Error;
                                break;
                          default:
                                if( i_ >= 1 )
                                    goto Error;
                                break;
                        }
                    }
                    value[1].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_mid_finish_3, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mid_finish_3:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n <= 255
                    )
                        goto Error;
                    struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    N i = value[1].n;
                    N l = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                    switch( value[0].type )
                    { case E_aml_Z_value_Z_type_S_string:
                        {   N l_ = E_text_Z_s0_R_l( value[0].p );
                            if( l > l_ )
                                l = l_;
                            break;
                        }
                      default:
                            if( l > value[0].buffer.n )
                                l = value[0].buffer.n;
                            break;
                    }
                    if( i > l )
                        i = l;
                    switch( value[0].type )
                    { case E_aml_Z_value_Z_type_S_string:
                        {   Pc s = M( l - i + 1 );
                            if( !s )
                                goto Error;
                            E_text_Z_s_P_copy_sl_0( s, value[0].buffer.p + i, l - i );
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p = s;
                            break;
                        }
                      default:
                        {   Pc s = M( l - i );
                            if( !s )
                                goto Error;
                            E_mem_Q_blk_I_copy( s, value[0].buffer.p + i, l - i );
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p = s;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n = l - i;
                            break;
                        }
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = value[0].type;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_mid_finish_4, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mid_finish_4:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                        switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type )
                        { case E_aml_Z_value_Z_type_S_string:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    goto Error;
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                Pc s = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p;
                                E_aml_S_object[ object_i ].data = s;
                                E_aml_S_object[ object_i ].n = ~0;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                                break;
                            }
                          default:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    goto Error;
                                struct E_aml_Z_buffer *M_(buffer);
                                if( !buffer )
                                    goto Error;
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                buffer->p = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p;
                                buffer->n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n;
                                E_aml_S_object[ object_i ].data = buffer;
                                E_aml_S_object[ object_i ].n = ~0;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                                break;
                            }
                        }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lnot_finish:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = !E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_load_table_finish_1:
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_load_table_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_load_table_finish_2:
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_load_table_finish_3, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_load_table_finish_3:
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_load_table_finish_4, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_load_table_finish_4:
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_load_table_finish_5, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_load_table_finish_5:
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_load_table_finish_6, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_load_table_finish_6:
                //TODO Umieścić operację ‘load table’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lor_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lor_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lor_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    if( !E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_match_finish_1:
                if( E_aml_S_parse_data + 1 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_package
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname
                    )
                        goto Error;
                    struct E_aml_Z_value *Mt_( value, E_aml_S_parse_stack_S_tmp_p_n );
                    if( !value )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p = value;
                    value[0] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                    value[1].n = *E_aml_S_parse_data;
                    if( value[1].n > 5 )
                        goto Error;
                    for_n( i, E_aml_S_parse_stack_S_tmp_p_n - 1 )
                        value[ 1 + i ].copy = no;
                }
                E_aml_S_parse_data++;
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_match_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_match_finish_2:
                if( E_aml_S_parse_data + 1 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_string
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_buffer
                    )
                        goto Error;
                    struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    value[2] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                    value[3].n = *E_aml_S_parse_data;
                    if( value[3].n > 5 )
                        goto Error;
                }
                E_aml_S_parse_data++;
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_match_finish_3, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_match_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_string
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_buffer
                    )
                        goto Error;
                    struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    value[4] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_match_finish_4, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_match_finish_4:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    N start = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                    if( start >= value[0].package->n )
                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = ~0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                        break;
                    }
                    if( !value[1].n
                    && !value[3].n
                    )
                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = 0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                        break;
                    }
                    N i;
                    for( i = start; i != value[0].package->n; i++ )
                    {   struct E_aml_Z_value *e;
                        N convert = E_aml_M_convert( &e, value[0].copy, &value[0].package->value[i], &value[2] );
                        if( !~convert )
                            goto Error;
                        if( convert == ~0 - 1 )
                            continue;
                        N cmp_1 = E_aml_M_cmp( value[1].n, e, &value[2] );
                        W(e);
                        if( !~cmp_1 )
                            goto Error;
                        if( value[2].type != value[4].type )
                        {   N convert = E_aml_M_convert( &e, value[0].copy, &value[0].package->value[i], &value[2] );
                            if( !~convert )
                                goto Error;
                            if( convert == ~0 - 1 )
                                continue;
                        }
                        N cmp_2 = E_aml_M_cmp( value[3].n, e, &value[4] );
                        W(e);
                        if( !~cmp_2 )
                            goto Error;
                        if( cmp_1
                        && cmp_2
                        )
                        {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = i;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                            break;
                        }
                    }
                    if( i == value[0].package->n )
                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = ~0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mod_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_mod_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mod_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    || !E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n
                    )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n %= E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_mod_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mod_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_multiply_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_multiply_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_multiply_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n *= E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_multiply_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_multiply_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_nand_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_nand_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_nand_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = ~( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n & E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n );
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_nand_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_nand_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_nor_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_nor_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_nor_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = ~( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n | E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n );
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_nor_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_nor_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_not_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = ~E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_not_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_not_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_object_type_finish:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N object_i = E_aml_M_clear_object();
                    if( !~object_i )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_S_object[ object_i ].type;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_or_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_or_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_or_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n |= E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_or_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_or_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_refof_finish:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( !~object_i )
                            goto Error;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n <<= E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_shift_right_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_shift_right_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n >>= E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_shift_right_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_shift_right_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_sizeof_finish:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {  if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_arg_ref )
                    {   if( !E_aml_S_procedure_invocation_stack_n )
                            goto Error;
                        N arg_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
                        if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].type != E_aml_Z_value_Z_type_S_pathname )
                            goto Error;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ];
                    }else if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_local_ref )
                    {   if( !E_aml_S_procedure_invocation_stack_n )
                            goto Error;
                        N local_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
                        if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].type != E_aml_Z_value_Z_type_S_pathname )
                            goto Error;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ];
                    }else if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_debug_ref )
                        break;
                    N object_i = E_aml_M_clear_object();
                    if( !~object_i
                    || ( E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_string
                      && E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_buffer
                      && E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_package
                    ))
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_Q_object_R_l( object_i );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_store_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_store_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_store_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   //NDFN Tutaj powinna być konwersja typu.
                        N i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i;
                        if( ~i
                        && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_package
                        )
                        {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package->value[i];
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                            i = ~0;
                        }
                        if( !~i )
                            switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type )
                            { case E_aml_Z_value_Z_type_S_number:
                                {   N object_i = E_aml_M_clear_object();
                                    if( !~object_i )
                                        goto Error;
                                    E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                                    break;
                                }
                              case E_aml_Z_value_Z_type_S_string:
                                {   N object_i = E_aml_M_clear_object();
                                    if( !~object_i )
                                        goto Error;
                                    Pc s;
                                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                        s = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p;
                                    }else
                                    {   s = E_text_Z_s0_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p );
                                        if( !s )
                                            goto Error;
                                    }
                                    E_aml_S_object[ object_i ].data = s;
                                    E_aml_S_object[ object_i ].n = ~0;
                                    E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                                    break;
                                }
                              case E_aml_Z_value_Z_type_S_buffer:
                                {   N object_i = E_aml_M_clear_object();
                                    if( !~object_i )
                                        goto Error;
                                    struct E_aml_Z_buffer *M_(buffer);
                                    if( !buffer )
                                        goto Error;
                                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                        buffer->p = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p;
                                    }else
                                    {   buffer->p = E_text_Z_sl_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p
                                        , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n
                                        );
                                        if( !buffer->p )
                                        {   W(buffer);
                                            goto Error;
                                        }
                                    }
                                    buffer->n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n;
                                    E_aml_S_object[ object_i ].data = buffer;
                                    E_aml_S_object[ object_i ].n = ~0;
                                    E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                                    break;
                                }
                              case E_aml_Z_value_Z_type_S_package:
                                {   N object_i = E_aml_M_clear_object();
                                    if( !~object_i )
                                        goto Error;
                                    struct E_aml_Z_package *package;
                                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                        package = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package;
                                    }else
                                    {   package = E_aml_Q_object_I_package_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package );
                                        if( !package )
                                            goto Error;
                                    }
                                    E_aml_S_object[ object_i ].data = package;
                                    E_aml_S_object[ object_i ].n = ~0;
                                    E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_package;
                                    break;
                                }
                              default:
                                    goto Error;
                            }
                        else
                            switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type )
                            { case E_aml_Z_value_Z_type_S_string:
                                {   N object_i = E_aml_M_clear_object();
                                    if( !~object_i )
                                        goto Error;
                                    Pc s = M( 1 + 1 );
                                    if( !s )
                                        goto Error;
                                    Pc s_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p;
                                    s[0] = s_[i];
                                    s[1] = '\0';
                                    E_aml_S_object[ object_i ].data = s;
                                    E_aml_S_object[ object_i ].n = ~0;
                                    E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                                    break;
                                }
                              case E_aml_Z_value_Z_type_S_buffer:
                                {   N object_i = E_aml_M_clear_object();
                                    if( !~object_i )
                                        goto Error;
                                    struct E_aml_Z_buffer *M_(buffer);
                                    if( !buffer )
                                        goto Error;
                                    buffer->p = M(1);
                                    if( !buffer->p )
                                    {   W(buffer);
                                        goto Error;
                                    }
                                    buffer->n = 1;
                                    buffer->p[0] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p[i];
                                    E_aml_S_object[ object_i ].data = buffer;
                                    E_aml_S_object[ object_i ].n = ~0;
                                    E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                                    break;
                              default:
                                    goto Error;
                                }
                            }
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_subtract_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_subtract_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_subtract_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n -= E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_subtract_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_subtract_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_bcd_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    N l = E_text_Z_n_N_bcd_G( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n
                    , sizeof( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n )
                    );
                    if( l > sizeof( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n ))
                        goto Error;
                    E_text_Z_n_N_bcd( (Pc)&E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n
                      + sizeof( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n )
                    , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n
                    , sizeof( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n )
                    , l
                    );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_bcd_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_bcd_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_buffer_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_string
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_buffer
                    )
                        goto Error;
                    if( E_aml_Q_value_N_convert( E_aml_Z_value_Z_type_S_number, &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result ))
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_buffer_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_buffer_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        struct E_aml_Z_buffer *M_(buffer);
                        if( !buffer )
                            goto Error;
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                        {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                            buffer->p = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p;
                        }else
                        {   buffer->p = E_text_Z_sl_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p
                            , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n
                            );
                            if( !buffer->p )
                            {   W(buffer);
                                goto Error;
                            }
                        }
                        buffer->n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n;
                        E_aml_S_object[ object_i ].data = buffer;
                        E_aml_S_object[ object_i ].n = ~0;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_decimal_string_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type )
                    { case E_aml_Z_value_Z_type_S_number:
                            if( E_aml_Q_value_N_convert( E_aml_Z_value_Z_type_S_string, &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result ) < 0 )
                                goto Error;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                            break;
                      case E_aml_Z_value_Z_type_S_string:
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                            break;
                      case E_aml_Z_value_Z_type_S_buffer:
                        {   Pc s = M(0);
                            if( !s )
                                goto Error;
                            N l = 0;
                            for_n( i, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.buffer.n )
                            {   N l_ = E_text_Z_n_N_s_G( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.buffer.p[i], sizeof( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.p[i] ), 10 );
                                if( !E_mem_Q_blk_I_append( &s, l_ + 1 ))
                                {   W(s);
                                    goto Error;
                                }
                                l += l_;
                                s[l] = ',';
                                E_text_Z_n_N_s( s + l, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.buffer.p[i], sizeof( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.buffer.p[i] ), 10 );
                                l++;
                            }
                            if( !l )
                            {   if( !E_mem_Q_blk_I_append( &s, 1 ))
                                {   W(s);
                                    goto Error;
                                }
                                l++;
                            }
                            *( s + l - 1 ) = '\0';
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_string;
                            break;
                        }
                      default:
                            goto Error;
                    }
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_decimal_string_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_decimal_string_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                        {   E_aml_S_object[ object_i ].data = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                        }else
                        {   Pc s = E_text_Z_s0_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p );
                            if( !s )
                                goto Error;
                            E_aml_S_object[ object_i ].data = s;
                        }
                        E_aml_S_object[ object_i ].n = ~0;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_hex_string_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type )
                    { case E_aml_Z_value_Z_type_S_number:
                        {   N l = E_text_Z_n_N_s_G( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n, sizeof( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n ), 16 );
                            Pc s = M( 2 + l + 1 );
                            if( !s )
                                goto Error;
                            s[ 2 + l ] = '\0';
                            E_text_Z_n_N_s( s + 2 + l, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n, sizeof( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n ), 16 );
                            s[1] = 'x';
                            s[0] = '0';
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p = s;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_string;
                            break;
                        }
                      case E_aml_Z_value_Z_type_S_string:
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                            break;
                      case E_aml_Z_value_Z_type_S_buffer:
                        {   Pc s = M(0);
                            if( !s )
                                goto Error;
                            N l = 0;
                            for_n( i, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.buffer.n )
                            {   N l_ = E_text_Z_n_N_s_G( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.buffer.p[i], sizeof( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.p[i] ), 16 );
                                if( !E_mem_Q_blk_I_append( &s, l_ + 1 ))
                                {   W(s);
                                    goto Error;
                                }
                                l += l_;
                                s[l] = ',';
                                E_text_Z_n_N_s( s + l, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.buffer.p[i], sizeof( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.buffer.p[i] ), 16 );
                                l++;
                            }
                            if( !l )
                            {   if( !E_mem_Q_blk_I_append( &s, 1 ))
                                {   W(s);
                                    goto Error;
                                }
                                l++;
                            }
                            *( s + l - 1 ) = '\0';
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_string;
                            break;
                        }
                      default:
                            goto Error;
                    }
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_hex_string_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_hex_string_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                        {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                            E_aml_S_object[ object_i ].data = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p;
                        }else
                        {   Pc s = E_text_Z_s0_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p );
                            if( !s )
                                goto Error;
                            E_aml_S_object[ object_i ].data = s;
                        }
                        E_aml_S_object[ object_i ].n = ~0;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_integer_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_string
                    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_buffer
                    )
                        goto Error;
                    if( E_aml_Q_value_N_convert( E_aml_Z_value_Z_type_S_number, &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result ))
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_integer_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_integer_finish_2:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_string_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_buffer )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_string_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_string_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    Pc p = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p;
                    N l = J_min( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n );
                    Pc s = E_text_Z_sl_R_search_0( p, l );
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                        if( s != p + l )
                        {   if( !E_mem_Q_blk_I_remove( &p, s + 1 - p, l - ( s + 1 - p )))
                                return ~0;
                        }else
                        {   if( !E_mem_Q_blk_I_append( &p, 1 ))
                                return ~0;
                            p[l] = '\0';
                        }
                    else
                    {   Pc p_ = M( s - p + 1 );
                        if( !p_ )
                            goto Error;
                        E_text_Z_s_P_copy_sl_0( p_, p, s - p );
                        p = p_;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p = p;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_string;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_string_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_string_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].data = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p;
                        E_aml_S_object[ object_i ].n = ~0;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_wait_finish_1:
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_wait_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_wait_finish_2:
                //TODO Umieścić operację ‘wait’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_xor_finish_1:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_xor_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_xor_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~0 - 1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number )
                        goto Error;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n ^= E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_xor_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_xor_finish_3:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                        goto Error;
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            goto Error;
                        E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_fatal_finish:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                    goto Error;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_load_finish:
                //TODO Umieścić operację ‘load’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_procedure_finish:
                E_aml_Q_current_path_P_precompilation(no);
                if( !~E_aml_Q_current_path_I_pop() )
                    goto Error;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish:
                if( E_aml_Q_current_path_S_precompilation_i )
                {
                }else
                {   for_n( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].arg ))
                    {   if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].arg[i].type == E_aml_Z_value_Z_type_S_uninitialized )
                            break;
                        if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].arg[i].copy )
                            E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].arg[i] );
                    }
                    for_n_( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].local ))
                        if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].local[i].type != E_aml_Z_value_Z_type_S_uninitialized
                        && E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].local[i].copy
                        )
                            E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].local[i] );
                    if( !E_mem_Q_blk_I_remove( &E_aml_S_procedure_invocation_stack, --E_aml_S_procedure_invocation_stack_n, 1 ))
                        goto Error;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.procedure_osi )
                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = no; //TODO W miarę dodawania obsługi funkcjonalności ACPI umieszczać sprawdzenia.
                    }
                }
                break;
        }
        if( !~ret )
            goto Error;
        if( ret == ~0 - 1 )
        {   if( !E_aml_M_res1() )
            {   ret = 0;
                goto Loop;
            }
            goto Error;
        }
        //E_font_I_print( "\nstack_n=" ); E_font_I_print_hex( E_aml_S_parse_stack_n );
        //E_font_I_print( ",stack_n_last=" ); E_font_I_print_hex( stack_n_last );
        //for_n( j, E_aml_S_parse_stack_n )
        //{   E_font_I_print( "\n-entity=" ); E_font_I_print_hex( E_aml_S_parse_stack[j].entity );
            //E_font_I_print( ",n=" ); E_font_I_print_hex( E_aml_S_parse_stack[j].n );
            //E_font_I_print( ",data_end=" ); E_font_I_print_hex( (N)E_aml_S_parse_stack[j].data_end );
        //}
        // Usunięcie wykananego “entity” po ‘push’.
        if( stack_n_last != E_aml_S_parse_stack_n
        && E_aml_S_parse_stack[ stack_n_last - 1 ].data_end // Czy element na stosie nie jest nie zainicjowanym którymś “finish”.
        && ~E_aml_S_parse_stack[ stack_n_last - 1 ].n
        && !E_aml_S_parse_stack[ stack_n_last - 1 ].n--
        )
        {   if( !E_mem_Q_blk_I_remove( &E_aml_S_parse_stack, stack_n_last - 1, 1 ))
                goto Error;
            E_aml_S_parse_stack_n--;
        }
        //for_n_( j, E_aml_S_parse_stack_n )
        //{   E_font_I_print( "\n-entity=" ); E_font_I_print_hex( E_aml_S_parse_stack[j].entity );
            //E_font_I_print( ",n=" ); E_font_I_print_hex( E_aml_S_parse_stack[j].n );
            //E_font_I_print( ",data_end=" ); E_font_I_print_hex( (N)E_aml_S_parse_stack[j].data_end );
        //}
        if( E_aml_S_parse_stack_n >= 2 )
        {   if( ~E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n // Czy bieżący element nie jest listą wyliczaną w nieskończoność.
            && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].data_end // Czy poprzedni element na stosie nie jest nie zainicjowanym którymś “finish”.
            && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end != E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].data_end // Czy było ustawione “pkg_length”.
            && E_aml_S_parse_data != E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end 
            ) // Sprawdzanie dla konkretnych “n”, czy interpretacja zakończyła się na końcu wcześniej odczytanego rozmiaru “pkg_length”.
            {   if( !E_aml_M_res1() )
                    goto Loop;
                goto Error;
            }
            if( !E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].data_end ) // Czy poprzedni element na stosie jest nie zainicjowanym którymś “finish”.
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].data_end = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end; // Zachowaj “data_end” do sprawdzenia podczas któregoś “finish”.
        }
        if( ~E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n
        && !E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n--
        )
        {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy )
                E_aml_Q_value_W( &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result );
            if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p )
            {   for_n( i, E_aml_S_parse_stack_S_tmp_p_n )
                {   struct E_aml_Z_value *value = &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p[i];
                    if( value->copy )
                        E_aml_Q_value_W(value);
                }
                W( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p );
            }
            if( !E_mem_Q_blk_I_remove( &E_aml_S_parse_stack, --E_aml_S_parse_stack_n, 1 ))
                goto Error;
        }
        if( !~E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n )
        {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy )
            {   E_aml_Q_value_W( &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result );
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
            }
            if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p )
            {   for_n( i, E_aml_S_parse_stack_S_tmp_p_n )
                {   struct E_aml_Z_value *value = &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p[i];
                    if( value->copy )
                        E_aml_Q_value_W(value);
                }
                W( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p );
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p = 0;
            }
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end ) // Sprawdzanie dla listy wyliczanej w nieskończoność, czy interpretacja zakończyła się.
            {   if( !--E_aml_S_parse_stack_n )
                    break;
                if( !E_mem_Q_blk_I_remove( &E_aml_S_parse_stack, E_aml_S_parse_stack_n, 1 ))
                    goto Error;
            }
        }
        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if )
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if--;
        //for_n_( j, E_aml_S_parse_stack_n )
        //{   E_font_I_print( "\n-entity=" ); E_font_I_print_hex( E_aml_S_parse_stack[j].entity );
            //E_font_I_print( ",n=" ); E_font_I_print_hex( E_aml_S_parse_stack[j].n );
            //E_font_I_print( ",data_end=" ); E_font_I_print_hex( (N)E_aml_S_parse_stack[j].data_end );
        //}
        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n > 1 )
        {   if( data == E_aml_S_parse_data ) // Czy kolejne “n” nic nie wykonują.
            {   if( !E_aml_M_res1() )
                    goto Loop;
                goto Error;
            }
            data = E_aml_S_parse_data;
        }
    }
    W( E_aml_S_procedure_invocation_stack );
    W( E_aml_S_parse_stack );
    E_aml_Q_current_path_W();
    return 0;
Error:
    E_aml_Q_object_W();
    for_n( i, E_aml_S_procedure_invocation_stack_n )
    {   for_n( j, J_a_R_n( E_aml_S_procedure_invocation_stack[i].arg ))
            if( E_aml_S_procedure_invocation_stack[i].arg[j].type != E_aml_Z_value_Z_type_S_uninitialized
            && E_aml_S_procedure_invocation_stack[i].arg[j].copy
            )
                E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[i].arg[j] );
        for_n_( j, J_a_R_n( E_aml_S_procedure_invocation_stack[i].local ))
            if( E_aml_S_procedure_invocation_stack[i].local[j].type != E_aml_Z_value_Z_type_S_uninitialized
            && E_aml_S_procedure_invocation_stack[i].local[j].copy
            )
                E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[i].local[j] );
    }
    W( E_aml_S_procedure_invocation_stack );
    for_n_( i, E_aml_S_parse_stack_n )
    {   if( E_aml_S_parse_stack[i].execution_context.result.copy )
            E_aml_Q_value_W( &E_aml_S_parse_stack[i].execution_context.result );
        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p )
        {   for_n( i, E_aml_S_parse_stack_S_tmp_p_n )
            {   struct E_aml_Z_value *value = &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p[i];
                if( value->copy )
                    E_aml_Q_value_W(value);
            }
            W( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p );
        }
    }
    W( E_aml_S_parse_stack );
    E_aml_Q_current_path_W();
    E_font_I_print( "\nerror" );
    return ~0;
}
/******************************************************************************/
