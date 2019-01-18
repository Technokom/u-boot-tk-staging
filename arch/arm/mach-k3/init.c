/*
 * K3: Architecture initialization
 *
 * Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/
 *	Lokesh Vutla <lokeshvutla@ti.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <spl.h>
#include <debug_uart.h>
#include <dm.h>
#include <dm/uclass-internal.h>
#include <dm/pinctrl.h>
#include <remoteproc.h>
#include <linux/libfdt.h>
#include <linux/soc/ti/ti_sci_protocol.h>
#include <image.h>
#include <asm/sections.h>
#include <asm/armv7_mpu.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sysfw-loader.h>
#include <asm/arch/sys_proto.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SPL_BUILD
#ifdef CONFIG_CPU_V7R
struct mpu_region_config k3_mpu_regions[16] = {
	/*
	 * Make all 4GB as Device Memory and not executable. We are overriding
	 * it with next region for any requirement.
	 */
	{0x00000000, REGION_0, XN_EN, PRIV_RW_USR_RW, SHARED_WRITE_BUFFERED,
	 REGION_4GB},

	/* SPL code area marking it as WB and Write allocate. */
	{CONFIG_SPL_TEXT_BASE, REGION_1, XN_DIS, PRIV_RW_USR_RW,
	 O_I_WB_RD_WR_ALLOC, REGION_8MB},

	/* U-Boot's code area marking it as WB and Write allocate */
	{CONFIG_SYS_SDRAM_BASE, REGION_2, XN_DIS, PRIV_RW_USR_RW,
	 O_I_WB_RD_WR_ALLOC, REGION_2GB},
	{0x0, 3, 0x0, 0x0, 0x0, 0x0},
	{0x0, 4, 0x0, 0x0, 0x0, 0x0},
	{0x0, 5, 0x0, 0x0, 0x0, 0x0},
	{0x0, 6, 0x0, 0x0, 0x0, 0x0},
	{0x0, 7, 0x0, 0x0, 0x0, 0x0},
	{0x0, 8, 0x0, 0x0, 0x0, 0x0},
	{0x0, 9, 0x0, 0x0, 0x0, 0x0},
	{0x0, 10, 0x0, 0x0, 0x0, 0x0},
	{0x0, 11, 0x0, 0x0, 0x0, 0x0},
	{0x0, 12, 0x0, 0x0, 0x0, 0x0},
	{0x0, 13, 0x0, 0x0, 0x0, 0x0},
	{0x0, 14, 0x0, 0x0, 0x0, 0x0},
	{0x0, 15, 0x0, 0x0, 0x0, 0x0},
};
#endif

static void mmr_unlock(u32 base, u32 partition)
{
	/* Translate the base address */
	phys_addr_t part_base = base + partition * CTRL_MMR0_PARTITION_SIZE;

	/* Unlock the requested partition if locked using two-step sequence */
	writel(CTRLMMR_LOCK_KICK0_UNLOCK_VAL, part_base + CTRLMMR_LOCK_KICK0);
	writel(CTRLMMR_LOCK_KICK1_UNLOCK_VAL, part_base + CTRLMMR_LOCK_KICK1);
}

static void ctrl_mmr_unlock(void)
{
	/* Unlock all WKUP_CTRL_MMR0 module registers */
	mmr_unlock(WKUP_CTRL_MMR0_BASE, 0);
	mmr_unlock(WKUP_CTRL_MMR0_BASE, 1);
	mmr_unlock(WKUP_CTRL_MMR0_BASE, 2);
	mmr_unlock(WKUP_CTRL_MMR0_BASE, 3);
	mmr_unlock(WKUP_CTRL_MMR0_BASE, 6);
	mmr_unlock(WKUP_CTRL_MMR0_BASE, 7);

	/* Unlock all MCU_CTRL_MMR0 module registers */
	mmr_unlock(MCU_CTRL_MMR0_BASE, 0);
	mmr_unlock(MCU_CTRL_MMR0_BASE, 1);
	mmr_unlock(MCU_CTRL_MMR0_BASE, 2);
	mmr_unlock(MCU_CTRL_MMR0_BASE, 6);

	/* Unlock all CTRL_MMR0 module registers */
	mmr_unlock(CTRL_MMR0_BASE, 0);
	mmr_unlock(CTRL_MMR0_BASE, 1);
	mmr_unlock(CTRL_MMR0_BASE, 2);
	mmr_unlock(CTRL_MMR0_BASE, 3);
	mmr_unlock(CTRL_MMR0_BASE, 6);
	mmr_unlock(CTRL_MMR0_BASE, 7);

}

static void store_boot_index_from_rom(void)
{
	u32 *boot_index = (u32 *)K3_BOOT_PARAM_TABLE_INDEX_VAL;

	*boot_index = *(u32 *)(CONFIG_BOOT_PARAM_TABLE_INDEX);
}

