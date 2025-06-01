/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         AML interpreter
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”             2025‒5‒13 I
*******************************************************************************/
//DFN W tym interpretatorze AML wartości albo są liczbowe, albo są referencjami do obiektów. Natomiast wartość może być kopią, jeśli powstała z algorytmu przetwarzania. Dopiero podczas przypisywania do obiektu wartości są kopiowane, jeśli nie były kopiami. Tzn. operator ASL “DerefOf” działa z opóźnieniem, a “RefOf” przenosi wartość.
//TODO Czy da się nie obliczać drugiego argumentu dla instrukcji “LOr”, jeśli nie potrzeba?
//NDFN Co robić z aliasami do aliasów?
#include "main.h"
//==============================================================================
struct E_aml_Z_pathname
{ Pc s;
  N8 n;
};
_internal struct E_aml_Z_pathname *E_aml_S_current_path;
_internal N E_aml_S_current_path_n;
_internal B E_aml_Q_current_path_S_precompilation;
//------------------------------------------------------------------------------
struct E_aml_Z_buffer
{ Pc p;
  N n;
};
struct E_aml_Z_buffer_field
{ struct E_aml_Z_pathname pathname;
  N i;
  N u;
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
  N i; // Jeśli “type == E_aml_Z_value_Z_type_S_*_ref”, to przechowuje numer zmiennej, w przeciwnym przypadku jeśli wartość “!~0”, to jest indeksem obiektu.
  enum E_aml_Z_value_Z_type type;
  B copy;
};
struct E_aml_Z_package
{ struct E_aml_Z_value *value;
  N n;
};
//------------------------------------------------------------------------------
_internal
struct
{ struct E_aml_Z_pathname name;
  struct E_aml_Z_value arg[7], local[8], result;
  Pc return_;
  N current_path_n;
  B computing_arg;
} *E_aml_S_procedure_invocation_stack;
_internal
N E_aml_S_procedure_invocation_stack_n;
_internal B E_aml_Q_procedure_invocation_stack_S_invokeing;
//------------------------------------------------------------------------------
enum E_aml_Z_field_access
{ E_aml_Z_field_access_S_any
, E_aml_Z_field_access_S_1
, E_aml_Z_field_access_S_2
, E_aml_Z_field_access_S_4
, E_aml_Z_field_access_S_8
, E_aml_Z_field_access_S_buffer
};
enum E_aml_Z_field_Z_type
{ E_aml_Z_field_Z_type_S_plain
, E_aml_Z_field_Z_type_S_bank
//NDFN
, E_aml_Z_field_Z_type_S_access
, E_aml_Z_field_Z_type_S_ex_access
, E_aml_Z_field_Z_type_S_connect
};
struct E_aml_Z_field_unit
{ struct E_aml_Z_pathname region;
  struct E_aml_Z_pathname bank_field;
  N bank_value;
  N i;
  N n;
  unsigned access       :3;
  unsigned lock         :1;
  unsigned update_rule  :2;
  enum E_aml_Z_field_Z_type type;
};
struct E_aml_Z_object_data_Z_procedure
{ Pc data, data_end;
  struct E_aml_Z_value (*procedure)(void);
  N8 arg_n;
};
enum E_aml_Z_op_region_space
{ E_aml_Z_op_region_space_S_system_memory
, E_aml_Z_op_region_space_S_system_io
, E_aml_Z_op_region_space_S_pci_config
, E_aml_Z_op_region_space_S_embedded_control
, E_aml_Z_op_region_space_S_smbus
, E_aml_Z_op_region_space_S_system_cmos
, E_aml_Z_op_region_space_S_pci_bar_target
, E_aml_Z_op_region_space_S_ipmi
, E_aml_Z_op_region_space_S_gpio
, E_aml_Z_op_region_space_S_gsb
, E_aml_Z_op_region_space_S_pcc
};
struct E_aml_Z_op_region
{ struct E_aml_Z_pathname region;
  N i;
  N l;
  enum E_aml_Z_op_region_space space;
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
, E_aml_Z_object_Z_type_S_processor
, E_aml_Z_object_Z_type_S_thermal_zone
, E_aml_Z_object_Z_type_S_buffer_field
, E_aml_Z_object_Z_type_S_reserved
, E_aml_Z_object_Z_type_S_debug
, E_aml_Z_object_Z_type_S_alias
//TODO Poniższe typy nie są obiektami?
, E_aml_Z_object_Z_type_S_data_table_region
, E_aml_Z_object_Z_type_S_index_field
};
_internal
struct
{ struct E_aml_Z_pathname name; // Tablica posortowana według nazwy.
  P data;
  N n; // Jeśli “type” równe “E_aml_Z_value_Z_type_S_string”, “E_aml_Z_value_Z_type_S_buffer” lub “E_aml_Z_value_Z_type_S_package”, to jest indeksem elementu obiektu; “~0” oznacza brak referencji do elementu.
  N procedure_invocation; // Jeśli “!0”, to obiekt jest obiektem tymczasowym podczas wykonywania procedury umieszczonej na “E_aml_S_procedure_invocation_stack[ procedure_invocation ]”.
  enum E_aml_S_object_Z_type type;
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
, E_aml_Z_parse_stack_Z_entity_S_lless_finish_1
, E_aml_Z_parse_stack_Z_entity_S_lless_finish_2
, E_aml_Z_parse_stack_Z_entity_S_lnotequal_finish_1
, E_aml_Z_parse_stack_Z_entity_S_lnotequal_finish_2
, E_aml_Z_parse_stack_Z_entity_S_llessequal_finish_1
, E_aml_Z_parse_stack_Z_entity_S_llessequal_finish_2
, E_aml_Z_parse_stack_Z_entity_S_lgreaterequal_finish_1
, E_aml_Z_parse_stack_Z_entity_S_lgreaterequal_finish_2
, E_aml_Z_parse_stack_Z_entity_S_lnot_finish
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
, E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish_1
, E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish_2
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
){  if( !value->copy )
        return;
    switch( value->type )
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
            if( !~value->i )
                break;
            switch(type)
            { case E_aml_Z_value_Z_type_S_number:
                {   if( !*(Pc)value->p )
                        break;
                    Pc s;
                    N n = E_text_Z_s0_N_n( value->p, &s, 10 );
                    if( *s )
                    {   if( s == value->p + 1
                        && *( s - 1 ) == '0'
                        && ( *s == 'x'
                          || *s == 'X'
                        ))
                        {   n = E_text_Z_s0_N_n( s + 1, &s, 16 );
                            if( *s )
                                break;
                        }else
                            break;
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
            if( !~value->i )
                break;
            switch(type)
            { case E_aml_Z_value_Z_type_S_number:
                {   if( !value->buffer.n )
                        break;
                    if( value->buffer.n < sizeof( value->n ))
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
    return ~1;
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
E_aml_E_current_path_M( void
){  E_aml_S_current_path_n = 0;
    Mt_( E_aml_S_current_path, E_aml_S_current_path_n );
    if( !E_aml_S_current_path )
        return ~0;
    E_aml_Q_current_path_S_precompilation = 0;
    return 0;
}
_internal
void
E_aml_E_current_path_W( void
){  for_n( i, E_aml_S_current_path_n )
        W( E_aml_S_current_path[i].s );
    W( E_aml_S_current_path );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
struct E_aml_Z_value
E_aml_I_procedure_I_osi( void
){  struct E_aml_Z_value ret;
    ret.n = no;
    ret.type = E_aml_Z_value_Z_type_S_number;
    return ret;
}
//==============================================================================
_internal
void
E_aml_Q_object_W_package( struct E_aml_Z_package *package
){  for_n( i, package->n )
        if( package->value[i].copy )
            switch( package->value[i].type )
            { case E_aml_Z_value_Z_type_S_string:
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
N
E_aml_Q_object_W_data( N object_i
){  if( E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_uninitialized
    && E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_number
    && E_aml_S_object[ object_i ].data
    )
    {   switch( E_aml_S_object[ object_i ].type )
        { case E_aml_Z_object_Z_type_S_buffer:
            {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                W( buffer->p );
                break;
            }
          case E_aml_Z_object_Z_type_S_package:
                E_aml_Q_object_W_package( E_aml_S_object[ object_i ].data );
                break;
          case E_aml_Z_object_Z_type_S_op_region:
            {   N i;
                for( i = object_i + 1; i != E_aml_S_object_n; i++ )
                {   if( E_aml_S_object[i].type != E_aml_Z_object_Z_type_S_field_unit )
                        break;
                    struct E_aml_Z_field_unit *field = E_aml_S_object[i].data;
                    if( field->region.s != E_aml_S_object[ object_i ].name.s )
                        break;
                    W( E_aml_S_object[i].name.s );
                }
                if( !E_mem_Q_blk_I_remove( &E_aml_S_object, object_i + 1, i - ( object_i + 1 )))
                    return ~0;
                E_aml_S_object_n -= i - ( object_i + 1 );
                break;
            }
          case E_aml_Z_object_Z_type_S_buffer_field:
            {   struct E_aml_Z_buffer_field *field = E_aml_S_object[ object_i ].data;
                W( field->pathname.s );
                break;
            }
        }
        W( E_aml_S_object[ object_i ].data );
    }
    return 0;
}
_internal
N
E_aml_Q_object_W( N object_i
){  for_n( i, E_aml_S_object_n )
        if( E_aml_S_object[i].type == E_aml_Z_object_Z_type_S_alias )
        {   struct E_aml_Z_pathname *pathname = E_aml_S_object[i].data;
            if( pathname->s == E_aml_S_object[ object_i ].name.s )
            {   if( !E_mem_Q_blk_I_remove( &E_aml_S_object, i, 1 ))
                    return ~0;
                if( object_i > i )
                    object_i--;
                i--;
            }
        }
    if( !~E_aml_Q_object_W_data( object_i ))
        return ~0;
    if( !E_mem_Q_blk_I_remove( &E_aml_S_object, object_i, 1 ))
        return ~0;
    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
N
E_aml_E_object_M( void
){  E_aml_S_object_n = 7;
    Mt_( E_aml_S_object, E_aml_S_object_n );
    if( !E_aml_S_object )
        return ~0;
    Pc name = E_text_Z_sl_M_duplicate( "_GL_", 4 );
    if( !name )
    {   W( E_aml_S_object );
        return ~0;
    }
    E_aml_S_object[0].name.s = name;
    E_aml_S_object[0].name.n = 1;
    E_aml_S_object[0].data = 0;
    E_aml_S_object[0].procedure_invocation = 0;
    E_aml_S_object[0].type = E_aml_Z_object_Z_type_S_mutex;
    name = E_text_Z_sl_M_duplicate( "_GPE", 4 );
    if( !name )
    {   W( E_aml_S_object[0].name.s );
        W( E_aml_S_object );
        return ~0;
    }
    E_aml_S_object[1].name.s = name;
    E_aml_S_object[1].name.n = 1;
    E_aml_S_object[1].data = 0;
    E_aml_S_object[1].procedure_invocation = 0;
    E_aml_S_object[1].type = E_aml_Z_object_Z_type_S_uninitialized;
    name = E_text_Z_sl_M_duplicate( "_OS_", 4 );
    if( !name )
    {   W( E_aml_S_object[1].name.s );
        W( E_aml_S_object[0].name.s );
        W( E_aml_S_object );
        return ~0;
    }
    E_aml_S_object[2].name.s = name;
    E_aml_S_object[2].name.n = 1;
    E_aml_S_object[2].data = E_text_Z_s0_M_duplicate( "OUX/C+ OS" );
    if( !E_aml_S_object[2].data )
    {   W( E_aml_S_object[2].name.s );
        W( E_aml_S_object[1].name.s );
        W( E_aml_S_object[0].name.s );
        W( E_aml_S_object );
        return ~0;
    }
    E_aml_S_object[2].n = ~0;
    E_aml_S_object[2].procedure_invocation = 0;
    E_aml_S_object[2].type = E_aml_Z_object_Z_type_S_string;
    name = E_text_Z_sl_M_duplicate( "_OSI", 4 );
    if( !name )
    {   E_aml_Q_object_W_data(2);
        W( E_aml_S_object[2].name.s );
        W( E_aml_S_object[1].name.s );
        W( E_aml_S_object[0].name.s );
        W( E_aml_S_object );
        return ~0;
    }
    E_aml_S_object[3].name.s = name;
    E_aml_S_object[3].name.n = 1;
    struct E_aml_Z_object_data_Z_procedure *M_(procedure);
    if( !procedure )
    {   W( E_aml_S_object[3].name.s );
        E_aml_Q_object_W_data(2);
        W( E_aml_S_object[2].name.s );
        W( E_aml_S_object[1].name.s );
        W( E_aml_S_object[0].name.s );
        W( E_aml_S_object );
        return ~0;
    }
    procedure->arg_n = 1;
    procedure->procedure = &E_aml_I_procedure_I_osi;
    E_aml_S_object[3].data = procedure;
    E_aml_S_object[3].procedure_invocation = 0;
    E_aml_S_object[3].type = E_aml_Z_object_Z_type_S_procedure;
    name = E_text_Z_sl_M_duplicate( "_REV", 4 );
    if( !name )
    {   E_aml_Q_object_W_data(3);
        W( E_aml_S_object[3].name.s );
        E_aml_Q_object_W_data(2);
        W( E_aml_S_object[2].name.s );
        W( E_aml_S_object[1].name.s );
        W( E_aml_S_object[0].name.s );
        W( E_aml_S_object );
        return ~0;
    }
    E_aml_S_object[4].name.s = name;
    E_aml_S_object[4].name.n = 1;
    E_aml_S_object[4].n = 2;
    E_aml_S_object[4].data = 0;
    E_aml_S_object[4].procedure_invocation = 0;
    E_aml_S_object[4].type = E_aml_Z_object_Z_type_S_number;
    name = E_text_Z_sl_M_duplicate( "_SB_", 4 );
    if( !name )
    {   W( E_aml_S_object[4].name.s );
        E_aml_Q_object_W_data(3);
        W( E_aml_S_object[3].name.s );
        E_aml_Q_object_W_data(2);
        W( E_aml_S_object[2].name.s );
        W( E_aml_S_object[1].name.s );
        W( E_aml_S_object[0].name.s );
        W( E_aml_S_object );
        return ~0;
    }
    E_aml_S_object[5].name.s = name;
    E_aml_S_object[5].name.n = 1;
    E_aml_S_object[5].data = 0;
    E_aml_S_object[5].procedure_invocation = 0;
    E_aml_S_object[5].type = E_aml_Z_object_Z_type_S_uninitialized;
    name = E_text_Z_sl_M_duplicate( "_SI_", 4 );
    if( !name )
    {   W( E_aml_S_object[5].name.s );
        W( E_aml_S_object[4].name.s );
        E_aml_Q_object_W_data(3);
        W( E_aml_S_object[3].name.s );
        E_aml_Q_object_W_data(2);
        W( E_aml_S_object[2].name.s );
        W( E_aml_S_object[1].name.s );
        W( E_aml_S_object[0].name.s );
        W( E_aml_S_object );
        return ~0;
    }
    E_aml_S_object[6].name.s = name;
    E_aml_S_object[6].name.n = 1;
    E_aml_S_object[6].data = 0;
    E_aml_S_object[6].procedure_invocation = 0;
    E_aml_S_object[6].type = E_aml_Z_object_Z_type_S_uninitialized;
    return 0;
}
_internal
void
E_aml_E_object_W( void
){  for_n( i, E_aml_S_object_n )
    {   E_aml_Q_object_W_data(i);
        W( E_aml_S_object[i].name.s );
    }
    W( E_aml_S_object );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_internal
Pc
E_aml_Q_object_N( N object_i
){  switch( E_aml_S_object[ object_i ].type )
    { case E_aml_Z_object_Z_type_S_uninitialized:
            return "[Uninitialized Object]";
      case E_aml_Z_object_Z_type_S_package:
            return "[Package]";
      case E_aml_Z_object_Z_type_S_field_unit:
            return "[Field]";
      case E_aml_Z_object_Z_type_S_device:
            return "[Device]";
      case E_aml_Z_object_Z_type_S_event:
            return "[Event]";
      case E_aml_Z_object_Z_type_S_procedure:
            return "[Control Method]";
      case E_aml_Z_object_Z_type_S_mutex:
            return "[Mutex]";
      case E_aml_Z_object_Z_type_S_op_region:
            return "[Operation Region]";
      case E_aml_Z_object_Z_type_S_power_res:
            return "[Power Resource]";
      case E_aml_Z_object_Z_type_S_processor:
            return "[Processor]";
      case E_aml_Z_object_Z_type_S_thermal_zone:
            return "[Thermal Zone]";
      case E_aml_Z_object_Z_type_S_buffer_field:
            return "[Buffer Field]";
      case E_aml_Z_object_Z_type_S_debug:
            return "[Debug Object]";
    }
    return "";
}
_internal
struct E_aml_Z_value
E_aml_Q_object_I_to_value( N object_i
){  struct E_aml_Z_value v;
    switch( E_aml_S_object[ object_i ].type )
    { case E_aml_Z_object_Z_type_S_number:
            v.n = E_aml_S_object[ object_i ].n;
            v.type = E_aml_Z_value_Z_type_S_number;
            break;
      case E_aml_Z_object_Z_type_S_string:
            v.p = E_aml_S_object[ object_i ].data;
            v.i = E_aml_S_object[ object_i ].n;
            v.type = E_aml_Z_value_Z_type_S_string;
            break;
      case E_aml_Z_object_Z_type_S_buffer:
        {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
            v.buffer.p = buffer->p;
            v.buffer.n = buffer->n;
            v.i = E_aml_S_object[ object_i ].n;
            v.type = E_aml_Z_value_Z_type_S_buffer;
            break;
        }
      case E_aml_Z_object_Z_type_S_package:
        {   v.package = E_aml_S_object[ object_i ].data;
            v.i = E_aml_S_object[ object_i ].n;
            v.type = E_aml_Z_value_Z_type_S_package;
            break;
        }
    }
    v.copy = no;
    return v;
}
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
                ret->value[i].copy = yes;
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
                ret->value[i].copy = yes;
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
                ret->value[i].copy = yes;
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
                return ~1;
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
    E_aml_S_object[middle].procedure_invocation = E_aml_S_procedure_invocation_stack_n;
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
Pc
E_aml_Q_path_R_segment_( Pc pkg_end
){  if( E_aml_S_parse_data + 4 > pkg_end )
        return (Pc)~0;
    if( !( E_aml_S_parse_data[0] == '_'
    || ( E_aml_S_parse_data[0] >= 'A'
      && E_aml_S_parse_data[0] <= 'Z'
    )))
        return (Pc)~1;
    for_n( j, 3 )
        if( !( E_aml_S_parse_data[ 1 + j ] == '_'
        || ( E_aml_S_parse_data[ 1 + j ] >= '0'
          && E_aml_S_parse_data[ 1 + j ] <= '9'
        )
        || ( E_aml_S_parse_data[ 1 + j ] >= 'A'
          && E_aml_S_parse_data[ 1 + j ] <= 'Z'
        )))
            return (Pc)~1;
    Pc s = M(4);
    if( !s )
        return (Pc)~0;
    E_mem_Q_blk_I_copy( s, E_aml_S_parse_data, 4 );
    E_aml_S_parse_data += 4;
    return s;
}
_internal
Pc
E_aml_Q_path_R_segment( void
){  return E_aml_Q_path_R_segment_( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end );
}
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
            {   *n = ~1;
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
            if( !E_aml_S_current_path_n )
            {   *n = ~1;
                return 0;
            }
            do
            {   if( ++E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end
                || ++parent_n > E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n
                )
                {   *n = ~1;
                    return 0;
                }
            }while( *E_aml_S_parse_data == '^' );
      default:
        {   *n = E_aml_Q_path_R_pathname();
            if( !~(S8)*n )
            {   *n = ~1;
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
    switch( *E_aml_S_parse_data )
    { case '\\':
        {   E_aml_S_parse_data++;
            *n = E_aml_Q_path_R_pathname();
            if( !~(S8)*n
            || !*n
            )
            {   *n = ~1;
                return 0;
            }
            Pc s = M( *n * 4 );
            if( !s )
            {   *n = ~0;
                return 0;
            }
            E_mem_Q_blk_I_copy( s, E_aml_S_parse_data - *n * 4, *n * 4 );
            *object_i = E_aml_Q_object_R(( struct E_aml_Z_pathname ){ s, *n });
            return s;
        }
      case '^':
        {   if( !E_aml_S_current_path_n )
            {   *n = ~1;
                return 0;
            }
            N parent_n = 0;
            do
            {   if( ++E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end
                || ++parent_n > E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n
                )
                {   *n = ~1;
                    return 0;
                }
            }while( *E_aml_S_parse_data == '^' );
            *n = E_aml_Q_path_R_pathname();
            if( !~(S8)*n
            || !*n
            )
            {   *n = ~1;
                return 0;
            }
            N depth = E_aml_S_current_path_n ? E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n - parent_n : 0;
            Pc s = M(( depth + *n ) * 4 );
            if( !s )
            {   *n = ~0;
                return 0;
            }
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
            {   *n = ~1;
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
                {   N depth = E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n;
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
E_aml_R_pkg_length_( Pc pkg_end
){  if( E_aml_S_parse_data == pkg_end )
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
    while( ++E_aml_S_parse_data != pkg_end
    && i != n
    )
    {   l |= (N)(N8)*E_aml_S_parse_data << ( 4 + i * 8 );
        i++;
    }
    //E_font_I_print( "\npkg_length=" ); E_font_I_print_hex(l);
    return l;
}
_internal
N
E_aml_R_pkg_length( void
){  return E_aml_R_pkg_length_( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end );
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
        return ~1;
    switch( (N8)*E_aml_S_parse_data++ )
    { case 0:
            E_font_I_print( ",0" );
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = 0;
            }
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 1:
            E_font_I_print( ",1" );
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = 1;
            }
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 0xff:
            E_font_I_print( ",~0" );
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = ~0;
            }
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 0xa: // byte const
            if( E_aml_S_parse_data + 1 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            E_font_I_print( ",byte" );
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = (N8)*E_aml_S_parse_data;
            }
            E_aml_S_parse_data++;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 0xb: // word const
            if( E_aml_S_parse_data + 2 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            E_font_I_print( ",word" );
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = *( N16 * )E_aml_S_parse_data;
            }
            E_aml_S_parse_data += 2;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 0xc: // dword const
            if( E_aml_S_parse_data + 4 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            E_font_I_print( ",dword" );
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = *( N32 * )E_aml_S_parse_data;
            }
            E_aml_S_parse_data += 4;
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
                return ~1;
            E_aml_S_parse_data++;
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_string;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p = s;
            }
            E_font_I_print(s);
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 0xe: // qword const
            if( E_aml_S_parse_data + 8 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            E_font_I_print( ",qword" );
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = *( N * )E_aml_S_parse_data;
            }
            E_aml_S_parse_data += 8;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
      case 0x5b:
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            switch( (N8)*E_aml_S_parse_data++ )
            { case 0x30: // revision
                    if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~1;
                    E_font_I_print( ",revision" );
                    if( E_aml_Q_current_path_S_precompilation )
                    {
                    }else
                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = 0x30;
                    }
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
                return ~1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            E_font_I_print( ",buffer" );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_buffer_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
        }
      case 0x12: // package
        {   Pc data_start = E_aml_S_parse_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            if( E_aml_S_parse_data == pkg_end )
                return ~1;
            N n = (N8)*E_aml_S_parse_data;
            E_aml_S_parse_data++;
            E_font_I_print( ",package," ); E_font_I_print_hex(n);
            if( E_aml_Q_current_path_S_precompilation )
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
                return ~1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            E_font_I_print( ",var package" );
            if( E_aml_Q_current_path_S_precompilation )
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
        return ~1;
    switch( (N8)*E_aml_S_parse_data++ )
    { case 0x5b: // ext
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            switch( (N8)*E_aml_S_parse_data++ )
            { case 0x32: // fatal
                    E_font_I_print( ",fatal" );
                    if( ++E_aml_S_parse_data > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~1;
                    if( E_aml_S_parse_data + 4 == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~1;
                    E_aml_S_parse_data += 4;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_fatal_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
              case 0x27: // release
                    E_font_I_print( ",release" );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_release_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
              case 0x26: // reset
                    E_font_I_print( ",reset" );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_reset_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
              case 0x24: // signal
                    E_font_I_print( ",signal" );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_signal_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
              case 0x22: // sleep
                    E_font_I_print( ",sleep" );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_sleep_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
              case 0x21: // stall
                    E_font_I_print( ",stall" );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_stall_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
              case 0x20: // load
                    E_font_I_print( ",load" );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_load_finish, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
              case 0x2a: // unload
                    E_font_I_print( ",unload" );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
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
                return ~1;
            E_aml_S_parse_data += 2;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
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
                return ~1;
            break;
        }
      case 0xcc: // breakpoint
        {   E_font_I_print( ",breakpoint" );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
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
                return ~1;
            break;
        }
      case 0xa0: // if
        {   Pc data_start = E_aml_S_parse_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            E_font_I_print( ",if" );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if++;
            E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_if_op_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
            break;
        }
      case 0xa1: // else
        {   if( !E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if )
                return ~1;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if--;
            Pc data_start = E_aml_S_parse_data;
            N pkg_length = E_aml_R_pkg_length();
            if( !~pkg_length )
                return ~1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            E_font_I_print( ",else" );
            if( E_aml_S_parse_data != pkg_end )
                if( E_aml_Q_current_path_S_precompilation )
                {   E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_else_op_finish, E_aml_Z_parse_stack_Z_entity_S_term );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                }else
                    if( !E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n )
                    {   E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_else_op_finish, E_aml_Z_parse_stack_Z_entity_S_term );
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                    }else
                        E_aml_S_parse_data = pkg_end;
            break;
        }
      case 0xa3: // noop
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
            break;
      case 0x86: // notify
            E_font_I_print( ",notify" );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
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
                return ~1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            E_font_I_print( ",while" );
            if( E_aml_S_parse_data != pkg_end )
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.continue_ = E_aml_S_parse_data;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
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
        return ~1;
    switch( (N8)*E_aml_S_parse_data++ )
    { case 6: // alias
        {   N8 name_n;
            N dest_object_i;
            Pc name = E_aml_Q_path_R_relative( &name_n, &dest_object_i );
            if( !name )
                return (S8)name_n;
            if( !~dest_object_i )
            {   W(name);
                return ~1;
            }
            Pc name_ = M( name_n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, name_n * 4 );
            E_font_I_print( ",alias=" ); E_font_I_print( name_ );
            W( name_ );
            W(name);
            N8 alias_n;
            Pc alias = E_aml_Q_path_R_root( &alias_n );
            if( !alias )
                return (S8)alias_n;
            name_ = M( alias_n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, alias, alias_n * 4 );
            E_font_I_print( ":" ); E_font_I_print( name_ );
            W( name_ );
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_alias, ( struct E_aml_Z_pathname ){ alias, alias_n });
                if( !~object_i
                || object_i == ~1
                )
                {   W(alias);
                    W(name);
                    return object_i;
                }
                struct E_aml_Z_pathname *M_(pathname);
                if( !pathname )
                {   E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                    return ~0;
                }
                *pathname = E_aml_S_object[ dest_object_i ].name;
                E_aml_S_object[ object_i ].data = pathname;
            }
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            break;
        }
      case 8: // object
        {   N8 n;
            Pc name = E_aml_Q_path_R_root( &n );
            if( !name )
                return (S8)n;
            if( n > 1
            && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
            )
            {   W(name);
                return ~1;
            }
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( "\n,object=" ); E_font_I_print( name_ );
            W( name_ );
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_uninitialized, ( struct E_aml_Z_pathname ){ name, n });
                if( !~object_i
                || object_i == ~1
                )
                {   W(name);
                    return object_i;
                }
                name_ = E_text_Z_sl_M_duplicate( name, n * 4 );
                if( !name_ )
                    return ~0;
                name = name_;
            }
            if( !~E_aml_Q_current_path_I_push(( struct E_aml_Z_pathname ){ name, n }))
            {   W(name);
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
                return ~1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            N8 n;
            N object_i;
            Pc name = E_aml_Q_path_R_relative( &n, &object_i );
            if( !name )
                return (S8)n;
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( "\n,scope=" ); E_font_I_print( name_ );
            W( name_ );
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   if( !~object_i
                || !( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_uninitialized
                  || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_device
                  || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_power_res
                  || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_thermal_zone
                  || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_processor
                ))
                {   W(name);
                    return ~1;
                }
            }
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
                return ~1;
            switch( (N8)*E_aml_S_parse_data++ )
            { case 0x87: // bank field
                {   Pc data_start = E_aml_S_parse_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~1;
                    N8 n;
                    N object_i;
                    Pc region_name = E_aml_Q_path_R_relative( &n, &object_i );
                    if( !region_name )
                        return (S8)n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, region_name, n * 4 );
                    E_font_I_print( ",region=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( !~object_i
                    || E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_op_region
                    )
                    {   W( region_name );
                        E_aml_S_parse_data = pkg_end; // Ignoruje brak ‘operation region’ i przeskakuje dalej.
                        break;
                    }
                    Pc bank_name = E_aml_Q_path_R_segment_( pkg_end );
                    if( !~(N)bank_name
                    || (N)bank_name == ~1
                    )
                        return (N)bank_name;
                    name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, bank_name, n * 4 );
                    E_font_I_print( ",bank=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( E_aml_Q_current_path_S_precompilation )
                    {   W( bank_name );
                    }else
                    {   struct E_aml_Z_value *Mt_( value, E_aml_S_parse_stack_S_tmp_p_n );
                        if( !value )
                        {   W( bank_name );
                            return ~1;
                        }
                        value[0].pathname = ( struct E_aml_Z_pathname ){ region_name, n };
                        value[0].copy = no;
                        value[1].p = bank_name;
                        value[1].copy = yes;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p = value;
                        for_n( i, E_aml_S_parse_stack_S_tmp_p_n - 2 )
                            value[ 2 + i ].copy = no;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_bank_field_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
                }
              case 1: // mutex
                {   N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( n > 1
                    && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    )
                    {   W(name);
                        return ~1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",mutex=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~1;
                    E_aml_S_parse_data++;
                    if( E_aml_Q_current_path_S_precompilation )
                    {
                    }else
                    {   N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_mutex, ( struct E_aml_Z_pathname ){ name, n });
                        if( !~object_i
                        || object_i == ~1
                        )
                        {   W(name);
                            return object_i;
                        }
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
                    if( n > 1
                    && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    )
                    {   W(name);
                        return ~1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",data table region=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( E_aml_Q_current_path_S_precompilation )
                    {
                    }else
                    {   N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_data_table_region, ( struct E_aml_Z_pathname ){ name, n });
                        if( !~object_i
                        || object_i == ~1
                        )
                        {   W(name);
                            return object_i;
                        }
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
                    if( n > 1
                    && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    )
                    {   W(name);
                        return ~1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",op region=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~1;
                    if( E_aml_Q_current_path_S_precompilation )
                    {
                    }else
                    {   struct E_aml_Z_pathname region_pathname = ( struct E_aml_Z_pathname ){ name, n };
                        N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_op_region, region_pathname );
                        if( !~object_i
                        || object_i == ~1
                        )
                        {   W(name);
                            return object_i;
                        }
                        struct E_aml_Z_op_region *M_(region);
                        if( !region )
                        {   E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                            return ~0;
                        }
                        region->space = *E_aml_S_parse_data;
                        if( region->space > E_aml_Z_op_region_space_S_pcc )
                        {   W(region);
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                            return ~1;
                        }
                        struct E_aml_Z_value *Mt_( value, E_aml_S_parse_stack_S_tmp_p_n );
                        if( !value )
                        {   W(region);
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                            return ~1;
                        }
                        value[0].pathname = region_pathname;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p = value;
                        for_n( i, E_aml_S_parse_stack_S_tmp_p_n )
                            value[i].copy = no;
                        E_aml_S_object[ object_i ].data = region;
                    }
                    E_aml_S_parse_data++;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_op_region_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
                }
              case 0x81: // field op
                {   Pc data_start = E_aml_S_parse_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~1;
                    N8 n;
                    N object_i;
                    Pc name = E_aml_Q_path_R_relative( &n, &object_i );
                    if( !name )
                        return (S8)n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",field op=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( E_aml_Q_current_path_S_precompilation )
                    {   W(name);
                        E_aml_S_parse_data = pkg_end; // Tymczasowo.
                    }else
                    {   if( !~object_i
                        || E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_op_region
                        )
                        {   W(name);
                            E_aml_S_parse_data = pkg_end; // Ignoruje brak ‘operation region’ i przeskakuje dalej.
                            break;
                        }
                        if( E_aml_S_parse_data == pkg_end )
                            return ~1;
                        struct E_aml_Z_field_unit field_default;
                        field_default.access = (N8)*E_aml_S_parse_data & 3;
                        field_default.lock = (N8)*E_aml_S_parse_data & 8;
                        field_default.update_rule = (N8)*E_aml_S_parse_data >> 5;
                        E_aml_S_parse_data++;
                        N i = 0;
                        while( E_aml_S_parse_data != pkg_end )
                            switch( *E_aml_S_parse_data++ )
                            { case 0: // reserved field
                                {   N n = E_aml_R_pkg_length_( pkg_end );
                                    if( !~n )
                                        return ~1;
                                    E_font_I_print( ",reserved field" );
                                    i += n;
                                    break;
                                }
                              case 1: // access field
                                {   if( E_aml_S_parse_data + 2 > pkg_end )
                                        return ~1;
                                    //field->i = i;
                                    //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                                    E_aml_S_parse_data++;
                                    //TODO Dodać ‘access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                                    E_aml_S_parse_data++;
                                    //field->type = E_aml_Z_field_Z_type_S_access;
                                    //i += n;
                                    break;
                                }
                              case 2: // connect field
                                    E_font_I_print( ",connect field" );
                                    return ~0; //NDFN
                                    break;
                              case 3: // extended access field
                                    if( E_aml_S_parse_data + 3 > pkg_end )
                                        return ~1;
                                    E_font_I_print( ",ext access field" );
                                    //field->i = i;
                                    //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                                    E_aml_S_parse_data++;
                                    //TODO Dodać ‘extended access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                                    E_aml_S_parse_data++;
                                    //TODO Dodać ‘access length’ do drzewa zinterpretowanej przestrzeni ACPI.
                                    E_aml_S_parse_data++; //NDFN Jaki rozmiar.
                                    //field->type = E_aml_Z_field_Z_type_S_ex_access;
                                    //i += n;
                                    break;
                              default: // named field
                                {   E_aml_S_parse_data--;
                                    if( E_aml_S_parse_data + 4 > pkg_end )
                                        return ~1;
                                    Pc name_ = E_aml_Q_path_R_segment_( pkg_end );
                                    if( !~(N)name_
                                    || (N)name_ == ~1
                                    )
                                        return (N)name_;
                                    Pc name__ = M( 4 + 1 );
                                    E_text_Z_s_P_copy_sl_0( name__, name_, 4 );
                                    E_font_I_print( ",name=" ); E_font_I_print( name__ );
                                    W( name__ );
                                    W( name_ );
                                    Pc field_name = M(( E_aml_S_object[ object_i ].name.n + 1 ) * 4 );
                                    if( !field_name )
                                    {   W( name_ );
                                        return ~0;
                                    }
                                    E_mem_Q_blk_I_copy( field_name, E_aml_S_object[ object_i ].name.s, E_aml_S_object[ object_i ].name.n * 4 );
                                    E_mem_Q_blk_I_copy( field_name + E_aml_S_object[ object_i ].name.n * 4, name_, 4 );
                                    W( name_ );
                                    N field_object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_field_unit, ( struct E_aml_Z_pathname ){ field_name, E_aml_S_object[ object_i ].name.n + 1 });
                                    if( !~field_object_i
                                    || field_object_i == ~1
                                    )
                                    {   W( field_name );
                                        return field_object_i;
                                    }
                                    struct E_aml_Z_field_unit *M_(field);
                                    if( !field )
                                    {   E_aml_S_object[ field_object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                                        return ~0;
                                    }
                                    E_aml_S_object[ field_object_i ].data = field;
                                    field->region = E_aml_S_object[ object_i ].name;
                                    field->type = E_aml_Z_field_Z_type_S_plain;
                                    N n = E_aml_R_pkg_length_( pkg_end );
                                    if( !~n )
                                        return ~1;
                                    field->i = i;
                                    field->n = n;
                                    field->access = field_default.access;
                                    field->lock = field_default.lock;
                                    field->update_rule = field_default.update_rule;
                                    i += n;
                                    break;
                                }
                            }
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    break;
                }
              case 0x86: // index field
                {   Pc data_start = E_aml_S_parse_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~1;
                    N8 n;
                    N object_i;
                    Pc name = E_aml_Q_path_R_relative( &n, &object_i );
                    if( !name )
                        return (S8)n;
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",index field=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( E_aml_Q_current_path_S_precompilation )
                    {   E_aml_S_parse_data = pkg_end; // Tymczasowo.
                    }else
                    {   if( !~object_i
                        || E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_op_region
                        )
                        {   W(name);
                            E_aml_S_parse_data = pkg_end; // Ignoruje brak ‘operation region’ i przeskakuje dalej.
                            break;
                        }
                        name = E_aml_Q_path_R_root( &n );
                        if( !name )
                        {   E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                            return (S8)n;
                        }
                        name_ = M( n * 4 + 1 );
                        E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                        E_font_I_print( ",=" ); E_font_I_print( name_ );
                        W( name_ );
                        W(name);
                        if( E_aml_S_parse_data == pkg_end )
                            return ~1;
                        struct E_aml_Z_field_unit field_default;
                        field_default.access = (N8)*E_aml_S_parse_data & 3;
                        field_default.lock = (N8)*E_aml_S_parse_data & 8;
                        field_default.update_rule = (N8)*E_aml_S_parse_data >> 5;
                        E_aml_S_parse_data++;
                        N i = 0;
                        while( E_aml_S_parse_data != pkg_end )
                            switch( *E_aml_S_parse_data++ )
                            { case 0: // reserved field
                                {   E_font_I_print( ",reserved field" );
                                    N n = E_aml_R_pkg_length_( pkg_end );
                                    if( !~n )
                                        return ~1;
                                    i += n;
                                    break;
                                }
                              case 1: // access field
                                    E_font_I_print( ",access field" );
                                    if( E_aml_S_parse_data + 2 > pkg_end )
                                        return ~1;
                                    //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                                    E_aml_S_parse_data++;
                                    //TODO Dodać ‘access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                                    E_aml_S_parse_data++;
                                    break;
                              case 2: // connect field
                                    E_font_I_print( ",connect field" );
                                    return ~0; //NDFN
                                    break;
                              case 3: // extended access field
                                    E_font_I_print( ",ext access field" );
                                    if( E_aml_S_parse_data + 3 > pkg_end )
                                        return ~1;
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
                                        return ~1;
                                    Pc name_ = E_aml_Q_path_R_segment_( pkg_end );
                                    if( !~(N)name_
                                    || (N)name_ == ~1
                                    )
                                        return (N)name_;
                                    Pc name__ = M( 4 + 1 );
                                    E_text_Z_s_P_copy_sl_0( name__, name_, 4 );
                                    E_font_I_print( ",name=" ); E_font_I_print( name__ );
                                    W( name__ );
                                    Pc field_name = M(( E_aml_S_object[ object_i ].name.n + 1 ) * 4 );
                                    if( !field_name )
                                    {   W( name_ );
                                        return ~0;
                                    }
                                    E_mem_Q_blk_I_copy( field_name, E_aml_S_object[ object_i ].name.s, E_aml_S_object[ object_i ].name.n * 4 );
                                    E_mem_Q_blk_I_copy( field_name + E_aml_S_object[ object_i ].name.n * 4, name_, 4 );
                                    W( name_ );
                                    N field_object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_field_unit, ( struct E_aml_Z_pathname ){ field_name, E_aml_S_object[ object_i ].name.n + 1 });
                                    if( !~field_object_i
                                    || field_object_i == ~1
                                    )
                                    {   W( field_name );
                                        return field_object_i;
                                    }
                                    struct E_aml_Z_field_unit *M_(field);
                                    if( !field )
                                    {   E_aml_S_object[ field_object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                                        return ~0;
                                    }
                                    E_aml_S_object[ field_object_i ].data = field;
                                    field->region = E_aml_S_object[ object_i ].name;
                                    field->type = E_aml_Z_field_Z_type_S_plain;
                                    N n = E_aml_R_pkg_length_( pkg_end );
                                    if( !~n )
                                        return ~1;
                                    field->i = i;
                                    field->n = n;
                                    field->access = field_default.access;
                                    field->lock = field_default.lock;
                                    field->update_rule = field_default.update_rule;
                                    i += n;
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
                        return ~1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~1;
                    N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( n > 1
                    && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    )
                    {   W(name);
                        return ~1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",device=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( E_aml_Q_current_path_S_precompilation )
                    {
                    }else
                    {   N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_device, ( struct E_aml_Z_pathname ){ name, n });
                        if( !~object_i
                        || object_i == ~1
                        )
                        {   W(name);
                            return object_i;
                        }
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
                    if( n > 1
                    && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    )
                    {   W(name);
                        return ~1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",event=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( E_aml_Q_current_path_S_precompilation )
                    {
                    }else
                    {   N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_event, ( struct E_aml_Z_pathname ){ name, n });
                        if( !~object_i
                        || object_i == ~1
                        )
                        {   W(name);
                            return object_i;
                        }
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    break;
                }
              case 0x84: // power res
                {   Pc data_start = E_aml_S_parse_data;
                    N pkg_length = E_aml_R_pkg_length();
                    if( !~pkg_length )
                        return ~1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~1;
                    N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( n > 1
                    && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    )
                    {   W(name);
                        return ~1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",power res=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( E_aml_Q_current_path_S_precompilation )
                    {
                    }else
                    {   N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_power_res, ( struct E_aml_Z_pathname ){ name, n });
                        if( !~object_i
                        || object_i == ~1
                        )
                        {   W(name);
                            return object_i;
                        }
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
                    if( E_aml_S_parse_data != pkg_end )
                    {   if( ++E_aml_S_parse_data > pkg_end )
                            return ~1;
                        E_aml_S_parse_data += 2;
                        if( E_aml_S_parse_data > pkg_end )
                            return ~1;
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
                        return ~1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~1;
                    N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( n > 1
                    && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    )
                    {   W(name);
                        return ~1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",thermal zone=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( E_aml_Q_current_path_S_precompilation )
                    {
                    }else
                    {   N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_thermal_zone, ( struct E_aml_Z_pathname ){ name, n });
                        if( !~object_i
                        || object_i == ~1
                        )
                        {   W(name);
                            return object_i;
                        }
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
                        return ~1;
                    Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
                    if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                        return ~1;
                    N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                        return (S8)n;
                    if( n > 1
                    && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    )
                    {   W(name);
                        return ~1;
                    }
                    Pc name_ = M( n * 4 + 1 );
                    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                    E_font_I_print( ",processor=" ); E_font_I_print( name_ );
                    W( name_ );
                    if( ++E_aml_S_parse_data > pkg_end )
                        return ~1;
                    if( E_aml_S_parse_data + 4 > pkg_end )
                        return ~1;
                    E_aml_S_parse_data += 4;
                    if( ++E_aml_S_parse_data > pkg_end )
                        return ~1;
                    if( E_aml_Q_current_path_S_precompilation )
                    {
                    }else
                    {   N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_processor, ( struct E_aml_Z_pathname ){ name, n });
                        if( !~object_i
                        || object_i == ~1
                        )
                        {   W(name);
                            return object_i;
                        }
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
                return ~1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            N8 n;
            Pc name = E_aml_Q_path_R_root( &n );
            if( !name )
                return (S8)n;
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( "\n,procedure=" ); E_font_I_print( name_ );
            W( name_ );
            if( n > 1
            && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
            )
            {   W(name);
                return ~1;
            }
            if( E_aml_S_parse_data == pkg_end )
                return ~1;
            struct E_aml_Z_object_data_Z_procedure *M_(procedure);
            if( !procedure )
                return ~0;
            procedure->arg_n = *E_aml_S_parse_data & 3;
            E_aml_S_parse_data++;
            E_font_I_print( "," ); E_font_I_print_hex( procedure->arg_n );
            N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_procedure, ( struct E_aml_Z_pathname ){ name, n });
            if( !~object_i
            || object_i == ~1
            )
            {   W(procedure);
                W(name);
                return object_i;
            }
            procedure->data = E_aml_S_parse_data;
            procedure->data_end = pkg_end;
            procedure->procedure = 0;
            E_aml_S_object[ object_i ].data = procedure;
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].match = yes;
            if( E_aml_Q_current_path_S_precompilation )
            {   if( E_aml_S_parse_data != pkg_end )
                {   name_ = E_text_Z_sl_M_duplicate( name, n * 4 );
                    if( !name_ )
                        return ~0;
                    if( !~E_aml_Q_current_path_I_push(( struct E_aml_Z_pathname ){ name_, n }))
                    {   W( name_ );
                        return ~0;
                    }
                    E_aml_I_delegate_pkg( E_aml_Z_parse_stack_Z_entity_S_procedure_finish, E_aml_Z_parse_stack_Z_entity_S_term );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                }
            }else
                E_aml_S_parse_data = pkg_end;
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
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.break_ = 0; \
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
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.break_ = 0; \
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
        return ~1;
    switch( (N8)*E_aml_S_parse_data++ )
    { case 0x5b: // ext
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
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
                    return ~1;
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
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            switch( (N8)*E_aml_S_parse_data++ )
            { case 0x93:
                    E_font_I_print( ",lnotequal" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lnotequal_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
              case 0x94:
                    E_font_I_print( ",llessequal" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_llessequal_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
              case 0x95:
                    E_font_I_print( ",lgreaterequal" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lgreaterequal_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
              default:
                    E_aml_S_parse_data--;
                    E_font_I_print( ",lnot" );
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lnot_finish, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                    break;
            }
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
                return ~1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            if( E_aml_S_parse_data == pkg_end )
                return ~1;
            N n = (N8)*E_aml_S_parse_data;
            E_aml_S_parse_data++;
            E_font_I_print( ",package," ); E_font_I_print_hex(n);
            if( E_aml_Q_current_path_S_precompilation )
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
                return ~1;
            Pc pkg_end = E_aml_S_parse_data + pkg_length - ( E_aml_S_parse_data - data_start );
            if( pkg_end > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            E_font_I_print( ",var package" );
            struct E_aml_Z_package *package;
            if( E_aml_Q_current_path_S_precompilation )
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
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   N object_i;
                Pc name = E_aml_Q_path_R_relative( &n, &object_i );
                if( !name )
                    return (S8)n;
                Pc name_ = M( n * 4 + 1 );
                E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                E_font_I_print( ",expr string=" ); E_font_I_print( name_ );
                W( name_ );
                if( !~object_i )
                {   if( n > 1
                    && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    )
                    {   W(name);
                        return ~1;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname = ( struct E_aml_Z_pathname ){ name, n };
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_pathname;
                    break;
                }
                if( E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_procedure )
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_pathname;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname = ( struct E_aml_Z_pathname ){ name, n };
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                }else
                {   E_font_I_print( ",invocation" );
                    if( !E_mem_Q_blk_I_append( &E_aml_S_procedure_invocation_stack, 1 ))
                        return ~0;
                    E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].name = E_aml_S_object[ object_i ].name;
                    for_n( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].arg ))
                        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].arg[i].type = E_aml_Z_value_Z_type_S_uninitialized;
                    for_n_( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].local ))
                        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].local[i].type = E_aml_Z_value_Z_type_S_uninitialized;
                    E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].result.type = E_aml_Z_value_Z_type_S_uninitialized;
                    E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n ].current_path_n = E_aml_S_current_path_n;
                    E_aml_S_procedure_invocation_stack_n++;
                    struct E_aml_Z_object_data_Z_procedure *procedure = E_aml_S_object[ object_i ].data;
                    if( procedure->arg_n )
                    {   E_font_I_print( "," ); E_font_I_print_hex( procedure->arg_n );
                        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].computing_arg = yes;
                        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].return_ = E_aml_S_parse_data;
                        if( !E_mem_Q_blk_I_append( &E_aml_S_parse_stack, 2 * procedure->arg_n - 1 ))
                            return ~0;
                        Pc data_end = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].entity = E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish_1;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = 0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end = 0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.break_ = 0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = E_aml_Z_parse_stack_Z_entity_S_term_arg;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 1;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = data_end;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.break_ = 0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0;
                        E_aml_S_parse_stack_n++;
                        for_n_( i, procedure->arg_n - 1 )
                        {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish_1;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 1;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = 0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.break_ = 0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0;
                            E_aml_S_parse_stack_n++;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].entity = E_aml_Z_parse_stack_Z_entity_S_term_arg;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].n = 1;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].data_end = data_end;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.break_ = 0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.result.copy = no;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n ].execution_context.tmp_p = 0;
                            E_aml_S_parse_stack_n++;
                        }
                    }else
                    {   if( procedure->procedure )
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = procedure->procedure();
                        else
                        {   E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].computing_arg = no;
                            E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].return_ = E_aml_S_parse_data;
                            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish_2, E_aml_Z_parse_stack_Z_entity_S_term );
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end = procedure->data_end;
                            E_aml_S_parse_data = procedure->data;
                        }
                    }
                    W(name);
                }
            }
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
        return ~1;
    switch( (N8)*E_aml_S_parse_data++ )
    { case 0x5b:
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                return ~1;
            switch( (N8)*E_aml_S_parse_data++ )
            { case 0x31: // debug
                    E_font_I_print( ",debug" );
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_debug_ref;
                    break;
              default:
                    E_aml_S_parse_data -= 2;
                    return ~1;
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
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   if( !E_aml_S_procedure_invocation_stack_n )
                    return ~1;
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
            if( E_aml_Q_current_path_S_precompilation )
            {
            }else
            {   if( !E_aml_S_procedure_invocation_stack_n )
                    return ~1;
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
            Pc name_ = M( n * 4 + 1 );
            E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
            E_font_I_print( ",supername=" ); E_font_I_print( name_ );
            W( name_ );
            if( E_aml_Q_current_path_S_precompilation )
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
        return ~0; \
    Pc data_end = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].entity = finish; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = 0; \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end = 0; \
    E_aml_Q_value_W( &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result ); \
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no; \
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
        E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ] );
        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result;
        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
        return yes;
    }
    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_local_ref )
    {   if( !E_aml_S_procedure_invocation_stack_n )
            return ~0;
        N local_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
        E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ] );
        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result;
        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
        return yes;
    }
    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_debug_ref )
        return yes;
    return no;
}
_internal
N
E_aml_M_clear_object( void
){  if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname )
        return ~1;
    N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
    if( !~object_i )
        return ~1;
    if( !~E_aml_Q_object_W_data( object_i ))
        return ~0;
    return object_i;
}
_internal
N
E_aml_M_number_pathname_v2_finish_2( N *v_1
, N *v_2
){  if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname
    )
    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
        return ~0;
    }
    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname );
        if( ~object_i )
        {   if( E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_number )
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                return ~0;
            }
            *v_1 = E_aml_S_object[ object_i ].n;
        }else
            *v_1 = 0;
    }else
        *v_1 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_number
    && E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname
    )
    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
        return ~0;
    }
    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
        if( ~object_i )
        {   if( E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_number )
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                return ~0;
            }
            *v_2 = E_aml_S_object[ object_i ].n;
        }else
            *v_2 = 0;
    }else
        *v_2 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
    return 0;
}
_internal
N
E_aml_M_number_finish_3( void
){  N ret = E_aml_M_arg_local_debug();
    if( !~ret )
        return ~1;
    if( !ret )
    {   N object_i = E_aml_M_clear_object();
        if( !~object_i
        || object_i == ~1
        )
            return object_i;
        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type != E_aml_Z_value_Z_type_S_uninitialized )
        {   E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n;
            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
        }else
            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
    }
    return 0;
}
_internal
N
E_aml_M_field_finish_2( N bytes
){  struct E_aml_Z_value o, i;
    if( E_aml_Q_current_path_S_precompilation )
    {
    }else
    {   N object_i;
        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
        {   object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname );
            if( ~object_i
            && E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
            )
                o = E_aml_Q_object_I_to_value( object_i );
            else
                o.type = E_aml_Z_value_Z_type_S_uninitialized;
        }else
            o.type = E_aml_Z_value_Z_type_S_uninitialized;
        if( ~o.i )
            o.type = E_aml_Z_value_Z_type_S_uninitialized;
        if( o.type != E_aml_Z_value_Z_type_S_uninitialized )
        {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
            {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                if( ~object_i )
                    if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                        i = E_aml_Q_object_I_to_value( object_i );
                    else
                        i.type = E_aml_Z_value_Z_type_S_uninitialized;
                else
                {   i.n = 0;
                    i.type = E_aml_Z_value_Z_type_S_number;
                }
            }else
                i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
            if( i.type != E_aml_Z_value_Z_type_S_number )
                i.type = E_aml_Z_value_Z_type_S_uninitialized;
            if( i.type != E_aml_Z_value_Z_type_S_uninitialized )
            {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                if( i.n + bytes > buffer->n )
                    return ~1;
            }
        }
    }
    N8 n;
    Pc name = E_aml_Q_path_R_root( &n );
    if( !name )
        return -(S8)n;
    Pc name_ = M( n * 4 + 1 );
    E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
    E_font_I_print( ",bytes field=" ); E_font_I_print( name_ );
    W( name_ );
    if( E_aml_Q_current_path_S_precompilation )
    {
        W(name);
    }else
    {   if( n > 1
        && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
        )
        {   W(name);
            return ~1;
        }
        N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_buffer_field, ( struct E_aml_Z_pathname ){ name, n });
        if( !~object_i
        || object_i == ~1
        )
        {   W(name);
            return object_i;
        }
        if( o.type != E_aml_Z_value_Z_type_S_uninitialized
        && i.type != E_aml_Z_value_Z_type_S_uninitialized
        )
        {   struct E_aml_Z_buffer_field *M_(field);
            if( !field )
                return ~0;
            if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                field->pathname = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname;
            else
            {   field->pathname.s = E_text_Z_sl_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname.s
                , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname.n * 4
                );
                if( !field->pathname.s )
                {   W(field);
                    return ~0;
                }
                field->pathname.n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname.n;
            }
            field->i = i.n * 8;
            field->u = bytes * 8;
            E_aml_S_object[ object_i ].data = field;
        }else
            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
    }
    return 0;
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
                    (*e)->p = E_text_Z_s0_M_duplicate( v_1->p );
                    if( !(*e)->p )
                    {   W( *e );
                        return ~0;
                    }
                    (*e)->i = ~0;
                    (*e)->type = E_aml_Z_value_Z_type_S_string;
                    break;
              case E_aml_Z_value_Z_type_S_buffer:
                    (*e)->buffer.p = E_text_Z_sl_M_duplicate( v_1->buffer.p, v_1->buffer.n );
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
            *e = v_1;
        N ret = E_aml_Q_value_N_convert( v_0->type, *e );
        if( ret < 0 )
        {   if( !copy )
                W( *e );
            return ret;
        }
    }else
        *e = v_1;
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
){  if( E_aml_Q_current_path_S_precompilation )
    {
    }else
        if(( E_aml_Q_procedure_invocation_stack_S_invokeing
          && E_aml_S_procedure_invocation_stack_n > 1
        )
        || ( !E_aml_Q_procedure_invocation_stack_S_invokeing
          && E_aml_S_procedure_invocation_stack_n
        )) // Wyjście do nadrzędnego bloku wywołanej procedury po błędzie składni.
        {   for_n_rev( i, E_aml_S_parse_stack_n )
                if( E_aml_S_parse_stack[i].entity == E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish_2 )
                {   for_n( j, E_aml_S_current_path_n - E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].current_path_n )
                        W( E_aml_S_current_path[ E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].current_path_n + j ].s );
                    if( !E_mem_Q_blk_I_remove( &E_aml_S_current_path
                    , E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].current_path_n
                    , E_aml_S_current_path_n - E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].current_path_n )
                    )
                        return ~0;
                    E_aml_S_current_path_n -= E_aml_S_current_path_n - E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].current_path_n;
                    if( E_aml_S_current_path_n )
                    {   Pc name_ = M( E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n * 4 + 1 );
                        E_text_Z_s_P_copy_sl_0( name_, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].s, E_aml_S_current_path[ E_aml_S_current_path_n - 1 ].n * 4 );
                        E_font_I_print( "\n,go over to=" ); E_font_I_print( name_ );
                        W( name_ );
                    }else
                        E_font_I_print( "\n,go over to=\\" );
                    if( !~E_aml_S_parse_stack[ i - 1 ].n ) // Dla listy wyliczanej w nieskończoność interpretacja zakończyła się.
                        if( !--i )
                            break;
                    for_n_rev_( j, E_aml_S_object_n )
                        if( E_aml_S_object[j].procedure_invocation == E_aml_S_procedure_invocation_stack_n )
                            if( !~E_aml_Q_object_W(j) )
                                return ~0;
                    for_n_( j, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg ))
                        if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[j].type != E_aml_Z_value_Z_type_S_uninitialized )
                            E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[j] );
                    for_n_( j, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local ))
                        if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[j].type != E_aml_Z_value_Z_type_S_uninitialized )
                            E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[j] );
                    E_aml_S_parse_data = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].return_;
                    if( !E_mem_Q_blk_I_remove( &E_aml_S_procedure_invocation_stack, --E_aml_S_procedure_invocation_stack_n, 1 ))
                        return ~0;
                    for( N j = i; j != E_aml_S_parse_stack_n; j++ )
                    {   E_aml_Q_value_W( &E_aml_S_parse_stack[j].execution_context.result );
                        struct E_aml_Z_value *value = E_aml_S_parse_stack[j].execution_context.tmp_p;
                        if(value)
                        {   for_n( i, E_aml_S_parse_stack_S_tmp_p_n )
                                E_aml_Q_value_W( &value[i] );
                            W(value);
                        }
                    }
                    if( !E_mem_Q_blk_I_remove( &E_aml_S_parse_stack, i, E_aml_S_parse_stack_n - i ))
                        return ~0;
                    E_aml_S_parse_stack_n = i;
                    return 0;
                }
        }else if( E_aml_S_current_path_n ) // Wyjście do nadrzędnego obiektu po błędzie składni.
        {   N current_path_i = E_aml_S_current_path_n;
            for_n_rev( i, E_aml_S_parse_stack_n )
            {   if( E_aml_S_parse_stack[i].entity == E_aml_Z_parse_stack_Z_entity_S_restore_current_path
                || E_aml_S_parse_stack[i].entity == E_aml_Z_parse_stack_Z_entity_S_power_res_finish
                || E_aml_S_parse_stack[i].entity == E_aml_Z_parse_stack_Z_entity_S_thermal_zone_finish
                || E_aml_S_parse_stack[i].entity == E_aml_Z_parse_stack_Z_entity_S_device_finish
                || E_aml_S_parse_stack[i].entity == E_aml_Z_parse_stack_Z_entity_S_processor_finish
                || E_aml_S_parse_stack[i].entity == E_aml_Z_parse_stack_Z_entity_S_procedure_finish
                )
                {   current_path_i--;
                    N object_i = E_aml_Q_object_R(( struct E_aml_Z_pathname ){ E_aml_S_current_path[ current_path_i ].s, E_aml_S_current_path[ current_path_i ].n });
                    if( ~object_i ) // Pomijanie ‘scope’.
                    {   if( !~E_aml_Q_object_W_data( object_i ))
                            return ~0;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                        for_n( j, E_aml_S_current_path_n - current_path_i )
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
                        if( !~E_aml_S_parse_stack[ i - 1 ].n ) // Dla listy wyliczanej w nieskończoność interpretacja zakończyła się.
                            if( !--i )
                                break;
                        E_aml_S_parse_data = E_aml_S_parse_stack[i].data_end;
                        for( N j = i; j != E_aml_S_parse_stack_n; j++ )
                        {   E_aml_Q_value_W( &E_aml_S_parse_stack[j].execution_context.result );
                            struct E_aml_Z_value *value = E_aml_S_parse_stack[j].execution_context.tmp_p;
                            if(value)
                            {   for_n( i, E_aml_S_parse_stack_S_tmp_p_n )
                                    E_aml_Q_value_W( &value[i] );
                                W(value);
                            }
                        }
                        if( !E_mem_Q_blk_I_remove( &E_aml_S_parse_stack, i, E_aml_S_parse_stack_n - i ))
                            return ~0;
                        E_aml_S_parse_stack_n = i;
                        return 0;
                    }
                }
                if( !current_path_i )
                    break;
            }
        }
    return ~0;
}
_internal
N
E_aml_M_parse( void
){  N stack_n_last;
    Pc data = 0;
    N ret = 0;
Loop:
    O{  stack_n_last = E_aml_S_parse_stack_n;
        //E_font_I_print( "\nentity=" ); E_font_I_print_hex( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].entity );
        //E_font_I_print( ",data=" ); E_font_I_print_hex( (N)E_aml_S_parse_data );
        //E_font_I_print( ",data_end=" ); E_font_I_print_hex( (N)E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end );
        //E_font_I_print( ",value=" ); E_font_I_print_hex( (( N * )E_aml_S_parse_data )[0] );
        //E_font_I_print( "," ); E_font_I_print_hex( (( N * )E_aml_S_parse_data )[1] );
        switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].entity )
        { case E_aml_Z_parse_stack_Z_entity_S_result_to_n:
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_restore_current_path:
                if( !~E_aml_Q_current_path_I_pop() )
                    return ~0;
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
                {   ret = ~1;
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
                        if( E_aml_Q_current_path_S_precompilation )
                        {
                        }else
                        {   if( !E_aml_S_procedure_invocation_stack_n )
                            {   ret = ~1;
                                break;
                            }
                            N arg_i = (N8)*( E_aml_S_parse_data - 1 ) - 0x68;
                            if( !E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].computing_arg )
                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ];
                                E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].copy = no;
                            }else
                            {   if( E_aml_S_procedure_invocation_stack_n < 2
                                || E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 2 ].arg[ arg_i ].type == E_aml_Z_value_Z_type_S_uninitialized
                                )
                                {   ret = ~1;
                                    break;
                                }
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 2 ].arg[ arg_i ];
                                E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 2 ].arg[ arg_i ].copy = no;
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
                        if( E_aml_Q_current_path_S_precompilation )
                        {
                        }else
                        {   if( !E_aml_S_procedure_invocation_stack_n )
                            {   ret = ~1;
                                break;
                            }
                            N local_i = (N8)*( E_aml_S_parse_data - 1 ) - 0x60;
                            if( !E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].computing_arg )
                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ];
                                E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].copy = no;
                            }else
                            {   if( E_aml_S_procedure_invocation_stack_n < 2
                                || E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 2 ].local[ local_i ].type == E_aml_Z_value_Z_type_S_uninitialized
                                )
                                {   ret = ~1;
                                    break;
                                }
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 2 ].local[ local_i ];
                                E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 2 ].local[ local_i ].copy = no;
                            }
                        }
                        break;
                  default:
                    {   E_aml_S_parse_data--;
                        if( !E_mem_Q_blk_I_append( &E_aml_S_parse_stack, 2 ))
                            return ~0;
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
                if( ret == ~1
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
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
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
                if( E_aml_Q_current_path_S_precompilation )
                {
                    E_aml_S_parse_data = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end; // Tymczasowo.
                }else
                {   struct E_aml_Z_value n;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                n = E_aml_Q_object_I_to_value( object_i );
                            else
                                n.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   n.n = 0;
                            n.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( n.type != E_aml_Z_value_Z_type_S_number )
                            n.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if( n.type == E_aml_Z_value_Z_type_S_uninitialized )
                    {   ret = ~1;
                        break;
                    }
                    if( E_aml_S_parse_data + n.n > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                    {   ret = ~1;
                        break;
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p = E_aml_S_parse_data;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n = n.n;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_buffer;
                    E_aml_S_parse_data += n.n;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_package:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end ) // Na przypadek startu z “n == 0”.
                    break;
                struct E_aml_Z_value value_package = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result;
                ret = E_aml_I_data_object();
                if( !~ret )
                    return ~0;
                if( ret == ~1 )
                    break;
                if( !E_aml_S_parse_stack[ stack_n_last - 2 ].match )
                {   N8 n;
                    Pc name = E_aml_Q_path_R_root( &n );
                    if( !name )
                    {   ret = (S8)n;
                        E_aml_Q_value_W( &value_package );
                        break;
                    }
                    if( E_aml_Q_current_path_S_precompilation )
                    {
                    }else
                    {   struct E_aml_Z_value *value = E_mem_Q_blk_I_append( &value_package.package->value, 1 );
                        if( !value )
                        {   W(name);
                            E_aml_Q_value_W( &value_package );
                            return ~0;
                        }
                        value_package.package->n++;
                        value->pathname = ( struct E_aml_Z_pathname ){ name, n };
                        value->type = E_aml_Z_value_Z_type_S_pathname;
                        E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result = value_package;
                    }
                }else
                    if( E_aml_Q_current_path_S_precompilation )
                    {
                    }else
                    {   struct E_aml_Z_value *value = E_mem_Q_blk_I_append( &value_package.package->value, 1 );
                        if( !value )
                        {   E_aml_Q_value_W( &value_package );
                            return ~0;
                        }
                        switch( E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.type )
                        { case E_aml_Z_value_Z_type_S_number:
                                value->n = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.n;
                                value->type = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.type;
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
                                        return ~0;
                                    }
                                }
                                value->p = s;
                                value->copy = yes;
                                value->i = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.i;
                                value->type = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.type;
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
                                        return ~0;
                                    }
                                }
                                value->buffer.n = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.buffer.n;
                                value->copy = yes;
                                value->i = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.i;
                                value->type = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.type;
                                break;
                            }
                          case E_aml_Z_value_Z_type_S_package:
                            {   *value = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result;
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result;
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                break;
                            }
                          default:
                                E_aml_Q_value_W( &value_package );
                                ret = ~1;
                                break;
                        }
                        if( ret != ~1 )
                        {   value_package.package->n++;
                            E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result = value_package;
                            E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.result.copy = no;
                        }
                    }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_package_finish:
            {   for_n_rev( i, E_aml_S_parse_stack_n )
                    if( E_aml_S_parse_stack[i].entity != E_aml_Z_parse_stack_Z_entity_S_buffer_finish
                    && E_aml_S_parse_stack[i].entity != E_aml_Z_parse_stack_Z_entity_S_package_finish
                    )
                        break;
                if( !~i )
                    E_aml_S_parse_stack[ stack_n_last - 1 ].execution_context.result.copy = yes;
                break;
            }
          case E_aml_Z_parse_stack_Z_entity_S_bank_field_finish:
            {   if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {   E_aml_S_parse_data = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end; // Tymczasowo.
                }else
                {   struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    N object_i = E_aml_Q_object_R( value[0].pathname );
                    struct E_aml_Z_value bank_value;
                    if( (N8)*E_aml_S_parse_data & 0x80 )
                        bank_value.type = E_aml_Z_value_Z_type_S_uninitialized;
                    else
                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                            if( ~object_i )
                                if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                    bank_value = E_aml_Q_object_I_to_value( object_i );
                                else
                                    bank_value.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                            {   bank_value.n = 0;
                                bank_value.type = E_aml_Z_value_Z_type_S_number;
                            }
                        }else
                        {   bank_value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                            if( bank_value.type != E_aml_Z_value_Z_type_S_number )
                                bank_value.type = E_aml_Z_value_Z_type_S_uninitialized;
                        }
                        Pc bank_name = M(( E_aml_S_object[ object_i ].name.n + 1 ) * 4 );
                        if( !bank_name )
                            return ~0;
                        E_mem_Q_blk_I_copy( bank_name, E_aml_S_object[ object_i ].name.s, E_aml_S_object[ object_i ].name.n * 4 );
                        E_mem_Q_blk_I_copy( bank_name + E_aml_S_object[ object_i ].name.n * 4, value[1].p, 4 );
                        N bank_object_i = E_aml_Q_object_R(( struct E_aml_Z_pathname ){ bank_name, E_aml_S_object[ object_i ].name.n + 1 });
                        if( !~bank_object_i
                        || E_aml_S_object[ bank_object_i ].type != E_aml_Z_object_Z_type_S_field_unit
                        )
                        {   W( bank_name );
                            ret = ~1;
                            break;
                        }
                        struct E_aml_Z_field_unit field_default;
                        field_default.access = (N8)*E_aml_S_parse_data & 3;
                        field_default.lock = (N8)*E_aml_S_parse_data & 8;
                        field_default.update_rule = (N8)*E_aml_S_parse_data >> 5;
                        E_aml_S_parse_data++;
                        N i = 0;
                        while( !ret
                        && E_aml_S_parse_data != E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end
                        )
                            switch( *E_aml_S_parse_data++ )
                            { case 0: // reserved field
                                {   N n = E_aml_R_pkg_length();
                                    if( !~n )
                                    {   ret = ~1;
                                        break;
                                    }
                                    E_font_I_print( ",reserved field" );
                                    i += n;
                                    break;
                                }
                              case 1: // access field
                                {   if( E_aml_S_parse_data + 2 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                                    {   ret = ~1;
                                        break;
                                    }
                                    //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                                    E_aml_S_parse_data++;
                                    //TODO Dodać ‘access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                                    E_aml_S_parse_data++;
                                    //i += n;
                                    break;
                                }
                              case 2: // connect field
                                {   E_font_I_print( ",connect field" );
                                    W( bank_name );
                                    return ~0; //NDFN
                                    break;
                                }
                              case 3: // extended access field
                                {   if( E_aml_S_parse_data + 3 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                                    {   ret = ~1;
                                        break;
                                    }
                                    E_font_I_print( ",ext access field" );
                                    //TODO Dodać ‘access type’ do drzewa zinterpretowanej przestrzeni ACPI.
                                    E_aml_S_parse_data++;
                                    //TODO Dodać ‘extended access attrib’ do drzewa zinterpretowanej przestrzeni ACPI.
                                    E_aml_S_parse_data++;
                                    //TODO Dodać ‘access length’ do drzewa zinterpretowanej przestrzeni ACPI.
                                    E_aml_S_parse_data++; //NDFN Jaki rozmiar.
                                    //i += n;
                                    break;
                                }
                              default: // named field
                                {   E_aml_S_parse_data--;
                                    if( E_aml_S_parse_data + 4 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                                        return ~1;
                                    Pc name_ = E_aml_Q_path_R_segment();
                                    if( !~(N)name_
                                    || (N)name_ == ~1
                                    )
                                    {   W( bank_name );
                                        return (N)name_;
                                    }
                                    Pc name__ = M( 4 + 1 );
                                    E_text_Z_s_P_copy_sl_0( name__, name_, 4 );
                                    E_font_I_print( ",name=" ); E_font_I_print( name__ );
                                    W( name__ );
                                    Pc field_name = M(( E_aml_S_object[ object_i ].name.n + 1 ) * 4 );
                                    if( !field_name )
                                    {   W( name_ );
                                        W( bank_name );
                                        return ~0;
                                    }
                                    E_mem_Q_blk_I_copy( field_name, E_aml_S_object[ object_i ].name.s, E_aml_S_object[ object_i ].name.n * 4 );
                                    E_mem_Q_blk_I_copy( field_name + E_aml_S_object[ object_i ].name.n * 4, name_, 4 );
                                    W( name_ );
                                    N field_object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_field_unit, ( struct E_aml_Z_pathname ){ field_name, E_aml_S_object[ object_i ].name.n + 1 });
                                    if( !~field_object_i
                                    || field_object_i == ~1
                                    )
                                    {   W( field_name );
                                        W( bank_name );
                                        return field_object_i;
                                    }
                                    name_ = E_text_Z_sl_M_duplicate( bank_name, ( E_aml_S_object[ object_i ].name.n + 1 ) * 4 );
                                    if( !name_ )
                                    {   E_aml_S_object[ field_object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                                        W( bank_name );
                                        return ~0;
                                    }
                                    struct E_aml_Z_field_unit *M_(field);
                                    if( !field )
                                    {   E_aml_S_object[ field_object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                                        W( bank_name );
                                        return ~0;
                                    }
                                    E_aml_S_object[ field_object_i ].data = field;
                                    field->region = E_aml_S_object[ object_i ].name;
                                    field->bank_field = ( struct E_aml_Z_pathname ){ name_, E_aml_S_object[ object_i ].name.n + 1 };
                                    field->type = E_aml_Z_field_Z_type_S_bank;
                                    N n = E_aml_R_pkg_length();
                                    if( !~n )
                                    {   ret = ~1;
                                        break;
                                    }
                                    field->bank_value = bank_value.n;
                                    field->i = i;
                                    field->n = n;
                                    field->access = field_default.access;
                                    field->lock = field_default.lock;
                                    field->update_rule = field_default.update_rule;
                                    i += n;
                                    break;
                                }
                            }
                        W( bank_name );
                    }
                    if( bank_value.type == E_aml_Z_value_Z_type_S_uninitialized )
                    {   if( !~E_aml_Q_object_W_data( object_i ))
                            return ~0;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                    }
                }
                break;
            }
          case E_aml_Z_parse_stack_Z_entity_S_bit_field_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_bit_field_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_bit_field_finish_2:
            {   struct E_aml_Z_value o, i;
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                i = E_aml_Q_object_I_to_value( object_i );
                            else
                                i.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   i.n = 0;
                            i.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( i.type != E_aml_Z_value_Z_type_S_number )
                            i.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if( i.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   N object_i;
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                        {   object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname );
                            if( ~object_i
                            && E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                            )
                                o = E_aml_Q_object_I_to_value( object_i );
                            else
                                o.type = E_aml_Z_value_Z_type_S_uninitialized;
                        }else
                            o.type = E_aml_Z_value_Z_type_S_uninitialized;
                        if( o.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                            if(( i.n + 1 ) / 8 + (( i.n + 1 ) % 8 ? 1 : 0 ) > buffer->n )
                            {   ret = ~1;
                                break;
                            }
                        }
                    }
                }
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
                if( E_aml_Q_current_path_S_precompilation )
                {
                    W(name);
                }else
                {   if( n > 1
                    && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    )
                    {   W(name);
                        ret = ~1;
                        break;
                    }
                    N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_buffer_field, ( struct E_aml_Z_pathname ){ name, n });
                    if( !~object_i
                    || object_i == ~1
                    )
                    {   W(name);
                        if( object_i == ~1 )
                        {   ret = ~1;
                            break;
                        }
                        return ~0;
                    }
                    if( i.type != E_aml_Z_value_Z_type_S_uninitialized
                    && o.type != E_aml_Z_value_Z_type_S_uninitialized
                    )
                    {   struct E_aml_Z_buffer_field *M_(field);
                        if( !field )
                            return ~0;
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                            field->pathname = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname;
                        else
                        {   field->pathname.s = E_text_Z_sl_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname.s
                            , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname.n * 4
                            );
                            if( !field->pathname.s )
                            {   W(field);
                                return ~0;
                            }
                            field->pathname.n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname.n;
                        }
                        field->i = i.n;
                        field->u = 1;
                        E_aml_S_object[ object_i ].data = field;
                    }else
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                }
                break;
            }
          case E_aml_Z_parse_stack_Z_entity_S_byte_field_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_byte_field_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_byte_field_finish_2:
                ret = E_aml_M_field_finish_2(1);
                if( !~ret )
                    return ~0;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_dword_field_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_dword_field_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_dword_field_finish_2:
                ret = E_aml_M_field_finish_2(4);
                if( !~ret )
                    return ~0;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_field_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value *Mt_( value, E_aml_S_parse_stack_S_tmp_p_n );
                    if( !value )
                        return ~0;
                    value[0] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p = value;
                    for_n( i, E_aml_S_parse_stack_S_tmp_p_n - 1 )
                        value[ 1 + i ].copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_field_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_field_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    value[1] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_field_finish_3, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_field_finish_3:
            {   struct E_aml_Z_value *value;
                struct E_aml_Z_value o, i, u;
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i
                        && E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                        )
                            u = E_aml_Q_object_I_to_value( object_i );
                        else
                            u.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }else
                    {   u = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( u.type != E_aml_Z_value_Z_type_S_number )
                            u.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if( u.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   if( !u.n )
                            return ~0;
                        value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                        N object_i;
                        if( value[0].type == E_aml_Z_value_Z_type_S_pathname )
                        {   object_i = E_aml_Q_object_R( value[0].pathname );
                            if( ~object_i
                            && E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                            && !~E_aml_S_object[ object_i ].n
                            )
                                o = E_aml_Q_object_I_to_value( object_i );
                            else
                                o.type = E_aml_Z_value_Z_type_S_uninitialized;
                        }else
                            o.type = E_aml_Z_value_Z_type_S_uninitialized;
                        if( o.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   if( value[1].type == E_aml_Z_value_Z_type_S_pathname )
                            {   N object_i = E_aml_Q_object_R( value[1].pathname );
                                if( ~object_i )
                                    if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                        i = E_aml_Q_object_I_to_value( object_i );
                                    else
                                        i.type = E_aml_Z_value_Z_type_S_uninitialized;
                                else
                                {   i.n = 0;
                                    i.type = E_aml_Z_value_Z_type_S_number;
                                }
                            }else
                            {   i = value[1];
                                if( i.type != E_aml_Z_value_Z_type_S_number )
                                    i.type = E_aml_Z_value_Z_type_S_uninitialized;
                            }
                            if( i.type != E_aml_Z_value_Z_type_S_uninitialized )
                            {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                                if(( i.n + u.n ) / 8 + (( i.n + u.n ) % 8 ? 1 : 0 ) > buffer->n )
                                    i.type = E_aml_Z_value_Z_type_S_uninitialized;
                            }
                        }
                    }
                }
                N8 n;
                Pc name = E_aml_Q_path_R_root( &n );
                if( !name )
                {   ret = -(S8)n;
                    break;
                }
                Pc name_ = M( n * 4 + 1 );
                E_text_Z_s_P_copy_sl_0( name_, name, n * 4 );
                E_font_I_print( ",bytes field=" ); E_font_I_print( name_ );
                W( name_ );
                if( E_aml_Q_current_path_S_precompilation )
                {
                    W(name);
                }else
                {   if( n > 1
                    && !~E_aml_Q_object_R(( struct E_aml_Z_pathname ){ name, n - 1 })
                    )
                    {   W(name);
                        ret = ~1;
                        break;
                    }
                    N object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_buffer_field, ( struct E_aml_Z_pathname ){ name, n });
                    if( !~object_i
                    || object_i == ~1
                    )
                    {   W(name);
                        if( object_i == ~1 )
                        {   ret = ~1;
                            break;
                        }
                        return ~0;
                    }
                    if( u.type != E_aml_Z_value_Z_type_S_uninitialized
                    && o.type != E_aml_Z_value_Z_type_S_uninitialized
                    && i.type != E_aml_Z_value_Z_type_S_uninitialized
                    )
                    {   struct E_aml_Z_buffer_field *M_(field);
                        if( !field )
                            return ~0;
                        if( value[0].copy )
                            field->pathname = value[0].pathname;
                        else
                        {   field->pathname.s = E_text_Z_sl_M_duplicate( value[0].pathname.s, value[0].pathname.n * 4 );
                            if( !field->pathname.s )
                            {   W(field);
                                return ~0;
                            }
                            field->pathname.n = value[0].pathname.n;
                        }
                        field->i = i.n;
                        field->u = u.n;
                        E_aml_S_object[ object_i ].data = field;
                    }else
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                }
                break;
            }
          case E_aml_Z_parse_stack_Z_entity_S_qword_field_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_qword_field_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_qword_field_finish_2:
                ret = E_aml_M_field_finish_2(8);
                if( !~ret )
                    return ~0;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_word_field_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_word_field_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_word_field_finish_2:
                ret = E_aml_M_field_finish_2(2);
                if( !~ret )
                    return ~0;
                break;
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
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value i;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                i.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                                i = E_aml_Q_object_I_to_value( object_i );
                        else
                        {   i.n = 0;
                            i.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( i.type != E_aml_Z_value_Z_type_S_number )
                            i.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    N object_i = E_aml_Q_object_R( value[0].pathname );
                    if( i.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   struct E_aml_Z_op_region *region = E_aml_S_object[ object_i ].data;
                        region->i = i.n;
                    }else
                    {   E_aml_Q_value_W( &value[0] );
                        value[0].type = E_aml_Z_value_Z_type_S_uninitialized;
                        value[0].copy = no;
                        if( !~E_aml_Q_object_W_data( object_i ))
                            return ~0;
                        E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                    }
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_op_region_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_op_region_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value l;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                l.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                                l = E_aml_Q_object_I_to_value( object_i );
                        else
                        {   l.n = 0;
                            l.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   l = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( l.type != E_aml_Z_value_Z_type_S_number )
                            l.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    if( value[0].type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   N object_i = E_aml_Q_object_R( value[0].pathname );
                        if( l.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   struct E_aml_Z_op_region *region = E_aml_S_object[ object_i ].data;
                            region->l = l.n;
                        }else
                        {   if( !~E_aml_Q_object_W_data( object_i ))
                                return ~0;
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                        }
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_power_res_finish:
                //TODO Dodać ‘power res’ do drzewa zinterpretowanej przestrzeni ACPI.
                if( !~E_aml_Q_current_path_I_pop() )
                    return ~0;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_thermal_zone_finish:
                //TODO Dodać ‘thermal zone’ do drzewa zinterpretowanej przestrzeni ACPI.
                if( !~E_aml_Q_current_path_I_pop() )
                    return ~0;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_device_finish:
                //TODO Dodać ‘device’ do drzewa zinterpretowanej przestrzeni ACPI.
                if( !~E_aml_Q_current_path_I_pop() )
                    return ~0;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_processor_finish:
                //TODO Dodać ‘processor’ do drzewa zinterpretowanej przestrzeni ACPI.
                if( !~E_aml_Q_current_path_I_pop() )
                    return ~0;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_if_op_finish_1:
                if( E_aml_S_parse_data != E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                    if( E_aml_Q_current_path_S_precompilation )
                    {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_if_op_finish_2, E_aml_Z_parse_stack_Z_entity_S_term );
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                    }else
                    {   struct E_aml_Z_value v;
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                            if( ~object_i )
                                if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                    v = E_aml_Q_object_I_to_value( object_i );
                                else
                                    v.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                            {   v.n = 0;
                                v.type = E_aml_Z_value_Z_type_S_number;
                            }
                        }else
                        {   v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                            if( v.type != E_aml_Z_value_Z_type_S_number )
                            {   ret = ~1;
                                break;
                            }
                        }
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                        if( v.n )
                        {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_if_op_finish_2, E_aml_Z_parse_stack_Z_entity_S_term );
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                        }else
                            E_aml_S_parse_data = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end;
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
            {   if( !E_aml_S_procedure_invocation_stack_n )
                {   ret = ~1;
                    break;
                }
                E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                E_aml_S_parse_data = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].return_;
                break;
            }
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
                    if( E_aml_Q_current_path_S_precompilation )
                    {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_while_op_finish_2, E_aml_Z_parse_stack_Z_entity_S_term );
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                    }else
                    {   struct E_aml_Z_value v;
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname );
                            if( ~object_i )
                                if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                    v = E_aml_Q_object_I_to_value( object_i );
                                else
                                    v.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                            {   v.n = 0;
                                v.type = E_aml_Z_value_Z_type_S_number;
                            }
                        }else
                        {   v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result;
                            if( v.type != E_aml_Z_value_Z_type_S_number )
                            {   ret = ~1;
                                break;
                            }
                        }
                        if( v.n )
                        {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.break_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end;
                            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_while_op_finish_2, E_aml_Z_parse_stack_Z_entity_S_term );
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                        }
                    }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_while_op_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_data = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.continue_;
                    E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_while_op_finish_1, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_acquire_finish:
                //TODO Dodać ‘mutex’ do drzewa zinterpretowanej przestrzeni ACPI.
                if( E_aml_S_parse_data + 2 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                E_aml_S_parse_data += 2;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_add_op_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_add_op_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_add_op_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = v_1 + v_2;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_add_op_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_add_op_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_and_op_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_and_op_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_and_op_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = v_1 & v_2;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_and_op_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_and_op_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_concat_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value *Mt_( value, E_aml_S_parse_stack_S_tmp_p_n );
                    if( !value )
                        return ~0;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p = value;
                    value[0] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                    for_n( i, E_aml_S_parse_stack_S_tmp_p_n - 1 )
                        value[ 1 + i ].copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_concat_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_concat_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    N object_0_i;
                    struct E_aml_Z_value v_0, v_1;
                    if( value[0].type == E_aml_Z_value_Z_type_S_pathname )
                    {   object_0_i = E_aml_Q_object_R( value[0].pathname );
                        if( ~object_0_i )
                            v_0 = E_aml_Q_object_I_to_value( object_0_i );
                        else
                        {   v_0.n = 0;
                            v_0.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   object_0_i = ~0;
                        v_0 = value[0];
                    }
                    if(( v_0.type == E_aml_Z_value_Z_type_S_string
                      || v_0.type == E_aml_Z_value_Z_type_S_buffer
                    )
                    && ~v_0.i
                    )
                        v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                    if( v_0.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   N object_1_i;
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                        {   object_1_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                            if( ~object_1_i )
                                v_1 = E_aml_Q_object_I_to_value( object_1_i );
                            else
                            {   v_1.n = 0;
                                v_1.type = E_aml_Z_value_Z_type_S_number;
                            }
                        }else
                        {   object_1_i = ~0;
                            v_1 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                        }
                        if( v_1.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   if( v_0.type == E_aml_Z_value_Z_type_S_number
                            || v_0.type == E_aml_Z_value_Z_type_S_string
                            || v_0.type == E_aml_Z_value_Z_type_S_buffer
                            )
                            {   struct E_aml_Z_value *e;
                                ret = E_aml_M_convert( &e, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy, &v_0, &v_1 );
                                if( !~ret )
                                    return ~0;
                                if( ret == ~1 )
                                {   if( v_0.type == E_aml_Z_value_Z_type_S_string )
                                    {   Pc s_1;
                                        if( !~object_1_i )
                                            if( v_1.type == E_aml_Z_value_Z_type_S_package )
                                                s_1 = "[Package]";
                                            else
                                                v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                                        else
                                            s_1 = E_aml_Q_object_N( object_0_i );
                                        if( v_1.type != E_aml_Z_value_Z_type_S_uninitialized )
                                        {   M_(e);
                                            if( !e )
                                                return ~0;
                                            e->p = s_1;
                                            e->copy = no;
                                            e->type = E_aml_Z_value_Z_type_S_string;
                                        }
                                    }else
                                        v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                                }
                                if( v_1.type != E_aml_Z_value_Z_type_S_uninitialized )
                                {   switch( v_0.type )
                                    { case E_aml_Z_value_Z_type_S_number:
                                        {   struct E_aml_Z_buffer buffer;
                                            buffer.n = 2 * sizeof(N);
                                            buffer.p = M( buffer.n );
                                            if( !buffer.p )
                                            {   E_aml_Q_value_W(e);
                                                W(e);
                                                return ~0;
                                            }
                                            (( N * )buffer.p )[0] = v_0.n;
                                            (( N * )buffer.p )[1] = e->n;
                                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer = buffer;
                                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_buffer;
                                            break;
                                        }
                                      case E_aml_Z_value_Z_type_S_string:
                                        {   N l_0 = E_text_Z_s0_R_l( v_0.p );
                                            N l_1 = E_text_Z_s0_R_l( e->p );
                                            Pc s = M( l_0 + l_1 + 1 );
                                            if( !s )
                                            {   E_aml_Q_value_W(e);
                                                W(e);
                                                return ~0;
                                            }
                                            E_text_Z_s_P_copy_s0_0( E_text_Z_s_P_copy_s0( s, v_0.p ), e->p );
                                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p = s;
                                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_string;
                                            break;
                                        }
                                      default:
                                        {   struct E_aml_Z_buffer buffer;
                                            buffer.n = v_0.buffer.n + e->buffer.n;
                                            buffer.p = M( buffer.n );
                                            if( !buffer.p )
                                            {   E_aml_Q_value_W(e);
                                                W(e);
                                                return ~0;
                                            }
                                            E_mem_Q_blk_I_copy( buffer.p, v_0.buffer.p, v_0.buffer.n );
                                            E_mem_Q_blk_I_copy( buffer.p + v_0.buffer.n, e->buffer.p, e->buffer.n );
                                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer = buffer;
                                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_buffer;
                                            break;
                                        }
                                    }
                                    E_aml_Q_value_W(e);
                                    W(e);
                                }
                            }else
                            {   E_aml_Q_value_W( &v_1 );
                                Pc s_0;
                                if( !~object_0_i )
                                    if( v_0.type == E_aml_Z_value_Z_type_S_package )
                                        s_0 = "[Package]";
                                    else
                                    {   ret = ~1;
                                        break;
                                    }
                                else
                                    s_0 = E_aml_Q_object_N( object_0_i );
                                Pc s_1;
                                if( !~object_1_i )
                                    if( v_1.type == E_aml_Z_value_Z_type_S_package )
                                        s_1 = "[Package]";
                                    else
                                    {   ret = ~1;
                                        break;
                                    }
                                else
                                    s_1 = E_aml_Q_object_N( object_1_i );
                                N l_0 = E_text_Z_s0_R_l( s_0 );
                                N l_1 = E_text_Z_s0_R_l( s_1 );
                                Pc s = M( l_0 + l_1 + 1 );
                                if( !s )
                                    return ~0;
                                E_text_Z_s_P_copy_s0_0( E_text_Z_s_P_copy_s0( s, s_0 ), s_1 );
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p = s;
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_string;
                            }
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                        }
                    }
                    if( v_0.type == E_aml_Z_value_Z_type_S_uninitialized
                    || v_1.type == E_aml_Z_value_Z_type_S_uninitialized
                    )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_concat_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_concat_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                    {   ret = ~1;
                        break;
                    }
                    if( !ret )
                        switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type )
                        { case E_aml_Z_value_Z_type_S_string:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    return ~0;
                                if( object_i == ~1 )
                                {   ret = ~1;
                                    break;
                                }
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                Pc s = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p;
                                E_aml_S_object[ object_i ].data = s;
                                E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                                break;
                            }
                          case E_aml_Z_value_Z_type_S_buffer:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    return ~0;
                                if( object_i == ~1 )
                                {   ret = ~1;
                                    break;
                                }
                                struct E_aml_Z_buffer *M_(buffer);
                                if( !buffer )
                                    return ~0;
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                buffer->p = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p;
                                buffer->n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n;
                                E_aml_S_object[ object_i ].data = buffer;
                                E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                                break;
                            }
                          default:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    return ~0;
                                if( object_i == ~1 )
                                {   ret = ~1;
                                    break;
                                }
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                                break;
                            }
                        }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_concat_res_finish_1:
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_concat_res_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_concat_res_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
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
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
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
                        return ~0;
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
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                    {   ret = ~1;
                        break;
                    }
                    if( !ret )
                    {   struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                        if( value[0].type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( value[0].pathname );
                            if( !~object_i )
                                break;
                            N dest_object_i = E_aml_M_clear_object();
                            if( !~dest_object_i )
                                return ~0;
                            if( dest_object_i == ~1 )
                            {   ret = ~1;
                                break;
                            }
                            struct E_aml_Z_pathname *M_(pathname);
                            if( !pathname )
                                return ~0;
                            pathname->s = E_text_Z_sl_M_duplicate( E_aml_S_object[ object_i ].name.s, E_aml_S_object[ object_i ].name.n * 4 );
                            if( !pathname->s )
                            {   W(pathname);
                                return ~0;
                            }
                            pathname->n = E_aml_S_object[ object_i ].name.n;
                            E_aml_S_object[ dest_object_i ].data = pathname;
                            E_aml_S_object[ dest_object_i ].type = E_aml_Z_object_Z_type_S_alias;
                            break;
                        }
                        N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            return ~0;
                        if( object_i == ~1 )
                        {   ret = ~1;
                            break;
                        }
                        switch( value[0].type )
                        { case E_aml_Z_value_Z_type_S_number:
                                E_aml_S_object[ object_i ].n = value[0].n;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                                break;
                          case E_aml_Z_value_Z_type_S_string:
                            {   Pc s;
                                if( value[0].copy )
                                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                    s = value[0].p;
                                }else
                                {   s = E_text_Z_s0_M_duplicate( value[0].p );
                                    if( !s )
                                        return ~0;
                                }
                                E_aml_S_object[ object_i ].data = s;
                                E_aml_S_object[ object_i ].n = value[0].i;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                                break;
                            }
                          case E_aml_Z_value_Z_type_S_buffer:
                            {   struct E_aml_Z_buffer *M_(buffer);
                                if( !buffer )
                                    return ~0;
                                if( value[0].copy )
                                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                    buffer->p = value[0].buffer.p;
                                }else
                                {   buffer->p = E_text_Z_sl_M_duplicate( value[0].buffer.p, value[0].buffer.n );
                                    if( !buffer->p )
                                    {   W(buffer);
                                        return ~0;
                                    }
                                }
                                buffer->n = value[0].buffer.n;
                                E_aml_S_object[ object_i ].data = buffer;
                                E_aml_S_object[ object_i ].n = value[0].i;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                                break;
                            }
                          case E_aml_Z_value_Z_type_S_package:
                            {   struct E_aml_Z_package *package;
                                if( value[0].copy )
                                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                    package = value[0].package;
                                }else
                                {   package = E_aml_Q_object_I_package_duplicate( value[0].package );
                                    if( !package )
                                        return ~0;
                                }
                                E_aml_S_object[ object_i ].data = package;
                                E_aml_S_object[ object_i ].n = value[0].i;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_package;
                                break;
                            }
                          default:
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                                break;
                        }
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_copy_object_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname
                    && !~E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname )
                    )
                    {   ret = ~1;
                        break;
                    }
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
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                    {   ret = ~1;
                        break;
                    }
                    if(ret)
                        break;
                    struct E_aml_Z_value v;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname );
                        if( ~object_i )
                            v = E_aml_Q_object_I_to_value( object_i );
                        else
                            v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }else
                        v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result;
                    switch( v.type )
                    { case E_aml_Z_value_Z_type_S_number:
                        {   N object_i = E_aml_M_clear_object();
                            if( !~object_i )
                                return ~0;
                            if( object_i == ~1 )
                            {   ret = ~1;
                                break;
                            }
                            E_aml_S_object[ object_i ].n = v.n;
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                            break;
                        }
                      case E_aml_Z_value_Z_type_S_string:
                        {   N object_i = E_aml_M_clear_object();
                            if( !~object_i )
                                return ~0;
                            if( object_i == ~1 )
                            {   ret = ~1;
                                break;
                            }
                            Pc s;
                            if( v.copy )
                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                s = v.p;
                            }else
                            {   s = E_text_Z_s0_M_duplicate( v.p );
                                if( !s )
                                    return ~0;
                            }
                            E_aml_S_object[ object_i ].data = s;
                            E_aml_S_object[ object_i ].n = v.i;
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                            break;
                        }
                      case E_aml_Z_value_Z_type_S_buffer:
                        {   N object_i = E_aml_M_clear_object();
                            if( !~object_i )
                                return ~0;
                            if( object_i == ~1 )
                            {   ret = ~1;
                                break;
                            }
                            struct E_aml_Z_buffer *M_(buffer);
                            if( !buffer )
                                return ~0;
                            if( v.copy )
                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                buffer->p = v.buffer.p;
                            }else
                            {   buffer->p = E_text_Z_sl_M_duplicate( v.buffer.p, v.buffer.n );
                                if( !buffer->p )
                                {   W(buffer);
                                    return ~0;
                                }
                            }
                            buffer->n = v.buffer.n;
                            E_aml_S_object[ object_i ].data = buffer;
                            E_aml_S_object[ object_i ].n = v.i;
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                            break;
                        }
                      case E_aml_Z_value_Z_type_S_package:
                        {   N object_i = E_aml_M_clear_object();
                            if( !~object_i )
                                return ~0;
                            if( object_i == ~1 )
                            {   ret = ~1;
                                break;
                            }
                            struct E_aml_Z_package *package;
                            if( v.copy )
                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                package = v.package;
                            }else
                            {   package = E_aml_Q_object_I_package_duplicate( v.package );
                                if( !package )
                                    return ~0;
                            }
                            E_aml_S_object[ object_i ].data = package;
                            E_aml_S_object[ object_i ].n = v.i;
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_package;
                            break;
                        }
                      default:
                        {   N object_i = E_aml_M_clear_object();
                            if( !~object_i )
                                return ~0;
                            if( object_i == ~1 )
                            {   ret = ~1;
                                break;
                            }
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                            break;
                        }
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_decrement_finish:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   B b = no;
                    struct E_aml_Z_pathname pathname;
                    switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type )
                    { case E_aml_Z_value_Z_type_S_arg_ref:
                        {   if( !E_aml_S_procedure_invocation_stack_n )
                            {   ret = ~1;
                                break;
                            }
                            N arg_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
                            if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].type != E_aml_Z_value_Z_type_S_pathname )
                            {   ret = ~1;
                                break;
                            }
                            pathname = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].pathname;
                            b = yes;
                            break;
                        }
                      case E_aml_Z_value_Z_type_S_local_ref:
                        {   if( !E_aml_S_procedure_invocation_stack_n )
                            {   ret = ~1;
                                break;
                            }
                            N local_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
                            if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].type != E_aml_Z_value_Z_type_S_pathname )
                            {   ret = ~1;
                                break;
                            }
                            pathname = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].pathname;
                            b = yes;
                            break;
                        }
                      case E_aml_Z_value_Z_type_S_pathname:
                            pathname = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname;
                            b = yes;
                            break;
                      default:
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                            break;
                    }
                    if(b)
                    {   N object_i = E_aml_Q_object_R(pathname);
                        if( !~object_i
                        || E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_number
                        )
                        {   ret = ~1;
                            break;
                        }
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = --E_aml_S_object[ object_i ].n;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                        break;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_derefof_finish:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                    {   ret = ~1;
                        break;
                    }
                    if( !ret )
                    {   N object_i;
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_string )
                        {   if( E_text_Z_s0_R_l( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.p ) != 4 )
                            {   ret = ~1;
                                break;
                            }
                            if( E_aml_S_current_path_n )
                                object_i = E_aml_Q_object_R_relative( E_aml_S_current_path[ E_aml_S_current_path_n - 1 ]
                                , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.p
                                );
                            else
                                object_i = E_aml_Q_object_R(( struct E_aml_Z_pathname ){ E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.p, 1 });
                        }else if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                            object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        else
                            object_i = ~0;
                        if( ~object_i
                        && ( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                          || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                          || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                          || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_package
                        ))
                        {   if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_package
                            && ~E_aml_S_object[ object_i ].n
                            )
                            {   struct E_aml_Z_package *package = E_aml_S_object[ object_i ].data;
                                if( E_aml_S_object[ object_i ].n >= package->n )
                                {   ret = ~1;
                                    break;
                                }
                            }
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_Q_object_I_to_value( object_i );
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                        }else
                        {   ret = ~1;
                            break;
                        }
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_divide_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_divide_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_divide_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                    {   if( !v_2 )
                            return ~0;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_n = v_1 % v_2;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = v_1 / v_2;
                    }else
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_divide_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_divide_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                    {   ret = ~1;
                        break;
                    }
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            return ~0;
                        if( object_i == ~1 )
                        {   ret = ~1;
                            break;
                        }
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_n;
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                        }else
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                    }
                }
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_divide_finish_4, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_divide_finish_4:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_fslb_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                v = E_aml_Q_object_I_to_value( object_i );
                            else
                                v.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v.n = 0;
                            v.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v.type != E_aml_Z_value_Z_type_S_number )
                            v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if( v.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_asm_I_bsr( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n ) + 1;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                    }else
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_fslb_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_fslb_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_fsrb_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                v = E_aml_Q_object_I_to_value( object_i );
                            else
                                v.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v.n = 0;
                            v.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                        v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    if( v.type != E_aml_Z_value_Z_type_S_number )
                        v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    if( v.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_asm_I_bsf( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.n ) + 1;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                    }else
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_fsrb_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_fsrb_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_from_bcd_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                v = E_aml_Q_object_I_to_value( object_i );
                            else
                                v.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v.n = 0;
                            v.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v.type != E_aml_Z_value_Z_type_S_number )
                            v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if( v.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   N n = 0;
                        for_n_rev( i, sizeof( v.n ))
                        {   n *= 10;
                            n += v.n >> 4;
                            n *= 10;
                            n += v.n & 0xf;
                        }
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = n;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                    }else
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_from_bcd_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_from_bcd_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_increment_finish:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   B b = no;
                    struct E_aml_Z_pathname pathname;
                    switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type )
                    { case E_aml_Z_value_Z_type_S_arg_ref:
                        {   if( !E_aml_S_procedure_invocation_stack_n )
                            {   ret = ~1;
                                break;
                            }
                            N arg_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
                            if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].type != E_aml_Z_value_Z_type_S_pathname )
                            {   ret = ~1;
                                break;
                            }
                            pathname = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].pathname;
                            b = yes;
                            break;
                        }
                      case E_aml_Z_value_Z_type_S_local_ref:
                        {   if( !E_aml_S_procedure_invocation_stack_n )
                            {   ret = ~1;
                                break;
                            }
                            N local_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
                            if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].type != E_aml_Z_value_Z_type_S_pathname )
                            {   ret = ~1;
                                break;
                            }
                            pathname = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].pathname;
                            b = yes;
                            break;
                        }
                      case E_aml_Z_value_Z_type_S_pathname:
                            pathname = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname;
                            b = yes;
                            break;
                      default:
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                            break;
                    }
                    if(b)
                    {   N object_i = E_aml_Q_object_R(pathname);
                        if( !~object_i
                        || E_aml_S_object[ object_i ].type != E_aml_Z_object_Z_type_S_number
                        )
                        {   ret = ~1;
                            break;
                        }
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = ++E_aml_S_object[ object_i ].n;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_index_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value *Mt_( value, E_aml_S_parse_stack_S_tmp_p_n );
                    if( !value )
                        return ~0;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p = value;
                    value[0] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                    for_n( i, E_aml_S_parse_stack_S_tmp_p_n - 1 )
                        value[ 1 + i ].copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_index_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_index_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value o, i;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                i = E_aml_Q_object_I_to_value( object_i );
                            else
                                i.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   i.n = 0;
                            i.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( i.type != E_aml_Z_value_Z_type_S_number )
                            i.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if( i.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                        if( value[1].type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( value[0].pathname );
                            if( ~object_i
                            && ( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                              || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                              || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_package
                            ))
                                o = E_aml_Q_object_I_to_value( object_i );
                            else
                                o.type = E_aml_Z_value_Z_type_S_uninitialized;
                        }else
                        {   o = value[0];
                            if( o.type != E_aml_Z_value_Z_type_S_string
                            && o.type != E_aml_Z_value_Z_type_S_buffer
                            && o.type != E_aml_Z_value_Z_type_S_package
                            )
                                o.type = E_aml_Z_value_Z_type_S_uninitialized;
                        }
                        if(( o.type == E_aml_Z_value_Z_type_S_string
                          || o.type == E_aml_Z_value_Z_type_S_buffer
                        )
                        && ~o.i
                        )
                            o.type = E_aml_Z_value_Z_type_S_uninitialized;
                        if( o.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   struct E_aml_Z_value *value_;
                            if( ~o.i
                            && o.type == E_aml_Z_value_Z_type_S_package
                            )
                                value_ = &o.package->value[ o.i ];
                            else
                                value_ = &o;
                            switch( value_->type )
                            { case E_aml_Z_value_Z_type_S_string:
                                    if( i.n >= E_text_Z_s0_R_l( value_->p ))
                                    {   ret = ~1;
                                        break;
                                    }
                                    break;
                              case E_aml_Z_value_Z_type_S_buffer:
                                    if( i.n >= value_->buffer.n )
                                    {   ret = ~1;
                                        break;
                                    }
                                    break;
                              default:
                                    if( i.n >= value_->package->n )
                                    {   ret = ~1;
                                        break;
                                    }
                                    break;
                            }
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = i.n;
                        }
                    }
                    if( i.type == E_aml_Z_value_Z_type_S_uninitialized
                    || o.type == E_aml_Z_value_Z_type_S_uninitialized
                    )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_index_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_index_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                    {   ret = ~1;
                        break;
                    }
                    if( !ret )
                        switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type )
                        { case E_aml_Z_value_Z_type_S_string:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    return ~0;
                                if( object_i == ~1 )
                                {   ret = ~1;
                                    break;
                                }
                                Pc s;
                                if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                    s = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p;
                                }else
                                {   s = E_text_Z_s0_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p );
                                    if( !s )
                                        return ~0;
                                }
                                E_aml_S_object[ object_i ].data = s;
                                E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                                break;
                            }
                          case E_aml_Z_value_Z_type_S_buffer:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    return ~0;
                                if( object_i == ~1 )
                                {   ret = ~1;
                                    break;
                                }
                                struct E_aml_Z_buffer *M_(buffer);
                                if( !buffer )
                                    return ~0;
                                if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                    buffer->p = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p;
                                }else
                                {   buffer->p = E_text_Z_sl_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p
                                    , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n
                                    );
                                    if( !buffer->p )
                                    {   W(buffer);
                                        return ~0;
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
                                    return ~0;
                                if( object_i == ~1 )
                                {   ret = ~1;
                                    break;
                                }
                                struct E_aml_Z_package *package;
                                if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                    package = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package;
                                }else
                                {   package = E_aml_Q_object_I_package_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.package );
                                    if( !package )
                                        return ~0;
                                }
                                E_aml_S_object[ object_i ].data = package;
                                E_aml_S_object[ object_i ].n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i;
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_package;
                                break;
                            }
                          default:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    return ~0;
                                if( object_i == ~1 )
                                {   ret = ~1;
                                    break;
                                }
                                E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                                break;
                            }
                        }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_land_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_land_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_land_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                        if( v_1 )
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = v_2;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lequal_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lequal_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lequal_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v_0, v_1;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                            )
                                v_0 = E_aml_Q_object_I_to_value( object_i );
                            else
                                v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v_0.n = 0;
                            v_0.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v_0 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v_0.type != E_aml_Z_value_Z_type_S_number
                        && v_0.type != E_aml_Z_value_Z_type_S_string
                        && v_0.type != E_aml_Z_value_Z_type_S_buffer
                        )
                            v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if(( v_0.type == E_aml_Z_value_Z_type_S_string
                      || v_0.type == E_aml_Z_value_Z_type_S_buffer
                    )
                    && ~v_0.i
                    )
                        v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                    if( v_0.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                            if( ~object_i )
                                v_1 = E_aml_Q_object_I_to_value( object_i );
                            else
                            {   v_1.n = 0;
                                v_1.type = E_aml_Z_value_Z_type_S_number;
                            }
                        }else
                            v_1 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if(( v_1.type == E_aml_Z_value_Z_type_S_string
                          || v_1.type == E_aml_Z_value_Z_type_S_buffer
                        )
                        && ~v_1.i
                        )
                            v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else if( v_1.type == E_aml_Z_value_Z_type_S_package
                        && ~v_1.i
                        )
                            v_1 = v_1.package->value[ v_1.i ];
                        if( v_1.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   struct E_aml_Z_value *e;
                            ret = E_aml_M_convert( &e, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy, &v_0, &v_1 );
                            if( !~ret )
                                return ~0;
                            if( ret == ~1 )
                                v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_Q_value_T_cmp_eq( &v_0, e );
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                                E_aml_Q_value_W(e);
                                W(e);
                            }
                        }
                    }
                    if( v_0.type == E_aml_Z_value_Z_type_S_uninitialized
                    || v_1.type == E_aml_Z_value_Z_type_S_uninitialized
                    )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lgreater_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lgreater_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lgreater_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v_0, v_1;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                            )
                                v_0 = E_aml_Q_object_I_to_value( object_i );
                            else
                                v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v_0.n = 0;
                            v_0.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v_0 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v_0.type != E_aml_Z_value_Z_type_S_number
                        && v_0.type != E_aml_Z_value_Z_type_S_string
                        && v_0.type != E_aml_Z_value_Z_type_S_buffer
                        )
                            v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if(( v_0.type == E_aml_Z_value_Z_type_S_string
                      || v_0.type == E_aml_Z_value_Z_type_S_buffer
                    )
                    && ~v_0.i
                    )
                        v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                    if( v_0.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                            if( ~object_i )
                                v_1 = E_aml_Q_object_I_to_value( object_i );
                            else
                            {   v_1.n = 0;
                                v_1.type = E_aml_Z_value_Z_type_S_number;
                            }
                        }else
                            v_1 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if(( v_1.type == E_aml_Z_value_Z_type_S_string
                          || v_1.type == E_aml_Z_value_Z_type_S_buffer
                        )
                        && ~v_1.i
                        )
                            v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else if( v_1.type == E_aml_Z_value_Z_type_S_package
                        && ~v_1.i
                        )
                            v_1 = v_1.package->value[ v_1.i ];
                        if( v_1.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   struct E_aml_Z_value *e;
                            N ret = E_aml_M_convert( &e, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy, &v_0, &v_1 );
                            if( !~ret )
                                return ~0;
                            if( ret == ~1 )
                                v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_Q_value_T_cmp_gt( &v_0, e );
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                                E_aml_Q_value_W(e);
                                W(e);
                            }
                        }
                    }
                    if( v_0.type == E_aml_Z_value_Z_type_S_uninitialized
                    || v_1.type == E_aml_Z_value_Z_type_S_uninitialized
                    )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lless_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lless_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lless_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v_0, v_1;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                            )
                                v_0 = E_aml_Q_object_I_to_value( object_i );
                            else
                                v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v_0.n = 0;
                            v_0.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v_0 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v_0.type != E_aml_Z_value_Z_type_S_number
                        && v_0.type != E_aml_Z_value_Z_type_S_string
                        && v_0.type != E_aml_Z_value_Z_type_S_buffer
                        )
                            v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if(( v_0.type == E_aml_Z_value_Z_type_S_string
                      || v_0.type == E_aml_Z_value_Z_type_S_buffer
                    )
                    && ~v_0.i
                    )
                        v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                    if( v_0.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                            if( ~object_i )
                                v_1 = E_aml_Q_object_I_to_value( object_i );
                            else
                            {   v_1.n = 0;
                                v_1.type = E_aml_Z_value_Z_type_S_number;
                            }
                        }else
                            v_1 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if(( v_1.type == E_aml_Z_value_Z_type_S_string
                          || v_1.type == E_aml_Z_value_Z_type_S_buffer
                        )
                        && ~v_1.i
                        )
                            v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else if( v_1.type == E_aml_Z_value_Z_type_S_package
                        && ~v_1.i
                        )
                            v_1 = v_1.package->value[ v_1.i ];
                        if( v_1.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   struct E_aml_Z_value *e;
                            N ret = E_aml_M_convert( &e, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy, &v_0, &v_1 );
                            if( !~ret )
                                return ~0;
                            if( ret == ~1 )
                                v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_Q_value_T_cmp_lt( &v_0, e );
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                                E_aml_Q_value_W(e);
                                W(e);
                            }
                        }
                    }
                    if( v_0.type == E_aml_Z_value_Z_type_S_uninitialized
                    || v_1.type == E_aml_Z_value_Z_type_S_uninitialized
                    )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lnotequal_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lnotequal_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lnotequal_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v_0, v_1;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                            )
                                v_0 = E_aml_Q_object_I_to_value( object_i );
                            else
                                v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v_0.n = 0;
                            v_0.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v_0 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v_0.type != E_aml_Z_value_Z_type_S_number
                        && v_0.type != E_aml_Z_value_Z_type_S_string
                        && v_0.type != E_aml_Z_value_Z_type_S_buffer
                        )
                            v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if(( v_0.type == E_aml_Z_value_Z_type_S_string
                      || v_0.type == E_aml_Z_value_Z_type_S_buffer
                    )
                    && ~v_0.i
                    )
                        v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                    if( v_0.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                            if( ~object_i )
                                v_1 = E_aml_Q_object_I_to_value( object_i );
                            else
                            {   v_1.n = 0;
                                v_1.type = E_aml_Z_value_Z_type_S_number;
                            }
                        }else
                            v_1 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if(( v_1.type == E_aml_Z_value_Z_type_S_string
                          || v_1.type == E_aml_Z_value_Z_type_S_buffer
                        )
                        && ~v_1.i
                        )
                            v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else if( v_1.type == E_aml_Z_value_Z_type_S_package
                        && ~v_1.i
                        )
                            v_1 = v_1.package->value[ v_1.i ];
                        if( v_1.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   struct E_aml_Z_value *e;
                            N ret = E_aml_M_convert( &e, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy, &v_0, &v_1 );
                            if( !~ret )
                                return ~0;
                            if( ret == ~1 )
                                v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = !E_aml_Q_value_T_cmp_eq( &v_0, e );
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                                E_aml_Q_value_W(e);
                                W(e);
                            }
                        }
                    }
                    if( v_0.type == E_aml_Z_value_Z_type_S_uninitialized
                    || v_1.type == E_aml_Z_value_Z_type_S_uninitialized
                    )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_llessequal_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_llessequal_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_llessequal_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v_0, v_1;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                            )
                                v_0 = E_aml_Q_object_I_to_value( object_i );
                            else
                                v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v_0.n = 0;
                            v_0.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v_0 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v_0.type != E_aml_Z_value_Z_type_S_number
                        && v_0.type != E_aml_Z_value_Z_type_S_string
                        && v_0.type != E_aml_Z_value_Z_type_S_buffer
                        )
                            v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if(( v_0.type == E_aml_Z_value_Z_type_S_string
                      || v_0.type == E_aml_Z_value_Z_type_S_buffer
                    )
                    && ~v_0.i
                    )
                        v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                    if( v_0.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                            if( ~object_i )
                                v_1 = E_aml_Q_object_I_to_value( object_i );
                            else
                            {   v_1.n = 0;
                                v_1.type = E_aml_Z_value_Z_type_S_number;
                            }
                        }else
                            v_1 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if(( v_1.type == E_aml_Z_value_Z_type_S_string
                          || v_1.type == E_aml_Z_value_Z_type_S_buffer
                        )
                        && ~v_1.i
                        )
                            v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else if( v_1.type == E_aml_Z_value_Z_type_S_package
                        && ~v_1.i
                        )
                            v_1 = v_1.package->value[ v_1.i ];
                        if( v_1.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   struct E_aml_Z_value *e;
                            N ret = E_aml_M_convert( &e, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy, &v_0, &v_1 );
                            if( !~ret )
                                return ~0;
                            if( ret == ~1 )
                                v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = !E_aml_Q_value_T_cmp_gt( &v_0, e );
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                                E_aml_Q_value_W(e);
                                W(e);
                            }
                        }
                    }
                    if( v_0.type == E_aml_Z_value_Z_type_S_uninitialized
                    || v_1.type == E_aml_Z_value_Z_type_S_uninitialized
                    )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lgreaterequal_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lgreaterequal_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lgreaterequal_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v_0, v_1;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                            )
                                v_0 = E_aml_Q_object_I_to_value( object_i );
                            else
                                v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v_0.n = 0;
                            v_0.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v_0 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v_0.type != E_aml_Z_value_Z_type_S_number
                        && v_0.type != E_aml_Z_value_Z_type_S_string
                        && v_0.type != E_aml_Z_value_Z_type_S_buffer
                        )
                            v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if(( v_0.type == E_aml_Z_value_Z_type_S_string
                      || v_0.type == E_aml_Z_value_Z_type_S_buffer
                    )
                    && ~v_0.i
                    )
                        v_0.type = E_aml_Z_value_Z_type_S_uninitialized;
                    if( v_0.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                            if( ~object_i )
                                v_1 = E_aml_Q_object_I_to_value( object_i );
                            else
                            {   v_1.n = 0;
                                v_1.type = E_aml_Z_value_Z_type_S_number;
                            }
                        }else
                            v_1 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if(( v_1.type == E_aml_Z_value_Z_type_S_string
                          || v_1.type == E_aml_Z_value_Z_type_S_buffer
                        )
                        && ~v_1.i
                        )
                            v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else if( v_1.type == E_aml_Z_value_Z_type_S_package
                        && ~v_1.i
                        )
                            v_1 = v_1.package->value[ v_1.i ];
                        if( v_1.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   struct E_aml_Z_value *e;
                            N ret = E_aml_M_convert( &e, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy, &v_0, &v_1 );
                            if( !~ret )
                                return ~0;
                            if( ret == ~1 )
                                v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = !E_aml_Q_value_T_cmp_lt( &v_0, e );
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                                E_aml_Q_value_W(e);
                                W(e);
                            }
                        }
                    }
                    if( v_0.type == E_aml_Z_value_Z_type_S_uninitialized
                    || v_1.type == E_aml_Z_value_Z_type_S_uninitialized
                    )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mid_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value *Mt_( value, E_aml_S_parse_stack_S_tmp_p_n );
                    if( !value )
                        return ~0;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p = value;
                    value[0] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                    for_n( i, E_aml_S_parse_stack_S_tmp_p_n - 1 )
                        value[ 1 + i ].copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_mid_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mid_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    value[1] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_mid_finish_3, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mid_finish_3:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value o, i, l;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                l = E_aml_Q_object_I_to_value( object_i );
                            else
                                l.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   l.n = 0;
                            l.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   l = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( l.type != E_aml_Z_value_Z_type_S_number )
                            l.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if( l.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                        if( value[1].type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( value[0].pathname );
                            if( ~object_i )
                                if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                                || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                                )
                                    o = E_aml_Q_object_I_to_value( object_i );
                                else
                                    o.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                            {   o.p = "";
                                o.i = ~0;
                                o.copy = no;
                                o.type = E_aml_Z_value_Z_type_S_string;
                            }
                        }else
                        {   o = value[0];
                            if( o.type != E_aml_Z_value_Z_type_S_string
                            && o.type != E_aml_Z_value_Z_type_S_buffer
                            )
                                o.type = E_aml_Z_value_Z_type_S_uninitialized;
                        }
                        if( ~o.i )
                            o.type = E_aml_Z_value_Z_type_S_uninitialized;
                        if( o.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   if( value[1].type == E_aml_Z_value_Z_type_S_pathname )
                            {   N object_i = E_aml_Q_object_R( value[0].pathname );
                                if( ~object_i )
                                    if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                        i = E_aml_Q_object_I_to_value( object_i );
                                    else
                                        i.type = E_aml_Z_value_Z_type_S_uninitialized;
                                else
                                {   i.n = 0;
                                    i.type = E_aml_Z_value_Z_type_S_number;
                                }
                            }else
                            {   i = value[1];
                                if( i.type != E_aml_Z_value_Z_type_S_number )
                                    i.type = E_aml_Z_value_Z_type_S_uninitialized;
                            }
                            if( i.type != E_aml_Z_value_Z_type_S_uninitialized )
                            {   N i_ = i.n;
                                N l_ = l.n;
                                switch( o.type )
                                { case E_aml_Z_value_Z_type_S_string:
                                    {   N l__ = E_text_Z_s0_R_l( o.p );
                                        if( l_ > l__ )
                                            l_ = l__;
                                        break;
                                    }
                                  default:
                                        if( l_ > o.buffer.n )
                                            l_ = o.buffer.n;
                                        break;
                                }
                                if( i_ > l_ )
                                    i_ = l_;
                                switch( o.type )
                                { case E_aml_Z_value_Z_type_S_string:
                                    {   Pc s = M( l_ - i_ + 1 );
                                        if( !s )
                                            return ~0;
                                        E_text_Z_s_P_copy_sl_0( s, o.buffer.p + i_, l_ - i_ );
                                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p = s;
                                        break;
                                    }
                                  default:
                                    {   Pc s = M( l_ - i_ );
                                        if( !s )
                                            return ~0;
                                        E_mem_Q_blk_I_copy( s, o.buffer.p + i_, l_ - i_ );
                                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p = s;
                                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n = l_ - i_;
                                        break;
                                    }
                                }
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = o.type;
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                            }
                        }
                    }
                    if( l.type == E_aml_Z_value_Z_type_S_uninitialized
                    || o.type == E_aml_Z_value_Z_type_S_uninitialized
                    || i.type == E_aml_Z_value_Z_type_S_uninitialized
                    )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_mid_finish_4, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mid_finish_4:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                    {   ret = ~1;
                        break;
                    }
                    if( !ret )
                        switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type )
                        { case E_aml_Z_value_Z_type_S_string:
                            {   N object_i = E_aml_M_clear_object();
                                if( !~object_i )
                                    return ~0;
                                if( object_i == ~1 )
                                {   ret = ~1;
                                    break;
                                }
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
                                    return ~0;
                                if( object_i == ~1 )
                                {   ret = ~1;
                                    break;
                                }
                                struct E_aml_Z_buffer *M_(buffer);
                                if( !buffer )
                                    return ~0;
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
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                v = E_aml_Q_object_I_to_value( object_i );
                            else
                                v.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v.n = 0;
                            v.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v.type != E_aml_Z_value_Z_type_S_number )
                            v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if( v.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = !v.n;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                    }else
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
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
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_lor_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_lor_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                        if( !v_1 )
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = v_2;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_match_finish_1:
                if( E_aml_S_parse_data + 1 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value *Mt_( value, E_aml_S_parse_stack_S_tmp_p_n );
                    if( !value )
                        return ~0;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p = value;
                    value[0] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                    value[1].n = *E_aml_S_parse_data;
                    if( value[1].n > 5 )
                    {   ret = ~1;
                        break;
                    }
                    for_n( i, E_aml_S_parse_stack_S_tmp_p_n - 1 )
                        value[ 1 + i ].copy = no;
                }
                E_aml_S_parse_data++;
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_match_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_match_finish_2:
                if( E_aml_S_parse_data + 1 > E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    value[2] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                    value[3].n = *E_aml_S_parse_data;
                    if( value[3].n > 5 )
                    {   ret = ~1;
                        break;
                    }
                }
                E_aml_S_parse_data++;
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_match_finish_3, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_match_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    value[4] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_match_finish_4, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_match_finish_4:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value start, package, match_1, match_2;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                start = E_aml_Q_object_I_to_value( object_i );
                            else
                                start.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   start.n = 0;
                            start.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   start = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( start.type != E_aml_Z_value_Z_type_S_number )
                            start.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.tmp_p;
                    if( start.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   if( value[0].type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( value[0].pathname );
                            if( ~object_i
                            && E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_package
                            )
                                package = E_aml_Q_object_I_to_value( object_i );
                            else
                                package.type = E_aml_Z_value_Z_type_S_uninitialized;
                        }else
                        {   package = value[0];
                            if( package.type != E_aml_Z_value_Z_type_S_package )
                                package.type = E_aml_Z_value_Z_type_S_uninitialized;
                        }
                        if( ~package.i )
                            package.type = E_aml_Z_value_Z_type_S_uninitialized;
                        if( package.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   if( value[2].type == E_aml_Z_value_Z_type_S_pathname )
                            {   N object_i = E_aml_Q_object_R( value[2].pathname );
                                if( ~object_i )
                                    if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                                    || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                                    || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                                    )
                                        match_1 = E_aml_Q_object_I_to_value( object_i );
                                    else
                                        match_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                                else
                                {   match_1.n = 0;
                                    match_1.type = E_aml_Z_value_Z_type_S_number;
                                }
                            }else
                            {   match_1 = value[2];
                                if( match_1.type != E_aml_Z_value_Z_type_S_number
                                && match_1.type != E_aml_Z_value_Z_type_S_string
                                && match_1.type != E_aml_Z_value_Z_type_S_buffer
                                )
                                    match_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                            }
                            if(( match_1.type == E_aml_Z_value_Z_type_S_string
                              || match_1.type == E_aml_Z_value_Z_type_S_buffer
                            )
                            && ~match_1.i
                            )
                                match_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                            if( !value[1].n
                            || match_1.type != E_aml_Z_value_Z_type_S_uninitialized
                            )
                            {   if( value[4].type == E_aml_Z_value_Z_type_S_pathname )
                                {   N object_i = E_aml_Q_object_R( value[4].pathname );
                                    if( ~object_i )
                                        if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                                        || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                                        || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                                        )
                                            match_2 = E_aml_Q_object_I_to_value( object_i );
                                        else
                                            match_2.type = E_aml_Z_value_Z_type_S_uninitialized;
                                    else
                                    {   match_2.n = 0;
                                        match_2.type = E_aml_Z_value_Z_type_S_number;
                                    }
                                }else
                                {   match_2 = value[4];
                                    if( match_2.type != E_aml_Z_value_Z_type_S_number
                                    && match_2.type != E_aml_Z_value_Z_type_S_string
                                    && match_2.type != E_aml_Z_value_Z_type_S_buffer
                                    )
                                        match_2.type = E_aml_Z_value_Z_type_S_uninitialized;
                                }
                                if(( match_2.type == E_aml_Z_value_Z_type_S_string
                                  || match_2.type == E_aml_Z_value_Z_type_S_buffer
                                )
                                && ~match_2.i
                                )
                                    match_2.type = E_aml_Z_value_Z_type_S_uninitialized;
                                if( !value[3].n
                                || match_2.type != E_aml_Z_value_Z_type_S_uninitialized
                                )
                                {   if( start.n >= package.n )
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
                                    N8 i;
                                    for( i = start.n; i != package.package->n; i++ )
                                    {   struct E_aml_Z_value *e;
                                        N ret = E_aml_M_convert( &e, package.copy, &package.package->value[i], &match_1 );
                                        if( !~ret )
                                            return ~0;
                                        if( ret == ~1 )
                                            continue;
                                        B cmp_1 = E_aml_M_cmp( value[1].n, e, &match_1 );
                                        E_aml_Q_value_W(e);
                                        W(e);
                                        if( match_1.type != match_2.type )
                                        {   N ret = E_aml_M_convert( &e, package.copy, &package.package->value[i], &match_2 );
                                            if( !~ret )
                                                return ~0;
                                            if( ret == ~1 )
                                                continue;
                                        }
                                        B cmp_2 = E_aml_M_cmp( value[3].n, e, &match_2 );
                                        E_aml_Q_value_W(e);
                                        W(e);
                                        if( cmp_1
                                        && cmp_2
                                        )
                                        {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = i;
                                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                                            break;
                                        }
                                    }
                                    if( i == package.package->n )
                                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = ~0;
                                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                                    }
                                }
                            }
                        }
                    }
                    if( start.type == E_aml_Z_value_Z_type_S_uninitialized
                    || package.type == E_aml_Z_value_Z_type_S_uninitialized
                    || ( value[1].n
                      && match_1.type == E_aml_Z_value_Z_type_S_uninitialized
                    )
                    || ( value[3].n
                      && match_2.type == E_aml_Z_value_Z_type_S_uninitialized
                    ))
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mod_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_mod_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mod_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = v_1 % v_2;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_mod_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_mod_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_multiply_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_multiply_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_multiply_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = v_1 * v_2;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_multiply_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_multiply_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_nand_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_nand_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_nand_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = ~( v_1 & v_2 );
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_nand_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_nand_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_nor_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_nor_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_nor_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = ~( v_1 | v_2 );
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_nor_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_nor_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_not_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                v = E_aml_Q_object_I_to_value( object_i );
                            else
                                v.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v.n = 0;
                            v.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v.type != E_aml_Z_value_Z_type_S_number )
                            v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if( v.type != E_aml_Z_value_Z_type_S_uninitialized )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = ~v.n;
                    else
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_Z_value_Z_type_S_uninitialized;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_not_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_not_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_object_type_finish:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname )
                    {   ret = ~1;
                        break;
                    }
                    N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                    if( ~object_i
                    && ( !( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                        || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                        || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_package
                      )
                      || !~E_aml_S_object[ object_i ].n
                    ))
                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_S_object[ object_i ].type;
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                    }else
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_or_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_or_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_or_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = v_1 | v_2;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_or_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_or_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_refof_finish:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( !~object_i )
                        {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                            break;
                        }
                    }
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = v_1 << v_2;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_shift_right_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_shift_left_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_shift_right_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = v_1 >> v_2;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_shift_right_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_shift_right_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_sizeof_finish:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   B b = no;
                    struct E_aml_Z_pathname pathname;
                    switch( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type )
                    { case E_aml_Z_value_Z_type_S_arg_ref:
                        {   if( !E_aml_S_procedure_invocation_stack_n )
                            {   ret = ~1;
                                break;
                            }
                            N arg_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
                            if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].type != E_aml_Z_value_Z_type_S_pathname )
                            {   ret = ~1;
                                break;
                            }
                            pathname = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_i ].pathname;
                            b = yes;
                            break;
                        }
                      case E_aml_Z_value_Z_type_S_local_ref:
                        {   if( !E_aml_S_procedure_invocation_stack_n )
                            {   ret = ~1;
                                break;
                            }
                            N local_i = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.i;
                            if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].type != E_aml_Z_value_Z_type_S_pathname )
                            {   ret = ~1;
                                break;
                            }
                            pathname = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[ local_i ].pathname;
                            b = yes;
                            break;
                        }
                      case E_aml_Z_value_Z_type_S_pathname:
                            pathname = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname;
                            b = yes;
                            break;
                      default:
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                            break;
                    }
                    if(b)
                    {   N object_i = E_aml_Q_object_R(pathname);
                        if( ~object_i
                        && ( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                          || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                          || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_package
                        )
                        && !~E_aml_S_object[ object_i ].n
                        )
                        {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = E_aml_Q_object_R_l( object_i );
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_number;
                        }else
                        {   ret = ~1;
                            break;
                        }
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_store_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_store_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_store_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                    {   ret = ~1;
                        break;
                    }
                    if( !ret )
                    {   //NDFN Tutaj powinna być konwersja typu.
                        struct E_aml_Z_value v;
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname );
                            if( ~object_i )
                                v = E_aml_Q_object_I_to_value( object_i );
                            else
                                v.type = E_aml_Z_value_Z_type_S_uninitialized;
                        }else
                            v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result;
                        if( v.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   N i = v.type == E_aml_Z_value_Z_type_S_number ? ~0 : v.i;
                            if( ~i
                            && v.type == E_aml_Z_value_Z_type_S_package
                            )
                            {   v = v.package->value[i];
                                i = ~0;
                            }
                            if( !~i )
                                switch( v.type )
                                { case E_aml_Z_value_Z_type_S_number:
                                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname )
                                        {   ret = ~1;
                                            break;
                                        }
                                        N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                                        if( !~object_i )
                                        {   ret = ~1;
                                            break;
                                        }
                                        if( ~E_aml_S_object[ object_i ].n )
                                            switch( E_aml_S_object[ object_i ].type )
                                            { case E_aml_Z_object_Z_type_S_string:
                                                {   Pc s = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= E_text_Z_s0_R_l(s) )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    s[ E_aml_S_object[ object_i ].n ] = v.n & 0xff;
                                                    break;
                                                }
                                              case E_aml_Z_object_Z_type_S_buffer:
                                                {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= buffer->n )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    buffer->p[ E_aml_S_object[ object_i ].n ] = v.n & 0xff;
                                                    break;
                                                }
                                              case E_aml_Z_object_Z_type_S_package:
                                                {   struct E_aml_Z_package *package = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= package->n )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    package->value[ E_aml_S_object[ object_i ].n ].n = v.n;
                                                    package->value[ E_aml_S_object[ object_i ].n ].type = E_aml_Z_value_Z_type_S_number;
                                                    break;
                                                }
                                              default:
                                                    if( !~E_aml_Q_object_W_data( object_i ))
                                                        return ~0;
                                                    E_aml_S_object[ object_i ].n = v.n;
                                                    E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                                                    break;
                                            }
                                        else
                                        {   if( !~E_aml_Q_object_W_data( object_i ))
                                                return ~0;
                                            E_aml_S_object[ object_i ].n = v.n;
                                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_number;
                                        }
                                        break;
                                    }
                                  case E_aml_Z_value_Z_type_S_string:
                                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname )
                                        {   ret = ~1;
                                            break;
                                        }
                                        N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                                        if( !~object_i )
                                        {   ret = ~1;
                                            break;
                                        }
                                        if( ~E_aml_S_object[ object_i ].n )
                                            switch( E_aml_S_object[ object_i ].type )
                                            { case E_aml_Z_object_Z_type_S_string:
                                                {   Pc s = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= E_text_Z_s0_R_l(s) )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    Pc s_ = v.p;
                                                    s[ E_aml_S_object[ object_i ].n ] = s_[0];
                                                    break;
                                                }
                                              case E_aml_Z_object_Z_type_S_buffer:
                                                {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= buffer->n )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    Pc s_ = v.p;
                                                    buffer->p[ E_aml_S_object[ object_i ].n ] = s_[0];
                                                    break;
                                                }
                                              case E_aml_Z_object_Z_type_S_package:
                                                {   struct E_aml_Z_package *package = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= package->n )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    Pc s;
                                                    if( v.copy )
                                                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                                        s = v.p;
                                                    }else
                                                    {   s = E_text_Z_s0_M_duplicate( v.p );
                                                        if( !s )
                                                            return ~0;
                                                    }
                                                    E_aml_Q_value_W( &package->value[ E_aml_S_object[ object_i ].n ] );
                                                    package->value[ E_aml_S_object[ object_i ].n ].p = s;
                                                    package->value[ E_aml_S_object[ object_i ].n ].i = ~0;
                                                    package->value[ E_aml_S_object[ object_i ].n ].type = E_aml_Z_value_Z_type_S_string;
                                                    break;
                                                }
                                              default:
                                                {   Pc s;
                                                    if( v.copy )
                                                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                                        s = v.p;
                                                    }else
                                                    {   s = E_text_Z_s0_M_duplicate( v.p );
                                                        if( !s )
                                                            return ~0;
                                                    }
                                                    if( !~E_aml_Q_object_W_data( object_i ))
                                                        return ~0;
                                                    E_aml_S_object[ object_i ].data = s;
                                                    E_aml_S_object[ object_i ].n = ~0;
                                                    E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                                                    break;
                                                }
                                            }
                                        else
                                        {   Pc s;
                                            if( v.copy )
                                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                                s = v.p;
                                            }else
                                            {   s = E_text_Z_s0_M_duplicate( v.p );
                                                if( !s )
                                                    return ~0;
                                            }
                                            if( !~E_aml_Q_object_W_data( object_i ))
                                                return ~0;
                                            E_aml_S_object[ object_i ].data = s;
                                            E_aml_S_object[ object_i ].n = ~0;
                                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                                        }
                                        break;
                                    }
                                  case E_aml_Z_value_Z_type_S_buffer:
                                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname )
                                        {   ret = ~1;
                                            break;
                                        }
                                        N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                                        if( !~object_i )
                                        {   ret = ~1;
                                            break;
                                        }
                                        if( ~E_aml_S_object[ object_i ].n )
                                            switch( E_aml_S_object[ object_i ].type )
                                            { case E_aml_Z_object_Z_type_S_string:
                                                {   Pc s = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= E_text_Z_s0_R_l(s) )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    s[ E_aml_S_object[ object_i ].n ] = v.buffer.p[0];
                                                    break;
                                                }
                                              case E_aml_Z_object_Z_type_S_buffer:
                                                {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= buffer->n )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    buffer->p[ E_aml_S_object[ object_i ].n ] = v.buffer.p[0];
                                                    break;
                                                }
                                              case E_aml_Z_object_Z_type_S_package:
                                                {   struct E_aml_Z_package *package = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= package->n )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    Pc s;
                                                    if( v.copy )
                                                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                                        s = v.buffer.p;
                                                    }else
                                                    {   s = E_text_Z_sl_M_duplicate( v.buffer.p, v.buffer.n );
                                                        if( !s )
                                                            return ~0;
                                                    }
                                                    E_aml_Q_value_W( &package->value[ E_aml_S_object[ object_i ].n ] );
                                                    package->value[ E_aml_S_object[ object_i ].n ].buffer.p = s;
                                                    package->value[ E_aml_S_object[ object_i ].n ].buffer.n = v.buffer.n;
                                                    package->value[ E_aml_S_object[ object_i ].n ].copy = yes;
                                                    package->value[ E_aml_S_object[ object_i ].n ].i = ~0;
                                                    package->value[ E_aml_S_object[ object_i ].n ].type = E_aml_Z_value_Z_type_S_buffer;
                                                    break;
                                                }
                                              default:
                                                {   struct E_aml_Z_buffer *M_(buffer);
                                                    if( !buffer )
                                                        return ~0;
                                                    if( v.copy )
                                                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                                        buffer->p = v.buffer.p;
                                                    }else
                                                    {   buffer->p = E_text_Z_sl_M_duplicate( v.buffer.p, v.buffer.n );
                                                        if( !buffer->p )
                                                            return ~0;
                                                    }
                                                    buffer->n = v.buffer.n;
                                                    if( !~E_aml_Q_object_W_data( object_i ))
                                                        return ~0;
                                                    E_aml_S_object[ object_i ].data = buffer;
                                                    E_aml_S_object[ object_i ].n = ~0;
                                                    E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                                                    break;
                                                }
                                            }
                                        else
                                        {   struct E_aml_Z_buffer *M_(buffer);
                                            if( !buffer )
                                                return ~0;
                                            if( v.copy )
                                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                                buffer->p = v.buffer.p;
                                            }else
                                            {   buffer->p = E_text_Z_sl_M_duplicate( v.buffer.p, v.buffer.n );
                                                if( !buffer->p )
                                                    return ~0;
                                            }
                                            buffer->n = v.buffer.n;
                                            if( !~E_aml_Q_object_W_data( object_i ))
                                                return ~0;
                                            E_aml_S_object[ object_i ].data = buffer;
                                            E_aml_S_object[ object_i ].n = ~0;
                                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                                        }
                                        break;
                                    }
                                  case E_aml_Z_value_Z_type_S_package:
                                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname )
                                        {   ret = ~1;
                                            break;
                                        }
                                        N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                                        if( !~object_i )
                                        {   ret = ~1;
                                            break;
                                        }
                                        if( ~E_aml_S_object[ object_i ].n )
                                            switch( E_aml_S_object[ object_i ].type )
                                            { case E_aml_Z_object_Z_type_S_string:
                                              case E_aml_Z_object_Z_type_S_buffer:
                                                    ret = ~1;
                                                    break;
                                              case E_aml_Z_object_Z_type_S_package:
                                                {   struct E_aml_Z_package *package = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= package->n )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    struct E_aml_Z_package *package_;
                                                    if( v.copy )
                                                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                                        package_ = v.package;
                                                    }else
                                                    {   package_ = E_aml_Q_object_I_package_duplicate( v.package );
                                                        if( !package_ )
                                                            return ~0;
                                                    }
                                                    E_aml_Q_value_W( &package->value[ E_aml_S_object[ object_i ].n ] );
                                                    package->value[ E_aml_S_object[ object_i ].n ].package = package_;
                                                    package->value[ E_aml_S_object[ object_i ].n ].i = ~0;
                                                    package->value[ E_aml_S_object[ object_i ].n ].type = E_aml_Z_value_Z_type_S_package;
                                                    break;
                                                }
                                              default:
                                                {   struct E_aml_Z_package *package;
                                                    if( v.copy )
                                                    {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                                        package = v.package;
                                                    }else
                                                    {   package = E_aml_Q_object_I_package_duplicate( v.package );
                                                        if( !package )
                                                            return ~0;
                                                    }
                                                    if( !~E_aml_Q_object_W_data( object_i ))
                                                        return ~0;
                                                    E_aml_S_object[ object_i ].data = package;
                                                    E_aml_S_object[ object_i ].n = ~0;
                                                    E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_package;
                                                    break;
                                                }
                                            }
                                        else
                                        {   struct E_aml_Z_package *package;
                                            if( v.copy )
                                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                                package = v.package;
                                            }else
                                            {   package = E_aml_Q_object_I_package_duplicate( v.package );
                                                if( !package )
                                                    return ~0;
                                            }
                                            if( !~E_aml_Q_object_W_data( object_i ))
                                                return ~0;
                                            E_aml_S_object[ object_i ].data = package;
                                            E_aml_S_object[ object_i ].n = ~0;
                                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_package;
                                        }
                                        break;
                                    }
                                  default:
                                        ret = ~1;
                                        break;
                                }
                            else
                                switch( v.type )
                                { case E_aml_Z_value_Z_type_S_string:
                                    {   if( i >= E_text_Z_s0_R_l( v.p ))
                                        {   ret = ~1;
                                            break;
                                        }
                                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname )
                                        {   ret = ~1;
                                            break;
                                        }
                                        N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                                        if( !~object_i )
                                        {   ret = ~1;
                                            break;
                                        }
                                        if( ~E_aml_S_object[ object_i ].n )
                                            switch( E_aml_S_object[ object_i ].type )
                                            { case E_aml_Z_object_Z_type_S_string:
                                                {   Pc s = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= E_text_Z_s0_R_l(s) )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    Pc s_ = v.p;
                                                    s[ E_aml_S_object[ object_i ].n ] = s_[i];
                                                    break;
                                                }
                                              case E_aml_Z_object_Z_type_S_buffer:
                                                {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= buffer->n )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    Pc s = v.p;
                                                    buffer->p[ E_aml_S_object[ object_i ].n ] = s[i];
                                                    break;
                                                }
                                              case E_aml_Z_object_Z_type_S_package:
                                                {   struct E_aml_Z_package *package = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= package->n )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    Pc s = v.p;
                                                    Pc s_ = M( 1 + 1 );
                                                    if( !s_ )
                                                        return ~0;
                                                    s_[0] = s[i];
                                                    s_[1] = '\0';
                                                    E_aml_Q_value_W( &package->value[ E_aml_S_object[ object_i ].n ] );
                                                    package->value[ E_aml_S_object[ object_i ].n ].p = s;
                                                    package->value[ E_aml_S_object[ object_i ].n ].copy = yes;
                                                    package->value[ E_aml_S_object[ object_i ].n ].i = ~0;
                                                    package->value[ E_aml_S_object[ object_i ].n ].type = E_aml_Z_value_Z_type_S_string;
                                                    break;
                                                }
                                              default:
                                                {   Pc s = v.p;
                                                    Pc s_ = M( 1 + 1 );
                                                    if( !s_ )
                                                        return ~0;
                                                    s_[0] = s[i];
                                                    s_[1] = '\0';
                                                    if( !~E_aml_Q_object_W_data( object_i ))
                                                        return ~0;
                                                    E_aml_S_object[ object_i ].data = s_;
                                                    E_aml_S_object[ object_i ].n = ~0;
                                                    E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                                                    break;
                                                }
                                            }
                                        else
                                        {   Pc s = v.p;
                                            Pc s_ = M( 1 + 1 );
                                            if( !s_ )
                                                return ~0;
                                            s_[0] = s[i];
                                            s_[1] = '\0';
                                            if( !~E_aml_Q_object_W_data( object_i ))
                                                return ~0;
                                            E_aml_S_object[ object_i ].data = s_;
                                            E_aml_S_object[ object_i ].n = ~0;
                                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                                        }
                                        break;
                                    }
                                  case E_aml_Z_value_Z_type_S_buffer:
                                    {   if( i >= v.buffer.n )
                                        {   ret = ~1;
                                            break;
                                        }
                                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname )
                                        {   ret = ~1;
                                            break;
                                        }
                                        N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                                        if( !~object_i )
                                        {   ret = ~1;
                                            break;
                                        }
                                        if( ~E_aml_S_object[ object_i ].n )
                                            switch( E_aml_S_object[ object_i ].type )
                                            { case E_aml_Z_object_Z_type_S_string:
                                                {   Pc s = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= E_text_Z_s0_R_l(s) )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    s[ E_aml_S_object[ object_i ].n ] = v.buffer.p[i];
                                                    break;
                                                }
                                              case E_aml_Z_object_Z_type_S_buffer:
                                                {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= buffer->n )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    buffer->p[ E_aml_S_object[ object_i ].n ] = v.buffer.p[i];
                                                    break;
                                                }
                                              case E_aml_Z_object_Z_type_S_package:
                                                {   struct E_aml_Z_package *package = E_aml_S_object[ object_i ].data;
                                                    if( E_aml_S_object[ object_i ].n >= package->n )
                                                    {   ret = ~1;
                                                        break;
                                                    }
                                                    Pc s = M(1);
                                                    if( !s )
                                                        return ~0;
                                                    s[0] = v.buffer.p[i];
                                                    E_aml_Q_value_W( &package->value[ E_aml_S_object[ object_i ].n ] );
                                                    package->value[ E_aml_S_object[ object_i ].n ].buffer.p = s;
                                                    package->value[ E_aml_S_object[ object_i ].n ].buffer.n = 1;
                                                    package->value[ E_aml_S_object[ object_i ].n ].copy = yes;
                                                    package->value[ E_aml_S_object[ object_i ].n ].i = ~0;
                                                    package->value[ E_aml_S_object[ object_i ].n ].type = E_aml_Z_value_Z_type_S_buffer;
                                                    break;
                                                }
                                              default:
                                                {   struct E_aml_Z_buffer *M_(buffer);
                                                    if( !buffer )
                                                        return ~0;
                                                    buffer->p = M(1);
                                                    if( !buffer->p )
                                                    {   W(buffer);
                                                        return ~0;
                                                    }
                                                    buffer->n = 1;
                                                    buffer->p[0] = v.buffer.p[i];
                                                    if( !~E_aml_Q_object_W_data( object_i ))
                                                        return ~0;
                                                    E_aml_S_object[ object_i ].data = buffer;
                                                    E_aml_S_object[ object_i ].n = ~0;
                                                    E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                                                    break;
                                                }
                                            }
                                        else
                                        {   struct E_aml_Z_buffer *M_(buffer);
                                            if( !buffer )
                                                return ~0;
                                            buffer->p = M(1);
                                            if( !buffer->p )
                                            {   W(buffer);
                                                return ~0;
                                            }
                                            buffer->n = 1;
                                            buffer->p[0] = v.buffer.p[i];
                                            if( !~E_aml_Q_object_W_data( object_i ))
                                                return ~0;
                                            E_aml_S_object[ object_i ].data = buffer;
                                            E_aml_S_object[ object_i ].n = ~0;
                                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                                        }
                                        break;
                                    }
                                  default:
                                        ret = ~1;
                                        break;
                                }
                        }else
                        {  N object_i = E_aml_M_clear_object();
                            if( !~object_i )
                                return ~0;
                            if( object_i == ~1 )
                            {   Pc s;
                                if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy )
                                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                                    s = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname.s;
                                }else
                                {   s = E_text_Z_sl_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname.s
                                    , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname.n * 4
                                    );
                                    if( !s )
                                        return ~0;
                                }
                                object_i = E_aml_Q_object_I_add( E_aml_Z_object_Z_type_S_uninitialized, ( struct E_aml_Z_pathname ){ s, E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname.n });
                                if( !~object_i )
                                {   if( !E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy )
                                        W(s);
                                    return ~0;
                                }
                            }
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                        }
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_subtract_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_subtract_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_subtract_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = v_1 - v_2;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_subtract_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_subtract_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_bcd_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                v = E_aml_Q_object_I_to_value( object_i );
                            else
                                v.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v.n = 0;
                            v.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v.type != E_aml_Z_value_Z_type_S_number )
                            v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if( v.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   N l = E_text_Z_n_N_bcd_G( v.n, sizeof( v.n ));
                        if( l <= sizeof( v.n ))
                        {   N ret = 0;
                            E_text_Z_n_N_bcd( (Pc)&ret + l, v.n, sizeof( v.n ), l );
                            v.n = ret;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = v;
                        }else
                            v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if( v.type == E_aml_Z_value_Z_type_S_uninitialized )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_bcd_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_bcd_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_buffer_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                            )
                                v = E_aml_Q_object_I_to_value( object_i );
                            else
                                v.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v.n = 0;
                            v.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v.type != E_aml_Z_value_Z_type_S_number
                        && v.type != E_aml_Z_value_Z_type_S_string
                        && v.type != E_aml_Z_value_Z_type_S_buffer
                        )
                            v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if(( v.type == E_aml_Z_value_Z_type_S_string
                      || v.type == E_aml_Z_value_Z_type_S_buffer
                    )
                    && ~v.i
                    )
                        v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    if( v.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   N ret = E_aml_Q_value_N_convert( E_aml_Z_value_Z_type_S_buffer, &v );
                        if( !ret )
                        {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = v;
                            if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname )
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                        }else if( ret == ~1 )
                            v.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                            return ~0;
                    }
                    if( v.type == E_aml_Z_value_Z_type_S_uninitialized )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_buffer_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_buffer_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                    {   ret = ~1;
                        break;
                    }
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            return ~0;
                        if( object_i == ~1 )
                        {   ret = ~1;
                            break;
                        }
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   struct E_aml_Z_buffer *M_(buffer);
                            if( !buffer )
                                return ~0;
                            if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                buffer->p = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p;
                            }else
                            {   buffer->p = E_text_Z_sl_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.p
                                , E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n
                                );
                                if( !buffer->p )
                                {   W(buffer);
                                    return ~0;
                                }
                            }
                            buffer->n = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.buffer.n;
                            E_aml_S_object[ object_i ].data = buffer;
                            E_aml_S_object[ object_i ].n = ~0;
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_buffer;
                        }else
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_decimal_string_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                            )
                                v = E_aml_Q_object_I_to_value( object_i );
                            else
                                v.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v.n = 0;
                            v.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v.type != E_aml_Z_value_Z_type_S_number
                        && v.type != E_aml_Z_value_Z_type_S_string
                        && v.type != E_aml_Z_value_Z_type_S_buffer
                        )
                            v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if(( v.type == E_aml_Z_value_Z_type_S_string
                      || v.type == E_aml_Z_value_Z_type_S_buffer
                    )
                    && ~v.i
                    )
                        v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    switch( v.type )
                    { case E_aml_Z_value_Z_type_S_number:
                        {   N ret = E_aml_Q_value_N_convert( E_aml_Z_value_Z_type_S_string, &v );
                            if( !ret )
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = v;
                            else if( ret == ~1 )
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                                return ~0;
                            break;
                        }
                      case E_aml_Z_value_Z_type_S_string:
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = v;
                            if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname )
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                            break;
                      case E_aml_Z_value_Z_type_S_buffer:
                        {   Pc s = M(0);
                            if( !s )
                                return ~0;
                            N l = 0;
                            for_n( i, v.buffer.n )
                            {   N l_ = E_text_Z_n_N_s_G( v.buffer.p[i], sizeof( v.buffer.p[i] ), 10 );
                                if( !E_mem_Q_blk_I_append( &s, l_ + 1 ))
                                {   W(s);
                                    return ~0;
                                }
                                l += l_;
                                s[l] = ',';
                                E_text_Z_n_N_s( s + l, v.buffer.p[i], sizeof( v.buffer.p[i] ), 10 );
                                l++;
                            }
                            if( !l )
                            {   if( !E_mem_Q_blk_I_append( &s, 1 ))
                                {   W(s);
                                    return ~0;
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
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                            break;
                    }
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_decimal_string_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_decimal_string_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                    {   ret = ~1;
                        break;
                    }
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            return ~0;
                        if( object_i == ~1 )
                        {   ret = ~1;
                            break;
                        }
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                            {   E_aml_S_object[ object_i ].data = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p;
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                            }else
                            {   Pc s = E_text_Z_s0_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p );
                                if( !s )
                                    return ~0;
                                E_aml_S_object[ object_i ].data = s;
                            }
                            E_aml_S_object[ object_i ].n = ~0;
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                        }else
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_hex_string_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                            )
                                v = E_aml_Q_object_I_to_value( object_i );
                            else
                                v.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v.n = 0;
                            v.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v.type != E_aml_Z_value_Z_type_S_number
                        && v.type != E_aml_Z_value_Z_type_S_string
                        && v.type != E_aml_Z_value_Z_type_S_buffer
                        )
                            v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if(( v.type == E_aml_Z_value_Z_type_S_string
                      || v.type == E_aml_Z_value_Z_type_S_buffer
                    )
                    && ~v.i
                    )
                        v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    switch( v.type )
                    { case E_aml_Z_value_Z_type_S_number:
                        {   N l = E_text_Z_n_N_s_G( v.n, sizeof( v.n ), 16 );
                            Pc s = M( 2 + l + 1 );
                            if( !s )
                                return ~0;
                            s[ 2 + l ] = '\0';
                            E_text_Z_n_N_s( s + 2 + l, v.n, sizeof( v.n ), 16 );
                            s[1] = 'x';
                            s[0] = '0';
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p = s;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_string;
                            break;
                        }
                      case E_aml_Z_value_Z_type_S_string:
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = v;
                            if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname )
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                            break;
                      case E_aml_Z_object_Z_type_S_buffer:
                        {   Pc s = M(0);
                            if( !s )
                                return ~0;
                            N l = 0;
                            for_n( i, v.buffer.n )
                            {   N l_ = E_text_Z_n_N_s_G( v.buffer.p[i], sizeof( v.buffer.p[i] ), 16 );
                                if( !E_mem_Q_blk_I_append( &s, l_ + 1 ))
                                {   W(s);
                                    return ~0;
                                }
                                l += l_;
                                s[l] = ',';
                                E_text_Z_n_N_s( s + l, v.buffer.p[i], sizeof( v.buffer.p[i] ), 16 );
                                l++;
                            }
                            if( !l )
                            {   if( !E_mem_Q_blk_I_append( &s, 1 ))
                                {   W(s);
                                    return ~0;
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
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                            break;
                    }
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_hex_string_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_hex_string_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                    {   ret = ~1;
                        break;
                    }
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            return ~0;
                        if( object_i == ~1 )
                        {   ret = ~1;
                            break;
                        }
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy )
                            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                                E_aml_S_object[ object_i ].data = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p;
                            }else
                            {   Pc s = E_text_Z_s0_M_duplicate( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p );
                                if( !s )
                                    return ~0;
                                E_aml_S_object[ object_i ].data = s;
                            }
                            E_aml_S_object[ object_i ].n = ~0;
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                        }else
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_integer_finish_1:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_string
                            || E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                            )
                                v = E_aml_Q_object_I_to_value( object_i );
                            else
                                v.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v.n = 0;
                            v.type = E_aml_Z_value_Z_type_S_number;
                        }
                    }else
                    {   v = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                        if( v.type != E_aml_Z_value_Z_type_S_number
                        && v.type != E_aml_Z_value_Z_type_S_string
                        && v.type != E_aml_Z_value_Z_type_S_buffer
                        )
                            v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if(( v.type == E_aml_Z_value_Z_type_S_string
                      || v.type == E_aml_Z_value_Z_type_S_buffer
                    )
                    && ~v.i
                    )
                        v.type = E_aml_Z_value_Z_type_S_uninitialized;
                    if( v.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   N ret = E_aml_Q_value_N_convert( E_aml_Z_value_Z_type_S_number, &v );
                        if( !ret )
                        {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = v;
                            if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type != E_aml_Z_value_Z_type_S_pathname )
                                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                        }else if( ret == ~1 )
                            v.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                            return ~0;
                    }
                    if( v.type == E_aml_Z_value_Z_type_S_uninitialized )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_integer_finish_2, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_integer_finish_2:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_string_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_string_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_string_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   struct E_aml_Z_value v_1, v_2;
                    if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                    {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.pathname );
                        if( ~object_i )
                            if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_buffer
                            && !~E_aml_S_object[ object_i ].n
                            )
                            {   struct E_aml_Z_buffer *buffer = E_aml_S_object[ object_i ].data;
                                v_1.buffer = *buffer;
                                v_1.i = ~0;
                                v_1.copy = no;
                                v_1.type = E_aml_Z_value_Z_type_S_buffer;
                            }else
                                v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                        else
                        {   v_1.buffer.p = "";
                            v_1.i = ~0;
                            v_1.copy = no;
                            v_1.type = E_aml_Z_value_Z_type_S_buffer;
                        }
                    }else
                    {   v_1 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result;
                        if( v_1.type != E_aml_Z_value_Z_type_S_buffer )
                            v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                    }
                    if( ~v_1.i )
                        v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                    if( v_1.type != E_aml_Z_value_Z_type_S_uninitialized )
                    {   if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.type == E_aml_Z_value_Z_type_S_pathname )
                        {   N object_i = E_aml_Q_object_R( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.pathname );
                            if( ~object_i )
                                if( E_aml_S_object[ object_i ].type == E_aml_Z_object_Z_type_S_number )
                                {   v_2.n = E_aml_S_object[ object_i ].n;
                                    v_2.type = E_aml_Z_value_Z_type_S_number;
                                }else
                                    v_2.type = E_aml_Z_value_Z_type_S_uninitialized;
                            else
                            {   v_2.n = 0;
                                v_2.type = E_aml_Z_value_Z_type_S_number;
                            }
                        }else
                        {   v_2 = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                            if( v_2.type != E_aml_Z_value_Z_type_S_number )
                                v_2.type = E_aml_Z_value_Z_type_S_uninitialized;
                        }
                        if( v_2.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   Pc p = v_1.buffer.p;
                            N l = J_min( v_1.buffer.n, v_2.n );
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
                                    return ~0;
                                E_text_Z_s_P_copy_sl_0( p_, p, s - p );
                                p = p_;
                            }
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p = p;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.i = ~0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = yes;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_string;
                        }
                    }else
                        v_1.type = E_aml_Z_value_Z_type_S_uninitialized;
                    if( v_1.type == E_aml_Z_value_Z_type_S_uninitialized
                    || v_2.type == E_aml_Z_value_Z_type_S_uninitialized
                    )
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type = E_aml_Z_value_Z_type_S_uninitialized;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_to_string_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_to_string_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N ret = E_aml_M_arg_local_debug();
                    if( !~ret )
                    {   ret = ~1;
                        break;
                    }
                    if( !ret )
                    {   N object_i = E_aml_M_clear_object();
                        if( !~object_i )
                            return ~0;
                        if( object_i == ~1 )
                        {   ret = ~1;
                            break;
                        }
                        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.type != E_aml_Z_value_Z_type_S_uninitialized )
                        {   E_aml_S_object[ object_i ].data = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.p;
                            E_aml_S_object[ object_i ].n = ~0;
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_string;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.copy = no;
                        }else
                            E_aml_S_object[ object_i ].type = E_aml_Z_object_Z_type_S_uninitialized;
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_wait_finish_1:
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_wait_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_wait_finish_2:
                //TODO Umieścić operację ‘wait’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_xor_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
                }
                E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_xor_finish_2, E_aml_Z_parse_stack_Z_entity_S_term_arg );
                break;
          case E_aml_Z_parse_stack_Z_entity_S_xor_finish_2:
                if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end )
                {   ret = ~1;
                    break;
                }
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N v_1, v_2;
                    if( ~E_aml_M_number_pathname_v2_finish_2( &v_1, &v_2 ))
                        E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result.n = v_1 ^ v_2;
                }
                if( *E_aml_S_parse_data )
                {   E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_xor_finish_3, E_aml_Z_parse_stack_Z_entity_S_supername );
                    break;
                }
                E_aml_S_parse_data++;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_xor_finish_3:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    ret = E_aml_M_number_finish_3();
                break;
          case E_aml_Z_parse_stack_Z_entity_S_fatal_finish:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                    return ~0;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_load_finish:
                //TODO Umieścić operację ‘load’ w drzewie zinterpretowanej przestrzeni ACPI.
                break;
          case E_aml_Z_parse_stack_Z_entity_S_procedure_finish:
                if( !~E_aml_Q_current_path_I_pop() )
                    return ~0;
                break;
          case E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish_1:
                if( E_aml_Q_current_path_S_precompilation )
                {
                }else
                {   N parse_stack_i = E_aml_S_parse_stack_n;
                    while( parse_stack_i >= 3
                    && E_aml_S_parse_stack[ parse_stack_i - 3 ].entity == E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish_1
                    )
                        parse_stack_i -= 2;
                    for_n( arg_n, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg ))
                        if( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_n ].type == E_aml_Z_value_Z_type_S_uninitialized )
                            break;
                    E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[ arg_n ] = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result;
                    if( parse_stack_i == E_aml_S_parse_stack_n )
                    {   E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].computing_arg = no;
                        N object_i = E_aml_Q_object_R( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].name );
                        struct E_aml_Z_object_data_Z_procedure *procedure = E_aml_S_object[ object_i ].data;
                        if( procedure->procedure )
                        {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = procedure->procedure();
                            for_n_rev( j, E_aml_S_object_n )
                                if( E_aml_S_object[j].procedure_invocation == E_aml_S_procedure_invocation_stack_n )
                                    if( !~E_aml_Q_object_W(j) )
                                        return ~0;
                            for_n( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg ))
                                E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[i] );
                            for_n_( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local ))
                                E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[i] );
                            if( !E_mem_Q_blk_I_remove( &E_aml_S_procedure_invocation_stack, --E_aml_S_procedure_invocation_stack_n, 1 ))
                                return ~0;
                        }else
                        {   E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].return_ = E_aml_S_parse_data;
                            E_aml_I_delegate( E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish_2, E_aml_Z_parse_stack_Z_entity_S_term );
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
                            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end = procedure->data_end;
                            E_aml_S_parse_data = procedure->data;
                        }
                    }
                }
                break;
          case E_aml_Z_parse_stack_Z_entity_S_procedure_invocation_finish_2:
            {   E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].execution_context.result = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].result;
                E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].result.copy = no;
                for_n_rev( j, E_aml_S_object_n )
                    if( E_aml_S_object[j].procedure_invocation == E_aml_S_procedure_invocation_stack_n )
                        if( !~E_aml_Q_object_W(j) )
                            return ~0;
                for_n( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg ))
                    E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[i] );
                for_n_( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local ))
                    E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[i] );
                if( !E_mem_Q_blk_I_remove( &E_aml_S_procedure_invocation_stack, --E_aml_S_procedure_invocation_stack_n, 1 ))
                    return ~0;
                E_aml_S_parse_data = E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].return_;
                break;
            }
        }
        if( !~ret )
            return ~0;
        if( ret == ~1 )
        {   if( !E_aml_M_res1() )
            {   ret = 0;
                goto Loop;
            }
            return ~0;
        }
        //E_font_I_print( "\nstack_n=" ); E_font_I_print_hex( E_aml_S_parse_stack_n );
        //E_font_I_print( ",stack_n_last=" ); E_font_I_print_hex( stack_n_last );
        //for_n( j, E_aml_S_parse_stack_n )
        //{   E_font_I_print( "\n-entity=" ); E_font_I_print_hex( E_aml_S_parse_stack[j].entity );
            //E_font_I_print( ",n=" ); E_font_I_print_hex( E_aml_S_parse_stack[j].n );
            //E_font_I_print( ",data_end=" ); E_font_I_print_hex( (N)E_aml_S_parse_stack[j].data_end );
            //E_font_I_print( ",last_op_if=" ); E_font_I_print_hex( E_aml_S_parse_stack[j].execution_context.last_op_if );
        //}
        // Usunięcie wykonanego “entity” po ‘push’.
        if( stack_n_last != E_aml_S_parse_stack_n
        && E_aml_S_parse_stack[ stack_n_last - 1 ].data_end // Czy element na stosie nie jest nie zainicjowanym którymś “finish”.
        && ~E_aml_S_parse_stack[ stack_n_last - 1 ].n
        && !E_aml_S_parse_stack[ stack_n_last - 1 ].n--
        )
        {   E_aml_S_parse_stack[ stack_n_last - 2 ].execution_context.tmp_p = E_aml_S_parse_stack[ stack_n_last - 1 ].execution_context.tmp_p;
            if( !E_mem_Q_blk_I_remove( &E_aml_S_parse_stack, stack_n_last - 1, 1 ))
                return ~0;
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
                return ~0;
            }
            if( !E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].data_end ) // Czy poprzedni element na stosie jest nie zainicjowanym którymś “finish”.
                E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 2 ].data_end = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end; // Zachowaj “data_end” do sprawdzenia podczas któregoś “finish”.
        }
        if( ~E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n
        && !E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n--
        )
        {   E_aml_Q_value_W( &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result );
            struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p;
            if(value)
            {   for_n( i, E_aml_S_parse_stack_S_tmp_p_n )
                    E_aml_Q_value_W( &value[i] );
                W(value);
            }
            if( !E_mem_Q_blk_I_remove( &E_aml_S_parse_stack, --E_aml_S_parse_stack_n, 1 ))
                return ~0;
        }
        if( !~E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n )
        {   E_aml_Q_value_W( &E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result );
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
            struct E_aml_Z_value *value = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p;
            if(value)
            {   for_n( i, E_aml_S_parse_stack_S_tmp_p_n )
                    E_aml_Q_value_W( &value[i] );
                W(value);
            }
            E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p = 0;
            if( E_aml_S_parse_data == E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end ) // Sprawdzanie dla listy wyliczanej w nieskończoność, czy interpretacja zakończyła się.
            {   if( !--E_aml_S_parse_stack_n )
                    break;
                if( !E_mem_Q_blk_I_remove( &E_aml_S_parse_stack, E_aml_S_parse_stack_n, 1 ))
                    return ~0;
            }
        }
        //for_n_( j, E_aml_S_parse_stack_n )
        //{   E_font_I_print( "\n-entity=" ); E_font_I_print_hex( E_aml_S_parse_stack[j].entity );
            //E_font_I_print( ",n=" ); E_font_I_print_hex( E_aml_S_parse_stack[j].n );
            //E_font_I_print( ",data_end=" ); E_font_I_print_hex( (N)E_aml_S_parse_stack[j].data_end );
        //}
        if( E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n > 1 )
        {   if( data == E_aml_S_parse_data ) // Czy kolejne “n” nic nie wykonują.
            {   if( !E_aml_M_res1() )
                    goto Loop;
                return ~0;
            }
            data = E_aml_S_parse_data;
        }
    }
    return 0;
}
_private
N
E_aml_I_procedure( struct E_aml_Z_pathname name
){  N object_i = E_aml_Q_object_R(name);
    if( !~object_i )
        return ~0;
    if( !~E_aml_E_current_path_M() )
        return ~0;
    E_aml_S_parse_stack_n = 1;
    Mt_( E_aml_S_parse_stack, E_aml_S_parse_stack_n );
    if( !E_aml_S_parse_stack )
    {   E_aml_E_current_path_W();
        return ~0;
    }
    struct E_aml_Z_object_data_Z_procedure *procedure = E_aml_S_object[ object_i ].data;
    E_aml_S_parse_data = procedure->data;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].entity = E_aml_Z_parse_stack_Z_entity_S_term;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end = procedure->data_end;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.break_ = 0;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p = 0;
    E_aml_S_procedure_invocation_stack_n = 1;
    Mt_( E_aml_S_procedure_invocation_stack, E_aml_S_procedure_invocation_stack_n );
    if( !E_aml_S_procedure_invocation_stack )
    {   W( E_aml_S_parse_stack );
        E_aml_E_current_path_W();
        return ~0;
    }
    E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].name = name;
    for_n( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg ))
        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[i].type = E_aml_Z_value_Z_type_S_uninitialized;
    for_n_( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local ))
        E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[i].type = E_aml_Z_value_Z_type_S_uninitialized;
    E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].computing_arg = no;
    E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].result.type = E_aml_Z_value_Z_type_S_uninitialized;
    E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].return_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end;
    Pc name_ = M( name.n * 4 + 1 );
    E_text_Z_s_P_copy_sl_0( name_, name.s, name.n * 4 );
    E_font_I_print( "\n,invocation=" ); E_font_I_print( name_ );
    W( name_ );
    if( !~E_aml_M_parse() )
        goto Error;
    E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].result );
    for_n_rev( j, E_aml_S_object_n )
        if( E_aml_S_object[j].procedure_invocation == E_aml_S_procedure_invocation_stack_n )
            if( !~E_aml_Q_object_W(j) )
                return ~0;
    for_n_( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg ))
        E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].arg[i] );
    for_n_( i, J_a_R_n( E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local ))
        E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[ E_aml_S_procedure_invocation_stack_n - 1 ].local[i] );
    W( E_aml_S_procedure_invocation_stack );
    W( E_aml_S_parse_stack );
    E_aml_E_current_path_W();
    return 0;
