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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qsysteminfo.h"
#include "qsysteminfo_p.h"
#include <qt_windows.h>

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

#include <locale.h>
//#include <windows.h>
#include <Wlanapi.h>
#include <Ntddvdeo.h>

#define _WCHAR_T_DEFINED
#define _TIME64_T_DEFINED


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
    return QString(setlocale(LC_ALL,NULL)).left(2);
}

// 2 letter ISO 639-1
QStringList QSystemInfoPrivate::availableLanguages() const
{
    QStringList lgList;
    QString rSubKey = "SOFTWARE\\Classes\\MIME\\Database\\Rfc1766";
    QSettings languageSetting("HKEY_LOCAL_MACHINE\\" + rSubKey, QSettings::NativeFormat);
    QStringList grp = languageSetting.childKeys();
    for (int i = 0; i < grp.count(); i++) {
        QString lg = languageSetting.value(grp.at(i)).toString().left(2);
        if(!lgList.contains(lg)) {
            lgList <<  lg;
            qWarning() << lg;
        }
    }
    return lgList;
    return QStringList() << currentLanguage();
}

// "major.minor.build" format.
/*QPair< int, double >*/ QString QSystemInfoPrivate::getVersion(QSystemInfo::Version type,  const QString &parameter)
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
    return QString(setlocale(LC_ALL,"")).mid(3,2);
}


bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    bool featureSupported = false;
    switch (feature) {
    case QSystemInfo::BluetoothFeature :
        {
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
        break;
    case QSystemInfo::WlanFeature :
        {
        }
        break;
    case QSystemInfo::SimFeature :
        break;
    case QSystemInfo::LocationFeature :
        break;
    case QSystemInfo::VideoOutFeature :
        {
        }
        break;
    case QSystemInfo::HapticsFeature:
        break;
    case QSystemInfo::UnknownFeature :
    default:
        featureSupported = true;
        break;
    };
    return featureSupported;
}

QString QSystemInfoPrivate::getDetailOfFeature(QSystemInfo::Feature feature)
{
    Q_UNUSED(feature);
    return "No other features";
}

//////// QSystemNetworkInfo
QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QObject *parent)
        : QObject(parent)
{
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
}

QSystemNetworkInfo::CellNetworkStatus QSystemNetworkInfoPrivate::getCellNetworkStatus()
{
    return QSystemNetworkInfo::NoNetworkAvailable;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
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

bool QSystemNetworkInfoPrivate::isLocationEnabled() const
{
    return false;
}

bool QSystemNetworkInfoPrivate::isWLANAccessible() const
{
    HANDLE phClientHandle = NULL;
    DWORD result;
    DWORD pdwNegotiatedVersion = 0;

    PWLAN_INTERFACE_INFO_LIST pIntfList = NULL;

    result = WlanOpenHandle(WLAN_API_VERSION, NULL, &pdwNegotiatedVersion, &phClientHandle);
    if( result != ERROR_SUCCESS ) {
        qWarning() << "Error opening Wlanapi" << result ;
        return false;
    }

    result = WlanEnumInterfaces(phClientHandle, NULL, &pIntfList);

    if( result != ERROR_SUCCESS ) {
        qWarning() << "Error in WlanEnumInterfaces" <<  result;
        return false;
    }
    return false;
}

QString QSystemNetworkInfoPrivate::operatorName()
{
    return "No Operator";
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
    qint32 brightness;
    HANDLE display = CreateFile(L"\\\\.\\LCD", FILE_ANY_ACCESS, 0, NULL, OPEN_EXISTING, 0, NULL);
    if( display != INVALID_HANDLE_VALUE )
    {
        DISPLAY_BRIGHTNESS bright;
        DWORD bytesReturned;
        if( DeviceIoControl(display, IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS, NULL, 0, &bright, sizeof(bright), &bytesReturned, NULL) ) {
            if( bytesReturned > 0 ) {
                brightness = bright.ucACBrightness;
                qWarning() << bright.ucACBrightness;
            }
        }
        CloseHandle(display);
    }

    return brightness;
}

int QSystemDisplayInfoPrivate::colorDepth(int screen)
{
    Q_UNUSED(screen);
}

bool QSystemDisplayInfoPrivate::isScreenLockOn()
{
    return false;
}

//////// QSystemMemoryInfo
QSystemMemoryInfoPrivate::QSystemMemoryInfoPrivate(QObject *parent)
        : QObject(parent)
{
}


QSystemMemoryInfoPrivate::~QSystemMemoryInfoPrivate()
{
}

bool QSystemMemoryInfoPrivate::hasRamMemoryLevel()
{
    return true;
}

qint64 QSystemMemoryInfoPrivate::freeMemoryLevel() const
{
}

qint64 QSystemMemoryInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
}

qint64 QSystemMemoryInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
}

QSystemMemoryInfo::VolumeType QSystemMemoryInfoPrivate::getVolumeType(const QString &driveVolume)
{
    return QSystemMemoryInfo::Internal;
}

QStringList QSystemMemoryInfoPrivate::listOfVolumes()
{
}

void QSystemMemoryInfoPrivate::getMountEntries()
{
}


bool QSystemMemoryInfoPrivate::isCriticalMemory() const
{
    return false;
}

//bool  QSystemMemoryInfoPrivate::isDiskSpaceCritical(const QString &driveVolume)
// {
//    Q_UNUSED(driveVolume);
//    return false;
// }

//////// QSystemDeviceInfo
QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QObject *parent)
        : QObject(parent)
{
    halIsAvailable = halAvailable();
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
}

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::getCurrentProfile()
{
    return QSystemDeviceInfo::UnknownProfile;
}

QSystemDeviceInfo::InputMethods QSystemDeviceInfoPrivate::getInputMethodType()
{
    return methods;
}

QString QSystemDeviceInfoPrivate::imei()
{
//    if(this->getSimStatus() == QSystemDeviceInfo::SimNotAvailable)
        return "Sim Not Available";
}

QString QSystemDeviceInfoPrivate::imsi()
{
//    if(getSimStatus() == QSystemDeviceInfo::SimNotAvailable)
        return "Sim Not Available";
}

QString QSystemDeviceInfoPrivate::manufacturer()
{
    return QString();
}

QString QSystemDeviceInfoPrivate::model()
{
    return QString();
}

QString QSystemDeviceInfoPrivate::productName()
{
    return QString();
}

bool QSystemDeviceInfoPrivate::isBatteryCharging()
{
    return isCharging;
}

QSystemDeviceInfo::BatteryLevel QSystemDeviceInfoPrivate::batteryLevel() const
{

    return QSystemDeviceInfo::NoBatteryLevel;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::getSimStatus()
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
        : QSystemScreenSaver(parent)
{

}

bool QSystemScreenSaverPrivate::setScreenSaverEnabled(QSystemScreenSaver::ScreenSaverState state)
{
    Q_UNUSED(state);

    return false;
}

bool QSystemScreenSaverPrivate::setScreenBlankingEnabled(QSystemScreenSaver::ScreenSaverState state)
{
    Q_UNUSED(state);
    return false;
}

QSystemScreenSaver::ScreenSaverState QSystemScreenSaverPrivate::screenSaverEnabled()
{
    return QSystemScreenSaver::UnknownScreenSaverState;
}

QSystemScreenSaver::ScreenSaverState QSystemScreenSaverPrivate::screenBlankingEnabled()
{
    return QSystemScreenSaver::UnknownScreenSaverState;
}


QT_END_NAMESPACE
