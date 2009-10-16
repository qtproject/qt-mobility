/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qsysteminfo.h"
#include "qsysteminfo_s60_p.h"

#include <QStringList>

#include <SysUtil.h>

//////// QSystemInfo
QSystemInfoPrivate::QSystemInfoPrivate(QObject *parent)
 : QObject(parent)
{
}

QSystemInfoPrivate::~QSystemInfoPrivate()
{
}

QString QSystemInfoPrivate::currentLanguage() const
{
    return QString();   //TODO
}

QStringList QSystemInfoPrivate::availableLanguages() const
{
    return QStringList();   //TODO
}

QString QSystemInfoPrivate::version(QSystemInfo::Version type,  const QString &parameter)
{
    return QString();   //TODO
}

QString QSystemInfoPrivate::currentCountryCode() const
{
    return QString();   //TODO
}


bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    return false;   //TODO
}

//////// QSystemNetworkInfo
QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QObject *parent)
        : QObject(parent)
{
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    return QSystemNetworkInfo::UndefinedStatus;     //TODO
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    return -1;   //TODO
}

int QSystemNetworkInfoPrivate::cellId()
{
    return -1;  //TODO
}

int QSystemNetworkInfoPrivate::locationAreaCode()
{
    return -1;  //TODO
}

// Mobile Country Code
QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
    return QString();   //TODO
}

// Mobile Network Code
QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
    return QString();   //TODO
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
    return QString();   //TODO
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
    return QString();   //TODO
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    return QString();   //TODO
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    return QString();   //TODO
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
    return QNetworkInterface();     //TODO
}

//////// QSystemDisplayInfo
QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QObject *parent)
        : QObject(parent)
{
}

QSystemDisplayInfoPrivate::~QSystemDisplayInfoPrivate()
{
}

int QSystemDisplayInfoPrivate::displayBrightness(int screen)
{
    return -1;  //TODO
}

int QSystemDisplayInfoPrivate::colorDepth(int screen)
{
    return -1;  //TODO
}

//////// QSystemMemoryInfo
QSystemMemoryInfoPrivate::QSystemMemoryInfoPrivate(QObject *parent)
        : QObject(parent)
{
}

QSystemMemoryInfoPrivate::~QSystemMemoryInfoPrivate()
{
}

qint64 QSystemMemoryInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
    return -1;  //TODO
}

qint64 QSystemMemoryInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    return -1;  //TODO
}

QStringList QSystemMemoryInfoPrivate::listOfVolumes()
{
    return QStringList();   //TODO
}

QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QObject *parent)
        : QObject(parent)
{
}

//////// QSystemDeviceInfo
QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
        : QObject(parent)
{
    DeviceInfo::instance()->batteryInfo()->addObserver(this);
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
    DeviceInfo::instance()->batteryInfo()->removeObserver(this);
}

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
    return QSystemDeviceInfo::UnknownProfile;   //TODO
}

QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfoPrivate::inputMethodType()
{
    QSystemDeviceInfo::InputMethodFlags methods;    //TODO
    return methods;
}

QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
{
    CTelephony::TBatteryStatus batteryStatus = DeviceInfo::instance()->batteryInfo()->batteryStatus();

    switch (batteryStatus) {
        case CTelephony::EPoweredByBattery:
            return QSystemDeviceInfo::BatteryPower;

        case CTelephony::EBatteryConnectedButExternallyPowered:
        {
            if (DeviceInfo::instance()->batteryInfo()->batteryLevel() < 100) {  //TODO: Use real indicator
                return QSystemDeviceInfo::WallPowerChargingBattery;
            } else {
                return QSystemDeviceInfo::WallPower;
            }
            return QSystemDeviceInfo::WallPower;
        }
        case CTelephony::ENoBatteryConnected:
            return QSystemDeviceInfo::WallPower;

        case CTelephony::EPowerFault:
        case CTelephony::EPowerStatusUnknown:
        default:
            return QSystemDeviceInfo::UnknownPower;
    }
}

QString QSystemDeviceInfoPrivate::imei()
{
    return DeviceInfo::instance()->phoneInfo()->imei();
}

QString QSystemDeviceInfoPrivate::imsi()
{
    return DeviceInfo::instance()->subscriberInfo()->imsi();
}

QString QSystemDeviceInfoPrivate::manufacturer()
{
    return DeviceInfo::instance()->phoneInfo()->manufacturer();
}

QString QSystemDeviceInfoPrivate::model()
{
    return DeviceInfo::instance()->phoneInfo()->model();
}

QString QSystemDeviceInfoPrivate::productName()
{
    QString productname;
    TBuf<KSysUtilVersionTextLength> versionBuf;
    if (SysUtil::GetSWVersion(versionBuf) == KErrNone) {
		productname = QString::fromUtf16(versionBuf.Ptr(), versionBuf.Length());
    }
    return productname.split("\n").at(2);
}

int QSystemDeviceInfoPrivate::batteryLevel() const
{
    return DeviceInfo::instance()->batteryInfo()->batteryLevel();
}

QSystemDeviceInfo::BatteryStatus QSystemDeviceInfoPrivate::batteryStatus()
{
    //TODO: To be moved in QSystemDeviceInfo?
    int batteryLevel = DeviceInfo::instance()->batteryInfo()->batteryLevel();
    if(batteryLevel < 4) {
        return QSystemDeviceInfo::BatteryCritical;
    }   else if (batteryLevel < 11) {
        return QSystemDeviceInfo::BatteryVeryLow;
    }  else if (batteryLevel < 41) {
        return QSystemDeviceInfo::BatteryLow;
    }   else if (batteryLevel > 40) {
        return QSystemDeviceInfo::BatteryNormal;
    }

    return QSystemDeviceInfo::NoBatteryLevel;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
    return QSystemDeviceInfo::SimNotAvailable;  //TODO
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
    return false;   //TODO
}

void QSystemDeviceInfoPrivate::batteryStatusChanged()
{
    emit powerStateChanged(currentPowerState());
}

void QSystemDeviceInfoPrivate::batteryLevelChanged()
{
    emit batteryLevelChanged(batteryLevel());
}

DeviceInfo *DeviceInfo::m_instance = NULL;

//////// QSystemScreenSaver
QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
        : QObject(parent)
{
}

QT_END_NAMESPACE
