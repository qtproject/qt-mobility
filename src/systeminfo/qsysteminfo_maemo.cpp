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
#include <QtGui>
#include <QDesktopWidget>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QTimer>
#include <QMapIterator>

#if !defined(QT_NO_DBUS)
#include <qhalservice_linux_p.h>
#include <QtDBus>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusPendingCallWatcher>
#include <QDBusObjectPath>
#include <QDBusPendingCall>
#include "gconfitem.h" // Temporarily here.
#endif

#include <locale.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <mntent.h>
#include <sys/stat.h>


#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>

#endif

//we cannot include iwlib.h as the platform may not have it installed
//there we have to go via the kernel's wireless.h
//#include <iwlib.h>
//must be defined to be able to include kernel includes
#ifndef __user
#define __user
#endif

#include <linux/types.h>    /* required for wireless.h */
#include <sys/socket.h>     /* required for wireless.h */
#include <net/if.h>         /* required for wireless.h */

/* A lot of wireless.h have kernel includes which should be protected by
   #ifdef __KERNEL__. They course include errors due to redefinitions of types.
   This prevents those kernel headers being included by Qtopia.
   */
#ifndef _LINUX_IF_H
#define _LINUX_IF_H
#endif
#ifndef _LINUX_SOCKET_H
#define _LINUX_SOCKET_H
#endif
#include <linux/wireless.h>
#include <sys/ioctl.h>

#include <QDBusInterface>

QTM_BEGIN_NAMESPACE

static bool halAvailable()
{
    QDBusConnection dbusConnection = QDBusConnection::systemBus();
    if (dbusConnection.isConnected()) {
        QDBusConnectionInterface *dbiface = dbusConnection.interface();
        QDBusReply<bool> reply = dbiface->isServiceRegistered("org.freedesktop.Hal");
        if (reply.isValid() && reply.value()) {
            return reply.value();
        }
    }
    return false;
}

bool halIsAvailable;
//////// QSystemInfo
QSystemInfoPrivate::QSystemInfoPrivate(QSystemInfoLinuxCommonPrivate *parent)
 : QSystemInfoLinuxCommonPrivate(parent)
{
    halIsAvailable = halAvailable();
}

QSystemInfoPrivate::~QSystemInfoPrivate()
{
}

// 2 letter ISO 639-1
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

// "major.minor.build" format.
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

#if !defined(QT_NO_DBUS)
bool QSystemInfoPrivate::hasHalDeviceFeature(const QString &param)
{
    QHalInterface halIface;
    QStringList halDevices = halIface.getAllDevices();
    foreach(QString device, halDevices) {
        if(device.contains(param)) {
            return true;
        }
    }
    return false;
}

bool QSystemInfoPrivate::hasHalUsbFeature(qint32 usbClass)
{
    QHalInterface halIface;
    QStringList halDevices = halIface.getAllDevices();
    foreach(QString device, halDevices) {
        QHalDeviceInterface ifaceDevice(device);
        if (ifaceDevice.isValid()) {
            if(ifaceDevice.getPropertyString("info.subsystem") == "usb_device") {
                if(ifaceDevice.getPropertyInt("usb.interface.class") == usbClass) {
                    return true;
                }
            }
        }
    }
    return false;
}
#endif

bool QSystemInfoPrivate::hasSysFeature(const QString &featureStr)
{
    QString sysPath = "/sys/class/";
    QDir sysDir(sysPath);
    QStringList filters;
    filters << "*";
    QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
    foreach(QString dir, sysList) {
        QDir sysDir2(sysPath + dir);
        if(dir.contains(featureStr)) {
            QStringList sysList2 = sysDir2.entryList( filters ,QDir::Dirs, QDir::Name);
            if(!sysList2.isEmpty()) {
                return true;
            }
        }
    }
    return false;
}

bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    bool featureSupported = false;
    switch (feature) {
    case QSystemInfo::BluetoothFeature :
        {
            QString sysPath = "/sys/class/bluetooth/";
            QDir sysDir(sysPath);
            QStringList filters;
            filters << "*";
            QStringList sysList = sysDir.entryList( filters ,QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
            foreach(QString dir, sysList) {
                QFileInfo btFile(sysPath + dir+"/address");
                if(btFile.exists()) {
                    featureSupported = true;
                }
            }
        }
    break;
    case QSystemInfo::CameraFeature :
        {
#if !defined(QT_NO_DBUS)
            featureSupported = hasHalUsbFeature(0x06); // image
            if(featureSupported)
                return featureSupported;
#endif
            featureSupported = hasSysFeature("video");
        }
        break;
    case QSystemInfo::FmradioFeature :
        {
            QString sysPath = "/sys/class/video4linux/";
            QDir sysDir(sysPath);
            QStringList filters;
            filters << "*";
            QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
            if(sysList.contains("radio0")) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::IrFeature :
        {
#if !defined(QT_NO_DBUS)
        featureSupported = hasHalUsbFeature(0xFE);
        if(featureSupported)
            return featureSupported;
#endif
        featureSupported = hasSysFeature("irda"); //?
    }
        break;
    case QSystemInfo::LedFeature :
        {
            featureSupported = hasSysFeature("led"); //?
        }
        break;
    case QSystemInfo::MemcardFeature :
        {
#if !defined(QT_NO_DBUS)
            QHalInterface iface;
            if (iface.isValid()) {
                QHalInterface halIface;
                QStringList halDevices = halIface.getAllDevices();
                foreach(QString device, halDevices) {
                    QHalDeviceInterface ifaceDevice(device);
                    if (ifaceDevice.isValid()) {
                        if(ifaceDevice.getPropertyString("info.subsystem") == "mmc_host") {
                            return true;
                        }
                        if(ifaceDevice.getPropertyBool("storage.removable")) {
                            return true;
                        }
                    }
                }
            }
#endif
        }
        break;
    case QSystemInfo::UsbFeature :
        {
#if !defined(QT_NO_DBUS)
        featureSupported = hasHalDeviceFeature("usb");
        if(featureSupported)
            return featureSupported;
#endif
            featureSupported = hasSysFeature("usb_host");
        }
        break;
    case QSystemInfo::VibFeature :
        featureSupported = hasHalDeviceFeature("vibra"); //might not always be true
        break;
    case QSystemInfo::WlanFeature :
        {
#if !defined(QT_NO_DBUS)
            QHalInterface iface;
            if (iface.isValid()) {
                QStringList list = iface.findDeviceByCapability("net.80211");
                if(!list.isEmpty()) {
                    featureSupported = true;
                    break;
                }
            }
#endif
            featureSupported = hasSysFeature("80211");
        }
        break;
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
            if(halIsAvailable) {
                QHalInterface iface;
                QStringList touchSupport =
                        iface.findDeviceByCapability("input.touchpad");
                if(touchSupport.count()) {
                    featureSupported = true;
                } else {
                    featureSupported = false;
                }
            }
        }
        break;
    default:
        featureSupported = false;
        break;
    };
    return featureSupported;
}

//////// QSystemNetworkInfo
QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QSystemNetworkInfoLinuxCommonPrivate *parent)
        : QSystemNetworkInfoLinuxCommonPrivate(parent)
{
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
}


QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::GsmMode:
        {
#if 0
#if !defined(QT_NO_DBUS)
            QDBusInterface connectionInterface("com.nokia.phone.net",
                                               "/com/nokia/phone/net",
                                               "com.nokia.SystemInfo",
                                                QDBusConnection::systemBus());
            if(!connectionInterface.isValid()) {
                qWarning() << "interfacenot valid";
            }
            QDBusReply< QByteArray > reply = connectionInterface.call("GetConfigValue", "/device/sw-release-ver");
            return reply.value();
#endif
#endif
        }
        break;
    case QSystemNetworkInfo::WlanMode:
        {
            QString baseSysDir = "/sys/class/net/";
            QDir wDir(baseSysDir);
            QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo wiFi(devFile + "/wireless");
                QFileInfo fi("/proc/net/route");
                if(wiFi.exists() && fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QString result = rx.readAll();
                        if(result.contains(dir)) {
                            return QSystemNetworkInfo::Connected;
                        } else {
                            return QSystemNetworkInfo::NoNetworkAvailable;
                        }
                    }
                }
            }
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            QString baseSysDir = "/sys/class/net/";
            QDir eDir(baseSysDir);
            QStringList dirs = eDir.entryList(QStringList() << "eth*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi("/proc/net/route");
                if(fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QString result = rx.readAll();
                        if(result.contains(dir)) {
                            return QSystemNetworkInfo::Connected;
                        } else {
                            return QSystemNetworkInfo::NoNetworkAvailable;
                        }
                    }
                }
            }
        }
        break;
        case QSystemNetworkInfo::BluetoothMode:
        {
#if !defined(QT_NO_DBUS)
            return getBluetoothNetStatus();
#endif
       }
        break;
    default:
        break;
    };
    return QSystemNetworkInfo::UndefinedStatus;
}

#if !defined(QT_NO_DBUS)
QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::getBluetoothNetStatus()
{
    QDBusConnection dbusConnection = QDBusConnection::systemBus();
    QDBusInterface *connectionInterface;
    connectionInterface = new QDBusInterface("org.bluez",
                                             "/org/bluez/network",
                                             "org.bluez.network.Manager",
                                             dbusConnection);
    if (connectionInterface->isValid()) {

        QDBusReply<  QStringList > reply = connectionInterface->call("ListConnections");
        if (reply.isValid()) {
            if(reply.value().count() > 0) {
                return QSystemNetworkInfo::Connected;
            } else {
                return QSystemNetworkInfo::NoNetworkAvailable;
            }

        } else {
            qWarning() << "NOT" << reply.error();
        }
       }
       return QSystemNetworkInfo::UndefinedStatus;
}
#endif


int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            QString result;
            QString baseSysDir = "/sys/class/net/";
            QDir wDir(baseSysDir);
            QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + "/wireless/link");
                if(fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&rx);
                        in >> result;
                        rx.close();
                        return result.toInt();

                    }
                }
            }
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            QString result;
            QString baseSysDir = "/sys/class/net/";
            QDir eDir(baseSysDir);
            QStringList dirs = eDir.entryList(QStringList() << "eth*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + "/carrier");
                if(fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&rx);
                        in >> result;
                        rx.close();
                        return result.toInt() * 100;

                    }
                }
            }
        }
        break;
        case QSystemNetworkInfo::BluetoothMode:
        {
#if !defined(QT_NO_DBUS)
            return getBluetoothRssi();
#endif
        }
        break;
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::WcdmaMode:
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
    return QString();
}

