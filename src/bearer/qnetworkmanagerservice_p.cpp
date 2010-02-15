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

#include <QObject>
#include <QList>
#include <QtDBus/QtDBus>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusPendingCallWatcher>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusPendingCall>

#include "qnetworkmanagerservice_p.h"
#include "qnmdbushelper_p.h"

QTM_BEGIN_NAMESPACE

QNetworkManagerInterface::QNetworkManagerInterface(QObject *parent)
        : QDBusAbstractInterface((const QString &)NM_DBUS_SERVICE, (const QString &)NM_DBUS_PATH,(const char *)NM_DBUS_INTERFACE, QDBusConnection::systemBus(), parent)
{
    nmDBusHelper = new QNmDBusHelper;
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
                    this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));
    connect(nmDBusHelper,SIGNAL(pathForStateChanged(const QString &, quint32)),
            this, SIGNAL(stateChanged(const QString&, quint32)));

}

QNetworkManagerInterface::~QNetworkManagerInterface()
{
}

bool QNetworkManagerInterface::setConnections()
{
    if(!isValid() )
        return false;
    bool allOk = false;
    QDBusConnection dbusConnection = QDBusConnection::systemBus();

    if (!dbusConnection.connect(service(),
                                  QLatin1String(NM_DBUS_PATH),
                                  QLatin1String(NM_DBUS_INTERFACE),
                                  QLatin1String("PropertiesChanged"),
                                nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>)))) {
        allOk = true;
    }
    if (!dbusConnection.connect(service(),
                                  QLatin1String(NM_DBUS_PATH),
                                  QLatin1String(NM_DBUS_INTERFACE),
                                  QLatin1String("DeviceAdded"),
                                this,SIGNAL(deviceAdded(QDBusObjectPath)))) {
        allOk = true;
    }
    if (!dbusConnection.connect(service(),
                                  QLatin1String(NM_DBUS_PATH),
                                  QLatin1String(NM_DBUS_INTERFACE),
                                  QLatin1String("DeviceRemoved"),
                                  this,SIGNAL(deviceRemoved(QDBusObjectPath)))) {
        allOk = true;
    }

    return allOk;
}

QList <QDBusObjectPath> QNetworkManagerInterface::getDevices()
{
    QDBusReply<QList<QDBusObjectPath> > reply = call(QLatin1String("GetDevices"));
    return reply.value();
}

void QNetworkManagerInterface::activateConnection( const QString &serviceName,
                                                  QDBusObjectPath connectionPath,
                                                  QDBusObjectPath devicePath,
                                                  QDBusObjectPath specificObject)
{
    QDBusPendingCall pendingCall = asyncCall(QLatin1String("ActivateConnection"),
                                                                    QVariant(serviceName),
                                                                    QVariant::fromValue(connectionPath),
                                                                    QVariant::fromValue(devicePath),
                                                                    QVariant::fromValue(specificObject));

   QDBusPendingCallWatcher *callWatcher = new QDBusPendingCallWatcher(pendingCall, this);
   connect(callWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                    this, SIGNAL(activationFinished(QDBusPendingCallWatcher*)));
}

void QNetworkManagerInterface::deactivateConnection(QDBusObjectPath connectionPath)
{
   call(QLatin1String("DeactivateConnection"), QVariant::fromValue(connectionPath));
}

bool QNetworkManagerInterface::wirelessEnabled()  const
{
    return property("WirelessEnabled").toBool();
}

bool QNetworkManagerInterface::wirelessHardwareEnabled()  const
{
    return property("WirelessHardwareEnabled").toBool();
}

QList <QDBusObjectPath> QNetworkManagerInterface::activeConnections() const
{
    QVariant prop = property("ActiveConnections");
    return prop.value<QList<QDBusObjectPath> >();
}

quint32 QNetworkManagerInterface::state()
{
    return property("State").toUInt();
}

QNetworkManagerInterfaceAccessPoint::QNetworkManagerInterfaceAccessPoint(const QString &dbusPathName, QObject *parent)
        : QDBusAbstractInterface((const QString &)NM_DBUS_SERVICE, dbusPathName,(const char *)NM_DBUS_INTERFACE_ACCESS_POINT, QDBusConnection::systemBus(), parent)
{
    if (!isValid()) {
        qWarning() << "Could not find InterfaceAccessPoint";
        return;
    }
}

QNetworkManagerInterfaceAccessPoint::~QNetworkManagerInterfaceAccessPoint()
{
}

