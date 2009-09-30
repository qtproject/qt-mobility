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
#include "qsysteminfo_mac_p.h"

#include <QStringList>
#include <QSize>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QLibraryInfo>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QSettings>
#include <QSysInfo>
#include <QNetworkInterface>
#include <QList>
#include <QSettings>
#include <QDir>
#include <QNetworkInterface>
#include <QString>

#include <locale.h>

#include <IOBluetoothHostController.h>
#include <SystemConfiguration/SystemConfiguration.h>
#include <CoreFoundation/CFArray.h>
////////
static QString stringFromCFString(CFStringRef value) {
    QString retVal;
    CFIndex maxLength = 2 * CFStringGetLength(value) + 1/*zero term*/; // max UTF8
    char *cstring = new char[maxLength];
    if (CFStringGetCString(CFStringRef(value), cstring, maxLength, kCFStringEncodingUTF8)) {
        retVal = QString::fromUtf8(cstring);
    }
    delete cstring;
    return retVal;
}

QSystemInfoPrivate::QSystemInfoPrivate(QObject *parent)
 : QObject(parent)
{
}

QSystemInfoPrivate::~QSystemInfoPrivate()
{

}

// 2 letter ISO 639-1
QString QSystemInfoPrivate::currentLanguage() const
{
 QString lang = QLocale::system().name().left(2);
    if(lang.isEmpty() || lang == "C") {
        lang = "en";
    }
    return lang;
}

// 2 letter ISO 639-1
QStringList QSystemInfoPrivate::availableLanguages() const
{

    return QStringList() << currentLanguage();
}

// "major.minor.build" format.
QString QSystemInfoPrivate::version(QSystemInfo::Version type,  const QString &parameter)
{
    Q_UNUSED(parameter);
    QString errorStr = "Not Available";
    bool useDate = false;
    if(parameter == "versionDate") {
        useDate = true;
    }
    switch(type) {
    case QSystemInfo::Os :
        {

        }
        break;
    case QSystemInfo::QtCore :
       return  qVersion();
       break;
    case QSystemInfo::WrtCore :
        {
        }
       break;
    case QSystemInfo::Webkit :
        {
        }
       break;
    case QSystemInfo::ServiceFramework :
        {
        }
       break;
    case QSystemInfo::WrtExtensions :
        {
        }
       break;
    case QSystemInfo::ServiceProvider :
        {
        }
       break;
    case QSystemInfo::NetscapePlugin :
        {
        }
       break;
    case QSystemInfo::WebApp :
        {
        }
       break;
   case QSystemInfo::Firmware :
       {
       }
       break;
    };
  return errorStr;
}


//2 letter ISO 3166-1
QString QSystemInfoPrivate::currentCountryCode() const
{
    return QLocale::system().name().mid(3,2);
}


bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    bool featureSupported = false;
    switch (feature) {
    case QSystemInfo::BluetoothFeature :
        {
            //IOBluetoothHostController
            //addressAsString
            IOBluetoothHostController* controller = [IOBluetoothHostController defaultController];
            if (controller != NULL) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::CameraFeature :
        {

        }
        break;
    case QSystemInfo::FmradioFeature :
        {

        }
        break;
    case QSystemInfo::IrFeature :
        {

        }
        break;
    case QSystemInfo::LedFeature :
        {

        }
        break;
    case QSystemInfo::MemcardFeature :
        {

        }
        break;
    case QSystemInfo::UsbFeature :
        {

        }
        break;
    case QSystemInfo::VibFeature :
        {

        }
        break;
    case QSystemInfo::WlanFeature :
        {

        }
        break;
    case QSystemInfo::SimFeature :
        {

        }
        break;
    case QSystemInfo::LocationFeature :
        {

        }
        break;
    case QSystemInfo::VideoOutFeature :
        {

        }
        break;
    case QSystemInfo::HapticsFeature:
        break;
    default:
        featureSupported = false;
        break;
    };
    return featureSupported;
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
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        break;
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
    case QSystemNetworkInfo::WlanMode:
        {
//SCNetworkConnectionStatus SCNetworkConnectionGetStatus (
  //  SCNetworkConnectionRef connection );

        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {

        }
        break;
        case QSystemNetworkInfo::BluetoothMode:
        {

        }
        break;
    case QSystemNetworkInfo::WimaxMode:
        break;
        default:
        break;
    };
    return QSystemNetworkInfo::NoNetworkAvailable;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
        case QSystemNetworkInfo::GsmMode:
        break;
        case QSystemNetworkInfo::CdmaMode:
        break;
        case QSystemNetworkInfo::WcdmaMode:
        break;
    case QSystemNetworkInfo::WlanMode:
        {

        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {


        }
        break;
    case QSystemNetworkInfo::BluetoothMode:
        break;
        case QSystemNetworkInfo::WimaxMode:
        break;
        default:
        break;
    };
    return -1;
}

int QSystemNetworkInfoPrivate::cellId()
{
    return -1;
}

