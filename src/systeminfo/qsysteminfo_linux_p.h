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


#include <QObject>
#include <QSize>
#include <QHash>

#include "qsysteminfo_linux_common_p.h"
#include "qsysteminfo.h"
#include <qmobilityglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE
class QStringList;
class QTimer;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QSystemNetworkInfo;
class QSystemInfoPrivate : public QSystemInfoLinuxCommonPrivate
{
    Q_OBJECT

public:

    QSystemInfoPrivate(QSystemInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemInfoPrivate();
    QStringList availableLanguages() const;

    QString version(QSystemInfo::Version,  const QString &parameter = QString());

private:
#if !defined(QT_NO_DBUS)
    bool hasHalDeviceFeature(const QString &param);
    bool hasHalUsbFeature(qint32 usbClass);
    QHalInterface halIface;
#endif
};

class QNetworkManagerInterface;
class QNetworkManagerInterfaceDeviceWired;
class QNetworkManagerInterfaceDeviceWireless;
class QNetworkManagerInterfaceAccessPoint;

class QSystemNetworkInfoPrivate : public QSystemNetworkInfoLinuxCommonPrivate
{
    Q_OBJECT

public:

    QSystemNetworkInfoPrivate(QSystemNetworkInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    qint32 networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    int cellId();
    int locationAreaCode();

    QString currentMobileCountryCode();
    QString currentMobileNetworkCode();

    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();
    QSystemNetworkInfo::NetworkMode currentMode();

public Q_SLOTS:
#if !defined(QT_NO_NETWORKMANAGER)
        void primaryModeChanged();
#endif

private:
#if !defined(QT_NO_NETWORKMANAGER)
    QNetworkManagerInterface *iface;
    QNetworkManagerInterfaceDeviceWired * devWiredIface;
    QNetworkManagerInterfaceDeviceWireless *devWirelessIface;
    QNetworkManagerInterfaceAccessPoint *accessPointIface;

    void setupNmConnections();
    bool isDefaultConnectionPath(const QString &path);

    QMap <QString, QString> activePaths;
    void updateActivePaths();
    

    inline QSystemNetworkInfo::NetworkMode deviceTypeToMode(quint32 type);

#endif
    QString getSysNetName(QSystemNetworkInfo::NetworkMode mode);

private Q_SLOTS:
#if !defined(QT_NO_NETWORKMANAGER)
    void nmPropertiesChanged( const QString &, QMap<QString,QVariant>);
    void nmAPPropertiesChanged( const QString &, QMap<QString,QVariant>);
#endif
};

class QSystemDisplayInfoPrivate : public QSystemDisplayInfoLinuxCommonPrivate
{
    Q_OBJECT

public:

    QSystemDisplayInfoPrivate(QSystemDisplayInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();
};

class QSystemStorageInfoPrivate : public QSystemStorageInfoLinuxCommonPrivate
{
    Q_OBJECT

public:

    QSystemStorageInfoPrivate(QSystemStorageInfoLinuxCommonPrivate *parent = 0);
    virtual ~QSystemStorageInfoPrivate();
};

class QSystemDeviceInfoPrivate : public QSystemDeviceInfoLinuxCommonPrivate
{
    Q_OBJECT

public:

    QSystemDeviceInfoPrivate(QSystemDeviceInfoLinuxCommonPrivate *parent = 0);
    ~QSystemDeviceInfoPrivate();

    QString imei();
    QString imsi();
    QSystemDeviceInfo::SimStatus simStatus();
    bool isDeviceLocked();
    QSystemDeviceInfo::Profile currentProfile();
    void setConnection();

private:
#if !defined(QT_NO_DBUS)
    QHalInterface *halIface;
    QHalDeviceInterface *halIfaceDevice;
    void setupBluetooth();

private Q_SLOTS:
#endif
};


class QSystemScreenSaverPrivate : public QSystemScreenSaverLinuxCommonPrivate
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QSystemScreenSaverLinuxCommonPrivate *parent = 0);
    ~QSystemScreenSaverPrivate();

    bool screenSaverInhibited();
    bool setScreenSaverInhibit();
    bool isScreenLockEnabled();
    bool isScreenSaverActive();

private:
    QString screenPath;
    QString settingsPath;
    bool screenSaverSecure;

    uint currentPid;
    bool kdeIsRunning;
    bool gnomeIsRunning;
    void whichWMRunning();

};

QTM_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_LINUX_P_H*/

// End of file