bool QNetworkManagerInterfaceAccessPoint::setConnections()
{
    if(!isValid() )
        return false;

    bool allOk = false;
    nmDBusHelper = new QNmDBusHelper;
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));

    if(QDBusConnection::systemBus().connect(service(),
                              path(),
                              interface(),
                              QLatin1String("PropertiesChanged"),
                              nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) ) {
        allOk = true;

    }
    return allOk;
}


quint32 QNetworkManagerInterfaceAccessPoint::flags() const
{
    return property("Flags").toUInt();
}

quint32 QNetworkManagerInterfaceAccessPoint::wpaFlags() const
{
    return property("WpaFlags").toUInt();
}

quint32 QNetworkManagerInterfaceAccessPoint::rsnFlags() const
{
    return property("RsnFlags").toUInt();
}

QString QNetworkManagerInterfaceAccessPoint::ssid() const
{
    return property("Ssid").toString();
}

quint32 QNetworkManagerInterfaceAccessPoint::frequency() const
{
    return property("Frequency").toUInt();
}

QString QNetworkManagerInterfaceAccessPoint::hwAddress() const
{
    return property("HwAddress").toString();
}

quint32 QNetworkManagerInterfaceAccessPoint::mode() const
{
    return property("Mode").toUInt();
}

quint32 QNetworkManagerInterfaceAccessPoint::maxBitrate() const
{
    return property("MaxBitrate").toUInt();
}

quint32 QNetworkManagerInterfaceAccessPoint::strength() const
{
    return property("Strength").toUInt();
}

QNetworkManagerInterfaceDevice::QNetworkManagerInterfaceDevice(const QString &deviceObjectPath, QObject *parent)
    : QDBusAbstractInterface((const QString &)NM_DBUS_SERVICE, deviceObjectPath,(const char *)NM_DBUS_INTERFACE_DEVICE, QDBusConnection::systemBus(), parent)
{
    if (!isValid()) {
        qWarning() << "Could not find NetworkManager";
        return;
    }
}

QNetworkManagerInterfaceDevice::~QNetworkManagerInterfaceDevice()
{
}


bool QNetworkManagerInterfaceDevice::setConnections()
{
    if(!isValid() )
        return false;

    bool allOk = false;
    nmDBusHelper = new QNmDBusHelper;
    connect(nmDBusHelper,SIGNAL(pathForStateChanged(const QString &, quint32)),
            this, SIGNAL(stateChanged(const QString&, quint32)));
    if(QDBusConnection::systemBus().connect(service(),
                              path(),
                              interface(),
                              QLatin1String("StateChanged"),
                              nmDBusHelper,SLOT(deviceStateChanged(quint32)))) {
        allOk = true;
    }
    return allOk;
}

QString QNetworkManagerInterfaceDevice::udi() const
{
    return property("Udi").toString();
}

QNetworkInterface QNetworkManagerInterfaceDevice::networkInterface() const
{
    return QNetworkInterface::interfaceFromName(property("Interface").toString());
}

quint32 QNetworkManagerInterfaceDevice::ip4Address() const
{
    return property("Ip4Address").toUInt();
}

quint32 QNetworkManagerInterfaceDevice::state() const
{
    return property("State").toUInt();
}

quint32 QNetworkManagerInterfaceDevice::deviceType() const
{
    return property("DeviceType").toUInt();
}

QDBusObjectPath QNetworkManagerInterfaceDevice::ip4config() const
{
    QVariant prop = property("Ip4Config");
    return prop.value<QDBusObjectPath>();
}

QNetworkManagerInterfaceDeviceWired::QNetworkManagerInterfaceDeviceWired(const QString &ifaceDevicePath, QObject *parent)
    : QDBusAbstractInterface((const QString &)NM_DBUS_SERVICE, ifaceDevicePath,(const char *)NM_DBUS_INTERFACE_DEVICE_WIRED, QDBusConnection::systemBus(), parent)
{

    if (!isValid()) {
        qWarning() << "Could not find InterfaceDeviceWired";
        return;
    }
}

QNetworkManagerInterfaceDeviceWired::~QNetworkManagerInterfaceDeviceWired()
{
}

bool QNetworkManagerInterfaceDeviceWired::setConnections()
{
    if(!isValid() )
        return false;

    bool allOk = false;

    nmDBusHelper = new QNmDBusHelper;
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));
    if(QDBusConnection::systemBus().connect(service(),
                              path(),
                              interface(),
                              QLatin1String("PropertiesChanged"),
                              nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) )  {
        allOk = true;
    }
    return allOk;
}

QString QNetworkManagerInterfaceDeviceWired::hwAddress() const
{
    return property("HwAddress").toString();
}