int QSystemNetworkInfoPrivate::locationAreaCode()
{
    return -1;
}

// Mobile Country Code
QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
    return "No Network";
}

// Mobile Network Code
QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
    return "No Network";
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
    return "No Network";
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
    return "No Network";
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    Q_UNUSED(mode);
    return "No Operator";
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    QString mac;
    if(mode == QSystemNetworkInfo::BluetoothMode) {
        NSString *addy;
        IOBluetoothHostController* controller = [IOBluetoothHostController defaultController];
        if (controller != NULL) {
            addy = [controller addressAsString];
            mac = [addy UTF8String];
            mac.replace("-",":");
        }
    } else {
        mac = interfaceForMode(mode).hardwareAddress();
    }
    return mac;
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
    //    qWarning() << __FUNCTION__ << mode;
    QNetworkInterface netInterface;

    CFArrayRef interfaceArray = SCNetworkInterfaceCopyAll(); //10.4
    CFStringRef iName;
    CFStringRef type;

    for ( long i = 0; i < CFArrayGetCount(interfaceArray); i++) {
        SCNetworkInterfaceRef thisInterface =  (SCNetworkInterfaceRef ) CFArrayGetValueAtIndex(interfaceArray, i);
        type = SCNetworkInterfaceGetInterfaceType(thisInterface);
        iName = SCNetworkInterfaceGetBSDName(thisInterface);
        //        qWarning() << stringFromCFString(type) <<stringFromCFString( iName);
        if (type != NULL) {
            if (CFEqual(type, kSCNetworkInterfaceTypeBluetooth) && mode == QSystemNetworkInfo::BluetoothMode) {
                netInterface = QNetworkInterface::interfaceFromName(stringFromCFString(iName));
                break;
            } else if (CFEqual(type, kSCNetworkInterfaceTypeEthernet) && mode == QSystemNetworkInfo::EthernetMode) {
                netInterface = QNetworkInterface::interfaceFromName(stringFromCFString(iName));
                break;
            } else if (CFEqual(type, kSCNetworkInterfaceTypeIEEE80211) && mode == QSystemNetworkInfo::WlanMode) {
                netInterface = QNetworkInterface::interfaceFromName(stringFromCFString(iName));
                break;
            }
        }
    }
    CFRelease(interfaceArray);
    return netInterface;
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
    Q_UNUSED(screen);
    return -1;
}

int QSystemDisplayInfoPrivate::colorDepth(int screen)
{
    Q_UNUSED(screen);

   int bpp=0;
    return bpp;
}

//////// QSystemStorageInfo
QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QObject *parent)
        : QObject(parent)
{
}


QSystemStorageInfoPrivate::~QSystemStorageInfoPrivate()
{
}

qint64 QSystemStorageInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
        Q_UNUSED(driveVolume);

    qint64 totalFreeBytes=0;
    return  totalFreeBytes;
}

qint64 QSystemStorageInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
        Q_UNUSED(driveVolume);

    qint64 totalBytes=0;
    return totalBytes;
}

QSystemStorageInfo::DriveType QSystemStorageInfoPrivate::typeForDrive(const QString &driveVolume)
{
        Q_UNUSED(driveVolume);

    return QSystemStorageInfo::NoDrive;
}

QStringList QSystemStorageInfoPrivate::logicalDrives()
{
    QStringList drivesList;
    return drivesList;
}

//////// QSystemDeviceInfo
QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
        : QObject(parent)
{
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
}

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
    return QSystemDeviceInfo::UnknownProfile;
}

QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfoPrivate::inputMethodType()
{
    QSystemDeviceInfo::InputMethodFlags methods;
    return methods;
}


QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
{
    return QSystemDeviceInfo::UnknownPower;
}

QString QSystemDeviceInfoPrivate::imei()
{
        return "Sim Not Available";
}

QString QSystemDeviceInfoPrivate::imsi()
{
        return "Sim Not Available";
}

QString QSystemDeviceInfoPrivate::manufacturer()
{
    return QString();
}

QString QSystemDeviceInfoPrivate::model()
{
    return  QString();
}

QString QSystemDeviceInfoPrivate::productName()
{
    return  QString();
}

int QSystemDeviceInfoPrivate::batteryLevel() const
{
    return 0;
}

QSystemDeviceInfo::BatteryStatus QSystemDeviceInfoPrivate::batteryStatus()
{
    return QSystemDeviceInfo::NoBatteryLevel;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
    return QSystemDeviceInfo::SimNotAvailable;
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
    return false;
}

//////////////
///////
QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
        : QObject(parent)
{
}

QSystemScreenSaverPrivate::~QSystemScreenSaverPrivate()
{
}

bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
{
    return false;
}


bool QSystemScreenSaverPrivate::screenSaverInhibited()
{
    return false;
}

bool QSystemScreenSaverPrivate::isScreenLockOn()
{
    return false;
}

QT_END_NAMESPACE
