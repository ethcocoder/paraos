/* Minimal Multiboot Kernel with Kludge */
.set MAGIC, 0x1BADB002
.set KLUDGE, 1<<16
.set FLAGS, KLUDGE
.set CHECKSUM, -(MAGIC + FLAGS)

.section .text
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

/* Kludge Fields */
.long 0x100000 /* header_addr */
.long 0x100000 /* load_addr */
.long 0 /* load_end_addr */
.long 0 /* bss_end_addr */
.long 0x100020 /* entry_addr (approx offset) */

.global _start
_start:
    movl $0x2F4B2F4F, 0xB8000
    cli
1:  hlt
    jmp 1b
