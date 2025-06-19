/*******************************************************************************
*   ___   public
*  ¦OUX¦  C
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         flow driver
* ©overcq                on ‟Gentoo Linux 13.0” “x86_64”             2015‒1‒26
*******************************************************************************/
enum E_flow_Z_run_state
{ E_flow_Z_run_state_S_ready
, E_flow_Z_run_state_S_waiting_for_report
, E_flow_Z_run_state_S_waiting_for_timer
, E_flow_Z_run_state_S_waiting_for_call_reply
, E_flow_Z_run_state_S_stopping_by_task
};
struct E_flow_Q_task_Z
{ Pc exe_stack;
  Pc stack;
    #ifdef C_line_report
  Pc proc_name;
    #endif
  I run_state_object;
  enum E_flow_Z_run_state run_state;
};
_private
struct E_flow_Z_scheduler
{ struct E_mem_Q_tab_Z *task;
  I current_task;
  struct E_mem_Q_tab_Z *report;
  struct E_mem_Q_tab_Z *timer;
  N last_time;
  N next_time;
  B U_R( signal, exit );
} *E_flow_S_scheduler;
_private N E_flow_S_scheduler_n;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct E_flow_Q_report_Z
{ N uid;
  N reported_count;
};
struct E_flow_Q_timer_Z
{ N left;
  N period;
  N lost_count;
  N uid;
  I task_to;
};
//==============================================================================
_internal void E_flow_Q_task_I_switch(I);
//==============================================================================
_private
N
E_flow_I_current_time( void
){  N32 high, low;
    __asm__ volatile (
    "\n"    "rdtsc"
    : "=d" (high), "=a" (low)
    );
    return ( (N)high << 32 ) | low;
}
_private
N8
E_flow_I_current_scheduler( void
){  return E_interrupt_Q_local_apic_R(2) >> 24;
}
_private
void
E_flow_I_sleep( N microseconds
){  N time = E_interrupt_S_cpu_freq * microseconds / 1000000;
    N acpi_timer_ticks = time / E_interrupt_S_apic_timer_tick_time;
    if( acpi_timer_ticks )
    {   __asm__ volatile (
            #if defined( __x86_64__ )
        "\n"    "cli"
            #else
#error not implemented
            #endif
        );
        E_interrupt_Q_local_apic_P( 0x38, acpi_timer_ticks );
        __asm__ volatile (
            #if defined( __x86_64__ )
        "\n"    "sti"
        "\n"    "hlt"
            #else
#error not implemented
            #endif
        );
    }
}
//------------------------------------------------------------------------------
_private
void
E_flow_I_lock( N8 *lock
){  __asm__ volatile (
    "\n"    "mov    $1,%%cl"
    "\n0:"  "xor    %%al,%%al"
    "\n"    "pause"
    "\n"    "lock cmpxchg %%cl,%0"
    "\n"    "jne    0b"
    :
    : "p" (lock)
    : "al", "cl"
    );
}
_private
void
E_flow_I_unlock( N8 *lock
){  *lock = 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Wymuszenie ‘prealokacji’ “stosu” ‹zadania›, by nie została wywołana procedura obsługi “sygnału” ‘uniksowego’ “SEGV” w trakcie zmieniania pamięci ‘alokowanej’ dynamicznie, z której korzysta.
_internal
void
E_flow_Q_task_I_touch_stack( N page_count
){  N pages = page_count ? page_count : 1; //CONF
    volatile Pc sp;
    __asm__ volatile (
    #if defined( __x86_64__ )
    "\n" "mov   %%rsp,%0"
    #else
#error not implemented
    #endif
    : "=r" (sp)
    );
    // Dlatego kolejno “strony” pamięci, ponieważ jest tylko jedna zabezpieczająca na dole “stosu” ‹zadania›.
    while( pages-- )
    {   sp -= E_mem_S_page_size;
        *sp = 0;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_private
N
E_flow_M( P main_stack
){  N sched_i = E_flow_I_current_scheduler();
    E_flow_S_scheduler[ sched_i ].task = E_mem_Q_tab_M( sizeof( struct E_flow_Q_task_Z ), 1 );
    if( !E_flow_S_scheduler[ sched_i ].task )
        return ~0;
    E_flow_S_scheduler[ sched_i ].current_task = 0;
    struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, E_flow_S_scheduler[ sched_i ].current_task );
    *task = ( struct E_flow_Q_task_Z )
    { .run_state = E_flow_Z_run_state_S_ready
    , .stack = main_stack
        #ifdef C_line_report
    , .proc_name = "main"
        #endif
    };
    E_mem_Q_stack_I_patch_page_table_set_guard( (N)main_stack, E_flow_S_scheduler[ sched_i ].current_task );
    E_flow_S_scheduler[ sched_i ].report = E_mem_Q_tab_M( sizeof( struct E_flow_Q_report_Z ), 0 );
    if( !E_flow_S_scheduler[ sched_i ].report )
    {   W( E_flow_S_scheduler[ sched_i ].task );
        return ~0;
    }
    E_flow_S_scheduler[ sched_i ].timer = E_mem_Q_tab_M( sizeof( struct E_flow_Q_timer_Z ), 0 );
    if( !E_flow_S_scheduler[ sched_i ].timer )
    {   W( E_flow_S_scheduler[ sched_i ].report );
        W( E_flow_S_scheduler[ sched_i ].task );
        return ~0;
    }
    E_flow_S_scheduler[ sched_i ].last_time = 0;
    E_flow_S_scheduler[ sched_i ].next_time = ~0;
    U_L( E_flow_S_scheduler[ sched_i ].signal, exit );
    return 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ‹Zadanie› synchroniczne wykonuje to po wyjściu z procedury; ma adres powrotu na stosie.
_internal
void
E_flow_Q_task_I_stop( void
){  N sched_i = E_flow_I_current_scheduler();
    struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, E_flow_S_scheduler[ sched_i ].current_task );
    E_flow_Q_task_I_switch( task->run_state_object ); // Powrót do ‹zadania› zwalniającego bieżące.
    _unreachable;
}
//------------------------------------------------------------------------------
_export
I
E_flow_Q_task_M( I *uid
, void (*task_proc)(void)
    #ifdef C_line_report
, Pc task_proc_name
    #endif
){  Pc stack = Mt( E_mem_S_page_size, 2 );
    if( !stack )
        return ~0;
    Pc exe_stack = stack + 2 * E_mem_S_page_size;
    N sched_i = E_flow_I_current_scheduler();
    E_flow_Q_task_I_touch_stack(0);
    I task_id = E_mem_Q_tab_I_add( E_flow_S_scheduler[ sched_i ].task );
    E_mem_Q_stack_I_patch_page_table_set_guard( (N)stack, task_id );
    struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, task_id );
    task->stack = stack;
        #ifdef C_line_report
    task->proc_name = task_proc_name;
        #endif
    task->run_state = E_flow_Z_run_state_S_ready;
    task->exe_stack = 0;
    task->run_state_object = E_flow_S_scheduler[ sched_i ].current_task;
    P *p = ( P * )exe_stack - 1;
    *p = (P)&E_flow_Q_task_I_stop;
    E_flow_Q_task_I_switch( task_id );
    if( !task->exe_stack ) // W bloku – nowe ‹zadanie›: nic nie zmieniać na stosie należącym do przełączanego.
    {   __asm__ volatile (
            #if defined( __x86_64__ )
        "\n" "mov   %0,%%rsp"
        "\n" "jmp   *%1"
        :
        : "r" (p), "r" ( task_proc )
            #else
#error not implemented
            #endif
        );
        _unreachable;
    }
    *uid = task_id;
    return task_id;
}
_export
void
E_flow_Q_task_W( I *uid
){  I id = *uid;
    *uid = ~0;
    N sched_i = E_flow_I_current_scheduler();
    struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, id );
    task->run_state = E_flow_Z_run_state_S_stopping_by_task;
    task->run_state_object = E_flow_S_scheduler[ sched_i ].current_task;
    E_flow_Q_task_I_switch(id); // Przełącz tylko po to, by ‹zadanie› zwalniane zwolniło zasoby; również stosowo, hierarchicznie z powrotem przełączając przy zwalnianiu ‹zadań› przez siebie uruchomionych.
    task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, id );
    E_flow_Q_task_I_touch_stack(0);
    E_mem_Q_stack_I_patch_page_table_remove_guard( (N)task->stack, id );
    W( task->stack );
    E_mem_Q_tab_I_remove( E_flow_S_scheduler[ sched_i ].task, id );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_export
