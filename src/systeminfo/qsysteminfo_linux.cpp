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
#include <qsysteminfo_linux_p.h>

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
#ifndef QT_NO_NETWORKMANAGER
#include <qnetworkmanagerservice_linux_p.h>
#endif
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



QTM_BEGIN_NAMESPACE

static bool halAvailable()
{
#if !defined(QT_NO_DBUS)
    QDBusConnection dbusConnection = QDBusConnection::systemBus();
    if (dbusConnection.isConnected()) {
        QDBusConnectionInterface *dbiface = dbusConnection.interface();
        QDBusReply<bool> reply = dbiface->isServiceRegistered(QLatin1String("org.freedesktop.Hal"));
        if (reply.isValid() && reply.value()) {
            return reply.value();
        }
    }
#endif
    return false;
}


bool halIsAvailable;
QSystemInfoPrivate::QSystemInfoPrivate(QSystemInfoLinuxCommonPrivate *parent)
 : QSystemInfoLinuxCommonPrivate(parent)
{
    halIsAvailable = halAvailable();
}

QSystemInfoPrivate::~QSystemInfoPrivate()
{
}

QStringList QSystemInfoPrivate::availableLanguages() const
{
    QDir transDir(QLibraryInfo::location (QLibraryInfo::TranslationsPath));
    QStringList langList;

    if(transDir.exists()) {
        QStringList localeList = transDir.entryList( QStringList() << QLatin1String("qt_*.qm") ,QDir::Files
                                                     | QDir::NoDotAndDotDot, QDir::Name);
        foreach(QString localeName, localeList) {
            QString lang = localeName.mid(3,2);
            if(!langList.contains(lang) && !lang.isEmpty() && !lang.contains(QLatin1String("help"))) {
                langList <<lang;
            }
        }
        if(langList.count() > 0) {
            return langList;
        }
    }
    return QStringList() << currentLanguage();
}

QString QSystemInfoPrivate::version(QSystemInfo::Version type,
                                    const QString &parameter)
{
    QString errorStr = QLatin1String("Not Available");

    bool useDate = false;
    if(parameter == QLatin1String("versionDate")) {
        useDate = true;
    }

    switch(type) {
        case QSystemInfo::Firmware :
        {
#if !defined(QT_NO_DBUS)
            QHalDeviceInterface iface(QLatin1String("/org/freedesktop/Hal/devices/computer"));
            QString str;
            if (iface.isValid()) {
                str = iface.getPropertyString(QLatin1String("system.kernel.version"));
                if(!str.isEmpty()) {
                    return str;
                }
                if(useDate) {
                    str = iface.getPropertyString(QLatin1String("system.firmware.release_date"));
                    if(!str.isEmpty()) {
                        return str;
                    }
                } else {
                    str = iface.getPropertyString(QLatin1String("system.firmware.version"));
                    if(str.isEmpty()) {
                        if(!str.isEmpty()) {
                            return str;
                        }
                    }
                }
            }
            break;
#endif            
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
            if(ifaceDevice.getPropertyString(QLatin1String("info.subsystem")) == QLatin1String("usb_device")) {
                if(ifaceDevice.getPropertyInt(QLatin1String("usb.interface.class")) == usbClass) {
                    return true;
                }
            }
        }
    }
    return false;
}
#endif

