/*******************************************************************************
*   ___   public
*  ¦OUX¦  GNU x86_64 assembly
*  ¦/C+¦  OUX/C+ OS
*   ---   kernel
*         interrupt handlers
* ©overcq                on ‟Gentoo Linux 23.0” “x86_64”              2025‒6‒4 V
*******************************************************************************/
.global E_interrupt_I_exeception_de
.global E_interrupt_I_exeception_db
.global E_interrupt_I_exeception_nmi
.global E_interrupt_I_exeception_bp
.global E_interrupt_I_exeception_of
.global E_interrupt_I_exeception_br
.global E_interrupt_I_exeception_ud
.global E_interrupt_I_exeception_nm
.global E_interrupt_I_exeception_df
.global E_interrupt_I_exeception_ts
.global E_interrupt_I_exeception_np
.global E_interrupt_I_exeception_ss
.global E_interrupt_I_exeception_gp
.global E_interrupt_I_exeception_pf
.global E_interrupt_I_exeception_mf
.global E_interrupt_I_exeception_ac
.global E_interrupt_I_exeception_mc
.global E_interrupt_I_exeception_xm
.global E_interrupt_I_exeception_ve
.global E_interrupt_I_exeception_cp
.global E_interrupt_I_external
.global E_interrupt_I_external_spurious
//==============================================================================
.data
msg_0:    .asciz "\nexception de"
msg_1:    .asciz "\nexception db"
msg_2:    .asciz "\nexception nmi"
msg_3:    .asciz "\nexception bp"
msg_4:    .asciz "\nexception of"
msg_5:    .asciz "\nexception br"
msg_6:    .asciz "\nexception ud"
msg_7:    .asciz "\nexception nm"
msg_8:    .asciz "\nexception df"
msg_10:   .asciz "\nexception ts"
msg_11:   .asciz "\nexception np"
msg_12:   .asciz "\nexception ss"
msg_13:   .asciz "\nexception gp"
msg_14:   .asciz "\nexception pf"
msg_16:   .asciz "\nexception mf"
msg_17:   .asciz "\nexception ac"
msg_18:   .asciz "\nexception mc"
msg_19:   .asciz "\nexception xm"
msg_20:   .asciz "\nexception ve"
msg_21:   .asciz "\nexception cp"
msg_x:    .asciz "\nexception spurious"
//==============================================================================
.text
E_interrupt_I_exeception_de:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_0-.-3(%rip),%rdi
    call    E_font_I_print
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
E_interrupt_I_exeception_db:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_1-.-3(%rip),%rdi
    call    E_font_I_print
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
E_interrupt_I_exeception_nmi:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_2-.-3(%rip),%rdi
    call    E_font_I_print
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
E_interrupt_I_exeception_bp:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_3-.-3(%rip),%rdi
    call    E_font_I_print
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
E_interrupt_I_exeception_of:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_4-.-3(%rip),%rdi
    call    E_font_I_print
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
E_interrupt_I_exeception_br:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_5-.-3(%rip),%rdi
    call    E_font_I_print
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
E_interrupt_I_exeception_ud:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_6-.-3(%rip),%rdi
    call    E_font_I_print
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
E_interrupt_I_exeception_nm:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_7-.-3(%rip),%rdi
    call    E_font_I_print
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
E_interrupt_I_exeception_df:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_8-.-3(%rip),%rdi
    call    E_font_I_print
    mov     7*8(%rsp),%rdi
    call    E_font_I_print_hex
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    lea     8(%rsp),%rsp
    iret
E_interrupt_I_exeception_ts:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_10-.-3(%rip),%rdi
    call    E_font_I_print
    mov     7*8(%rsp),%rdi
    call    E_font_I_print_hex
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    lea     8(%rsp),%rsp
    iret
E_interrupt_I_exeception_np:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_11-.-3(%rip),%rdi
    call    E_font_I_print
    mov     7*8(%rsp),%rdi
    call    E_font_I_print_hex
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    lea     8(%rsp),%rsp
    iret
E_interrupt_I_exeception_ss:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_12-.-3(%rip),%rdi
    call    E_font_I_print
    mov     7*8(%rsp),%rdi
    call    E_font_I_print_hex
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    lea     8(%rsp),%rsp
    iret
E_interrupt_I_exeception_gp:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_13-.-3(%rip),%rdi
    call    E_font_I_print
    mov     7*8(%rsp),%rdi
    call    E_font_I_print_hex
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    lea     8(%rsp),%rsp
    iret
E_interrupt_I_exeception_pf:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_14-.-3(%rip),%rdi
    call    E_font_I_print
    mov     7*8(%rsp),%rdi
    call    E_font_I_print_hex
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    lea     8(%rsp),%rsp
    iret
E_interrupt_I_exeception_mf:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_16-.-3(%rip),%rdi
    call    E_font_I_print
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
E_interrupt_I_exeception_ac:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_17-.-3(%rip),%rdi
    call    E_font_I_print
    mov     7*8(%rsp),%rdi
    call    E_font_I_print_hex
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    lea     8(%rsp),%rsp
    iret
E_interrupt_I_exeception_mc:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_18-.-3(%rip),%rdi
    call    E_font_I_print
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
E_interrupt_I_exeception_xm:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_19-.-3(%rip),%rdi
    call    E_font_I_print
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
E_interrupt_I_exeception_ve:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_20-.-3(%rip),%rdi
    call    E_font_I_print
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
E_interrupt_I_exeception_cp:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_21-.-3(%rip),%rdi
    call    E_font_I_print
    mov     7*8(%rsp),%rdi
    call    E_font_I_print_hex
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    lea     8(%rsp),%rsp
    iret
E_interrupt_I_external:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    mov     $0xb,%rdi
    mov     $0,%rsi
    call    E_interrupt_Q_local_apic_P
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
E_interrupt_I_external_spurious:
    push    %rax
    push    %rcx
    push    %rdx
    push    %rbx
    push    %rbp
    push    %rsi
    push    %rdi
    lea     msg_x-.-3(%rip),%rdi
    call    E_font_I_print
    pop     %rdi
    pop     %rsi
    pop     %rbp
    pop     %rbx
    pop     %rdx
    pop     %rcx
    pop     %rax
    iret
/******************************************************************************/