Error:
    for_n_rev_( j, E_aml_S_object_n )
        if( E_aml_S_object[j].procedure_invocation )
            if( !~E_aml_Q_object_W(j) )
                return ~0;
    for_n_( i, E_aml_S_procedure_invocation_stack_n )
    {   for_n( j, J_a_R_n( E_aml_S_procedure_invocation_stack[i].arg ))
            E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[i].arg[j] );
        for_n_( j, J_a_R_n( E_aml_S_procedure_invocation_stack[i].local ))
            E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[i].local[j] );
    }
    W( E_aml_S_procedure_invocation_stack );
    for_n_( i, E_aml_S_parse_stack_n )
    {   E_aml_Q_value_W( &E_aml_S_parse_stack[i].execution_context.result );
        struct E_aml_Z_value *value = E_aml_S_parse_stack[i].execution_context.tmp_p;
        if(value)
        {   for_n( i, E_aml_S_parse_stack_S_tmp_p_n )
                E_aml_Q_value_W( &value[i] );
            W(value);
        }
    }
    W( E_aml_S_parse_stack );
    E_aml_E_current_path_W();
    E_font_I_print( "\nerror 2" );
    return ~0;
}
_private
N
E_aml_M( Pc table
, N l
){  if( !~E_aml_E_current_path_M() )
        return ~0;
    E_aml_S_parse_stack_n = 1;
    Mt_( E_aml_S_parse_stack, E_aml_S_parse_stack_n );
    if( !E_aml_S_parse_stack )
    {   E_aml_E_current_path_W();
        return ~0;
    }
    if( !~E_aml_E_object_M() )
    {   W( E_aml_S_parse_stack );
        E_aml_E_current_path_W();
        return ~0;
    }
    E_aml_S_parse_data = table;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].entity = E_aml_Z_parse_stack_Z_entity_S_term;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].n = ~0;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.last_op_if = 0;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].data_end = table + l;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.continue_ = E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.break_ = 0;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.result.copy = no;
    E_aml_S_parse_stack[ E_aml_S_parse_stack_n - 1 ].execution_context.tmp_p = 0;
    E_aml_S_procedure_invocation_stack_n = 0;
    Mt_( E_aml_S_procedure_invocation_stack, E_aml_S_procedure_invocation_stack_n );
    if( !E_aml_S_procedure_invocation_stack )
    {   E_aml_E_object_W();
        W( E_aml_S_parse_stack );
        E_aml_E_current_path_W();
        return ~0;
    }
    E_aml_Q_current_path_S_precompilation = no;
    E_aml_Q_procedure_invocation_stack_S_invokeing = no;
    if( !~E_aml_M_parse() )
        goto Error;
    W( E_aml_S_procedure_invocation_stack );
    W( E_aml_S_parse_stack );
    E_aml_E_current_path_W();
    E_aml_Q_procedure_invocation_stack_S_invokeing = yes;
    E_aml_I_procedure(( struct E_aml_Z_pathname ){ "_SB__INI", 2 });
    return 0;
Error:
    E_aml_E_object_W();
    for_n( i, E_aml_S_procedure_invocation_stack_n )
    {   for_n( j, J_a_R_n( E_aml_S_procedure_invocation_stack[i].arg ))
            E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[i].arg[j] );
        for_n_( j, J_a_R_n( E_aml_S_procedure_invocation_stack[i].local ))
            E_aml_Q_value_W( &E_aml_S_procedure_invocation_stack[i].local[j] );
    }
    W( E_aml_S_procedure_invocation_stack );
    for_n_( i, E_aml_S_parse_stack_n )
    {   E_aml_Q_value_W( &E_aml_S_parse_stack[i].execution_context.result );
        struct E_aml_Z_value *value = E_aml_S_parse_stack[i].execution_context.tmp_p;
        if(value)
        {   for_n( i, E_aml_S_parse_stack_S_tmp_p_n )
                E_aml_Q_value_W( &value[i] );
            W(value);
        }
    }
    W( E_aml_S_parse_stack );
    E_aml_E_current_path_W();
    E_font_I_print( "\nerror" );
    return ~0;
}
/******************************************************************************/
