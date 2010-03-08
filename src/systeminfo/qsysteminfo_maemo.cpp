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

#if !defined(QT_NO_DBUS)
QDBusArgument &operator<<(QDBusArgument &argument, const ProfileDataValue &value)
{
  argument.beginStructure();
  argument << value.key << value.val << value.type;
  argument.endStructure();
  return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, ProfileDataValue &value)
{
  argument.beginStructure();
  argument >> value.key >> value.val >> value.type;
  argument.endStructure();
  return argument;
}
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
    QStringList languages;

    GConfItem languagesItem("/apps/osso/inputmethod/available_languages");
    const QStringList locales = languagesItem.value().toStringList();

    foreach(const QString locale, locales) {
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
            const QStringList locationKeys = locationValues.listEntries();

            foreach (const QString str, locationKeys) {
                if (str.contains("sim_imsi"))
                    featureSupported = true;
                break;
            }
        }
        break;
    case QSystemInfo::LocationFeature :
        {
            GConfItem locationValues("/system/nokia/location");
            const QStringList locationKeys = locationValues.listEntries();
            if(locationKeys.count()) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::VideoOutFeature :
        {
            const QString sysPath = "/sys/class/video4linux/";
            const QDir sysDir(sysPath);
            QStringList filters;
            filters << "*";
            const QStringList sysList = sysDir.entryList( filters ,QDir::Dirs, QDir::Name);
            if(sysList.contains("video0")) {
                featureSupported = true;
            }
        }
        break;
    case QSystemInfo::HapticsFeature:
        {
           // if(halIsAvailable) {
                QHalInterface iface;
                const QStringList touchSupport =
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

QSystemNetworkInfo::NetworkMode QSystemNetworkInfoPrivate::currentMode()
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
    setConnection();
    flightMode = false;
 #if !defined(QT_NO_DBUS)
    previousPowerState = QSystemDeviceInfo::UnknownPower;
    setupBluetooth();
    setupProfile();
#endif
}

QSystemDeviceInfoPrivate::~QSystemDeviceInfoPrivate()
{
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
        if((map.at(i).toString() == "maemo.charger.connection_status")
        || (map.at(i).toString() == "maemo.rechargeable.charging_status")) {
            QSystemDeviceInfo::PowerState state = currentPowerState();
            if (previousPowerState != state)
                emit powerStateChanged(state);
            previousPowerState = state;
       }} //end map
}
#endif