quint32 QNetworkManagerInterfaceDeviceWired::speed() const
{
    return property("Speed").toUInt();
}

bool QNetworkManagerInterfaceDeviceWired::carrier() const
{
    return property("Carrier").toBool();
}

QNetworkManagerInterfaceDeviceWireless::QNetworkManagerInterfaceDeviceWireless(const QString &ifaceDevicePath, QObject *parent)
    : QDBusAbstractInterface((const QString &)NM_DBUS_SERVICE, ifaceDevicePath,(const char *)NM_DBUS_INTERFACE_DEVICE_WIRELESS, QDBusConnection::systemBus(), parent)
{
    if (!isValid()) {
        qWarning() << "Could not find InterfaceDeviceWireless";
        return;
    }
}

QNetworkManagerInterfaceDeviceWireless::~QNetworkManagerInterfaceDeviceWireless()
{
}

bool QNetworkManagerInterfaceDeviceWireless::setConnections()
{
    if(!isValid() )
        return false;

    bool allOk = false;
    nmDBusHelper = new QNmDBusHelper;
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));

    connect(nmDBusHelper, SIGNAL(pathForAccessPointAdded(const QString &,QDBusObjectPath)),
            this,SIGNAL(accessPointAdded(const QString &,QDBusObjectPath)));

    connect(nmDBusHelper, SIGNAL(pathForAccessPointRemoved(const QString &,QDBusObjectPath)),
            this,SIGNAL(accessPointRemoved(const QString &,QDBusObjectPath)));
    QDBusConnection dbusConnection = QDBusConnection::systemBus();

    if(!dbusConnection.connect(service(),
                              path(),
                              interface(),
                              QLatin1String("AccessPointAdded"),
                              nmDBusHelper, SLOT(slotAccessPointAdded( QDBusObjectPath )))) {
        allOk = true;
    }


    if(!dbusConnection.connect(service(),
                              path(),
                              interface(),
                              QLatin1String("AccessPointRemoved"),
                              nmDBusHelper, SLOT(slotAccessPointRemoved( QDBusObjectPath )))) {
        allOk = true;
    }


    if(!dbusConnection.connect(service(),
                              path(),
                              interface(),
                              QLatin1String("PropertiesChanged"),
                              nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>)))) {
        allOk = true;
    }

    return allOk;
}


QList <QDBusObjectPath> QNetworkManagerInterfaceDeviceWireless::getAccessPoints()
{
    QDBusReply<QList<QDBusObjectPath> > reply = call(QLatin1String("GetAccessPoints"));
    return reply.value();
}

QString QNetworkManagerInterfaceDeviceWireless::hwAddress() const
{
    return property("HwAddress").toString();
}

quint32 QNetworkManagerInterfaceDeviceWireless::mode() const
{
    return property("Mode").toUInt();
}

quint32 QNetworkManagerInterfaceDeviceWireless::bitrate() const
{
    return property("Bitrate").toUInt();
}

QDBusObjectPath QNetworkManagerInterfaceDeviceWireless::activeAccessPoint() const
{
    return property("ActiveAccessPoint").value<QDBusObjectPath>();
}

quint32 QNetworkManagerInterfaceDeviceWireless::wirelessCapabilities() const
{
    return property("WirelelessCapabilities").toUInt();
}


QNetworkManagerSettings::QNetworkManagerSettings(const QString &settingsService, QObject *parent)
    : QDBusAbstractInterface(settingsService, (const QString &)NM_DBUS_PATH_SETTINGS,(const char *)NM_DBUS_IFACE_SETTINGS, QDBusConnection::systemBus(), parent)
{

    if (!isValid()) {
        qWarning() << "Could not find NetworkManagerSettings";
        return;
    }
}

QNetworkManagerSettings::~QNetworkManagerSettings()
{
}

bool QNetworkManagerSettings::setConnections()
{
    bool allOk = false;

    if (!QDBusConnection::systemBus().connect(service(), path(),
                           interface(), QLatin1String("NewConnection"),
                           this, SIGNAL(newConnection(QDBusObjectPath)))) {
        allOk = true;
    }

    return allOk;
}

QList <QDBusObjectPath> QNetworkManagerSettings::listConnections()
{
    QDBusReply<QList<QDBusObjectPath> > reply = call(QLatin1String("ListConnections"));
    return  reply.value();
}


