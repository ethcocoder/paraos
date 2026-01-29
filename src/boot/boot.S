/* ParadoxOS Bootloader - Video Enabled */

.set MAGIC, 0x1BADB002
.set KLUDGE, 1<<16
.set VIDEO, 1<<2  /* Request Video Mode */
.set FLAGS, KLUDGE | VIDEO | 3 /* ALIGN | MEMINFO | VIDEO | KLUDGE */
.set CHECKSUM, -(MAGIC + FLAGS)

.section .text
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

/* AOUT Kludge - Hardcoded addresses matching verified working structure */
.long 0x100000          /* header_addr: link address */
.long 0x100000          /* load_addr: start of physical memory load */
.long 0                 /* load_end_addr: 0 = load whole file */
.long 0                 /* bss_end_addr: 0 */
.long _start            /* entry_addr */

/* Video Request Fields */
.long 0          /* Mode: 0 = Linear Framebuffer */
.long 1024       /* Width */
.long 768        /* Height */
.long 32         /* BPP */

.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start
_start:
	/* Stack setup */
	movl $stack_top, %esp
    
    /* Push multiboot info pointer and magic */
    pushl %ebx  /* Multiboot info structure pointer */
    pushl %eax  /* Multiboot magic */

	call _kernel_main
    
	cli
1:	hlt
	jmp 1b
