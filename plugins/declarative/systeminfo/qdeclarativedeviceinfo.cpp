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


#include "qdeclarativedeviceinfo_p.h"
#include "qsystemdeviceinfo.h"
#include <QMetaType>

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QSystemDeviceInfo, deviceInfo)

QDeclarativeDeviceInfo::QDeclarativeDeviceInfo(QObject *parent) :
    QSystemDeviceInfo(parent)
{
}

void QDeclarativeDeviceInfo::startBatteryLevelChanged()
{
    connect(deviceInfo(),SIGNAL(batteryLevelChanged(int)),
            this,SLOT(declarativeBatteryLevelChanged(int)));
}

void QDeclarativeDeviceInfo::startBatteryStatusChanged()
{
    connect(deviceInfo(),SIGNAL(batteryStatusChanged(QSystemDeviceInfo::BatteryStatus)),
            this,SLOT(declarativeBatteryStatusChanged(QSystemDeviceInfo::BatteryStatus)));
}

void QDeclarativeDeviceInfo::startPowerStateChanged()
{
    connect(deviceInfo(),SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
            this,SLOT(declarativePowerStateChanged(QSystemDeviceInfo::PowerState)));
}

void QDeclarativeDeviceInfo::startCurrentProfileChanged()
{
    connect(deviceInfo(),SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)),
            this,SLOT(declarativeCurrentProfileChanged(QSystemDeviceInfo::Profile)));
}

void QDeclarativeDeviceInfo::startBluetoothStateChanged()
{
    connect(deviceInfo(),SIGNAL(bluetoothStateChanged(bool)),
            this,SLOT(declarativeBluetoothStateChanged(bool)));
}

void QDeclarativeDeviceInfo::declarativeBatteryLevelChanged(int level)
{
    Q_EMIT batteryLevelChanged(level);
}

void QDeclarativeDeviceInfo::declarativeBatteryStatusChanged(QSystemDeviceInfo::BatteryStatus batteryStatus)
{
    Q_EMIT batteryStatusChanged(batteryStatus);
}

void QDeclarativeDeviceInfo::declarativePowerStateChanged(QSystemDeviceInfo::PowerState powerState)
{
    Q_EMIT powerStateChanged(powerState);
}

void QDeclarativeDeviceInfo::declarativeCurrentProfileChanged(QSystemDeviceInfo::Profile currentProfile)
{
    Q_EMIT currentProfileChanged(currentProfile);
}

void QDeclarativeDeviceInfo::declarativeBluetoothStateChanged(bool on)
{
    Q_EMIT bluetoothStateChanged(on);
}


