/*
 * am43xx_tk.h
 *
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_AM43XX_TK_H
#define __CONFIG_AM43XX_TK_H

#define CONFIG_ARCH_CPU_INIT
#define CONFIG_MAX_RAM_BANK_SIZE	(1024 << 21)	/* 2GB */
#define CONFIG_SYS_TIMERBASE		0x48040000	/* Use Timer2 */

#include <asm/arch/omap.h>

/* NS16550 Configuration */
#define CONFIG_SYS_NS16550_CLK		48000000
#if !defined(CONFIG_SPL_DM) || !defined(CONFIG_DM_SERIAL)
#define CONFIG_SYS_NS16550_REG_SIZE    (-4)
#define CONFIG_SYS_NS16550_SERIAL
#endif


/* Power */
#define CONFIG_POWER
#define CONFIG_POWER_I2C
#define CONFIG_POWER_TPS65218

/* SPL defines. */
#define CONFIG_SPL_TEXT_BASE		CONFIG_ISW_ENTRY_ADDR
#define CONFIG_SYS_SPL_ARGS_ADDR	(CONFIG_SYS_SDRAM_BASE + \
					 (128 << 20))

/* Enabling L2 Cache */
#define CONFIG_SYS_L2_PL310
#define CONFIG_SYS_PL310_BASE	0x48242000

/*
 * Since SPL did pll and ddr initialization for us,
 * we don't need to do it twice.
 */
#if !defined(CONFIG_SPL_BUILD) && !defined(CONFIG_QSPI_BOOT) 
#define CONFIG_SKIP_LOWLEVEL_INIT
#endif

/*
 * When building U-Boot such that there is no previous loader
 * we need to call board_early_init_f.  This is taken care of in
 * s_init when we have SPL used.
 */

/* Now bring in the rest of the common code. */
#include <configs/ti_armv7_omap.h>

/* Always 64 KiB env size */
#define CONFIG_ENV_SIZE			(64 << 10)

#define CONFIG_ENV_VARS_UBOOT_RUNTIME_CONFIG

/* Clock Defines */
#define V_OSCK				24000000  /* Clock output from T2 */
#define V_SCLK				(V_OSCK)

/* NS16550 Configuration */
#define CONFIG_SYS_NS16550_COM1		0x44e09000	/* Base EVM has UART0 */

/* SPL USB Support */

#if defined(CONFIG_SPL_USB_HOST_SUPPORT) || !defined(CONFIG_SPL_BUILD)
#define CONFIG_SYS_USB_FAT_BOOT_PARTITION		1
#define CONFIG_USB_XHCI_OMAP

#define CONFIG_OMAP_USB_PHY
#define CONFIG_AM437X_USB2PHY2_HOST
#endif

#if defined(CONFIG_SPL_BUILD) && !defined(CONFIG_SPL_USBETH_SUPPORT)
#undef CONFIG_USB_DWC3_PHY_OMAP
#undef CONFIG_USB_DWC3_OMAP
#undef CONFIG_USB_DWC3
#undef CONFIG_USB_DWC3_GADGET

#undef CONFIG_USB_GADGET_DOWNLOAD
#undef CONFIG_USB_GADGET_VBUS_DRAW
#undef CONFIG_USB_GADGET_MANUFACTURER
#undef CONFIG_USB_GADGET_VENDOR_NUM
#undef CONFIG_USB_GADGET_PRODUCT_NUM
#undef CONFIG_USB_GADGET_DUALSPEED
#endif

/*
 * Disable MMC DM for SPL build and can be re-enabled after adding
 * DM support in SPL
 */
#ifdef CONFIG_SPL_BUILD
#undef CONFIG_TIMER
#endif

#ifndef CONFIG_SPL_BUILD
/* USB Device Firmware Update support */
#define DFUARGS \
	"dfu_bufsiz=0x10000\0" \
	DFU_ALT_INFO_MMC \
	DFU_ALT_INFO_EMMC \
	DFU_ALT_INFO_RAM \
	DFU_ALT_INFO_QSPI_XIP
#else
#define DFUARGS
#endif

#ifdef CONFIG_QSPI_BOOT
#define CONFIG_SYS_REDUNDAND_ENVIRONMENT
#define CONFIG_ENV_SPI_MAX_HZ           CONFIG_SF_DEFAULT_SPEED
#define CONFIG_ENV_SECT_SIZE           (64 << 10) /* 64 KB sectors */
#define CONFIG_ENV_OFFSET              0x110000
#define CONFIG_ENV_OFFSET_REDUND       0x120000
#endif

/* SPI */
#define CONFIG_TI_SPI_MMAP
#define CONFIG_QSPI_SEL_GPIO                   48
#define CONFIG_SF_DEFAULT_SPEED                48000000
#define CONFIG_SF_DEFAULT_MODE                 SPI_MODE_3
#define CONFIG_QSPI_QUAD_SUPPORT
#define CONFIG_TI_EDMA3

#ifndef CONFIG_SPL_BUILD
#include <environment/ti/dfu.h>
#include <environment/ti/mmc.h>

#define CONFIG_EXTRA_ENV_SETTINGS \
	DEFAULT_LINUX_BOOT_ENV \
	DEFAULT_MMC_TI_ARGS \
	DEFAULT_FIT_TI_ARGS \
	"fdtfile=undefined\0" \
	"bootpart=0:2\0" \
	"bootdir=/boot\0" \
	"bootfile=zImage\0" \
	"console=ttyO0,115200n8\0" \
	"partitions=" \
		"uuid_disk=${uuid_gpt_disk};" \
		"name=rootfs,start=2MiB,size=-,uuid=${uuid_gpt_rootfs}\0" \
	"optargs=\0" \
	"findfdt="\
			"setenv fdtfile tk-som-am43xx.dtb; fi; \0" \
	NETARGS \
	DFUARGS \

#define CONFIG_BOOTCOMMAND \
	"run findfdt; " \
	"run envboot;" \
	"run mmcboot;" \

#endif

#ifndef CONFIG_SPL_BUILD
/* CPSW Ethernet */
#define CONFIG_MII
#define CONFIG_BOOTP_DEFAULT
#define CONFIG_BOOTP_DNS
#define CONFIG_BOOTP_DNS2
#define CONFIG_BOOTP_SEND_HOSTNAME
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_SUBNETMASK
#define CONFIG_NET_RETRY_COUNT		10
#endif

#define CONFIG_DRIVER_TI_CPSW
#define PHY_ANEG_TIMEOUT	8000 /* PHY needs longer aneg time at 1G */

#define CONFIG_SYS_RX_ETH_BUFFER	64


#if defined(CONFIG_TI_SECURE_DEVICE)
/* Avoid relocating onto firewalled area at end of DRAM */
#define CONFIG_PRAM (64 * 1024)
#endif /* CONFIG_TI_SECURE_DEVICE */

#endif	/* __CONFIG_AM43XX_EVM_H */
