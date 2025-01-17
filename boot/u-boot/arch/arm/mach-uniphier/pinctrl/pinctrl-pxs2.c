/*
 * Copyright (C) 2015 Masahiro Yamada <yamada.masahiro@socionext.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <linux/io.h>

#include "../init.h"
#include "../sg-regs.h"

void uniphier_pxs2_pin_init(void)
{
	/* Comment format:    PAD Name -> Function Name */

#ifdef CONFIG_NAND_DENALI
	sg_set_pinsel(30, 8, 8, 4);	/* XNFRE  -> XNFRE */
	sg_set_pinsel(31, 8, 8, 4);	/* XNFWE  -> XNFWE */
	sg_set_pinsel(32, 8, 8, 4);	/* NFALE  -> NFALE */
	sg_set_pinsel(33, 8, 8, 4);	/* NFCLE  -> NFCLE */
	sg_set_pinsel(34, 8, 8, 4);	/* XNFWP  -> XNFWP */
	sg_set_pinsel(35, 8, 8, 4);	/* XNFCE0 -> XNFCE0 */
	sg_set_pinsel(36, 8, 8, 4);	/* NRYBY0 -> NRYBY0 */
	sg_set_pinsel(37, 8, 8, 4);	/* XNFCE1 -> NRYBY1 */
	sg_set_pinsel(38, 8, 8, 4);	/* NRYBY1 -> XNFCE1 */
	sg_set_pinsel(39, 8, 8, 4);	/* NFD0   -> NFD0 */
	sg_set_pinsel(40, 8, 8, 4);	/* NFD1   -> NFD1 */
	sg_set_pinsel(41, 8, 8, 4);	/* NFD2   -> NFD2 */
	sg_set_pinsel(42, 8, 8, 4);	/* NFD3   -> NFD3 */
	sg_set_pinsel(43, 8, 8, 4);	/* NFD4   -> NFD4 */
	sg_set_pinsel(44, 8, 8, 4);	/* NFD5   -> NFD5 */
	sg_set_pinsel(45, 8, 8, 4);	/* NFD6   -> NFD6 */
	sg_set_pinsel(46, 8, 8, 4);	/* NFD7   -> NFD7 */
#endif
}