#ifdef CONFIG_K3_EARLY_CONS
int early_console_init(void)
{
	struct udevice *dev;
	int ret;

	gd->baudrate = CONFIG_BAUDRATE;

	ret = uclass_get_device_by_seq(UCLASS_SERIAL, CONFIG_K3_EARLY_CONS_IDX,
				       &dev);
	if (ret) {
		printf("Error getting serial dev for early console! (%d)\n",
		       ret);
		return ret;
	}

	gd->cur_serial_dev = dev;
	gd->flags |= GD_FLG_SERIAL_READY;
	gd->have_console = 1;

	return 0;
}
#endif

void board_init_f(ulong dummy)
{
#if defined(CONFIG_K3_LOAD_SYSFW) || defined(CONFIG_K3_AM654_DDRSS)
	struct udevice *dev;
	int ret;
#endif
	/*
	 * Cannot delay this further as there is a chance that
	 * BOOT_PARAM_TABLE_INDEX can be over written by SPL MALLOC section.
	 */
	store_boot_index_from_rom();

	/* Make all control module registers accessible */
	ctrl_mmr_unlock();

#ifdef CONFIG_DEBUG_UART_OMAP
	/* Initialize debug UART *after* control module regs are unlocked */
	debug_uart_init();

#ifdef CONFIG_DEBUG_UART_ANNOUNCE
	/* Provide a tad more info and a LF for cleaner console output */
#ifdef CONFIG_ARM64
	puts("ARM64 SPL here\n");
#elif CONFIG_CPU_V7R
	puts("R5 SPL here\n");
#endif
#endif
#endif

#ifdef CONFIG_CPU_V7R
	setup_mpu_regions(k3_mpu_regions, ARRAY_SIZE(k3_mpu_regions));

	/*
	 * When running SPL on R5 we are using SRAM for BSS to have global
	 * data etc. working prior to relocation. Since this means we need
	 * to self-manage BSS, clear that section now.
	 */
	memset(__bss_start, 0, __bss_end - __bss_start);
#endif

	/* Init DM early */
	spl_early_init();

#ifdef CONFIG_K3_EARLY_CONS
	/*
	 * Allow establishing an early console as required for example when
	 * doing a UART-based boot. Note that this console may not "survive"
	 * through a SYSFW PM-init step and will need a re-init in some way
	 * due to changing module clock frequencies.
	 */
	early_console_init();
#endif

#ifdef CONFIG_K3_LOAD_SYSFW
	/*
	 * Process pinctrl for the serial0 a.k.a. WKUP_UART0 module and continue
	 * regardless of the result of pinctrl. Do this without probing the
	 * device, but instead by searching the device that would request the
	 * given sequence number if probed. The UART will be used by the system
	 * firmware (SYSFW) image for various purposes and SYSFW depends on us
	 * to initialize its pin settings.
	 */
	ret = uclass_find_device_by_seq(UCLASS_SERIAL, 0, true, &dev);
	if (!ret) {
		pinctrl_select_state(dev, "default");
	}

	/*
	 * Load, start up, and configure system controller firmware. Provide
	 * the U-Boot console init function to the SYSFW post-PM configuration
	 * callback hook, effectively switching on (or over) the console
	 * output.
	 */
	k3_sysfw_loader(preloader_console_init);
#else
	/* Prepare console output */
	preloader_console_init();
#endif

	/* Perform EEPROM-based board detection */
	do_board_detect();

#ifdef CONFIG_K3_AM654_DDRSS
	ret = uclass_get_device(UCLASS_RAM, 0, &dev);
	if (ret) {
		printf("DRAM init failed: %d\n", ret);
		hang();
	}
#endif
}

u32 spl_boot_mode(const u32 boot_device)
{
	u32 devstat = readl(CTRLMMR_MAIN_DEVSTAT);
	u32 bootindex = readl(K3_BOOT_PARAM_TABLE_INDEX_VAL);

	u32 bootmode = (devstat & CTRLMMR_MAIN_DEVSTAT_BOOTMODE_MASK) >>
			CTRLMMR_MAIN_DEVSTAT_BOOTMODE_SHIFT;

	/* eMMC boot mode is only supported for primary boot */
	if (bootindex == K3_PRIMARY_BOOTMODE &&
	    bootmode == BOOT_DEVICE_MMC1)
		return MMCSD_MODE_EMMCBOOT;

	/* Everything else use filesystem */
	return MMCSD_MODE_FS;
}

static u32 __get_backup_bootmedia(u32 devstat)
{
	u32 bkup_boot = (devstat & CTRLMMR_MAIN_DEVSTAT_BKUP_BOOTMODE_MASK) >>
			CTRLMMR_MAIN_DEVSTAT_BKUP_BOOTMODE_SHIFT;

	u32 bootmode = BOOT_DEVICE_RAM;

	switch (bkup_boot) {
#define __BKUP_BOOT_DEVICE(n)			\
	case BACKUP_BOOT_DEVICE_##n:		\
		bootmode =  BOOT_DEVICE_##n;
	__BKUP_BOOT_DEVICE(USB);
	__BKUP_BOOT_DEVICE(UART);
	__BKUP_BOOT_DEVICE(ETHERNET);
	__BKUP_BOOT_DEVICE(MMC2);
	__BKUP_BOOT_DEVICE(SPI);
	__BKUP_BOOT_DEVICE(HYPERFLASH);
	__BKUP_BOOT_DEVICE(I2C);
	};

	if (bootmode == BOOT_DEVICE_MMC2) {
		u32 port = (devstat & CTRLMMR_MAIN_DEVSTAT_BKUP_MMC_PORT_MASK) >>
				CTRLMMR_MAIN_DEVSTAT_BKUP_MMC_PORT_SHIFT;
		if (port == 0x0)
			bootmode = BOOT_DEVICE_MMC1;
	}

	return bootmode;
}