// Mobile Network Code
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
    case QSystemNetworkInfo::WlanMode:
        {
            if(networkStatus(mode) != QSystemNetworkInfo::Connected) {
                qWarning() << "not connected";
                return netname;
            }

            QString wlanInterface;
            QString baseSysDir = "/sys/class/net/";
            QDir wDir(baseSysDir);
            QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + "/wireless");
                if(fi.exists()) {
                    wlanInterface = dir;
                    qWarning() << "interface is" << wlanInterface;
                }
            }
            int sock = socket(PF_INET, SOCK_DGRAM, 0);
            if (sock > 0) {
                const char* someRandomBuffer[IW_ESSID_MAX_SIZE + 1];
                struct iwreq wifiExchange;
                memset(&wifiExchange, 0, sizeof(wifiExchange));
                memset(someRandomBuffer, 0, sizeof(someRandomBuffer));

                wifiExchange.u.essid.pointer = (caddr_t) someRandomBuffer;
                wifiExchange.u.essid.length = IW_ESSID_MAX_SIZE;
                wifiExchange.u.essid.flags = 0;

                const char* interfaceName = wlanInterface.toLatin1();
                strncpy(wifiExchange.ifr_name, interfaceName, IFNAMSIZ);
                wifiExchange.u.essid.length = IW_ESSID_MAX_SIZE + 1;

                if (ioctl(sock, SIOCGIWESSID, &wifiExchange) == 0) {
                    const char *ssid = (const char *)wifiExchange.u.essid.pointer;
                    netname = ssid;
                }
            } else {
                qWarning() << "no socket";
            }
            close(sock);
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            QFile resFile("/etc/resolv.conf");
            if(resFile.exists()) {
                if(resFile.exists() && resFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QString line;
                    QTextStream in(&resFile);
                    do {
                        line = in.readLine();
                        if(line.contains("domain")) {
                            netname = line.section(" ",1,1); //guessing here
                        }
                } while (!line.isNull());
                resFile.close();
            }
        }
    }
    break;
        case QSystemNetworkInfo::BluetoothMode:
            {
    #if !defined(QT_NO_DBUS)
        netname = getBluetoothInfo("name");
#endif
    }
        break;
    default:
        break;
    };
    return netname;
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
        case QSystemNetworkInfo::WlanMode:
        {
            QString result;
            QString baseSysDir = "/sys/class/net/";
            QDir wDir(baseSysDir);
            QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + "/wireless");
                if(fi.exists()) {
                    QFile rx(devFile + "/address");
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&rx);
                        in >> result;
                        rx.close();
                        return result;
                    }
                }
            }
        }
        break;
        case QSystemNetworkInfo::EthernetMode:
        {
            QString result;
            QString baseSysDir = "/sys/class/net/";
            QDir eDir(baseSysDir);
            QStringList dirs = eDir.entryList(QStringList() << "eth*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + "/address");
                if(fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&rx);
                        in >> result;
                        rx.close();
                        return result;
                    }
                }
            }
        }
        break;
        case QSystemNetworkInfo::BluetoothMode:
        {
#if !defined(QT_NO_DBUS)
            return getBluetoothInfo("address");
#endif
        }
        break;
    default:
        break;
    };
    return QString();
}

QNetworkInterface QSystemNetworkInfoPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
#if !defined(QT_NO_DBUS)
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            QHalInterface iface;
            if (iface.isValid()) {
                QStringList list = iface.findDeviceByCapability("net.80211");
                if(!list.isEmpty()) {
                    foreach(QString netDev, list) {
                        QString deviceName ;
                        QHalDeviceInterface ifaceDevice(netDev);
                        deviceName  = ifaceDevice.getPropertyString("net.interface");
                        if(list.count() > 1) {
                            QString baseFIle = "/sys/class/net/" + deviceName+"/operstate";
                            QFile rx(baseFIle);
                            if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                                QString operatingState;
                                QTextStream in(&rx);
                                in >> operatingState;
                                rx.close();
                                if(!operatingState.contains("unknown")
                                    || !operatingState.contains("down")) {
                                    if(isDefaultInterface(deviceName))
                                        return QNetworkInterface::interfaceFromName(deviceName);
                                }
                            }
                        } else {
                            return QNetworkInterface::interfaceFromName(deviceName);
                        }
                    }
                }
            }
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            QHalInterface iface;
            if (iface.isValid()) {
                QStringList list = iface.findDeviceByCapability("net.80203");
                if(!list.isEmpty()) {
                    foreach(QString netDev, list) {
                        QString deviceName ;
                        QHalDeviceInterface ifaceDevice(netDev);
                        deviceName  = ifaceDevice.getPropertyString("net.interface");
                        if(list.count() > 1) {
                            QString baseFIle = "/sys/class/net/" + deviceName+"/operstate";
                            QFile rx(baseFIle);
                            if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                                QString operatingState;
                                QTextStream in(&rx);
                                in >> operatingState;
                                rx.close();
                                if(!operatingState.contains("unknown")
                                    || !operatingState.contains("down")) {
                                    if(isDefaultInterface(deviceName))
                                        return QNetworkInterface::interfaceFromName(deviceName);
                                }
                            }
                        } else {
                            return QNetworkInterface::interfaceFromName(deviceName);
                        }
                    }
                }
            }
        }
        break;
        case QSystemNetworkInfo::BluetoothMode:
        {
        }
        break;
    default:
        break;
    };