QSystemDeviceInfo::Profile QSystemDeviceInfoPrivate::currentProfile()
{
#if !defined(QT_NO_DBUS)
    if (flightMode)
        return QSystemDeviceInfo::OfflineProfile;

    if (silentProfile )
        return vibratingAlertEnabled ? QSystemDeviceInfo::VibProfile : QSystemDeviceInfo::SilentProfile;

    if (ringingAlertVolume > 75)
        return QSystemDeviceInfo::LoudProfile;

    return QSystemDeviceInfo::NormalProfile;
#endif

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
    return GConfItem("/system/nokia/location/sim_imsi").value().toString();
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfoPrivate::simStatus()
{
    GConfItem locationValues("/system/nokia/location");
    const QStringList locationKeys = locationValues.listEntries();
    QStringList result;
    int count = 0;
    foreach (const QString str, locationKeys) {
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
#if !defined(QT_NO_DBUS)
    QDBusConnection systemDbusConnection = QDBusConnection::systemBus();

    QDBusInterface mceConnectionInterface("com.nokia.mce",
                                      "/com/nokia/mce/request",
                                      "com.nokia.mce.request",
                                      systemDbusConnection);
    if (mceConnectionInterface.isValid()) {
        QDBusReply<QString> tkLockModeReply = mceConnectionInterface.call("get_tklock_mode");
        return tkLockModeReply.value() == "locked";
    }

    qWarning() << "mce interface not valid";
#endif
    return false;
}

QSystemDeviceInfo::PowerState QSystemDeviceInfoPrivate::currentPowerState()
{
#if !defined(QT_NO_DBUS)
        QHalInterface iface;
        const QStringList list = iface.findDeviceByCapability("battery");
        if(!list.isEmpty()) {
            foreach(const QString dev, list) {
                QHalDeviceInterface ifaceDevice(dev);
                if (iface.isValid()) {
                    if (ifaceDevice.getPropertyString("maemo.charger.connection_status") == "connected") {
                        if (ifaceDevice.getPropertyString("maemo.rechargeable.charging_status") == "full")
                            return QSystemDeviceInfo::WallPower;
                        return QSystemDeviceInfo::WallPowerChargingBattery;
                    }
                    return QSystemDeviceInfo::BatteryPower;
                }
            }
        }
#endif
    return QSystemDeviceInfo::UnknownPower;
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

#if !defined(QT_NO_DBUS)

void QSystemDeviceInfoPrivate::setupProfile()
{
    QDBusConnection systemDbusConnection = QDBusConnection::systemBus();

    QDBusInterface mceConnectionInterface("com.nokia.mce",
                                      "/com/nokia/mce/request",
                                      "com.nokia.mce.request",
                                      systemDbusConnection);
    if (!mceConnectionInterface.isValid()) {
        qWarning() << "mce interface not valid";
        return;
    } else {
        QDBusReply<QString> deviceModeReply = mceConnectionInterface.call("get_device_mode");
        flightMode = deviceModeReply.value() == "flight";
    }

    if (!systemDbusConnection.connect("com.nokia.mce",
                           "/com/nokia/mce/signal",
                           "com.nokia.mce.signal",
                           "sig_device_mode_ind",
                           this, SLOT(deviceModeChanged(QString)))) {
        qWarning() << "unable to connect to sig_device_mode_ind";
    }


    QDBusInterface connectionInterface("com.nokia.profiled",
                                      "/com/nokia/profiled",
                                      "com.nokia.profiled",
                                      QDBusConnection::sessionBus());
    if(!connectionInterface.isValid()) {
       qWarning() << "profiled interface not valid";
       return;
    }

    QDBusReply<QString> profileNameReply = connectionInterface.call("get_profile");
    if (profileNameReply.isValid())
        profileName = profileNameReply.value();

    QDBusReply<QString> ringingAlertTypeReply = connectionInterface.call("get_value", profileName, "ringing.alert.type");
    if (ringingAlertTypeReply.isValid())
        silentProfile = ringingAlertTypeReply.value() == "silent";

    QDBusReply<QString> vibratingAlertEnabledReply = connectionInterface.call("get_value", profileName, "vibrating.alert.enabled");
    if (vibratingAlertEnabledReply.isValid())
        vibratingAlertEnabled = vibratingAlertEnabledReply.value() == "On";

    QDBusReply<QString> ringingAlertVolumeReply = connectionInterface.call("get_value", profileName, "ringing.alert.volume");
    if (ringingAlertVolumeReply.isValid())
        ringingAlertVolume = ringingAlertVolumeReply.value().toInt();

    qDBusRegisterMetaType<ProfileDataValue>();
    qDBusRegisterMetaType<QList<ProfileDataValue> >();

    QDBusConnection sessionDbusConnection = QDBusConnection::sessionBus();
    if (!sessionDbusConnection.connect("com.nokia.profiled",
                           "/com/nokia/profiled",
                           "com.nokia.profiled",
                           "profile_changed",
                           this, SLOT(profileChanged(bool, bool, QString, QList<ProfileDataValue>)))) {
        qWarning() << "unable to connect to profile_changed";
    }

}

void QSystemDeviceInfoPrivate::deviceModeChanged(QString newMode)
{
    bool previousFlightMode = flightMode;
    flightMode = newMode == "flight";
    if (previousFlightMode != flightMode)
        emit currentProfileChanged(currentProfile());
}

void QSystemDeviceInfoPrivate::profileChanged(bool changed, bool active, QString profile, QList<ProfileDataValue> values)
{
    const QSystemDeviceInfo::Profile previousProfile = currentProfile();

    profileName = profile;
    foreach (const ProfileDataValue value, values) {
        if (value.key == "ringing.alert.type")
            silentProfile = value.val == "silent";
        else if (value.key == "vibrating.alert.enabled")
            vibratingAlertEnabled = value.val == "On";
        else if (value.key == "ringing.alert.volume")
            ringingAlertVolume = value.val.toInt();
    }

    QSystemDeviceInfo::Profile newProfile = currentProfile();
    if (previousProfile != newProfile)
        emit currentProfileChanged(newProfile);
}

#endif

//////////////
///////
QSystemScreenSaverPrivate::QSystemScreenSaverPrivate(QObject *parent)
        : QSystemScreenSaverLinuxCommonPrivate(parent), m_screenSaverInhibited(false)

{
    ssTimer = new QTimer(this);
}

QSystemScreenSaverPrivate::~QSystemScreenSaverPrivate()
{
     if(ssTimer->isActive())
         ssTimer->stop();

     m_screenSaverInhibited = false;
}

bool QSystemScreenSaverPrivate::screenSaverInhibited()
{
    return m_screenSaverInhibited;
}

bool QSystemScreenSaverPrivate::setScreenSaverInhibit()
{
    if (m_screenSaverInhibited)
        return true;

     m_screenSaverInhibited = true;
     display_blanking_pause();
     connect(ssTimer, SIGNAL(timeout()), this, SLOT(display_blanking_pause()));
     ssTimer->start(3000); //3 seconds interval
     return true;
}

void QSystemScreenSaverPrivate::display_blanking_pause()
{
#if !defined(QT_NO_DBUS)
    QDBusInterface connectionInterface("com.nokia.mce",
                                       "/com/nokia/mce/request",
                                       "com.nokia.mce.request",
                                       QDBusConnection::systemBus());
    if (!connectionInterface.isValid()) {
        qWarning() << "interface not valid";
        return;
    }
    connectionInterface.call("req_display_blanking_pause");
#endif
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
