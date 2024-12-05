#ifndef __BACKPORT_NETDEVICE_H
#define __BACKPORT_NETDEVICE_H
#include_next <linux/netdevice.h>
#include <linux/version.h>
#include <backport/magic.h>

#if LINUX_VERSION_IS_LESS(4,15,0)
static inline int _bp_netdev_upper_dev_link(struct net_device *dev,
					    struct net_device *upper_dev)
{
	return netdev_upper_dev_link(dev, upper_dev);
}
#define netdev_upper_dev_link3(dev, upper, extack) \
	netdev_upper_dev_link(dev, upper)
#define netdev_upper_dev_link2(dev, upper) \
	netdev_upper_dev_link(dev, upper)
#define netdev_upper_dev_link(...) \
	macro_dispatcher(netdev_upper_dev_link, __VA_ARGS__)(__VA_ARGS__)
#endif

#if LINUX_VERSION_IS_LESS(4,19,0)
static inline void netif_receive_skb_list(struct sk_buff_head *head)
{
	struct sk_buff *skb, *next;

	skb_queue_walk_safe(head, skb, next) {
		__skb_unlink(skb, head);
		netif_receive_skb(skb);
	}
}
#endif

#if LINUX_VERSION_IS_LESS(5,0,0)
static inline int backport_dev_open(struct net_device *dev, struct netlink_ext_ack *extack)
{
	return dev_open(dev);
}
#define dev_open LINUX_BACKPORT(dev_open)
#endif

#if LINUX_VERSION_IS_LESS(5,10,0)
#define dev_fetch_sw_netstats LINUX_BACKPORT(dev_fetch_sw_netstats)
void dev_fetch_sw_netstats(struct rtnl_link_stats64 *s,
			   const struct pcpu_sw_netstats __percpu *netstats);

#define netif_rx_any_context LINUX_BACKPORT(netif_rx_any_context)
int netif_rx_any_context(struct sk_buff *skb);

static inline void dev_sw_netstats_rx_add(struct net_device *dev, unsigned int len)
{
	struct pcpu_sw_netstats *tstats = this_cpu_ptr(dev->tstats);

	u64_stats_update_begin(&tstats->syncp);
	tstats->rx_bytes += len;
	tstats->rx_packets++;
	u64_stats_update_end(&tstats->syncp);
}

#endif /* < 5.10 */

#if LINUX_VERSION_IS_LESS(5,10,0)
static inline void dev_sw_netstats_tx_add(struct net_device *dev,
					  unsigned int packets,
					  unsigned int len)
{
	struct pcpu_sw_netstats *tstats = this_cpu_ptr(dev->tstats);

	u64_stats_update_begin(&tstats->syncp);
	tstats->tx_bytes += len;
	tstats->tx_packets += packets;
	u64_stats_update_end(&tstats->syncp);
}
#endif /* < 5.10 */

#if LINUX_VERSION_IS_LESS(5,10,0)
#define dev_sw_netstats_rx_add LINUX_BACKPORT(dev_sw_netstats_rx_add)
static inline void dev_sw_netstats_rx_add(struct net_device *dev, unsigned int len)
{
	struct pcpu_sw_netstats *tstats = this_cpu_ptr(dev->tstats);

	u64_stats_update_begin(&tstats->syncp);
	tstats->rx_bytes += len;
	tstats->rx_packets++;
	u64_stats_update_end(&tstats->syncp);
}
#endif /* < 5.10 */

#if LINUX_VERSION_IS_LESS(5,11,0)
#define dev_sw_netstats_tx_add LINUX_BACKPORT(dev_sw_netstats_tx_add)
static inline void dev_sw_netstats_tx_add(struct net_device *dev,
					  unsigned int packets,
					  unsigned int len)
{
	struct pcpu_sw_netstats *tstats = this_cpu_ptr(dev->tstats);

	u64_stats_update_begin(&tstats->syncp);
	tstats->tx_bytes += len;
	tstats->tx_packets += packets;
	u64_stats_update_end(&tstats->syncp);
}
#endif /* < 5.11 */

#if LINUX_VERSION_IS_LESS(5,11,0)
#define dev_get_tstats64 LINUX_BACKPORT(dev_get_tstats64)
void dev_get_tstats64(struct net_device *dev, struct rtnl_link_stats64 *s);
#endif /* < 5.11 */

#if LINUX_VERSION_IS_LESS(5,15,0)
#define get_user_ifreq LINUX_BACKPORT(get_user_ifreq)
int get_user_ifreq(struct ifreq *ifr, void __user **ifrdata, void __user *arg);
#define put_user_ifreq LINUX_BACKPORT(put_user_ifreq)
int put_user_ifreq(struct ifreq *ifr, void __user *arg);
#endif

