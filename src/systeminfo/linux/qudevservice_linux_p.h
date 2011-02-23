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

#ifndef QUDEVSERVICE_H
#define QUDEVSERVICE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QObject>
#include "qmobilityglobal.h"
#include <libudev.h>

QTM_BEGIN_NAMESPACE

static const char *UDEV_DRIVER_MUSB         = "musb_hdrc";
static const char *UDEV_DRIVER_USB          = "usb";

static const char *UDEV_SUBSYSTEM_BLUETOOTH = "bluetooth";
static const char *UDEV_SUBSYSTEM_LEDS      = "leds";
static const char *UDEV_SUBSYSTEM_MEMCARD   = "mmc_host";
static const char *UDEV_SUBSYSTEM_WLAN      = "ieee80211";

static const char *UDEV_PROPERTY_DRIVER     = "DRIVER";
static const char *UDEV_PROPERTY_INTERFACE  = "INTERFACE";
static const char *UDEV_PROPERTY_NAME       = "NAME";
static const char *UDEV_PROPERTY_V4L_CAP    = "ID_V4L_CAPABILITIES";

struct QUdevFeatureMatrix {
    bool bluetooth;
    bool camera;
    bool gps;
    bool haptics;
    bool infrared;
    bool leds;
    bool memcard;
    bool radio;
    bool usb;
    bool vibration;
    bool videoOut;
    bool wlan;
};

class QUdevService : public QObject {
    Q_OBJECT

public:
    QUdevService();
    virtual ~QUdevService();
    bool isSubsystemAvailable(const char *subsystem);
    bool isPropertyAvailable(const char *property, const char *value);
    QUdevFeatureMatrix availableFeatures();

private:
    struct udev *context;
};

QTM_END_NAMESPACE

#endif // QUDEVSERVICE_H
