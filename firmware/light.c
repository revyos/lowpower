/*
* Copyright (C) 2017-2020 Alibaba Group Holding Limited
*
* SPDX-License-Identifier: GPL-2.0+
*/

#include "include/asm/csr.h"
#include "include/asm/io.h"
#include "include/asm/barrier.h"
#include "include/light.h"

void mdelay(int ms)
{
	long i;
	long tick = (long)ms*1000*CPU_CLK_MHZ;
	for (i = 0; i < tick; i++);
}

void release_dsp_tcm(void)
{
    unsigned int tmp;
    tmp = readl((void *)DSPSYS_SW_RST);
    tmp |= (0x1<<10)|(0x1<<9)|(0x1<<8); // release dsp0
    tmp |= (0x1<<14)|(0x1<<13)|(0x1<<12); // release dsp1
    writel(tmp,(void *)DSPSYS_SW_RST);
}
void setup_ddr_pmp(void)
{
    /* clear pmp entry0,entry1 setting in bootrom */
    writel(0x0 >> 12, (void *)(PMP_BASE_ADDR + 0x104));
    writel(0x0 >> 12, (void *)(PMP_BASE_ADDR + 0x100));
    writel(0x0 >> 12, (void *)(PMP_BASE_ADDR + 0x10c));
    writel(0x0 >> 12, (void *)(PMP_BASE_ADDR + 0x108));

    writel(0, (void *)(PMP_BASE_ADDR + 0x000));

    sync_is();
}

void cpu_performance_enable(void)
{
#define CSR_MHINT2_E    0x7cc
#define CSR_MHINT4  0x7ce
    csr_write(CSR_SMPEN, 0x1);
    csr_write(CSR_MHINT2_E, csr_read(CSR_MHINT2_E) | 0x20000);
    csr_write(CSR_MHINT4, csr_read(CSR_MHINT4) | 0x410);
    csr_write(CSR_MCCR2, 0xe2490009);
    csr_write(CSR_MHCR, 0x117f); // clear bit7 to disable indirect brantch prediction
    csr_write(CSR_MXSTATUS, 0x638000);
    csr_write(CSR_MHINT, 0x6e30c | (1<<21) | (1<<22)); // set bit21 & bit 22 to close tlb & fence broadcast
}

void clk_reinit(void)
{
    unsigned int tmp;
    tmp = readl((void *)LIGHT_APCLK_ADDRBASE + 0x220);
    tmp |= 0x7;
    writel(tmp, (void *)LIGHT_APCLK_ADDRBASE + 0x220);

    /* AP rst_gen: VP/VO/VI/DSP */
    writel(0xf, (void *)LIGHT_APSYS_RSTGEN_ADDRBASE + 0x220);

    /* enable dsp0/1_cclk, dsp0/1_pclk */
    tmp = readl((void *)LIGHT_DSP_SUBSYS_ADDRBASE + 0x24);
    tmp |= 0xf;
    writel(tmp, (void *)LIGHT_DSP_SUBSYS_ADDRBASE + 0x24);

    /* enable gpu_core_clk, gpu_cfg_aclk */
    tmp = readl((void *)LIGHT_VO_SUBSYS_ADDRBASE + 0x50);
    tmp |= 0x18;
    writel(tmp, (void *)LIGHT_VO_SUBSYS_ADDRBASE + 0x50);

    tmp = readl((void *)LIGHT_VO_SUBSYS_R_ADDRBASE + 0x50);
    tmp |= 0x3ff;
    writel(tmp, (void *)LIGHT_VO_SUBSYS_R_ADDRBASE + 0x50);

    /* enable dpu_pixelclk0/1, dpu_hclk, dpu_aclk, dpu_cclk */
    tmp = readl((void *)LIGHT_VO_SUBSYS_ADDRBASE + 0x50);
    tmp |= 0x3e0;
    writel(tmp, (void *)LIGHT_VO_SUBSYS_ADDRBASE + 0x50);

    /* enable npu_axi_aclk, npu_core_clk */
    tmp = readl((void *)LIGHT_APCLK_ADDRBASE + 0x1c8);
    tmp |= 0x30;
    writel(tmp, (void *)LIGHT_APCLK_ADDRBASE + 0x1c8);

}

