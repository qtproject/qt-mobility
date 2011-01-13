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
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5)
#include "qdevicekitservice_linux_p.h"
#endif
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QCryptographicHash>
#include <QVariantMap>

#if !defined(QT_NO_DBUS)
#include "qhalservice_linux_p.h"
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

#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MEEGO)
#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#endif
#endif

#if defined(BLKID_SUPPORTED)
#include <blkid/blkid.h>
#include <linux/fs.h>
#endif

#ifdef BLUEZ_SUPPORTED
# include <bluetooth/bluetooth.h>
# include <bluetooth/bnep.h>
#endif
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/inotify.h>

#include <linux/fb.h>
#include <fcntl.h>

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

#if defined(UDEV_SUPPORTED)
#include "qudevservice_linux_p.h"
#endif
#include "qsysteminfo_dbus_p.h"

static QString sysinfodValueForKey(const QString& key)
{
    QString value = "";
#if !defined(QT_NO_DBUS)
    QDBusInterface connectionInterface("com.nokia.SystemInfo",
                                       "/com/nokia/SystemInfo",
                                       "com.nokia.SystemInfo",
                                       QDBusConnection::systemBus());
    QDBusReply<QByteArray> reply = connectionInterface.call("GetConfigValue", key);
    if (reply.isValid()) {
        /*
         * sysinfod automatically terminates after some idle time (no D-Bus traffic).
         * Therefore, we cannot use isServiceRegistered() to determine if sysinfod is available.
         *
         * Thus, make a query to sysinfod and if we got back a valid reply, sysinfod
         * is available.
         */
        value = reply.value();
    }
#endif
    return value;
}

//#endif

bool halIsAvailable;
bool udisksIsAvailable;
bool connmanIsAvailable;
bool ofonoIsAvailable;
bool uPowerIsAvailable;

static bool btHasPower() {
#if !defined(QT_NO_DBUS)
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
                 QDBusReply<QVariantMap > reply =  adapterInterface->call(QLatin1String("GetProperties"));
                 QVariant var;
                 QString property="Powered";
                 QVariantMap map = reply.value();
                 if (map.contains(property)) {
                     return map.value(property ).toBool();
                 }
             }
         }
     }
#endif
     return false;
}

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
    if (langCached.isEmpty()) {
        currentLanguage();
    }
    checkLang = checkLang.left(2);
    if (checkLang != langCached) {
        emit currentLanguageChanged(checkLang);
        langCached = checkLang;
    }
    langTimer = new QTimer(this);
    QTimer::singleShot(5000, this, SLOT(startLanguagePolling()));
}

QString QSystemInfoLinuxCommonPrivate::currentLanguage() const
{
    QString lang;
    if (langCached.isEmpty()) {
        lang  = QLocale::system().name().left(2);
        if (lang.isEmpty() || lang == QLatin1String("C")) {
            lang = QLatin1String("en");
        }
    } else {
        lang = langCached;
    }
    return lang;
}

bool QSystemInfoLinuxCommonPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && defined(UDEV_SUPPORTED)
     QUdevService udevService;
