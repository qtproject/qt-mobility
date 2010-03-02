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
#include <qsysteminfo.h>
#include <qsysteminfo_maemo_p.h>

#include <QStringList>
#include <QSize>
#include <QFile>
#include <QTextStream>
#include <QLocale>
#include <QLibraryInfo>
//#include <QtGui>
#include <QDesktopWidget>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QTimer>
#include <QMapIterator>

#if !defined(QT_NO_DBUS)
#include "gconfitem.h" // Temporarily here.
#endif

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>

#endif

#include <QDBusInterface>

QTM_BEGIN_NAMESPACE

QSystemInfoPrivate::QSystemInfoPrivate(QSystemInfoLinuxCommonPrivate *parent)
 : QSystemInfoLinuxCommonPrivate(parent)
{
}

QSystemInfoPrivate::~QSystemInfoPrivate()
{
}

QStringList QSystemInfoPrivate::availableLanguages() const
{
    QStringList languages;

    GConfItem languagesItem("/apps/osso/inputmethod/available_languages");
    QStringList locales = languagesItem.value().toStringList();

    foreach(QString locale, locales) {
        languages << locale.mid(0,2);
    }
    languages << currentLanguage();
    languages.removeDuplicates();

    return languages;
}

QString QSystemInfoPrivate::version(QSystemInfo::Version type,
                                    const QString &parameter)
{
    QString errorStr = "Not Available";

    switch(type) {
        case QSystemInfo::Firmware :
        {
            QDBusInterface connectionInterface("com.nokia.SystemInfo",
                                               "/com/nokia/SystemInfo",
                                               "com.nokia.SystemInfo",
                                               QDBusConnection::systemBus());
            if(!connectionInterface.isValid()) {
                qWarning() << "interfacenot valid";
            } else {
                QDBusReply< QByteArray > reply =
                    connectionInterface.call("GetConfigValue",
                                             "/device/sw-release-ver");
                if(reply.isValid())
                    return reply.value();
            }
            break;
        }
        default:
            return QSystemInfoLinuxCommonPrivate::version(type, parameter);
            break;
    };
    return errorStr;
}

bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    bool featureSupported = false;
    switch (feature) {
    case QSystemInfo::SimFeature :
        {
            GConfItem locationValues("/system/nokia/location");
            QStringList locationKeys = locationValues.listEntries();

            foreach (QString str, locationKeys) {
                if (str.contains("sim_imsi"))
                    featureSupported = true;
                break;
            }
        }
        break;
    case QSystemInfo::LocationFeature :
        {
            GConfItem locationValues("/system/nokia/location");
            QStringList locationKeys = locationValues.listEntries();
            if(locationKeys.count()) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::VideoOutFeature :
        {
            QString sysPath = "/sys/class/video4linux/";
            QDir sysDir(sysPath);
            QStringList filters;
            filters << "*";
            QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
            if(sysList.contains("video0")) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::HapticsFeature:
        {
           // if(halIsAvailable) {
                QHalInterface iface;
                QStringList touchSupport =
                        iface.findDeviceByCapability("input.touchpad");
                if(touchSupport.count()) {
                    featureSupported = true;
                } else {
                    featureSupported = false;
                }
            }
      //  }
        break;
    default:
        featureSupported = QSystemInfoLinuxCommonPrivate::hasFeatureSupported(feature);
        break;
    };
    return featureSupported;
}

QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QSystemNetworkInfoLinuxCommonPrivate *parent)
        : QSystemNetworkInfoLinuxCommonPrivate(parent)
{
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
}


QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    qWarning() << __PRETTY_FUNCTION__ << mode;

    switch(mode) {
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::WcdmaMode:
        {
            qWarning() << __FUNCTION__<< "GSM" << mode;
//#if 0
//#if !defined(QT_NO_DBUS)
//            QDBusInterface connectionInterface("com.nokia.phone.net",
//                                               "/com/nokia/phone/net",
//                                               "com.nokia.SystemInfo",
//                                                QDBusConnection::systemBus());
//            if(!connectionInterface.isValid()) {
//                qWarning() << "interfacenot valid";
//            }
//            QDBusReply< QByteArray > reply = connectionInterface.call("GetConfigValue", "/device/sw-release-ver");
//            return reply.value();
//#endif
//#endif
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
    case QSystemNetworkInfo::WlanMode:
    case QSystemNetworkInfo::BluetoothMode:
        {
            return QSystemNetworkInfoLinuxCommonPrivate::networkStatus(mode);
        }
        break;
    };
    return QSystemNetworkInfo::UndefinedStatus;
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::WcdmaMode:
        break;
    case QSystemNetworkInfo::EthernetMode:
    case QSystemNetworkInfo::WlanMode:
    case QSystemNetworkInfo::BluetoothMode:
        return QSystemNetworkInfoLinuxCommonPrivate::networkSignalStrength(mode);
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

QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
    return QString();
}

QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
    return QString();
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
    return QString();
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
    return QString();
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    QString netname = "";

    switch(mode) {

    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::WcdmaMode:
    case QSystemNetworkInfo::WimaxMode:
        break;
        break;
    default:
        return QSystemNetworkInfoLinuxCommonPrivate::networkName(mode);
        break;
    };
    return netname;
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {

    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::WcdmaMode:
    case QSystemNetworkInfo::WimaxMode:
        break;
    default:
        return QSystemNetworkInfoLinuxCommonPrivate::networkName(mode);
        break;
    };
    return QString();
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
#if !defined(QT_NO_DBUS)
    switch(mode) {
    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::WcdmaMode:
    case QSystemNetworkInfo::WimaxMode:
        break;
    default:
        return QSystemNetworkInfoLinuxCommonPrivate::interfaceForMode(mode);
        break;
    };
#endif
    return QNetworkInterface();
}

QSystemNetworkInfo::NetworkMode QSystemDisplayInfoPrivate::currentMode()
{
    return QSystemNetworkInfo::UnknownMode;
}

QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QSystemDisplayInfoLinuxCommonPrivate *parent)
        : QSystemDisplayInfoLinuxCommonPrivate(parent)
{
}

QSystemDisplayInfoPrivate::~QSystemDisplayInfoPrivate()
{
}

int QSystemDisplayInfoPrivate::displayBrightness(int screen)
{
    Q_UNUSED(screen);
    GConfItem currentBrightness("/system/osso/dsm/display/display_brightness");
    GConfItem maxBrightness("/system/osso/dsm/display/max_display_brightness_levels");
    if(maxBrightness.value().toInt()) {
        float retVal = 100 * (currentBrightness.value().toFloat() /
                              maxBrightness.value().toFloat());
        return retVal;
    }

    return -1;
}

QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QSystemStorageInfoLinuxCommonPrivate *parent)
        : QSystemStorageInfoLinuxCommonPrivate(parent)
{
}

QSystemStorageInfoPrivate::~QSystemStorageInfoPrivate()
{
}

QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QSystemDeviceInfoLinuxCommonPrivate *parent)
        : QSystemDeviceInfoLinuxCommonPrivate(parent)
{
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
}

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
    return QSystemDeviceInfo::UnknownProfile;
}

QString QSystemDeviceInfoPrivate::imei()
{
 #if !defined(QT_NO_DBUS)
    QDBusInterface connectionInterface("com.nokia.phone.SIM",
                                       "/com/nokia/csd/info",
                                       "com.nokia.csd.Info",
                                        QDBusConnection::systemBus());
    if(!connectionInterface.isValid()) {
        qWarning() << "interfacenot valid";
    }

    QDBusReply< QString > reply = connectionInterface.call("GetIMEINumber");
    return reply.value();

#endif
        return "Not Available";
}

QString QSystemDeviceInfoPrivate::imsi()
{
    QString retVal = "Not Available";
    GConfItem locationValues("/system/nokia/location");
    QStringList locationKeys = locationValues.listEntries();

    QStringList result;
    foreach (QString str, locationKeys) {
        if (str.contains("sim_imsi"))
            retVal = "Available";
        break;
    }
    return retVal;
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
    GConfItem locationValues("/system/nokia/location");
    QStringList locationKeys = locationValues.listEntries();
    QStringList result;
    int count = 0;
    foreach (QString str, locationKeys) {
        if (str.contains("sim_imsi"))
            count++;
    }

    if(count == 1) {
        return QSystemDeviceInfo::SingleSimAvailable;
    } else if (count == 2) {
        return QSystemDeviceInfo::DualSimAvailable;
    }
    return QSystemDeviceInfo::SimNotAvailable;
}

bool QSystemDeviceInfoPrivate::isDeviceLocked()
{
    QSystemScreenSaverPrivate priv;

    if(priv.isScreenLockEnabled()
        && priv.isScreenSaverActive()) {
        return true;
    }

    return false;
}

 QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QSystemScreenSaverLinuxCommonPrivate *parent)
         : QSystemScreenSaverLinuxCommonPrivate(parent)
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

bool QSystemScreenSaverPrivate::isScreenLockEnabled()
{
   return false;
}

bool QSystemScreenSaverPrivate::isScreenSaverActive()
{
    return false;
}

#include "moc_qsysteminfo_maemo_p.cpp"

QTM_END_NAMESPACE