#else
    QString result;
    QString baseSysDir = "/sys/class/net/";
    QDir eDir(baseSysDir);
    QStringList dirs = eDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach(QString dir, dirs) {
        QString devFile = baseSysDir + dir;
        QFileInfo devfi(devFile + "/device");
        if(!devfi.exists()) {
            continue;
        }
        QString baseFIle = "/sys/class/net/" + devFile+"/operstate";
        QFile rx(baseFIle);
        if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString operatingState;
            QTextStream in(&rx);
            in >> operatingState;
            rx.close();
            if(operatingState.contains("unknown")) {
                continue;
            }
        }
        switch(mode) {
        case QSystemNetworkInfo::WlanMode:
            {
                QFileInfo fi(devFile + "/wireless");
                if(fi.exists()) {
                    return QNetworkInterface::interfaceFromName(dir);
                }
            }
            break;
            case QSystemNetworkInfo::EthernetMode:
                {
                QFileInfo fi(devFile + "/wireless");
                if(!fi.exists()) {
                    return QNetworkInterface::interfaceFromName(dir);
                }
            }
            break;
            case QSystemNetworkInfo::BluetoothMode:
            {

            }
            break;

            default:
            break;
        };
    }
#endif
    return QNetworkInterface();
}

#if !defined(QT_NO_DBUS)
bool QSystemNetworkInfoPrivate::isDefaultInterface(const QString &deviceName)
{
    QFile routeFilex("/proc/net/route");
    if(routeFilex.exists() && routeFilex.open(QIODevice::ReadOnly
                                              | QIODevice::Text)) {
        QTextStream rin(&routeFilex);
        QString line = rin.readLine();
        while (!line.isNull()) {
            QString lineSection = line.section("\t",2,2,QString::SectionSkipEmpty);
            if(lineSection != "00000000" && lineSection!="Gateway")
                if(line.section("\t",0,0,QString::SectionSkipEmpty) == deviceName) {
                routeFilex.close();
                return true;
            }
            line = rin.readLine();
        }
    }
    routeFilex.close();
    return false;
}

int QSystemNetworkInfoPrivate::getBluetoothRssi()
{
    return 0;
}

QString QSystemNetworkInfoPrivate::getBluetoothInfo(const QString &file)
{
    QString sysPath = "/sys/class/bluetooth/";
    QDir sysDir(sysPath);
    QStringList filters;
    filters << "*";
    QStringList sysList = sysDir.entryList( filters ,QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    foreach(QString dir, sysList) {
        QFile btFile(sysPath + dir+"/"+file);
        if(btFile.exists()) {
            if (btFile.open(QIODevice::ReadOnly)) {
                QTextStream btFileStream(&btFile);
                QString line = btFileStream.readAll();
                return line.simplified();
            }
        }
    }
    return QString();
}
#endif

//////// QSystemDisplayInfo
QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QSystemDisplayInfoLinuxCommonPrivate *parent)
        : QSystemDisplayInfoLinuxCommonPrivate(parent)
{
    halIsAvailable = halAvailable();
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


/*    Q_UNUSED(screen);
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalInterface iface;
        if (iface.isValid()) {
            QStringList list = iface.findDeviceByCapability("laptop_panel");
            if(!list.isEmpty()) {
                foreach(QString lapDev, list) {
                    QHalDeviceInterface ifaceDevice(lapDev);
                    QHalDeviceLaptopPanelInterface lapIface(lapDev);
                    float numLevels = ifaceDevice.getPropertyInt("laptop_panel.num_levels") - 1;
                    float curLevel = lapIface.getBrightness();
                    return curLevel / numLevels * 100;
                }
            }
        }
#endif
    } else {
        QString backlightPath = "/proc/acpi/video/";
        QDir videoDir(backlightPath);
        QStringList filters;
        filters << "*";
        QStringList brightnessList = videoDir.entryList(filters,
                                                        QDir::Dirs
                                                        | QDir::NoDotAndDotDot,
                                                        QDir::Name);
        foreach(QString brightnessFileName, brightnessList) {
            float numLevels = 0.0;
            float curLevel = 0.0;
            QFile curBrightnessFile(backlightPath+brightnessFileName+"/LCD/brightness");
            if(!curBrightnessFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qWarning()<<"File not opened";
            } else {
                QString  strvalue;
                strvalue = curBrightnessFile.readAll().trimmed();
                if(strvalue.contains("levels")) {
                    QStringList list = strvalue.split(" ");
                    numLevels = list.at(2).toFloat();
                }
                if(strvalue.contains("current")) {
                    QStringList list = strvalue.split(": ");
                    curLevel = list.at(list.count()-1).toFloat();
                }
                curBrightnessFile.close();
                return curLevel / numLevels * 100;
            }
        }
    }
#if 0
    QString backlightPath = "/sys/devices/virtual/backlight/";
    QDir videoDir(backlightPath);
    QStringList filters;
    filters << "*";
    QStringList brightnessList = videoDir.entryList(filters,
                                                     QDir::Dirs
                                                     | QDir::NoDotAndDotDot,
                                                     QDir::Name);
    foreach(QString brightnessFileName, brightnessList) {
        float numLevels = 0.0;
        float curLevel = 0.0;
        QFile curBrightnessFile(backlightPath+brightnessFileName+"/brightness");
        if(!curBrightnessFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning()<<"File not opened";
        } else {
            QString strvalue;
            strvalue = curBrightnessFile.readLine().trimmed();
            curBrightnessFile.close();
            curLevel = strvalue.toFloat();

            QFile maxBrightnessFile(backlightPath+brightnessFileName+"/max_brightness");
            if(!maxBrightnessFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qWarning()<<"File not opened";
            } else {
                QString strvalue;
                strvalue = maxBrightnessFile.readLine().trimmed();
                maxBrightnessFile.close();
                numLevels = strvalue.toFloat();
            }
            return curLevel / numLevels * 100;
        }
    }
#endif */
    return -1;
}