static u32 __get_primary_bootmedia(u32 devstat)
{
	u32 bootmode = (devstat & CTRLMMR_MAIN_DEVSTAT_BOOTMODE_MASK) >>
			CTRLMMR_MAIN_DEVSTAT_BOOTMODE_SHIFT;

	if (bootmode == BOOT_DEVICE_OSPI || bootmode ==	BOOT_DEVICE_QSPI)
		bootmode = BOOT_DEVICE_SPI;

	if (bootmode == BOOT_DEVICE_MMC2) {
		u32 port = (devstat & CTRLMMR_MAIN_DEVSTAT_MMC_PORT_MASK) >>
			    CTRLMMR_MAIN_DEVSTAT_MMC_PORT_SHIFT;
		if (port == 0x0)
			bootmode = BOOT_DEVICE_MMC1;
	} else if (bootmode == BOOT_DEVICE_MMC1) {
		u32 port = (devstat & CTRLMMR_MAIN_DEVSTAT_EMMC_PORT_MASK) >>
			    CTRLMMR_MAIN_DEVSTAT_EMMC_PORT_SHIFT;
		if (port == 0x1)
			bootmode = BOOT_DEVICE_MMC2;
	}

	return bootmode;
}

u32 spl_boot_device(void)
{
	u32 devstat = readl(CTRLMMR_MAIN_DEVSTAT);
	u32 bootindex = readl(K3_BOOT_PARAM_TABLE_INDEX_VAL);

	if (bootindex == K3_PRIMARY_BOOTMODE)
		return __get_primary_bootmedia(devstat);
	else
		return __get_backup_bootmedia(devstat);
}
#endif

#ifndef CONFIG_SYSRESET
void reset_cpu(ulong ignored)
{
}
#endif

#ifdef CONFIG_K3_SPL_ATF

#define AM6_DEV_MCU_RTI0			134
#define AM6_DEV_MCU_RTI1			135
#define AM6_DEV_MCU_ARMSS0_CPU0			159
#define AM6_DEV_MCU_ARMSS0_CPU1			245

void release_resources_for_core_shutdown(void)
{
	struct udevice *dev;
	struct ti_sci_handle *ti_sci;
	struct ti_sci_dev_ops *dev_ops;
	int ret;
	u32 i;

	const u32 put_device_ids[] = {
		AM6_DEV_MCU_RTI0,
		AM6_DEV_MCU_RTI1,
		AM6_DEV_MCU_ARMSS0_CPU1,
		AM6_DEV_MCU_ARMSS0_CPU0,	/* Handle CPU0 after CPU1 */
	};

	/* Get handle to Device Management and Security Controller (SYSFW) */
	ret = uclass_get_device_by_name(UCLASS_FIRMWARE, "dmsc", &dev);
	if (ret) {
		printf("Failed to get handle to SYSFW (%d)\n", ret);
		hang();
	}

	ti_sci = (struct ti_sci_handle *)(ti_sci_get_handle_from_sysfw(dev));
	dev_ops = &ti_sci->ops.dev_ops;

	/* Iterate through list of devices to put (shutdown) */
	for (i = 0; i < ARRAY_SIZE(put_device_ids); i++) {
		u32 id = put_device_ids[i];

		ret = dev_ops->put_device(ti_sci, id);
		if (ret) {
			printf("Failed to put device %u (%d)\n", id, ret);
			hang();
		}
	}
}

void __noreturn jump_to_image_no_args(struct spl_image_info *spl_image)
{
	int ret;

	/*
	 * It is assumed that remoteproc device 1 is the corresponding
	 * cortex A core which runs ATF. Make sure DT reflects the same.
	 */
	ret = rproc_dev_init(1);
	if (ret) {
		printf("%s: ATF failed to Initialize on rproc: ret= %d\n",
		       __func__, ret);
		hang();
	}

	ret = rproc_load(1, spl_image->entry_point, 0x200);
	if (ret) {
		printf("%s: ATF failed to load on rproc: ret= %d\n",
		       __func__, ret);
		hang();
	}

	/* Add an extra newline to differentiate the ATF logs from SPL */
	printf("Starting ATF on ARM64 core...\n\n");

	ret = rproc_start(1);
	if (ret) {
		printf("%s: ATF failed to start on rproc: ret= %d\n",
		       __func__, ret);
		hang();
	}

	debug("Releasing resources...\n");
	release_resources_for_core_shutdown();

	debug("Finalizing core shutdown...\n");
	while (1)
		asm volatile("wfe");
}
#endif