void plic_reinit(void)
{
	//config plic ctrl
    writel(1, (void *)0xFFD81FFFFC);
}

void iopmp_reinit(void)
{
	//Set PMP dummy start addr
	writel(0xFFCB01E, (void *)(AO_IOPMP_BASE + 0x008));
	// Set PMP start addr, start from 0x0
	writel(0x0 ,(void *)(AO_IOPMP_BASE + 0x280));
	// Set PMP end addr, end at 3G
	writel(0xC0000, (void *)(AO_IOPMP_BASE + 0x284));
	// Set PMP_CFG, disable WR
	writel(0x0, (void *)(AO_IOPMP_BASE + 0x080));

	//config aon sys iopmp config
	writel(0xffffffff, (void *)(AO_IOPMP_BASE + 0xc0));

}

#define LP_NONE             (0)
#define LP_HW_VAD           (1 << 16)
#define LP_STANDBY          (2 << 16)

/* REG_AON_CHIP_LP_MODE bit define */
/*
|  31~16  |  2 ~ 15 |    1       |     0    |
| LP MODE | Reserve | IOPMP FLAG | DDR FLAG |
*/
#define CHIP_LP_MODE_OFFSET   (16)
#define CHIP_LP_MODE_MASK   (0xFFFFUL << CHIP_LP_MODE_OFFSET)

/* ddr resume complete flag */
#define CHIP_LP_MODE_DDR_FLAG_OFFSET (0)
#define CHIP_LP_MODE_DDR_FLAG_MASK (1UL << CHIP_LP_MODE_DDR_FLAG_OFFSET)
/* iopmp resume complete flag */
#define CHIP_LP_MODE_IOPMP_FLAG_OFFSET (1)
#define CHIP_LP_MODE_IOPMP_FLAG_MASK (1UL << CHIP_LP_MODE_IOPMP_FLAG_OFFSET)

int get_str_flag(void)
{
    if ((readl((void *)REG_AON_STR_INDICATOR_0) == 0x5a5a5a5a) &&
        (readl((void *)REG_AON_STR_INDICATOR_1) == 0x12345678) &&
        (readl((void *)REG_AON_STR_INDICATOR_2) == 0x32fde438) &&
        (readl((void *)REG_AON_STR_INDICATOR_3) == 0x8ab4c52c))
	{
        return 1;
	}
	return 0;
}

int chip_lp_mode_get_iopmp_flag(void)
{
    return ((readl((void *)REG_AON_CHIP_LP_MODE) & CHIP_LP_MODE_IOPMP_FLAG_MASK) >> CHIP_LP_MODE_IOPMP_FLAG_OFFSET);
}

int chip_lp_mode_set_iopmp_flag(int enable)
{
    unsigned int data = readl((void *)REG_AON_CHIP_LP_MODE);
    if (enable)
    {
        data |= CHIP_LP_MODE_IOPMP_FLAG_MASK;
    }
    else
    {
        data &= ~CHIP_LP_MODE_IOPMP_FLAG_MASK;
    }

    writel(data, ((void *)REG_AON_CHIP_LP_MODE));

    return 0;
}

int chip_lp_mode_get_ddr_flag(void)
{
    return ((readl((void *)REG_AON_CHIP_LP_MODE) & CHIP_LP_MODE_DDR_FLAG_MASK) >> CHIP_LP_MODE_DDR_FLAG_OFFSET);
}

int chip_lp_mode_set_ddr_flag(int enable)
{
    unsigned int data = readl((void *)REG_AON_CHIP_LP_MODE);
    if (enable)
    {
        data |= CHIP_LP_MODE_DDR_FLAG_MASK;
    }
    else
    {
        data &= ~CHIP_LP_MODE_DDR_FLAG_MASK;
    }

    writel(data, ((void *)REG_AON_CHIP_LP_MODE));

    return 0;
}

unsigned long get_sbi_addr(void)
{
	unsigned long addr;
	addr = (( (unsigned long )*(volatile unsigned int *)(REG_AON_RESERVED_REG_3)) << 32) |
           (*(volatile unsigned int *)(REG_AON_RESERVED_REG_2));
	return addr;
}