int QSystemDisplayInfoPrivate::colorDepth(int screen)
{
#ifdef Q_WS_X11
    QDesktopWidget wid;
    return wid.screen(screen)->x11Info().depth();
#else
        return QPixmap::defaultDepth();
#endif
}


//////// QSystemStorageInfo
QSystemStorageInfoPrivate::QSystemStorageInfoPrivate(QSystemStorageInfoLinuxCommonPrivate *parent)
        : QSystemStorageInfoLinuxCommonPrivate(parent)
{
    halIsAvailable = halAvailable();
}


QSystemStorageInfoPrivate::~QSystemStorageInfoPrivate()
{
}

qint64 QSystemStorageInfoPrivate::availableDiskSpace(const QString &driveVolume)
{
    mountEntries();
    struct statfs fs;
    if(statfs(mountEntriesMap[driveVolume].toLatin1(), &fs ) == 0 ) {
                long blockSize = fs.f_bsize;
                long availBlocks = fs.f_bavail;
                return (double)availBlocks * blockSize;
            }
    return 0;
}

qint64 QSystemStorageInfoPrivate::totalDiskSpace(const QString &driveVolume)
{
    mountEntries();
    struct statfs fs;
    if(statfs(mountEntriesMap[driveVolume].toLatin1(), &fs ) == 0 ) {
        long blockSize = fs.f_bsize;
        long totalBlocks = fs.f_blocks;
        return (double)totalBlocks * blockSize;
    }
    return 0;
}

QSystemStorageInfo::DriveType QSystemStorageInfoPrivate::typeForDrive(const QString &driveVolume)
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QStringList mountedVol;
        QHalInterface iface;
        QStringList list = iface.findDeviceByCapability("volume");
        if(!list.isEmpty()) {
            foreach(QString vol, list) {
                QHalDeviceInterface ifaceDevice(vol);
                if(driveVolume == ifaceDevice.getPropertyString("block.device")) {
                    QHalDeviceInterface ifaceDeviceParent(ifaceDevice.getPropertyString("info.parent"), this);

                    if(ifaceDeviceParent.getPropertyBool("storage.removable")
                        ||  ifaceDeviceParent.getPropertyString("storage.drive_type") != "disk") {
                        return QSystemStorageInfo::RemovableDrive;
                        break;
                    } else {
                         return QSystemStorageInfo::InternalDrive;
                    }
                }
            }
        }
#endif
    } else {
        //no hal need to manually read sys file for block device
        QString dmFile;

        if(driveVolume.contains("mapper")) {
            struct stat stat_buf;
            stat( driveVolume.toLatin1(), &stat_buf);
            //                    qWarning() << "Device number"
            //                            << ((stat_buf.st_rdev >> 8) & 0377)
            //                            << (stat_buf.st_rdev & 0377);

            dmFile = QString("/sys/block/dm-%1/removable").arg(stat_buf.st_rdev & 0377);

        } else {

            dmFile = driveVolume.section("/",2,3);
            if (dmFile.left(3) == "mmc") { //assume this dev is removable sd/mmc card.
                return QSystemStorageInfo::RemovableDrive;
            }

            if(dmFile.length() > 3) { //if device has number, we need the 'parent' device
                dmFile.chop(1);
                if (dmFile.right(1) == "p") //get rid of partition number
                    dmFile.chop(1);
            }
            dmFile = "/sys/block/"+dmFile+"/removable";
        }

        QFile file(dmFile);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open sys file";
        } else {
            QTextStream sysinfo(&file);
            QString line = sysinfo.readAll();
            if(line.contains("1")) {
                return QSystemStorageInfo::RemovableDrive;
            }
        }
    }
    return QSystemStorageInfo::InternalDrive;
}

QStringList QSystemStorageInfoPrivate::logicalDrives()
{
    mountEntries();
    return mountEntriesMap.keys();
}