I
E_flow_Q_report_M( N uid
){  N sched_i = E_flow_I_current_scheduler();
    for_each( report_id, E_flow_S_scheduler[ sched_i ].report, E_mem_Q_tab )
    {   struct E_flow_Q_report_Z *report = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].report, report_id );
        if( report->uid == uid )
            break;
    }
    if( !~report_id )
    {   report_id = E_mem_Q_tab_I_add( E_flow_S_scheduler[ sched_i ].report );
        struct E_flow_Q_report_Z *report = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].report, report_id );
        report->uid = uid;
        report->reported_count = 0;
    }
    return report_id;
}
_export
void
E_flow_Q_report_W( I id
){  N sched_i = E_flow_I_current_scheduler();
    E_mem_Q_tab_I_remove( E_flow_S_scheduler[ sched_i ].report, id );
}
//------------------------------------------------------------------------------
_export
void
E_flow_Q_report_I_signal( I id
){  N sched_i = E_flow_I_current_scheduler();
    struct E_flow_Q_report_Z *report = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].report, id );
    if( ~report->reported_count )
        report->reported_count++;
    for_each( task_id, E_flow_S_scheduler[ sched_i ].task, E_mem_Q_tab )
    {   struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, task_id );
        if( task->run_state == E_flow_Z_run_state_S_waiting_for_report
        && task->run_state_object == id
        )
        {   task->run_state = E_flow_Z_run_state_S_ready;
            break;
        }
    }
}
_export
B
E_flow_Q_report_I_wait( I id
, N *lost_count
){  B ret;
    N sched_i = E_flow_I_current_scheduler();
    struct E_flow_Q_report_Z *report = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].report, id );
    if( report->reported_count )
    {   //for_each( task_id, E_flow_S_scheduler[ sched_i ].task, E_mem_Q_tab )
        //{   struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, task_id );
            //if( task->run_state == E_flow_Z_run_state_S_emiting_report
            //&& task->run_state_object == id
            //)
                //task->run_state = E_flow_Z_run_state_S_ready;
        //}
        ret = no; // Nie wywołuje “schedule”, ponieważ w przełączanym tylko w oznaczonych punktach przepływie wykonania — bieżące ‹zadanie› mogło umożliwić zaistnienie ‹raportu›, na który czeka, tylko wtedy, jeśli przełącza do innych ‹zadań› ·w innych punktach niż to oczekiwanie na ‹raport›·, więc po co czekać, skoro nie zaburza cyklu przełączania ‹zadań›, a tylko w implementacji własnego ‹zadania› zmienia na złożoną (przesuniętą) sekwencję przełączania.
    }else
    {   struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, E_flow_S_scheduler[ sched_i ].current_task );
        task->run_state = E_flow_Z_run_state_S_waiting_for_report;
        task->run_state_object = id;
        ret = E_flow_Q_task_I_schedule();
        report = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].report, id );
    }
    if( lost_count )
        *lost_count = report->reported_count - 1;
    report->reported_count = 0;
    return ret;
}
_export
void
E_flow_Q_report_I_clear( I id
){  N sched_i = E_flow_I_current_scheduler();
    struct E_flow_Q_report_Z *report = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].report, id );
    report->reported_count = 0;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_export