QNetworkManagerSettingsConnection::QNetworkManagerSettingsConnection(const QString &settingsService, const QString &connectionObjectPath, QObject *parent)
    : QDBusAbstractInterface(settingsService, connectionObjectPath,(const char *)NM_DBUS_IFACE_SETTINGS_CONNECTION, QDBusConnection::systemBus(), parent)
{
    qDBusRegisterMetaType<QNmSettingsMap>();

    if (!isValid()) {
        qWarning() << "Could not find NetworkManagerSettingsConnection";
        return;
    }
    QDBusReply< QNmSettingsMap > rep = call(QLatin1String("GetSettings"));
    settingsMap = rep.value();
}

QNetworkManagerSettingsConnection::~QNetworkManagerSettingsConnection()
{
}

bool QNetworkManagerSettingsConnection::setConnections()
{
    if(!isValid() )
        return false;

    bool allOk = false;
    QDBusConnection dbusConnection = QDBusConnection::systemBus();
    if(!dbusConnection.connect(service(), path(),
                           interface(), QLatin1String("NewConnection"),
                           this, SIGNAL(updated(QNmSettingsMap)))) {
        allOk = true;
    }

    nmDBusHelper = new QNmDBusHelper;
    connect(nmDBusHelper, SIGNAL(pathForSettingsRemoved(const QString &)),
            this,SIGNAL(removed( const QString &)));

    if (!dbusConnection.connect(service(), path(),
                           interface(), QLatin1String("Removed"),
                           nmDBusHelper, SIGNAL(slotSettingsRemoved()))) {
        allOk = true;
    }

    return allOk;
}
//QNetworkManagerSettingsConnection::update(QNmSettingsMap map)
//{
//    d->connectionInterface->call("Update", QVariant::fromValue(map));
//}

QNmSettingsMap QNetworkManagerSettingsConnection::getSettings()
{
    QDBusReply< QNmSettingsMap > rep = call(QLatin1String("GetSettings"));
    settingsMap = rep.value();
    return settingsMap;
}

NMDeviceType QNetworkManagerSettingsConnection::getType()
{
    QNmSettingsMap::const_iterator i = settingsMap.find(QLatin1String("connection"));
    while (i != settingsMap.end() && i.key() == QLatin1String("connection")) {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("type"));
        while (ii != innerMap.end() && ii.key() == QLatin1String("type")) {
            QString devType = ii.value().toString();
            if (devType == QLatin1String("802-3-ethernet")) {
                return 	DEVICE_TYPE_802_3_ETHERNET;
            }
            if (devType == QLatin1String("802-11-wireless")) {
                return 	DEVICE_TYPE_802_11_WIRELESS;
            }
            ii++;
        }
        i++;
    }
    return 	DEVICE_TYPE_UNKNOWN;
}

bool QNetworkManagerSettingsConnection::isAutoConnect()
{
    QNmSettingsMap::const_iterator i = settingsMap.find(QLatin1String("connection"));
    while (i != settingsMap.end() && i.key() == QLatin1String("connection")) {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("autoconnect"));
        while (ii != innerMap.end() && ii.key() == QLatin1String("autoconnect")) {
            return ii.value().toBool();
            ii++;
        }
        i++;
    }
    return true; //default networkmanager is autoconnect
}

quint64 QNetworkManagerSettingsConnection::getTimestamp()
{
    QNmSettingsMap::const_iterator i = settingsMap.find(QLatin1String("connection"));
    while (i != settingsMap.end() && i.key() == QLatin1String("connection")) {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("timestamp"));
        while (ii != innerMap.end() && ii.key() == QLatin1String("timestamp")) {
            return ii.value().toUInt();
            ii++;
        }
        i++;
    }
    return 	0;
}

QString QNetworkManagerSettingsConnection::getId()
{
    QNmSettingsMap::const_iterator i = settingsMap.find(QLatin1String("connection"));
    while (i != settingsMap.end() && i.key() == QLatin1String("connection")) {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("id"));
        while (ii != innerMap.end() && ii.key() == QLatin1String("id")) {
            return ii.value().toString();
            ii++;
        }
        i++;
    }
    return 	QString();
}

QString QNetworkManagerSettingsConnection::getUuid()
{
    QNmSettingsMap::const_iterator i = settingsMap.find(QLatin1String("connection"));
    while (i != settingsMap.end() && i.key() == QLatin1String("connection")) {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("uuid"));
        while (ii != innerMap.end() && ii.key() == QLatin1String("uuid")) {
            return ii.value().toString();
            ii++;
        }
        i++;
    }
    // is no uuid, return the connection path
    return 	path();
}

QString QNetworkManagerSettingsConnection::getSsid()
{
    QNmSettingsMap::const_iterator i = settingsMap.find(QLatin1String("802-11-wireless"));
    while (i != settingsMap.end() && i.key() == QLatin1String("802-11-wireless")) {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("ssid"));
        while (ii != innerMap.end() && ii.key() == QLatin1String("ssid")) {
            return ii.value().toString();
            ii++;
        }
        i++;
    }
    return 	QString();
}

