/*
 * UniPhier Specific Glue Layer for DWC3
 *
 * Copyright (C) 2016 Socionext Inc.
 *   Author: Masahiro Yamada <yamada.masahiro@socionext.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <mapmem.h>
#include <dm/device.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/sizes.h>

#define UNIPHIER_PRO4_DWC3_RESET	0x40
#define   UNIPHIER_PRO4_DWC3_RESET_XIOMMU	BIT(5)
#define   UNIPHIER_PRO4_DWC3_RESET_XLINK	BIT(4)

#define UNIPHIER_PRO5_DWC3_RESET	0x00
#define   UNIPHIER_PRO5_DWC3_RESET_XLINK	BIT(15)
#define   UNIPHIER_PRO5_DWC3_RESET_XIOMMU	BIT(14)

#define UNIPHIER_PXS2_DWC3_RESET	0x00
#define   UNIPHIER_PXS2_DWC3_RESET_XLINK	BIT(15)

static int uniphier_pro4_dwc3_init(void __iomem *regs)
{
	u32 tmp;

	tmp = readl(regs + UNIPHIER_PRO4_DWC3_RESET);
	tmp |= UNIPHIER_PRO4_DWC3_RESET_XIOMMU | UNIPHIER_PRO4_DWC3_RESET_XLINK;
	writel(tmp, regs + UNIPHIER_PRO4_DWC3_RESET);

	return 0;
}

static int uniphier_pro5_dwc3_init(void __iomem *regs)
{
	u32 tmp;

	tmp = readl(regs + UNIPHIER_PRO5_DWC3_RESET);
	tmp |= UNIPHIER_PRO5_DWC3_RESET_XLINK | UNIPHIER_PRO5_DWC3_RESET_XIOMMU;
	writel(tmp, regs + UNIPHIER_PRO5_DWC3_RESET);

	return 0;
}


static int uniphier_pxs2_dwc3_init(void __iomem *regs)
{
	u32 tmp;

	tmp = readl(regs + UNIPHIER_PXS2_DWC3_RESET);
	tmp |= UNIPHIER_PXS2_DWC3_RESET_XLINK;
	writel(tmp, regs + UNIPHIER_PXS2_DWC3_RESET);

	return 0;
}

static int uniphier_dwc3_probe(struct udevice *dev)
{
	fdt_addr_t base;
	void __iomem *regs;
	int (*init)(void __iomem *regs);
	int ret;

	base = dev_get_addr(dev);
	if (base == FDT_ADDR_T_NONE)
		return -EINVAL;

	regs = map_sysmem(base, SZ_32K);
	if (!regs)
		return -ENOMEM;

	init = (int (*)(void __iomem *regs))dev_get_driver_data(dev);
	ret = init(regs);
	if (ret) {
		dev_err(dev, "failed to init glue layer\n");
		return ret;
	}

	unmap_sysmem(regs);

	return 0;
}

static const struct udevice_id uniphier_dwc3_match[] = {
	{
		.compatible = "socionext,uniphier-pro4-dwc3",
		.data = (ulong)uniphier_pro4_dwc3_init,
	},
	{
		.compatible = "socionext,uniphier-pro5-dwc3",
		.data = (ulong)uniphier_pro5_dwc3_init,
	},
	{
		.compatible = "socionext,uniphier-pxs2-dwc3",
		.data = (ulong)uniphier_pxs2_dwc3_init,
	},
	{
		.compatible = "socionext,uniphier-ld20-dwc3",
		.data = (ulong)uniphier_pxs2_dwc3_init,
	},
	{ /* sentinel */ }
};

U_BOOT_DRIVER(usb_xhci) = {
	.name	= "uniphier-dwc3",
	.id	= UCLASS_SIMPLE_BUS,
	.of_match = uniphier_dwc3_match,
	.probe = uniphier_dwc3_probe,
};
