/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qudevservice_linux_p.h"
#include <QDebug>

QTM_BEGIN_NAMESPACE

QUdevService::QUdevService()
{
    // NOTE: udev_new can fail, if we get a NULL pointer back, can't create udev
    context = udev_new();
}

QUdevService::~QUdevService()
{
    if (context) {
        udev_unref(context), context = 0;
    }
}

bool QUdevService::isSubsystemAvailable(const char *subsystem)
{
    bool available = false;
    struct udev_enumerate *enumerate = 0;

    if (!context) goto CLEAN;
    if (!(enumerate = udev_enumerate_new(context))) goto CLEAN;
    if (0 != udev_enumerate_add_match_subsystem(enumerate, subsystem)) goto CLEAN;
    if (0 != udev_enumerate_scan_devices(enumerate)) goto CLEAN;

    available = (0 != udev_enumerate_get_list_entry(enumerate)); 

    CLEAN:

    if (enumerate) {
        udev_enumerate_unref(enumerate);
    }
    return available;
}

bool QUdevService::isPropertyAvailable(const char *property, const char *value)
{
    bool available = false;
    struct udev_enumerate *enumerate = 0;

    if (!context) goto CLEAN;
    if (!(enumerate = udev_enumerate_new(context))) goto CLEAN;
    if (0 != udev_enumerate_add_match_property(enumerate, property, value)) goto CLEAN;
    if (0 != udev_enumerate_scan_devices(enumerate)) goto CLEAN;

    available = (0 != udev_enumerate_get_list_entry(enumerate));

    CLEAN:

    if (enumerate) {
        udev_enumerate_unref(enumerate);
    }
    return available;
}

QUdevFeatureMatrix QUdevService::availableFeatures()
{
    QUdevFeatureMatrix featureMatrix = {
        /* bluetooth   */  isSubsystemAvailable(UDEV_SUBSYSTEM_BLUETOOTH),
        /* camera      */  isPropertyAvailable(UDEV_PROPERTY_V4L_CAP, "*:capture:*"),
        /* gps         */  isPropertyAvailable(UDEV_PROPERTY_DRIVER, "*gps*"),
        /* haptics     */  (isPropertyAvailable(UDEV_PROPERTY_NAME, "*touch*")
        || isPropertyAvailable(UDEV_PROPERTY_NAME, "*Touch*")),
        /* infrared    */  isPropertyAvailable(UDEV_PROPERTY_DRIVER, "*irda*"),
        /* leds        */  isSubsystemAvailable(UDEV_SUBSYSTEM_LEDS),
        /* memcard     */  isSubsystemAvailable(UDEV_SUBSYSTEM_MEMCARD),
        /* radio       */  isPropertyAvailable(UDEV_PROPERTY_V4L_CAP, "*:radio:*"),
        /* usb         */  (isPropertyAvailable(UDEV_PROPERTY_DRIVER, UDEV_DRIVER_MUSB)
        || isPropertyAvailable(UDEV_PROPERTY_DRIVER, UDEV_DRIVER_USB)
        || isPropertyAvailable(UDEV_PROPERTY_INTERFACE, "usb*")),
        /* vibration   */  (isPropertyAvailable(UDEV_PROPERTY_NAME, "*vibra*")
        || isPropertyAvailable(UDEV_PROPERTY_NAME, "*Vibra*")),
        /* videoOut    */  isPropertyAvailable(UDEV_PROPERTY_V4L_CAP, "*:video_output:*"),
        /* wlan        */  isSubsystemAvailable(UDEV_SUBSYSTEM_WLAN)
    };
    return featureMatrix;
}

#include "moc_qudevservice_linux_p.cpp"

QTM_END_NAMESPACE
