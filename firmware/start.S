
#include "include/light.h"

	.section .entry, "ax", %progbits
	.align 3
	.globl _start
_start:
    /* Disable indirect branch prediction once entering into uboot world */
    li t0, 0x117f
    csrw 0x7c1, t0
    /* Disable fence broadcase and HW TLB */
    li t0, 0x66e30c
    csrw 0x7c5, t0
    /* Enable cache ASAP as LIGHT's requirement */
    /* icache enable */
    /* mhcr is 0x7c1 */
	csrr x29, 0x7c1
	ori x28, x29, 0x1
	csrw 0x7c1, x28

	/* dcache enable */
	csrr x29, 0x7c1
	ori x28, x29, 0x2
	csrw 0x7c1, x28

    la  t0, _trap_entry
    csrw    mtvec, t0

    /* mask all interrupts */
    csrw    mie, zero

    /* set MSIE bit to receive IPIs */
    #li  t0, MIE_MSIE
    #csrs    mie, t0

	/* setup stack */
    li  t0, -16
    la  t1, g_top_stack
    and sp, t1, t0      /* force 16 byte alignment */
    mv  gp, sp

	jal bringup_prepare
/*
 * never return
 */


    .section .entry, "ax", %progbits
    .align 3
    .globl _trap_entry
_trap_entry:
    wfi
    j   _trap_entry

.section .stack
    .align  4
    .global g_base_stack
    .global g_top_stack
g_base_stack:
    .space 0x400
g_top_stack:
