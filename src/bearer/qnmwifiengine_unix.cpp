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

#include "qnmwifiengine_unix_p.h"
#include "qnetworkconfiguration_p.h"
#include <qnetworkconfiguration.h>

#include <QtCore/qstringlist.h>
#include <QScopedPointer>

#include <QtNetwork/qnetworkinterface.h>
#include <NetworkManager/NetworkManager.h>
#include <qnetworkmanagerservice_p.h>

#include <QNetworkInterface>



QTM_BEGIN_NAMESPACE
Q_GLOBAL_STATIC(QNmWifiEngine, nmWifiEngine)
typedef  QList<QList<uint> > QNmSettingsAddressMap;
QTM_END_NAMESPACE

Q_DECLARE_METATYPE(QTM_PREPEND_NAMESPACE(QNmSettingsAddressMap))

QTM_BEGIN_NAMESPACE

QNmWifiEngine::QNmWifiEngine(QObject *parent)
:   QNetworkSessionEngine(parent)
{
    iface = new QNetworkManagerInterface(this);
    if(!iface->isValid()) {
        return;
    }
    iface->setConnections();
    connect(iface,SIGNAL(deviceAdded(QDBusObjectPath)),
            this,SLOT(addDevice(QDBusObjectPath)));
    connect(iface,SIGNAL(deviceRemoved(QDBusObjectPath)),
            this,SLOT(removeDevice(QDBusObjectPath)));
    connect(iface, SIGNAL(activationFinished(QDBusPendingCallWatcher*)),
            this, SLOT(slotActivationFinished(QDBusPendingCallWatcher*)));

    QList<QDBusObjectPath> list = iface->getDevices();

    foreach(QDBusObjectPath path, list) {
        addDevice(path);
    }

    QStringList connectionServices;
    connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
    connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;
    QNetworkManagerSettings *settingsiface;
    foreach (QString service, connectionServices) {
        settingsiface = new QNetworkManagerSettings(service, this);
        settingsiface->setConnections();
        connect(settingsiface,SIGNAL(newConnection(QDBusObjectPath)),
                this,(SLOT(newConnection(QDBusObjectPath))));
    }

    updated = false;
}

QNmWifiEngine::~QNmWifiEngine()
{
}

QString QNmWifiEngine::getNameForConfiguration(QNetworkManagerInterfaceDevice *devIface)
{
    QString newname;
    if (devIface->state() == NM_DEVICE_STATE_ACTIVATED) {
        QString path = devIface->ip4config().path();
        QNetworkManagerIp4Config * ipIface;
        ipIface = new QNetworkManagerIp4Config(path);
        newname = ipIface->domains().join(" ");
        delete ipIface;
    }
    //fallback to interface name
    if(newname.isEmpty())
        newname = devIface->interface().name();
    return newname;
}


QList<QNetworkConfigurationPrivate *> QNmWifiEngine::getConfigurations(bool *ok)
{
//    qWarning() << Q_FUNC_INFO << updated;
    if (ok)
        *ok = false;

    if(!updated) {
        foundConfigurations.clear();
        if(knownSsids.isEmpty())
            getKnownSsids(); // list of ssids that have user configurations.

        scanForAccessPoints();
        getActiveConnectionsPaths();
        knownConnections();

        accessPointConnections();

//        findConnections();
        //add access points
        updated = true;
    }
    return foundConfigurations;
}

void QNmWifiEngine::findConnections()
{
    QList<QDBusObjectPath> list = iface->getDevices();

    QScopedPointer<QNetworkManagerInterfaceDevice> devIface;
    foreach(QDBusObjectPath path, list) {
        devIface.reset(new QNetworkManagerInterfaceDevice(path.path()));

        //// eth
        switch (devIface->deviceType()) {
//            qWarning() << devIface->connectionInterface()->path();

        case DEVICE_TYPE_802_3_ETHERNET:
            {
                QString ident;
                QScopedPointer<QNetworkManagerInterfaceDeviceWired> devWiredIface;
                devWiredIface.reset(new QNetworkManagerInterfaceDeviceWired(devIface->connectionInterface()->path()));

                ident = devWiredIface->hwAddress();

                QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
                cpPriv->name = getNameForConfiguration(devIface.data());
                cpPriv->isValid = true;
                cpPriv->id = ident;
                cpPriv->internet = devWiredIface->carrier();

                cpPriv->serviceInterface = devIface->interface();
                cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
                switch (devIface->state()) {
                case  NM_DEVICE_STATE_UNKNOWN:
                case NM_DEVICE_STATE_UNMANAGED:
                case NM_DEVICE_STATE_FAILED:
                    cpPriv->state = (cpPriv->state | QNetworkConfiguration::Undefined);
                    break;
                case  NM_DEVICE_STATE_UNAVAILABLE:
                    cpPriv->state = (cpPriv->state | QNetworkConfiguration::Defined);
                    break;
                case NM_DEVICE_STATE_PREPARE:
                case NM_DEVICE_STATE_CONFIG:
                case NM_DEVICE_STATE_NEED_AUTH:
                case NM_DEVICE_STATE_IP_CONFIG:
                case NM_DEVICE_STATE_DISCONNECTED:
                    {
                        cpPriv->state = ( cpPriv->state | QNetworkConfiguration::Discovered
                                          | QNetworkConfiguration::Defined);
                    }
                    break;
                case NM_DEVICE_STATE_ACTIVATED:
                    cpPriv->state = (cpPriv->state | QNetworkConfiguration::Active );
                    break;
                default:
                    cpPriv->state = (cpPriv->state | QNetworkConfiguration::Undefined);
                    break;
                };
                cpPriv->purpose = QNetworkConfiguration::PublicPurpose;
                foundConfigurations.append(cpPriv);
                configurationInterface[cpPriv->id] = cpPriv->serviceInterface.name();
            }
            break;
        case DEVICE_TYPE_802_11_WIRELESS:
            {
//                QNetworkManagerInterfaceDeviceWireless *devWirelessIface;
//                devWirelessIface = new QNetworkManagerInterfaceDeviceWireless(devIface->connectionInterface()->path());
//
//               //// connections
//                QStringList connectionServices;
//                connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
//                connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;
//
//                QString connPath;
//
//                foreach (QString service, connectionServices) {
//                    QString ident;
//                    QNetworkManagerSettings *settingsiface;
//                    settingsiface = new QNetworkManagerSettings(service);
//                    QList<QDBusObjectPath> list = settingsiface->listConnections();
//
//                    foreach(QDBusObjectPath path, list) { //for each connection path
//qWarning() << path.path();
//                        ident = path.path();
//                        bool addIt = false;
//                        QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
//                        cpPriv->isValid = true;
//                        cpPriv->id = ident;
//                        cpPriv->internet = true;
//
//                        cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
//                        cpPriv->state = ( cpPriv->state | QNetworkConfiguration::Discovered
//                                          | QNetworkConfiguration::Defined);
//                        cpPriv->purpose = QNetworkConfiguration::PrivatePurpose;
//
//                        QNetworkManagerSettingsConnection *sysIface;
//                        sysIface = new QNetworkManagerSettingsConnection(service, path.path());
//                        cpPriv->name = sysIface->getId();//ii.value().toString();
//qWarning() << cpPriv->name;
//                        if(sysIface->getType() ==  DEVICE_TYPE_802_3_ETHERNET/*type == "802-3-ethernet"*/
//                           && devIface->deviceType() == DEVICE_TYPE_802_3_ETHERNET) {
//                            cpPriv->serviceInterface = devIface->interface();
//                            addIt = true;
//                        } else if(sysIface->getType() == DEVICE_TYPE_802_11_WIRELESS/*type == "802-11-wireless"*/
//                                  && devIface->deviceType() == DEVICE_TYPE_802_11_WIRELESS) {
//                            cpPriv->serviceInterface = devIface->interface();
//                            addIt = true;
//                            // get the wifi interface state first.. do we need this?
//                            //   QString activeAPPath = devWirelessIface->activeAccessPoint().path();
//                        }
//
//                        //#if 0
//                        foreach(QString conpath, activeConnectionPaths) {
//                            QNetworkManagerConnectionActive *aConn;
//                            aConn = new QNetworkManagerConnectionActive(conpath);
//                            // in case of accesspoint, specificObject will hold the accessPOintObjectPath
//                            //                        qWarning() << aConn->connection().path() << aConn->specificObject().path() << aConn->devices().count();
//                            if( aConn->connection().path() == ident) {
//
//                                QList <QDBusObjectPath>  devs = aConn->devices();
//                                foreach(QDBusObjectPath device, devs) {
//                                    QNetworkManagerInterfaceDevice *ifaceDevice;
//                                    ifaceDevice = new QNetworkManagerInterfaceDevice(device.path());
//                                    cpPriv->serviceInterface = ifaceDevice->interface();
//                                    cpPriv->state = getStateFlag(ifaceDevice->state());
//                                    //cpPriv->accessPoint = aConn->specificObject().path();
//
//                                    break;
//                                }
//                            }
//                        }
//                        //#endif
//              //      } //end while connection
//                    if(addIt) {
//                        foundConfigurations.append(cpPriv);
//                        configurationInterface[cpPriv->id] = cpPriv->serviceInterface.name();
//                    }
//                }
//            } //end each connection service
//
//            //                ////////////// AccessPoints
////                QList<QDBusObjectPath> apList = devWirelessIface->getAccessPoints();
//////                qWarning() << apList.count();
////                foreach(QDBusObjectPath path, apList) {
////                    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
////                    cpPriv = addAccessPoint( devIface->connectionInterface()->path(), path);
////                    if(cpPriv->isValid) {
////                        foundConfigurations.append(cpPriv);
////                    }
////                }
            } // end DEVICE_TYPE_802_11_WIRELESS
            break;
        };
    } //end foreach device
}

