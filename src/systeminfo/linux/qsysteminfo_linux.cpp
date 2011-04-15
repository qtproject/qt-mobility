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
#include "qsysteminfocommon_p.h"
#include "linux/qsysteminfo_linux_p.h"

#include <unistd.h> // for getppid

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
#include <QSettings>

#ifndef QT_NO_NETWORKMANAGER
#include "linux/qnetworkmanagerservice_linux_p.h"
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

//#ifdef Q_WS_X11
//#include <QX11Info>
//#include <X11/Xlib.h>
#if !defined(Q_WS_MAEMO_6)/* && defined(QT_NO_MEEGO)*/
#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#endif
//#endif

#endif
#include "qsysteminfo_dbus_p.h"

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

    if (transDir.exists()) {
        QStringList localeList = transDir.entryList(QStringList() << QLatin1String("qt_*.qm"),
                                                    QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
        foreach (const QString &localeName, localeList) {
            const QString lang = localeName.mid(3, 2);
            if (!langList.contains(lang) && !lang.isEmpty() && !lang.contains(QLatin1String("help")))
                langList <<lang;
        }

        if (langList.count() > 0)
            return langList;
    }

    return QStringList() << currentLanguage();
}

QSystemNetworkInfoPrivate::QSystemNetworkInfoPrivate(QSystemNetworkInfoLinuxCommonPrivate *parent)
    : QSystemNetworkInfoLinuxCommonPrivate(parent)
{
#if !defined(QT_NO_NETWORKMANAGER)
    setupNmConnections();
    updateActivePaths();
#endif // QT_NO_NETWORKMANAGER
}

QSystemNetworkInfoPrivate::~QSystemNetworkInfoPrivate()
{
}

#if !defined(QT_NO_NETWORKMANAGER)
void QSystemNetworkInfoPrivate::setupNmConnections()
{
    iface = new QNetworkManagerInterface(this);

    foreach (const QDBusObjectPath &path, iface->getDevices()) {
        QNetworkManagerInterfaceDevice *devIface = new QNetworkManagerInterfaceDevice(path.path(), this);

        switch(devIface->deviceType()) {
        case DEVICE_TYPE_802_3_ETHERNET:
            devWiredIface = new QNetworkManagerInterfaceDeviceWired(devIface->connectionInterface()->path(), this);
            devWiredIface->setConnections();
            connect(devWiredIface, SIGNAL(propertiesChanged(QString,QMap<QString,QVariant>)),
                    this, SLOT(nmPropertiesChanged(QString,QMap<QString,QVariant>)));
            break;

        case DEVICE_TYPE_802_11_WIRELESS:
            devWirelessIface = new QNetworkManagerInterfaceDeviceWireless(devIface->connectionInterface()->path(), this);
            devWirelessIface->setConnections();
            connect(devWirelessIface, SIGNAL(propertiesChanged(QString,QMap<QString,QVariant>)),
                    this, SLOT(nmPropertiesChanged(QString,QMap<QString,QVariant>)));
            if (devWirelessIface->activeAccessPoint().path().length() > 2) {
                accessPointIface = new QNetworkManagerInterfaceAccessPoint(devWirelessIface->activeAccessPoint().path(), this);
                accessPointIface->setConnections();
                connect(accessPointIface, SIGNAL(propertiesChanged(QMap<QString,QVariant>)),
                        this, SLOT(nmAPPropertiesChanged(QString,QMap<QString,QVariant>)));
            }
            break;

        case DEVICE_TYPE_GSM:
            devGsmIface = new QNetworkManagerInterfaceDeviceGsm(devIface->connectionInterface()->path(), this);
            devGsmIface->setConnections();
            connect(devGsmIface, SIGNAL(propertiesChanged(QString,QMap<QString,QVariant>)),
                    this, SLOT(nmPropertiesChanged(QString,QMap<QString,QVariant>)));
            break;

        case DEVICE_TYPE_CDMA:
            qDebug() << Q_FUNC_INFO << devIface->deviceType() << "CDMA";
            break;

        default:
            break;
        };
    }
}

