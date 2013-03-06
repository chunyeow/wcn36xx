/*
 * Contact: Eugene Krasnikov <k.eugene.e@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _WCN36XX_H_
#define _WCN36XX_H_

#include <linux/completion.h>
#include <linux/printk.h>
#include <linux/firmware.h>
#include <linux/workqueue.h>
#include <mach/msm_smd.h>
#include <net/mac80211.h>

#include "smd.h"
#include "dxe.h"

#define DRIVER_PREFIX "wcn36xx: "
#define WLAN_NV_FILE               "wlan/prima/WCNSS_qcom_wlan_nv.bin"

#define wcn36xx_error(fmt, arg...) \
	pr_err(DRIVER_PREFIX "ERROR " fmt "\n", ##arg); \
	__WARN()

#define wcn36xx_info(fmt, arg...) \
	pr_info(DRIVER_PREFIX fmt "\n", ##arg)

#define wcn36xx_dbg(fmt, arg...) \
	pr_debug(DRIVER_PREFIX fmt "\n", ##arg)

#define DEFINE_DYNAMIC_DEBUG_METADATA(name, fmt)		\
	static struct _ddebug __used __aligned(8)		\
	__attribute__((section("__verbose"))) name = {		\
		.modname = KBUILD_MODNAME,			\
		.function = __func__,				\
		.filename = __FILE__,				\
		.format = (fmt),				\
		.lineno = __LINE__,				\
		.flags =  _DPRINTK_FLAGS_DEFAULT,		\
	}

#define wcn36xx_dbg_dump(prefix_str, buf, len)			\
do {									\
	DEFINE_DYNAMIC_DEBUG_METADATA(descriptor, prefix_str);		\
	if (unlikely(descriptor.flags & _DPRINTK_FLAGS_PRINT))		\
		print_hex_dump(KERN_DEBUG, prefix_str,			\
			       DUMP_PREFIX_ADDRESS, 32, 1,		\
			       buf, len, false);			\
} while (0)

#define ENTER() pr_info(DRIVER_PREFIX "%s\n", __func__)

static inline void buff_to_be(u32 *buf, size_t len)
{
	int i;
	for (i = 0; i< len; i++)
	{
		buf[i] = cpu_to_be32(buf[i]);
	}
}
struct nv_data {
   int    	is_valid;
   void 	*table;
};
struct wcn_vif {
	u8 vif_id;
};
struct wcn_sta {
	u8 sta_id;
};
struct wcn36xx_dxe_ch;
struct wcn36xx {
	struct ieee80211_hw 	*hw;
	struct workqueue_struct 	*wq;
	struct workqueue_struct 	*ctl_wq;
	struct device 		*dev;
	const struct firmware 	*nv;
	struct mac_address addresses[2];
	u8 		ch;

	// IRQs
	int 			tx_irq; 	// TX complete irq
	int 			rx_irq; 	// RX ready irq
	void __iomem    	*mmio;

	// SMD related
	smd_channel_t 		*smd_ch;
	u8			*smd_buf;
	struct work_struct 	smd_work;
	struct work_struct 	start_work;
	struct work_struct 	rx_ready_work;
	struct completion 	smd_compl;

	// DXE chanels
	struct wcn36xx_dxe_ch 	dxe_tx_l_ch;	// TX low channel
	struct wcn36xx_dxe_ch 	dxe_tx_h_ch;	// TX high channel
	struct wcn36xx_dxe_ch 	dxe_rx_l_ch;	// RX low channel
	struct wcn36xx_dxe_ch 	dxe_rx_h_ch;	// RX high channel

	// Memory pools
	struct wcn36xx_dxe_mem_pool	mgmt_mem_pool;
	struct wcn36xx_dxe_mem_pool	data_mem_pool;
};

#endif	/* _WCN36XX_H_ */
