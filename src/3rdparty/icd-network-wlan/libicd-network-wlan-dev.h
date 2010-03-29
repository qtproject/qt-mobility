#ifndef WLAN_DEV_H
#define WLAN_DEV_H

/**
 * Copyright (C) 2007,2008 Nokia Corporation. All rights reserved.
 *
 * @author jukka.rissanen@nokia.com
 *
 * @file libicd-network-wlan-dev.h
 */

#include <glib.h>
#include <dbus/dbus.h>
#include <wlancond.h>
#include <icd/network_api_defines.h>


/* Difference between network attributes value and wlan capabilities:
 *
 *                                    WLAN
 *             Network             capability             Combined
 *   LSB      attribute         (from wlancond.h)
 *  ----      ---------            ----------             --------
 *    0                               mode                  mode
 *    1                               mode                  mode
 *    2                               mode                  mode
 *    3                          autoconnect attempt       method
 *    4                             encrypt method         method
 *    5                             encrypt method         method
 *    6                             encrypt method         method
 *    7                             encrypt method          WPA2
 *    8                               WPA2                algorithm
 *    9                               WPS                 algorithm
 *   10                              WPS PIN              algorithm
 *   11                             WPS push button       algorithm
 *   12                             WPS configured          WPS
 *   13                                                    WPS PIN
 *   14                                                  WPS push button
 *   15                                                  WPS configured 
 *   16                               rate
 *   17                               rate
 *   18                               rate
 *   19                               rate
 *   20                               rate
 *   21                               rate
 *   22                               rate
 *   23        iap name               rate                 iap name
 *   24         silent                rate                  silent
 *   25                               rate
 *   26      
 *   27
 *   28                           security algorithm
 *   29      always online        security algorithm      always online
 *   30                           security algorithm
 *   31                           security algorithm
 *   MSB
 *
 *
 * Same view from different angle.
 *
 *   MSB                            LSB   (of 32 bits)
 *    10987654321098765432109876543210
 *
 *                    WWWWaaaaWmmmmmmm    Combined format
 *                    PPPPllllPeeeeooo 
 *                    SSSSggggAttttddd 
 *                    cbp oooo2hhhheee
 *                    fti      oooo 
 *                    gnn      dddd
 *
 *    aaaa                   Wmmmm mmm    Capability format
 *    llll                   Peeee ooo 
 *    gggg                   Atttt ddd 
 *    oooo                   2hhhh eee
 *                            oooo 
 *                            dddd
 *
 *
 * Note that WPS bits in nwattrs are moved towards MSB because algorithm
 * was there first and we do not want to break binary compatibility.
 */

/* capability bits inside network attributes var */
#define NWATTR_WPS_MASK       0x0000F000
#define NWATTR_ALGORITHM_MASK 0x00000F00
#define NWATTR_WPA2_MASK      0x00000080
#define NWATTR_METHOD_MASK    0x00000078
#define NWATTR_MODE_MASK      0x00000007

#define CAP_LOCALMASK         0x0FFFE008

/* how much to shift between capability and network attributes var */
#define CAP_SHIFT_WPS        3
#define CAP_SHIFT_ALGORITHM 20
#define CAP_SHIFT_WPA2       1
#define CAP_SHIFT_METHOD     1
#define CAP_SHIFT_MODE       0
#define CAP_SHIFT_ALWAYS_ONLINE 26

/* ------------------------------------------------------------------------- */
/* From combined to capability */
static inline dbus_uint32_t nwattr2cap(guint nwattrs, dbus_uint32_t *cap)
{
	guint oldval = *cap;

	*cap &= CAP_LOCALMASK; /* clear old capabilities */
	*cap |=
		((nwattrs & ICD_NW_ATTR_ALWAYS_ONLINE) >> CAP_SHIFT_ALWAYS_ONLINE) |
		((nwattrs & NWATTR_WPS_MASK) >> CAP_SHIFT_WPS) |
		((nwattrs & NWATTR_ALGORITHM_MASK) << CAP_SHIFT_ALGORITHM) |
		((nwattrs & NWATTR_WPA2_MASK) << CAP_SHIFT_WPA2) |
		((nwattrs & NWATTR_METHOD_MASK) << CAP_SHIFT_METHOD) |
		(nwattrs & NWATTR_MODE_MASK);

	return oldval;
}


/* ------------------------------------------------------------------------- */
/* From capability to combined */
static inline guint cap2nwattr(dbus_uint32_t cap, guint *nwattrs)
{
	guint oldval = *nwattrs;

	*nwattrs &= ~ICD_NW_ATTR_LOCALMASK; /* clear old capabilities */
        *nwattrs |=
#ifdef WLANCOND_WPS_MASK
		((cap & WLANCOND_WPS_MASK) << CAP_SHIFT_WPS) |
#endif
		((cap & (WLANCOND_ENCRYPT_ALG_MASK |
			 WLANCOND_ENCRYPT_GROUP_ALG_MASK)) >> CAP_SHIFT_ALGORITHM)|
		((cap & WLANCOND_ENCRYPT_WPA2_MASK) >> CAP_SHIFT_WPA2) |
		((cap & WLANCOND_ENCRYPT_METHOD_MASK) >> CAP_SHIFT_METHOD) |
		(cap & WLANCOND_MODE_MASK);

	return oldval;
}


#endif
