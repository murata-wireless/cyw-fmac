#ifndef __BACKPORT_DEVICE_H_
#define __BACKPORT_DEVICE_H_
#include_next <linux/device.h>

#ifndef DEVICE_ATTR_ADMIN_RW
#define DEVICE_ATTR_ADMIN_RW(_name) \
	struct device_attribute dev_attr_##_name = __ATTR_RW_MODE(_name, 0600)
#endif

#ifndef DEVICE_ATTR_ADMIN_RO
#define DEVICE_ATTR_ADMIN_RO(_name) \
	struct device_attribute dev_attr_##_name = __ATTR_RO_MODE(_name, 0400)
#endif

#endif /* __BACKPORT_DEVICE_H_ */