void QSystemStorageInfoPrivate::mountEntries()
{
    mountEntriesMap.clear();
    FILE *mntfp = setmntent( _PATH_MOUNTED/*_PATH_MNTTAB*//*"/proc/mounts"*/, "r" );
    mntent *me = getmntent(mntfp);
    while(me != NULL) {
        struct statfs fs;
        if(statfs(me->mnt_dir, &fs ) ==0 ) {
            QString num;
            // weed out a few types
            if ( fs.f_type != 0x01021994 //tmpfs
                 && fs.f_type != 0x9fa0 //procfs
                 && fs.f_type != 0x1cd1 //
                 && fs.f_type != 0x62656572
                 && fs.f_type != 0xabababab // ???
                 && fs.f_type != 0x52654973
                 && fs.f_type != 0x42494e4d
                 && fs.f_type != 0x64626720
                 && fs.f_type != 0x73636673 //securityfs
                 && fs.f_type != 0x65735543 //fusectl
                 ) {
                if(!mountEntriesMap.keys().contains(me->mnt_dir)
                    && QString(me->mnt_fsname).contains("/dev")) {
                    mountEntriesMap[me->mnt_fsname] = me->mnt_dir;
                }
            }
        }
        me = getmntent(mntfp);
    }
    endmntent(mntfp);
}


//////// QSystemDeviceInfo
QSystemDeviceInfoPrivate::QSystemDeviceInfoPrivate(QSystemDeviceInfoLinuxCommonPrivate *parent)
        : QSystemDeviceInfoLinuxCommonPrivate(parent)
{
    halIsAvailable = halAvailable();
    setConnection();
 #if !defined(QT_NO_DBUS)
    setupBluetooth();
#endif
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
}

void QSystemDeviceInfoPrivate::setConnection()
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalInterface iface;

        QStringList list = iface.findDeviceByCapability("battery");
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid()) {
                    QString batType = halIfaceDevice->getPropertyString("battery.type");
                    if(batType == "primary" || batType == "pda") {
                        if(halIfaceDevice->setConnections() ) {
                            if(!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                        this,SLOT(halChanged(int,QVariantList)))) {
                                qWarning() << "connection malfunction";
                            }
                        }
                        break;
                    }
                }
            }
        }

        list = iface.findDeviceByCapability("ac_adapter");
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid()) {
                    if(halIfaceDevice->setConnections() ) {
                        if(!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                    this,SLOT(halChanged(int,QVariantList)))) {
                            qWarning() << "connection malfunction";
                        }
                    }
                    break;
                }
            }
        }

        list = iface.findDeviceByCapability("battery");
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid()) {
                    if(halIfaceDevice->setConnections()) {
                        if(!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                    this,SLOT(halChanged(int,QVariantList)))) {
                            qWarning() << "connection malfunction";
                        }
                    }
                    break;
                }
            }
        }

#endif
    }
}


#if !defined(QT_NO_DBUS)
void QSystemDeviceInfoPrivate::halChanged(int,QVariantList map)
{
    for(int i=0; i < map.count(); i++) {
//       qWarning() << __FUNCTION__ << map.at(i).toString();
       if(map.at(i).toString() == "battery.charge_level.percentage") {
            int level = batteryLevel();
            emit batteryLevelChanged(level);
            if(level < 4) {
                emit batteryStatusChanged(QSystemDeviceInfo::BatteryCritical);
            } else if(level < 11) {
                emit batteryStatusChanged(QSystemDeviceInfo::BatteryVeryLow);
            } else if(level < 41) {
                emit batteryStatusChanged(QSystemDeviceInfo::BatteryLow);
            } else if(level > 40) {
                emit batteryStatusChanged(QSystemDeviceInfo::BatteryNormal);
            }
            else {
                emit batteryStatusChanged(QSystemDeviceInfo::NoBatteryLevel);
            }
        }
        if((map.at(i).toString() == "ac_adapter.present")
        || (map.at(i).toString() == "battery.rechargeable.is_charging")) {
            QSystemDeviceInfo::PowerState state = currentPowerState();
            emit powerStateChanged(state);
       }} //end map
}
#endif

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
    return QSystemDeviceInfo::UnknownProfile;
}

QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfoPrivate::inputMethodType()
{
    QSystemDeviceInfo::InputMethodFlags methods = 0;
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalInterface iface2;
        if (iface2.isValid()) {
            QStringList capList;
            capList << "input.keyboard" << "input.keys" << "input.keypad" << "input.mouse" << "input.tablet" << "input.touchpad";
            for(int i = 0; i < capList.count(); i++) {
                QStringList list = iface2.findDeviceByCapability(capList.at(i));
                if(!list.isEmpty()) {
                    switch(i) {
                    case 0:
                        methods = (methods | QSystemDeviceInfo::Keyboard);
                        break;
                    case 1:
                        methods = (methods | QSystemDeviceInfo::Keys);
                        break;
                    case 2:
                        methods = (methods | QSystemDeviceInfo::Keypad);
                        break;
                    case 3:
                        methods = (methods | QSystemDeviceInfo::Mouse);
                        break;
                    case 4:
                    case 5:
                        methods = (methods | QSystemDeviceInfo::SingleTouch);
                        break;
                    };
                }
            }
            if(methods != 0)
                return methods;
        }
#endif
    }
    QString inputsPath = "/sys/class/input/";
    QDir inputDir(inputsPath);
    QStringList filters;
    filters << "event*";
    QStringList inputList = inputDir.entryList( filters ,QDir::Dirs, QDir::Name);
    foreach(QString inputFileName, inputList) {
        QFile file(inputsPath+inputFileName+"/device/name");
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning()<<"File not opened";
        } else {
            QString strvalue;
            strvalue = file.readLine();
            file.close();
            if(strvalue.contains("keyboard")) {
                if( (methods & QSystemDeviceInfo::Keyboard) != QSystemDeviceInfo::Keyboard) {
                    methods = (methods | QSystemDeviceInfo::Keyboard);
                }
            } else if(strvalue.contains("Mouse")) {
                if( (methods & QSystemDeviceInfo::Mouse) != QSystemDeviceInfo::Mouse) {
                    methods = (methods | QSystemDeviceInfo::Mouse);
                }
            } else if(strvalue.contains("Button")) {
                if( (methods & QSystemDeviceInfo::Keys) != QSystemDeviceInfo::Keys) {
                    methods = (methods | QSystemDeviceInfo::Keys);
                }
            } else if(strvalue.contains("TouchScreen")) {
                if( (methods & QSystemDeviceInfo::SingleTouch) != QSystemDeviceInfo::SingleTouch) {
                    methods = (methods | QSystemDeviceInfo::SingleTouch);
                }
            }
        }
    }
    return methods;
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

QString QSystemDeviceInfoPrivate::manufacturer()
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalDeviceInterface iface("/org/freedesktop/Hal/devices/computer");
        QString manu;
        if (iface.isValid()) {
            manu = iface.getPropertyString("system.firmware.vendor");
            if(manu.isEmpty()) {
                manu = iface.getPropertyString("system.hardware.vendor");
                if(!manu.isEmpty()) {
                    return manu;
                }
            }
        }
#endif
    }
    QFile vendorId("/sys/devices/virtual/dmi/id/board_vendor");
    if (vendorId.open(QIODevice::ReadOnly)) {
        QTextStream cpuinfo(&vendorId);
        return cpuinfo.readLine().trimmed();
    } else {
        //        qWarning() << "Could not open /sys/devices/virtual/dmi/id/board_vendor";
        QFile file("/proc/cpuinfo");
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open /proc/cpuinfo";
        } else {
            QTextStream cpuinfo(&file);
            QString line = cpuinfo.readLine();
            while (!line.isNull()) {
                line = cpuinfo.readLine();
                if(line.contains("vendor_id")) {
                    return line.split(": ").at(1).trimmed();
                }
            }
        }
    }
    return QString();
}

QString QSystemDeviceInfoPrivate::model()
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalDeviceInterface iface("/org/freedesktop/Hal/devices/computer");
        QString model;
        if (iface.isValid()) {
            model = iface.getPropertyString("system.kernel.machine");
            if(!model.isEmpty())
                model += " ";
            model += iface.getPropertyString("system.chassis.type");
            if(!model.isEmpty())
                return model;
        }
#endif
    }
    QFile file("/proc/cpuinfo");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open /proc/cpuinfo";
    } else {
        QTextStream cpuinfo(&file);
        QString line = cpuinfo.readLine();
        while (!line.isNull()) {
            line = cpuinfo.readLine();
            if(line.contains("model name")) {
                return line.split(": ").at(1).trimmed();
            }
        }
    }
    return QString();
}

QString QSystemDeviceInfoPrivate::productName()
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalDeviceInterface iface("/org/freedesktop/Hal/devices/computer");
        QString productName;
        if (iface.isValid()) {
            productName = iface.getPropertyString("system.hardware.product");
//            productName = iface.getPropertyString("info.product");
            if(productName.isEmpty()) {
                productName = iface.getPropertyString("system.product");
                if(!productName.isEmpty())
                    return productName;
            } else {
                return productName;
            }
        }
#endif
    }
    QDir dir("/etc");
    if(dir.exists()) {
        QStringList langList;
        QFileInfoList localeList = dir.entryInfoList(QStringList() << "*release",
                                                     QDir::Files | QDir::NoDotAndDotDot,
                                                     QDir::Name);
        foreach(QFileInfo fileInfo, localeList) {
            QString filepath = fileInfo.filePath();
            QFile file(filepath);
            if (file.open(QIODevice::ReadOnly)) {
                QTextStream prodinfo(&file);
                QString line = prodinfo.readLine();
                while (!line.isNull()) {
                    if(filepath.contains("lsb.release")) {
                        if(line.contains("DISTRIB_DESCRIPTION")) {
                            return line.split("=").at(1).trimmed();
                        }
                    } else {
                        return line;
                    }
                    line = prodinfo.readLine();
                }
            }
        } //end foreach
    }

    QFile file("/etc/issue");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open /proc/cpuinfo";
    } else {
        QTextStream prodinfo(&file);
        QString line = prodinfo.readLine();
        while (!line.isNull()) {
            line = prodinfo.readLine();
            if(!line.isEmpty()) {
                QStringList lineList = line.split(" ");
                for(int i = 0; i < lineList.count(); i++) {
                    if(lineList.at(i).toFloat()) {
                        return lineList.at(i-1) + " "+ lineList.at(i);
                    }
                }
            }
        }
    }
    return QString();
}

