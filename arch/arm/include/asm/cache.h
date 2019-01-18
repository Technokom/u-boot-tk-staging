/*
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _ASM_CACHE_H
#define _ASM_CACHE_H

#include <asm/system.h>

#ifndef CONFIG_ARM64

/*
 * Invalidate L2 Cache using co-proc instruction
 */
#if CONFIG_IS_ENABLED(SYS_THUMB_BUILD)
void invalidate_l2_cache(void);
#else
static inline void invalidate_l2_cache(void)
{
	unsigned int val=0;

	asm volatile("mcr p15, 1, %0, c15, c11, 0 @ invl l2 cache"
		: : "r" (val) : "cc");
	isb();
}
#endif

int check_cache_range(unsigned long start, unsigned long stop);

void l2_cache_enable(void);
void l2_cache_disable(void);
void set_section_dcache(int section, enum dcache_option option);

void arm_init_before_mmu(void);
void arm_init_domains(void);
void cpu_cache_initialization(void);
void dram_bank_mmu_setup(int bank);
void sram_bank_mmu_setup(phys_addr_t start, phys_addr_t size);

#endif

/*
 * The value of the largest data cache relevant to DMA operations shall be set
 * for us in CONFIG_SYS_CACHELINE_SIZE.  In some cases this may be a larger
 * value than found in the L1 cache but this is OK to use in terms of
 * alignment.
 */
#define ARCH_DMA_MINALIGN	CONFIG_SYS_CACHELINE_SIZE

#endif /* _ASM_CACHE_H */
