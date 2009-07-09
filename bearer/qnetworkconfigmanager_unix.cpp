/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qnetworkconfigmanager_p.h"

#include <QNetworkInterface>
#include <QTimer>

#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
#include <NetworkManager/NetworkManager.h>
#include <QtDBus>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <qnetworkmanagerservice_p.h>
#endif
#include <arpa/inet.h>

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    static QDBusConnection dbusConnection = QDBusConnection::systemBus();
    static QDBusInterface iface(NM_DBUS_SERVICE, NM_DBUS_PATH, NM_DBUS_INTERFACE, dbusConnection);
#endif

static bool NetworkManagerAvailable()
{
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    QDBusConnection dbusConnection = QDBusConnection::systemBus();
    if (dbusConnection.isConnected()) {
        QDBusConnectionInterface *dbiface = dbusConnection.interface();
        QDBusReply<bool> reply = dbiface->isServiceRegistered("org.freedesktop.NetworkManager");
        if (reply.isValid())
            return reply.value();
    }
#endif
    return false;
}

QString ifaceToId( const QNetworkInterface& netiface)
{
    int idx = netiface.index();
    if (idx) {
        return QString::number(qHash(idx));
    } else {
        return QString::number(qHash(netiface.hardwareAddress()));
    }
}

void QNetworkConfigurationManagerPrivate::registerPlatformCapabilities()
{
    if (NetworkManagerAvailable())
        capFlags = QNetworkConfigurationManager::BearerManagement
                    | QNetworkConfigurationManager::ForcedRoaming
                    | QNetworkConfigurationManager::DataStatistics
                    /*| QNetworkConfigurationManager::SystemSessionSupport*/;
#ifdef Q_OS_LINUX
    //see SO_BINDTODEVICE
//    capFlags |= QNetworkConfigurationManager::DirectConnectionRouting;
#endif


}

QTimer* updateTimer = 0;

void QNetworkConfigurationManagerPrivate::updateConfigurations()
{

    QList<QString> knownConfigs = accessPointConfigurations.keys();
    if (!NetworkManagerAvailable()) {
        QList<QNetworkInterface> systemIfaces = QNetworkInterface::allInterfaces();
        foreach (QNetworkInterface ii, systemIfaces) {
            if (!ii.isValid())
                continue;
            QString ident = ifaceToId(ii);
            if (accessPointConfigurations.contains(ident)) {
                knownConfigs.removeOne(ident);
            }
            QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
            QString hrs = ii.humanReadableName();
            cpPriv->name = hrs.isEmpty() ? ii.name() : hrs;
            cpPriv->isValid = true;
            cpPriv->id = ident;
            cpPriv->state = (QNetworkConfiguration::Discovered);
            cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
            if (ii.flags() & QNetworkInterface::IsUp)
                cpPriv->state = (cpPriv->state | QNetworkConfiguration::Active );
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
            accessPointConfigurations.insert(ident, ptr);
            if (!firstUpdate) {
                QNetworkConfiguration item;
                item.d = ptr;
                emit configurationAdded(item);
            }
        }

        foreach(QString oldIface, knownConfigs) {
            //remove non existing ifaces
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(oldIface);
            priv->isValid = false;
            if (!firstUpdate) {
                QNetworkConfiguration item;
                item.d = priv;
                emit configurationRemoved(item);
            }
            //if we would have SNAP support we would have to remove the references
            //from existing ServiceNetworks to the removed access point configuration
        }

        if (!updateTimer) {
            //QNetworkInterface doesn't provide notifications if something changes
            //we have to poll
            updateTimer = new QTimer(this);
            connect(updateTimer, SIGNAL(timeout()), this, SLOT(performAsyncConfigurationUpdate()));
            updateTimer->start(1000);
        }

    } else {
        // netman
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)

        updating = true;
        knownSsids = getKnownSsids();
        iface = new QNetworkManagerInterface();
        QList<QDBusObjectPath> list = iface->getDevices();
        foreach(QDBusObjectPath path, list) {

            devIface = new QNetworkManagerInterfaceDevice(path.path());
            devIface->setConnections();

            connect(devIface,SIGNAL(stateChanged(const QString &, quint32)),
                    this, SLOT(updateDeviceInterfaceState(const QString&, quint32)));

            switch (devIface->deviceType()) {
                    case DEVICE_TYPE_802_3_ETHERNET:
                updateEthConfigurations(devIface);
                break;
                    case DEVICE_TYPE_802_11_WIRELESS:
                updateWifiConfigurations(devIface);
                break;
                    case DEVICE_TYPE_GSM:
                    case DEVICE_TYPE_CDMA:
                // TODO
                break;
            };
            updateServiceNetworks(devIface);
        }
        updating = false;
#endif
    } //end netman
    emit configurationUpdateComplete();
    //   QTimer::singleShot(0, this, SIGNAL( configurationUpdateComplete()));

    if (firstUpdate)
        firstUpdate = false;
}

QNetworkConfiguration QNetworkConfigurationManagerPrivate::defaultConfiguration()
{
    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> firstActive;
    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> firstDiscovered;

    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> >::const_iterator i =
        accessPointConfigurations.constBegin();
    while (i != accessPointConfigurations.constEnd()) {
        QNetworkConfigurationPrivate *priv = i.value().data();
        if (!firstActive && priv->isValid &&
            (priv->state & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            firstActive = i.value();
        }
        if (!firstDiscovered && priv->isValid &&
            (priv->state & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered) {
            firstDiscovered = i.value();
        }
        ++i;
    }

    QNetworkConfiguration item;

    if (firstActive)
        item.d = firstActive;
    else if (firstDiscovered)
        item.d = firstDiscovered;
    return item;
}

void QNetworkConfigurationManagerPrivate::performAsyncConfigurationUpdate()
{
    //TODO
    updateConfigurations();
}

#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
void QNetworkConfigurationManagerPrivate::updateEthConfigurations(QNetworkManagerInterfaceDevice *devIface)
{
    QList<QString> knownConfigs = accessPointConfigurations.keys();

    QString ident;
    QNetworkManagerInterfaceDeviceWired *devWiredIface;
    devWiredIface = new QNetworkManagerInterfaceDeviceWired(devIface->connectionInterface()->path());
        ident = devWiredIface->hwAddress();

        if (!knownConfigs.contains(ident)) {
            QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
            cpPriv->name = getNameForConfiguration(devIface);
            cpPriv->isValid = true;
            cpPriv->id = ident;
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
            cpPriv->purpose = QNetworkConfiguration::Public;

            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
            accessPointConfigurations.insert(ident, ptr);
            if (!firstUpdate) {
                QNetworkConfiguration item;
                item.d = ptr;
                emit configurationAdded(item);
            }
   if (!firstUpdate && !updating) {
        emit configurationUpdateComplete();
        }
}
}

void QNetworkConfigurationManagerPrivate::updateWifiConfigurations(QNetworkManagerInterfaceDevice *devIface)
{
    QString ident;
    QList<QString> knownConfigs = accessPointConfigurations.keys();

    devWirelessIface = new QNetworkManagerInterfaceDeviceWireless(devIface->connectionInterface()->path());
    if(firstUpdate) { //only connect once
        devWirelessIface->setConnections();
        connect(devWirelessIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                this,SLOT(cmpPropertiesChanged( const QString &, QMap<QString,QVariant>)));
        connect(devWirelessIface, SIGNAL(accessPointAdded(const QString &,QDBusObjectPath)),
                this,SLOT(accessPointAdded(const QString &,QDBusObjectPath)));

        connect(devWirelessIface, SIGNAL(accessPointRemoved(const QString &,QDBusObjectPath)),
                this,SLOT(accessPointRemoved(const QString &,QDBusObjectPath)));
    }
    // get the wifi interface state first.. do we need this?
    QString activeAPPath = devWirelessIface->activeAccessPoint().path();
    ident = devWirelessIface->hwAddress();

#if 0
    if (!knownConfigs.contains(ident)) {
        QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
        cpPriv->name = getNameForConfiguration(devIface);
        cpPriv->isValid = true;
        cpPriv->id = ident;
        cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
        cpPriv->serviceInterface = devIface->interface();
        if(activeAPPath.length() > 2) {
            cpPriv->state = (cpPriv->state | QNetworkConfiguration::Defined
                             | QNetworkConfiguration::Discovered
                             | QNetworkConfiguration::Active );
        } else {
            switch (devIface->state()) {
            case  NM_DEVICE_STATE_UNKNOWN:
                cpPriv->state = (cpPriv->state | QNetworkConfiguration::Undefined);
                break;
            case  NM_DEVICE_STATE_UNAVAILABLE:
                cpPriv->state = (cpPriv->state | QNetworkConfiguration::Defined);
                break;
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
        }

        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
        accessPointConfigurations.insert(ident, ptr);
        if (!firstUpdate) {
            QNetworkConfiguration item;
            item.d = ptr;
            emit configurationAdded(item);
        }
    }
#endif
    
    QList<QDBusObjectPath> list = devWirelessIface->getAccessPoints();
    foreach(QDBusObjectPath path, list) {
        ////////////// AccessPoints
        accessPointAdded( devIface->connectionInterface()->path(), path);
    }
    updateServiceNetworks(devIface);
    if (!firstUpdate&& !updating) {
        emit configurationUpdateComplete();
        }
}

QString QNetworkConfigurationManagerPrivate::getNameForConfiguration(QNetworkManagerInterfaceDevice *devIface)
{
    QString newname;
    if (devIface->state() == NM_DEVICE_STATE_ACTIVATED) {
        QString path = devIface->ip4config().path();
        QNetworkManagerIp4Config * ipIface;
        ipIface = new QNetworkManagerIp4Config(path);
        newname = ipIface->domains().join(" ");
    }
    //fallback to interface name
    if(newname.isEmpty())
        newname = devIface->interface().name();
    return newname;
}

void QNetworkConfigurationManagerPrivate::updateServiceNetworks(QNetworkManagerInterfaceDevice *devIface)
{
    QVariant vs = devIface->state();

    if (!snapConfigurations.contains("Internet Service Network")) {
        QNetworkConfigurationPrivate* spPriv = new QNetworkConfigurationPrivate();
        spPriv->name = "Internet";
        spPriv->isValid = true;
        spPriv->id = "Internet Service Network";
        spPriv->state =  QNetworkConfiguration::Defined;
        spPriv->serviceInterface = devIface->interface();
        spPriv->type = QNetworkConfiguration::ServiceNetwork;

        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> sptr(spPriv);
        snapConfigurations.insert(spPriv->id, sptr);
        if (!firstUpdate) {
            QNetworkConfiguration item;
            item.d = sptr;
            emit configurationAdded(item);
        }
    }
    updateServiceNetworkState(false);
    bool isWifi = false;

    switch (devIface->deviceType()) {
    case DEVICE_TYPE_802_3_ETHERNET:
        break;
    case DEVICE_TYPE_802_11_WIRELESS:
        isWifi = true;
        break;
    case DEVICE_TYPE_GSM:
    case DEVICE_TYPE_CDMA:
        // TODO
        break;
    };

    if(isWifi) {
        if (!snapConfigurations.contains("Wireless Service Network")) {
            QNetworkConfigurationPrivate* wspPriv = new QNetworkConfigurationPrivate();
            wspPriv->name = "Wireless";
            wspPriv->isValid = true;
            wspPriv->id = "Wireless Service Network";
            wspPriv->state =  QNetworkConfiguration::Defined;
            wspPriv->serviceInterface = devIface->interface();
            wspPriv->type = QNetworkConfiguration::ServiceNetwork;

            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> wsptr(wspPriv);
            snapConfigurations.insert(wspPriv->id, wsptr);
            if (!firstUpdate) {
                QNetworkConfiguration item;
                item.d = wsptr;
                emit configurationAdded(item);
            }
        }
        updateServiceNetworkState(isWifi);
    }
}

void QNetworkConfigurationManagerPrivate::updateServiceNetworkState(bool isWifi)
{
    QList<QString> knownConfigs = accessPointConfigurations.keys();
    if(!isWifi) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr =
                snapConfigurations.value(QLatin1String("Internet Service Network"));

        ptr.data()->serviceNetworkMembers.clear();

        QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> >::const_iterator i =
                accessPointConfigurations.constBegin();

        QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Defined;

        while (i != accessPointConfigurations.constEnd()) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> child = i.value();
            if (!knownConfigs.contains(child.data()->name)
                && ((child.data()->state & QNetworkConfiguration::Defined)
                    == QNetworkConfiguration::Defined)) {
                ptr.data()->serviceNetworkMembers.append(child);
                state |= child.data()->state;
            }
            ++i;
        }
        ptr.data()->state = state;
        if (!firstUpdate ) {
            QNetworkConfiguration item;
            item.d = ptr;
            emit configurationChanged(item);
        }

    } else {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> wptr =
                snapConfigurations.value(QLatin1String("Wireless Service Network"));

        wptr.data()->serviceNetworkMembers.clear();

        QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> >::const_iterator wi =
                accessPointConfigurations.constBegin();

        QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Defined;

        while (wi != accessPointConfigurations.constEnd()) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> child = wi.value();
            if (!knownConfigs.contains(child.data()->name)
                && ((child.data()->state & QNetworkConfiguration::Defined)
                    == QNetworkConfiguration::Defined)
                && child.data()->id.contains("AccessPoint")) {
                wptr.data()->serviceNetworkMembers.append(child);
                state |= child.data()->state;
            }
            ++wi;
        }
        wptr.data()->state = state;
        if (!firstUpdate ) {
            QNetworkConfiguration item;
            item.d = wptr;
            emit configurationChanged(item);
        }
    }

   if (!firstUpdate && !updating) {
       emit configurationUpdateComplete();
   }
}


QStringList QNetworkConfigurationManagerPrivate::getActiveConnectionsPaths(QDBusInterface &dbiface)
{
    QStringList aCons;
    QVariant prop = dbiface.property("ActiveConnections");
    QList<QDBusObjectPath> connections = prop.value<QList<QDBusObjectPath> >();
    foreach(QDBusObjectPath conpath, connections) {
        QDBusInterface conDetails(NM_DBUS_SERVICE,
                                  conpath.path(),
                                  NM_DBUS_INTERFACE_ACTIVE_CONNECTION,
                                  dbusConnection);
        if (conDetails.isValid()) {
            aCons << conpath.path();
        }
    }
    return aCons;
}

void QNetworkConfigurationManagerPrivate::accessPointAdded( const QString &iPath, QDBusObjectPath path)
{
    QNetworkManagerInterfaceDevice *devIface;
    devIface = new QNetworkManagerInterfaceDevice(iPath);
    QNetworkManagerInterfaceDeviceWireless *devWirelessIface;
    devWirelessIface = new QNetworkManagerInterfaceDeviceWireless(iPath);

    QList<QString> knownConfigs = accessPointConfigurations.keys();
    QString activeAPPath = devWirelessIface->activeAccessPoint().path();

    QNetworkManagerInterfaceAccessPoint *accessPointIface;
    accessPointIface = new QNetworkManagerInterfaceAccessPoint(path.path());

    QString ident = accessPointIface->connectionInterface()->path();
    quint32 nmState = devIface->state();

    QString ssid = accessPointIface->ssid();
    QString hwAddy = accessPointIface->hwAddress();
    QString sInterface = devIface->interface().name();

    if (!knownConfigs.contains(ident)) {
        QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
        cpPriv->name = ssid;
        cpPriv->isValid = true;
        cpPriv->id = ident;
        cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
        cpPriv->serviceInterface = devIface->interface();

        cpPriv->state = getAPState(nmState, knownSsids.contains(cpPriv->name));

        if(activeAPPath == accessPointIface->connectionInterface()->path()) {
            cpPriv->state = ( cpPriv->state | QNetworkConfiguration::Active);
        }
        if(accessPointIface->flags() == NM_802_11_AP_FLAGS_PRIVACY)
            cpPriv->purpose = QNetworkConfiguration::Private;
        else
            cpPriv->purpose = QNetworkConfiguration::Public;

        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
        accessPointConfigurations.insert(ident, ptr);
        if (!firstUpdate) {
            QNetworkConfiguration item;
            item.d = ptr;
            emit configurationAdded(item);
        } // end devWirelessIface
    } else {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv
                = accessPointConfigurations.value(ident);
        QNetworkConfiguration::StateFlags oldState = priv->state;

        if(activeAPPath == accessPointIface->connectionInterface()->path()) {
            oldState = ( priv->state | QNetworkConfiguration::Active);
        } else {
            QNetworkConfiguration::StateFlags newState = getAPState(nmState, knownSsids.contains(priv->name));
            if(oldState != newState) {
                priv->state = newState;
                    QNetworkConfiguration item;
                    item.d = priv;
                    emit configurationChanged(item);
                }
        }
    }

    if(!updating) {
        emit configurationUpdateComplete();
    }
}

void QNetworkConfigurationManagerPrivate::accessPointRemoved( const QString &aPath, QDBusObjectPath oPath)
{
    if(aPath.contains("devices")) {
        if (accessPointConfigurations.contains(oPath.path())) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv
                    = accessPointConfigurations.take(oPath.path());
            priv->isValid = false;
            QNetworkConfiguration item;
            item.d = priv;
            emit configurationRemoved(item);
            if(!firstUpdate && !updating) {
                emit configurationUpdateComplete();
            }
        } else {
        }
    }
    updating = false;
}

QNetworkConfiguration::StateFlags QNetworkConfigurationManagerPrivate::getAPState(qint32 nmState, bool isKnown)
{
    QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Undefined;

    // this is the state of the wifi device interface
    if(isKnown)
        state = ( QNetworkConfiguration::Defined);

    switch(nmState) { //device interface state, not AP state
    case  NM_DEVICE_STATE_UNKNOWN:
    case  NM_DEVICE_STATE_UNMANAGED:
        state = (QNetworkConfiguration::Undefined);
        break;
    case NM_DEVICE_STATE_UNAVAILABLE:
    case NM_DEVICE_STATE_DISCONNECTED:
        {
            if(isKnown)
                state = ( state | QNetworkConfiguration::Discovered);
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

void QNetworkConfigurationManagerPrivate::cmpPropertiesChanged( const QString & path, QMap<QString,QVariant> map)
{
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();

        if( i.key() == "State") { //only applies to device interfaces
            updateAccessPointState(path, i.value().toUInt());
            QNetworkManagerInterfaceDevice *devIface;
            devIface = new QNetworkManagerInterfaceDevice(path);
            updateServiceNetworkState(devIface->deviceType());
        }
        else if( i.key() == "ActiveAccessPoint") {
            if(i.value().value<QDBusObjectPath>().path().length() > 2)
               updateConfigurations();
        }
    }
}

void QNetworkConfigurationManagerPrivate::updateAccessPointState(const QString &deviceInterfacePath, quint32 state)
{
    bool updated = false;
    switch(state) {
    case NM_DEVICE_STATE_ACTIVATED:
        {
            QNetworkManagerInterfaceDeviceWireless *devWirelessIface;
            devWirelessIface = new QNetworkManagerInterfaceDeviceWireless(deviceInterfacePath);
            QString activeAPPath = devWirelessIface->activeAccessPoint().path();
//            QNetworkManagerInterfaceAccessPoint *accessPointIface;
//            accessPointIface = new QNetworkManagerInterfaceAccessPoint(activeAPPath);
            updateState(activeAPPath, state);
            currentActiveAP = activeAPPath;
            updated = true;
        }
        break;
    case NM_DEVICE_STATE_DISCONNECTED:
    case NM_DEVICE_STATE_UNAVAILABLE:
    case NM_DEVICE_STATE_FAILED:
        {
            updateDeviceInterfaceState(deviceInterfacePath, state); //update device interface state just for good measure
            // QNetworkManagerInterfaceDeviceWireless *devWirelessIface;
            // devWirelessIface = new QNetworkManagerInterfaceDeviceWireless(deviceInterfacePath);
            if(!currentActiveAP.isEmpty() ){
                updateState(currentActiveAP, state);
                updated = true;
                currentActiveAP = "";
            } else {
                updateConfigurations();
            }
            currentActiveAP = "";
        }
        break;
    };
    if(!updating && updated) {
        emit configurationUpdateComplete();
    }
}

void QNetworkConfigurationManagerPrivate::updateDeviceInterfaceState(const QString &path, quint32 nmState)
{
    QString ident = path;
    ident.remove("/org/freedesktop/Hal/devices/net_");
    ident.replace("_",":");
    ident = ident.toUpper();
    updateState(ident, nmState);

    if(!updating) {
        emit configurationUpdateComplete();
    }
}

void QNetworkConfigurationManagerPrivate::updateState(const QString &ident, quint32 nmState)
{
    if(nmState == NM_DEVICE_STATE_ACTIVATED
       || nmState == NM_DEVICE_STATE_DISCONNECTED
       || nmState == NM_DEVICE_STATE_UNAVAILABLE
       || nmState == NM_DEVICE_STATE_FAILED) {

        QList<QString> knownConfigs = accessPointConfigurations.keys();
        if (knownConfigs.contains(ident)) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv
                    = accessPointConfigurations.take(ident);
            QNetworkConfiguration::StateFlags oldState = priv->state;

            priv->state = (QNetworkConfiguration::Defined);
            switch (nmState) {
            case  NM_DEVICE_STATE_UNKNOWN:
            case NM_DEVICE_STATE_FAILED:
                priv->state = (QNetworkConfiguration::Undefined);
                break;
            case  NM_DEVICE_STATE_UNAVAILABLE:
                priv->state = (QNetworkConfiguration::Defined);
                break;
            case NM_DEVICE_STATE_DISCONNECTED:
                priv->state = ( priv->state | QNetworkConfiguration::Discovered );
                break;
            case NM_DEVICE_STATE_ACTIVATED:
                {
                    priv->state = ( priv->state | QNetworkConfiguration::Discovered
                                    | QNetworkConfiguration::Active );
                }
                break;
            default:
                priv->state = ( QNetworkConfiguration::Defined);
                break;
            };
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(priv);
            accessPointConfigurations.insert(ident, ptr);
            if (!firstUpdate) {
                QNetworkConfiguration item;
                item.d = ptr;
                emit configurationChanged( item);
            }

            if(oldState != priv->state) {
                if ( (priv->state & QNetworkConfiguration::Active) == QNetworkConfiguration::Active
                     || (oldState & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
                    switch(nmState) {
                    case NM_DEVICE_STATE_ACTIVATED:
                        emit onlineStateChanged(true);
                        break;
                    default:
                        emit onlineStateChanged(false);
                        break;
                    };
                }
            }
        }
    }
}

QStringList QNetworkConfigurationManagerPrivate::getKnownSsids()
{
    QStringList knownSsids;
    QStringList connectionServices;
    connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
    connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;

    foreach (QString service, connectionServices) {
        QNetworkManagerSettings *settingsiface;
        settingsiface = new QNetworkManagerSettings(service);
        QList<QDBusObjectPath> list = settingsiface->listConnections();
        foreach(QDBusObjectPath path, list) {
            QNetworkManagerSettingsConnection *sysIface;
            sysIface = new QNetworkManagerSettingsConnection(service, path.path());
            QNmSettingsMap map = sysIface->getSettings();

            QMap< QString, QMap<QString,QVariant> >::const_iterator i = map.find("802-11-wireless");
            while (i != map.end() && i.key() == "802-11-wireless") {
                QMap<QString,QVariant> innerMap = i.value();
                QMap<QString,QVariant>::const_iterator ii = innerMap.find("ssid");
                while (ii != innerMap.end() && ii.key() == "ssid") {
                    knownSsids << ii.value().toString();
                    ii++;
                }
                i++;
            }
        }
    }
    return knownSsids;
}
#endif

QT_END_NAMESPACE