void QNmWifiEngine::knownConnections()
{
//    qWarning() << Q_FUNC_INFO;
    //// connections
    QStringList connectionServices;
    connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
    connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;

    QString connPath;

    QScopedPointer<QNetworkManagerSettings> settingsiface;
    foreach (QString service, connectionServices) {
        QString ident;
        settingsiface.reset(new QNetworkManagerSettings(service));
        QList<QDBusObjectPath> list = settingsiface->listConnections();

        QScopedPointer<QNetworkManagerSettingsConnection> sysIface;
        QScopedPointer<QNetworkConfigurationPrivate> cpPriv;

        foreach(QDBusObjectPath path, list) { //for each connection path
            //qWarning() << "COnnection path:" << path.path();
            ident = path.path();
            bool addIt = false;

            cpPriv.reset(new QNetworkConfigurationPrivate());
            sysIface.reset(new QNetworkManagerSettingsConnection(service, path.path(), this));
            sysIface->setConnections();

            connect(sysIface.data(), SIGNAL(removed(QString)),
                    this,SLOT(settingsConnectionRemoved(QString)));

            cpPriv->name = sysIface->getId();
            cpPriv->isValid = true;
            cpPriv->id = sysIface->getUuid();
            cpPriv->internet = true;
            cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
            cpPriv->state = getStateForId(cpPriv->id);

            cpPriv->purpose = QNetworkConfiguration::PrivatePurpose;

            if(sysIface->getType() == DEVICE_TYPE_802_3_ETHERNET) {
                QString mac = sysIface->getMacAddress();
                if(!mac.length() > 2) {
                    //qWarning() <<"XXXXXXXXXXXXXXXXXXXXXXXXX" << mac << "type ethernet";
                    QString devPath;
                    devPath = deviceConnectionPath(mac);

                    QNetworkManagerInterfaceDevice devIface(devPath);
                    cpPriv->serviceInterface = devIface.interface();
                    QScopedPointer<QNetworkManagerInterfaceDeviceWired> devWiredIface;
                    devWiredIface.reset(new QNetworkManagerInterfaceDeviceWired(devIface.connectionInterface()->path()));
                    cpPriv->internet = devWiredIface->carrier();

                    // use this mac addy
                } else {
                    cpPriv->serviceInterface = getBestInterface( DEVICE_TYPE_802_3_ETHERNET, cpPriv->id);
                }

                cpPriv->internet = true;//sysIface->isAutoConnect();

                addIt = true;
            } else if(sysIface->getType() == DEVICE_TYPE_802_11_WIRELESS) {
                QString mac = sysIface->getMacAddress();;
                if(!mac.length() > 2) {
                    qWarning() <<"XXXXXXXXXXXXXXXXXXXXXXXXX" << mac << "type wireless";
                    QString devPath;
                    devPath = deviceConnectionPath(mac);
//                    qWarning() << Q_FUNC_INFO << devPath;

                    QNetworkManagerInterfaceDevice devIface(devPath);
                    cpPriv->serviceInterface = devIface.interface();
                    // use this mac addy
                } else {
                    cpPriv->serviceInterface = getBestInterface( DEVICE_TYPE_802_11_WIRELESS, cpPriv->id);
                }
                addIt = true;
                // get the wifi interface state first.. do we need this?
                //   QString activeAPPath = devWirelessIface->activeAccessPoint().path();
            }
            if(addIt) {
                foundConfigurations.append(cpPriv.data());
                configurationInterface[cpPriv->id] = cpPriv->serviceInterface.name();
                cpPriv->bearer = bearerName(cpPriv->id);
            }
        } //end each connection service
    }
}

void QNmWifiEngine::accessPointConnections()
{
    QList<QDBusObjectPath> list = iface->getDevices();
    QScopedPointer<QNetworkManagerInterfaceDevice> devIface;
    foreach(QDBusObjectPath path, list) {
        devIface.reset(new QNetworkManagerInterfaceDevice(path.path()));
        if(devIface->deviceType() == DEVICE_TYPE_802_11_WIRELESS) {
            QList<QString> apList = availableAccessPoints.uniqueKeys();

            QList<QString>::const_iterator i;
            for (i = apList.constBegin(); i != apList.constEnd(); ++i) {
                QNetworkConfigurationPrivate* cpPriv;
                cpPriv = addAccessPoint( devIface->connectionInterface()->path(), availableAccessPoints[*i]);
                if(cpPriv->isValid) {
                    foundConfigurations.append(cpPriv);
                    //qWarning() << "adding" << cpPriv->name << "to things";
                    configurationInterface[cpPriv->id] = cpPriv->serviceInterface.name();
                    cpPriv->bearer = bearerName(cpPriv->id);
                }
            }
        }
    }
}

QString QNmWifiEngine::getInterfaceFromId(const QString &id)
{
    return configurationInterface.value(id);
}

bool QNmWifiEngine::hasIdentifier(const QString &id)
{
    if (configurationInterface.contains(id))
        return true;
    foreach (QNetworkConfigurationPrivate *cpPriv, getConfigurations()) {
        if (cpPriv->id == id)
            return true;
    }
    return false;
}

QString QNmWifiEngine::bearerName(const QString &id)
{
    QString interface = getInterfaceFromId(id);

    QScopedPointer<QNetworkManagerInterfaceDevice> devIface;
    QList<QDBusObjectPath> list = iface->getDevices();
    foreach(QDBusObjectPath path, list) {
        devIface.reset(new QNetworkManagerInterfaceDevice(path.path()));
        if(interface == devIface->interface().name()) {
            switch(devIface->deviceType()) {
                case DEVICE_TYPE_802_3_ETHERNET/*NM_DEVICE_TYPE_ETHERNET*/:
                    return QLatin1String("Ethernet");
                    break;
                case DEVICE_TYPE_802_11_WIRELESS/*NM_DEVICE_TYPE_WIFI*/:
                    return QLatin1String("WLAN");
                    break;
                case DEVICE_TYPE_GSM/*NM_DEVICE_TYPE_GSM*/:
                    return QLatin1String("2G");
                    break;
                case DEVICE_TYPE_CDMA/*NM_DEVICE_TYPE_CDMA*/:
                    return QLatin1String("CDMA2000");
                    break;
                default:
                    break;
            }
        }
    }
    return QLatin1String("Unknown");
}

void QNmWifiEngine::connectToId(const QString &id)
{
    activatingConnectionPath = id;
    QStringList connectionSettings = getConnectionPathForId(id);
    if(connectionSettings.isEmpty()) {
        emit connectionError(id, OperationNotSupported);
        return;
    }

    QDBusObjectPath connectionPath(connectionSettings.at(1));
    QString interface = getInterfaceFromId(id);

    interface = QNetworkInterface::interfaceFromName(interface).hardwareAddress().toLower();
    QString devPath;
    devPath = deviceConnectionPath(interface);
    QDBusObjectPath devicePath(devPath);

    iface->activateConnection(
            connectionSettings.at(0),
            connectionPath,
            devicePath,
            connectionPath);
}

void QNmWifiEngine::disconnectFromId(const QString &id)
{
    QString activeConnectionPath = getActiveConnectionPath(id);

    if (!activeConnectionPath.isEmpty()) {
        QScopedPointer<QNetworkManagerConnectionActive> activeCon;
        activeCon.reset(new QNetworkManagerConnectionActive(activeConnectionPath));
        QScopedPointer<QNetworkManagerSettingsConnection> settingsCon;
        settingsCon.reset(new QNetworkManagerSettingsConnection(activeCon->serviceName(), activeCon->connection().path()));

        if(settingsCon->isAutoConnect()) {
//            qWarning() << id << "is autoconnect";
            emit connectionError(id, OperationNotSupported);
            //unsupported
        } else {
//            qWarning() <<id << "is NOT autoconnect";
            QDBusObjectPath dbpath(activeConnectionPath);
            iface->deactivateConnection(dbpath);
            activatingConnectionPath = "";
        }
    }
}

void QNmWifiEngine::requestUpdate()
{
    updated = false;
    knownSsids.clear();
    availableAccessPoints.clear();
    emitConfigurationsChanged();
}

QNmWifiEngine *QNmWifiEngine::instance()
{
    QDBusConnection dbusConnection = QDBusConnection::systemBus();
    if (dbusConnection.isConnected()) {
        QDBusConnectionInterface *dbiface = dbusConnection.interface();
        QDBusReply<bool> reply = dbiface->isServiceRegistered("org.freedesktop.NetworkManager");
        if (reply.isValid() && reply.value())
            return nmWifiEngine();
    }

    return 0;
}

void  QNmWifiEngine::getKnownSsids()
{
    QStringList connectionServices;
    connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
    connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;

    QScopedPointer<QNetworkManagerSettings> settingsiface;
    foreach (QString service, connectionServices) {
        settingsiface.reset(new QNetworkManagerSettings(service));
        QList<QDBusObjectPath> list = settingsiface->listConnections();
        foreach(QDBusObjectPath path, list) {
            QNetworkManagerSettingsConnection sysIface(service, path.path());
            knownSsids << sysIface.getSsid();
        }
    }
}

void QNmWifiEngine::getActiveConnectionsPaths()
{
//    qWarning() << Q_FUNC_INFO;
    QScopedPointer<QNetworkManagerInterface> dbIface;
    activeConnectionPaths.clear();
    dbIface.reset(new QNetworkManagerInterface);
    QList <QDBusObjectPath> connections = dbIface->activeConnections();

    foreach(QDBusObjectPath conpath, connections) {
        activeConnectionPaths << conpath.path();
//        qWarning() << __FUNCTION__ <<  conpath.path() << activeConnectionPaths.count();

        //QNetworkManagerConnectionActive *activeConn;
        //activeConn = new QNetworkManagerConnectionActive(conpath.path(), this);

//        qWarning() << activeConn->connection().path() /*<< activeConn->specificObject().path() */<< activeConn->devices()[0].path();

    }
}

QNetworkConfigurationPrivate * QNmWifiEngine::addAccessPoint( const QString &iPath, QDBusObjectPath path)
{

    QScopedPointer<QNetworkManagerInterfaceDevice> devIface(new QNetworkManagerInterfaceDevice(iPath));
    QScopedPointer<QNetworkManagerInterfaceDeviceWireless> devWirelessIface(new QNetworkManagerInterfaceDeviceWireless(iPath));

    QString activeAPPath = devWirelessIface->activeAccessPoint().path();

    QScopedPointer<QNetworkManagerInterfaceAccessPoint> accessPointIface(new QNetworkManagerInterfaceAccessPoint(path.path()));

    QString ident = accessPointIface->connectionInterface()->path();
    quint32 nmState = devIface->state();

    QString ssid = accessPointIface->ssid();
    QString hwAddy = accessPointIface->hwAddress();
    QString sInterface = devIface->interface().name();

    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
    bool addIt = true;
    //qWarning() << availableAccessPoints.count() << ssid;

//    if(availableAccessPoints.contains(ssid)) {
//            addIt = false;
//
//    }
//    foreach (QNetworkConfigurationPrivate *cpPriv, foundConfigurations) {
//        if (cpPriv->name == ssid) { //weed out duplicate ssid's ??
//            addIt = false;
//            break;
//        }
//    }

    if(addIt) {

        cpPriv->name = ssid;
        cpPriv->isValid = true;
        cpPriv->id = ident;
        cpPriv->internet = true;
        cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
        cpPriv->serviceInterface = devIface->interface();

        cpPriv->state = getAPState(nmState, knownSsids.contains(cpPriv->name));

        if(activeAPPath == accessPointIface->connectionInterface()->path()) {
            cpPriv->state = ( cpPriv->state | QNetworkConfiguration::Active);
        }
        if(accessPointIface->flags() == NM_802_11_AP_FLAGS_PRIVACY)
            cpPriv->purpose = QNetworkConfiguration::PrivatePurpose;
        else
            cpPriv->purpose = QNetworkConfiguration::PublicPurpose;
        return cpPriv;
    } else {
        cpPriv->isValid = false;
    }
    return cpPriv;
}


 QNetworkConfiguration::StateFlags QNmWifiEngine::getAPState(qint32 nmState, bool isKnown)
{
    QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Undefined;
    //qWarning() << nmState << knownSsids;
    // this is the state of the wifi device interface
    if(isKnown)
        state = ( QNetworkConfiguration::Defined);

    switch(nmState) { //device interface state, not AP state
    case  NM_DEVICE_STATE_UNKNOWN:
    case  NM_DEVICE_STATE_UNMANAGED:
    case NM_DEVICE_STATE_UNAVAILABLE:
        state = (QNetworkConfiguration::Undefined);
        break;
    case NM_DEVICE_STATE_DISCONNECTED:
        {
            if(isKnown)
                state = ( state  | QNetworkConfiguration::Discovered);
        }
        break;
    case NM_DEVICE_STATE_PREPARE:
    case NM_DEVICE_STATE_CONFIG:
    case NM_DEVICE_STATE_NEED_AUTH:
    case NM_DEVICE_STATE_IP_CONFIG:
        if(isKnown)
            state = ( state | QNetworkConfiguration::Discovered);
        break;
    case NM_DEVICE_STATE_ACTIVATED:
        {
            if(isKnown)
                state = ( state | QNetworkConfiguration::Discovered);
        }
        break;
    };
    return state;
}

QString QNmWifiEngine::getActiveConnectionPath(const QString &id)
{
    QStringList connectionSettings = getConnectionPathForId(id);
    QNetworkManagerInterface ifaceD;
    QList<QDBusObjectPath> connections = ifaceD.activeConnections();
    QScopedPointer<QNetworkManagerConnectionActive> conDetailsD;
    foreach(QDBusObjectPath path, connections) {
        conDetailsD.reset(new QNetworkManagerConnectionActive( path.path()));
        if(conDetailsD->connection().path() == connectionSettings.at(1)
            && conDetailsD->serviceName() == connectionSettings.at(0))
            return path.path();
    }
    return QString();
}

 QNetworkConfiguration::StateFlags QNmWifiEngine::getStateFlag(quint32 nmstate)
 {
//     qWarning() << Q_FUNC_INFO << nmstate;
     QNetworkConfiguration::StateFlags flag;
     switch (nmstate) {
     case  NM_DEVICE_STATE_UNKNOWN:
     case NM_DEVICE_STATE_FAILED:
     case NM_DEVICE_STATE_UNMANAGED:
         flag = (QNetworkConfiguration::Undefined);
         break;
     case NM_DEVICE_STATE_PREPARE:
     case NM_DEVICE_STATE_CONFIG:
     case NM_DEVICE_STATE_NEED_AUTH:
     case NM_DEVICE_STATE_IP_CONFIG:
     case  NM_DEVICE_STATE_UNAVAILABLE:
         flag = (QNetworkConfiguration::Defined);
         break;
     case NM_DEVICE_STATE_DISCONNECTED:
         flag = ( flag | QNetworkConfiguration::Discovered );
        break;
     case NM_DEVICE_STATE_ACTIVATED:
         {
             flag = ( flag | QNetworkConfiguration::Discovered
                      | QNetworkConfiguration::Active );
         }
         break;
     default:
         flag = ( QNetworkConfiguration::Defined);
         break;
     };
     return flag;
 }

void QNmWifiEngine::updateDeviceInterfaceState(const QString &/*path*/, quint32 nmState)
{
//    qWarning() << Q_FUNC_INFO << path << nmState;

    if(nmState == NM_DEVICE_STATE_ACTIVATED
       || nmState == NM_DEVICE_STATE_DISCONNECTED
       || nmState == NM_DEVICE_STATE_UNAVAILABLE
       || nmState == NM_DEVICE_STATE_FAILED) {

/*      InterfaceLookupError = 0,
        ConnectError,
        OperationNotSupported,
        DisconnectionError,
*/
//        qWarning() << Q_FUNC_INFO << ident;
        QNetworkConfiguration::StateFlags state = (QNetworkConfiguration::Defined);
        switch (nmState) {
                case  NM_DEVICE_STATE_UNKNOWN:
                case NM_DEVICE_STATE_FAILED:
            state = (QNetworkConfiguration::Undefined);
            emit connectionError(activatingConnectionPath, ConnectError);
            requestUpdate();
//                qWarning() << Q_FUNC_INFO;
            break;
                case  NM_DEVICE_STATE_UNAVAILABLE:
            state = (QNetworkConfiguration::Defined);
//            emit connectionError(activatingConnectionPath, ConnectError);
            requestUpdate();
            break;
                case NM_DEVICE_STATE_DISCONNECTED:
            state = ( state | QNetworkConfiguration::Discovered );
            requestUpdate();
            break;
                case NM_DEVICE_STATE_ACTIVATED:
            {
                state = ( state | QNetworkConfiguration::Discovered
                          | QNetworkConfiguration::Active );
                requestUpdate();
            }
            break;
                default:
            state = ( QNetworkConfiguration::Defined);
            break;
        };
    }
}

void QNmWifiEngine::addDevice(QDBusObjectPath path)
{
    //qWarning() << Q_FUNC_INFO << path.path();
    QNetworkManagerInterfaceDevice *devIface = new QNetworkManagerInterfaceDevice(path.path(), this);
    devIface->setConnections();
    connect(devIface,SIGNAL(stateChanged(const QString &, quint32)),
            this, SLOT(updateDeviceInterfaceState(const QString&, quint32)));

    if(!devicePaths.contains(path.path()))
        devicePaths << path.path();

    switch(devIface->deviceType()) {
    case DEVICE_TYPE_802_3_ETHERNET:
        {
            QNetworkManagerInterfaceDeviceWired * devWiredIface;
            devWiredIface = new QNetworkManagerInterfaceDeviceWired(devIface->connectionInterface()->path(), this);
            devWiredIface->setConnections();
            connect(devWiredIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                    this,SLOT(cmpPropertiesChanged( const QString &, QMap<QString,QVariant>)));
            requestUpdate();
        }
        break;
    case DEVICE_TYPE_802_11_WIRELESS:
        {
            QNetworkManagerInterfaceDeviceWireless *devWirelessIface;
            devWirelessIface = new QNetworkManagerInterfaceDeviceWireless(devIface->connectionInterface()->path(), this);
            devWirelessIface->setConnections();

            connect(devWirelessIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                    this,SLOT(cmpPropertiesChanged( const QString &, QMap<QString,QVariant>)));

            connect(devWirelessIface, SIGNAL(accessPointAdded(const QString &,QDBusObjectPath)),
                    this,SLOT(accessPointAdded(const QString &,QDBusObjectPath)));

            connect(devWirelessIface, SIGNAL(accessPointRemoved(const QString &,QDBusObjectPath)),
                    this,SLOT(accessPointRemoved(const QString &,QDBusObjectPath)));
            requestUpdate();

        }
        break;
            default:
        break;
    };
}

void QNmWifiEngine::removeDevice(QDBusObjectPath /*path*/)
{
//    qWarning() << Q_FUNC_INFO << path.path();
//    disconnect(devIface,SIGNAL(stateChanged(const QString &, quint32)),
//               this, SLOT(updateDeviceInterfaceState(const QString&, quint32)));
//
//    if(devIface->deviceType() == DEVICE_TYPE_802_11_WIRELESS) {
//        //        devWirelessIface = new QNetworkManagerInterfaceDeviceWireless(devIface->connectionInterface()->path());
//        //        devWirelessIface->setConnections();
//
//        disconnect(devWirelessIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
//                   this,SIGNAL(cmpPropertiesChanged( const QString &, QMap<QString,QVariant>)));
//
//        disconnect(devWirelessIface, SIGNAL(accessPointAdded(const QString &,QDBusObjectPath)),
//                   this,SIGNAL(accessPointAdded(const QString &,QDBusObjectPath)));
//
//        disconnect(devWirelessIface, SIGNAL(accessPointRemoved(const QString &,QDBusObjectPath)),
//                   this,SIGNAL(accessPointRemoved(const QString &,QDBusObjectPath)));
//
//    }
}
void QNmWifiEngine::cmpPropertiesChanged(const QString &path, QMap<QString,QVariant> map)
{
   QMapIterator<QString, QVariant> i(map);
   while (i.hasNext()) {
       i.next();
//       qWarning() << Q_FUNC_INFO << path <<  i.key() << i.value().toUInt();
       if( i.key() == "State") { //only applies to device interfaces
           updateDeviceInterfaceState(path, i.value().toUInt());
       }
       if( i.key() == "ActiveAccessPoint") {
       }
       if( i.key() == "Carrier") { //someone got plugged in
        //   requestUpdate();
       }
   }
}

