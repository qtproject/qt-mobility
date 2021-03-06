/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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
    Q_PROPERTY(bool monitorBatteryLevelChanges READ monitorBatteryLevelChanges WRITE startBatteryLevelChanged)
    Q_PROPERTY(bool monitorBatteryStatusChanges READ monitorBatteryStatusChanges WRITE startBatteryStatusChanged)
    Q_PROPERTY(bool monitorPowerStateChanges READ monitorPowerStateChanges WRITE startPowerStateChanged)
    Q_PROPERTY(bool monitorCurrentProfileChanges READ monitorCurrentProfileChanges WRITE startCurrentProfileChanged)
    Q_PROPERTY(bool monitorBluetoothStateChanges READ monitorBluetoothStateChanges WRITE startBluetoothStateChanged)
    Q_PROPERTY(bool monitorThermalStateChanges READ monitorThermalStateChanges WRITE startThermalStateChanged)

    Q_PROPERTY(bool monitorWirelessKeyboardConnects READ monitorWirelessKeyboardConnects WRITE startWirelessKeyboardConnected)
    Q_PROPERTY(bool monitorKeyboardFlips READ monitorKeyboardFlips WRITE startKeyboardFlipped)
    Q_PROPERTY(bool monitorDeviceLocks READ monitorDeviceLocks WRITE startDeviceLocked)
    Q_PROPERTY(bool monitorLockStatusChanges READ monitorLockStatusChanges WRITE startLockStatusChanged)

    Q_PROPERTY(int messageRingtoneVolume READ messageRingtoneVolume CONSTANT)
    Q_PROPERTY(int voiceRingtoneVolume READ voiceRingtoneVolume CONSTANT)
    Q_PROPERTY(bool vibrationActive READ vibrationActive CONSTANT)

    Q_PROPERTY(bool primaryKeypadLightOn READ primaryKeypadLightOn CONSTANT)
    Q_PROPERTY(bool secondaryKeypadLightOn READ secondaryKeypadLightOn CONSTANT)

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

    bool monitorThermalStateChanges();
    void startThermalStateChanged(bool on);

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

    bool primaryKeypadLightOn();
    bool secondaryKeypadLightOn();


 protected:

    bool monitoringBatteryLevel;
    bool monitoringBatteryStatus;
    bool monitoringPowerState;
    bool monitoringCurrentProfile;
    bool monitorBluetoothState;
    bool monitorThermalState;

    bool monitoringWirelessKeyboard;
    bool monitoringKeyboardFlipped;
    bool monitoringDeviceLocked;
    bool monitoringLockStatusChanged;

    QSystemDeviceInfo::ProfileDetails currentProfileDetails;

};

QT_END_NAMESPACE
QT_END_HEADER
#endif // QDECLARATIVEDEVICEINFO_P_H
