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
#include <f32file.h>


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
    DeviceInfo::instance()->cellSignalStrenghtInfo()->addObserver(this);
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
    DeviceInfo::instance()->cellSignalStrenghtInfo()->removeObserver(this);
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    //TODO: CDMA, WCDMA and WLAN modes
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        {
            CTelephony::TRegistrationStatus networkStatus = DeviceInfo::instance()
                ->cellNetworkRegistrationInfo()->cellNetworkStatus();

            switch(networkStatus) {
                case CTelephony::ERegistrationUnknown:
                    return QSystemNetworkInfo::UndefinedStatus;

                case CTelephony::ENotRegisteredNoService:
                    return QSystemNetworkInfo::NoNetworkAvailable;

                case CTelephony::ENotRegisteredEmergencyOnly:
                    return QSystemNetworkInfo::EmergencyOnly;

                case CTelephony::ENotRegisteredSearching:
                    return QSystemNetworkInfo::Searching;

                case CTelephony::ERegisteredBusy:
                    return QSystemNetworkInfo::Busy;
                /*
                case CTelephony::ERegisteredOnHomeNetwork: //How this case should be handled? There are no connected state in CTelephony. Should this be same as HomeNetwork?
                    return QSystemNetworkInfo::Connected;
                */
                case CTelephony::ERegisteredOnHomeNetwork:
                    return QSystemNetworkInfo::HomeNetwork;

                case CTelephony::ERegistrationDenied:
                    return QSystemNetworkInfo::Denied;

                case CTelephony::ERegisteredRoaming:
                    return QSystemNetworkInfo::Roaming;

            };
        }
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
        case QSystemNetworkInfo::WlanMode:
        break;
        case QSystemNetworkInfo::EthernetMode:
        break;
        case QSystemNetworkInfo::BluetoothMode:
        break;
        case QSystemNetworkInfo::WimaxMode:
        break;
    };
    return QSystemNetworkInfo::NoNetworkAvailable;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    //TODO: CDMA, WCDMA and WLAN modes
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
            return DeviceInfo::instance()->cellSignalStrenghtInfo()->cellNetworkSignalStrength();

        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
        case QSystemNetworkInfo::WlanMode:
        break;
        case QSystemNetworkInfo::EthernetMode:
        break;
        case QSystemNetworkInfo::BluetoothMode:
        break;
        case QSystemNetworkInfo::WimaxMode:
        break;
    };
    return -1;  //TODO
}

int QSystemNetworkInfoPrivate::cellId()
{
	return DeviceInfo::instance()->cellNetworkInfo()->cellId();
}

int QSystemNetworkInfoPrivate::locationAreaCode()
{
	return DeviceInfo::instance()->cellNetworkInfo()->locationAreaCode();
}

// Mobile Country Code
QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
	return DeviceInfo::instance()->cellNetworkInfo()->countryCode();
}

// Mobile Network Code
QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
	return DeviceInfo::instance()->cellNetworkInfo()->networkCode();
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
    QString imsi = DeviceInfo::instance()->subscriberInfo()->imsi();
    if (imsi.length() >= 3) {
        return imsi.left(3);
    }
	return QString();
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
    return QString();   //TODO
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    //TODO: CDMA, WCDMA and WLAN modes
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
            return DeviceInfo::instance()->cellNetworkInfo()->networkName();

        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
        case QSystemNetworkInfo::WlanMode:
        break;
        case QSystemNetworkInfo::EthernetMode:
        break;
        case QSystemNetworkInfo::BluetoothMode:
        break;
        case QSystemNetworkInfo::WimaxMode:
        break;
    };
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        break;
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
        case QSystemNetworkInfo::WlanMode:
        break;
        case QSystemNetworkInfo::EthernetMode:
        break;
        case QSystemNetworkInfo::BluetoothMode:
        break;
        case QSystemNetworkInfo::WimaxMode:
        break;
    };
    return QString();   //TODO
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
    return QNetworkInterface();     //TODO
}