I
E_flow_Q_timer_M( N miliseconds
){  N time = E_flow_I_current_time();
    N sched_i = E_flow_I_current_scheduler();
    I timer_id = E_mem_Q_tab_I_add( E_flow_S_scheduler[ sched_i ].timer );
    struct E_flow_Q_timer_Z *timer = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].timer, timer_id );
    timer->period = E_interrupt_S_cpu_freq * miliseconds / 1000;
    timer->lost_count = 0;
    timer->uid = ~0;
    timer->task_to = E_flow_S_scheduler[ sched_i ].current_task;
    if( E_mem_Q_tab_R_n( E_flow_S_scheduler[ sched_i ].timer ) != 1 )
    {   for_each_out( timer_id, timer_id_, E_flow_S_scheduler[ sched_i ].timer, E_mem_Q_tab )
        {   struct E_flow_Q_timer_Z *timer_ = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].timer, timer_id_ );
            if( timer_->period // Jest co najmniej jeden ‹cykler›
            || timer_->left // lub co najmniej jeden wzbudzony ‹impulsator›.
            )
            {   time += timer->period;
                if( E_flow_S_scheduler[ sched_i ].next_time > time )
                    E_flow_S_scheduler[ sched_i ].next_time = time;
                timer->left = time - E_flow_S_scheduler[ sched_i ].last_time;
                return timer_id;
            }
        }
    }
    E_flow_S_scheduler[ sched_i ].last_time = time;
    E_flow_S_scheduler[ sched_i ].next_time = time + timer->period;
    timer->left = timer->period;
    return timer_id;
}
_export
void
E_flow_Q_timer_W( I id
){  N sched_i = E_flow_I_current_scheduler();
    E_mem_Q_tab_I_remove( E_flow_S_scheduler[ sched_i ].timer, id );
    for_each( timer_id, E_flow_S_scheduler[ sched_i ].timer, E_mem_Q_tab )
    {   struct E_flow_Q_timer_Z *timer = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].timer, timer_id );
        if( timer->period
        || timer->left
        )
            return;
    }
    E_flow_S_scheduler[ sched_i ].next_time = ~0;
}
//------------------------------------------------------------------------------
_export
B
E_flow_Q_timer_I_wait( I id
, N *lost_count
){  N sched_i = E_flow_I_current_scheduler();
    struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, E_flow_S_scheduler[ sched_i ].current_task );
    task->run_state = E_flow_Z_run_state_S_waiting_for_timer;
    task->run_state_object = id;
    B ret = E_flow_Q_task_I_schedule();
    struct E_flow_Q_timer_Z *timer = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].timer, id );
    if( lost_count )
        *lost_count = timer->lost_count;
    timer->lost_count = 0;
    return ret;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
