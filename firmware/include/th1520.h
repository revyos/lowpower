/*
* Copyright (C) 2017-2020 Alibaba Group Holding Limited
*
* SPDX-License-Identifier: GPL-2.0+
*/

#ifndef __TH1520_BOARD_H__
#define __TH1520_BOARD_H__

#include "int-ll64.h"

#define FW_TEXT_START 0xffe0000000

#define TH1520_APCLK_ADDRBASE    0xffff011000
#define TH1520_MISC_SUBSYS_ADDRBASE  0xfffc02d000
#define TH1520_APCLK_ADDRBASE    0xffff011000
#define TH1520_AONCLK_ADDRBASE   0xfffff46000
#define TH1520_DDRCLK_ADDRBASE   0xffff005000
#define TH1520_MISC_SUBSYS_ADDRBASE  0xfffc02d000
#define TH1520_VI_SUBSYS_ADDRBASE    0xfff4041000
#define TH1520_VO_SUBSYS_ADDRBASE    0xffff401000
#define TH1520_VO_SUBSYS_R_ADDRBASE  0xffef528000
#define TH1520_VP_SUBSYS_ADDRBASE    0xfffcc01000
#define TH1520_DSP_SUBSYS_ADDRBASE   0xffff041000
#define TH1520_AUDIO_SUBSYS_ADDRBASE 0xffcb000000
#define TH1520_APSYS_RSTGEN_ADDRBASE 0xffff015000
#define TH1520_DPU_CLOCK_GATING_CTRL0    0xffef601A28
#define TH1520_DPU_CLOCK_GATING_CTRL1    0xffef601A2C
#define PMP_BASE_ADDR    0xffdc020000
#define AONSYS_REG_BASE                 0xFFFFF48000
#define AO_IOPMP_BASE       0xFFFFC21000
#define DSPSYS_SW_RST       0xFFEF040028


#define REG_AON_CHIP_LP_MODE     (AONSYS_REG_BASE + 0x04 )

#define REG_AON_STR_INDICATOR_0     (AONSYS_REG_BASE + 0x20 )
#define REG_AON_STR_INDICATOR_1     (AONSYS_REG_BASE + 0x24 )
#define REG_AON_STR_INDICATOR_2     (AONSYS_REG_BASE + 0x28 )
#define REG_AON_STR_INDICATOR_3     (AONSYS_REG_BASE + 0x2c )
#define REG_AON_RESERVED_REG_0      (AONSYS_REG_BASE + 0x50 )
#define REG_AON_RESERVED_REG_1      (AONSYS_REG_BASE + 0x54 )
#define REG_AON_RESERVED_REG_2      (AONSYS_REG_BASE + 0x58 )
#define REG_AON_RESERVED_REG_3      (AONSYS_REG_BASE + 0x5c )

#define CPU_CLK_MHZ (50)

void setup_ddr_pmp(void);
void cpu_performance_enable(void);
void clk_reinit(void);
void mdelay(int ms);
void plic_reinit(void);
void iopmp_reinit(void);
void iopmp_restore(void);
int get_str_flag(void);
int chip_lp_mode_get_iopmp_flag(void);
int chip_lp_mode_set_iopmp_flag(int enable);
int chip_lp_mode_get_ddr_flag(void);
int chip_lp_mode_set_ddr_flag(int enable);
unsigned long get_sbi_addr(void);

#endif // # __TH1520_BOARD_H__