void QSystemNetworkInfoPrivate::countryCodeChanged()
{
    emit currentMobileCountryCodeChanged(DeviceInfo::instance()->cellNetworkInfo()->countryCode());
}

void QSystemNetworkInfoPrivate::networkCodeChanged()
{
    emit currentMobileNetworkCodeChanged(DeviceInfo::instance()->cellNetworkInfo()->networkCode());
}

void QSystemNetworkInfoPrivate::networkNameChanged()
{
    emit networkNameChanged(QSystemNetworkInfo::GsmMode, DeviceInfo::instance()->cellNetworkInfo()->networkName());
}

void QSystemNetworkInfoPrivate::cellNetworkSignalStrengthChanged()
{
    emit networkSignalStrengthChanged(QSystemNetworkInfo::GsmMode,
        DeviceInfo::instance()->cellSignalStrenghtInfo()->cellNetworkSignalStrength());
}

void QSystemNetworkInfoPrivate::cellNetworkStatusChanged()
{
    //TODO
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

//////// QSystemStorageInfo
QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QObject *parent)
    : QObject(parent)
{
}

QSystemStorageInfoPrivate::~QSystemStorageInfoPrivate()
{
}

qlonglong QSystemStorageInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    return -1;
}

qlonglong QSystemStorageInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
    return -1;
}

QStringList QSystemStorageInfoPrivate::logicalDrives()
{
    QStringList logicalDrives;
    RFs fsSession;
    TRAPD(err,
        User::LeaveIfError(fsSession.Connect());
        CleanupClosePushL(fsSession);
        TDriveList drivelist;
        User::LeaveIfError(fsSession.DriveList(drivelist));
        for (int i = 0; i < KMaxDrives; ++i) {
            if (drivelist[i] != 0) {
                TChar driveChar;
                User::LeaveIfError(RFs::DriveToChar(i, driveChar));
                logicalDrives << QChar(driveChar);
            }
        }
        CleanupStack::PopAndDestroy(&fsSession);
    )
    if (err != KErrNone) {
        return QStringList();
    }
    return logicalDrives;
}

QSystemStorageInfo::DriveType QSystemStorageInfoPrivate::typeForDrive(const QString &driveVolume)
{
    return QSystemStorageInfo::NoDrive;
};


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
            if (DeviceInfo::instance()->batteryInfo()->batteryLevel() < 100) { //TODO: Use real indicator, EPSHWRMChargingStatus::EChargingStatusNotCharging?
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

void QSystemDeviceInfoPrivate::batteryLevelChanged()
{
    emit batteryLevelChanged(batteryLevel());
    
    int batteryLevel = DeviceInfo::instance()->batteryInfo()->batteryLevel();
    QSystemDeviceInfo::BatteryStatus status(batteryStatus());
    
    if(batteryLevel < 4 && status != QSystemDeviceInfo::BatteryCritical) {
        emit batteryStatusChanged(QSystemDeviceInfo::BatteryCritical);
    } else if (batteryLevel < 11 && status != QSystemDeviceInfo::BatteryVeryLow) {
        emit batteryStatusChanged(QSystemDeviceInfo::BatteryVeryLow);
    } else if (batteryLevel < 41 && status != QSystemDeviceInfo::BatteryLow) {
        emit batteryStatusChanged(QSystemDeviceInfo::BatteryLow);
    } else if (batteryLevel > 40 && status != QSystemDeviceInfo::BatteryNormal) {
        emit batteryStatusChanged(QSystemDeviceInfo::BatteryNormal);
    } else {
        emit batteryStatusChanged(QSystemDeviceInfo::NoBatteryLevel);
    }
}

void QSystemDeviceInfoPrivate::powerStateChanged()
{
    emit powerStateChanged(currentPowerState());
}

DeviceInfo *DeviceInfo::m_instance = NULL;

//////// QSystemScreenSaver
QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
    : QObject(parent)
{
}

QT_END_NAMESPACE