bool QSystemInfoPrivate::hasFeatureSupported(QSystemInfo::Feature feature)
{
    bool featureSupported = false;
    switch (feature) {
    case QSystemInfo::BluetoothFeature :
        {
            QString sysPath = QLatin1String("/sys/class/bluetooth/");
            QDir sysDir(sysPath);
            QStringList filters;
            filters << QLatin1String("*");
            QStringList sysList = sysDir.entryList( filters ,QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
            foreach(QString dir, sysList) {
                QFileInfo btFile(sysPath + dir+QLatin1String("/address"));
                if(btFile.exists()) {
                    return true;
                }
            }
        }
    break;
    case QSystemInfo::CameraFeature :
        {
#if !defined(QT_NO_DBUS)
            featureSupported = hasHalUsbFeature(0x06);
            if(featureSupported)
                return featureSupported;
#endif
            featureSupported = hasSysFeature(QLatin1String("video"));
        }
        break;
    case QSystemInfo::FmradioFeature :
        {
            QString sysPath = QLatin1String("/sys/class/video4linux/");
            QDir sysDir(sysPath);
            QStringList filters;
            filters << QLatin1String("*");
            QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
            if(sysList.contains(QLatin1String("radio"))) {
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
        featureSupported = hasSysFeature(QLatin1String("irda"));
    }
        break;
    case QSystemInfo::LedFeature :
        {
            featureSupported = hasSysFeature(QLatin1String("led"));
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
                        if(ifaceDevice.getPropertyString(QLatin1String("info.subsystem")) == QLatin1String("mmc_host")) {
                            return true;
                        }
                        if(ifaceDevice.getPropertyBool(QLatin1String("storage.removable"))) {
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
        featureSupported = hasHalDeviceFeature(QLatin1String("usb"));
        if(featureSupported)
            return featureSupported;
#endif
            featureSupported = hasSysFeature(QLatin1String("usb_host"));
        }
        break;
    case QSystemInfo::VibFeature :
#if !defined(QT_NO_DBUS)
        featureSupported = hasHalDeviceFeature(QLatin1String("vibrator"));
        if(featureSupported)
            return featureSupported;
#endif
        break;
    case QSystemInfo::WlanFeature :
        {
#if !defined(QT_NO_DBUS)
            QHalInterface iface;
            if (iface.isValid()) {
                QStringList list = iface.findDeviceByCapability(QLatin1String("net.80211"));
                if(!list.isEmpty()) {
                    featureSupported = true;
                    break;
                }
            }
#endif
            featureSupported = hasSysFeature(QLatin1String("80211"));
        }
        break;
    case QSystemInfo::SimFeature :
        break;
    case QSystemInfo::LocationFeature :
#if !defined(QT_NO_DBUS)
        featureSupported = hasHalDeviceFeature(QLatin1String("gps"));
        if(featureSupported)
            return featureSupported;

#endif
        break;
    case QSystemInfo::VideoOutFeature :
        {
            QString sysPath = QLatin1String("/sys/class/video4linux/");
            QDir sysDir(sysPath);
            QStringList filters;
            filters << QLatin1String("*");
            QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
            if(sysList.contains(QLatin1String("video"))) {
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

QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QSystemNetworkInfoLinuxCommonPrivate *parent)
        : QSystemNetworkInfoLinuxCommonPrivate(parent)
{
#if !defined(QT_NO_NETWORKMANAGER)
    setupNmConnections();
    updateActivePaths();
    QTimer::singleShot(200, this,SLOT(getPrimaryMode()));
#endif
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
}

#if !defined(QT_NO_NETWORKMANAGER)
void QSystemNetworkInfoPrivate::setupNmConnections()
{
    iface = new QNetworkManagerInterface(this);

   foreach(QDBusObjectPath path, iface->getDevices()) {
        QNetworkManagerInterfaceDevice *devIface = new QNetworkManagerInterfaceDevice(path.path());

        switch(devIface->deviceType()) {
        case DEVICE_TYPE_802_3_ETHERNET:
            {
                devWiredIface = new QNetworkManagerInterfaceDeviceWired(devIface->path());
                devWiredIface->setConnections();
                connect(devWiredIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                        this,SLOT(nmPropertiesChanged( const QString &, QMap<QString,QVariant>)));
            }
            break;
        case DEVICE_TYPE_802_11_WIRELESS:
            {
                devWirelessIface = new QNetworkManagerInterfaceDeviceWireless(devIface->path());
                devWirelessIface->setConnections();

                connect(devWirelessIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                        this,SLOT(nmPropertiesChanged( const QString &, QMap<QString,QVariant>)));

                if(devWirelessIface->activeAccessPoint().path().length() > 2) {
                    accessPointIface = new QNetworkManagerInterfaceAccessPoint(devWirelessIface->activeAccessPoint().path());
                    accessPointIface->setConnections();
                    connect(accessPointIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                            this,SLOT(nmAPPropertiesChanged( const QString &, QMap<QString,QVariant>)));
                }
            }
            break;
        default:
            break;
        };
    }
}


bool QSystemNetworkInfoPrivate::isDefaultConnectionPath(const QString &path)
{
    bool isDefault = false;

    QMapIterator<QString, QString> i(activePaths);
    QString devicepath;
    while (i.hasNext()) {
        i.next();
        QScopedPointer<QNetworkManagerConnectionActive> activeCon;
        activeCon.reset(new QNetworkManagerConnectionActive(i.key()));
        if(i.value() == path) {
            isDefault = activeCon->defaultRoute();
        }
    }
    return isDefault;
}

void QSystemNetworkInfoPrivate::getPrimaryMode()
{
    // try to see if there are any default route
    bool anyDefaultRoute = false;

    QMapIterator<QString, QString> i(activePaths);
    QString devicepath;
    while (i.hasNext()) {
        i.next();
        QScopedPointer<QNetworkManagerConnectionActive> activeCon;
        activeCon.reset(new QNetworkManagerConnectionActive(i.key()));

        if(activeCon->defaultRoute()) {
            anyDefaultRoute = activeCon->defaultRoute();
            QNetworkManagerInterfaceDevice *devIface = new QNetworkManagerInterfaceDevice(i.value());
            emit networkModeChanged(deviceTypeToMode(devIface->deviceType()));
        }
        devicepath = i.value();
    }

    if(!anyDefaultRoute) {
        emit networkModeChanged(QSystemNetworkInfo::UnknownMode);
    }
}


QString QSystemNetworkInfoPrivate::getNmNetName(QSystemNetworkInfo::NetworkMode mode)
{
    QString devPath;
    QMapIterator<QString, QString> i(activePaths);
    QString devicepath;
    while (i.hasNext()) {
        i.next();
        devicepath = i.value();
        QScopedPointer<QNetworkManagerInterfaceDevice> devIface;
        devIface.reset(new QNetworkManagerInterfaceDevice(devicepath));

        if(deviceTypeToMode(devIface->deviceType()) == mode) {
            devPath = i.value();

            if(mode == QSystemNetworkInfo::WlanMode
               && devIface->deviceType() == DEVICE_TYPE_802_11_WIRELESS) {

                QNetworkManagerInterfaceDeviceWireless *devWirelessIfaceL;
                devWirelessIfaceL = new QNetworkManagerInterfaceDeviceWireless(devPath);
                if(devWirelessIfaceL->activeAccessPoint().path().length() > 2) {
                    QNetworkManagerInterfaceAccessPoint *accessPointIfaceL;
                    accessPointIfaceL = new QNetworkManagerInterfaceAccessPoint(devWirelessIfaceL->activeAccessPoint().path());
                    QString ssid =  accessPointIfaceL->ssid();

                    if(ssid.isEmpty()) {
                        ssid = QLatin1String("Hidden Network");
                    }
                    return ssid;
                }
            } else {
                QDBusObjectPath ip4Path = devIface->ip4config();

                QNetworkManagerIp4Config *ip4ConfigInterface;
                ip4ConfigInterface = new QNetworkManagerIp4Config(ip4Path.path(),this);

                QStringList domains = ip4ConfigInterface->domains();

                if(!domains.isEmpty()) {
                    return domains.at(0);
                }
            }
        }
    }
    return getNetworkNameForConnectionPath(devPath);
}

QString QSystemNetworkInfoPrivate::getNetworkNameForConnectionPath(const QString &path)
{
    QMapIterator<QString, QString> i(activePaths);
    QString devicepath;
    while (i.hasNext()) {
        i.next();
        QScopedPointer<QNetworkManagerConnectionActive> activeCon;
        activeCon.reset(new QNetworkManagerConnectionActive(i.key()));
        if(i.value() == path) {
            QScopedPointer<QNetworkManagerSettingsConnection> settingsConIface;
            settingsConIface.reset(new QNetworkManagerSettingsConnection(activeCon->serviceName(),activeCon->connection().path()));
            if(settingsConIface->isValid()) {
                qWarning() << settingsConIface->getId();
                return settingsConIface->getId();
            } else {
                //qWarning() << "not valid";
            }
        }
    }
    return QLatin1String("");
}

void QSystemNetworkInfoPrivate::updateActivePaths()
{
    activePaths.clear();
    QScopedPointer<QNetworkManagerInterface> dbIface;
    dbIface.reset(new QNetworkManagerInterface(this));

    QList <QDBusObjectPath> connections = dbIface->activeConnections();

    foreach(QDBusObjectPath activeconpath, connections) {

        QScopedPointer<QNetworkManagerConnectionActive> activeCon;
        activeCon.reset(new QNetworkManagerConnectionActive(activeconpath.path()));

        QList<QDBusObjectPath> devices = activeCon->devices();
        foreach(QDBusObjectPath device, devices) {
            activePaths.insert(activeconpath.path(),device.path());
        }
    }
}

void QSystemNetworkInfoPrivate::nmPropertiesChanged( const QString & path, QMap<QString,QVariant> map)
{
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();

        if( i.key() == QLatin1String("State")) {
            QNetworkManagerInterfaceDevice *devIface = new QNetworkManagerInterfaceDevice(path);
            quint32 nmState = i.value().toUInt();
            quint32 nmDevType = devIface->deviceType();
            QSystemNetworkInfo::NetworkMode mode = deviceTypeToMode(nmDevType);

            if(nmState == NM_DEVICE_STATE_DISCONNECTED
                || nmState == NM_DEVICE_STATE_UNAVAILABLE
                || nmState == NM_DEVICE_STATE_FAILED) {
                updateActivePaths();
                emit networkNameChanged(mode, QLatin1String(""));
                emit networkStatusChanged(mode, QSystemNetworkInfo::NoNetworkAvailable);
            }
            if(nmState == NM_DEVICE_STATE_PREPARE
               || nmState == NM_DEVICE_STATE_CONFIG
               || nmState == NM_DEVICE_STATE_NEED_AUTH) {
                emit networkNameChanged(mode, QLatin1String(""));
                emit networkStatusChanged(mode, QSystemNetworkInfo::Searching);
            }

            if(nmState == NM_DEVICE_STATE_ACTIVATED) {
                updateActivePaths();
                emit networkStatusChanged(mode, QSystemNetworkInfo::Connected);

                if(nmDevType == DEVICE_TYPE_802_11_WIRELESS){
                    QNetworkManagerInterfaceDeviceWireless *devWirelessIfaceL;
                    devWirelessIfaceL = new QNetworkManagerInterfaceDeviceWireless(path);
                    if(devWirelessIfaceL->activeAccessPoint().path().length() > 2) {
                        QNetworkManagerInterfaceAccessPoint *accessPointIfaceL;
                        accessPointIfaceL = new QNetworkManagerInterfaceAccessPoint(devWirelessIfaceL->activeAccessPoint().path());
                        QString ssid = accessPointIfaceL->ssid();
                        emit networkNameChanged(QSystemNetworkInfo::WlanMode, ssid);
                        emit networkSignalStrengthChanged(QSystemNetworkInfo::WlanMode, accessPointIfaceL->strength());
                    }
                }
            }
        }
        if( i.key() == QLatin1String("ActiveAccessPoint")) {
            accessPointIface = new QNetworkManagerInterfaceAccessPoint(path);

            accessPointIface->setConnections();
            if(!connect(accessPointIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                        this,SLOT(nmAPPropertiesChanged( const QString &, QMap<QString,QVariant>)))) {
             //   qWarning() << "connect is false";
            }

        }
        if( i.key() == QLatin1String("Carrier")) {
            int strength = 0;
            switch(i.value().toUInt()) {
            case 0:
                break;
            case 1:
                strength = 100;
                break;
            };
            emit networkSignalStrengthChanged(QSystemNetworkInfo::EthernetMode, strength);
        }
        if( i.key() == QLatin1String("Ip4Config")) {
            // || i.key() == "Ip46Config") {
            getPrimaryMode();
        }
    }
}

void QSystemNetworkInfoPrivate::nmAPPropertiesChanged( const QString & /*path*/, QMap<QString,QVariant> map)
{
   QMapIterator<QString, QVariant> i(map);
   while (i.hasNext()) {
       i.next();
       if( i.key() == QLatin1String("Strength")) {
           emit networkSignalStrengthChanged(QSystemNetworkInfo::WlanMode,  i.value().toUInt());
       }
   }
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::deviceTypeToMode(quint32 type)
{
    switch(type) {
    case DEVICE_TYPE_802_3_ETHERNET:
        return QSystemNetworkInfo::EthernetMode;
        break;
    case DEVICE_TYPE_802_11_WIRELESS:
        return QSystemNetworkInfo::WlanMode;
        break;
    case DEVICE_TYPE_UNKNOWN:
    case DEVICE_TYPE_GSM:
    case DEVICE_TYPE_CDMA:
        break;
    };
    return QSystemNetworkInfo::UnknownMode;
}

#endif

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            QString baseSysDir = QLatin1String("/sys/class/net/");
            QDir wDir(baseSysDir);
            QStringList dirs = wDir.entryList(QStringList() << QLatin1String("*"), QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo wiFi(devFile + QLatin1String("/wireless"));
                QFileInfo fi(QLatin1String("/proc/net/route"));
                if(wiFi.exists() && fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QString result = QLatin1String(rx.readAll());
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
            QString baseSysDir = QLatin1String("/sys/class/net/");
            QDir eDir(baseSysDir);
            QStringList dirs = eDir.entryList(QStringList() << QLatin1String("eth*"), QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(QLatin1String("/proc/net/route"));
                if(fi.exists()) {
                    QFile rx(fi.absoluteFilePath());
                    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        QString result = QLatin1String(rx.readAll());
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
#if !defined(QT_NO_NETWORKMANAGER)
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
    connectionInterface = new QDBusInterface(QLatin1String("org.bluez"),
                                             QLatin1String("/org/bluez/network"),
                                             QLatin1String("org.bluez.network.Manager"),
                                             dbusConnection);
    if (connectionInterface->isValid()) {

        QDBusReply<  QStringList > reply = connectionInterface->call(QLatin1String("ListConnections"));
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
            QString baseSysDir = QLatin1String("/sys/class/net/");
            QDir wDir(baseSysDir);
            QStringList dirs = wDir.entryList(QStringList() << QLatin1String("*"), QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + QLatin1String("/wireless/link"));
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
            QString baseSysDir = QLatin1String("/sys/class/net/");
            QDir eDir(baseSysDir);
            QStringList dirs = eDir.entryList(QStringList() << QLatin1String("eth*"), QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + QLatin1String("/carrier"));
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
    QString netname = QLatin1String("");

#if !defined(QT_NO_NETWORKMANAGER)
    netname =  getNmNetName(mode);
#else
    netname =  getSysNetName(mode);
#endif
    return netname;
}

QString QSystemNetworkInfoPrivate::getSysNetName(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            QString wlanInterface;
            QString netname;

            QString baseSysDir = QLatin1String("/sys/class/net/");
            QDir wDir(baseSysDir);
            QStringList dirs = wDir.entryList(QStringList() << QLatin1String("*"), QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + QLatin1String("/wireless"));
                if(fi.exists()) {
                    wlanInterface = dir;
                    //     qWarning() << "interface is" << wlanInterface;
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
                    netname = QLatin1String(ssid);
                }
            } else {
                qWarning() << "no socket";
            }
            close(sock);
            return netname;
        }
        break;
    case QSystemNetworkInfo::EthernetMode:
        {
            QString netname;
            QFile resFile(QLatin1String("/etc/resolv.conf"));
            if(resFile.exists()) {
                if(resFile.exists() && resFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QString line;
                    QTextStream in(&resFile);
                    do {
                        line = in.readLine();
                        if(line.contains(QLatin1String("domain"))) {
                            netname = line.section(QLatin1String(" "),1,1);
                        }
                    } while (!line.isNull());
                    resFile.close();
                }
            }
            return netname;
        }
        break;
    case QSystemNetworkInfo::BluetoothMode:
        {
#if !defined(QT_NO_DBUS)
            return getBluetoothInfo(QLatin1String("name"));
#endif
        }
        break;
    default:
        break;
    };
    return QLatin1String("");
}

QString QSystemNetworkInfoPrivate::macAddress(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
        case QSystemNetworkInfo::WlanMode:
        {
            QString result;
            QString baseSysDir = QLatin1String("/sys/class/net/");
            QDir wDir(baseSysDir);
            QStringList dirs = wDir.entryList(QStringList() << QLatin1String("*"), QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + QLatin1String("/wireless"));
                if(fi.exists()) {
                    QFile rx(devFile + QLatin1String("/address"));
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
            QString baseSysDir = QLatin1String("/sys/class/net/");
            QDir eDir(baseSysDir);
            QStringList dirs = eDir.entryList(QStringList() << QLatin1String("eth*"), QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(QString dir, dirs) {
                QString devFile = baseSysDir + dir;
                QFileInfo fi(devFile + QLatin1String("/address"));
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
            return getBluetoothInfo(QLatin1String("address"));
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
                QStringList list = iface.findDeviceByCapability(QLatin1String("net.80211"));
                if(!list.isEmpty()) {
                    foreach(QString netDev, list) {
                        QString deviceName ;
                        QHalDeviceInterface ifaceDevice(netDev);
                        deviceName  = ifaceDevice.getPropertyString(QLatin1String("net.interface"));
                        if(list.count() > 1) {
                            QString baseFIle = QLatin1String("/sys/class/net/") + deviceName+QLatin1String("/operstate");
                            QFile rx(baseFIle);
                            if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                                QString operatingState;
                                QTextStream in(&rx);
                                in >> operatingState;
                                rx.close();
                                if(!operatingState.contains(QLatin1String("unknown"))
                                    || !operatingState.contains(QLatin1String("down"))) {
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
                QStringList list = iface.findDeviceByCapability(QLatin1String("net.80203"));
                if(!list.isEmpty()) {
                    foreach(QString netDev, list) {
                        QString deviceName ;
                        QHalDeviceInterface ifaceDevice(netDev);
                        deviceName  = ifaceDevice.getPropertyString(QLatin1String("net.interface"));
                        if(list.count() > 1) {
                            QString baseFIle = QLatin1String("/sys/class/net/") + deviceName+QLatin1String("/operstate");
                            QFile rx(baseFIle);
                            if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
                                QString operatingState;
                                QTextStream in(&rx);
                                in >> operatingState;
                                rx.close();
                                if(!operatingState.contains(QLatin1String("unknown"))
                                    || !operatingState.contains(QLatin1String("down"))) {
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
    QString baseSysDir = QLatin1String("/sys/class/net/");
    QDir eDir(baseSysDir);
    QStringList dirs = eDir.entryList(QStringList() << QLatin1String("*"), QDir::AllDirs | QDir::NoDotAndDotDot);
    foreach(QString dir, dirs) {
        QString devFile = baseSysDir + dir;
        QFileInfo devfi(devFile + QLatin1String("/device"));
        if(!devfi.exists()) {
            continue;
        }
        QString baseFIle = QLatin1String("/sys/class/net/" )+ devFile+QLatin1String("/operstate");
        QFile rx(baseFIle);
        if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString operatingState;
            QTextStream in(&rx);
            in >> operatingState;
            rx.close();
            if(operatingState.contains(QLatin1String("unknown"))) {
                continue;
            }
        }
        switch(mode) {
        case QSystemNetworkInfo::WlanMode:
            {
                QFileInfo fi(devFile + QLatin1String("/wireless"));
                if(fi.exists()) {
                    return QNetworkInterface::interfaceFromName(dir);
                }
            }
            break;
            case QSystemNetworkInfo::EthernetMode:
                {
                QFileInfo fi(devFile + QLatin1String("/wireless"));
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

bool QSystemNetworkInfoPrivate::isDefaultInterface(const QString &deviceName)
{
    QFile routeFilex(QLatin1String("/proc/net/route"));
    if(routeFilex.exists() && routeFilex.open(QIODevice::ReadOnly
                                              | QIODevice::Text)) {
        QTextStream rin(&routeFilex);
        QString line = rin.readLine();
        while (!line.isNull()) {
            QString lineSection = line.section(QLatin1String("\t"),2,2,QString::SectionSkipEmpty);
            if(lineSection != QLatin1String("00000000")&& lineSection!=QLatin1String("Gateway"))
                if(line.section(QLatin1String("\t"),0,0,QString::SectionSkipEmpty) == deviceName) {
                routeFilex.close();
                return true;
            }
            line = rin.readLine();
        }
    }
    routeFilex.close();
    return false;
}

#if !defined(QT_NO_DBUS)
int QSystemNetworkInfoPrivate::getBluetoothRssi()
{
    return 0;
}

QString QSystemNetworkInfoPrivate::getBluetoothInfo(const QString &file)
{
    QString sysPath = QLatin1String("/sys/class/bluetooth/");
    QDir sysDir(sysPath);
    QStringList filters;
    filters << QLatin1String("*");
    QStringList sysList = sysDir.entryList( filters ,QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    foreach(QString dir, sysList) {
        QFile btFile(sysPath + dir+QLatin1String("/")+file);
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
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalInterface iface;
        if (iface.isValid()) {
            QStringList list = iface.findDeviceByCapability(QLatin1String("laptop_panel"));
            if(!list.isEmpty()) {
                foreach(QString lapDev, list) {
                    QHalDeviceInterface ifaceDevice(lapDev);
                    QHalDeviceLaptopPanelInterface lapIface(lapDev);
                    float numLevels = ifaceDevice.getPropertyInt(QLatin1String("laptop_panel.num_levels")) - 1;
                    float curLevel = lapIface.getBrightness();
                    return curLevel / numLevels * 100;
                }
            }
        }
#endif
    } else {
        QString backlightPath = QLatin1String("/proc/acpi/video/");
        QDir videoDir(backlightPath);
        QStringList filters;
        filters << QLatin1String("*");
        QStringList brightnessList = videoDir.entryList(filters,
                                                        QDir::Dirs
                                                        | QDir::NoDotAndDotDot,
                                                        QDir::Name);
        foreach(QString brightnessFileName, brightnessList) {
            float numLevels = 0.0;
            float curLevel = 0.0;
            QFile curBrightnessFile(backlightPath+brightnessFileName+QLatin1String("/LCD/brightness"));
            if(!curBrightnessFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qWarning()<<"File not opened";
            } else {
                QString  strvalue;
                strvalue = QLatin1String(curBrightnessFile.readAll().trimmed());
                if(strvalue.contains(QLatin1String("levels"))) {
                    QStringList list = strvalue.split(QLatin1String(" "));
                    numLevels = list.at(2).toFloat();
                }
                if(strvalue.contains(QLatin1String("current"))) {
                    QStringList list = strvalue.split(QLatin1String(": "));
                    curLevel = list.at(list.count()-1).toFloat();
                }
                curBrightnessFile.close();
                return curLevel / numLevels * 100;
            }
        }
    }
#if 0
    QString backlightPath = QLatin1String("/sys/devices/virtual/backlight/");
    QDir videoDir(backlightPath);
    QStringList filters;
    filters << QLatin1String("*");
    QStringList brightnessList = videoDir.entryList(filters,
                                                     QDir::Dirs
                                                     | QDir::NoDotAndDotDot,
                                                     QDir::Name);
    foreach(QString brightnessFileName, brightnessList) {
        float numLevels = 0.0;
        float curLevel = 0.0;
        QFile curBrightnessFile(backlightPath+brightnessFileName + QLatin1String("/brightness");
        if(!curBrightnessFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning()<<"File not opened";
        } else {
            QString strvalue;
            strvalue = curBrightnessFile.readLine().trimmed();
            curBrightnessFile.close();
            curLevel = strvalue.toFloat();

            QFile maxBrightnessFile(backlightPath+brightnessFileName+QLatin1String("/max_brightness");
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
#endif
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
    if(driveVolume.left(2) == QLatin1String("//")) {
        return 0;
    }
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
    if(driveVolume.left(2) == QLatin1String("//")) {
        return 0;
    }
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
        QStringList list = iface.findDeviceByCapability(QLatin1String("volume"));
        if(!list.isEmpty()) {
            foreach(QString vol, list) {
                QHalDeviceInterface ifaceDevice(vol);
                if(driveVolume == ifaceDevice.getPropertyString(QLatin1String("block.device"))) {
                    QHalDeviceInterface ifaceDeviceParent(ifaceDevice.getPropertyString(QLatin1String("info.parent")), this);

                    if(ifaceDeviceParent.getPropertyBool(QLatin1String("storage.removable"))
                        ||  ifaceDeviceParent.getPropertyString(QLatin1String("storage.drive_type")) != QLatin1String("disk")) {
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
        QString dmFile;

        if(driveVolume.contains(QLatin1String("mapper"))) {
            struct stat stat_buf;
            stat( driveVolume.toLatin1(), &stat_buf);

            dmFile = QString(QLatin1String("/sys/block/dm-%1/removable")).arg(stat_buf.st_rdev & 0377);

        } else {

            dmFile = driveVolume.section(QLatin1String("/"),2,3);
            if (dmFile.left(3) == QLatin1String("mmc")) {
                return QSystemStorageInfo::RemovableDrive;
            }

            if(dmFile.length() > 3) {
                dmFile.chop(1);
                if (dmFile.right(1) == QLatin1String("p"))
                    dmFile.chop(1);
            }
            dmFile = QLatin1String("/sys/block/")+dmFile+QLatin1String("/removable");
        }

        QFile file(dmFile);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open sys file";
        } else {
            QTextStream sysinfo(&file);
            QString line = sysinfo.readAll();
            if(line.contains(QLatin1String("1"))) {
                return QSystemStorageInfo::RemovableDrive;
            }
        }
    }
    if(driveVolume.left(2) == QLatin1String("//")) {
        return QSystemStorageInfo::RemoteDrive;
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
        if(ok && !mountEntriesMap.keys().contains(QLatin1String(me->mnt_dir))) {
            mountEntriesMap[QLatin1String(me->mnt_fsname)] = QLatin1String(me->mnt_dir);
        }

        me = getmntent(mntfp);
    }
    endmntent(mntfp);
}

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

        QStringList list = iface.findDeviceByCapability(QLatin1String("battery"));
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                halIfaceDevice = new QHalDeviceInterface(dev);
                if (halIfaceDevice->isValid()) {
                    QString batType = halIfaceDevice->getPropertyString(QLatin1String("battery.type"));
                    if(batType == QLatin1String("primary") || batType == QLatin1String("pda")) {
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

        list = iface.findDeviceByCapability(QLatin1String("ac_adapter"));
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

        list = iface.findDeviceByCapability(QLatin1String("battery"));
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
       if(map.at(i).toString() == QLatin1String("battery.charge_level.percentage")) {
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
        if((map.at(i).toString() == QLatin1String("ac_adapter.present"))
        || (map.at(i).toString() == QLatin1String("battery.rechargeable.is_charging"))) {
            QSystemDeviceInfo::PowerState state = currentPowerState();
            emit powerStateChanged(state);
       }
    }
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
            capList << QLatin1String("input.keyboard")
                    << QLatin1String("input.keys")
                    << QLatin1String("input.keypad")
                    << QLatin1String("input.mouse")
                    << QLatin1String("input.tablet");
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
                    };
                }
            }
            if(methods != 0)
                return methods;
        }
#endif
    }
    QString inputsPath = QLatin1String("/sys/class/input/");
    QDir inputDir(inputsPath);
    QStringList filters;
    filters << QLatin1String("event*");
    QStringList inputList = inputDir.entryList( filters ,QDir::Dirs, QDir::Name);
    foreach(QString inputFileName, inputList) {
        QFile file(inputsPath+inputFileName+QLatin1String("/device/name"));
        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning()<<"File not opened";
        } else {
            QString strvalue;
            strvalue = QLatin1String(file.readLine());
            file.close();
            if(strvalue.contains(QLatin1String("keyboard"))) {
                if( (methods & QSystemDeviceInfo::Keyboard) != QSystemDeviceInfo::Keyboard) {
                    methods = (methods | QSystemDeviceInfo::Keyboard);
                }
            } else if(strvalue.contains(QLatin1String("Mouse"))) {
                if( (methods & QSystemDeviceInfo::Mouse) != QSystemDeviceInfo::Mouse) {
                    methods = (methods | QSystemDeviceInfo::Mouse);
                }
            } else if(strvalue.contains(QLatin1String("Button"))) {
                if( (methods & QSystemDeviceInfo::Keys) != QSystemDeviceInfo::Keys) {
                    methods = (methods | QSystemDeviceInfo::Keys);
                }
            } else if(strvalue.contains(QLatin1String("TouchScreen"))) {
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
        return QLatin1String("Sim Not Available");
}

QString QSystemDeviceInfoPrivate::imsi()
{
        return QLatin1String("Sim Not Available");
}

QString QSystemDeviceInfoPrivate::manufacturer()
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalDeviceInterface iface(QLatin1String("/org/freedesktop/Hal/devices/computer"));
        QString manu;
        if (iface.isValid()) {
            manu = iface.getPropertyString(QLatin1String("system.firmware.vendor"));
            if(manu.isEmpty()) {
                manu = iface.getPropertyString(QLatin1String("system.hardware.vendor"));
                if(!manu.isEmpty()) {
                    return manu;
                }
            }
        }
#endif
    }
    QFile vendorId(QLatin1String("/sys/devices/virtual/dmi/id/board_vendor"));
    if (vendorId.open(QIODevice::ReadOnly)) {
        QTextStream cpuinfo(&vendorId);
        return cpuinfo.readLine().trimmed();
    } else {
        QFile file(QLatin1String("/proc/cpuinfo"));
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Could not open /proc/cpuinfo";
        } else {
            QTextStream cpuinfo(&file);
            QString line = cpuinfo.readLine();
            while (!line.isNull()) {
                line = cpuinfo.readLine();
                if(line.contains(QLatin1String("vendor_id"))) {
                    return line.split(QLatin1String(": ")).at(1).trimmed();
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
        QHalDeviceInterface iface(QLatin1String("/org/freedesktop/Hal/devices/computer"));
        QString model;
        if (iface.isValid()) {
            model = iface.getPropertyString(QLatin1String("system.kernel.machine"));
            if(!model.isEmpty())
                model += QLatin1String(" ");
            model += iface.getPropertyString(QLatin1String("system.chassis.type"));
            if(!model.isEmpty())
                return model;
        }
#endif
    }
    QFile file(QLatin1String("/proc/cpuinfo"));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open /proc/cpuinfo";
    } else {
        QTextStream cpuinfo(&file);
        QString line = cpuinfo.readLine();
        while (!line.isNull()) {
            line = cpuinfo.readLine();
            if(line.contains(QLatin1String("model name"))) {
                return line.split(QLatin1String(": ")).at(1).trimmed();
            }
        }
    }
    return QString();
}

QString QSystemDeviceInfoPrivate::productName()
{
    if(halIsAvailable) {
#if !defined(QT_NO_DBUS)
        QHalDeviceInterface iface(QLatin1String("/org/freedesktop/Hal/devices/computer"));
        QString productName;
        if (iface.isValid()) {
            productName = iface.getPropertyString(QLatin1String("info.product"));
            if(productName.isEmpty()) {
                productName = iface.getPropertyString(QLatin1String("system.product"));
                if(!productName.isEmpty())
                    return productName;
            } else {
                return productName;
            }
        }
#endif
    }
    QDir dir(QLatin1String("/etc"));
    if(dir.exists()) {
        QStringList langList;
        QFileInfoList localeList = dir.entryInfoList(QStringList() << QLatin1String("*release"),
                                                     QDir::Files | QDir::NoDotAndDotDot,
                                                     QDir::Name);
        foreach(QFileInfo fileInfo, localeList) {
            QString filepath = fileInfo.filePath();
            QFile file(filepath);
            if (file.open(QIODevice::ReadOnly)) {
                QTextStream prodinfo(&file);
                QString line = prodinfo.readLine();
                while (!line.isNull()) {
                    if(filepath.contains(QLatin1String("lsb.release"))) {
                        if(line.contains(QLatin1String("DISTRIB_DESCRIPTION"))) {
                            return line.split(QLatin1String("=")).at(1).trimmed();
                        }
                    } else {
                        return line;
                    }
                    line = prodinfo.readLine();
                }
            }
        }
    }

    QFile file(QLatin1String("/etc/issue"));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open /proc/cpuinfo";
    } else {
        QTextStream prodinfo(&file);
        QString line = prodinfo.readLine();
        while (!line.isNull()) {
            line = prodinfo.readLine();
            if(!line.isEmpty()) {
                QStringList lineList = line.split(QLatin1String(" "));
                for(int i = 0; i < lineList.count(); i++) {
                    if(lineList.at(i).toFloat()) {
                        return lineList.at(i-1) + QLatin1String(" ")+ lineList.at(i);
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
        QStringList list = iface.findDeviceByCapability(QLatin1String("battery"));
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (ifaceDevice.isValid()) {
                    if(!ifaceDevice.getPropertyBool(QLatin1String("battery.present"))
                        && (ifaceDevice.getPropertyString(QLatin1String("battery.type")) != QLatin1String("pda")
                             || ifaceDevice.getPropertyString(QLatin1String("battery.type")) != QLatin1String("primary"))) {
                        qWarning() << "XXXXXXXXXXXXX";
                        return 0;
                    } else {
                        level = ifaceDevice.getPropertyInt(QLatin1String("battery.charge_level.percentage"));
                        return level;
                    }
                }
            }
        }
#endif
    } else {
        QFile infofile(QLatin1String("/proc/acpi/battery/BAT0/info"));
        if (!infofile.open(QIODevice::ReadOnly)) {
            return QSystemDeviceInfo::NoBatteryLevel;
        } else {
            QTextStream batinfo(&infofile);
            QString line = batinfo.readLine();
            while (!line.isNull()) {
                if(line.contains(QLatin1String("design capacity"))) {
                    levelWhenFull = line.split(QLatin1String(" ")).at(1).trimmed().toFloat();
                    qWarning() << levelWhenFull;
                    infofile.close();
                    break;
                }
                line = batinfo.readLine();
            }
            infofile.close();
        }

        QFile statefile(QLatin1String("/proc/acpi/battery/BAT0/state"));
        if (!statefile.open(QIODevice::ReadOnly)) {
            return QSystemDeviceInfo::NoBatteryLevel;
        } else {
            QTextStream batstate(&statefile);
            QString line = batstate.readLine();
            while (!line.isNull()) {
                if(line.contains(QLatin1String("remaining capacity"))) {
                    level = line.split(QLatin1String(" ")).at(1).trimmed().toFloat();
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
        QStringList list = iface.findDeviceByCapability(QLatin1String("battery"));
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (iface.isValid()) {
                    if (ifaceDevice.getPropertyBool(QLatin1String("battery.rechargeable.is_charging"))) {
                        return QSystemDeviceInfo::WallPowerChargingBattery;
                    }
                }
            }
        }

        list = iface.findDeviceByCapability(QLatin1String("ac_adapter"));
        if(!list.isEmpty()) {
            foreach(QString dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (ifaceDevice.isValid()) {
                    if(ifaceDevice.getPropertyBool(QLatin1String("ac_adapter.present"))) {
                        return QSystemDeviceInfo::WallPower;
                    } else {
                        return QSystemDeviceInfo::BatteryPower;
                    }
                }
            }
        }

#else
        QFile statefile(QLatin1String("/proc/acpi/battery/BAT0/state"));
        if (!statefile.open(QIODevice::ReadOnly)) {
        } else {
            QTextStream batstate(&statefile);
            QString line = batstate.readLine();
            while (!line.isNull()) {
                if(line.contains(QLatin1String("charging state"))) {
                    if(line.split(QLatin1String(" ")).at(1).trimmed() == QLatin1String("discharging") {
                        return QSystemDeviceInfo::BatteryPower;
                    }
                    if(line.split(QLatin1String(" ")).at(1).trimmed() == QLatin1String("charging")) {
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
     connectionInterface = new QDBusInterface(QLatin1String("org.bluez"),
                                              QLatin1String("/"),
                                              QLatin1String("org.bluez.Manager"),
                                              dbusConnection);
     if (connectionInterface->isValid()) {

         QDBusReply<  QDBusObjectPath > reply = connectionInterface->call(QLatin1String("DefaultAdapter"));
         if (reply.isValid()) {
             QDBusInterface *adapterInterface;
             adapterInterface = new QDBusInterface(QLatin1String("org.bluez"),
                                                   reply.value().path(),
                                                   QLatin1String("org.bluez.Adapter"),
                                                   dbusConnection);
             if (adapterInterface->isValid()) {
                 if (!dbusConnection.connect(QLatin1String("org.bluez"),
                                           reply.value().path(),
                                            QLatin1String("org.bluez.Adapter"),
                                            QLatin1String("PropertyChanged"),
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

 QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QSystemScreenSaverLinuxCommonPrivate *parent)
         : QSystemScreenSaverLinuxCommonPrivate(parent)
 {
     kdeIsRunning = false;
     gnomeIsRunning = false;
     whichWMRunning();
 }

 QSystemScreenSaverPrivate::~QSystemScreenSaverPrivate()
 {
 }

 bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
 {
     if(kdeIsRunning || gnomeIsRunning) {
#if !defined(QT_NO_DBUS)
         pid_t pid = getppid();
         QDBusConnection dbusConnection = QDBusConnection::sessionBus();

         QStringList ifaceList;
         ifaceList <<  QLatin1String("org.freedesktop.ScreenSaver");
         ifaceList << QLatin1String("org.gnome.ScreenSaver");
         QDBusInterface *connectionInterface;
         foreach(QString iface, ifaceList) {
             connectionInterface = new QDBusInterface(QLatin1String(iface.toLatin1()),
                                                      QLatin1String("/ScreenSaver"),
                                                      QLatin1String(iface.toLatin1()),
                                                      dbusConnection);
             QDBusReply<uint> reply =  connectionInterface->call(QLatin1String("Inhibit"),
                                                                 QString::number((int)pid),
                                                                 QLatin1String("QSystemScreenSaver"));
             if(reply.isValid()) {
                 currentPid = reply.value();
                 qWarning() << "Inhibit" << currentPid;
                 return reply.isValid();
             } else {
                 qWarning() << reply.error();
             }
         }
#endif
     } else {
#ifdef Q_WS_X11
         int timeout;
         int interval;
         int preferBlank;
         int allowExp;
         XGetScreenSaver(QX11Info::display(), &timeout, &interval, &preferBlank, &allowExp);
             timeout = -1;
         XSetScreenSaver(QX11Info::display(), timeout, interval, preferBlank, allowExp);
#endif
     }
    return false;
}


bool QSystemScreenSaverPrivate::screenSaverInhibited()
{
    if(kdeIsRunning) {
        QString kdeSSConfig;
        if(QDir( QDir::homePath()+QLatin1String("/.kde4/")).exists()) {
            kdeSSConfig = QDir::homePath()+QLatin1String("/.kde4/share/config/kscreensaverrc");
        } else if(QDir(QDir::homePath()+QLatin1String("/.kde/")).exists()) {
            kdeSSConfig = QDir::homePath()+QLatin1String("/.kde/share/config/kscreensaverrc");
        }
        QSettings kdeScreenSaveConfig(kdeSSConfig, QSettings::IniFormat);
        kdeScreenSaveConfig.beginGroup(QLatin1String("ScreenSaver"));
        if(kdeScreenSaveConfig.status() == QSettings::NoError) {
            if(kdeScreenSaveConfig.value(QLatin1String("Enabled")).toBool() == false) {
            } else {
                return true;
            }
        }
    } else if(gnomeIsRunning) {

    }

#ifdef Q_WS_X11
    int timeout;
    int interval;
    int preferBlank;
    int allowExp;
    XGetScreenSaver(QX11Info::display(), &timeout, &interval, &preferBlank, &allowExp);
    if(timeout != 0) {
        return true;
    }

#endif

    return false;
}

void QSystemScreenSaverPrivate::whichWMRunning()
{
#if !defined(QT_NO_DBUS)
    QDBusConnection dbusConnection = QDBusConnection::sessionBus();
    QDBusInterface *connectionInterface;
    connectionInterface = new QDBusInterface(QLatin1String("org.kde.kwin"),
                                             QLatin1String("/KWin"),
                                             QLatin1String("org.kde.KWin"),
                                             dbusConnection);
    if(connectionInterface->isValid()) {
        kdeIsRunning = true;
        return;
    }
    connectionInterface = new QDBusInterface(QLatin1String("org.gnome.SessionManager"),
                                             QLatin1String("/org/gnome/SessionManager"),
                                             QLatin1String("org.gnome.SessionManager"),
                                             dbusConnection);
    if(connectionInterface->isValid()) {
       gnomeIsRunning = true;
       return;
    }
#endif
}

bool QSystemScreenSaverPrivate::isScreenLockEnabled()
{
    if(kdeIsRunning) {
        QString kdeSSConfig;
        if(QDir( QDir::homePath()+QLatin1String("/.kde4/")).exists()) {
            kdeSSConfig = QDir::homePath()+QLatin1String("/.kde4/share/config/kscreensaverrc");
        } else if(QDir(QDir::homePath()+QLatin1String("/.kde/")).exists()) {
            kdeSSConfig = QDir::homePath()+QLatin1String("/.kde/share/config/kscreensaverrc");
        }
        QSettings kdeScreenSaveConfig(kdeSSConfig, QSettings::IniFormat);
        kdeScreenSaveConfig.beginGroup(QLatin1String("ScreenSaver"));
        if(kdeScreenSaveConfig.status() == QSettings::NoError) {
            return kdeScreenSaveConfig.value(QLatin1String("Lock")).toBool();
        }
    } else if(gnomeIsRunning) {

    }

   return false;
}

bool QSystemScreenSaverPrivate::isScreenSaverActive()
{
    if(kdeIsRunning || gnomeIsRunning) {
#if !defined(QT_NO_DBUS)
        pid_t pid = getppid();
        QDBusConnection dbusConnection = QDBusConnection::sessionBus();

        QStringList ifaceList;
        ifaceList <<  QLatin1String("org.freedesktop.ScreenSaver");
        ifaceList << QLatin1String("org.gnome.ScreenSaver");
        QDBusInterface *connectionInterface;
        foreach(QString iface, ifaceList) {
            connectionInterface = new QDBusInterface(QLatin1String(iface.toLatin1()),
                                                     QLatin1String("/ScreenSaver"),
                                                     QLatin1String(iface.toLatin1()),
                                                     dbusConnection);

            QDBusReply<bool> reply =  connectionInterface->call(QLatin1String("GetActive"),
                                                                QString::number((int)pid),
                                                                QLatin1String("QSystemScreenSaver"));
            if(reply.isValid()) {
                return reply.value();
            }
        }
#endif
    }
    return false;
}

#include "moc_qsysteminfo_linux_p.cpp"

QTM_END_NAMESPACE
