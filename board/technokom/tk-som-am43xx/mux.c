/*
 * mux.c
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mux.h>
#include "../common/board_detect.h"
#include "board.h"

static struct module_pin_mux rmii1_pin_mux[] = {
	{OFFSET(mii1_txen), MODE(1)},			/* RMII1_TXEN */
	{OFFSET(mii1_txd1), MODE(1)},			/* RMII1_TD1 */
	{OFFSET(mii1_txd0), MODE(1)},			/* RMII1_TD0 */
	{OFFSET(mii1_rxd1), MODE(1) | RXACTIVE},	/* RMII1_RD1 */
	{OFFSET(mii1_rxd0), MODE(1) | RXACTIVE},	/* RMII1_RD0 */
	//{OFFSET(mii1_rxdv), MODE(1) | RXACTIVE},	/* RMII1_RXDV */
	{OFFSET(mii1_crs), MODE(1) | RXACTIVE},		/* RMII1_CRS_DV */
	{OFFSET(mii1_rxerr), MODE(1) | RXACTIVE},	/* RMII1_RXERR */
	{OFFSET(rmii1_refclk), MODE(0) | RXACTIVE},	/* RMII1_refclk */
	{-1},
};

static struct module_pin_mux mdio_pin_mux[] = {
	{OFFSET(mdio_data), MODE(0) | RXACTIVE | PULLUP_EN},/* MDIO_DATA */
	{OFFSET(mdio_clk), MODE(0) | PULLUP_EN},	/* MDIO_CLK */
	{-1},
};

static struct module_pin_mux uart0_pin_mux[] = {
	{OFFSET(uart0_rxd), (MODE(0) | PULLUP_EN | RXACTIVE | SLEWCTRL)},
	{OFFSET(uart0_txd), (MODE(0) | PULLUDDIS | PULLUP_EN | SLEWCTRL)},
	{-1},
};

static struct module_pin_mux mmc0_pin_mux[] = {
	{OFFSET(mmc0_clk), (MODE(0) | PULLUDDIS | RXACTIVE)},  /* MMC0_CLK */
	{OFFSET(mmc0_cmd), (MODE(0) | PULLUP_EN | RXACTIVE)},  /* MMC0_CMD */
	{OFFSET(mmc0_dat0), (MODE(0) | PULLUP_EN | RXACTIVE)}, /* MMC0_DAT0 */
	{OFFSET(mmc0_dat1), (MODE(0) | PULLUP_EN | RXACTIVE)}, /* MMC0_DAT1 */
	{OFFSET(mmc0_dat2), (MODE(0) | PULLUP_EN | RXACTIVE)}, /* MMC0_DAT2 */
	{OFFSET(mmc0_dat3), (MODE(0) | PULLUP_EN | RXACTIVE)}, /* MMC0_DAT3 */
	{OFFSET(spi0_cs1), (MODE(3) | RXACTIVE | PULLUP_EN)},	/* GPIO0_6 */
	{-1},
};

static struct module_pin_mux mmc2_pin_mux[] = {
	{OFFSET(gpmc_clk),  (MODE(3) | PULLUDDIS | RXACTIVE)},  /* MMC2_CLK */
	{OFFSET(gpmc_csn3), (MODE(3) | RXACTIVE)},  /* MMC2_CMD */
	{OFFSET(gpmc_a1),   (MODE(3) | RXACTIVE)}, /* MMC2_DAT0 */
	{OFFSET(gpmc_a2),   (MODE(3) | RXACTIVE)}, /* MMC2_DAT1 */
	{OFFSET(gpmc_a3),   (MODE(3) | RXACTIVE)}, /* MMC2_DAT2 */
	{OFFSET(gpmc_be1n), (MODE(3) | RXACTIVE)}, /* MMC2_DAT3 */
	{-1},
};

static struct module_pin_mux i2c0_pin_mux[] = {
	{OFFSET(i2c0_sda), (MODE(0) | PULLUP_EN | RXACTIVE | SLEWCTRL)},
	{OFFSET(i2c0_scl), (MODE(0) | PULLUP_EN | RXACTIVE | SLEWCTRL)},
	{-1},
};


#ifdef CONFIG_NAND
static struct module_pin_mux nand_pin_mux[] = {
	{OFFSET(gpmc_ad0),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD0 */
	{OFFSET(gpmc_ad1),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD1 */
	{OFFSET(gpmc_ad2),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD2 */
	{OFFSET(gpmc_ad3),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD3 */
	{OFFSET(gpmc_ad4),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD4 */
	{OFFSET(gpmc_ad5),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD5 */
	{OFFSET(gpmc_ad6),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD6 */
	{OFFSET(gpmc_ad7),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD7 */
#ifdef CONFIG_SYS_NAND_BUSWIDTH_16BIT
	{OFFSET(gpmc_ad8),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD8  */
	{OFFSET(gpmc_ad9),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD9  */
	{OFFSET(gpmc_ad10),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD10 */
	{OFFSET(gpmc_ad11),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD11 */
	{OFFSET(gpmc_ad12),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD12 */
	{OFFSET(gpmc_ad13),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD13 */
	{OFFSET(gpmc_ad14),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD14 */
	{OFFSET(gpmc_ad15),	(MODE(0) | PULLUDDIS | RXACTIVE)}, /* AD15 */
#endif
	{OFFSET(gpmc_wait0),	(MODE(0) | RXACTIVE | PULLUP_EN)}, /* Wait */
	{OFFSET(gpmc_wpn),	(MODE(7) | PULLUP_EN)},	/* Write Protect */
	{OFFSET(gpmc_csn0),	(MODE(0) | PULLUP_EN)},	/* Chip-Select */
	{OFFSET(gpmc_wen),	(MODE(0) | PULLDOWN_EN)}, /* Write Enable */
	{OFFSET(gpmc_oen_ren),	(MODE(0) | PULLDOWN_EN)}, /* Read Enable */
	{OFFSET(gpmc_advn_ale),	(MODE(0) | PULLDOWN_EN)}, /* Addr Latch Enable*/
	{OFFSET(gpmc_be0n_cle),	(MODE(0) | PULLDOWN_EN)}, /* Byte Enable */
	{-1},
};
#endif


void enable_uart0_pin_mux(void)
{
	configure_module_pin_mux(uart0_pin_mux);
}

void enable_board_pin_mux(void)
{
	configure_module_pin_mux(mmc0_pin_mux);
	configure_module_pin_mux(mmc2_pin_mux);
	configure_module_pin_mux(i2c0_pin_mux);
	configure_module_pin_mux(mdio_pin_mux);
	configure_module_pin_mux(rmii1_pin_mux);
    //configure_module_pin_mux(periph_enable);
#if defined(CONFIG_NAND)
	configure_module_pin_mux(nand_pin_mux);
#endif

}

void enable_i2c0_pin_mux(void)
{
	//configure_module_pin_mux(i2c0_pin_mux);
}
