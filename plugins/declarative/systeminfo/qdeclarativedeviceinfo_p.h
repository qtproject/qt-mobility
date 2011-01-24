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

#include <QObject>
#include "qmobilityglobal.h"

#include "qsystemdeviceinfo.h"

QT_BEGIN_HEADER
QTM_USE_NAMESPACE

class QDeclarativeDeviceInfo : public QSystemDeviceInfo
{
    Q_OBJECT
    Q_PROPERTY(bool monitorBatteryLevelChanges READ monitorBatteryLevelChanges WRITE startBatteryLevelChanged CONSTANT)
    Q_PROPERTY(bool monitorBatteryStatusChanges READ monitorBatteryStatusChanges WRITE startBatteryStatusChanged CONSTANT)
    Q_PROPERTY(bool monitorPowerStateChanges READ monitorPowerStateChanges WRITE startPowerStateChanged CONSTANT)
    Q_PROPERTY(bool monitorCurrentProfileChanges READ monitorCurrentProfileChanges WRITE startCurrentProfileChanged CONSTANT)
    Q_PROPERTY(bool monitorBluetoothStateChanges READ monitorBluetoothStateChanges WRITE startBluetoothStateChanged CONSTANT)

    Q_PROPERTY(bool monitorWirelessKeyboardConnects READ monitorWirelessKeyboardConnects WRITE startWirelessKeyboardConnected CONSTANT)
    Q_PROPERTY(bool monitorKeyboardFlips READ monitorKeyboardFlips WRITE startKeyboardFlipped CONSTANT)
    Q_PROPERTY(bool monitorDeviceLocks READ monitorDeviceLocks WRITE startDeviceLocked CONSTANT)
    Q_PROPERTY(bool monitorLockStatusChanges READ monitorLockStatusChanges WRITE startLockStatusChanged CONSTANT)

    Q_PROPERTY(int messageRingtoneVolume READ messageRingtoneVolume CONSTANT)
    Q_PROPERTY(int voiceRingtoneVolume READ voiceRingtoneVolume CONSTANT)
    Q_PROPERTY(bool vibrationActive READ vibrationActive CONSTANT)

    Q_PROPERTY(QString uniqueID READ uniqueID CONSTANT)

public:
    explicit QDeclarativeDeviceInfo(QObject *parent = 0);

    bool monitorBatteryLevelChanges();
    void startBatteryLevelChanged(bool on);

    bool monitorBatteryStatusChanges();
    void startBatteryStatusChanged(bool on);

    bool monitorPowerStateChanges();
    void startPowerStateChanged(bool on);

    bool monitorCurrentProfileChanges();
    void startCurrentProfileChanged(bool on);

    bool monitorBluetoothStateChanges();
    void startBluetoothStateChanged(bool on);


    bool monitorWirelessKeyboardConnects();
    void startWirelessKeyboardConnected(bool on);

    bool monitorKeyboardFlips();
    void startKeyboardFlipped(bool on);

    bool monitorDeviceLocks();
    void startDeviceLocked(bool on);

    bool monitorLockStatusChanges();
    void startLockStatusChanged(bool on);

    int messageRingtoneVolume();
    int voiceRingtoneVolume();
    bool vibrationActive();

    QString uniqueID();


 protected:

    bool monitoringBatteryLevel;
    bool monitoringBatteryStatus;
    bool monitoringPowerState;
    bool monitoringCurrentProfile;
    bool monitorBluetoothState;

    bool monitoringWirelessKeyboard;
    bool monitoringKeyboardFlipped;
    bool monitoringDeviceLocked;
    bool monitoringLockStatusChanged;

    QSystemDeviceInfo::ProfileDetails currentProfileDetails;

};

QT_END_NAMESPACE
QT_END_HEADER
#endif // QDECLARATIVEDEVICEINFO_P_H