int QSystemDeviceInfoPrivate::batteryLevel() const
{
    float levelWhenFull = 0.0;
    float level = 0.0;
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalInterface iface;
        QStringList list = iface.findDeviceByCapability("battery");
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (ifaceDevice.isValid()) {
//                    qWarning() << ifaceDevice.getPropertyString("battery.type")
//                            << ifaceDevice.getPropertyInt("battery.charge_level.percentage");
                    if(!ifaceDevice.getPropertyBool("battery.present")
                        && (ifaceDevice.getPropertyString("battery.type") != "pda"
                             || ifaceDevice.getPropertyString("battery.type") != "primary")) {
                        qWarning() << "XXXXXXXXXXXXX";
                        return 0;
                    } else {
                        level = ifaceDevice.getPropertyInt("battery.charge_level.percentage");
                        return level;
                    }
                }
            }
        }
#endif
    } else {
        QFile infofile("/proc/acpi/battery/BAT0/info");
        if (!infofile.open(QIODevice::ReadOnly)) {
            //   qWarning() << "Could not open /proc/acpi/battery/BAT0/info";
            return QSystemDeviceInfo::NoBatteryLevel;
        } else {
            QTextStream batinfo(&infofile);
            QString line = batinfo.readLine();
            while (!line.isNull()) {
                if(line.contains("design capacity")) {
                    levelWhenFull = line.split(" ").at(1).trimmed().toFloat();
                    qWarning() << levelWhenFull;
                    infofile.close();
                    break;
                }
                line = batinfo.readLine();
            }
            infofile.close();
        }

        QFile statefile("/proc/acpi/battery/BAT0/state");
        if (!statefile.open(QIODevice::ReadOnly)) {
            //     qWarning() << "Could not open /proc/acpi/battery/BAT0/state";
            return QSystemDeviceInfo::NoBatteryLevel;
        } else {
            QTextStream batstate(&statefile);
            QString line = batstate.readLine();
            while (!line.isNull()) {
                if(line.contains("remaining capacity")) {
                    level = line.split(" ").at(1).trimmed().toFloat();
                    qWarning() << level;
                    statefile.close();
                    break;
                }
                line = batstate.readLine();
            }
        }
        if(level != 0 && levelWhenFull != 0) {
            level = level / levelWhenFull * 100;
            return level;
        }
    }
    return 0;
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

 QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
 {
#if !defined(QT_NO_DBUS)
        QHalInterface iface;
        QStringList list = iface.findDeviceByCapability("battery");
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (iface.isValid()) {
                    if (ifaceDevice.getPropertyBool("battery.rechargeable.is_charging")) {
                        return QSystemDeviceInfo::WallPowerChargingBattery;
                    }
                }
            }
        }

        list = iface.findDeviceByCapability("ac_adapter");
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (ifaceDevice.isValid()) {
                    if(ifaceDevice.getPropertyBool("ac_adapter.present")) {
                        return QSystemDeviceInfo::WallPower;
                    } else {
                        return QSystemDeviceInfo::BatteryPower;
                    }
                }
            }
        }

#else
        QFile statefile("/proc/acpi/battery/BAT0/state");
        if (!statefile.open(QIODevice::ReadOnly)) {
            //  qWarning() << "Could not open /proc/acpi/battery/BAT0/state";
        } else {
            QTextStream batstate(&statefile);
            QString line = batstate.readLine();
            while (!line.isNull()) {
                if(line.contains("charging state")) {
                    if(line.split(" ").at(1).trimmed() == "discharging") {
                        return QSystemDeviceInfo::BatteryPower;
                    }
                    if(line.split(" ").at(1).trimmed() == "charging") {
                        return QSystemDeviceInfo::WallPowerChargingBattery;
                    }
                }
            }
        }
#endif
        return QSystemDeviceInfo::WallPower;
 }

#if !defined(QT_NO_DBUS)
 void QSystemDeviceInfoPrivate::setupBluetooth()
 {
     QDBusConnection dbusConnection = QDBusConnection::systemBus();
     QDBusInterface *connectionInterface;
     connectionInterface = new QDBusInterface("org.bluez",
                                              "/",
                                              "org.bluez.Manager",
                                              dbusConnection);
     if (connectionInterface->isValid()) {

         QDBusReply<  QDBusObjectPath > reply = connectionInterface->call("DefaultAdapter");
         if (reply.isValid()) {
             QDBusInterface *adapterInterface;
             adapterInterface = new QDBusInterface("org.bluez",
                                                   reply.value().path(),
                                                   "org.bluez.Adapter",
                                                   dbusConnection);
             if (adapterInterface->isValid()) {
                 if (!dbusConnection.connect("org.bluez",
                                           reply.value().path(),
                                            "org.bluez.Adapter",
                                            "PropertyChanged",
                                            this,SLOT(bluezPropertyChanged(QString, QDBusVariant)))) {
                     qWarning() << "bluez could not connect signal";
                 }
             }
         }
     }
 }
#endif

#if !defined(QT_NO_DBUS)
 void QSystemDeviceInfoPrivate::bluezPropertyChanged(const QString &str, QDBusVariant v)
 {
     qWarning() << str << v.variant().toBool();
     emit bluetoothStateChanged(v.variant().toBool());
 }
#endif

 //////////////
 ///////
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
