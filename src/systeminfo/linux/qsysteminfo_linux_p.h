/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QSYSTEMINFO_LINUX_P_H
#define QSYSTEMINFO_LINUX_P_H

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

#include "qmobilityglobal.h"
#include "qsysteminfo_linux_common_p.h"

#if !defined(QT_NO_NETWORKMANAGER)
#include "qnetworkmanagerservice_linux_p.h"
#endif // QT_NO_NETWORKMANAGER

QTM_BEGIN_NAMESPACE

class QSystemInfoPrivate : public QSystemInfoLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemInfoPrivate(QSystemInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemInfoPrivate();

    QStringList availableLanguages() const;
};

class QSystemNetworkInfoPrivate : public QSystemNetworkInfoLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemNetworkInfoPrivate(QSystemNetworkInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    QString networkName(QSystemNetworkInfo::NetworkMode mode);
    QSystemNetworkInfo::NetworkMode currentMode();
    QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);

#if !defined(QT_NO_NETWORKMANAGER)
private Q_SLOTS:
    void nmPropertiesChanged(const QString &path, QMap<QString, QVariant> map);
    void nmAPPropertiesChanged(const QString &path, QMap<QString, QVariant> map);

private:
    QNetworkManagerInterface *iface;
    QNetworkManagerInterfaceDeviceWired *devWiredIface;
    QNetworkManagerInterfaceDeviceWireless *devWirelessIface;
    QNetworkManagerInterfaceAccessPoint *accessPointIface;
    QNetworkManagerInterfaceDeviceGsm *devGsmIface;
    QMap<QString, QString> activePaths;

    void setupNmConnections();
    void updateActivePaths();
    inline QSystemNetworkInfo::NetworkMode deviceTypeToMode(quint32 type);
#endif // QT_NO_NETWORKMANAGER
};

class QSystemDisplayInfoPrivate : public QSystemDisplayInfoLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemDisplayInfoPrivate(QSystemDisplayInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();
};

class QSystemDeviceInfoPrivate : public QSystemDeviceInfoLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemDeviceInfoPrivate(QSystemDeviceInfoLinuxCommonPrivate *parent = 0);
    ~QSystemDeviceInfoPrivate();

    bool isDeviceLocked();
    bool isKeyboardFlippedOpen(); //1.2
    bool keypadLightOn(QSystemDeviceInfo::KeypadType type); //1.2
    bool vibrationActive(); //1.2
    int messageRingtoneVolume(); //1.2
    int voiceRingtoneVolume(); //1.2
    QString imei();
    QString imsi();
    QString model();
    QString productName();
    QSystemDeviceInfo::ProfileDetails currentProfileDetails;
    QSystemDeviceInfo::LockTypeFlags lockStatus(); //1.2
    QSystemDeviceInfo::Profile currentProfile();
    QSystemDeviceInfo::SimStatus simStatus();
    QSystemDeviceInfo::ThermalState currentThermalState();
};

class QSystemScreenSaverPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QObject *parent = 0);
    ~QSystemScreenSaverPrivate();

    bool screenSaverInhibited();
    bool setScreenSaverInhibit();
    bool isScreenLockEnabled();
    bool isScreenSaverActive();
    void setScreenSaverInhibited(bool on);

private:
    bool gnomeIsRunning;
    bool kdeIsRunning;
    bool meegoIsRunning;
    bool screenSaverIsInhibited;
    bool screenSaverSecure;
    uint currentPid;
    QString screenPath;
    QString settingsPath;

    void whichWMRunning();
#if defined(Q_WS_X11)
     int changeTimeout(int timeout);
#endif // Q_WS_X11
};

class QSystemBatteryInfoPrivate : public QSystemBatteryInfoLinuxCommonPrivate
{
    Q_OBJECT
public:
    QSystemBatteryInfoPrivate(QSystemBatteryInfoLinuxCommonPrivate *parent = 0);
    ~QSystemBatteryInfoPrivate();
};

QTM_END_NAMESPACE

#endif // QSYSTEMSINFO_LINUX_P_H