_export
I
E_flow_Q_impulser_M( N uid
){  struct E_flow_Q_timer_Z *timer;
    N sched_i = E_flow_I_current_scheduler();
    for_each( timer_id, E_flow_S_scheduler[ sched_i ].timer, E_mem_Q_tab )
    {   timer = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].timer, timer_id );
        if( timer->uid == uid )
            break;
    }
    if( !~timer_id )
    {   timer_id = E_mem_Q_tab_I_add( E_flow_S_scheduler[ sched_i ].timer );
        timer = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].timer, timer_id );
        timer->left = timer->period = 0;
        timer->uid = uid;
    }
    timer->task_to = E_flow_S_scheduler[ sched_i ].current_task;
    return timer_id;
}
_export
I
E_flow_Q_impulser_M_srv( N uid
){  N sched_i = E_flow_I_current_scheduler();
    for_each( timer_id, E_flow_S_scheduler[ sched_i ].timer, E_mem_Q_tab )
    {   struct E_flow_Q_timer_Z *timer = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].timer, timer_id );
        if( timer->uid == uid )
            break;
    }
    if( !~timer_id )
    {   timer_id = E_mem_Q_tab_I_add( E_flow_S_scheduler[ sched_i ].timer );
        struct E_flow_Q_timer_Z *timer = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].timer, timer_id );
        timer->left = timer->period = 0;
        timer->uid = uid;
    }
    return timer_id;
}
//------------------------------------------------------------------------------
_export
void
E_flow_Q_impulser_I_activate( I id
, N miliseconds
){  N time = E_flow_I_current_time();
    N sched_i = E_flow_I_current_scheduler();
    struct E_flow_Q_timer_Z *timer = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].timer, id );
    timer->left = E_interrupt_S_cpu_freq * miliseconds / 1000;
    if( E_mem_Q_tab_R_n( E_flow_S_scheduler[ sched_i ].timer ) != 1 )
    {   for_each_out( id, timer_id_, E_flow_S_scheduler[ sched_i ].timer, E_mem_Q_tab )
        {   struct E_flow_Q_timer_Z *timer_ = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].timer, timer_id_ );
            if( timer_->period
            || timer_->left
            )
            {   time += timer->left;
                if( E_flow_S_scheduler[ sched_i ].next_time > time )
                    E_flow_S_scheduler[ sched_i ].next_time = time;
                timer->left = time - E_flow_S_scheduler[ sched_i ].last_time;
                return;
            }
        }
    }
    E_flow_S_scheduler[ sched_i ].last_time = time;
    E_flow_S_scheduler[ sched_i ].next_time = time + timer->left;
}
_export
void
E_flow_Q_impulser_I_deactivate( I id
){  N sched_i = E_flow_I_current_scheduler();
    struct E_flow_Q_timer_Z *timer = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].timer, id );
    if( !timer->left )
        return;
    timer->left = 0;
    for_each_out( id, timer_id_, E_flow_S_scheduler[ sched_i ].timer, E_mem_Q_tab )
    {   struct E_flow_Q_timer_Z *timer_ = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].timer, timer_id_ );
        if( timer_->period
        || timer_->left
        )
            return;
    }
    E_flow_S_scheduler[ sched_i ].next_time = ~0;
}
//------------------------------------------------------------------------------
//NDFN Dodać “lost_count”?
_export
B
E_flow_Q_impulser_I_wait( I id
){  N sched_i = E_flow_I_current_scheduler();
    struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, E_flow_S_scheduler[ sched_i ].current_task );
    task->run_state = E_flow_Z_run_state_S_waiting_for_timer;
    task->run_state_object = id;
    return E_flow_Q_task_I_schedule();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Każde ‹zadanie› synchroniczne po wywołaniu “E_flow_Q_task_I_schedule” i po przełączeniu w tej procedurze do innego ‹zadania› (“E_flow_Q_task_I_switch”) czeka przed instrukcją powrotu z tej procedury, by kontynuować w miejscu wywołania i ewentualnie zakończyć własne ‹zadanie› po powrocie.