QString QNetworkManagerSettingsConnection::getMacAddress()
{
    if(getType() == DEVICE_TYPE_802_3_ETHERNET) {
        QNmSettingsMap::const_iterator i = settingsMap.find(QLatin1String("802-3-ethernet"));
        while (i != settingsMap.end() && i.key() == QLatin1String("802-3-ethernet")) {
            QMap<QString,QVariant> innerMap = i.value();
            QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("mac-address"));
            while (ii != innerMap.end() && ii.key() == QLatin1String("mac-address")) {
                return ii.value().toString();
                ii++;
            }
            i++;
        }
    }

    else if(getType() == DEVICE_TYPE_802_11_WIRELESS) {
        QNmSettingsMap::const_iterator i = settingsMap.find(QLatin1String("802-11-wireless"));
        while (i !=settingsMap.end() && i.key() == QLatin1String("802-11-wireless")) {
            QMap<QString,QVariant> innerMap = i.value();
            QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("mac-address"));
            while (ii != innerMap.end() && ii.key() == QLatin1String("mac-address")) {
                return ii.value().toString();
                ii++;
            }
            i++;
        }
    }
    return 	QString();
}

QStringList  QNetworkManagerSettingsConnection::getSeenBssids()
{
 if(getType() == DEVICE_TYPE_802_11_WIRELESS) {
        QNmSettingsMap::const_iterator i = settingsMap.find(QLatin1String("802-11-wireless"));
        while (i != settingsMap.end() && i.key() == QLatin1String("802-11-wireless")) {
            QMap<QString,QVariant> innerMap = i.value();
            QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("seen-bssids"));
            while (ii != innerMap.end() && ii.key() == QLatin1String("seen-bssids")) {
                return ii.value().toStringList();
                ii++;
            }
            i++;
        }
    }
 return QStringList();
}


QNetworkManagerConnectionActive::QNetworkManagerConnectionActive( const QString &activeConnectionObjectPath, QObject *parent)
    : QDBusAbstractInterface((const QString &)NM_DBUS_SERVICE, activeConnectionObjectPath,(const char *)NM_DBUS_INTERFACE_ACTIVE_CONNECTION, QDBusConnection::systemBus(), parent)
{
    if (!isValid()) {
        qWarning() << "Could not find NetworkManagerSettingsConnection";
        return;
    }
}

QNetworkManagerConnectionActive::~QNetworkManagerConnectionActive()
{
}

bool QNetworkManagerConnectionActive::setConnections()
{
    if(!isValid() )
        return false;

    bool allOk = false;
    nmDBusHelper = new QNmDBusHelper;
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));
    if(QDBusConnection::systemBus().connect(service(),
                              path(),
                              interface(),
                              QLatin1String("PropertiesChanged"),
                              nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) )  {
        allOk = true;
    }

    return allOk;
}

QString QNetworkManagerConnectionActive::serviceName() const
{
    return property("ServiceName").toString();
}

QDBusObjectPath QNetworkManagerConnectionActive::connection() const
{
    QVariant prop = property("Connection");
    return prop.value<QDBusObjectPath>();
}

QDBusObjectPath QNetworkManagerConnectionActive::specificObject() const
{
    QVariant prop = property("SpecificObject");
    return prop.value<QDBusObjectPath>();
}

QList<QDBusObjectPath> QNetworkManagerConnectionActive::devices() const
{
    QVariant prop = property("Devices");
    return prop.value<QList<QDBusObjectPath> >();
}

quint32 QNetworkManagerConnectionActive::state() const
{
    return property("State").toUInt();
}

bool QNetworkManagerConnectionActive::defaultRoute() const
{
    return property("Default").toBool();
}


QNetworkManagerIp4Config::QNetworkManagerIp4Config( const QString &deviceObjectPath, QObject *parent)
    : QDBusAbstractInterface((const QString &)NM_DBUS_SERVICE, deviceObjectPath,(const char *)NM_DBUS_INTERFACE_IP4_CONFIG, QDBusConnection::systemBus(), parent)
{
    if (!isValid()) {
        qWarning() << "Could not find NetworkManagerIp4Config";
        return;
    }
}

QNetworkManagerIp4Config::~QNetworkManagerIp4Config()
{
}

QStringList QNetworkManagerIp4Config::domains() const
{
    return property("Domains").toStringList();
}

#include "moc_qnetworkmanagerservice_p.cpp"

QTM_END_NAMESPACE
