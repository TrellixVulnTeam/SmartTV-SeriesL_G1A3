/*
 * Copyright (C) 2011-2015 Masahiro Yamada <yamada.masahiro@socionext.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <linux/io.h>

#include "../init.h"
#include "../sg-regs.h"

void uniphier_pro4_pin_init(void)
{
	/* Comment format:    PAD Name -> Function Name */

#ifdef CONFIG_NAND_DENALI
	sg_set_pinsel(40, 0, 4, 8);	/* NFD0   -> NFD0 */
	sg_set_pinsel(41, 0, 4, 8);	/* NFD1   -> NFD1 */
	sg_set_pinsel(42, 0, 4, 8);	/* NFD2   -> NFD2 */
	sg_set_pinsel(43, 0, 4, 8);	/* NFD3   -> NFD3 */
	sg_set_pinsel(44, 0, 4, 8);	/* NFD4   -> NFD4 */
	sg_set_pinsel(45, 0, 4, 8);	/* NFD5   -> NFD5 */
	sg_set_pinsel(46, 0, 4, 8);	/* NFD6   -> NFD6 */
	sg_set_pinsel(47, 0, 4, 8);	/* NFD7   -> NFD7 */
	sg_set_pinsel(48, 0, 4, 8);	/* NFALE  -> NFALE */
	sg_set_pinsel(49, 0, 4, 8);	/* NFCLE  -> NFCLE */
	sg_set_pinsel(50, 0, 4, 8);	/* XNFRE  -> XNFRE */
	sg_set_pinsel(51, 0, 4, 8);	/* XNFWE  -> XNFWE */
	sg_set_pinsel(52, 0, 4, 8);	/* XNFWP  -> XNFWP */
	sg_set_pinsel(53, 0, 4, 8);	/* XNFCE0 -> XNFCE0 */
	sg_set_pinsel(54, 0, 4, 8);	/* NRYBY0 -> NRYBY0 */
	/* sg_set_pinsel(131, 1, 4, 8); */	/* RXD2   -> NRYBY1 */
	/* sg_set_pinsel(132, 1, 4, 8); */	/* TXD2   -> XNFCE1 */
#endif

	writel(1, SG_LOADPINCTRL);
}
