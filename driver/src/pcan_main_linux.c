// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (C) 2001-2009  PEAK System-Technik GmbH
 *
 * linux@peak-system.com
 * www.peak-system.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Maintainer(s): Stephane Grosjean (s.grosjean@peak-system.com)
 *
 * Major contributions by:
 *                Edouard Tisserant (edouard.tisserant@lolitech.fr) XENOMAI
 *                Laurent Bessard   (laurent.bessard@lolitech.fr)   XENOMAI
 *                Oliver Hartkopp   (oliver.hartkopp@volkswagen.de) socketCAN
 *                Klaus Hitschler   (klaus.hitschler@gmx.de)
 *
 * Contributions: Marcel Offermans (marcel.offermans@luminis.nl)
 *                Philipp Baer     (philipp.baer@informatik.uni-ulm.de)
 *                Garth Zeglin     (garthz@ri.cmu.edu)
 *                Harald Koenig    (H.Koenig@science-computing.de)
 *****************************************************************************/

/*****************************************************************************
 *
 * pcan_main_linux.c - the starting point of the driver,
 *               init and cleanup and proc interface
 *
 * $Id$
 *
 *****************************************************************************/

#define DEV_REGISTER()		dev_register()
#define DEV_UNREGISTER()	dev_unregister()
#define REMOVE_DEV_LIST		remove_dev_list

static int pcan_is_hotplug(struct pcandev *dev)
{
	switch (dev->wType) {
	case HW_PCI:
	case HW_PCIE_FD:
	case HW_ISA:
		/* udev events were not generated for ISA.
		 * Thx David Leonard */
	case HW_ISA_SJA:
	case HW_DONGLE_SJA:
	case HW_DONGLE_SJA_EPP:
		return 0;
	default:
		break;
	}

	/* hotplug devices have to register/unregister by themselves */
	return 1;
}

/* contrary to former implementation this function only registers devices and
 * do register a driver nor request a major number in case of dynamic major
 * number allocation */
static int dev_register(void)
{
	DPRINTK(KERN_DEBUG "%s: %s()\n", DEVICE_NAME, __func__);

#ifdef NETDEV_SUPPORT
	{
		struct list_head *ptr;
		struct pcandev *pdev;
		unsigned long flags;

		/* create all netdevice entries except those for hotplug-devices
		 * USB   : is done by pcan_usb_plugin().
		 * PCCARD: is done by pcan_pccard_register_devices() at driver
		 *         init time (here & now! - see above) or at plugin
		 *         time. */
		pcan_lock_get_irqsave(&pcan_drv.devices_lock, flags);

		for (ptr=pcan_drv.devices.next; ptr != &pcan_drv.devices;
								ptr=ptr->next) {
			pdev = (struct pcandev *)ptr;
			if (!pcan_is_hotplug(pdev))
				pcan_netdev_register(pdev);
		}

		pcan_lock_put_irqrestore(&pcan_drv.devices_lock, flags);
	}
#endif

#ifdef UDEV_SUPPORT
	{
		struct list_head *ptr;
		struct pcandev *pdev;
		unsigned long flags;

		pcan_lock_get_irqsave(&pcan_drv.devices_lock, flags);

		for (ptr = pcan_drv.devices.next; ptr != &pcan_drv.devices;
							ptr = ptr->next) {

			pdev = (struct pcandev *)ptr;
			if (!pcan_is_hotplug(pdev))
				pcan_sysfs_dev_node_create(pdev);
		}

		pcan_lock_put_irqrestore(&pcan_drv.devices_lock, flags);
	}
#endif

	/* for compatibility to former implementation it is returned */
	return pcan_drv.nMajor;
}

/* contrary to former implementation this function only unregisters only
 * devices */
void dev_unregister(void)
{
#ifdef DEBUG
	pr_info(DEVICE_NAME ": %s()\n", __func__);
#endif

#ifdef UDEV_SUPPORT
	{
		struct list_head *ptr;
		struct pcandev *pdev;
		unsigned long flags;

		pcan_lock_get_irqsave(&pcan_drv.devices_lock, flags);

		for (ptr=pcan_drv.devices.next; ptr != &pcan_drv.devices;
								ptr=ptr->next) {
			pdev = (struct pcandev *)ptr;
			if (!pcan_is_hotplug(pdev))
				pcan_sysfs_dev_node_destroy(pdev);
		}

		pcan_lock_put_irqrestore(&pcan_drv.devices_lock, flags);
	}
#endif

#ifdef NETDEV_SUPPORT
	{
		struct list_head *ptr;
		struct pcandev *pdev;
		unsigned long flags;

		/* remove all netdevice registrations except those for
		 * USB-devices which is done by pcan_usb_plugout(). */
		pcan_lock_get_irqsave(&pcan_drv.devices_lock, flags);

		for (ptr=pcan_drv.devices.next; ptr != &pcan_drv.devices;
								ptr=ptr->next) {
			pdev = (struct pcandev *)ptr;
			if (!pcan_is_hotplug(pdev))
				pcan_netdev_unregister(pdev);
		}

		pcan_lock_put_irqrestore(&pcan_drv.devices_lock, flags);
	}
#endif
}
