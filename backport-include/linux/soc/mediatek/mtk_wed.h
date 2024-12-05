#ifndef __BACKPORT_MTK_WED_H
#define __BACKPORT_MTK_WED_H
#include <linux/version.h>

#if LINUX_VERSION_IS_GEQ(5,19,0)
#include_next <linux/soc/mediatek/mtk_wed.h>
#else
#include <linux/kernel.h>
#include <linux/rcupdate.h>
#include <linux/regmap.h>
#include <linux/pci.h>

#define MTK_WED_TX_QUEUES		2

struct mtk_wed_hw;
struct mtk_wdma_desc;

enum mtk_wed_bus_tye {
	MTK_WED_BUS_PCIE,
	MTK_WED_BUS_AXI,
};

struct mtk_wed_ring {
	struct mtk_wdma_desc *desc;
	dma_addr_t desc_phys;
	u32 desc_size;
	int size;

	u32 reg_base;
	void __iomem *wpdma;
};

struct mtk_wed_device {

};

static inline int
mtk_wed_device_attach(struct mtk_wed_device *dev)
{
	return -ENODEV;
}

static inline bool mtk_wed_device_active(struct mtk_wed_device *dev)
{
	return false;
}
#define mtk_wed_device_detach(_dev) do {} while (0)
#define mtk_wed_device_start(_dev, _mask) do {} while (0)
#define mtk_wed_device_tx_ring_setup(_dev, _ring, _regs) -ENODEV
#define mtk_wed_device_txfree_ring_setup(_dev, _ring, _regs) -ENODEV
#define mtk_wed_device_reg_read(_dev, _reg) 0
#define mtk_wed_device_reg_write(_dev, _reg, _val) do {} while (0)
#define mtk_wed_device_irq_get(_dev, _mask) 0
#define mtk_wed_device_irq_set_mask(_dev, _mask) do {} while (0)

#endif /* >= 5.19.0 */

#endif /* __BACKPORT_MTK_WED_H */
