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
#include "qsysteminfo_linux_common_p.h"
#include <QTimer>
#include <QFile>
#include <QDir>

#if !defined(QT_NO_DBUS)
#include <qhalservice_linux_p.h>
#include <QtDBus/QtDBus>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusPendingCallWatcher>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusPendingCall>
#endif

#include <QDesktopWidget>

#include <locale.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <mntent.h>
#include <sys/stat.h>

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#endif

#ifdef BLUEZ_SUPPORTED
# include <bluetooth/bluetooth.h>
# include <bluetooth/bnep.h>
#endif
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

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

static bool halAvailable()
{
#if !defined(QT_NO_DBUS)
    QDBusConnection dbusConnection = QDBusConnection::systemBus();
    if (dbusConnection.isConnected()) {
        QDBusConnectionInterface *dbiface = dbusConnection.interface();
        QDBusReply<bool> reply = dbiface->isServiceRegistered("org.freedesktop.Hal");
        if (reply.isValid() && reply.value()) {
            return reply.value();
        }
    }
#endif
    return false;
}


bool halIsAvailable;

QTM_BEGIN_NAMESPACE

QSystemInfoLinuxCommonPrivate::QSystemInfoLinuxCommonPrivate(QObject *parent) : QObject(parent)
{
    halIsAvailable = halAvailable();
    langCached = currentLanguage();
}

QSystemInfoLinuxCommonPrivate::~QSystemInfoLinuxCommonPrivate()
{
}

void QSystemInfoLinuxCommonPrivate::startLanguagePolling()
{
    QString checkLang = QString::fromLocal8Bit(qgetenv("LANG"));
    if(langCached.isEmpty()) {
        currentLanguage();
    }
    checkLang = checkLang.left(2);
    if(checkLang != langCached) {
        emit currentLanguageChanged(checkLang);
        langCached = checkLang;
    }
    langTimer = new QTimer(this);
    QTimer::singleShot(1000, this, SLOT(startLanguagePolling()));
}

QString QSystemInfoLinuxCommonPrivate::currentLanguage() const
{
    QString lang;
    if(langCached.isEmpty()) {
        lang  = QLocale::system().name().left(2);
        if(lang.isEmpty() || lang == QLatin1String("C")) {
            lang = QLatin1String("en");
        }
    } else {
        lang = langCached;
    }
    return lang;
}

bool QSystemInfoLinuxCommonPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
     bool featureSupported = false;
     switch (feature) {
     case QSystemInfo::BluetoothFeature :
         {
             const QString sysPath = "/sys/class/bluetooth/";
             const QDir sysDir(sysPath);
             QStringList filters;
             filters << "*";
             const QStringList sysList = sysDir.entryList( filters ,QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
             foreach(const QString dir, sysList) {
                 const QFileInfo btFile(sysPath + dir+"/address");
                 if(btFile.exists()) {
                     return true;
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
             const QString sysPath = "/sys/class/video4linux/";
             const QDir sysDir(sysPath);
             QStringList filters;
             filters << "*";
             QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
             foreach(const QString dir, sysList) {
                if (dir.contains("radio")) {
                    featureSupported = true;
                }
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
                 const QStringList halDevices = halIface.getAllDevices();
                 foreach(const QString device, halDevices) {
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
 #if !defined(QT_NO_DBUS)
         if(hasHalDeviceFeature("vibrator") || hasHalDeviceFeature("vib")) {
             return true;
     }
#endif
         break;
     case QSystemInfo::WlanFeature :
         {
 #if !defined(QT_NO_DBUS)
             QHalInterface iface;
             if (iface.isValid()) {
                 const QStringList list = iface.findDeviceByCapability("net.80211");
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
         break;
     case QSystemInfo::LocationFeature :
 #if !defined(QT_NO_DBUS)
         featureSupported = hasHalDeviceFeature("gps"); //might not always be true
         if(featureSupported)
             return featureSupported;

 #endif
         break;
     case QSystemInfo::VideoOutFeature :
         {
             const QString sysPath = "/sys/class/video4linux/";
             const QDir sysDir(sysPath);
             QStringList filters;
             filters << "*";
             const QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
             if(sysList.contains("video")) {
                 featureSupported = true;
             }
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

 #if !defined(QT_NO_DBUS)
 bool QSystemInfoLinuxCommonPrivate::hasHalDeviceFeature(const QString &param)
 {
     QHalInterface halIface;
     const QStringList halDevices = halIface.getAllDevices();
     foreach(const QString device, halDevices) {
         if(device.contains(param)) {
             return true;
         }
     }
     return false;
 }

 bool QSystemInfoLinuxCommonPrivate::hasHalUsbFeature(qint32 usbClass)
 {
     QHalInterface halIface;
     const QStringList halDevices = halIface.getAllDevices();
     foreach(const QString device, halDevices) {
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

QString QSystemInfoLinuxCommonPrivate::version(QSystemInfo::Version type,
                                               const QString &parameter)
{
    Q_UNUSED(parameter);
    QString errorStr = QLatin1String("Not Available");

    switch(type) {
        case QSystemInfo::Os :
        {
#if !defined(QT_NO_DBUS)
            QHalDeviceInterface iface(QLatin1String("/org/freedesktop/Hal/devices/computer"));
            QString str;
            if (iface.isValid()) {
                str = iface.getPropertyString(QLatin1String("system.kernel.version"));
                if(!str.isEmpty()) {
                    return str;
                }
            }
#endif
            const QString versionPath = QLatin1String("/proc/version");
            QFile versionFile(versionPath);
            if(!versionFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << "File not opened";
            } else {
                QString  strvalue;
                strvalue = QLatin1String(versionFile.readAll().trimmed());
                strvalue = strvalue.split(QLatin1String(" ")).at(2);
                versionFile.close();
                return strvalue;
            }
            break;
        }
        case QSystemInfo::QtCore :
            return QLatin1String(qVersion());
            break;
        default:
            break;
    };
    return errorStr;
}

QString QSystemInfoLinuxCommonPrivate::currentCountryCode() const
{
    return QLocale::system().name().mid(3,2);
}

bool QSystemInfoLinuxCommonPrivate::hasSysFeature(const QString &featureStr)
{
    const QString sysPath = QLatin1String("/sys/class/");
    const QDir sysDir(sysPath);
    QStringList filters;
    filters << QLatin1String("*");
    const QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
    foreach(const QString dir, sysList) {
        const QDir sysDir2(sysPath + dir);
        if(dir.contains(featureStr)) {
            const QStringList sysList2 = sysDir2.entryList( filters ,QDir::Dirs, QDir::Name);
            if(!sysList2.isEmpty()) {
                return true;
            }
        }
    }
    return false;
}

QSystemNetworkInfoLinuxCommonPrivate::QSystemNetworkInfoLinuxCommonPrivate(QObject *parent) : QObject(parent)
{
}

QSystemNetworkInfoLinuxCommonPrivate::~QSystemNetworkInfoLinuxCommonPrivate()
{
}
QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoLinuxCommonPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            const QString baseSysDir = "/sys/class/net/";
            const QDir wDir(baseSysDir);
            const QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(const QString dir, dirs) {
                const QString devFile = baseSysDir + dir;
                const QFileInfo wiFi(devFile + "/wireless");
                const QFileInfo fi("/proc/net/route");
                if(wiFi.exists() && fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        const QString result = rx.readAll();
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
            const QString baseSysDir = "/sys/class/net/";
            const QDir eDir(baseSysDir);
            const QString dir = QSystemNetworkInfoLinuxCommonPrivate::interfaceForMode(mode).name();

            const QString devFile = baseSysDir + dir;
            const QFileInfo fi("/proc/net/route");
            if(fi.exists()) {
                QFile rx(fi.absoluteFilePath());
                if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    const QString result = rx.readAll();
                    if(result.contains(dir)) {
                        return QSystemNetworkInfo::Connected;
                    } else {
                        return QSystemNetworkInfo::NoNetworkAvailable;
                    }
                }
            }
        }
        break;
        case QSystemNetworkInfo::BluetoothMode:
        {
            return getBluetoothNetStatus();
       }
        break;
    default:
        break;
    };
    return QSystemNetworkInfo::UndefinedStatus;
}

QString QSystemNetworkInfoLinuxCommonPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    QString netname = "";

    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            if(networkStatus(mode) != QSystemNetworkInfo::Connected) {
                return netname;
            }

            QString wlanInterface;
            const QString baseSysDir = "/sys/class/net/";
            const QDir wDir(baseSysDir);
            const QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(const QString dir, dirs) {
                const QString devFile = baseSysDir + dir;
                const QFileInfo fi(devFile + "/wireless");
                if(fi.exists()) {
                    wlanInterface = dir;
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
                qDebug() << "no socket";
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

QString QSystemNetworkInfoLinuxCommonPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
        case QSystemNetworkInfo::WlanMode:
        {
            QString result;
            const QString baseSysDir = "/sys/class/net/";
            const QDir wDir(baseSysDir);
            const QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(const QString dir, dirs) {
                const QString devFile = baseSysDir + dir;
                const QFileInfo fi(devFile + "/wireless");
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
            const QString baseSysDir = "/sys/class/net/";
            const QDir eDir(baseSysDir);
            const QStringList dirs = eDir.entryList(QStringList() << "eth*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(const QString dir, dirs) {
                const QString devFile = baseSysDir + dir;
                const QFileInfo fi(devFile + "/address");
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

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoLinuxCommonPrivate::getBluetoothNetStatus()
{
#ifdef BLUEZ_SUPPORTED
    int ctl = socket(PF_BLUETOOTH,SOCK_RAW,BTPROTO_BNEP);
    if (ctl < 0) {
        qDebug() << "Cannot open bnep socket";
        return QSystemNetworkInfo::UndefinedStatus;
    }

    struct bnep_conninfo info[36];
    struct bnep_connlist_req req;

    req.ci = info;
    req.cnum = 36;

    if (ioctl(ctl,BNEPGETCONNLIST,&req) < 0) {
        qDebug() << "Cannot get bnep connection list.";
        return QSystemNetworkInfo::UndefinedStatus;
    }
    for (uint j = 0; j< req.cnum; j++) {
        if(info[j].state == BT_CONNECTED) {
            return QSystemNetworkInfo::Connected;
        }
    }
    close(ctl);
#endif

    return QSystemNetworkInfo::UndefinedStatus;
}

qint32 QSystemNetworkInfoLinuxCommonPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            QString result;
            const QString baseSysDir = "/sys/class/net/";
            const QDir wDir(baseSysDir);
            const QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(const QString dir, dirs) {
                const QString devFile = baseSysDir + dir;
                const QFileInfo fi(devFile + "/wireless/link");
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
            const QString baseSysDir = "/sys/class/net/";
            const QDir eDir(baseSysDir);
            const QStringList dirs = eDir.entryList(QStringList() << "eth*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(const QString dir, dirs) {
                const QString devFile = baseSysDir + dir;
                const QFileInfo fi(devFile + "/carrier");
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
    default:
        break;
    };

    return -1;
}

QNetworkInterface QSystemNetworkInfoLinuxCommonPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
#if !defined(QT_NO_DBUS)
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            QHalInterface iface;
            if (iface.isValid()) {
                const QStringList list = iface.findDeviceByCapability("net.80211");
                if(!list.isEmpty()) {
                    foreach(const QString netDev, list) {
                        QHalDeviceInterface ifaceDevice(netDev);
                        const QString deviceName  = ifaceDevice.getPropertyString("net.interface");
                        if(list.count() > 1) {
                            const QString baseFIle = "/sys/class/net/" + deviceName+"/operstate";
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
                const QStringList list = iface.findDeviceByCapability("net.80203");
                if(!list.isEmpty()) {
                    foreach(const QString netDev, list) {
                        QHalDeviceInterface ifaceDevice(netDev);
                        const QString deviceName  = ifaceDevice.getPropertyString("net.interface");
                        if(list.count() > 1) {
                            const QString baseFIle = "/sys/class/net/" + deviceName+"/operstate";
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
    const QString baseSysDir = "/sys/class/net/";
    const QDir eDir(baseSysDir);
    const QStringList dirs = eDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach(const QString dir, dirs) {
        const QString devFile = baseSysDir + dir;
        const QFileInfo devfi(devFile + "/device");
        if(!devfi.exists()) {
            continue;
        }
        const QString baseFIle = "/sys/class/net/" + devFile+"/operstate";
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
                const QFileInfo fi(devFile + "/wireless");
                if(fi.exists()) {
                    return QNetworkInterface::interfaceFromName(dir);
                }
            }
            break;
            case QSystemNetworkInfo::EthernetMode:
            {
                const QFileInfo fi(devFile + "/wireless");
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
bool QSystemNetworkInfoLinuxCommonPrivate::isDefaultInterface(const QString &deviceName)
{
    QFile routeFilex("/proc/net/route");
    if(routeFilex.exists() && routeFilex.open(QIODevice::ReadOnly
                                              | QIODevice::Text)) {
        QTextStream rin(&routeFilex);
        QString line = rin.readLine();
        while (!line.isNull()) {
            const QString lineSection = line.section("\t",2,2,QString::SectionSkipEmpty);
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

int QSystemNetworkInfoLinuxCommonPrivate::getBluetoothRssi()
{
    return 0;
}

QString QSystemNetworkInfoLinuxCommonPrivate::getBluetoothInfo(const QString &file)
{
    const QString sysPath = "/sys/class/bluetooth/";
    const QDir sysDir(sysPath);
    QStringList filters;
    filters << "*";
    const QStringList sysList = sysDir.entryList( filters ,QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    foreach(const QString dir, sysList) {
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

QSystemDisplayInfoLinuxCommonPrivate::QSystemDisplayInfoLinuxCommonPrivate(QObject *parent) : QObject(parent)
{
    halIsAvailable = halAvailable();
}

QSystemDisplayInfoLinuxCommonPrivate::~QSystemDisplayInfoLinuxCommonPrivate()
{
}

int QSystemDisplayInfoLinuxCommonPrivate::colorDepth(int screen)
{
#ifdef Q_WS_X11
    QDesktopWidget wid;
    return wid.screen(screen)->x11Info().depth();
#else
        return QPixmap::defaultDepth();
#endif
}


int QSystemDisplayInfoLinuxCommonPrivate::displayBrightness(int screen)
{
    Q_UNUSED(screen);
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalInterface iface;
        if (iface.isValid()) {
            const QStringList list = iface.findDeviceByCapability("laptop_panel");
            if(!list.isEmpty()) {
                foreach(const QString lapDev, list) {
                    QHalDeviceInterface ifaceDevice(lapDev);
                    QHalDeviceLaptopPanelInterface lapIface(lapDev);
                    const float numLevels = ifaceDevice.getPropertyInt("laptop_panel.num_levels") - 1;
                    const float curLevel = lapIface.getBrightness();
                    return curLevel / numLevels * 100;
                }
            }
        }
#endif
    } else {
        const QString backlightPath = "/proc/acpi/video/";
        const QDir videoDir(backlightPath);
        QStringList filters;
        filters << "*";
        const QStringList brightnessList = videoDir.entryList(filters,
                                                        QDir::Dirs
                                                        | QDir::NoDotAndDotDot,
                                                        QDir::Name);
        foreach(const QString brightnessFileName, brightnessList) {
            float numLevels = 0.0;
            float curLevel = 0.0;
            QFile curBrightnessFile(backlightPath+brightnessFileName+"/LCD/brightness");
            if(!curBrightnessFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug()<<"File not opened";
            } else {
                const QString strvalue = curBrightnessFile.readAll().trimmed();
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
            qDebug()<<"File not opened";
        } else {
            QString strvalue;
            strvalue = curBrightnessFile.readLine().trimmed();
            curBrightnessFile.close();
            curLevel = strvalue.toFloat();

            QFile maxBrightnessFile(backlightPath+brightnessFileName+"/max_brightness");
            if(!maxBrightnessFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug()<<"File not opened";
            } else {
                QString strvalue;
                strvalue = maxBrightnessFile.readLine().trimmed();
                maxBrightnessFile.close();
                numLevels = strvalue.toFloat();
            }
            return curLevel / numLevels * 100;
        }
    }
#endif
    return -1;
}


QSystemDisplayInfo::DisplayOrientation QSystemDisplayInfoLinuxCommonPrivate::getOrientation(int screen)
{
    QSystemDisplayInfo::DisplayOrientation orientation = QSystemDisplayInfo::Unknown;
    XRRScreenConfiguration *sc;
    Rotation cur_rotation;
    sc = XRRGetScreenInfo(QX11Info::display(), RootWindow(QX11Info::display(), screen));
    if (!sc) {
        return orientation;
    }
    XRRConfigRotations(sc, &cur_rotation);

    if(screen < 16 && screen > -1) {
        switch(cur_rotation) {
        case RR_Rotate_0:
            orientation = QSystemDisplayInfo::Landscape;
            break;
        case RR_Rotate_90:
            orientation = QSystemDisplayInfo::Portrait;
            break;
        case RR_Rotate_180:
            orientation = QSystemDisplayInfo::InvertedLandscape;
            break;
        case RR_Rotate_270:
            orientation = QSystemDisplayInfo::InvertedPortrait;
            break;
        };
    }
    return orientation;
}


float QSystemDisplayInfoLinuxCommonPrivate::contrast(int screen)
{
    Q_UNUSED(screen);

    return 0.0;
}

int QSystemDisplayInfoLinuxCommonPrivate::getDPIWidth(int screen)
{
    int dpi=0;
    if(screen < 16 && screen > -1) {
        dpi = QDesktopWidget().screenGeometry().width() / (physicalWidth(0) / 25.4);
    }
    return dpi;
}

int QSystemDisplayInfoLinuxCommonPrivate::getDPIHeight(int screen)
{
    int dpi=0;
    if(screen < 16 && screen > -1) {
        dpi = QDesktopWidget().screenGeometry().height() / (physicalHeight(0) / 25.4);
    }
    return dpi;
}

int QSystemDisplayInfoLinuxCommonPrivate::physicalHeight(int screen)
{
    int height=0;
    XRRScreenResources *sr;

    sr = XRRGetScreenResources(QX11Info::display(), RootWindow(QX11Info::display(), screen));
    for (int i = 0; i < sr->noutput; ++i) {
        XRROutputInfo *output = XRRGetOutputInfo(QX11Info::display(),sr,sr->outputs[i]);
        if (output->crtc) {
           height = output->mm_height;
        }
        XRRFreeOutputInfo(output);
    }
    XRRFreeScreenResources(sr);
    return height;
}

int QSystemDisplayInfoLinuxCommonPrivate::physicalWidth(int screen)
{
    int width=0;
    XRRScreenResources *sr;

    sr = XRRGetScreenResources(QX11Info::display(), RootWindow(QX11Info::display(), screen));
    for (int i = 0; i < sr->noutput; ++i) {
        XRROutputInfo *output = XRRGetOutputInfo(QX11Info::display(),sr,sr->outputs[i]);
        if (output->crtc) {
           width = output->mm_width;
        }
        XRRFreeOutputInfo(output);
    }
    XRRFreeScreenResources(sr);

    return width;
}

QSystemStorageInfoLinuxCommonPrivate::QSystemStorageInfoLinuxCommonPrivate(QObject *parent)
    : QObject(parent)
{
    halIsAvailable = halAvailable();

#if !defined(QT_NO_DBUS)
    halIface = new QHalInterface();
#endif
}

QSystemStorageInfoLinuxCommonPrivate::~QSystemStorageInfoLinuxCommonPrivate()
{
}

void QSystemStorageInfoLinuxCommonPrivate::connectNotify(const char *signal)
{
    if (QLatin1String(signal) ==
        QLatin1String(QMetaObject::normalizedSignature(SIGNAL(logicalDrivesChanged(bool))))) {
        mtabWatcherA = new QFileSystemWatcher(QStringList() << "/etc/mtab",this);
        connect(mtabWatcherA,SIGNAL(fileChanged(const QString &)),
                this,SLOT(deviceChanged(const QString &)));
    }

    if (QLatin1String(signal) ==
        QLatin1String(QMetaObject::normalizedSignature(SIGNAL(logicalDrivesChanged(bool))))) {
        mtabWatcherB = new QFileSystemWatcher(QStringList() << "/etc/mtab",this);
        connect(mtabWatcherB,SIGNAL(fileChanged(const QString &)),
                this,SLOT(deviceChanged(const QString &)));
    }
}

void QSystemStorageInfoLinuxCommonPrivate::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) ==
        QLatin1String(QMetaObject::normalizedSignature(SIGNAL(storageAdded())))) {
        delete mtabWatcherA;
        mtabWatcherA = 0;

    }
    if (QLatin1String(signal) ==
        QLatin1String(QMetaObject::normalizedSignature(SIGNAL(storageRemoved())))) {
        delete mtabWatcherB;
        mtabWatcherB = 0;
    }
}

void QSystemStorageInfoLinuxCommonPrivate::deviceChanged(const QString &path)
{
    Q_UNUSED(path);
    QMap<QString, QString> mountEntriesMap2 = mountEntriesMap;
    mountEntries();

    if(mountEntriesMap.count() > mountEntriesMap2.count()) {
        delete mtabWatcherA;
        mtabWatcherA = 0;
        mtabWatcherA = new QFileSystemWatcher(QStringList() << "/proc/mounts",this);
        connect(mtabWatcherA,SIGNAL(fileChanged(const QString &)),
                this,SLOT(deviceChanged(const QString &)));
        emit logicalDrivesChanged(true);

    } else if(mountEntriesMap.count() < mountEntriesMap2.count()) {
        delete mtabWatcherB;
        mtabWatcherB = 0;
        mtabWatcherB = new QFileSystemWatcher(QStringList() << "/proc/mounts",this);
        connect(mtabWatcherB,SIGNAL(fileChanged(const QString &)),
                this,SLOT(deviceChanged(const QString &)));

        emit logicalDrivesChanged(false);
    }
}


qint64 QSystemStorageInfoLinuxCommonPrivate::availableDiskSpace(const QString &driveVolume)
{
    if(driveVolume.left(2) == "//") {
        return 0;
    }
    mountEntries();
    struct statfs fs;
    if(statfs(driveVolume.toLatin1(), &fs ) == 0 ) {
                long blockSize = fs.f_bsize;
                long availBlocks = fs.f_bavail;
                return (double)availBlocks * blockSize;
            }
    return 0;
}

qint64 QSystemStorageInfoLinuxCommonPrivate::totalDiskSpace(const QString &driveVolume)
{
    if(driveVolume.left(2) == "//") {
        return 0;
    }
    mountEntries();
    struct statfs fs;
    if(statfs(driveVolume.toLatin1(), &fs ) == 0 ) {
        const long blockSize = fs.f_bsize;
        const long totalBlocks = fs.f_blocks;
        return (double)totalBlocks * blockSize;
    }
    return 0;
}

QSystemStorageInfo::DriveType QSystemStorageInfoLinuxCommonPrivate::typeForDrive(const QString &driveVolume)
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QStringList mountedVol;
        QHalInterface iface;
        const QStringList list = iface.findDeviceByCapability("volume");
        if(!list.isEmpty()) {
            foreach(const QString vol, list) {
                QHalDeviceInterface ifaceDevice(vol);
                if(mountEntriesMap.value(driveVolume) == ifaceDevice.getPropertyString("block.device")) {
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

        if(mountEntriesMap.value(driveVolume).contains("mapper")) {
            struct stat stat_buf;
            stat( mountEntriesMap.value(driveVolume).toLatin1(), &stat_buf);

            dmFile = QString("/sys/block/dm-%1/removable").arg(stat_buf.st_rdev & 0377);

        } else {

            dmFile = mountEntriesMap.value(driveVolume).section("/",2,3);
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
            qDebug() << "Could not open sys file";
        } else {
            QTextStream sysinfo(&file);
            QString line = sysinfo.readAll();
            if(line.contains("1")) {
                return QSystemStorageInfo::RemovableDrive;
            }
        }
    }
    if(driveVolume.left(2) == "//") {
        return QSystemStorageInfo::RemoteDrive;
    }
    return QSystemStorageInfo::InternalDrive;
}

QStringList QSystemStorageInfoLinuxCommonPrivate::logicalDrives()
{
    mountEntries();
    return mountEntriesMap.keys();
}

void QSystemStorageInfoLinuxCommonPrivate::mountEntries()
{
    mountEntriesMap.clear();
    FILE *mntfp = setmntent( _PATH_MOUNTED, "r" );
    mntent *me = getmntent(mntfp);
    bool ok;
    while(me != NULL) {
        struct statfs fs;
        ok = false;
        if(strcmp(me->mnt_type, "cifs") != 0) { //smb has probs with statfs
            if(statfs(me->mnt_dir, &fs ) ==0 ) {
                QString num;
                // weed out a few types
                if ( fs.f_type != 0x01021994 //tmpfs
                     && fs.f_type != 0x9fa0 //procfs
                     && fs.f_type != 0x1cd1 //
                     && fs.f_type != 0x62656572
                     && (unsigned)fs.f_type != 0xabababab // ???
                     && fs.f_type != 0x52654973
                     && fs.f_type != 0x42494e4d
                     && fs.f_type != 0x64626720
                     && fs.f_type != 0x73636673 //securityfs
                     && fs.f_type != 0x65735543 //fusectl
                     && fs.f_type != 0x65735546 // fuse.gvfs-fuse-daemon

                     ) {
                    ok = true;
                }
            }
        } else {
            ok = true;
        }
        if(ok && !mountEntriesMap.keys().contains(me->mnt_fsname)) {
            mountEntriesMap[me->mnt_dir] = me->mnt_fsname;
        }

        me = getmntent(mntfp);
    }
    endmntent(mntfp);
}



QSystemDeviceInfoLinuxCommonPrivate::QSystemDeviceInfoLinuxCommonPrivate(QObject *parent) : QObject(parent)
{
    halIsAvailable = halAvailable();
    setConnection();
 #if !defined(QT_NO_DBUS)
    setupBluetooth();
#endif
}

QSystemDeviceInfoLinuxCommonPrivate::~QSystemDeviceInfoLinuxCommonPrivate()
{
}


void QSystemDeviceInfoLinuxCommonPrivate::setConnection()
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalInterface iface;

        QStringList list = iface.findDeviceByCapability("battery");
        if(!list.isEmpty()) {
            foreach(const QString dev, list) {
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid()) {
                    const QString batType = halIfaceDevice->getPropertyString("battery.type");
                    if(batType == "primary" || batType == "pda") {
                        if(halIfaceDevice->setConnections() ) {
                            if(!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                        this,SLOT(halChanged(int,QVariantList)))) {
                                qDebug() << "connection malfunction";
                            }
                        }
                        break;
                    }
                }
            }
        }

        list = iface.findDeviceByCapability("ac_adapter");
        if(!list.isEmpty()) {
            foreach(const QString dev, list) {
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid()) {
                    if(halIfaceDevice->setConnections() ) {
                        if(!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                    this,SLOT(halChanged(int,QVariantList)))) {
                            qDebug() << "connection malfunction";
                        }
                    }
                    break;
                }
            }
        }

        list = iface.findDeviceByCapability("battery");
        if(!list.isEmpty()) {
            foreach(const QString dev, list) {
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid()) {
                    if(halIfaceDevice->setConnections()) {
                        if(!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                    this,SLOT(halChanged(int,QVariantList)))) {
                            qDebug() << "connection malfunction";
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
void QSystemDeviceInfoLinuxCommonPrivate::halChanged(int,QVariantList map)
{
    for(int i=0; i < map.count(); i++) {
       if(map.at(i).toString() == "battery.charge_level.percentage") {
            const int level = batteryLevel();
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

QString QSystemDeviceInfoLinuxCommonPrivate::manufacturer()
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
        QFile file("/proc/cpuinfo");
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Could not open /proc/cpuinfo";
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

QString QSystemDeviceInfoLinuxCommonPrivate::model()
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
        qDebug() << "Could not open /proc/cpuinfo";
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

QString QSystemDeviceInfoLinuxCommonPrivate::productName()
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalDeviceInterface iface("/org/freedesktop/Hal/devices/computer");
        QString productName;
        if (iface.isValid()) {
            productName = iface.getPropertyString("info.product");
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
    const QDir dir("/etc");
    if(dir.exists()) {
        QStringList langList;
        QFileInfoList localeList = dir.entryInfoList(QStringList() << "*release",
                                                     QDir::Files | QDir::NoDotAndDotDot,
                                                     QDir::Name);
        foreach(const QFileInfo fileInfo, localeList) {
            const QString filepath = fileInfo.filePath();
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
        qDebug() << "Could not open /proc/cpuinfo";
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

QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfoLinuxCommonPrivate::inputMethodType()
{
    QSystemDeviceInfo::InputMethodFlags methods = 0;
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalInterface iface2;
        if (iface2.isValid()) {
            QStringList capList;
            capList << QLatin1String("input.keyboard")
                    << QLatin1String("input.keys")
                    << QLatin1String("input.keypad")
                    << QLatin1String("input.mouse")
                    << QLatin1String("input.tablet")
                    << QLatin1String("input.touchpad");
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
                        methods = (methods | QSystemDeviceInfo::SingleTouch);
                        break;
                    case 5:
                        methods = (methods | QSystemDeviceInfo::SingleTouch);
                        break;
                    }
                }
            }
            if(methods != 0)
                return methods;
        }
#endif
    }
    const QString inputsPath = "/sys/class/input/";
    const QDir inputDir(inputsPath);
    QStringList filters;
    filters << "event*";
    const QStringList inputList = inputDir.entryList( filters ,QDir::Dirs, QDir::Name);
    foreach(const QString inputFileName, inputList) {
        QFile file(inputsPath+inputFileName+"/device/name");
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug()<<"File not opened";
        } else {
            QString strvalue;
            strvalue = file.readLine();
            file.close();
            if(strvalue.contains("keyboard",Qt::CaseInsensitive)) {
                if( (methods & QSystemDeviceInfo::Keyboard) != QSystemDeviceInfo::Keyboard) {
                    methods = (methods | QSystemDeviceInfo::Keyboard);
                }
            } else if(strvalue.contains("Mouse",Qt::CaseInsensitive)) {
                if( (methods & QSystemDeviceInfo::Mouse) != QSystemDeviceInfo::Mouse) {
                    methods = (methods | QSystemDeviceInfo::Mouse);
                }
            } else if(strvalue.contains("Button",Qt::CaseInsensitive)) {
                if( (methods & QSystemDeviceInfo::Keys) != QSystemDeviceInfo::Keys) {
                    methods = (methods | QSystemDeviceInfo::Keypad);
                }
            } else if(strvalue.contains("keypad",Qt::CaseInsensitive)) {
                if( (methods & QSystemDeviceInfo::Keypad) != QSystemDeviceInfo::Keypad) {
                    methods = (methods | QSystemDeviceInfo::Keys);
                }
            } else if(strvalue.contains("TouchScreen",Qt::CaseInsensitive)) {
                if( (methods & QSystemDeviceInfo::SingleTouch) != QSystemDeviceInfo::SingleTouch) {
                    methods = (methods | QSystemDeviceInfo::SingleTouch);
                }
            }
        }
    }
    return methods;
}

int QSystemDeviceInfoLinuxCommonPrivate::batteryLevel() const
{
    float levelWhenFull = 0.0;
    float level = 0.0;
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalInterface iface;
        const QStringList list = iface.findDeviceByCapability("battery");
        if(!list.isEmpty()) {
            foreach(const QString dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (ifaceDevice.isValid()) {
                    if(!ifaceDevice.getPropertyBool("battery.present")
                        && (ifaceDevice.getPropertyString("battery.type") != "pda"
                             || ifaceDevice.getPropertyString("battery.type") != "primary")) {
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
            return QSystemDeviceInfo::NoBatteryLevel;
        } else {
            QTextStream batinfo(&infofile);
            QString line = batinfo.readLine();
            while (!line.isNull()) {
                if(line.contains("design capacity")) {
                    levelWhenFull = line.split(" ").at(1).trimmed().toFloat();
                    infofile.close();
                    break;
                }
                line = batinfo.readLine();
            }
            infofile.close();
        }

        QFile statefile("/proc/acpi/battery/BAT0/state");
        if (!statefile.open(QIODevice::ReadOnly)) {
            return QSystemDeviceInfo::NoBatteryLevel;
        } else {
            QTextStream batstate(&statefile);
            QString line = batstate.readLine();
            while (!line.isNull()) {
                if(line.contains("remaining capacity")) {
                    level = line.split(" ").at(1).trimmed().toFloat();
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

QSystemDeviceInfo::PowerState QSystemDeviceInfoLinuxCommonPrivate::currentPowerState()
{
#if !defined(QT_NO_DBUS)
       QHalInterface iface;
       QStringList list = iface.findDeviceByCapability("battery");
       if(!list.isEmpty()) {
           foreach(const QString dev, list) {
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
           foreach(const QString dev, list) {
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
 void QSystemDeviceInfoLinuxCommonPrivate::setupBluetooth()
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
                     qDebug() << "bluez could not connect signal";
                 }
             }
         }
     }
 }

 void QSystemDeviceInfoLinuxCommonPrivate::bluezPropertyChanged(const QString &str, QDBusVariant v)
  {
     if(str == "Powered") {
          emit bluetoothStateChanged(v.variant().toBool());
      }
      // Pairable Name Class Discoverable
  }
 #endif

 bool QSystemDeviceInfoLinuxCommonPrivate::currentBluetoothPowerState()
 {
     return false;
 }

QSystemScreenSaverLinuxCommonPrivate::QSystemScreenSaverLinuxCommonPrivate(QObject *parent) : QObject(parent)
{
}

QSystemScreenSaverLinuxCommonPrivate::~QSystemScreenSaverLinuxCommonPrivate()
{
}


#include "moc_qsysteminfo_linux_common_p.cpp"

QTM_END_NAMESPACE