void QNmWifiEngine::accessPointRemoved( const QString &aPath, QDBusObjectPath /*oPath*/)
{
    //qWarning() << Q_FUNC_INFO << aPath << oPath.path();

    if(aPath.contains("devices")) {
        requestUpdate();
    }
}

void QNmWifiEngine::accessPointAdded( const QString &aPath, QDBusObjectPath oPath)
{
   /*QNetworkConfigurationPrivate* cpPriv;
   cpPriv = addAccessPoint( aPath, oPath);*/
   requestUpdate();
}

QNetworkConfiguration::StateFlags QNmWifiEngine::getStateForId(const QString &id)
{
    bool isAvailable = false;
    QStringList conPath = getConnectionPathForId(id);
    QString aconpath = getActiveConnectionPath(id);

    if(!aconpath.isEmpty()) {
        //active connection
        QNetworkManagerConnectionActive aConn(aconpath);

        QList <QDBusObjectPath> devs = aConn.devices();

        QScopedPointer<QNetworkManagerInterfaceDevice> ifaceDevice;
        QScopedPointer<QNetworkManagerInterfaceDeviceWired> devWiredIface;
        foreach(QDBusObjectPath dev, devs) {
            //qWarning() << "foreach" << dev.path();
            ifaceDevice.reset(new QNetworkManagerInterfaceDevice(dev.path()));

            if(ifaceDevice->deviceType() == DEVICE_TYPE_802_3_ETHERNET) {

                if(isAddressOfConnection(id, ifaceDevice->ip4Address())) {
                    // this is it!
                    return getStateFlag(ifaceDevice->state());
                } else {
                    continue;
                }

                if(ifaceDevice->state() == NM_DEVICE_STATE_UNAVAILABLE ||
                   ifaceDevice->state() == NM_DEVICE_STATE_DISCONNECTED) {
                    isAvailable = true;

                    devWiredIface.reset(new QNetworkManagerInterfaceDeviceWired(ifaceDevice->connectionInterface()->path()));
                    if(!devWiredIface->carrier())
                        return QNetworkConfiguration::Defined;
                } //end eth
            } else if(ifaceDevice->deviceType() == DEVICE_TYPE_802_11_WIRELESS) {
                qWarning() << "FIXME!!!!!!!!!!!!!!!!!";
            }

            return getStateFlag(ifaceDevice->state());
        }
    } else {
        // not active
        QScopedPointer<QNetworkManagerSettingsConnection> sysIface;
        sysIface.reset(new QNetworkManagerSettingsConnection(conPath.at(0),conPath.at(1)));
        if(sysIface->isValid()) {
            if(sysIface->getType() == DEVICE_TYPE_802_11_WIRELESS) {
                QString ssid = sysIface->getSsid();
                bool ok = false;

                if(knownSsids.contains(ssid, Qt::CaseSensitive)) {
                    foreach(QString onessid, knownSsids) {
                        // qWarning() << ssid << onessid;
                        if(onessid == ssid && availableAccessPoints.contains(ssid)) {
                            // qWarning() <<ssid  << "In known and available SSIDS";
                            ok = true;
                            break;
                        }
                    }
                }   
                if(ok)
                    return getStateFlag(NM_DEVICE_STATE_DISCONNECTED);
                else
                    return getStateFlag(NM_DEVICE_STATE_UNAVAILABLE);
            }
        }
    }

    return QNetworkConfiguration::Defined; //not active, but we know this connection so just fake it
}

bool QNmWifiEngine::isAddressOfConnection(const QString &id, quint32 ipaddress)
{
    QStringList conPath = getConnectionPathForId(id);
    QString aConPath = getActiveConnectionPath(id);
    if(aConPath.isEmpty()) {
        // not active
        return false;
    }

    QScopedPointer<QNetworkManagerConnectionActive> aConn;
    aConn.reset(new QNetworkManagerConnectionActive(aConPath));
    QScopedPointer<QNetworkManagerInterfaceDevice> ifaceDevice;
    QList<QDBusObjectPath> devices = aConn->devices();
    foreach(QDBusObjectPath device, devices) {
        ifaceDevice.reset(new QNetworkManagerInterfaceDevice(device.path()));
        if(ifaceDevice->ip4Address() == ipaddress) {
            return true;
        }
    }
    return false;
}

