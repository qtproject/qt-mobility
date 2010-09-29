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


#ifndef QDECLARATIVEDEVICEINFO_P_H
#define QDECLARATIVEDEVICEINFO_P_H
/*

 Many more issues can be raised;
is it really "WallPower" not "MainsPowered"? shouldn't "BatteryNormal" be "BatteryGood"?
*/

#include <QObject>
#include "qsystemdeviceinfo.h"
QT_BEGIN_HEADER
QTM_USE_NAMESPACE

class QDeclarativeDeviceInfo : public QSystemDeviceInfo
{
    Q_OBJECT
public:
    explicit QDeclarativeDeviceInfo(QObject *parent = 0);


public slots:
    void startBatteryLevelChanged();
    void startBatteryStatusChanged();
    void startPowerStateChanged();
    void startCurrentProfileChanged();
    void startBluetoothStateChanged();

Q_SIGNALS:
    void batteryLevelChanged(int level);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus batteryStatus);
    void powerStateChanged(QSystemDeviceInfo::PowerState powerState);
    void currentProfileChanged(QSystemDeviceInfo::Profile currentProfile);
    void bluetoothStateChanged(bool on);

private slots:
    void declarativeBatteryLevelChanged(int level);
    void declarativeBatteryStatusChanged(QSystemDeviceInfo::BatteryStatus batteryStatus);
    void declarativePowerStateChanged(QSystemDeviceInfo::PowerState powerState);
    void declarativeCurrentProfileChanged(QSystemDeviceInfo::Profile currentProfile);
    void declarativeBluetoothStateChanged(bool on);
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // QDECLARATIVEDEVICEINFO_P_H
