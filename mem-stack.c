/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         task stacks
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”             2025‒6‒14 J
*******************************************************************************/
#define E_cpu_Z_page_entry_S_present    ( 1ULL << 0 )
#define E_cpu_Z_page_entry_S_write      ( 1ULL << 1 )
//==============================================================================
_private N E_mem_stack_S_additional_pages;
//==============================================================================
_internal
void
E_mem_Q_stack_I_patch_page_table_I_next_page( P *address
){  if( E_mem_blk_S.reserved_from_end )
        *address = (Pc)*address - E_mem_S_page_size;
    else
        *address = (Pc)*address + E_mem_S_page_size;
}
_private
void
E_mem_Q_stack_I_patch_page_table_add_guard( N guard_page
, N additional_i
){  E_flow_I_lock( &E_mem_blk_S_mem_lock );
    N allocated_i = E_mem_Q_blk_R( E_main_S_page_table );
    N page_table_size = E_mem_blk_S.allocated[ allocated_i ].n * E_mem_blk_S.allocated[ allocated_i ].u;
    E_flow_I_unlock( &E_mem_blk_S_mem_lock );
    P address = E_mem_blk_S.reserved_from_end ? (P)( (Pc)E_main_S_page_table + page_table_size - E_mem_S_page_size ) : E_main_S_page_table;
    N additional;
    const N table_n = E_mem_S_page_size / sizeof(N);
    for_n( pml4_i, table_n )
    {   E_mem_Q_stack_I_patch_page_table_I_next_page( &address );
        for_n( pdpt_i, table_n )
        {   E_mem_Q_stack_I_patch_page_table_I_next_page( &address );
            for_n( pd_i, table_n )
            {   E_mem_Q_stack_I_patch_page_table_I_next_page( &address );
                volatile Pn pt = address;
                for_n( pt_i, table_n )
                {   N virtual_address = ( pml4_i * ( 1ULL << 39 )) | ( pdpt_i * ( 1 << 30 )) | ( pd_i * ( 1 << 21 )) | ( pt_i * E_mem_S_page_size );
                    if( virtual_address == guard_page )
                    {   additional = pt[ pt_i ];
                        pt[ pt_i ] = 0;
                    }else if( virtual_address == E_mem_blk_S.memory_size + additional_i * E_mem_S_page_size )
                    {   pt[ pt_i ] = additional;
                        return;
                    }
                }
            }
        }
    }
    _unreachable;
}
_private
void
E_mem_Q_stack_I_patch_page_table_remove_guard( N guard_page
, N additional_i
){  E_flow_I_lock( &E_mem_blk_S_mem_lock );
    N allocated_i = E_mem_Q_blk_R( E_main_S_page_table );
    N page_table_size = E_mem_blk_S.allocated[ allocated_i ].n * E_mem_blk_S.allocated[ allocated_i ].u;
    E_flow_I_unlock( &E_mem_blk_S_mem_lock );
    P address = E_mem_blk_S.reserved_from_end ? (P)( (Pc)E_main_S_page_table + page_table_size - E_mem_S_page_size ) : E_main_S_page_table;
    N additional;
    const N table_n = E_mem_S_page_size / sizeof(N);
    for_n( pml4_i, table_n )
    {   E_mem_Q_stack_I_patch_page_table_I_next_page( &address );
        for_n( pdpt_i, table_n )
        {   E_mem_Q_stack_I_patch_page_table_I_next_page( &address );
            for_n( pd_i, table_n )
            {   E_mem_Q_stack_I_patch_page_table_I_next_page( &address );
                volatile Pn pt = address;
                for_n( pt_i, table_n )
                {   N virtual_address = ( pml4_i * ( 1ULL << 39 )) | ( pdpt_i * ( 1 << 30 )) | ( pd_i * ( 1 << 21 )) | ( pt_i * E_mem_S_page_size );
                    if( virtual_address == E_mem_blk_S.memory_size + additional_i * E_mem_S_page_size )
                    {   additional = pt[ pt_i ];
                        pt[ pt_i ] = 0;
                        goto Exit_for;
                    }
                }
            }
        }
    }
Exit_for:
    address = E_mem_blk_S.reserved_from_end ? (P)( (Pc)E_main_S_page_table + page_table_size - E_mem_S_page_size ) : E_main_S_page_table;
    for_n_( pml4_i, table_n )
    {   E_mem_Q_stack_I_patch_page_table_I_next_page( &address );
        for_n( pdpt_i, table_n )
        {   E_mem_Q_stack_I_patch_page_table_I_next_page( &address );
            for_n( pd_i, table_n )
            {   E_mem_Q_stack_I_patch_page_table_I_next_page( &address );
                volatile Pn pt = address;
                for_n( pt_i, table_n )
                {   N virtual_address = ( pml4_i * ( 1ULL << 39 )) | ( pdpt_i * ( 1 << 30 )) | ( pd_i * ( 1 << 21 )) | ( pt_i * E_mem_S_page_size );
                    if( virtual_address == guard_page )
                    {   pt[ pt_i ] = additional;
                        return;
                    }
                }
            }
        }
    }
    _unreachable;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_private
void
E_mem_Q_stack_I_page_fault( N address
){  address = E_simple_Z_n_I_align_down_to_v2( address, E_mem_S_page_size );
    N sched_i = E_flow_I_current_scheduler();
    for_each( task_id, E_flow_S_scheduler[ sched_i ].task, E_mem_Q_tab )
    {   struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, task_id );
        if( (N)task->stack == address )
        {   E_font_I_print( "\nLarger stack required for task: " ); E_font_I_print_hex( task_id );
                #ifdef C_line_report
            E_font_I_print( ", " );
            E_font_I_print( task->proc_name );
                #endif
            break;
        }
    }
    E_main_I_error_fatal();
}
/******************************************************************************/