#endif
     bool featureSupported = false;
     switch (feature) {
     case QSystemInfo::BluetoothFeature :
         {
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && defined(UDEV_SUPPORTED)
             return udevService.isSubsystemAvailable(UDEV_SUBSYSTEM_BLUETOOTH);
#endif
             const QString sysPath = "/sys/class/bluetooth/";
             const QDir sysDir(sysPath);
             QStringList filters;
             filters << "*";
             const QStringList sysList = sysDir.entryList( filters ,QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
             foreach (const QString &dir, sysList) {
                 const QFileInfo btFile(sysPath + dir+"/address");
                 if (btFile.exists()) {
                     return true;
                 }
             }
         }
     break;
     case QSystemInfo::CameraFeature :
         {
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && defined(UDEV_SUPPORTED)
             return udevService.isPropertyAvailable(UDEV_PROPERTY_V4L_CAP, "*:capture:*");
#endif
 #if !defined(QT_NO_DBUS)
             featureSupported = hasHalUsbFeature(0x06); // image
             if (featureSupported)
                 return featureSupported;
 #endif
             featureSupported = hasSysFeature("video");
         }
         break;
     case QSystemInfo::FmradioFeature :
         {
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && defined(UDEV_SUPPORTED)
             return udevService.isPropertyAvailable(UDEV_PROPERTY_V4L_CAP, "*:radio:*");
#endif
             const QString sysPath = "/sys/class/video4linux/";
             const QDir sysDir(sysPath);
             QStringList filters;
             filters << "*";
             QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
             foreach (const QString &dir, sysList) {
                if (dir.contains("radio")) {
                    featureSupported = true;
                }
            }
         }
         break;
     case QSystemInfo::IrFeature :
         {
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && defined(UDEV_SUPPORTED)
         return udevService.isPropertyAvailable(UDEV_PROPERTY_DRIVER, "*irda*");
#endif
 #if !defined(QT_NO_DBUS)
         featureSupported = hasHalUsbFeature(0xFE);
         if (featureSupported)
             return featureSupported;
 #endif
         featureSupported = hasSysFeature("irda"); //?
     }
         break;
     case QSystemInfo::LedFeature :
         {
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && defined(UDEV_SUPPORTED)
             return udevService.isSubsystemAvailable(UDEV_SUBSYSTEM_LEDS);
#endif
             featureSupported = hasSysFeature("led"); //?
         }
         break;
     case QSystemInfo::MemcardFeature :
         {
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && defined(UDEV_SUPPORTED)
             return udevService.isSubsystemAvailable(UDEV_SUBSYSTEM_MEMCARD);
#endif
 #if !defined(QT_NO_DBUS)
             QHalInterface iface;
             if (iface.isValid()) {
                 QHalInterface halIface;
                 const QStringList halDevices = halIface.findDeviceByCapability("mmc_host");
                 foreach (const QString &device, halDevices) {
                     QHalDeviceInterface ifaceDevice(device);
                     if (ifaceDevice.isValid()) {
                         if (ifaceDevice.getPropertyString("info.subsystem") == "mmc_host") {
                             return true;
                         }
                         if (ifaceDevice.getPropertyBool("storage.removable")) {
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
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && defined(UDEV_SUPPORTED)
             if (udevService.isPropertyAvailable(UDEV_PROPERTY_DRIVER, UDEV_DRIVER_MUSB))
                 return true;
             if(udevService.isPropertyAvailable(UDEV_PROPERTY_DRIVER, UDEV_DRIVER_USB))
                 return true;
             if(udevService.isPropertyAvailable(UDEV_PROPERTY_INTERFACE, "usb*"))
                 return true;
#endif
 #if !defined(QT_NO_DBUS)
         featureSupported = hasHalDeviceFeature("usb");
         if (featureSupported)
             return featureSupported;
 #endif
             featureSupported = hasSysFeature("usb_host");
         }
         break;
     case QSystemInfo::VibFeature :
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && defined(UDEV_SUPPORTED)
         if (udevService.isPropertyAvailable(UDEV_PROPERTY_NAME, "*vibra*"))
            return true;
         if(udevService.isPropertyAvailable(UDEV_PROPERTY_NAME, "*Vibra*"))
         return true;
#endif
 #if !defined(QT_NO_DBUS)
         if (hasHalDeviceFeature("vibrator") || hasHalDeviceFeature("vib")) {
             return true;
     }
#endif
         break;
     case QSystemInfo::WlanFeature :
         {
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && defined(UDEV_SUPPORTED)
             return udevService.isSubsystemAvailable(UDEV_SUBSYSTEM_WLAN);
#endif
 #if !defined(QT_NO_DBUS)
             QHalInterface iface;
             if (iface.isValid()) {
                 const QStringList list = iface.findDeviceByCapability("net.80211");
                 if (!list.isEmpty()) {
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
#if !defined(QT_NO_CONNMAN)
 if (ofonoAvailable()) {
     QOfonoManagerInterface ofonoManager(this);
     QString modempath = ofonoManager.currentModem().path();
     if (!modempath.isEmpty()) {
         QOfonoSimInterface simInterface(modempath,this);
         return simInterface.isValid();
     }
 }
#endif

         }
         break;
     case QSystemInfo::LocationFeature :
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && defined(UDEV_SUPPORTED)
             return udevService.isPropertyAvailable(UDEV_PROPERTY_DRIVER, "*gps*");
#endif
 #if !defined(QT_NO_DBUS)
         featureSupported = hasHalDeviceFeature("gps"); //might not always be true
         if (featureSupported)
             return featureSupported;

 #endif
         break;
     case QSystemInfo::VideoOutFeature :
         {
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && defined(UDEV_SUPPORTED)
             return udevService.isPropertyAvailable(UDEV_PROPERTY_V4L_CAP, "*:video_output:*");
#endif
             const QString sysPath = "/sys/class/video4linux/";
             const QDir sysDir(sysPath);
             QStringList filters;
             filters << "*";
             const QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
             if (sysList.contains("video")) {
                 featureSupported = true;
             }
         }
         break;
     case QSystemInfo::HapticsFeature:

         break;
     case QSystemInfo::FmTransmitterFeature:
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
     foreach (const QString &device, halDevices) {
         if (device.contains(param)) {
             return true;
         }
     }
     return false;
 }

 bool QSystemInfoLinuxCommonPrivate::hasHalUsbFeature(qint32 usbClass)
 {
     QHalInterface halIface;
      const QStringList halDevices = halIface.findDeviceByCapability("usb_device");
      foreach (const QString &device, halDevices) {
         QHalDeviceInterface ifaceDevice(device);
         if (ifaceDevice.isValid()) {
             if (ifaceDevice.getPropertyString("info.subsystem") == "usb_device") {
                 if (ifaceDevice.getPropertyInt("usb.interface.class") == usbClass) {
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
     QString errorStr = QLatin1String("Not Available");

     bool useDate = false;
     if (parameter == QLatin1String("versionDate")) {
         useDate = true;
     }

     switch(type) {
     case QSystemInfo::Firmware :
     {
#if !defined(QT_NO_DBUS)
         QString sysinfodValue = sysinfodValueForKey("/device/sw-release-ver");
         if (!sysinfodValue.isEmpty()) {
             return sysinfodValue;
         }
         QHalDeviceInterface iface(QLatin1String("/org/freedesktop/Hal/devices/computer"));
         QString str;
         if (iface.isValid()) {
             str = iface.getPropertyString(QLatin1String("system.kernel.version"));
             if (!str.isEmpty()) {
                 return str;
             }
             if (useDate) {
                 str = iface.getPropertyString(QLatin1String("system.firmware.release_date"));
                 if (!str.isEmpty()) {
                     return str;
                 }
             } else {
                 str = iface.getPropertyString(QLatin1String("system.firmware.version"));
                 if (str.isEmpty()) {
                     if (!str.isEmpty()) {
                         return str;
                     }
                 }
             }
         }
         break;
#endif
     }
     case QSystemInfo::Os :
    {

        if(QFileInfo("/usr/bin/lsb_release").exists()) {
            QProcess syscall;
            QString program = "/usr/bin/lsb_release";
            QStringList arguments;
            arguments << "-d";
            syscall.start(program, arguments);
            syscall.waitForFinished();
            QString desc = syscall.readAllStandardOutput();
            desc = desc.section(":",1,1);
            return desc.simplified();
        }

        QFile versionFile2(QLatin1String("/etc/issue"));
        if(!versionFile2.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "File not opened";
        } else {
            QString line;
            QString strvalue;
            QTextStream in(&versionFile2);
            do {
                line = in.readLine();
                line.remove("\\n");
                line.remove("\\l");
                strvalue = line.simplified();
                break;
            } while (!line.isNull());
            versionFile2.close();
            if(!strvalue.isEmpty()) {
                return strvalue;
            }
        }

        QFile versionFile3(QLatin1String("/proc/version"));
        if(!versionFile3.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "File not opened";
        } else {
            QString  strvalue;
            strvalue = QLatin1String(versionFile3.readAll().trimmed());
            versionFile3.close();
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
    foreach (const QString &dir, sysList) {
        const QDir sysDir2(sysPath + dir);
        if (dir.contains(featureStr)) {
            const QStringList sysList2 = sysDir2.entryList( filters ,QDir::Dirs, QDir::Name);
            if (!sysList2.isEmpty()) {
                return true;
            }
        }
    }
    return false;
}

QSystemNetworkInfoLinuxCommonPrivate::QSystemNetworkInfoLinuxCommonPrivate(QObject *parent)
    : QObject(parent)
{
#if !defined(QT_NO_DBUS)
#if !defined(QT_NO_CONNMAN)
    connmanIsAvailable = connmanAvailable();
    if (connmanIsAvailable) {
        initConnman();
    }
    ofonoIsAvailable = ofonoAvailable();
    if (ofonoIsAvailable) {
        initOfono();
    }
#endif
#if !defined(QT_NO_UDISKS)
    uPowerIsAvailable = uPowerAvailable();
#endif
#endif
}

QSystemNetworkInfoLinuxCommonPrivate::~QSystemNetworkInfoLinuxCommonPrivate()
{
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoLinuxCommonPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
#if !defined(QT_NO_CONNMAN)

//    QSystemNetworkInfo::UndefinedStatus
//    QSystemNetworkInfo::NoNetworkAvailable
//    QSystemNetworkInfo::EmergencyOnly
//    QSystemNetworkInfo::Searching
//    QSystemNetworkInfo::Busy
//    QSystemNetworkInfo::Connected
//    QSystemNetworkInfo::HomeNetwork
//    QSystemNetworkInfo::Denied
//    QSystemNetworkInfo::Roaming
    if (connmanIsAvailable) {

        QDBusObjectPath path = connmanManager->lookupService(interfaceForMode(mode).name());
        if (!path.path().isEmpty()) {
            QConnmanServiceInterface serviceIface(path.path(),this);

            if (mode == QSystemNetworkInfo::GsmMode ||
               mode == QSystemNetworkInfo::CdmaMode ||
               mode == QSystemNetworkInfo::WcdmaMode) {
                return getOfonoStatus(mode);
            }
            //        if (serviceIface.isRoaming()) {
            //            return QSystemNetworkInfo::Roaming;
            //        }
            if (serviceIface.isValid()) {
                return stateToStatus(serviceIface.getState());
            }
        }
        return QSystemNetworkInfo::UndefinedStatus;
        //   }
    }
#else
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            const QString baseSysDir = "/sys/class/net/";
            const QDir wDir(baseSysDir);
            const QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach (const QString &dir, dirs) {
                const QString devFile = baseSysDir + dir;
                const QFileInfo wiFi(devFile + "/phy80211");
                const QFileInfo fi("/proc/net/route");
                if (wiFi.exists() && fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if (rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        const QString result = rx.readAll();
                        if (result.contains(dir)) {
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
            if (fi.exists()) {
                QFile rx(fi.absoluteFilePath());
                if (rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    const QString result = rx.readAll();
                    if (result.contains(dir)) {
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
#endif
    return QSystemNetworkInfo::UndefinedStatus;
}

#if !defined(QT_NO_CONNMAN)
QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoLinuxCommonPrivate::getOfonoStatus(QSystemNetworkInfo::NetworkMode mode)
{
    QSystemNetworkInfo::NetworkStatus networkStatus = QSystemNetworkInfo::UndefinedStatus;
    if (ofonoIsAvailable) {
        QString modempath = ofonoManager->currentModem().path();
        if (!modempath.isEmpty()) {

            QOfonoNetworkRegistrationInterface ofonoNetwork(modempath,this);
            if (ofonoNetwork.isValid()) {
                foreach (const QDBusObjectPath &op,ofonoNetwork.getOperators()) {
                    if (!op.path().isEmpty()) {
                        QOfonoNetworkOperatorInterface opIface(op.path(),this);

                        foreach (const QString &opTech, opIface.getTechnologies()) {
                            if (mode == ofonoTechToMode(opTech)) {
                                networkStatus = ofonoStatusToStatus(ofonoNetwork.getStatus());
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    // Busy
    return networkStatus;
}
#endif

QString QSystemNetworkInfoLinuxCommonPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    QString netname = "";
    if (networkStatus(mode) != QSystemNetworkInfo::Connected
       && networkStatus(mode) != QSystemNetworkInfo::Roaming
       && networkStatus(mode) != QSystemNetworkInfo::HomeNetwork) {
        return netname;
    }
#if !defined(QT_NO_CONNMAN)
    QString tech = modeToTechnology(mode);
    if (tech.contains("cellular")) {
        QString modempath = ofonoManager->currentModem().path();
        if (!modempath.isEmpty()) {
            QOfonoNetworkRegistrationInterface ofonoOpNetwork(modempath,this);
            if (ofonoOpNetwork.isValid()) {
                return ofonoOpNetwork.getOperatorName();
            }
        }
    } else {
        QDBusObjectPath path = connmanManager->lookupService(tech);
        if (!path.path().isEmpty()) {
            QConnmanServiceInterface service(path.path(),this);
            if (service.isValid()) {
                netname = service.getName();
            }
        }
    }
#else
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            if (networkStatus(mode) != QSystemNetworkInfo::Connected) {
                return netname;
            }

            QString wlanInterface;
            const QString baseSysDir = "/sys/class/net/";
            const QDir wDir(baseSysDir);
            const QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach (const QString &dir, dirs) {
                const QString devFile = baseSysDir + dir;
                const QFileInfo fi(devFile + "/phy80211");
                if (fi.exists()) {
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
            if (resFile.exists()) {
                if (resFile.exists() && resFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QString line;
                    QTextStream in(&resFile);
                    do {
                        line = in.readLine();
                        if (line.contains("domain")) {
                            netname = line.section(" ",1,1); //guessing here
                        }
                } while (!line.isNull());
                resFile.close();
            }
        }
            if (netname.isEmpty()) {
             netname = "Wired";
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
#endif
    return netname;
}

QString QSystemNetworkInfoLinuxCommonPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
#if !defined(QT_NO_CONNMAN)
    if (connmanIsAvailable) {
        foreach (const QString &servicePath, connmanManager->getServices()) {
            QConnmanServiceInterface *serviceIface;
            serviceIface = new QConnmanServiceInterface(servicePath,this);
            if(serviceIface->getType() == modeToTechnology(mode)) {
                return QNetworkInterface::interfaceFromName(serviceIface->getInterface()).hardwareAddress();
            }
        }
    }
#else
    switch(mode) {
        case QSystemNetworkInfo::WlanMode:
        {
            QString result;
            const QString baseSysDir = "/sys/class/net/";
            const QDir wDir(baseSysDir);
            const QStringList dirs = wDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach (const QString &dir, dirs) {
                const QString devFile = baseSysDir + dir;
                const QFileInfo fi(devFile + "/phy80211");
                if (fi.exists()) {
                    bool powered=false;
                    QFile linkmode(devFile+"/link_mode"); //check for dev power
                    if (linkmode.exists() && linkmode.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&linkmode);
                        in >> result;
                        if (result.contains("1"))
                            powered = true;
                        linkmode.close();
                    }

                    QFile rx(devFile + "/address");
                    if (rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QTextStream in(&rx);
                        in >> result;
                        rx.close();
                        if (powered)
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
            const QStringList dirs = eDir.entryList(QStringList() << "eth*", QDir::Dirs | QDir::NoDotAndDotDot);
            foreach(const QString dir, dirs) {
                const QString devFile = baseSysDir + dir;
                const QFileInfo fi(devFile + "/address");
                if (fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if (rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
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
#endif
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
        if (info[j].state == BT_CONNECTED) {
            return QSystemNetworkInfo::Connected;
        }
    }
    close(ctl);
#endif

    return QSystemNetworkInfo::UndefinedStatus;
}

qint32 QSystemNetworkInfoLinuxCommonPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
#if !defined(QT_NO_CONNMAN)
    if (connmanIsAvailable) {
        qint32 sig=0;
        QString tech = modeToTechnology(mode);
        if (ofonoIsAvailable && tech.contains("cellular")) {

            QString modempath = ofonoManager->currentModem().path();
            if (!modempath.isEmpty()) {
                QOfonoNetworkRegistrationInterface ofonoNetwork(modempath,this);
                if (ofonoNetwork.isValid()) {
                    return ofonoNetwork.getSignalStrength();
                }
            }
        } else {
            QDBusObjectPath path = connmanManager->lookupService(tech);
            if (!path.path().isEmpty()) {
                QConnmanServiceInterface service(path.path(),this);
                if (service.isValid()) {
                    sig = service.getSignalStrength();

                    if (sig == 0 && (service.getState() == "ready" ||
                                    service.getState() == "online")) {
                        sig = 100;
                    }
                }
            }
        }
        return sig;
    }
#else
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            QString iface = interfaceForMode(QSystemNetworkInfo::WlanMode).name();
            QFile file("/proc/net/wireless");

            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                return 0;
            QTextStream in(&file);
            QString line = in.readLine();
            while (!line.isNull()) {
                if (line.left(6).contains(iface)) {
                    QString token = line.section(" ",4,5).simplified();
                    token.chop(1);
                    bool ok;
                    int percent = (int)rint ((log (token.toInt(&ok)) / log (92)) * 100.0);
                    if (ok)
                        return percent;
                    else
                        return 0;
                }
                line = in.readLine();
            }
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            QString result;
            const QString baseSysDir = "/sys/class/net/";
            const QDir eDir(baseSysDir);
            const QStringList dirs = eDir.entryList(QStringList() << "eth*", QDir::Dirs | QDir::NoDotAndDotDot);
            foreach (const QString dir, dirs) {
                const QString devFile = baseSysDir + dir;
                const QFileInfo fi(devFile + "/carrier");
                if (fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if (rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
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

#endif
    return -1;
}

QNetworkInterface QSystemNetworkInfoLinuxCommonPrivate::interfaceForMode(QSystemNetworkInfo::NetworkMode mode)
{
#if !defined(QT_NO_DBUS)
#if !defined(QT_NO_CONNMAN)
    if (connmanIsAvailable) {
        foreach (const QString &servicePath, connmanManager->getServices()) {
            QConnmanServiceInterface *serviceIface;
            serviceIface = new QConnmanServiceInterface(servicePath,this);
            if(serviceIface->getType() == modeToTechnology(mode)) {
                return QNetworkInterface::interfaceFromName(serviceIface->getInterface());
            }
        }
   }
#else
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            QHalInterface iface;
            if (iface.isValid()) {
                const QStringList list = iface.findDeviceByCapability("net.80211");
                if (!list.isEmpty()) {
                    foreach (const QString &netDev, list) {
                        QHalDeviceInterface ifaceDevice(netDev);
                        const QString deviceName  = ifaceDevice.getPropertyString("net.interface");
                        if (list.count() > 1) {
                            const QString baseFIle = "/sys/class/net/" + deviceName+"/operstate";
                            QFile rx(baseFIle);
                            if (rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                                QString operatingState;
                                QTextStream in(&rx);
                                in >> operatingState;
                                rx.close();
                                if (!operatingState.contains("unknown")
                                    || !operatingState.contains("down")) {
                                    if (isDefaultInterface(deviceName))
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
                if (!list.isEmpty()) {
                    foreach (const QString &netDev, list) {
                        QHalDeviceInterface ifaceDevice(netDev);
                        const QString deviceName  = ifaceDevice.getPropertyString("net.interface");
                        if (list.count() > 1) {
                            const QString baseFIle = "/sys/class/net/" + deviceName+"/operstate";
                            QFile rx(baseFIle);
                            if (rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                                QString operatingState;
                                QTextStream in(&rx);
                                in >> operatingState;
                                rx.close();
                                if (!operatingState.contains("unknown")
                                    || !operatingState.contains("down")) {
                                    if (isDefaultInterface(deviceName))
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
#endif
#else
    QString result;
    const QString baseSysDir = "/sys/class/net/";
    const QDir eDir(baseSysDir);
    const QStringList dirs = eDir.entryList(QStringList() << "*", QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach (const QString &dir, dirs) {
        const QString devFile = baseSysDir + dir;
        const QFileInfo devfi(devFile + "/device");
        if (!devfi.exists()) {
            continue;
        }
        const QString baseFIle = "/sys/class/net/" + devFile+"/operstate";
        QFile rx(baseFIle);
        if (rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString operatingState;
            QTextStream in(&rx);
            in >> operatingState;
            rx.close();
            if (operatingState.contains("unknown")) {
                continue;
            }
        }
        switch(mode) {
        case QSystemNetworkInfo::WlanMode:
            {
                const QFileInfo fi(devFile + "/wireless");
                if (fi.exists()) {
                    return QNetworkInterface::interfaceFromName(dir);
                }
            }
            break;
            case QSystemNetworkInfo::EthernetMode:
            {
                const QFileInfo fi(devFile + "/wireless");
                if (!fi.exists()) {
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
    if (routeFilex.exists() && routeFilex.open(QIODevice::ReadOnly
                                              | QIODevice::Text)) {
        QTextStream rin(&routeFilex);
        QString line = rin.readLine();
        while (!line.isNull()) {
            const QString lineSection = line.section("\t",2,2,QString::SectionSkipEmpty);
            if (lineSection != "00000000" && lineSection!="Gateway")
                if (line.section("\t",0,0,QString::SectionSkipEmpty) == deviceName) {
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
    if (btHasPower()) {
        const QString sysPath = "/sys/class/bluetooth/";
        const QDir sysDir(sysPath);
        QStringList filters;
        filters << "*";
        const QStringList sysList = sysDir.entryList( filters ,QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        foreach (const QString &dir, sysList) {
            QFile btFile(sysPath + dir+"/"+file);
            if (btFile.exists()) {
                if (btFile.open(QIODevice::ReadOnly)) {
                    QTextStream btFileStream(&btFile);
                    QString line = btFileStream.readAll();
                    return line.simplified();
                }
            }
        }
    }
    return QString();
}
#endif

#if !defined(QT_NO_CONNMAN)
void QSystemNetworkInfoLinuxCommonPrivate::initConnman()
{
    connmanManager = new QConnmanManagerInterface(this);
    connect(connmanManager,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
            this,SLOT(connmanPropertyChangedContext(QString,QString,QDBusVariant)));

    foreach (const QString &servicePath, connmanManager->getServices()) {
        QConnmanServiceInterface *serviceIface;
        serviceIface = new QConnmanServiceInterface(servicePath,this);

        connect(serviceIface,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                this,SLOT(connmanServicePropertyChangedContext(QString,QString,QDBusVariant)));
    }

    foreach (const QString &techPath, connmanManager->getTechnologies()) {
        QConnmanTechnologyInterface *tech;
        tech = new QConnmanTechnologyInterface(techPath, this);

        connect(tech,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
                this,SLOT(connmanTechnologyPropertyChangedContext(QString,QString,QDBusVariant)));

//        foreach (const QString &devicePath,tech->getDevices()) {
//            QConnmanDeviceInterface *dev;
//            dev = new QConnmanDeviceInterface(devicePath);
//                connect(dev,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
//                        this,SLOT(connmanDevicePropertyChangedContext(QString,QString,QDBusVariant)));
//        }
    }
}

void QSystemNetworkInfoLinuxCommonPrivate::initOfono()
{
    ofonoManager = new QOfonoManagerInterface(this);
    connect(ofonoManager,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
            this,SLOT(ofonoPropertyChangedContext(QString,QString,QDBusVariant)));

    foreach (const QDBusObjectPath &path, ofonoManager->getModems()) {
        initModem(path.path());
    }
}

void QSystemNetworkInfoLinuxCommonPrivate::initModem(const QString &path)
{
    QOfonoModemInterface *modemIface;
    modemIface = new QOfonoModemInterface(path,this);
    knownModems << path;

    connect(modemIface,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
            this,SLOT(ofonoModemPropertyChangedContext(QString,QString,QDBusVariant)));

    QOfonoNetworkRegistrationInterface *ofonoNetworkInterface;
    ofonoNetworkInterface = new QOfonoNetworkRegistrationInterface(path,this);
    connect(ofonoNetworkInterface,SIGNAL(propertyChangedContext(QString,QString,QDBusVariant)),
            this,SLOT(ofonoNetworkPropertyChangedContext(QString,QString,QDBusVariant)));
}

#endif

void QSystemNetworkInfoLinuxCommonPrivate::connectNotify(const char *signal)
{
/*
   void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
   void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int);
   void currentMobileCountryCodeChanged(const QString &);
   void currentMobileNetworkCodeChanged(const QString &);
   void networkNameChanged(QSystemNetworkInfo::NetworkMode, const QString &);
   void networkModeChanged(QSystemNetworkInfo::NetworkMode);

*/    if (QLatin1String(signal) ==
        QLatin1String(QMetaObject::normalizedSignature(SIGNAL(logicalDriveChanged(bool, const QString &))))) {

    }
}

void QSystemNetworkInfoLinuxCommonPrivate::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) ==
        QLatin1String(QMetaObject::normalizedSignature(SIGNAL(logicalDriveChanged(bool, const QString &))))) {

    }
}

#if !defined(QT_NO_CONNMAN)
QString QSystemNetworkInfoLinuxCommonPrivate::modeToTechnology(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            return "wifi";
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            return "ethernet";
        }
        break;
    case QSystemNetworkInfo::BluetoothMode:
        {
            return "bluetooth";
        }
        break;
    case QSystemNetworkInfo::GsmMode:
    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::WcdmaMode:
        {
            return "cellular";
        }
        break;
    case QSystemNetworkInfo::WimaxMode:
        {
            return "wimax";
        }
        break;
    default:
        break;
    };
    return QString();
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoLinuxCommonPrivate::stateToStatus(const QString &state)
{
    if (state == "idle") {
        return QSystemNetworkInfo::NoNetworkAvailable;
    } else if (state == "failure") {
        return QSystemNetworkInfo::Denied;
    } else if (state == "association" || state == "configuration" || state == "login") {
        return QSystemNetworkInfo::Searching;
    } else if (state == "ready" || state == "online") {
        return QSystemNetworkInfo::Connected;
    }
    return QSystemNetworkInfo::UndefinedStatus;
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoLinuxCommonPrivate::typeToMode(const QString &type)
{
    if (type == "ethernet") {
        return QSystemNetworkInfo::EthernetMode;
    } else if (type == "wifi") {
        return QSystemNetworkInfo::WlanMode;

    } else if (type == "bluetooth") {
        return QSystemNetworkInfo::BluetoothMode;

    } else if (type == "wimax") {
        return QSystemNetworkInfo::WimaxMode;

    } else if (type == "cellular") {
        if (ofonoIsAvailable) {

            QOfonoNetworkRegistrationInterface ofonoNetwork(ofonoManager->currentModem().path(),this);
            return ofonoTechToMode(ofonoNetwork.getTechnology());
        }
    }
    return QSystemNetworkInfo::UnknownMode;
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoLinuxCommonPrivate::ofonoTechToMode(const QString &ofonoTech)
{
    if (ofonoTech == "gsm") {
        return QSystemNetworkInfo::GsmMode;
    }
    if (ofonoTech == "edge"){
        return QSystemNetworkInfo::GsmMode;
    }
    if (ofonoTech == "umts"){
        return QSystemNetworkInfo::WcdmaMode;
    }
    if (ofonoTech == "hspa"){
        // handle this?
         return QSystemNetworkInfo::WcdmaMode;
    }
    if (ofonoTech == "lte"){
        return QSystemNetworkInfo::WimaxMode;
    }
    return QSystemNetworkInfo::GsmMode; //its ofono, default to gsm
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoLinuxCommonPrivate::ofonoStatusToStatus(const QString &status)
{
    QSystemNetworkInfo::NetworkStatus networkStatus = QSystemNetworkInfo::UndefinedStatus;
    if (status == "unregistered") {
        networkStatus = QSystemNetworkInfo::EmergencyOnly;
    }
    if (status == "registered") {
        networkStatus = QSystemNetworkInfo::HomeNetwork;
    }
    if (status == "searching") {
        networkStatus = QSystemNetworkInfo::Searching;
    }
    if (status == "denied") {
        networkStatus = QSystemNetworkInfo::Denied;
    }
    if (status == "unknown") {
        networkStatus = QSystemNetworkInfo::UndefinedStatus;
    }
    if (status == "roaming") {
        networkStatus = QSystemNetworkInfo::Roaming;
    }
    return networkStatus;
}

void QSystemNetworkInfoLinuxCommonPrivate::connmanPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value)
{
//    qDebug() << __FUNCTION__ << path << item << value.variant();
    if (item == "Services") {
    }

    if (item == "State") {
        // qDebug() << value.variant();
    }

    if (item == "DefaultTechnology") {
        // qDebug() << value.variant();
        QConnmanServiceInterface serviceIface(path,this);
        emit networkNameChanged(typeToMode(value.variant().toString()), serviceIface.getName());
        emit networkModeChanged(typeToMode(value.variant().toString()));
    }

}

void QSystemNetworkInfoLinuxCommonPrivate::connmanTechnologyPropertyChangedContext(const QString &/*path*/,const QString &/*item*/, const QDBusVariant &/*value*/)
{
//        qDebug() << __FUNCTION__ << path << item << value.variant();

}

void QSystemNetworkInfoLinuxCommonPrivate::connmanDevicePropertyChangedContext(const QString &/*path*/,const QString &/*item*/, const QDBusVariant &/*value*/)
{
  //      qDebug() << __FUNCTION__ << path << item << value.variant();

}

void QSystemNetworkInfoLinuxCommonPrivate::connmanServicePropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value)
{
//    qDebug() << __FUNCTION__ << path << item << value.variant();
    if (item == "State") {
        QConnmanServiceInterface serviceIface(path,this);
      //  QString type = serviceIface.getType();
        QString state = value.variant().toString();
        QSystemNetworkInfo::NetworkMode mode = typeToMode(serviceIface.getType());
        emit networkStatusChanged(mode, stateToStatus(state));
        if (state == "idle" || state == "failure") {
            emit networkNameChanged(mode, QString());
            emit networkSignalStrengthChanged(mode, 0);
        } else  if (state == "ready" || state == "online")
        emit networkNameChanged(mode, serviceIface.getName());
        if (serviceIface.isRoaming()) {
            emit networkStatusChanged(mode, QSystemNetworkInfo::Roaming);
        }
    }
    if (item == "Strength") {
        QConnmanServiceInterface serviceIface(path,this);
        emit networkSignalStrengthChanged(typeToMode(serviceIface.getType()),value.variant().toUInt());
    }
    if (item == "Roaming") {
        QConnmanServiceInterface serviceIface(path,this);
        emit networkStatusChanged(typeToMode(serviceIface.getType()), QSystemNetworkInfo::Roaming);
    }
    if (item == "MCC") {
        emit currentMobileCountryCodeChanged(value.variant().toString());
    }
    if (item == "MNC") {
        emit currentMobileNetworkCodeChanged(value.variant().toString());
    }
    if (item == "Mode") {
        //"gprs" "edge" "umts"
        QConnmanServiceInterface serviceIface(path,this);
        if (serviceIface.getType() == "cellular") {
            emit networkModeChanged(ofonoTechToMode(value.variant().toString()));
        }
    }
}

void QSystemNetworkInfoLinuxCommonPrivate::ofonoPropertyChangedContext(const QString &/*contextpath*/,const QString &item, const QDBusVariant &value)
{
//    qDebug() << __FUNCTION__ << path << item << value.variant();
    if (item == "Modems") {
        QList <QDBusObjectPath> modems =  qdbus_cast<QList <QDBusObjectPath> > (value.variant());

        if (modems.count() >  knownModems.count()) {
            //add a modem
            foreach (const QDBusObjectPath &path, modems) {
                if (!knownModems.contains(path.path())) {
                    initModem(path.path());
                }
            }
        }  else {
            //remove one
            QStringList newModemList;
            foreach (const QDBusObjectPath &path, modems) {
                newModemList << path.path();
            }

            foreach (const QString &path, knownModems) {
                if (!newModemList.contains(path)) {
                    knownModems.removeAll(path);
                }
            }
        }
    }
}

void QSystemNetworkInfoLinuxCommonPrivate::ofonoNetworkPropertyChangedContext(const QString &path,const QString &item, const QDBusVariant &value)
{
    QOfonoNetworkRegistrationInterface netiface(path);

    if (item == "Strength") {
        Q_EMIT networkSignalStrengthChanged(ofonoTechToMode(netiface.getTechnology()),value.variant().toInt());
    }
    if (item == "Status") {

       Q_EMIT networkStatusChanged(ofonoTechToMode(netiface.getTechnology()), ofonoStatusToStatus(value.variant().toString()));
    }
    if (item == "LocationAreaCode") {

    }
    if (item == "CellId") {
        Q_EMIT cellIdChanged(value.variant().toUInt());
    }
    if (item == "Technology") {

    }
    if (item == "Name") {
        Q_EMIT networkNameChanged(ofonoTechToMode(netiface.getTechnology()), value.variant().toString());

    }
}

void QSystemNetworkInfoLinuxCommonPrivate::ofonoModemPropertyChangedContext(const QString &/*path*/,const QString &/*item*/, const QDBusVariant &/*value*/)
{
}

#endif

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoLinuxCommonPrivate::currentMode()
{
#if !defined(QT_NO_CONNMAN)
    if (connmanIsAvailable) {
        QString curMode = connmanManager->getDefaultTechnology();
        if (curMode == "wifi") {
            return QSystemNetworkInfo::WlanMode;
        } else if (curMode == "ethernet") {
            return QSystemNetworkInfo::EthernetMode;
        } else if (curMode == "cellular") {
            return typeToMode(curMode);
        } else if (curMode == "bluetooth") {
            return QSystemNetworkInfo::BluetoothMode;
        } else if (curMode == "wimax") {
            return QSystemNetworkInfo::WimaxMode;
        }
    }
#endif
    return QSystemNetworkInfo::UnknownMode;
}

qint32 QSystemNetworkInfoLinuxCommonPrivate::cellId()
{
#if !defined(QT_NO_CONNMAN)
    if (ofonoIsAvailable) {
        QString modempath = ofonoManager->currentModem().path();
        if(!modempath.isEmpty()) {
            QOfonoNetworkRegistrationInterface ofonoNetwork(modempath,this);
            return ofonoNetwork.getCellId();
        }
    }
#endif
    return 0;
}

int QSystemNetworkInfoLinuxCommonPrivate::locationAreaCode()
{
#if !defined(QT_NO_CONNMAN)
    if (ofonoIsAvailable) {
        QString modempath = ofonoManager->currentModem().path();
        if(!modempath.isEmpty()) {
            QOfonoNetworkRegistrationInterface ofonoNetwork(modempath,this);
            return ofonoNetwork.getLac();
        }
    }
#endif
    return 0;
}

QString QSystemNetworkInfoLinuxCommonPrivate::currentMobileCountryCode()
{
#if !defined(QT_NO_CONNMAN)
    if (ofonoIsAvailable) {
        QString modempath = ofonoManager->currentModem().path();
        if(!modempath.isEmpty()) {
            QOfonoNetworkRegistrationInterface ofonoNetworkOperator(modempath,this);
            foreach (const QDBusObjectPath &opPath, ofonoNetworkOperator.getOperators()) {
                if (!opPath.path().isEmpty()) {
                    QOfonoNetworkOperatorInterface netop(opPath.path(),this);
                    if (netop.getStatus() == "current")
                        return netop.getMcc();
                }
            }
        }
    }
#endif
    return QString();
}

QString QSystemNetworkInfoLinuxCommonPrivate::currentMobileNetworkCode()
{
#if !defined(QT_NO_CONNMAN)
    if (ofonoIsAvailable) {
        QString modempath = ofonoManager->currentModem().path();
        if(!modempath.isEmpty()) {
            QOfonoNetworkRegistrationInterface ofonoNetworkOperator(modempath,this);
            foreach (const QDBusObjectPath &opPath, ofonoNetworkOperator.getOperators()) {
                if (!opPath.path().isEmpty()) {
                    QOfonoNetworkOperatorInterface netop(opPath.path(),this);
                    //                if (netop.getStatus() == "current")
                    return netop.getMnc();
                }
            }
        }
    }
#endif
    return QString();
}

QString QSystemNetworkInfoLinuxCommonPrivate::homeMobileCountryCode()
{
#if !defined(QT_NO_CONNMAN)
    if (ofonoIsAvailable) {
        QString modem = ofonoManager->currentModem().path();
        if (!modem.isEmpty()) {
            QOfonoSimInterface simInterface(modem,this);
            if (simInterface.isPresent()) {
                QString home = simInterface.getHomeMcc();
                if (!home.isEmpty()) {
                    return home;
                } else {
                    QOfonoNetworkRegistrationInterface netIface(modem,this);
                        return currentMobileCountryCode();
                }
            }
        }
    }
#endif
    return QString();
}

QString QSystemNetworkInfoLinuxCommonPrivate::homeMobileNetworkCode()
{
#if !defined(QT_NO_CONNMAN)
    if (ofonoIsAvailable) {
        QString modem = ofonoManager->currentModem().path();
        if (!modem.isEmpty()) {
            QOfonoSimInterface simInterface(modem,this);
            if (simInterface.isPresent()) {
                QString home = simInterface.getHomeMnc();
                if (!home.isEmpty()) {
                    return home;
                } else {
                    QOfonoNetworkRegistrationInterface netIface(modem,this);
                    if (netIface.getStatus() == "registered") {
                        //on home network, not roaming
                        return currentMobileNetworkCode();
                    }
                }
            }
        }
    }
#endif
    return QString();
}


QSystemDisplayInfoLinuxCommonPrivate::QSystemDisplayInfoLinuxCommonPrivate(QObject *parent)
    : QObject(parent)
{
    halIsAvailable = halAvailable();
}

QSystemDisplayInfoLinuxCommonPrivate::~QSystemDisplayInfoLinuxCommonPrivate()
{
}

int QSystemDisplayInfoLinuxCommonPrivate::colorDepth(int screen)
{
#if !defined(Q_WS_MAEMO_6)  && !defined(Q_WS_MEEGO)
#ifdef Q_WS_X11
    QDesktopWidget wid;
    return wid.screen(screen)->x11Info().depth();
#else
#endif
#endif
    return QPixmap::defaultDepth();
}


int QSystemDisplayInfoLinuxCommonPrivate::displayBrightness(int screen)
{
    Q_UNUSED(screen);

#if !defined(QT_NO_DBUS)
    if (halIsAvailable) {
        QHalInterface iface;
        if (iface.isValid()) {
            const QStringList list = iface.findDeviceByCapability("laptop_panel");
            if (!list.isEmpty()) {
                foreach (const QString &lapDev, list) {
                    QHalDeviceInterface ifaceDevice(lapDev);
                    QHalDeviceLaptopPanelInterface lapIface(lapDev);
                    const float numLevels = ifaceDevice.getPropertyInt("laptop_panel.num_levels") - 1;
                    const float curLevel = lapIface.getBrightness();
                    return curLevel / numLevels * 100;
                }
            }
        }
    }
#endif
    const QString backlightPath = "/proc/acpi/video/";
    const QDir videoDir(backlightPath);
    QStringList filters;
    filters << "*";
    const QStringList brightnessList = videoDir.entryList(filters,
                                                          QDir::Dirs
                                                          | QDir::NoDotAndDotDot,
                                                          QDir::Name);
    foreach (const QString &brightnessFileName, brightnessList) {
        float numLevels = 0.0;
        float curLevel = 0.0;

        const QDir videoSubDir(backlightPath+"/"+brightnessFileName);

        const QStringList vidDirList = videoSubDir.entryList(filters,
                                                             QDir::Dirs
                                                             | QDir::NoDotAndDotDot,
                                                             QDir::Name);
        foreach (const QString &vidFileName, vidDirList) {
            QFile curBrightnessFile(backlightPath+brightnessFileName+"/"+vidFileName+"/brightness");
            if (!curBrightnessFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug()<<"File not opened";
            } else {
                QTextStream bri(&curBrightnessFile);
                QString line = bri.readLine();
                while(!line.isNull()) {
                    if (!line.contains("not supported")) {
                        if (line.contains("levels")) {
                            QString level = line.section(" ",-1);
                            bool ok;
                            numLevels = level.toFloat(&ok);
                            if (!ok)
                                numLevels = -1;
                        } else if (line.contains("current")) {
                            QString level = line.section(": ",-1);
                            bool ok;
                            curLevel = level.toFloat(&ok);
                            if (!ok)
                                curLevel = 0;
                        }
                    }
                    line = bri.readLine();
                }
                curBrightnessFile.close();
                if (curLevel > -1 && numLevels > 0) {
                    return curLevel / numLevels * 100;
                }

            }
        }
    }
    return -1;
}

QSystemDisplayInfo::BacklightState  QSystemDisplayInfoLinuxCommonPrivate::backlightStatus(int screen)
{
    Q_UNUSED(screen)
    return QSystemDisplayInfo::BacklightStateUnknown;
}

int QSystemDisplayInfoLinuxCommonPrivate::physicalHeight(int screen)
{
    QString frameBufferDevicePath = QString("/dev/fb%1").arg(screen);
    int height = 0;
    int fd;
    struct fb_var_screeninfo vi;

    if (-1 == (fd = open(frameBufferDevicePath.toStdString().c_str(), O_RDONLY | O_NONBLOCK))) {
        goto out;
    }
    if (-1 == ioctl(fd, FBIOGET_VSCREENINFO, &vi)) {
        goto out;
    }
    height = vi.height;
out:
    if (fd != -1) {
        close(fd);
    }
    return height;
}

int QSystemDisplayInfoLinuxCommonPrivate::physicalWidth(int screen)
{
    QString frameBufferDevicePath = QString("/dev/fb%1").arg(screen);
    int width = 0;
    int fd;
    struct fb_var_screeninfo vi;

    if (-1 == (fd = open(frameBufferDevicePath.toStdString().c_str(), O_RDONLY | O_NONBLOCK))) {
        goto out;
    }
    if (-1 == ioctl(fd, FBIOGET_VSCREENINFO, &vi)) {
        goto out;
    }
    width = vi.width;
out:
    if (fd != -1) {
        close(fd);
    }
    return width;
}

QSystemStorageInfoLinuxCommonPrivate::QSystemStorageInfoLinuxCommonPrivate(QObject *parent)
    : QObject(parent)
{
    halIsAvailable = halAvailable();
#if !defined(QT_NO_UDISKS)
    udisksIsAvailable = udisksAvailable();
#endif

#if !defined(QT_NO_DBUS)
    if (halIsAvailable)
        halIface = new QHalInterface(this);
#if !defined(QT_NO_CONNMAN)
#if !defined(QT_NO_UDISKS)
    if (udisksIsAvailable)
        udisksIface = new QUDisksInterface(this);
#endif
#endif
#endif
    storageChanged = true;

    logicalDrives();
    storageChanged = false;

    checkAvailableStorage();
}

QSystemStorageInfoLinuxCommonPrivate::~QSystemStorageInfoLinuxCommonPrivate()
{
    ::close(inotifyFD);
}

void QSystemStorageInfoLinuxCommonPrivate::connectNotify(const char *signal)
{
    if (QLatin1String(signal) ==
        QLatin1String(QMetaObject::normalizedSignature(SIGNAL(logicalDriveChanged(bool, const QString &))))) {
        if (udisksIsAvailable) {
#if !defined(QT_NO_DBUS)
#if !defined(QT_NO_UDISKS)
            connect(udisksIface,SIGNAL(deviceChanged(QDBusObjectPath)),
                    this,SLOT(udisksDeviceChanged(QDBusObjectPath)));
#endif
#endif
        } else {

            inotifyFD = ::inotify_init();
            mtabWatchA = ::inotify_add_watch(inotifyFD, "/etc/mtab", IN_MODIFY);
            if (mtabWatchA > 0) {
                QSocketNotifier *notifier = new QSocketNotifier
                                            (inotifyFD, QSocketNotifier::Read, this);
                connect(notifier, SIGNAL(activated(int)), this, SLOT(inotifyActivated()));
            }
        }
    }
}

void QSystemStorageInfoLinuxCommonPrivate::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) ==
        QLatin1String(QMetaObject::normalizedSignature(SIGNAL(logicalDriveChanged(bool, const QString &))))) {
        if (udisksIsAvailable) {
#if !defined(QT_NO_DBUS)
#if !defined(QT_NO_UDISKS)
            disconnect(udisksIface,SIGNAL(deviceChanged(QDBusObjectPath)),
                       this,SLOT(udisksDeviceChanged(QDBusObjectPath)));
#endif
#endif
        } else {
            ::inotify_rm_watch(inotifyFD, mtabWatchA);
        }
    }
}


void QSystemStorageInfoLinuxCommonPrivate::inotifyActivated()
{
    char buffer[1024];
    struct inotify_event *event;
    int len = ::read(inotifyFD, (void *)buffer, sizeof(buffer));
    if (len > 0) {
        event = (struct inotify_event *)buffer;
        if (event->wd == mtabWatchA /*&& (event->mask & IN_IGNORED) == 0*/) {
            ::inotify_rm_watch(inotifyFD, mtabWatchA);
            QTimer::singleShot(1000,this,SLOT(deviceChanged()));//give this time to finish write
            mtabWatchA = ::inotify_add_watch(inotifyFD, "/etc/mtab", IN_MODIFY);
        }
    }
}

void QSystemStorageInfoLinuxCommonPrivate::deviceChanged()
{
    QMap<QString, QString> oldDrives = mountEntriesMap;
    storageChanged = true;
    mountEntries();

    if (mountEntriesMap.count() < oldDrives.count()) {
        QMapIterator<QString, QString> i(oldDrives);
        while (i.hasNext()) {
            i.next();
            if (!mountEntriesMap.contains(i.key())) {
                emit logicalDriveChanged(false, i.key());
            }
        }
    } else if (mountEntriesMap.count() > oldDrives.count()) {
        QMapIterator<QString, QString> i(mountEntriesMap);
        while (i.hasNext()) {
            i.next();

            if (oldDrives.contains(i.key()))
                continue;
            emit logicalDriveChanged(true,i.key());
        }
    }
    storageChanged = false;

}

#if !defined(QT_NO_DBUS)
void QSystemStorageInfoLinuxCommonPrivate::udisksDeviceChanged(const QDBusObjectPath &path)
{
    storageChanged = true;
    if (udisksIsAvailable) {
#if !defined(QT_NO_UDISKS)
        QUDisksDeviceInterface devIface(path.path());
        QString mountp;
        if (devIface.deviceMountPaths().count() > 0)
            mountp = devIface.deviceMountPaths().at(0);

        if (devIface.deviceIsMounted()) {
            emit logicalDriveChanged(true, mountp);
        } else {
            emit logicalDriveChanged(false, mountEntriesMap.key(devIface.deviceFilePresentation()));
        }
#else
Q_UNUSED(path);
#endif
    }
    mountEntries();
}
#endif

qint64 QSystemStorageInfoLinuxCommonPrivate::availableDiskSpace(const QString &driveVolume)
{
    if (driveVolume.left(2) == "//") {
        return 0;
    }
    mountEntries();
    struct statfs fs;
    if (statfs(driveVolume.toLatin1(), &fs ) == 0 ) {
                long blockSize = fs.f_bsize;
                long availBlocks = fs.f_bavail;
                return (double)availBlocks * blockSize;
            }
    return 0;
}

qint64 QSystemStorageInfoLinuxCommonPrivate::totalDiskSpace(const QString &driveVolume)
{
    if (driveVolume.left(2) == "//") {
        return 0;
    }
    mountEntries();
    struct statfs fs;
    if (statfs(driveVolume.toLatin1(), &fs ) == 0 ) {
        const long blockSize = fs.f_bsize;
        const long totalBlocks = fs.f_blocks;
        return (double)totalBlocks * blockSize;
    }
    return 0;
}

QSystemStorageInfo::DriveType QSystemStorageInfoLinuxCommonPrivate::typeForDrive(const QString &driveVolume)
{
    if (udisksIsAvailable) {
#if !defined(QT_NO_DBUS)
#if !defined(QT_NO_UDISKS)
        QString udiskdev = mountEntriesMap.value(driveVolume);
        udiskdev.chop(1);
        QUDisksDeviceInterface devIfaceParent(udiskdev);
        QUDisksDeviceInterface devIface(mountEntriesMap.value(driveVolume));

        if (devIface.deviceIsMounted()) {
            QString chopper = devIface.deviceFile();
            QString mountp;
            if (devIfaceParent.deviceIsRemovable()) {
                return QSystemStorageInfo::RemovableDrive;
            } else if (devIfaceParent.deviceIsSystemInternal()) {
                if (devIfaceParent.driveIsRotational()) {
                    return QSystemStorageInfo::InternalDrive;
                } else {
                    return QSystemStorageInfo::InternalFlashDrive;
                }
            } else {
                return QSystemStorageInfo::RemoteDrive;
            }
        } else {
            //udisks cannot see mounted samba shares
            if (mountEntriesMap.value(driveVolume).left(2) == "//") {
                return QSystemStorageInfo::RemoteDrive;
            }
        }
#endif
#endif
    }
    if (halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QStringList mountedVol;
        QHalInterface iface;
        const QStringList list = iface.findDeviceByCapability("volume");
        if (!list.isEmpty()) {
            foreach (const QString &vol, list) {
                QHalDeviceInterface ifaceDevice(vol);
                if (mountEntriesMap.value(driveVolume) == ifaceDevice.getPropertyString("block.device")) {
                    QHalDeviceInterface ifaceDeviceParent(ifaceDevice.getPropertyString("info.parent"), this);

                    if (ifaceDeviceParent.getPropertyBool("storage.removable")
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

        if (mountEntriesMap.value(driveVolume).contains("mapper")) {
            struct stat stat_buf;
            stat( mountEntriesMap.value(driveVolume).toLatin1(), &stat_buf);

            dmFile = QString("/sys/block/dm-%1/removable").arg(stat_buf.st_rdev & 0377);

        } else {

            dmFile = mountEntriesMap.value(driveVolume).section("/",2,3);
            if (dmFile.left(3) == "mmc") { //assume this dev is removable sd/mmc card.
                return QSystemStorageInfo::RemovableDrive;
            }

            if (dmFile.length() > 3) { //if device has number, we need the 'parent' device
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
            if (line.contains("1")) {
                return QSystemStorageInfo::RemovableDrive;
            }
        }
    }
    if (driveVolume.left(2) == "//") {
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
    if (!storageChanged)
        return;
    mountEntriesMap.clear();
#if !defined(QT_NO_DBUS)
#if !defined(QT_NO_UDISKS)
    if (udisksAvailable()) {
        foreach (const QDBusObjectPath &device,udisksIface->enumerateDevices() ) {
            QUDisksDeviceInterface devIface(device.path());
            if (devIface.deviceIsMounted()) {
                QString fsname = devIface.deviceMountPaths().at(0);
                if ( !mountEntriesMap.keys().contains(fsname)) {
                    mountEntriesMap[fsname.toLatin1()] = device.path()/*.section("/")*/;
                }
            }
        }
    }
#endif
#endif
    FILE *mntfp = setmntent( _PATH_MOUNTED, "r" );
    mntent *me = getmntent(mntfp);
    bool ok;
    while(me != NULL) {
        struct statfs fs;
        ok = false;
        if (strcmp(me->mnt_type, "cifs") != 0) { //smb has probs with statfs
            if (statfs(me->mnt_dir, &fs ) ==0 ) {
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
        if (ok && !mountEntriesMap.keys().contains(me->mnt_dir)
            && QString(me->mnt_fsname).left(1) == "/") {
            mountEntriesMap[me->mnt_dir] = me->mnt_fsname;
        }

        me = getmntent(mntfp);
    }
    endmntent(mntfp);
}

QString QSystemStorageInfoLinuxCommonPrivate::getUuid(const QString &vol)
{
    QDir uuidDir("/dev/disk/by-uuid");
    if (uuidDir.exists()) {
        QFileInfoList fileList = uuidDir.entryInfoList();
        foreach (const QFileInfo &fi, fileList) {
            if (fi.isSymLink()) {
                if (fi.symLinkTarget().contains(mountEntriesMap.value(vol).section("/",-1))) {
                    return fi.baseName();
                }
            }
        }
    }
    return QString();
}

QString QSystemStorageInfoLinuxCommonPrivate::uriForDrive(const QString &driveVolume)
{
#if !defined(QT_NO_DBUS)
#if !defined(QT_NO_UDISKS)
    if (udisksIsAvailable) {
        QUDisksDeviceInterface devIface(mountEntriesMap.value(driveVolume));
        if (devIface.deviceIsMounted()) {
            return devIface.uuid();
        }
        return QString();
    }
#endif
#endif
    QString driveUuid = getUuid(driveVolume);
    if(!driveUuid.isEmpty()) {
        return  driveUuid;
    }
    //last resort
#if defined (BLKID_SUPPORTED)
    int fd;
    blkid_probe pr = NULL;
    uint64_t size;
    const char *label;
    char *ret;
    QFile dev(mountEntriesMap.value(driveVolume));
    dev.open(QIODevice::ReadOnly);
    fd = dev.handle();
    if (fd < 0) {
//        qDebug() << "XXXXXXXXXXXXXXXXXXXXXXXXXXXX" << mountEntriesMap.value(driveVolume);
       return QString();
    } else {
        pr = blkid_new_probe();
        blkid_probe_set_request (pr, BLKID_PROBREQ_UUID);
        ::ioctl(fd, BLKGETSIZE64, &size);
        blkid_probe_set_device (pr, fd, 0, size);
        blkid_do_safeprobe (pr);
        blkid_probe_lookup_value(pr, "UUID", &label, NULL);
        ret = strdup(label);
        blkid_free_probe (pr);
        close(fd);
        return label;
    }
#endif
    return QString();
}

QSystemStorageInfo::StorageState QSystemStorageInfoLinuxCommonPrivate::getStorageState(const QString &driveVolume)
{
    QSystemStorageInfo::StorageState storState = QSystemStorageInfo::UnknownStorageState;
    struct statfs fs;
    if (statfs(driveVolume.toLocal8Bit(), &fs) == 0) {
        if ( fs.f_bfree != 0) {
            long percent = 100 -(fs.f_blocks - fs.f_bfree) * 100 / fs.f_blocks;
            //       qDebug()  << driveVolume << percent;

            if (percent < 41 && percent > 10 ) {
                storState = QSystemStorageInfo::LowStorageState;
            } else if (percent < 11 && percent > 2 ) {
                storState =  QSystemStorageInfo::VeryLowStorageState;
            } else if (percent < 3  ) {
                storState =  QSystemStorageInfo::CriticalStorageState;
            } else {
                 storState =  QSystemStorageInfo::NormalStorageState;
            }
        }
    }
//    qDebug()  << driveVolume << storState;
   return storState;
}

//QT_LINUXBASE

void QSystemStorageInfoLinuxCommonPrivate::checkAvailableStorage()
{
    QMap<QString, QString> oldDrives = mountEntriesMap;
    foreach (const QString &vol, oldDrives.keys()) {
        QSystemStorageInfo::StorageState storState = getStorageState(vol);
        if (!stateMap.contains(vol)) {
            stateMap.insert(vol,storState);
        } else {
            if (stateMap.value(vol) != storState) {
                stateMap[vol] = storState;
                //      qDebug() << "storage state changed" << storState;
                Q_EMIT storageStateChanged(vol, storState);
            }
        }
    }
}

QSystemDeviceInfoLinuxCommonPrivate::QSystemDeviceInfoLinuxCommonPrivate(QObject *parent)
    : QObject(parent),hasWirelessKeyboardConnected(0)
{
#if !defined(QT_NO_DBUS)
    halIsAvailable = halAvailable();
    setConnection();
    setupBluetooth();
    currentPowerState();
#endif
   initBatteryStatus();
}

QSystemDeviceInfoLinuxCommonPrivate::~QSystemDeviceInfoLinuxCommonPrivate()
{
}

void QSystemDeviceInfoLinuxCommonPrivate::initBatteryStatus()
{
    //    const int level = batteryLevel();
    //    currentBatLevel = level;

    //    QSystemDeviceInfo::BatteryStatus stat = QSystemDeviceInfo::NoBatteryLevel;

    //    if (level < 4) {
    //        stat = QSystemDeviceInfo::BatteryCritical;
    //    } else if (level < 11) {
    //         stat = QSystemDeviceInfo::BatteryVeryLow;
    //    } else if (level < 41) {
    //         stat =  QSystemDeviceInfo::BatteryLow;
    //    } else if (level > 40) {
    //         stat = QSystemDeviceInfo::BatteryNormal;
    //    }
    //        currentBatStatus = stat;
    //    }
}

void QSystemDeviceInfoLinuxCommonPrivate::setConnection()
{
#if !defined(QT_NO_DBUS)
    if (halIsAvailable) {
        QHalInterface iface;
        QStringList list = iface.findDeviceByCapability("battery");
        if (!list.isEmpty()) {
            QString lastdev;
            foreach (const QString &dev, list) {
                if (lastdev == dev)
                    continue;
                lastdev = dev;
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid()) {
                    const QString batType = halIfaceDevice->getPropertyString("battery.type");
                    if ((batType == "primary" || batType == "pda") &&
                       halIfaceDevice->setConnections() ) {
                            if (!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                        this,SLOT(halChanged(int,QVariantList)))) {
                                qDebug() << "connection malfunction";
                            }
                    }
                    return;
                }
            }
        }

        list = iface.findDeviceByCapability("ac_adapter");
        if (!list.isEmpty()) {
            foreach (const QString &dev, list) {
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid()) {
                    if (halIfaceDevice->setConnections() ) {
                        qDebug() << "connect ac_adapter" ;
                        if (!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                    this,SLOT(halChanged(int,QVariantList)))) {
                            qDebug() << "connection malfunction";
                        }
                    }
                    return;
                }
            }
        }

        list = iface.findDeviceByCapability("battery");
        if (!list.isEmpty()) {
            foreach (const QString &dev, list) {
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid()) {
                    if (halIfaceDevice->setConnections()) {
                        qDebug() << "connect battery" <<  halIfaceDevice->getPropertyString("battery.type");
                        if (!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                    this,SLOT(halChanged(int,QVariantList)))) {
                            qDebug() << "connection malfunction";
                        }
                    }
                    return;
                }
            }
        }

        list = iface.findDeviceByCapability("input.keyboard");
        if (!list.isEmpty()) {
            QStringList btList = iface.findDeviceByCapability("bluetooth_acl");
            foreach (const QString &btdev, btList) {
                foreach (const QString &dev, list) {
                    if (dev.contains(btdev.section("/",-1))) { //ugly, I know.
//                                 qDebug() <<"Found wireless keyboard:"<< dev << btList;
                     //   hasWirelessKeyboard = true;
                        halIfaceDevice = new QHalDeviceInterface(dev);
                        if (halIfaceDevice->isValid() && halIfaceDevice->setConnections()) {
                            if (!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                        this,SLOT(halChanged(int,QVariantList)))) {
                                qDebug() << "connection malfunction";
                            }
                        }
                    }

                }
            }
        }
    }
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && !defined(QT_NO_UDISKS)
    if (uPowerAvailable()) {
        QUPowerInterface *power;
        power = new QUPowerInterface(this);
        connect(power,SIGNAL(changed()),this,(SLOT(upowerChanged())));
        foreach (const QDBusObjectPath &objpath, power->enumerateDevices()) {
            QUPowerDeviceInterface *powerDevice;
            powerDevice = new QUPowerDeviceInterface(objpath.path(),this);
//qDebug() << objpath.path() << powerDevice->getType();
            if (powerDevice->getType() == 2) {
                connect(powerDevice,SIGNAL(changed()),this,SLOT(upowerDeviceChanged()));
            //    return powerDevice.percentLeft();
            }
        }
    }
#endif
#endif
}


#if !defined(QT_NO_DBUS)
void QSystemDeviceInfoLinuxCommonPrivate::halChanged(int,QVariantList map)
{
    for(int i=0; i < map.count(); i++) {
       if (map.at(i).toString() == "battery.charge_level.percentage") {
            const int level = batteryLevel();
            emit batteryLevelChanged(level);
            QSystemDeviceInfo::BatteryStatus stat = QSystemDeviceInfo::NoBatteryLevel;

            if (level < 4) {
                stat = QSystemDeviceInfo::BatteryCritical;
            } else if (level < 11) {
                 stat = QSystemDeviceInfo::BatteryVeryLow;
            } else if (level < 41) {
                 stat =  QSystemDeviceInfo::BatteryLow;
            } else if (level > 40) {
                 stat = QSystemDeviceInfo::BatteryNormal;
            }
            if (currentBatStatus != stat) {
                currentBatStatus = stat;
                Q_EMIT batteryStatusChanged(stat);
            }
        }
        if ((map.at(i).toString() == "ac_adapter.present")
        || (map.at(i).toString() == "battery.rechargeable.is_charging")) {
            QSystemDeviceInfo::PowerState state = currentPowerState();
            emit powerStateChanged(state);
       }

        if ((map.at(i).toString() == "input.keyboard")) {
            qDebug()<<"keyboard changed";

        }
/*

        list = iface.findDeviceByCapability("input.keyboard");
        if (!list.isEmpty()) {
            QStringList btList = iface.findDeviceByCapability("bluetooth_acl");
            foreach (const QString btdev, btList) {
                foreach (const QString dev, list) {
                    if (dev.contains(btdev.section("/",-1))) { //ugly, I know.
                        qDebug() <<"Found wireless keyboard:"<< dev << btList;
                        hasWirelessKeyboard = true;
                    }

                }
            }
        } */
    } //end map
}

void QSystemDeviceInfoLinuxCommonPrivate::upowerChanged()
{
    currentPowerState();
}

void QSystemDeviceInfoLinuxCommonPrivate::upowerDeviceChanged()
{
    initBatteryStatus();
}

#endif

QString QSystemDeviceInfoLinuxCommonPrivate::manufacturer()
{
#if !defined(QT_NO_DBUS)
    if (halIsAvailable) {
        QHalDeviceInterface iface("/org/freedesktop/Hal/devices/computer");
        QString manu;
        if (iface.isValid()) {
            manu = iface.getPropertyString("system.firmware.vendor");
            if (manu.isEmpty()) {
                manu = iface.getPropertyString("system.hardware.vendor");
                if (!manu.isEmpty()) {
                    return manu;
                }
            }
        }
    }
#endif
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
                if (line.contains("vendor_id")) {
                    return line.split(": ").at(1).trimmed();
                }
            }
        }
    }
    return QString();
}


QSystemDeviceInfo::InputMethodFlags QSystemDeviceInfoLinuxCommonPrivate::inputMethodType()
{
    QSystemDeviceInfo::InputMethodFlags methods ;
    if (halIsAvailable) {
#if !defined(QT_NO_DBUS) && !defined(Q_WS_MEEGO)
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
                if (!list.isEmpty()) {
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
            if (methods != 0)
                return methods;
        }
#endif
    }
    const QString inputsPath = "/sys/class/input/";
    const QDir inputDir(inputsPath);
    QStringList filters;
    filters << "event*";
    const QStringList inputList = inputDir.entryList( filters ,QDir::Dirs, QDir::Name);
    foreach (const QString &inputFileName, inputList) {
        QFile file(inputsPath+inputFileName+"/device/name");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug()<<"File not opened";
        } else {
            QString strvalue;
            strvalue = file.readLine();
            file.close();
            if (strvalue.contains("keyboard",Qt::CaseInsensitive)) {
                if ( (methods & QSystemDeviceInfo::Keyboard) != QSystemDeviceInfo::Keyboard) {
                    methods = (methods | QSystemDeviceInfo::Keyboard);
                }
            } else if (strvalue.contains("Mouse",Qt::CaseInsensitive)) {
                if ( (methods & QSystemDeviceInfo::Mouse) != QSystemDeviceInfo::Mouse) {
                    methods = (methods | QSystemDeviceInfo::Mouse);
                }
            } else if (strvalue.contains("Button",Qt::CaseInsensitive)) {
                if ( (methods & QSystemDeviceInfo::Keys) != QSystemDeviceInfo::Keys) {
                    methods = (methods | QSystemDeviceInfo::Keypad);
                }
            } else if (strvalue.contains("keypad",Qt::CaseInsensitive)) {
                if ( (methods & QSystemDeviceInfo::Keypad) != QSystemDeviceInfo::Keypad) {
                    methods = (methods | QSystemDeviceInfo::Keys);
                }
            } else if (strvalue.contains("Touch",Qt::CaseInsensitive)) {
                if ( (methods & QSystemDeviceInfo::SingleTouch) != QSystemDeviceInfo::SingleTouch) {
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
#if !defined(QT_NO_DBUS)
    if (halAvailable()) {
        QHalInterface iface;
        const QStringList list = iface.findDeviceByCapability("battery");
        if (!list.isEmpty()) {
            foreach (const QString &dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (ifaceDevice.isValid()) {
                    if (!ifaceDevice.getPropertyBool("battery.present")
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
    }
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && !defined(QT_NO_UDISKS)
    if (uPowerAvailable()) {
        QUPowerInterface power;
        foreach (const QDBusObjectPath &objpath, power.enumerateDevices()) {
            QUPowerDeviceInterface powerDevice(objpath.path());
            if (powerDevice.getType() == 2) {
                return powerDevice.percentLeft();
            }
        }
    }
#endif
#endif
    QFile infofile("/proc/acpi/battery/BAT0/info");
    if (!infofile.open(QIODevice::ReadOnly)) {
        return QSystemDeviceInfo::NoBatteryLevel;
    } else {
        QTextStream batinfo(&infofile);
        QString line = batinfo.readLine();
        while (!line.isNull()) {
            if (line.contains("last full capacity")) {
                bool ok;
                line = line.simplified();
                QString levels = line.section(" ", 3,3);
                levelWhenFull = levels.toFloat(&ok);
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
            if (line.contains("remaining capacity")) {
                bool ok;
                line = line.simplified();
                QString levels = line.section(" ", 2,2);
                level = levels.toFloat(&ok);
                statefile.close();
                break;
            }
            line = batstate.readLine();
        }
    }
    if (level != 0 && levelWhenFull != 0) {
        level = level / levelWhenFull * 100;
        return level;
    }
    return 0;
}

QSystemDeviceInfo::PowerState QSystemDeviceInfoLinuxCommonPrivate::currentPowerState()
{
#if !defined(QT_NO_DBUS)
    if (halIsAvailable) {
        QHalInterface iface;
        QStringList list = iface.findDeviceByCapability("battery");
        if (!list.isEmpty()) {
            foreach (const QString &dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (iface.isValid()) {
                    if (ifaceDevice.getPropertyBool("battery.rechargeable.is_charging")) {
                        return QSystemDeviceInfo::WallPowerChargingBattery;
                    }
                }
            }
        }

        list = iface.findDeviceByCapability("ac_adapter");
        if (!list.isEmpty()) {
            foreach (const QString &dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (ifaceDevice.isValid()) {
                    if (ifaceDevice.getPropertyBool("ac_adapter.present")) {
                        return QSystemDeviceInfo::WallPower;
                    } else {
                        return QSystemDeviceInfo::BatteryPower;
                    }
                }
            }
        }
    }
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && !defined(QT_NO_UDISKS)
    if (uPowerAvailable()) {
        QSystemDeviceInfo::PowerState pState = QSystemDeviceInfo::UnknownPower;

        QUPowerInterface power(this);
        foreach (const QDBusObjectPath &objpath, power.enumerateDevices()) {
            QUPowerDeviceInterface powerDevice(objpath.path(),this);
            if (powerDevice.getType() == 2) {
                switch(powerDevice.getState()) {
                case 0:
                    break;
                case 1:
                case 5:
                    pState = QSystemDeviceInfo::WallPowerChargingBattery;
                    break;
                case 2:
                case 6:
                    pState = QSystemDeviceInfo::BatteryPower;
                    break;
                case 4:
                    pState = QSystemDeviceInfo::WallPower;
                    break;
                default:
                    pState = QSystemDeviceInfo::UnknownPower;
                };
            }
        }
        if (!power.onBattery() && pState == QSystemDeviceInfo::UnknownPower)
            pState = QSystemDeviceInfo::WallPower;
        if (curPowerState != pState) {
            curPowerState = pState;
            Q_EMIT powerStateChanged(pState);
        }
    return pState;
 }
#endif
#endif
    QFile statefile("/proc/acpi/battery/BAT0/state");
       if (!statefile.open(QIODevice::ReadOnly)) {
       } else {
           QTextStream batstate(&statefile);
           QString line = batstate.readLine();
           while (!line.isNull()) {
               if (line.contains("charging state")) {
                   QString batstate = (line.simplified()).split(" ").at(2);
                   if (batstate == "discharging") {
                       return QSystemDeviceInfo::BatteryPower;
                   }
                   if (batstate == "charging") {
                       return QSystemDeviceInfo::WallPowerChargingBattery;
                   }
               }
               line = batstate.readLine();
           }
       }
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
                 QDBusReply<QVariantMap > reply =  adapterInterface->call(QLatin1String("GetProperties"));
                 QVariant var;
                 QVariantMap map = reply.value();
                 QString property="Powered";
                 if (map.contains(property)) {
                     btPowered = map.value(property).toBool();
                 }

                 property="Devices";
                 if (map.contains(property)) {
                     QList<QDBusObjectPath> devicesList = qdbus_cast<QList<QDBusObjectPath> >(map.value(property));
                     foreach (const QDBusObjectPath &device, devicesList) {
                         //      qDebug() << device.path();
                         QDBusInterface *devadapterInterface = new QDBusInterface("org.bluez",
                                                                                  device.path(),
                                                                                  "org.bluez.Device",
                                                                                  dbusConnection);
                         if (!dbusConnection.connect("org.bluez",
                                                   device.path(),
                                                    "org.bluez.Device",
                                                    "PropertyChanged",
                                                    this,SLOT(bluezPropertyChanged(QString, QDBusVariant)))) {
                             qDebug() << "bluez could not connect signal";
                         }
                         QDBusReply<QVariantMap > reply =  devadapterInterface->call(QLatin1String("GetProperties"));
                         QVariant var;
                         QVariantMap map = reply.value();
                         QString property="Class";
                         //0x002540  9536
                         if (map.contains(property)) {
                             uint classId = map.value(property).toUInt();
                             if ((classId = 9536) &&  map.value("Connected").toBool()) {
                                 // keyboard"
                                 hasWirelessKeyboardConnected = true;
                                 //     qDebug() <<map.value("Name").toString() << map.value(property);
                             }
                         }
                     }
                 }
             }

             adapterInterface = new QDBusInterface("org.bluez",
                                                   reply.value().path(),
                                                   "org.bluez.Input",
                                                   dbusConnection);

             if (adapterInterface->isValid()) {
                 if (!dbusConnection.connect("org.bluez",
                                           reply.value().path(),
                                            "org.bluez.Input",
                                            "PropertyChanged",
                                            this,SLOT(bluezPropertyChanged(QString, QDBusVariant)))) {
                     qDebug() << "bluez could not connect Input signal";
                 }
             }
         }
     }
 }

 void QSystemDeviceInfoLinuxCommonPrivate::bluezPropertyChanged(const QString &str, QDBusVariant v)
  {
     if (str == "Powered") {
             if (btPowered != v.variant().toBool()) {
             btPowered = !btPowered;
             emit bluetoothStateChanged(btPowered);
         }
      }
     if (str == "Connected") {
         bool conn =  v.variant().toBool();

         QDBusInterface *devadapterInterface = new QDBusInterface("org.bluez",
                                                                  str,
                                                                  "org.bluez.Device",
                                                                   QDBusConnection::systemBus());
         QDBusReply<QVariantMap > reply =  devadapterInterface->call(QLatin1String("GetProperties"));
         QVariant var;
         QVariantMap map = reply.value();
         QString property="Class";
         //0x002540  9536
         if (map.contains(property)) {
             uint classId = map.value(property).toUInt();
             if ((classId = 9536) &&  conn) {
                 // keyboard"
                     hasWirelessKeyboardConnected = conn;
                     emit wirelessKeyboardConnected(conn);
                     //          qDebug() <<map.value("Name").toString() << map.value(property);
             }
         }
     }
      // Pairable Name Class Discoverable
  }
 #endif

 bool QSystemDeviceInfoLinuxCommonPrivate::currentBluetoothPowerState()
 {
     return btPowered;
 }


 QSystemDeviceInfo::KeyboardTypeFlags QSystemDeviceInfoLinuxCommonPrivate::keyboardTypes()
 {
     QSystemDeviceInfo::InputMethodFlags methods = inputMethodType();
     QSystemDeviceInfo::KeyboardTypeFlags keyboardFlags;

     if ((methods & QSystemDeviceInfo::Keyboard)) {
         keyboardFlags = (keyboardFlags | QSystemDeviceInfo::FullQwertyKeyboard);
   }
     if (isWirelessKeyboardConnected()) {
         keyboardFlags = (keyboardFlags | QSystemDeviceInfo::WirelessKeyboard);
     }
// how to check softkeys on desktop?
     return keyboardFlags;
 }

 bool QSystemDeviceInfoLinuxCommonPrivate::isWirelessKeyboardConnected()
 {
     return hasWirelessKeyboardConnected;
 }

 bool QSystemDeviceInfoLinuxCommonPrivate::isKeyboardFlippedOpen()
 {
     return false;
 }

void QSystemDeviceInfoLinuxCommonPrivate::keyboardConnected(bool connect)
{
    if (connect != hasWirelessKeyboardConnected)
        hasWirelessKeyboardConnected = connect;
    Q_EMIT wirelessKeyboardConnected(connect);
}

bool QSystemDeviceInfoLinuxCommonPrivate::keypadLightOn(QSystemDeviceInfo::KeypadType /*type*/)
{
    return false;
}

QUuid QSystemDeviceInfoLinuxCommonPrivate::uniqueDeviceID()
{
#if defined(Q_WS_MAEMO_6)
    // create one from imei and uuid of /
    QByteArray driveuid;
    if (halIsAvailable) {
        QHalInterface iface;
        QStringList list = iface.findDeviceByCapability("volume");
        if (!list.isEmpty()) {
            QString lastdev;
            foreach (const QString &dev, list) {
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid() && (halIfaceDevice->getPropertyString("volume.mount_point") == "/")) {
                    driveuid =  halIfaceDevice->getPropertyString("volume.uuid").toLocal8Bit();
                    break;
                }
            }
        }
    }
    QByteArray bytes = imei().toLocal8Bit();
    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(bytes);
    hash.addData(driveuid);

    return QUuid(QString(hash.result().toHex()));
#endif
#if !defined(QT_NO_DBUS)
    if (halIsAvailable) {
        QHalDeviceInterface iface("/org/freedesktop/Hal/devices/computer");
        QString id;
        if (iface.isValid()) {
            id = iface.getPropertyString("system.hardware.uuid");
            return QUuid(id);
        }
    }
#if defined(Q_WS_MEEGO)
    QDBusInterface connectionInterface("org.freedesktop.PolicyKit1",
                                       "/org/freedesktop/PolicyKit1/Authority",
                                       "org.freedesktop.DBus.Peer",
                                        QDBusConnection::systemBus());
    if(!connectionInterface.isValid()) {
        qDebug() <<connectionInterface.lastError().message()<< "not valid";
    }

    QDBusReply< QString > reply = connectionInterface.call("GetMachineId");
    QString uid = reply.value();
// grrrrrr PolicyKit returns a malformed uuid
    uid.insert(8,"-");
    uid.insert(13,"-");
    uid.insert(18,"-");
    uid.insert(23,"-");
    return uid;
#endif
#endif
    return QUuid(QString::number(gethostid()));
}

QSystemDeviceInfo::LockTypeFlags QSystemDeviceInfoLinuxCommonPrivate::lockStatus()
{
    return QSystemDeviceInfo::UnknownLock;
}

QString QSystemDeviceInfoLinuxCommonPrivate::model()
{
#if !defined(QT_NO_DBUS)
    QString productName = sysinfodValueForKey("/component/product-name");
    if (!productName.isEmpty()) {
        return productName.split("/").at(0);
    }
#endif
    if (halAvailable()) {
#if !defined(QT_NO_DBUS)
        QHalDeviceInterface iface("/org/freedesktop/Hal/devices/computer");
        QString model;
        if (iface.isValid()) {
            model = iface.getPropertyString("system.kernel.machine");
            if (!model.isEmpty())
                model += " ";
            model += iface.getPropertyString("system.chassis.type");
            if (!model.isEmpty())
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
            if (line.contains("model name")) {
                return line.split(": ").at(1).trimmed();
            }
        }
    }
    return QString();
}

QString QSystemDeviceInfoLinuxCommonPrivate::productName()
{
#if !defined(QT_NO_DBUS)
    QString productName = sysinfodValueForKey("/component/product-name");
    if (!productName.isEmpty()) {
        return productName;
    }
#endif
    if (halAvailable()) {
#if !defined(QT_NO_DBUS)
        QHalDeviceInterface iface("/org/freedesktop/Hal/devices/computer");
        QString productName;
        if (iface.isValid()) {
            productName = iface.getPropertyString("info.product");
            if (productName.isEmpty()) {
                productName = iface.getPropertyString("system.product");
                if (!productName.isEmpty())
                    return productName;
            } else {
                return productName;
            }
        }
#endif
    }
    const QDir dir("/etc");
    if (dir.exists()) {
        QStringList langList;
        QFileInfoList localeList = dir.entryInfoList(QStringList() << "*release",
                                                     QDir::Files | QDir::NoDotAndDotDot,
                                                     QDir::Name);
        foreach (const QFileInfo &fileInfo, localeList) {
            const QString filepath = fileInfo.filePath();
            QFile file(filepath);
            if (file.open(QIODevice::ReadOnly)) {
                QTextStream prodinfo(&file);
                QString line = prodinfo.readLine();
                while (!line.isNull()) {
                    if (filepath.contains("lsb.release")) {
                        if (line.contains("DISTRIB_DESCRIPTION")) {
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
            if (!line.isEmpty()) {
                QStringList lineList = line.split(" ");
                for(int i = 0; i < lineList.count(); i++) {
                    if (lineList.at(i).toFloat()) {
                        return lineList.at(i-1) + " "+ lineList.at(i);
                    }
                }
            }
        }
    }
    return QString();
}

QSystemScreenSaverLinuxCommonPrivate::QSystemScreenSaverLinuxCommonPrivate(QObject *parent)
    : QObject(parent)
{

}

QSystemScreenSaverLinuxCommonPrivate::~QSystemScreenSaverLinuxCommonPrivate()
{
}


QSystemBatteryInfoLinuxCommonPrivate::QSystemBatteryInfoLinuxCommonPrivate(QObject *parent)
: QObject(parent), batteryIsPresent(0)
{
#if !defined(QT_NO_DBUS)
    halIsAvailable = halAvailable();
    setConnection();
#endif
    getBatteryStats();
}

QSystemBatteryInfoLinuxCommonPrivate::~QSystemBatteryInfoLinuxCommonPrivate()
{
}


QSystemBatteryInfo::ChargerType QSystemBatteryInfoLinuxCommonPrivate::chargerType() const
{
    return curChargeType;
}

QSystemBatteryInfo::ChargingState QSystemBatteryInfoLinuxCommonPrivate::chargingState() const
{
    return curChargeState;
}


int QSystemBatteryInfoLinuxCommonPrivate::nominalCapacity() const
{
    return capacity;
}

int QSystemBatteryInfoLinuxCommonPrivate::remainingCapacityPercent() const
{
    return currentBatLevelPercent;
}

int QSystemBatteryInfoLinuxCommonPrivate::remainingCapacity() const
{
    return remainingEnergy;
}


int QSystemBatteryInfoLinuxCommonPrivate::voltage() const
{
    return currentVoltage;
}

int QSystemBatteryInfoLinuxCommonPrivate::remainingChargingTime() const
{
    return timeToFull;
}

int QSystemBatteryInfoLinuxCommonPrivate::currentFlow() const
{
    return dischargeRate;
}

int QSystemBatteryInfoLinuxCommonPrivate::remainingCapacityBars() const
{
    return 0;
}

int QSystemBatteryInfoLinuxCommonPrivate::maxBars() const
{
    return 0;
}

QSystemBatteryInfo::BatteryStatus QSystemBatteryInfoLinuxCommonPrivate::batteryStatus() const
{
    return currentBatStatus;
}

void QSystemBatteryInfoLinuxCommonPrivate::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            batteryLevelChanged(int))))) {
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            batteryStatusChanged(QSystemBatteryInfo::BatteryStatus))))) {
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            chargingStateChanged(QSystemBatteryInfo::ChargingState))))) {
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            chargerTypeChanged(QSystemBatteryInfo::ChargerType))))) {
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            nominalCapacityChanged(int))))) {
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingCapacityPercentChanged(int))))) {
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingCapacityChanged(int))))) {
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            currentFlowChanged(int))))) {
    }

    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingCapacityBarsChanged(int))))) {
    }
    if (QLatin1String(signal) == QLatin1String(QMetaObject::normalizedSignature(SIGNAL(
            remainingChargingTimeChanged(int))))) {
    }

}

void QSystemBatteryInfoLinuxCommonPrivate::disconnectNotify(const char */*signal*/)
{

}


void QSystemBatteryInfoLinuxCommonPrivate::setConnection()
{
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && !defined(QT_NO_UDISKS)
    if (uPowerAvailable()) {
        QUPowerInterface *power;
        power = new QUPowerInterface(this);
   //     connect(power,SIGNAL(changed()),this,(SLOT(upowerChanged())));
        foreach (const QDBusObjectPath &objpath, power->enumerateDevices()) {
            QUPowerDeviceInterface *powerDevice;
            powerDevice = new QUPowerDeviceInterface(objpath.path(),this);
            if (powerDevice->getType() == 2) {
                battery = new QUPowerDeviceInterface(objpath.path(),this);
                pMap = battery->getProperties();
                connect(battery,SIGNAL(propertyChanged(QString,QVariant)),
                        this,SLOT(propertyChanged(QString,QVariant)));
            }
//            connect(powerDevice,SIGNAL(changed()),this,SLOT(upowerDeviceChanged()));
//            connect(powerDevice,SIGNAL(propertyChanged(QString,QVariant)),
//                    this,SLOT(propertyChanged(QString,QVariant)));
            //    return powerDevice.percentLeft();
           // }
        }
    }
#endif
#if !defined(QT_NO_DBUS)
    if (halIsAvailable) {
        QHalInterface iface;
        QStringList list = iface.findDeviceByCapability("battery");
        if (!list.isEmpty()) {
            QString lastdev;
            foreach (const QString &dev, list) {
                if (lastdev == dev)
                    continue;
                lastdev = dev;
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid()) {
                    const QString batType = halIfaceDevice->getPropertyString("battery.type");
                    if ((batType == "primary" || batType == "pda") &&
                            halIfaceDevice->setConnections() ) {
                        if (!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                    this,SLOT(halChanged(int,QVariantList)))) {
                            qDebug() << "connection malfunction";
                        }
                    }
                }
            }
        }

        list = iface.findDeviceByCapability("ac_adapter");
        if (!list.isEmpty()) {
            foreach (const QString &dev, list) {
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid()) {
                    if (halIfaceDevice->setConnections() ) {
                        qDebug() << "connect ac_adapter" ;
                        if (!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                    this,SLOT(halChanged(int,QVariantList)))) {
                            qDebug() << "connection malfunction";
                        }
                    }
                    return;
                }
            }
        }

        list = iface.findDeviceByCapability("battery");
        if (!list.isEmpty()) {
            foreach (const QString &dev, list) {
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid()) {
                    if (halIfaceDevice->setConnections()) {
                        qDebug() << "connect battery" <<  halIfaceDevice->getPropertyString("battery.type");
                        if (!connect(halIfaceDevice,SIGNAL(propertyModified(int, QVariantList)),
                                    this,SLOT(halChanged(int,QVariantList)))) {
                            qDebug() << "connection malfunction";
                        }
                    }
                    return;
                }
            }
        }
    }
#endif
}

#if !defined(QT_NO_DBUS)
void QSystemBatteryInfoLinuxCommonPrivate::halChanged(int count,QVariantList map)
{
    QHalInterface iface;
    QStringList list = iface.findDeviceByCapability("battery");
    QHalDeviceInterface ifaceDevice(list.at(0)); //default battery
    if (ifaceDevice.isValid()) {
        for(int i=0; i < count; i++) {
            QString mapS = map.at(i).toString();
            if (mapS == "battery.present") {
                batteryIsPresent = true;

            }
            if (mapS == "battery.charge_level.percentage") {
                currentBatLevelPercent = ifaceDevice.getPropertyInt("battery.charge_level.percentage");
                emit remainingCapacityPercentChanged(currentBatLevelPercent);

                QSystemBatteryInfo::BatteryStatus stat = QSystemBatteryInfo::BatteryUnknown;

                if (currentBatLevelPercent == 0) {
                    stat = QSystemBatteryInfo::BatteryEmpty;
                } else if (currentBatLevelPercent < 4) {
                    stat = QSystemBatteryInfo::BatteryCritical;
                } else if (currentBatLevelPercent < 11) {
                    stat = QSystemBatteryInfo::BatteryVeryLow;
                } else if (currentBatLevelPercent < 41) {
                    stat =  QSystemBatteryInfo::BatteryLow;
                } else if (currentBatLevelPercent > 40 && currentBatLevelPercent < 100) {
                    stat = QSystemBatteryInfo::BatteryOk;
                } else if (currentBatLevelPercent == 100) {
                    stat = QSystemBatteryInfo::BatteryFull;
                }
                Q_EMIT batteryStatusChanged(currentBatStatus);
            }

            if (mapS == "ac_adapter.present") {
                if (curChargeType != QSystemBatteryInfo::WallCharger) {
                    curChargeType = QSystemBatteryInfo::WallCharger;
                } else {
                    curChargeType = QSystemBatteryInfo::NoCharger;
                }
                Q_EMIT chargerTypeChanged(curChargeType);
            }

            if (mapS == "battery.rechargeable.is_charging") {

                if (ifaceDevice.getPropertyBool("battery.rechargeable.is_charging")) {
                    curChargeState = QSystemBatteryInfo::Charging;
                } else {
                    curChargeState = QSystemBatteryInfo::NotCharging;
                }
                Q_EMIT chargingStateChanged(curChargeState);
            }

            if (mapS == "battery.voltage.current") {
                currentVoltage = ifaceDevice.getPropertyInt("battery.voltage.current");
            }

            if (mapS == "battery.charge_level.rate") {
                dischargeRate = ifaceDevice.getPropertyInt("battery.charge_level.rate");
                Q_EMIT currentFlowChanged(dischargeRate);
            }

            if (mapS == "battery.reporting.last_full") {
                capacity = ifaceDevice.getPropertyInt("battery.reporting.last_full");
                Q_EMIT nominalCapacityChanged(capacity);
            }

            if (mapS == "battery.reporting.current") {
                if(ifaceDevice.getPropertyBool("battery.rechargeable.is_charging")) {
                    remainingEnergy = ifaceDevice.getPropertyInt("battery.reporting.current");
                } else {
                    remainingEnergy = -1;
                }
                Q_EMIT remainingCapacityChanged(remainingEnergy);
            }

            if (mapS == "battery.remaining_time") {
                if(ifaceDevice.getPropertyBool("battery.rechargeable.is_charging")) {
                    remainingEnergy = ifaceDevice.getPropertyInt("battery.remaining_time");
                    Q_EMIT remainingChargingTimeChanged(remainingEnergy);
                }
            }

        }
    } else {
        currentBatLevelPercent = 0;
        currentBatStatus = QSystemBatteryInfo::BatteryUnknown;
        curChargeType = QSystemBatteryInfo::WallCharger;
        curChargeState = QSystemBatteryInfo::NotCharging;
        currentVoltage = 0;
        dischargeRate = 0;
        capacity = 0;
        remainingEnergy = 0;
    }
}

void QSystemBatteryInfoLinuxCommonPrivate::getBatteryStats()
{
    int cLevel = 0;
    int cEnergy = 0;
    int cVoltage = 0;
    int cTime = 0;
    int rEnergy = 0;

    QSystemBatteryInfo::ChargingState cState = QSystemBatteryInfo::ChargingError;
    QSystemBatteryInfo::ChargerType cType = QSystemBatteryInfo::UnknownCharger;

#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && !defined(QT_NO_UDISKS)
    if (uPowerAvailable()) {

        QUPowerInterface power(this);
        foreach (const QDBusObjectPath &objpath, power.enumerateDevices()) {
            QUPowerDeviceInterface powerDevice(objpath.path(),this);
            if (!powerDevice.isPowerSupply())
                continue;
            if (powerDevice.getType() == 1) { //line power
                if(powerDevice.isOnline()) {
                    cType = QSystemBatteryInfo::WallCharger;
                } else {
                    cType = QSystemBatteryInfo::NoCharger;
                }
            }

            if (powerDevice.getType() == 2) { //battery power
                batteryIsPresent = true;
                switch(powerDevice.getState()) {
                case 1: // charging
                    {
                        cState = QSystemBatteryInfo::Charging;
                    }
                    break;
                case 2: //discharging
                case 3: //empty
                case 4: //fully charged
                case 5: //pending charge
                case 6: //pending discharge
                    cState = QSystemBatteryInfo::NotCharging;
                    break;
                default:
                    cState = QSystemBatteryInfo::ChargingError;
                    break;
                };
                //energy flow
                cVoltage = powerDevice.voltage();
                cEnergy = (powerDevice.energyDischargeRate() / cVoltage) * 1000;
                cLevel = powerDevice.percentLeft();
                capacity = (powerDevice.energyWhenFull() / cVoltage) * 1000;
                cTime = powerDevice.timeToFull();
                if (cState != QSystemBatteryInfo::Charging) {
                    cTime = -1;
                }
                rEnergy = (powerDevice.currentEnergy() / cVoltage) * 1000;
                cVoltage =  powerDevice.voltage() * 1000; // mV
                break;
            }

        } //end enumerateDevices
    }
#endif
#if !defined(QT_NO_DBUS)
    if (halIsAvailable) {

        QHalInterface iface;
        QStringList list = iface.findDeviceByCapability("battery");
        if (!list.isEmpty()) {
            foreach (const QString &dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (iface.isValid()) {
                    batteryIsPresent = (ifaceDevice.getPropertyBool("battery.present")
                    && (ifaceDevice.getPropertyString("battery.type") == "pda"
                        || ifaceDevice.getPropertyString("battery.type") == "primary"));
                    if (ifaceDevice.getPropertyBool("battery.rechargeable.is_charging")) {
                        cState = QSystemBatteryInfo::Charging;
                        cTime = ifaceDevice.getPropertyInt("battery.remaining_time");
                    } else {
                        cState = QSystemBatteryInfo::NotCharging;
                        cTime = 0;
                    }
                    cVoltage = ifaceDevice.getPropertyInt("battery.voltage.current");
                    cEnergy = ifaceDevice.getPropertyInt("battery.charge_level.rate");
                    cLevel = ifaceDevice.getPropertyInt("battery.charge_level.percentage");
                    capacity = ifaceDevice.getPropertyInt("battery.reporting.last_full");
                    rEnergy = ifaceDevice.getPropertyInt("battery.reporting.current");
                    break;
                }
            }
        } else {
            cLevel = -1;
            cType = QSystemBatteryInfo::UnknownCharger;
            cState = QSystemBatteryInfo::NotCharging;
            cVoltage = -1;
            cEnergy = 0;
            capacity = -1;
            rEnergy = -1;
            cTime = -1;
        }

        list = iface.findDeviceByCapability("ac_adapter");
        if (!list.isEmpty()) {
            foreach (const QString &dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (ifaceDevice.isValid()) {
                    if (ifaceDevice.getPropertyBool("ac_adapter.present")) {
                        cType = QSystemBatteryInfo::WallCharger;
                        break;
                    } else {
                        cType = QSystemBatteryInfo::NoCharger;
                        break;
                    }
                }
            }
        }
    }
#endif

    curChargeType = cType;
    currentVoltage = cVoltage;
    curChargeState = cState;
    dischargeRate = cEnergy;
    currentBatLevelPercent = cLevel;
    timeToFull = cTime;
    remainingEnergy = rEnergy;

    QSystemBatteryInfo::BatteryStatus stat = QSystemBatteryInfo::BatteryUnknown;
    if(batteryIsPresent) {
        if (cLevel == 0) {
            stat = QSystemBatteryInfo::BatteryEmpty;
        } else if (cLevel < 4) {
            stat = QSystemBatteryInfo::BatteryCritical;
        } else if (cLevel < 11) {
            stat = QSystemBatteryInfo::BatteryVeryLow;
        } else if (cLevel < 41) {
            stat =  QSystemBatteryInfo::BatteryLow;
        } else if (cLevel > 40 && cLevel < 99) {
            stat = QSystemBatteryInfo::BatteryOk;
        } else if (cLevel == 100) {
            stat = QSystemBatteryInfo::BatteryFull;
        }
    }
    currentBatStatus = stat;
}
#endif

void QSystemBatteryInfoLinuxCommonPrivate::timeout()
{

}

QSystemBatteryInfo::EnergyUnit QSystemBatteryInfoLinuxCommonPrivate::energyMeasurementUnit() const
{
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && !defined(QT_NO_UDISKS)
    if(uPowerAvailable() && batteryIsPresent) {
        return QSystemBatteryInfo::UnitmWh;
    }
#endif
#if !defined(QT_NO_DBUS)
    if (halIsAvailable && batteryIsPresent) {
        return QSystemBatteryInfo::UnitmWh;
    }
#endif
    return QSystemBatteryInfo::UnitUnknown;
}

int QSystemBatteryInfoLinuxCommonPrivate::batteryLevel() const
{
    float levelWhenFull = 0.0;
    float level = 0.0;
#if !defined(QT_NO_DBUS)
    if (halAvailable()) {
        QHalInterface iface;
        const QStringList list = iface.findDeviceByCapability("battery");
        if (!list.isEmpty()) {
            foreach (const QString &dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (ifaceDevice.isValid()) {
                    if (!ifaceDevice.getPropertyBool("battery.present")
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
    }
#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5) && !defined(QT_NO_UDISKS)
    if (uPowerAvailable()) {
        QUPowerInterface power;
        foreach (const QDBusObjectPath &objpath, power.enumerateDevices()) {
            QUPowerDeviceInterface powerDevice(objpath.path());
            if (powerDevice.getType() == 2) {
                return powerDevice.percentLeft();
            }
        }
    }
#endif
#endif
    QFile infofile("/proc/acpi/battery/BAT0/info");
    if (!infofile.open(QIODevice::ReadOnly)) {
        return QSystemDeviceInfo::NoBatteryLevel;
    } else {
        QTextStream batinfo(&infofile);
        QString line = batinfo.readLine();
        while (!line.isNull()) {
            if (line.contains("last full capacity")) {
                bool ok;
                line = line.simplified();
                QString levels = line.section(" ", 3,3);
                levelWhenFull = levels.toFloat(&ok);
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
            if (line.contains("remaining capacity")) {
                bool ok;
                line = line.simplified();
                QString levels = line.section(" ", 2,2);
                level = levels.toFloat(&ok);
                statefile.close();
                break;
            }
            line = batstate.readLine();
        }
    }
    if (level != 0 && levelWhenFull != 0) {
        level = level / levelWhenFull * 100;
        return level;
    }
    return 0;
}

#if !defined(Q_WS_MAEMO_6) && !defined(Q_WS_MAEMO_5)
void QSystemBatteryInfoLinuxCommonPrivate::propertyChanged(const QString & prop, const QVariant &v)
{
 //   qDebug() << __FUNCTION__ << prop << v;

     if (prop == QLatin1String("Energy")) {
        remainingEnergy = (v.toDouble() /  battery->voltage()) * 1000;
        emit remainingCapacityChanged(remainingEnergy);
    } else if (prop == QLatin1String("EnergyRate")) {
        dischargeRate = (v.toDouble() / battery->voltage()) * 1000;
       emit currentFlowChanged(dischargeRate);
    } else if (prop == QLatin1String("Percentage")) {
        currentBatLevelPercent = v.toUInt();
        emit remainingCapacityPercentChanged(currentBatLevelPercent);

        QSystemBatteryInfo::BatteryStatus stat = QSystemBatteryInfo::BatteryUnknown;

        if (currentBatLevelPercent < 4) {
            stat = QSystemBatteryInfo::BatteryCritical;
        } else if (currentBatLevelPercent < 11) {
             stat = QSystemBatteryInfo::BatteryVeryLow;
        } else if (currentBatLevelPercent < 41) {
             stat =  QSystemBatteryInfo::BatteryLow;
        } else if (currentBatLevelPercent > 40 && currentBatLevelPercent < 100) {
             stat = QSystemBatteryInfo::BatteryOk;
        } else if (currentBatLevelPercent == 100) {
             stat = QSystemBatteryInfo::BatteryFull;
        }
        if(currentBatStatus != stat) {
            currentBatStatus = stat;
            emit batteryStatusChanged(stat);
        }

    } else if (prop == QLatin1String("Voltage")) {
        currentVoltage = v.toDouble() * 1000;
    } else if (prop == QLatin1String("State")) {
        switch(v.toUInt()) {
        case 1: // charging
            {
                curChargeState = QSystemBatteryInfo::Charging;
                curChargeType = QSystemBatteryInfo::WallCharger;
            }
            break;
        case 2: //discharging
        case 3: //empty
        case 4: //fully charged
        case 5: //pending charge
        case 6: //pending discharge
            curChargeState = QSystemBatteryInfo::NotCharging;
            curChargeType = QSystemBatteryInfo::NoCharger;
            break;
        default:
            curChargeState = QSystemBatteryInfo::ChargingError;
            break;
        };
        emit chargingStateChanged(curChargeState);
        emit chargerTypeChanged(curChargeType);
    } else if (prop == QLatin1String("Capacity")) {
    } else if (prop == QLatin1String("UpdateTime")) {
    } else if (prop == QLatin1String("TimeToFull")) {
        timeToFull = v.toUInt();
        emit remainingChargingTimeChanged(timeToFull);
    } else if (prop == QLatin1String("Type")) {
    }

}
#endif

#include "moc_qsysteminfo_linux_common_p.cpp"

QTM_END_NAMESPACE
