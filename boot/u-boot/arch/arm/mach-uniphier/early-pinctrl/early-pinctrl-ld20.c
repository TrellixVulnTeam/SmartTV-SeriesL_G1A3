/*
 * Copyright (C) 2016 Masahiro Yamada <yamada.masahiro@socionext.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include "../init.h"
#include "../sg-regs.h"

int uniphier_ld20_early_pin_init(const struct uniphier_board_data *bd)
{
	/* Comment format:    PAD Name -> Function Name */
	sg_set_pinsel(0, 0, 8, 4);	/* XECS1  -> XECS1 */
	sg_set_pinsel(1, 0, 8, 4);	/* ERXW   -> ERXW  */
	sg_set_pinsel(2, 0, 8, 4);	/* XERWE1 -> XERWE1 */
	sg_set_iectrl_range(0, 2);

	if(((readl(SG_PINMON0) >> 1) & 0x1f) <= 0x17) {
		/* bootmode_sel is nand */
		sg_set_pinsel(3, 0, 8, 4);	/* XNFWP   -> XNFWP */
		sg_set_pinsel(4, 0, 8, 4);	/* XNFCE0  -> XNFCE0 */
		sg_set_pinsel(5, 0, 8, 4);	/* NFRYBY0 -> NFRYBY0 */
		sg_set_pinsel(6, 0, 8, 4);	/* XNFRE   -> XNFRE */
		sg_set_pinsel(7, 0, 8, 4);	/* XNFWE   -> XNFWE */
		sg_set_pinsel(8, 0, 8, 4);	/* NFALE   -> NFALE */
		sg_set_pinsel(9, 0, 8, 4);	/* NFCLE   -> NFCLE */
		sg_set_pinsel(10, 0, 8, 4);	/* NFD0    -> NFD0 */
		sg_set_pinsel(11, 0, 8, 4);	/* NFD1    -> NFD1 */
		sg_set_pinsel(12, 0, 8, 4);	/* NFD2    -> NFD2 */
		sg_set_pinsel(13, 0, 8, 4);	/* NFD3    -> NFD3 */
		sg_set_pinsel(14, 0, 8, 4);	/* NFD4    -> NFD4 */
		sg_set_pinsel(15, 0, 8, 4);	/* NFD5    -> NFD5 */
		sg_set_pinsel(16, 0, 8, 4);	/* NFD6    -> NFD6 */
		sg_set_pinsel(17, 0, 8, 4);	/* NFD7    -> NFD7 */
		sg_set_iectrl_range(3, 17);
	} else {
		sg_set_pinsel(6, 2, 8, 4);	/* XNFRE  -> XERWE0 */
		sg_set_pinsel(7, 2, 8, 4);	/* XNFWE  -> ES0 */
		sg_set_pinsel(8, 2, 8, 4);	/* NFALE  -> ES1 */
		sg_set_pinsel(9, 2, 8, 4);	/* NFCLE  -> ES2 */
		sg_set_pinsel(10, 2, 8, 4);	/* NFD0   -> ED0 */
		sg_set_pinsel(11, 2, 8, 4);	/* NFD1   -> ED1 */
		sg_set_pinsel(12, 2, 8, 4);	/* NFD2   -> ED2 */
		sg_set_pinsel(13, 2, 8, 4);	/* NFD3   -> ED3 */
		sg_set_pinsel(14, 2, 8, 4);	/* NFD4   -> ED4 */
		sg_set_pinsel(15, 2, 8, 4);	/* NFD5   -> ED5 */
		sg_set_pinsel(16, 2, 8, 4);	/* NFD6   -> ED6 */
		sg_set_pinsel(17, 2, 8, 4);	/* NFD7   -> ED7 */
		sg_set_iectrl_range(6, 17);
	}

	return 0;
}
