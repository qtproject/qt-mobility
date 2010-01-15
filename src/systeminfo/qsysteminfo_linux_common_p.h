/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef QSYSTEMINFO_LINUX_COMMON_P_H
#define QSYSTEMINFO_LINUX_COMMON_P_H


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

#include "qsysteminfo.h"
#include <qmobilityglobal.h>
#if !defined(QT_NO_DBUS)
#include <qhalservice_linux_p.h>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE
class QStringList;
class QTimer;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QSystemNetworkInfo;
class QSystemInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemInfoPrivate(QObject *parent = 0);
    virtual ~QSystemInfoPrivate();
// general
    virtual QString currentLanguage() const = 0; // 2 letter ISO 639-1
    virtual QStringList availableLanguages() const = 0;	 // 2 letter ISO 639-1

    virtual QString version(QSystemInfo::Version,  const QString &parameter = QString()) = 0;

    virtual QString currentCountryCode() const = 0; //2 letter ISO 3166-1
//features
    virtual bool hasFeatureSupported(QSystemInfo::Feature feature) = 0;
Q_SIGNALS:
    void currentLanguageChanged(const QString &);
};

class QNetworkManagerInterface;
class QNetworkManagerInterfaceDeviceWired;
class QNetworkManagerInterfaceDeviceWireless;
class QNetworkManagerInterfaceAccessPoint;

class QSystemNetworkInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemNetworkInfoPrivate(QObject *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    virtual QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode) = 0;
    virtual qint32 networkSignalStrength(QSystemNetworkInfo::NetworkMode mode) = 0;
    virtual int cellId() = 0;
    virtual int locationAreaCode() = 0;

    virtual QString currentMobileCountryCode() = 0; // Mobile Country Code
    virtual QString currentMobileNetworkCode() = 0; // Mobile Network Code

    virtual QString homeMobileCountryCode() = 0;
    virtual QString homeMobileNetworkCode() = 0;

    virtual QString networkName(QSystemNetworkInfo::NetworkMode mode) = 0;
    virtual QString macAddress(QSystemNetworkInfo::NetworkMode mode) = 0;

    virtual QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode) = 0;

Q_SIGNALS:
   void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
   void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int);
   void currentMobileCountryCodeChanged(const QString &);
   void currentMobileNetworkCodeChanged(const QString &);
   void networkNameChanged(QSystemNetworkInfo::NetworkMode, const QString &);
   void networkModeChanged(QSystemNetworkInfo::NetworkMode);
};

class QSystemDisplayInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDisplayInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();


// display
    virtual int displayBrightness(int screen) = 0;
    virtual int colorDepth(int screen) = 0;
};

class QSystemStorageInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemStorageInfoPrivate(QObject *parent = 0);
    virtual ~QSystemStorageInfoPrivate();

    // memory
    virtual qint64 availableDiskSpace(const QString &driveVolume) = 0;
    virtual qint64 totalDiskSpace(const QString &driveVolume) = 0;
    virtual QStringList logicalDrives() = 0;
    virtual QSystemStorageInfo::DriveType typeForDrive(const QString &driveVolume) = 0; //returns enum
};

class QSystemDeviceInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDeviceInfoPrivate(QObject *parent = 0);
    ~QSystemDeviceInfoPrivate();

// device

    virtual QString imei() = 0;
    virtual QString imsi() = 0;
    virtual QString manufacturer() = 0;
    virtual QString model() = 0;
    virtual QString productName() = 0;

    virtual QSystemDeviceInfo::InputMethodFlags inputMethodType() = 0;

    virtual int  batteryLevel() const = 0;

    virtual QSystemDeviceInfo::SimStatus simStatus() = 0;
    virtual bool isDeviceLocked() = 0;
    virtual QSystemDeviceInfo::Profile currentProfile() = 0;

    virtual QSystemDeviceInfo::PowerState currentPowerState() = 0;
    virtual void setConnection() = 0;

Q_SIGNALS:
    void batteryLevelChanged(int);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus );

    void powerStateChanged(QSystemDeviceInfo::PowerState);
    void currentProfileChanged(QSystemDeviceInfo::Profile);
    void bluetoothStateChanged(bool);
};


class QSystemScreenSaverPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QObject *parent = 0);
    ~QSystemScreenSaverPrivate();

    virtual bool screenSaverInhibited() = 0;
    virtual bool setScreenSaverInhibit() = 0;
    virtual bool isScreenLockEnabled() = 0;
    virtual bool isScreenSaverActive() = 0;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMINFO_LINUX_COMMON_P_H*/

// End of file

