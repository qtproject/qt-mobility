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
{
    Q_UNUSED(parent);
    availableLanguages();
}

// 2 letter ISO 639-1
QString QSystemInfoPrivate::currentLanguage() const
{
   return QString(setlocale(LC_ALL,NULL)).left(2);
}

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
}

QString QSystemInfoPrivate::qtVersion() const
{
    return qVersion();
}

QString QSystemInfoPrivate::browserVersion() const
{
    return QString();
}

QString QSystemInfoPrivate::firmwareVersion() const
{
    return QString();
}


QString QSystemInfoPrivate::platformVersion() const
{
    //    QString wVer;
    DWORD qt_cever = 0;
    OSVERSIONINFOW osver;
    osver.dwOSVersionInfoSize = sizeof(osver);
    GetVersionEx(&osver);
    qt_cever = osver.dwMajorVersion * 100;
    qt_cever += osver.dwMinorVersion * 10;
    QString wVer = QString("%1.%2.%3")
                   .arg(osver.dwMajorVersion)
                   .arg(osver.dwMinorVersion)
                   .arg(osver.dwBuildNumber);
    wVer += " " + QString::fromWCharArray(osver.szCSDVersion);

    return wVer;
}

//2 letter ISO 3166-1
QString QSystemInfoPrivate::countryCode() const
{
   return QString(setlocale(LC_ALL,"")).mid(3,2);
}

qint32 QSystemInfoPrivate::networkSignalStrength()
{
    return -1;
}

qint32 QSystemInfoPrivate::cellId()
{
    return -1;
}

qint32 QSystemInfoPrivate::lac()
{
    return -1;
}

// Mobile Country Code
qint32 QSystemInfoPrivate::currentMCC()
{
    return -1;
}

// Mobile Network Code
qint32 QSystemInfoPrivate::currentMNC()
{
    return -1;
}

qint32 QSystemInfoPrivate::homeMCC()
{
    return -1;
}

qint32 QSystemInfoPrivate::homeMNC()
{
    return -1;
}

bool QSystemInfoPrivate::isLocationEnabled() const
{
    return false;
}

bool QSystemInfoPrivate::isWLANAccessible() const
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
    return true;
}

 //returns OR'd feature
qint32 QSystemInfoPrivate::supportedFeatures()
{
    return -1;
}

// display
qint32 QSystemInfoPrivate::displayBrightness()
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

qint32 QSystemInfoPrivate::screenSaverTimeout()
{
    return -1;
}

qint32 QSystemInfoPrivate::screenBlankingTimeout()
{
    return -1;
}

QSize QSystemInfoPrivate::screenSize()
{
    QSize sz = QApplication::desktop()->availableGeometry().size();
    if (sz.isValid())
        return sz;
    else
        return QSize();
}


qint32 QSystemInfoPrivate::colorDepth()
{
    return QPixmap::defaultDepth();
}

qint64 QSystemInfoPrivate::memory() const
{
    return -1;
}

qint64 QSystemInfoPrivate::diskSpace(const QString &drive)
{
Q_UNUSED(drive);
    return -1;
}

QStringList QSystemInfoPrivate::listOfVolumes()
{
    return QStringList();
}

// device
QString QSystemInfoPrivate::imei() const
{
    return QString();
}

QString QSystemInfoPrivate::imsi() const
{
    return QString();
}

QString QSystemInfoPrivate::manufacturer() const
{
    return QString();
}

QString QSystemInfoPrivate::model() const
{
    return QString();
}

//
//qint32 QSystemInfoPrivate::batteryLevel() const
//{
//    return -1;
//}

bool QSystemInfoPrivate::isSimAvailable()
{
    return false;
}

bool QSystemInfoPrivate::isDiskSpaceCritical(const QString &drive)
{
    Q_UNUSED(drive);
    return false;
}


bool QSystemInfoPrivate::isBatteryCharging()
{
    return false;
}

bool QSystemInfoPrivate::isCriticalMemory() const
{
return false;
}