void QSystemNetworkInfoPrivate::updateActivePaths()
{
    activePaths.clear();
    QScopedPointer<QNetworkManagerInterface> dbIface;
    dbIface.reset(new QNetworkManagerInterface(this));

    const QList<QDBusObjectPath> connections = dbIface->activeConnections();

    foreach (const QDBusObjectPath &activeconpath, connections) {
        QScopedPointer<QNetworkManagerConnectionActive> activeCon;
        activeCon.reset(new QNetworkManagerConnectionActive(activeconpath.path(), this));
        const QList<QDBusObjectPath> devices = activeCon->devices();
        foreach (const QDBusObjectPath &device, devices)
            activePaths.insert(activeconpath.path(),device.path());
    }
}

void QSystemNetworkInfoPrivate::nmPropertiesChanged(const QString &path, QMap<QString, QVariant> map)
{
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();

        if (i.key() == QLatin1String("State")) {
            QNetworkManagerInterfaceDevice *devIface = new QNetworkManagerInterfaceDevice(path, this);
            quint32 nmState = i.value().toUInt();
            quint32 nmDevType = devIface->deviceType();
            QSystemNetworkInfo::NetworkMode mode = deviceTypeToMode(nmDevType);

            if (nmState == NM_DEVICE_STATE_DISCONNECTED
                || nmState == NM_DEVICE_STATE_UNAVAILABLE
                || nmState == NM_DEVICE_STATE_FAILED) {
                updateActivePaths();
                Q_EMIT networkNameChanged(mode, QString());
                Q_EMIT networkStatusChanged(mode, QSystemNetworkInfo::NoNetworkAvailable);
            } else if (nmState == NM_DEVICE_STATE_PREPARE
                       || nmState == NM_DEVICE_STATE_CONFIG
                       || nmState == NM_DEVICE_STATE_NEED_AUTH) {
                Q_EMIT networkNameChanged(mode, QString());
                Q_EMIT networkStatusChanged(mode, QSystemNetworkInfo::Searching);
            } else if (nmState == NM_DEVICE_STATE_ACTIVATED) {
                updateActivePaths();
                Q_EMIT networkStatusChanged(mode, QSystemNetworkInfo::Connected);

                if (nmDevType == DEVICE_TYPE_802_11_WIRELESS) {
                    QNetworkManagerInterfaceDeviceWireless *devWirelessIfaceL = new QNetworkManagerInterfaceDeviceWireless(path, this);
                    if (devWirelessIfaceL->activeAccessPoint().path().length() > 2) {
                        QNetworkManagerInterfaceAccessPoint *accessPointIfaceL = new QNetworkManagerInterfaceAccessPoint(devWirelessIfaceL->activeAccessPoint().path(), this);
                        Q_EMIT networkNameChanged(QSystemNetworkInfo::WlanMode, accessPointIfaceL->ssid());
                        Q_EMIT networkSignalStrengthChanged(QSystemNetworkInfo::WlanMode, accessPointIfaceL->strength());
                    }
                } else if (nmDevType == DEVICE_TYPE_GSM) {
                    qDebug() << Q_FUNC_INFO << "gsm changeed";
                }
            }
        } else if (i.key() == QLatin1String("ActiveAccessPoint")) {
            accessPointIface = new QNetworkManagerInterfaceAccessPoint(path, this);
            accessPointIface->setConnections();
            connect(accessPointIface, SIGNAL(propertiesChanged(QString,QMap<QString,QVariant>)),
                    this, SLOT(nmPropertiesChanged(QString,QMap<QString,QVariant>)));

        } else if (i.key() == QLatin1String("Carrier")) {
            int strength = 0;
            if (i.value().toUInt() == 100)
                strength = 100;
            Q_EMIT networkSignalStrengthChanged(QSystemNetworkInfo::EthernetMode, strength);
        } else if (i.key() == QLatin1String("Ip4Config")) {
            Q_EMIT networkModeChanged(currentMode());
        }
    }
}