#if LINUX_VERSION_IS_LESS(5,15,0)
static inline void backport_dev_put(struct net_device *dev)
{
	if (dev)
		dev_put(dev);
}
#define dev_put LINUX_BACKPORT(dev_put)

static inline void backport_dev_hold(struct net_device *dev)
{
	if (dev)
		dev_hold(dev);
}
#define dev_hold LINUX_BACKPORT(dev_hold)
#endif /* < 5.15 */

#ifndef NET_DEVICE_PATH_STACK_MAX
enum net_device_path_type {
	DEV_PATH_ETHERNET = 0,
	DEV_PATH_VLAN,
	DEV_PATH_BRIDGE,
	DEV_PATH_PPPOE,
	DEV_PATH_DSA,
};

struct net_device_path {
	enum net_device_path_type	type;
	const struct net_device		*dev;
	union {
		struct {
			u16		id;
			__be16		proto;
			u8		h_dest[ETH_ALEN];
		} encap;
		struct {
			enum {
				DEV_PATH_BR_VLAN_KEEP,
				DEV_PATH_BR_VLAN_TAG,
				DEV_PATH_BR_VLAN_UNTAG,
				DEV_PATH_BR_VLAN_UNTAG_HW,
			}		vlan_mode;
			u16		vlan_id;
			__be16		vlan_proto;
		} bridge;
		struct {
			int port;
			u16 proto;
		} dsa;
	};
};

#define NET_DEVICE_PATH_STACK_MAX	5
#define NET_DEVICE_PATH_VLAN_MAX	2

struct net_device_path_stack {
	int			num_paths;
	struct net_device_path	path[NET_DEVICE_PATH_STACK_MAX];
};

struct net_device_path_ctx {
	const struct net_device *dev;
	const u8		*daddr;

	int			num_vlans;
	struct {
		u16		id;
		__be16		proto;
	} vlan[NET_DEVICE_PATH_VLAN_MAX];
};
#endif /* NET_DEVICE_PATH_STACK_MAX */

#if LINUX_VERSION_IS_LESS(5,17,0)
static inline void backport_txq_trans_cond_update(struct netdev_queue *txq)
{
	unsigned long now = jiffies;

	if (READ_ONCE(txq->trans_start) != now)
		WRITE_ONCE(txq->trans_start, now);
}
#define txq_trans_cond_update LINUX_BACKPORT(txq_trans_cond_update)
#endif /* < 5.17 */

#if LINUX_VERSION_IS_LESS(5,19,0)
#define netif_napi_add_weight LINUX_BACKPORT(netif_napi_add_weight)
static inline void netif_napi_add_weight(struct net_device *dev,
				   struct napi_struct *napi,
				   int (*poll)(struct napi_struct *, int),
				   int weight)
{
	netif_napi_add(dev, napi, poll, weight);
}
#endif /* < 5.19.0 */

#if LINUX_VERSION_IS_LESS(6,1,0)
static inline void backport_netif_napi_add(struct net_device *dev,
					   struct napi_struct *napi,
					   int (*poll)(struct napi_struct *, int))
{
	netif_napi_add(dev, napi, poll, NAPI_POLL_WEIGHT);
}
#define netif_napi_add LINUX_BACKPORT(netif_napi_add)

static inline void backport_netif_napi_add_tx(struct net_device *dev,
					      struct napi_struct *napi,
					      int (*poll)(struct napi_struct *, int))
{
	netif_tx_napi_add(dev, napi, poll, NAPI_POLL_WEIGHT);
}
#define netif_napi_add_tx LINUX_BACKPORT(netif_napi_add_tx)
#endif /* < 6.1 */

#if LINUX_VERSION_IS_LESS(5,15,0)
#define dev_addr_mod LINUX_BACKPORT(dev_addr_mod)
static inline void
dev_addr_mod(struct net_device *dev, unsigned int offset,
	const void *addr, size_t len)
{
	memcpy(&dev->dev_addr[offset], addr, len);
}
#endif /* < 5.15 */

#if LINUX_VERSION_IS_LESS(5,13,0)
#define dev_set_threaded LINUX_BACKPORT(dev_set_threaded)
static inline int dev_set_threaded(struct net_device *dev, bool threaded)
{
	return -EOPNOTSUPP;
}
#endif /* < 5.13 */

#endif /* __BACKPORT_NETDEVICE_H */