QNetworkInterface QNmWifiEngine::getBestInterface( quint32 type, const QString &id)
{
    // check active connections first.
    QStringList conIdPath = getConnectionPathForId(id);

    QNetworkInterface interface;
    QScopedPointer<QNetworkManagerConnectionActive> aConn;
    foreach(QString conpath, activeConnectionPaths) {
        aConn.reset(new QNetworkManagerConnectionActive(conpath));

        if(aConn->connection().path() == conIdPath.at(1)
            && aConn->serviceName() == conIdPath.at(0)) {

            QList <QDBusObjectPath> devs = aConn->devices();
            QNetworkManagerInterfaceDevice ifaceDevice(devs[0].path()); //just take the first one
            return ifaceDevice.interface();
        }
    }

    //try guessing
    QList<QDBusObjectPath> list = iface->getDevices();
    QScopedPointer<QNetworkManagerInterfaceDevice> devIface;
    foreach(QDBusObjectPath path, list) {
        devIface.reset(new QNetworkManagerInterfaceDevice(path.path()));
        if(devIface->deviceType() == type /*&& devIface->managed()*/) {
            if(devIface->state() == NM_STATE_DISCONNECTED) {
                return devIface->interface();
            }
        }
    }
    return QNetworkInterface();
}

quint64 QNmWifiEngine::receivedDataForId(const QString &id) const
{
    if(configurationInterface.count() > 1)
        return 0;
    quint64 result = 0;

    QString devFile;
    devFile =  configurationInterface.value(id);
    QFile rx("/sys/class/net/"+devFile+"/statistics/rx_bytes");
    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&rx);
        in >> result;
        rx.close();
    }
    return result;
}

quint64 QNmWifiEngine::sentDataForId(const QString &id) const
{
    if(configurationInterface.count() > 1)
        return 0;
    quint64 result = 0;
    QString devFile;
    devFile =  configurationInterface.value(id);

    QFile tx("/sys/class/net/"+devFile+"/statistics/tx_bytes");
    if(tx.exists() && tx.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&tx);
        in >> result;
        tx.close();
    }
    return result;
}

void QNmWifiEngine::newConnection(QDBusObjectPath /*path*/)
{
    //qWarning() << Q_FUNC_INFO;
    requestUpdate();
}

void QNmWifiEngine::settingsConnectionRemoved(const QString &/*path*/)
{
    //qWarning() << Q_FUNC_INFO;
    requestUpdate();
}

void QNmWifiEngine::slotActivationFinished(QDBusPendingCallWatcher *openCall)
{
    QDBusPendingReply<QDBusObjectPath> reply = *openCall;
    if (reply.isError()) {
        qWarning() <<"Error" <<  reply.error().name() << reply.error().message()
                <<activatingConnectionPath;
        emit connectionError(activatingConnectionPath, ConnectError);
    } /*else {
        QDBusObjectPath result = reply.value();
        //qWarning() << result.path();
    }*/
}

void QNmWifiEngine::scanForAccessPoints()
{
    availableAccessPoints.clear();
    QList<QDBusObjectPath> list = iface->getDevices();

    QScopedPointer<QNetworkManagerInterfaceDevice> devIface;
    QScopedPointer<QNetworkManagerInterfaceDeviceWireless> devWirelessIface;
    QScopedPointer<QNetworkManagerInterfaceAccessPoint> accessPointIface;
    foreach(QDBusObjectPath path, list) {
        devIface.reset(new QNetworkManagerInterfaceDevice(path.path()));

        if(devIface->deviceType() == DEVICE_TYPE_802_11_WIRELESS) {

            devWirelessIface.reset(new QNetworkManagerInterfaceDeviceWireless(devIface->connectionInterface()->path()));
            ////////////// AccessPoints
            QList<QDBusObjectPath> apList = devWirelessIface->getAccessPoints();

            foreach(QDBusObjectPath path, apList) {
                accessPointIface.reset(new QNetworkManagerInterfaceAccessPoint(path.path()));
                QString ssid = accessPointIface->ssid();
                availableAccessPoints.insert(ssid, path);
            }
        }
    }
}

QString QNmWifiEngine::deviceConnectionPath(const QString &mac)
{
//    qWarning() << __FUNCTION__ << mac;
    QString newMac = mac;
    newMac = newMac.replace(":","_").toLower();
    //device object path might not contain just mac address
    //might contain extra numbers on the end. thanks HAL
    foreach(QString device, devicePaths) {
        if(device.contains(newMac)) {
            newMac = device;
            break;
        }
    }
    return newMac;
}

QStringList QNmWifiEngine::getConnectionPathForId(const QString &uuid)
{
    QStringList connectionServices;
    connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
    connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;
    QScopedPointer<QNetworkManagerSettings> settingsiface;
    foreach (QString service, connectionServices) {
        settingsiface.reset(new QNetworkManagerSettings(service));
        QList<QDBusObjectPath> list = settingsiface->listConnections();
        QScopedPointer<QNetworkManagerSettingsConnection> sysIface;
        foreach(QDBusObjectPath path, list) {
            sysIface.reset(new QNetworkManagerSettingsConnection(service, path.path()));
            if(sysIface->getUuid() == uuid) 
                return QStringList() << service << sysIface->connectionInterface()->path();
        }
    }
    return QStringList();
}

#include "moc_qnmwifiengine_unix_p.cpp"

QTM_END_NAMESPACE