void QSystemNetworkInfoPrivate::nmAPPropertiesChanged( const QString &/*path*/, QMap<QString, QVariant> map)
{
   QMapIterator<QString, QVariant> i(map);
   while (i.hasNext()) {
       i.next();

       if (i.key() == QLatin1String("Strength"))
           Q_EMIT networkSignalStrengthChanged(QSystemNetworkInfo::WlanMode, i.value().toUInt());
   }
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::deviceTypeToMode(quint32 type)
{
    switch(type) {
    case DEVICE_TYPE_802_3_ETHERNET:
        return QSystemNetworkInfo::EthernetMode;
    case DEVICE_TYPE_802_11_WIRELESS:
        return QSystemNetworkInfo::WlanMode;
    case DEVICE_TYPE_GSM:
        return QSystemNetworkInfo::GsmMode;
    case DEVICE_TYPE_CDMA:
        return QSystemNetworkInfo::CdmaMode;
    case DEVICE_TYPE_UNKNOWN:
        break;
    };

    return QSystemNetworkInfo::UnknownMode;
}
#endif // QT_NO_NETWORKMANAGER

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::currentMode()
{
#if !defined(QT_NO_NETWORKMANAGER)
    QSystemNetworkInfo::NetworkMode mode = QSystemNetworkInfo::UnknownMode;
    QMapIterator<QString, QString> i(activePaths);
    QString devicepath;
    while (i.hasNext()) {
        i.next();
        QScopedPointer<QNetworkManagerConnectionActive> activeCon;
        activeCon.reset(new QNetworkManagerConnectionActive(i.key(), this));

        if (activeCon->defaultRoute()) {
            QNetworkManagerInterfaceDevice *devIface = new QNetworkManagerInterfaceDevice(i.value(), this);
            return deviceTypeToMode(devIface->deviceType());
        }
        devicepath = i.value();
    }
    return mode;
#else // QT_NO_NETWORKMANAGER
    return QSystemNetworkInfoLinuxCommonPrivate::currentMode();
#endif // QT_NO_NETWORKMANAGER
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfoPrivate::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
#if !defined(QT_NO_NETWORKMANAGER)
    switch (mode) {
    case QSystemNetworkInfo::GsmMode:
        foreach (const QDBusObjectPath &path, iface->getDevices()) {
            QNetworkManagerInterfaceDevice devIface(path.path(), this);
            if (devIface.deviceType() == DEVICE_TYPE_GSM) {
                QModemManagerGsmNetworkInterface interface(devIface.udi());
                if (interface.isValid()) {
                    MMRegistrationInfoType info = interface.registrationStatus();
                    if (interface.signalQuality() != 0) {
                        switch (info.status) {
                        case MMRegistrationInfoType::Idle:
                            return QSystemNetworkInfo::NoNetworkAvailable;
                        case MMRegistrationInfoType::Home:
                            return QSystemNetworkInfo::HomeNetwork;
                        case MMRegistrationInfoType::Searching:
                            return QSystemNetworkInfo::Searching;
                        case MMRegistrationInfoType::Denied:
                            return QSystemNetworkInfo::Denied;
                        case MMRegistrationInfoType::Unknown:
                            return QSystemNetworkInfo::UndefinedStatus;
                        case MMRegistrationInfoType::Roaming:
                            return QSystemNetworkInfo::Roaming;
                        };
                    } else {
                        return QSystemNetworkInfo::UndefinedStatus;
                    }
                }
            }
        }
        break;

    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::WcdmaMode:
        foreach (const QDBusObjectPath &path, iface->getDevices()) {
            QNetworkManagerInterfaceDevice devIface(path.path(), this);
            if (devIface.deviceType() == DEVICE_TYPE_CDMA) {
                QModemManagerGsmNetworkInterface interface(devIface.udi());
                if (interface.isValid()) {
                    MMRegistrationInfoType info = interface.registrationStatus();
                    if (interface.signalQuality() != 0) {
                        switch (info.status) {
                        case MMRegistrationInfoType::Idle:
                            return QSystemNetworkInfo::NoNetworkAvailable;
                        case MMRegistrationInfoType::Home:
                            return QSystemNetworkInfo::HomeNetwork;
                        case MMRegistrationInfoType::Searching:
                            return QSystemNetworkInfo::Searching;
                        case MMRegistrationInfoType::Denied:
                            return QSystemNetworkInfo::Denied;
                        case MMRegistrationInfoType::Unknown:
                            return QSystemNetworkInfo::UndefinedStatus;
                        case MMRegistrationInfoType::Roaming:
                            return QSystemNetworkInfo::Roaming;
                        };
                    } else {
                        return QSystemNetworkInfo::UndefinedStatus;
                    }
                }
            }
        }
        break;

//    case QSystemNetworkInfo::WlanMode:
//    case QSystemNetworkInfo::EthernetMode:
//    case QSystemNetworkInfo::BluetoothMode:
//    case QSystemNetworkInfo::WimaxMode:
//    case QSystemNetworkInfo::LteMode:
    default:
        break;
    }
#endif // QT_NO_NETWORKMANAGER

    return QSystemNetworkInfoLinuxCommonPrivate::networkStatus(mode);
}

int QSystemNetworkInfoPrivate::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
#if !defined(QT_NO_NETWORKMANAGER)
    switch (mode) {
    case QSystemNetworkInfo::GsmMode:
        foreach (const QDBusObjectPath &path, iface->getDevices()) {
            QNetworkManagerInterfaceDevice devIface(path.path(), this);
            if (devIface.deviceType() == DEVICE_TYPE_GSM) {
                QModemManagerGsmNetworkInterface interface(devIface.udi());
                return interface.signalQuality();
            }
        }
        break;

    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::WcdmaMode:
        foreach (const QDBusObjectPath &path, iface->getDevices()) {
            QNetworkManagerInterfaceDevice devIface(path.path(), this);
            if (devIface.deviceType() == DEVICE_TYPE_CDMA) {
                QModemManagerGsmNetworkInterface interface(devIface.udi());
                return interface.signalQuality();
            }
        }
        break;

//    case QSystemNetworkInfo::WlanMode:
//    case QSystemNetworkInfo::EthernetMode:
//    case QSystemNetworkInfo::BluetoothMode:
//    case QSystemNetworkInfo::WimaxMode:
//    case QSystemNetworkInfo::LteMode:
    default:
        break;
    }
#endif // QT_NO_NETWORKMANAGER

    return QSystemNetworkInfoLinuxCommonPrivate::networkSignalStrength(mode);
}

