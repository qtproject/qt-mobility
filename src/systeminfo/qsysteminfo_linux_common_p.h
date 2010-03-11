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
class QSystemInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemInfoLinuxCommonPrivate();
    QString currentLanguage() const;
    QStringList availableLanguages() const {return QStringList();}

    QString version(QSystemInfo::Version,  const QString &/*parameter*/ = QString());
    QString currentCountryCode() const;
    virtual bool hasFeatureSupported(QSystemInfo::Feature feature);
    bool hasSysFeature(const QString &featureStr);

Q_SIGNALS:
    void currentLanguageChanged(const QString &);

private:
    QTimer *langTimer;
    QString langCached;

protected Q_SLOTS:
    void startLanguagePolling();

protected:
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

class QSystemNetworkInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemNetworkInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemNetworkInfoLinuxCommonPrivate();

    virtual QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);
    qint32 networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    int cellId() {return 0;}
    int locationAreaCode() {return 0;}

    QString currentMobileCountryCode() {return QString();}
    QString currentMobileNetworkCode() {return QString();}

    QString homeMobileCountryCode() {return QString();}
    QString homeMobileNetworkCode() {return QString();}

    virtual QString networkName(QSystemNetworkInfo::NetworkMode mode);
    virtual QString macAddress(QSystemNetworkInfo::NetworkMode mode);

    virtual QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);
   // virtual QSystemNetworkInfo::NetworkMode currentMode();

//public Q_SLOTS:
//    void getPrimaryMode();

Q_SIGNALS:
   void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
   void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int);
   void currentMobileCountryCodeChanged(const QString &);
   void currentMobileNetworkCodeChanged(const QString &);
   void networkNameChanged(QSystemNetworkInfo::NetworkMode, const QString &);
   void networkModeChanged(QSystemNetworkInfo::NetworkMode);

protected:
#if !defined(QT_NO_DBUS)
    int getBluetoothRssi();
    QString getBluetoothInfo(const QString &file);
    bool isDefaultInterface(const QString &device);
#endif
    QSystemNetworkInfo::NetworkStatus getBluetoothNetStatus();

};

class QSystemDisplayInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDisplayInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemDisplayInfoLinuxCommonPrivate();

    int displayBrightness(int screen);
    int colorDepth(int screen);
};

class QSystemStorageInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemStorageInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemStorageInfoLinuxCommonPrivate();

    qint64 availableDiskSpace(const QString &driveVolume);
    qint64 totalDiskSpace(const QString &driveVolume);
    QStringList logicalDrives();
    QSystemStorageInfo::DriveType typeForDrive(const QString &driveVolume);

private:
     QMap<QString, QString> mountEntriesMap;
     void mountEntries();

};

class QSystemDeviceInfoLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDeviceInfoLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemDeviceInfoLinuxCommonPrivate();

    QString imei() {return QString();}
    QString imsi() {return QString();}
    QString manufacturer();
    QString model();
    QString productName();

    QSystemDeviceInfo::InputMethodFlags inputMethodType();

    int  batteryLevel() const ;

    QSystemDeviceInfo::SimStatus simStatus() {return QSystemDeviceInfo::SimNotAvailable;}
    bool isDeviceLocked() {return false;}
    QSystemDeviceInfo::Profile currentProfile() {return QSystemDeviceInfo::UnknownProfile;}

    QSystemDeviceInfo::PowerState currentPowerState();
    void setConnection();

Q_SIGNALS:
    void batteryLevelChanged(int);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus );

    void powerStateChanged(QSystemDeviceInfo::PowerState);
    void currentProfileChanged(QSystemDeviceInfo::Profile);
    void bluetoothStateChanged(bool);
protected:
#if !defined(QT_NO_DBUS)
    QHalInterface *halIface;
    QHalDeviceInterface *halIfaceDevice;
    void setupBluetooth();

private Q_SLOTS:
    virtual void halChanged(int,QVariantList);
    void bluezPropertyChanged(const QString&, QDBusVariant);
#endif
};


class QSystemScreenSaverLinuxCommonPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemScreenSaverLinuxCommonPrivate(QObject *parent = 0);
    virtual ~QSystemScreenSaverLinuxCommonPrivate();

    bool screenSaverInhibited() {return false;}
    bool setScreenSaverInhibit() {return false;}
    bool isScreenLockEnabled() {return false;}
    bool isScreenSaverActive() {return false;}
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMINFO_LINUX_COMMON_P_H*/

// End of file