_export
__attribute__ (( __noinline__, __returns_twice__, __hot__ ))
B
E_flow_Q_task_I_schedule( void
){  N sched_i = E_flow_I_current_scheduler();
    O{  if( U_E( E_flow_S_scheduler[ sched_i ].signal, exit ))
        {   struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, 0 );
            task->run_state = E_flow_Z_run_state_S_stopping_by_task;
            E_flow_Q_task_I_switch(0);
            task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, E_flow_S_scheduler[ sched_i ].current_task );
            return task->run_state == E_flow_Z_run_state_S_stopping_by_task;
        }
        N time = E_flow_I_current_time();
        if( time >= E_flow_S_scheduler[ sched_i ].next_time ) // Czy trzeba uaktualnić kolejne czasy ‹cyklerów›.
        {   N elapsed_time = time - E_flow_S_scheduler[ sched_i ].last_time;
            //NDFN Uzupełnić o jakieś przewidywanie ‘overhead’ na podstawie poprzedniego, by wyeliminować możliwość powtarzania pętli w pesymistycznym przypadku dla każdego ‹cyklera›? Ale obliczać ten czas tylko wtedy, jeżeli ten fragment nie będzie mógł być wywłaszczony z wykonywania w czasie rzeczywistym (wszystkie przerwania wyłączone).
            O{  B some_timer_is_active = no, some_timer_has_deactivated = no;
                B some_task_got_ready = no;
                N suspend_time = ~0;
                for_each( timer_id, E_flow_S_scheduler[ sched_i ].timer, E_mem_Q_tab )
                {   struct E_flow_Q_timer_Z *timer = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].timer, timer_id );
                    if( timer->period ) // ‹cykler›.
                    {   if( elapsed_time >= timer->left ) // ‹cykler› wykonał obieg— ‹zadanie› do wznowienia.
                        {   N overlate_time = elapsed_time - timer->left;
                            if( overlate_time >= timer->period ) // ‹cykler› wykonał więcej niż jeden obieg.
                                do
                                {   timer->lost_count++;
                                    overlate_time -= timer->period;
                                }while( overlate_time >= timer->period );
                            timer->left = timer->period - overlate_time;
                            struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, timer->task_to );
                            if( task->run_state == E_flow_Z_run_state_S_waiting_for_timer
                            && task->run_state_object == timer_id
                            )
                            {   task->run_state = E_flow_Z_run_state_S_ready;
                                some_task_got_ready = yes;
                            }else
                                timer->lost_count++;
                        }else
                            timer->left -= elapsed_time;
                        if( suspend_time > timer->left )
                        {   suspend_time = timer->left;
                            some_timer_is_active = yes;
                        }
                    }else if( timer->left ) // Aktywowany ‹impulsator›.
                    {   if( elapsed_time >= timer->left )
                        {   timer->left = 0;
                            struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, timer->task_to );
                            if( task->run_state == E_flow_Z_run_state_S_waiting_for_timer
                            && task->run_state_object == timer_id
                            )
                            {   task->run_state = E_flow_Z_run_state_S_ready;
                                some_task_got_ready = yes;
                            } // Narazie – jeśli nie może wznowić ‹zadania›, to gubi impuls.
                            some_timer_has_deactivated = yes;
                        }else
                        {   timer->left -= elapsed_time;
                            if( suspend_time > timer->left )
                            {   suspend_time = timer->left;
                                some_timer_is_active = yes;
                            }
                        }
                    }
                }
                if( some_timer_has_deactivated
                && !some_timer_is_active
                ){  E_flow_S_scheduler[ sched_i ].next_time = ~0;
                    break;
                }
                N time_2 = E_flow_I_current_time();
                elapsed_time = time_2 - time;
                if( elapsed_time < suspend_time // Czy przeliczanie czasów ‹cyklerów› trwało krócej niż obliczony czas oczekiwania do pierwszego budzącego ‹zadanie›?
                || !some_task_got_ready
                ){  E_flow_S_scheduler[ sched_i ].last_time = time;
                    E_flow_S_scheduler[ sched_i ].next_time = time + suspend_time; //NDFN Rozważyć przepełnienie licznika czasu rzeczywistego.
                    break;
                }
                time = time_2;
            }
        }
        I task_iter = E_mem_Q_tab_Q_iter_M( E_flow_S_scheduler[ sched_i ].task, E_flow_S_scheduler[ sched_i ].current_task );
        if( !~task_iter )
            E_main_I_error_fatal();
        for_each_q( task_id, E_flow_S_scheduler[ sched_i ].task, task_iter, E_mem_Q_tab )
        {   B task_skip = no;
            for_each( task_id_, E_flow_S_scheduler[ sched_i ].task, E_mem_Q_tab )
            {   struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, task_id_ );
                if( task->run_state == E_flow_Z_run_state_S_stopping_by_task
                && task->run_state_object == task_id
                ) // Nie przełączaj do ‹zadania›, które jest w trakcie wyrzucania innego.
                {   task_skip = yes;
                    break;
                }
            }
            if( !task_skip )
            {   struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, task_id );
                if( task->run_state == E_flow_Z_run_state_S_ready )
                {   E_mem_Q_tab_Q_iter_W( E_flow_S_scheduler[ sched_i ].task, task_iter );
                    E_flow_Q_task_I_switch( task_id );
                    task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, E_flow_S_scheduler[ sched_i ].current_task );
                    return task->run_state == E_flow_Z_run_state_S_stopping_by_task;
                }
            }
        }
        B task_skip = no;
        for_each( task_id_, E_flow_S_scheduler[ sched_i ].task, E_mem_Q_tab )
        {   struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, task_id_ );
            if( task->run_state == E_flow_Z_run_state_S_stopping_by_task
            && task->run_state_object == E_flow_S_scheduler[ sched_i ].current_task
            )
            {   task_skip = yes;
                break;
            }
        }
        if( !task_skip )
        {   struct E_flow_Q_task_Z *task = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, E_flow_S_scheduler[ sched_i ].current_task );
            if( task->run_state == E_flow_Z_run_state_S_ready )
                return task->run_state == E_flow_Z_run_state_S_stopping_by_task;
        }
        if( U_R( E_flow_S_scheduler[ sched_i ].signal, exit ))
            continue;
        B U_has_suspend_time = ~E_flow_S_scheduler[ sched_i ].next_time;
        if( U_has_suspend_time )
            time = E_flow_I_current_time();
        if( !U_has_suspend_time
        || time < E_flow_S_scheduler[ sched_i ].next_time
        ){  if( U_has_suspend_time )
            {   time = E_flow_S_scheduler[ sched_i ].next_time - time;
                N acpi_timer_ticks = time / E_interrupt_S_apic_timer_tick_time;
                if( !acpi_timer_ticks )
                    continue;
                __asm__ volatile (
                    #if defined( __x86_64__ )
                "\n"    "cli"
                    #else
#error not implemented
                    #endif
                );
                E_interrupt_Q_local_apic_P( 0x38, acpi_timer_ticks );
                __asm__ volatile (
                    #if defined( __x86_64__ )
                "\n"    "sti"
                    #else
#error not implemented
                    #endif
                );
            }
            __asm__ volatile (
                #if defined( __x86_64__ )
            "\n"    "hlt"
                #else
#error not implemented
                #endif
            );
        }
    }
}
// Przełączenie do ‹zadania› przez przełączenie wskaźnika “stosu” wykonania.
_internal
__attribute__ (( __noinline__, __returns_twice__, __hot__ ))
void
E_flow_Q_task_I_switch( I task_to_id
){  //feclearexcept( FE_ALL_EXCEPT );
    N sched_i = E_flow_I_current_scheduler();
    struct E_flow_Q_task_Z *task_from = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, E_flow_S_scheduler[ sched_i ].current_task );
    struct E_flow_Q_task_Z *task_to = E_mem_Q_tab_R( E_flow_S_scheduler[ sched_i ].task, task_to_id );
    E_flow_S_scheduler[ sched_i ].current_task = task_to_id;
    __asm__ volatile (
        #if defined( __x86_64__ )
    "\n" "mov       %%rsp,%0"
    "\n" "test      %1,%1"
    "\n" "cmovnz    %1,%%rsp"
    : "=m" ( task_from->exe_stack )
    : "r" ( task_to->exe_stack )
    : "cc", "memory"
//CONF Jest FPU?
    , "st", "st(1)", "st(2)", "st(3)", "st(4)", "st(5)", "st(6)", "st(7)"
            #ifdef __MMX__
    , "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7"
                #ifdef __SSE__
    , "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7"
                #endif
            #endif
            #if defined( __i386__ )
    , "ebx", "ecx", "esi", "edi"
            #else
    , "rbx", "rcx", "rsi", "rdi"
    , "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
                #ifdef __SSE3__
    , "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15"
                    #ifdef __AVX__
    , "ymm0", "ymm1", "ymm2", "ymm3", "ymm4", "ymm5", "ymm6", "ymm7", "ymm8", "ymm9", "ymm10", "ymm11", "ymm12", "ymm13", "ymm14", "ymm15"
                    #endif
                #endif
            #endif
        #else
#error not implemented
        #endif
    );
}
_private
__attribute__ (( __noreturn__ ))
void
E_flow_I_main_task( void
){  O{  __asm__ volatile (
        "\n"    "hlt"
        );
    }
}
/******************************************************************************/