QString QSystemNetworkInfoPrivate::networkName(QSystemNetworkInfo::NetworkMode mode)
{
#if !defined(QT_NO_NETWORKMANAGER)
    switch (mode) {
    case QSystemNetworkInfo::GsmMode:
        foreach (const QDBusObjectPath &path, iface->getDevices()) {
            QNetworkManagerInterfaceDevice devIface(path.path(), this);
            if (devIface.deviceType() == DEVICE_TYPE_GSM) {
                QModemManagerGsmNetworkInterface interface(devIface.udi());
                if(interface.isValid()) {
                    MMRegistrationInfoType info = interface.registrationStatus();
                    return info.operatorName;
                }
            }
        }
        break;

    case QSystemNetworkInfo::CdmaMode:
    case QSystemNetworkInfo::WcdmaMode:
        foreach (const QDBusObjectPath &path, iface->getDevices()) {
            QNetworkManagerInterfaceDevice devIface(path.path(), this);
            if (devIface.deviceType() == DEVICE_TYPE_CDMA) {
                QModemManagerGsmNetworkInterface interface(devIface.udi());
                if(interface.isValid()) {
                    MMRegistrationInfoType info = interface.registrationStatus();
                    return info.operatorName;
                }
            }
        }
        break;

//    case QSystemNetworkInfo::WlanMode:
//    case QSystemNetworkInfo::EthernetMode:
//    case QSystemNetworkInfo::BluetoothMode:
//    case QSystemNetworkInfo::WimaxMode:
//    case QSystemNetworkInfo::LteMode:
    default:
        break;
    }
#endif // QT_NO_NETWORKMANAGER

    return QSystemNetworkInfoLinuxCommonPrivate::networkName(mode);
}

QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QSystemDisplayInfoLinuxCommonPrivate *parent)
    : QSystemDisplayInfoLinuxCommonPrivate(parent)
{
}

QSystemDisplayInfoPrivate::~QSystemDisplayInfoPrivate()
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
#if !defined(QT_NO_NETWORKMANAGER)

     QNetworkManagerInterface iface;
     foreach (const QDBusObjectPath &path, iface.getDevices()) {
         QNetworkManagerInterfaceDevice devIface(path.path(), this);
         if (devIface.deviceType() == DEVICE_TYPE_GSM) {
             QModemManagerModemGsmCard card(devIface.udi());
             return card.imei();
         }
     }

#endif
#if !defined(QT_NO_CONNMAN)
     if(ofonoAvailable()) {
         QOfonoManagerInterface ofonoManager;
         QString modem = ofonoManager.currentModem().path();
         if(!modem.isEmpty()) {
             QOfonoModemInterface modemIface(modem,this);

             QString imei = modemIface.getSerial();
             if(!imei.isEmpty()) {
                 return imei;
             }
         }
     }
#endif
#endif
     return QLatin1String("");
 }

QString QSystemDeviceInfoPrivate::imsi()
{
#if !defined(QT_NO_DBUS)
#if !defined(QT_NO_NETWORKMANAGER)
     QNetworkManagerInterface iface;
     foreach (const QDBusObjectPath &path, iface.getDevices()) {
         QNetworkManagerInterfaceDevice devIface(path.path(), this);
         if (devIface.deviceType() == DEVICE_TYPE_GSM) {
             QModemManagerModemGsmCard card(devIface.udi());
             return card.imsi();
         }
     }
#endif
#if !defined(QT_NO_CONNMAN)
     if(ofonoAvailable()) {
         QOfonoManagerInterface ofonoManager;
         QString modem = ofonoManager.currentModem().path();
         if(!modem.isEmpty()) {
             QOfonoSimInterface simInterface(modem,this);
             if(simInterface.isPresent()) {
                 QString id = simInterface.getImsi();
                 if(!id.isEmpty()) {
                     return id;
                 }
             }
         }
     }
#endif
#endif
         return QLatin1String("");
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

QString QSystemDeviceInfoPrivate::model()
{
    if(halAvailable()) {
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

QString QSystemDeviceInfoPrivate::productName()
{
    if(halAvailable()) {
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
        foreach (const QFileInfo &fileInfo, localeList) {
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

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
#if !defined(QT_NO_DBUS)
#if !defined(QT_NO_NETWORKMANAGER)
    QNetworkManagerInterface iface;
    foreach (const QDBusObjectPath &path, iface.getDevices()) {
        QNetworkManagerInterfaceDevice devIface(path.path(), this);
        if (devIface.deviceType() == DEVICE_TYPE_GSM
                || devIface.deviceType() == DEVICE_TYPE_CDMA) {
            QModemManager manager(this);
            if (manager.enumerateDevices().count() > 1) {
                return QSystemDeviceInfo::DualSimAvailable;
            }

            QModemManagerModem modem(devIface.udi());
            if(!modem.unlockRequired().isEmpty()) {
                return QSystemDeviceInfo::SimLocked;
            }
            return QSystemDeviceInfo::SingleSimAvailable;
        }
    }

#endif
#if !defined(QT_NO_CONNMAN)
     if(ofonoAvailable()) {
         QOfonoManagerInterface ofonoManager;
         QString modem = ofonoManager.currentModem().path();
         if(!modem.isEmpty()) {
             QOfonoSimInterface simInterface(modem,this);
             QString simpin = simInterface.pinRequired();
             if(simpin == "pin"
            || simpin == "phone"
            || simpin == "firstphone"
            || simpin == "pin2"
            || simpin == "puk"
            || simpin == "firstphonepuk"
            || simpin == "puk2"
            ) {
                 return QSystemDeviceInfo::SimLocked;
             }
             if(simInterface.isPresent()) {
                 return QSystemDeviceInfo::SingleSimAvailable;
             }
         }
     }
#endif
#endif
     return QSystemDeviceInfo::SimNotAvailable;
}

QSystemDeviceInfo::LockTypeFlags QSystemDeviceInfoPrivate::lockStatus()
{
    return QSystemDeviceInfo::UnknownLock;
}

int QSystemDeviceInfoPrivate::messageRingtoneVolume()
{
    return 0;
}

int QSystemDeviceInfoPrivate::voiceRingtoneVolume()
{
    return 0;
}

bool QSystemDeviceInfoPrivate::vibrationActive()
{
    return false;
}

QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
    : QObject(parent)
    , currentPid(0)
    , kdeIsRunning(0)
    , meegoIsRunning(0)
    , gnomeIsRunning(0)
{
    whichWMRunning();
}

QSystemScreenSaverPrivate::~QSystemScreenSaverPrivate()
{
    setScreenSaverInhibited(false);
#if defined(QT_NO_DBUS) && defined(Q_WS_X11) && !defined(Q_WS_MEEGO)
    changeTimeout(-1);
#endif
}

#if defined(Q_WS_X11)
int QSystemScreenSaverPrivate::changeTimeout(int timeout)
{
#if !defined(Q_WS_MEEGO)
    int ttime;
    int interval;
    int preferBlank;
    int allowExp;

    Display *dis = QX11Info::display();
    if (dis) {
        XGetScreenSaver(dis, &ttime, &interval, &preferBlank, &allowExp);
        int result = XSetScreenSaver(dis, timeout, interval, preferBlank, allowExp);
        return result;
    }
#endif // Q_WS_MEEGO
    Q_UNUSED(timeout)
    return 0;
}
#endif // Q_WS_X11

bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
{
    if (kdeIsRunning || gnomeIsRunning || meegoIsRunning) {
#if !defined(QT_NO_DBUS)
        const pid_t pid = getppid();
        QStringList ifaceList;
        ifaceList << QLatin1String("org.freedesktop.ScreenSaver");
        ifaceList << QLatin1String("org.gnome.ScreenSaver");
        QDBusInterface *connectionInterface;
        bool ok = false;
        QDBusReply<uint> reply;
        foreach (const QString iface, ifaceList) {
            connectionInterface = new QDBusInterface(QLatin1String(iface.toLatin1()),
                                                     QLatin1String("/ScreenSaver"),
                                                     QLatin1String(iface.toLatin1()),
                                                     QDBusConnection::sessionBus());
            if(connectionInterface->isValid()) {
                reply =  connectionInterface->call(QLatin1String("Inhibit"),
                                                   QString::number((int)pid),
                                                   QLatin1String("QSystemScreenSaver"));
                if (reply.isValid())
                    ok = true;
                if(ok) {
                    currentPid = reply.value();
                    screenSaverIsInhibited = true;
                    return reply.isValid();
                }
            }
        }
#endif // QT_NO_DBUS
    } else {
#if defined(Q_WS_X11) && !defined(Q_WS_MEEGO)
        changeTimeout(0);
        screenSaverIsInhibited = true;
#endif
    }
    return false;
}

bool QSystemScreenSaverPrivate::screenSaverInhibited()
{
    if (kdeIsRunning || gnomeIsRunning || meegoIsRunning)
        return screenSaverIsInhibited;

#if defined(Q_WS_X11) && !defined(Q_WS_MEEGO)
    int timeout;
    int interval;
    int preferBlank;
    int allowExp;
    Display *display = QX11Info::display();

    if (display) {
        XGetScreenSaver(display, &timeout, &interval, &preferBlank, &allowExp);
        if (preferBlank == DontPreferBlanking || timeout == 0)
            return true;
    }
#endif

    return false;
}

void QSystemScreenSaverPrivate::whichWMRunning()
{
#if !defined(QT_NO_DBUS)
    QDBusInterface *connectionInterface;
    connectionInterface = new QDBusInterface(QLatin1String("org.kde.kwin"),
                                             QLatin1String("/KWin"),
                                             QLatin1String("org.kde.KWin"),
                                             QDBusConnection::sessionBus());
    if (connectionInterface->isValid()) {
        kdeIsRunning = true;
        return;
    }

    if (QFileInfo("/etc/meego-release").exists()) {
        meegoIsRunning = true;
        return;
    }

    connectionInterface = new QDBusInterface(QLatin1String("org.gnome.SessionManager"),
                                             QLatin1String("/org/gnome/SessionManager"),
                                             QLatin1String("org.gnome.SessionManager"),
                                             QDBusConnection::systemBus());
    if (connectionInterface->isValid()) {
        gnomeIsRunning = true;
        return;
    }
#endif
}

bool QSystemScreenSaverPrivate::isScreenLockEnabled()
{
    if (kdeIsRunning) {
        QString kdeSSConfig;
        if (QDir( QDir::homePath() + QLatin1String("/.kde4/")).exists())
            kdeSSConfig = QDir::homePath() + QLatin1String("/.kde4/share/config/kscreensaverrc");
        else if (QDir(QDir::homePath() + QLatin1String("/.kde/")).exists())
            kdeSSConfig = QDir::homePath() + QLatin1String("/.kde/share/config/kscreensaverrc");

        QSettings kdeScreenSaveConfig(kdeSSConfig, QSettings::IniFormat);
        kdeScreenSaveConfig.beginGroup(QLatin1String("ScreenSaver"));
        if (kdeScreenSaveConfig.status() == QSettings::NoError)
            return kdeScreenSaveConfig.value(QLatin1String("Lock")).toBool();
    } else if (meegoIsRunning || gnomeIsRunning) {
    }

    return false;
}

bool QSystemScreenSaverPrivate::isScreenSaverActive()
{
#if !defined(QT_NO_DBUS)
    if (kdeIsRunning || gnomeIsRunning || meegoIsRunning) {
        const pid_t pid = getppid();
        QStringList ifaceList;
        ifaceList << QLatin1String("org.freedesktop.ScreenSaver");
        ifaceList << QLatin1String("org.gnome.ScreenSaver");
        QDBusInterface *connectionInterface;
        foreach (const QString iface, ifaceList) {
            connectionInterface = new QDBusInterface(QLatin1String(iface.toLatin1()),
                                                     QLatin1String("/ScreenSaver"),
                                                     QLatin1String(iface.toLatin1()),
                                                     QDBusConnection::systemBus());

            const QDBusReply<bool> reply =  connectionInterface->call(QLatin1String("GetActive"),
                                                                      QString::number((int)pid),
                                                                      QLatin1String("QSystemScreenSaver"));
            if (reply.isValid())
                return reply.value();
        }
    }
#endif // QT_NO_DBUS

    return false;
}

void QSystemScreenSaverPrivate::setScreenSaverInhibited(bool on)
{
    if (on) {
        setScreenSaverInhibit();
    } else {
#if !defined(QT_NO_DBUS)
        QStringList ifaceList;
        ifaceList << QLatin1String("org.freedesktop.ScreenSaver");
        ifaceList << QLatin1String("org.gnome.ScreenSaver");
        QDBusInterface *connectionInterface;
        foreach (const QString iface, ifaceList) {
            connectionInterface = new QDBusInterface(QLatin1String(iface.toLatin1()),
                                                     QLatin1String("/ScreenSaver"),
                                                     QLatin1String(iface.toLatin1()),
                                                     QDBusConnection::sessionBus());
            if (connectionInterface->isValid()) {
                connectionInterface->call(QLatin1String("UnInhibit"), currentPid);
                screenSaverIsInhibited = false;
            }
        }
#endif // QT_NO_DBUS
    }
}

QSystemBatteryInfoPrivate::QSystemBatteryInfoPrivate(QSystemBatteryInfoLinuxCommonPrivate *parent)
    : QSystemBatteryInfoLinuxCommonPrivate(parent)
{

}

QSystemBatteryInfoPrivate::~QSystemBatteryInfoPrivate()
{

}


#include "moc_qsysteminfo_linux_p.cpp"

QTM_END_NAMESPACE
