/*
* Copyright (C) 2017-2020 Alibaba Group Holding Limited
*
* SPDX-License-Identifier: GPL-2.0+
*/
#include "include/asm/csr.h"
#include "include/asm/io.h"
#include "include/asm/barrier.h"
#include "include/light.h"
extern void setup_ddr_pmp(void);
extern void cpu_performance_enable(void);
extern void clk_reinit(void);
extern void mdelay(int ms);
extern void plic_reinit(void);
extern void iopmp_reinit(void);
extern int get_str_flag(void);
extern int chip_lp_mode_get_iopmp_flag(void);
extern int chip_lp_mode_set_iopmp_flag(int enable);
extern int chip_lp_mode_get_ddr_flag(void);
extern int chip_lp_mode_set_ddr_flag(int enable);
extern unsigned long get_sbi_addr(void);
extern void bringup_prepare(void);
#define CONFIG_SYS_CACHELINE_SIZE 64

void invalidate_icache_all(void)
{
	asm volatile ("fence.i" ::: "memory");
}

void invalid_dcache_range(unsigned long start, unsigned long end)
{
	register unsigned long i asm("a0") = start & ~(CONFIG_SYS_CACHELINE_SIZE - 1);

	for (; i < end; i += CONFIG_SYS_CACHELINE_SIZE)
		asm volatile(".long 0x02a5000b");  /* dcache.ipa a0 */

	sync_is();
}

void bringup_prepare(void)
{
    void (*fp)(unsigned long);
    unsigned long run_addr;

    if (get_str_flag()) {
        clk_reinit();
        mdelay(1);

        plic_reinit();

        iopmp_reinit();

        //set iopmp flag, notify aon sys is restored
        chip_lp_mode_set_iopmp_flag(1);

        //wait ddr restore complete
        while(!chip_lp_mode_get_ddr_flag());

        cpu_performance_enable();

        /* Check success, read jump address and jump */
        run_addr = get_sbi_addr();

        fp = (void (*)(unsigned long ))((unsigned long *)(run_addr));

        invalid_dcache_range(0, 0x1fffff);
        invalidate_icache_all();
        setup_ddr_pmp();

        (*fp)(0);
    }

    return;
}

