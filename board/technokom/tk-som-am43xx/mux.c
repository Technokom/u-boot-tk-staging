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
	{OFFSET(mcasp0_aclkx), (MODE(7) | RXACTIVE | PULLUDDIS)},
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

}

void enable_i2c0_pin_mux(void)
{
	configure_module_pin_mux(i2c0_pin_mux);
}
