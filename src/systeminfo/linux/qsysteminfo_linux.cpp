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

    if(transDir.exists()) {
        QStringList localeList = transDir.entryList( QStringList() << QLatin1String("qt_*.qm") ,QDir::Files
                                                     | QDir::NoDotAndDotDot, QDir::Name);
        foreach (const QString &localeName, localeList) {
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
#if defined(QT_NO_CONNMAN)
    iface = new QNetworkManagerInterface(this);

   foreach (const QDBusObjectPath &path, iface->getDevices()) {
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
#endif
}


bool QSystemNetworkInfoPrivate::isDefaultConnectionPath(const QString &path)
{
    bool isDefault = false;

#if defined(QT_NO_CONNMAN)
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
#endif
    return isDefault;
}

void QSystemNetworkInfoPrivate::primaryModeChanged()
{
    emit networkModeChanged(currentMode());
}



void QSystemNetworkInfoPrivate::updateActivePaths()
{
#if defined(QT_NO_CONNMAN)
    activePaths.clear();
    QScopedPointer<QNetworkManagerInterface> dbIface;
    dbIface.reset(new QNetworkManagerInterface(this));

    const QList <QDBusObjectPath> connections = dbIface->activeConnections();

    foreach (const QDBusObjectPath &activeconpath, connections) {
        QScopedPointer<QNetworkManagerConnectionActive> activeCon;
        activeCon.reset(new QNetworkManagerConnectionActive(activeconpath.path(), this));

        const QList<QDBusObjectPath> devices = activeCon->devices();
        foreach (const QDBusObjectPath &device, devices) {
            activePaths.insert(activeconpath.path(),device.path());
        }
    }
#endif
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

int QSystemNetworkInfoPrivate::cellId()
{
#if !defined(QT_NO_CONNMAN)
    return QSystemNetworkInfoLinuxCommonPrivate::cellId();
#endif
    return -1;
}

int QSystemNetworkInfoPrivate::locationAreaCode()
{
#if !defined(QT_NO_CONNMAN)
    return QSystemNetworkInfoLinuxCommonPrivate::locationAreaCode();
#endif
    return -1;
}

QString QSystemNetworkInfoPrivate::currentMobileCountryCode()
{
#if !defined(QT_NO_CONNMAN)
    return QSystemNetworkInfoLinuxCommonPrivate::currentMobileCountryCode();
#endif
    return QString();
}

QString QSystemNetworkInfoPrivate::currentMobileNetworkCode()
{
#if !defined(QT_NO_CONNMAN)
    return QSystemNetworkInfoLinuxCommonPrivate::currentMobileNetworkCode();
#endif
    return QString();
}

QString QSystemNetworkInfoPrivate::homeMobileCountryCode()
{
#if !defined(QT_NO_CONNMAN)
    return QSystemNetworkInfoLinuxCommonPrivate::homeMobileCountryCode();
#endif
    return QString();
}

QString QSystemNetworkInfoPrivate::homeMobileNetworkCode()
{
#if !defined(QT_NO_CONNMAN)
    return QSystemNetworkInfoLinuxCommonPrivate::homeMobileNetworkCode();
#endif
    return QString();
}

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::currentMode()
{
#if !defined(QT_NO_CONNMAN)
    return QSystemNetworkInfoLinuxCommonPrivate::currentMode();
#endif

    QSystemNetworkInfo::NetworkMode mode = QSystemNetworkInfo::UnknownMode;
#if !defined(QT_NO_NETWORKMANAGER)
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
#endif

    return mode;
}

QSystemDisplayInfoPrivate::QSystemDisplayInfoPrivate(QSystemDisplayInfoLinuxCommonPrivate *parent)
        : QSystemDisplayInfoLinuxCommonPrivate(parent)
{
}

QSystemDisplayInfoPrivate::~QSystemDisplayInfoPrivate()
{
}

QSystemDisplayInfo::DisplayOrientation QSystemDisplayInfoPrivate::orientation(int screen)
{
    QSystemDisplayInfo::DisplayOrientation orientation = QSystemDisplayInfo::Unknown;
#if defined(Q_WS_X11)
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
#else
Q_UNUSED(screen)
#endif
    return orientation;
}


float QSystemDisplayInfoPrivate::contrast(int screen)
{
    Q_UNUSED(screen);

    return 0.0;
}

int QSystemDisplayInfoPrivate::getDPIWidth(int screen)
{
#if defined(Q_WS_X11)
    return QX11Info::appDpiY(screen);
#else
    return 0;
#endif
}

int QSystemDisplayInfoPrivate::getDPIHeight(int screen)
{
#if defined(Q_WS_X11)
    return QX11Info::appDpiX(screen);
#else
    return 0;
#endif
}

int QSystemDisplayInfoPrivate::physicalHeight(int screen)
{
    int height=0;
#if defined(Q_WS_X11)
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
#else
Q_UNUSED(screen)
#endif
    return height;
}

int QSystemDisplayInfoPrivate::physicalWidth(int screen)
{
    int width=0;
#if defined(Q_WS_X11)
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
#else
Q_UNUSED(screen)
#endif
    return width;
}

QSystemDisplayInfo::BacklightState  QSystemDisplayInfoPrivate::backlightStatus(int screen)
{
    Q_UNUSED(screen)
    return QSystemDisplayInfo::BacklightStateUnknown;
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

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
#if !defined(QT_NO_DBUS)
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

QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QSystemScreenSaverLinuxCommonPrivate *parent)
         : QSystemScreenSaverLinuxCommonPrivate(parent), currentPid(0)
 {
     kdeIsRunning = false;
     gnomeIsRunning = false;
     meegoIsRunning = false;
     whichWMRunning();
 }

 QSystemScreenSaverPrivate::~QSystemScreenSaverPrivate()
 {
     if(currentPid != 0) {
#if !defined(QT_NO_DBUS)
         QDBusConnection dbusConnection = QDBusConnection::sessionBus();

         QStringList ifaceList;
         ifaceList <<  QLatin1String("org.freedesktop.ScreenSaver");
         ifaceList << QLatin1String("org.gnome.ScreenSaver");
         QDBusInterface *connectionInterface;
         foreach (const QString iface, ifaceList) {
             connectionInterface = new QDBusInterface(QLatin1String(iface.toLatin1()),
                                                      QLatin1String("/ScreenSaver"),
                                                      QLatin1String(iface.toLatin1()),
                                                      dbusConnection);
             if(connectionInterface->isValid()) {
                 QDBusReply<uint> reply =  connectionInterface->call(QLatin1String("UnInhibit"),
                                                                     currentPid);
             }
         }
#endif
     }
#if defined(QT_NO_DBUS) && defined(Q_WS_X11) && !defined(Q_WS_MEEGO)
     changeTimeout(-1);
#endif
 }

 int QSystemScreenSaverPrivate::changeTimeout(int timeout)
 {
#if defined(Q_WS_X11) && !defined(Q_WS_MEEGO)

     int ttime;
     int interval;
     int preferBlank;
     int allowExp;

     Display *dis = QX11Info::display();
     if(dis) {
         XGetScreenSaver(dis, &ttime, &interval, &preferBlank, &allowExp);
         int result = XSetScreenSaver(QX11Info::display(), timeout, interval, preferBlank, allowExp);
         return result;
     }
#else
Q_UNUSED(timeout)
#endif
     return 0;
 }

 bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
 {
     if(kdeIsRunning || gnomeIsRunning || meegoIsRunning) {
#if !defined(QT_NO_DBUS)
         const pid_t pid = getppid();
         QDBusConnection dbusConnection = QDBusConnection::sessionBus();

         QStringList ifaceList;
         ifaceList <<  QLatin1String("org.freedesktop.ScreenSaver");
         ifaceList << QLatin1String("org.gnome.ScreenSaver");
         QDBusInterface *connectionInterface;
         foreach (const QString iface, ifaceList) {
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
                 }
             }
         }
#endif
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
    if(kdeIsRunning || gnomeIsRunning || meegoIsRunning) {
        if(currentPid != 0) {
            return true;
        } else {
            return false;
        }
    }

#if defined(Q_WS_X11) && !defined(Q_WS_MEEGO)

    int timeout;
    int interval;
    int preferBlank;
    int allowExp;
    XGetScreenSaver(QX11Info::display(), &timeout, &interval, &preferBlank, &allowExp);
    if(preferBlank == DontPreferBlanking || timeout == 0) {
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
    if(QFileInfo("/etc/meego-release").exists()) {
        meegoIsRunning = true;
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
    } else if(meegoIsRunning || gnomeIsRunning) {

    }

   return false;
}

bool QSystemScreenSaverPrivate::isScreenSaverActive()
{
    if(kdeIsRunning || gnomeIsRunning || meegoIsRunning) {
#if !defined(QT_NO_DBUS)
        const pid_t pid = getppid();
        QDBusConnection dbusConnection = QDBusConnection::sessionBus();

        QStringList ifaceList;
        ifaceList <<  QLatin1String("org.freedesktop.ScreenSaver");
        ifaceList << QLatin1String("org.gnome.ScreenSaver");
        QDBusInterface *connectionInterface;
        foreach (const QString iface, ifaceList) {
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

QSystemBatteryInfoPrivate::QSystemBatteryInfoPrivate(QSystemBatteryInfoLinuxCommonPrivate *parent)
    : QSystemBatteryInfoLinuxCommonPrivate(parent)
{

}

QSystemBatteryInfoPrivate::~QSystemBatteryInfoPrivate()
{

}


#include "moc_qsysteminfo_linux_p.cpp"

QTM_END_NAMESPACE
