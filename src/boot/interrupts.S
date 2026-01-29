.section .text
.global _idt_flush
_idt_flush:
    movl 4(%esp), %eax
    lidt (%eax)
    ret

.global _isr_common_stub
_isr_common_stub:
    pusha
    movw %ds, %ax
    pushl %eax
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    call _isr_handler

    popl %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    popa
    addl $8, %esp
    iret

# IRQ stub
.global _irq_common_stub
_irq_common_stub:
    pusha
    movw %ds, %ax
    pushl %eax
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    call _irq_handler

    popl %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    popa
    addl $8, %esp
    iret

# Specific handlers
.global _isr0
_isr0:
    pushl $0
    pushl $0
    jmp _isr_common_stub

.global _irq1
_irq1:
    pushl $0
    pushl $33
    jmp _irq_common_stub
