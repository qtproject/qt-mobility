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
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QTimer>
#include <QMapIterator>

#ifndef QT_NO_NETWORKMANAGER
#include <qnetworkmanagerservice_linux_p.h>
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

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>

#endif

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
    QDir transDir(QLibraryInfo::location (QLibraryInfo::TranslationsPath));
    QStringList langList;

    if(transDir.exists()) {
        QStringList localeList = transDir.entryList( QStringList() << QLatin1String("qt_*.qm") ,QDir::Files
                                                     | QDir::NoDotAndDotDot, QDir::Name);
        foreach(const QString localeName, localeList) {
            const QString lang = localeName.mid(3,2);
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

QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QSystemNetworkInfoLinuxCommonPrivate *parent)
        : QSystemNetworkInfoLinuxCommonPrivate(parent)
{
#if !defined(QT_NO_NETWORKMANAGER)
    setupNmConnections();
    updateActivePaths();
#endif
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
}

#if !defined(QT_NO_NETWORKMANAGER)
void QSystemNetworkInfoPrivate::setupNmConnections()
{
    iface = new QNetworkManagerInterface(this);

   foreach(const QDBusObjectPath path, iface->getDevices()) {
        QNetworkManagerInterfaceDevice *devIface = new QNetworkManagerInterfaceDevice(path.path(), this);

        switch(devIface->deviceType()) {
        case DEVICE_TYPE_802_3_ETHERNET:
            {
                devWiredIface = new QNetworkManagerInterfaceDeviceWired(devIface->connectionInterface()->path(), this);
                devWiredIface->setConnections();
                connect(devWiredIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                        this,SLOT(nmPropertiesChanged( const QString &, QMap<QString,QVariant>)));
            }
            break;
        case DEVICE_TYPE_802_11_WIRELESS:
            {
                devWirelessIface = new QNetworkManagerInterfaceDeviceWireless(devIface->connectionInterface()->path(), this);
                devWirelessIface->setConnections();

                connect(devWirelessIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                        this,SLOT(nmPropertiesChanged( const QString &, QMap<QString,QVariant>)));

                if(devWirelessIface->activeAccessPoint().path().length() > 2) {
                    accessPointIface = new QNetworkManagerInterfaceAccessPoint(devWirelessIface->activeAccessPoint().path(), this);
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
        activeCon.reset(new QNetworkManagerConnectionActive(i.key(), this));
        if(i.value() == path) {
            isDefault = activeCon->defaultRoute();
        }
    }
    return isDefault;
}

void QSystemNetworkInfoPrivate::primaryModeChanged()
{
    emit networkModeChanged(currentMode());
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
                devWirelessIfaceL = new QNetworkManagerInterfaceDeviceWireless(devPath, this);
                if(devWirelessIfaceL->activeAccessPoint().path().length() > 2) {
                    QNetworkManagerInterfaceAccessPoint *accessPointIfaceL;
                    accessPointIfaceL = new QNetworkManagerInterfaceAccessPoint(devWirelessIfaceL->activeAccessPoint().path(), this);
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
            settingsConIface.reset(new QNetworkManagerSettingsConnection(activeCon->serviceName(),activeCon->connection().path(), this));
            if(settingsConIface->isValid()) {
                return settingsConIface->getId();
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

    const QList <QDBusObjectPath> connections = dbIface->activeConnections();

    foreach(const QDBusObjectPath activeconpath, connections) {
        QScopedPointer<QNetworkManagerConnectionActive> activeCon;
        activeCon.reset(new QNetworkManagerConnectionActive(activeconpath.path(), this));

        const QList<QDBusObjectPath> devices = activeCon->devices();
        foreach(const QDBusObjectPath device, devices) {
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
            QNetworkManagerInterfaceDevice *devIface = new QNetworkManagerInterfaceDevice(path, this);
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
                    devWirelessIfaceL = new QNetworkManagerInterfaceDeviceWireless(path, this);
                    if(devWirelessIfaceL->activeAccessPoint().path().length() > 2) {
                        QNetworkManagerInterfaceAccessPoint *accessPointIfaceL;
                        accessPointIfaceL = new QNetworkManagerInterfaceAccessPoint(devWirelessIfaceL->activeAccessPoint().path(), this);
                        QString ssid = accessPointIfaceL->ssid();
                        emit networkNameChanged(QSystemNetworkInfo::WlanMode, ssid);
                        emit networkSignalStrengthChanged(QSystemNetworkInfo::WlanMode, accessPointIfaceL->strength());
                    }
                }
            }
        }
        if( i.key() == QLatin1String("ActiveAccessPoint")) {
            accessPointIface = new QNetworkManagerInterfaceAccessPoint(path, this);

            accessPointIface->setConnections();
            if(!connect(accessPointIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                        this,SLOT(nmAPPropertiesChanged( const QString &, QMap<QString,QVariant>)))) {
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
            primaryModeChanged();
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

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    switch(mode) {
    case QSystemNetworkInfo::WlanMode:
        {
            QString result;
            const QString baseSysDir = QLatin1String("/sys/class/net/");
            const QDir wDir(baseSysDir);
            const QStringList dirs = wDir.entryList(QStringList() << QLatin1String("*"), QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(const QString dir, dirs) {
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
            const QString baseSysDir = QLatin1String("/sys/class/net/");
            const QDir eDir(baseSysDir);
            const QStringList dirs = eDir.entryList(QStringList() << QLatin1String("eth*"), QDir::AllDirs | QDir::NoDotAndDotDot);
            foreach(const QString dir, dirs) {
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

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::currentMode()
{
    QSystemNetworkInfo::NetworkMode mode;

    bool anyDefaultRoute = false;

    QMapIterator<QString, QString> i(activePaths);
    QString devicepath;
    while (i.hasNext()) {
        i.next();
        QScopedPointer<QNetworkManagerConnectionActive> activeCon;
        activeCon.reset(new QNetworkManagerConnectionActive(i.key(), this));

        if(activeCon->defaultRoute()) {
            anyDefaultRoute = activeCon->defaultRoute();
            QNetworkManagerInterfaceDevice *devIface = new QNetworkManagerInterfaceDevice(i.value(), this);
            return deviceTypeToMode(devIface->deviceType());
        }
        devicepath = i.value();
    }

    if(!anyDefaultRoute) {
       mode = QSystemNetworkInfo::UnknownMode;
    }

    return QSystemNetworkInfo::UnknownMode;
}

QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QSystemDisplayInfoLinuxCommonPrivate *parent)
        : QSystemDisplayInfoLinuxCommonPrivate(parent)
{
}

QSystemDisplayInfoPrivate::~QSystemDisplayInfoPrivate()
{
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
        return QLatin1String("Sim Not Available");
}

QString QSystemDeviceInfoPrivate::imsi()
{
        return QLatin1String("Sim Not Available");
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
         const pid_t pid = getppid();
         QDBusConnection dbusConnection = QDBusConnection::sessionBus();

         QStringList ifaceList;
         ifaceList <<  QLatin1String("org.freedesktop.ScreenSaver");
         ifaceList << QLatin1String("org.gnome.ScreenSaver");
         QDBusInterface *connectionInterface;
         foreach(const QString iface, ifaceList) {
             connectionInterface = new QDBusInterface(QLatin1String(iface.toLatin1()),
                                                      QLatin1String("/ScreenSaver"),
                                                      QLatin1String(iface.toLatin1()),
                                                      dbusConnection);
             if(connectionInterface->isValid()) {
                 QDBusReply<uint> reply =  connectionInterface->call(QLatin1String("Inhibit"),
                                                                     QString::number((int)pid),
                                                                     QLatin1String("QSystemScreenSaver"));
                 if(reply.isValid()) {
                     currentPid = reply.value();
                     return reply.isValid();
                 } else {
                     qWarning() << reply.error();
                 }
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
        const pid_t pid = getppid();
        QDBusConnection dbusConnection = QDBusConnection::sessionBus();

        QStringList ifaceList;
        ifaceList <<  QLatin1String("org.freedesktop.ScreenSaver");
        ifaceList << QLatin1String("org.gnome.ScreenSaver");
        QDBusInterface *connectionInterface;
        foreach(const QString iface, ifaceList) {
            connectionInterface = new QDBusInterface(QLatin1String(iface.toLatin1()),
                                                     QLatin1String("/ScreenSaver"),
                                                     QLatin1String(iface.toLatin1()),
                                                     dbusConnection);

            const QDBusReply<bool> reply =  connectionInterface->call(QLatin1String("GetActive"),
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
