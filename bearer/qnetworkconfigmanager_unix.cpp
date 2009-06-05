/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
#include <qnmdbushelper_p.h>
#endif
#include <arpa/inet.h>

QT_BEGIN_NAMESPACE

#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    static QDBusConnection dbusConnection = QDBusConnection::systemBus();
#endif

static bool NetworkManagerAvailable()
{
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    if (dbusConnection.isConnected()) {
        QDBusConnectionInterface *iface = dbusConnection.interface();
        QDBusReply<bool> reply = iface->isServiceRegistered("org.freedesktop.NetworkManager");
        if (reply.isValid())
            return reply.value();
    }
#endif
    return false;
}

QString ifaceToId( const QNetworkInterface& iface)
{
    int idx = iface.index();
    if (idx) {
        return QString::number(qHash(idx));
    } else {
        return QString::number(qHash(iface.hardwareAddress()));
    }
}

void QNetworkConfigurationManagerPrivate::registerPlatformCapabilities()
{
    if (NetworkManagerAvailable())
        capFlags |= QNetworkConfigurationManager::BearerManagement;
#ifdef Q_OS_LINUX
    //see SO_BINDTODEVICE
    capFlags |= QNetworkConfigurationManager::DirectConnectionRouting;
#endif
    capFlags |= QNetworkConfigurationManager::DataStatistics;

 #if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    testobj = new QNmDBusHelper;

    connect(testobj,SIGNAL(pathForStateChanged(const QString &, quint32)),
            this, SLOT(updateDeviceInterfaceState(const QString&, quint32)));

    connect(testobj, SIGNAL(pathForAccessPointAdded(const QString &,QDBusObjectPath)),
            this,SLOT(accessPointAdded(const QString &,QDBusObjectPath)));

    connect(testobj, SIGNAL(pathForAccessPointRemoved(const QString &,QDBusObjectPath)),
            this,SLOT(accessPointRemoved(const QString &,QDBusObjectPath)));

    connect(testobj, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SLOT(propertiesChanged( const QString &, QMap<QString,QVariant>)));

#endif
}


QTimer* updateTimer = 0;

#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
QStringList QNetworkConfigurationManagerPrivate::getKnownSsids()
{
    QStringList knownSsids;
    QStringList connectionServices;
    connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
    connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;
    qDBusRegisterMetaType<SettingsMap>();

    foreach (QString service, connectionServices) {
        QDBusInterface settingsiface(service,
                                     NM_DBUS_PATH_SETTINGS,
                                     NM_DBUS_IFACE_SETTINGS,
                                     dbusConnection);
        //NetworkManagerSettings interface
        if (settingsiface.isValid()) {
            QDBusReply<QList<QDBusObjectPath> > reply2 = settingsiface.call("ListConnections");
            if ( reply2.isValid() ) {
                QList<QDBusObjectPath> list = reply2.value();
                foreach(QDBusObjectPath path, list) {
                    QDBusInterface sysIface(service,
                                            path.path(),
                                            NM_DBUS_IFACE_SETTINGS_CONNECTION,
                                            dbusConnection);
                    if (sysIface.isValid()) {

                        QDBusReply< SettingsMap > rep = sysIface.call("GetSettings");
                        if(rep.isValid()) {
                            QMap< QString, QMap<QString,QVariant> > map = rep.value();
                            QList<QString> list = map.keys();
                            foreach (QString key, list) {
//                                qWarning() << key;
                                QMap<QString,QVariant> innerMap = map[key];
                                QMap<QString,QVariant>::const_iterator i = innerMap.constBegin();
                                while (i != innerMap.constEnd()) {
                                    QString k = i.key();
//                                    qWarning() << k;
                                    if (k == "ssid") {
//                                    qWarning() <<__FUNCTION__<< "Known ssid:"<< i.value().toString();
                                        knownSsids << i.value().toString();
                                    }
                                    i++;
                                }
                            }
                        }
                    } else
                        qWarning() << "GetSettings failed";
                }
            } else
                qWarning() << "Reply is not valid";
        }
    }
    return knownSsids;
}
#endif

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
            } else {
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

        QDBusInterface iface(NM_DBUS_SERVICE,
                             NM_DBUS_PATH,
                             NM_DBUS_INTERFACE,
                             dbusConnection); //NetworkManager interface
        if (!iface.isValid()) {
            qDebug() << "Could not find NetworkManager";
            return;
        }

        QDBusReply<QList<QDBusObjectPath> > reply = iface.call("GetDevices");
        if ( reply.isValid() ) {
            QList<QDBusObjectPath> list = reply.value();
            foreach(QDBusObjectPath path, list) {
//                qWarning() << __FUNCTION__ << "foreach device" << path.path();
                QDBusInterface devIface(NM_DBUS_SERVICE,
                                        path.path(),
                                        NM_DBUS_INTERFACE_DEVICE,
                                        dbusConnection);

                if (devIface.isValid()) {
                    if(dbusConnection.connect(NM_DBUS_SERVICE,
                                              path.path(),
                                              NM_DBUS_INTERFACE_DEVICE,
                                              "StateChanged",
                                              testobj,SLOT(deviceStateChanged(quint32)))) {
                    }

                    switch (devIface.property("DeviceType").toInt()) {
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
            }
        }
    updating = false;
#endif
    } //end netman
    emit configurationUpdateComplete();
//   QTimer::singleShot(0, this, SIGNAL( configurationUpdateComplete()));

    if (firstUpdate)
        firstUpdate = false;
}

#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
void QNetworkConfigurationManagerPrivate::updateEthConfigurations(QDBusInterface &devIface)
{
    QList<QString> knownConfigs = accessPointConfigurations.keys();

    QString ident;
    QDBusInterface devWiredIface(NM_DBUS_SERVICE,
                                 devIface.path(),
                                 NM_DBUS_INTERFACE_DEVICE_WIRED,
                                 dbusConnection);
    if (devWiredIface.isValid()) {
        ident = devWiredIface.property("HwAddress").toString(); // devWiredIface.path() //?

        if (accessPointConfigurations.contains(ident)) {
            knownConfigs.removeOne(ident);
              }
            QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
            cpPriv->name = getInterfaceName(devIface);//devIface.property("Interface").toString();
            cpPriv->isValid = true;
            cpPriv->id = ident;
            cpPriv->hwAddress = devWiredIface.property("HwAddress").toString();
            cpPriv->serviceInterface
                    = QNetworkInterface::interfaceFromName(devIface.property("Interface").toString());
            cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
            QVariant v = devIface.property("State");
            switch (v.toInt()) {
            case  NM_DEVICE_STATE_UNKNOWN:
            case  NM_DEVICE_STATE_UNAVAILABLE:
                    cpPriv->state = (cpPriv->state | QNetworkConfiguration::Undefined);
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
            cpPriv->purpose = QNetworkConfiguration::Public;

            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
            accessPointConfigurations.insert(ident, ptr);
            if (!firstUpdate) {
                QNetworkConfiguration item;
                item.d = ptr;
                emit configurationAdded(item);
            }
    }
   if (!firstUpdate && !updating) {
        emit configurationUpdateComplete();
        }
}

void QNetworkConfigurationManagerPrivate::updateWifiConfigurations(QDBusInterface &devIface)
{
    QString ident;
    QList<QString> knownConfigs = accessPointConfigurations.keys();
    QDBusInterface devWirelessIface(NM_DBUS_SERVICE,
                                    devIface.path(),
                                    NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                                    dbusConnection);
    if (devWirelessIface.isValid()) {
        if(firstUpdate) { //only connect once
            if(dbusConnection.connect(NM_DBUS_SERVICE,
                                      devIface.path(),
                                      NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                                      "AccessPointAdded",
                                      testobj, SLOT(slotAccessPointAdded( QDBusObjectPath ))) ) {
            } else {
                qWarning() << "AccessPointAdded NOT connected";
            }

            if(dbusConnection.connect(NM_DBUS_SERVICE,
                                      devIface.path(),
                                      NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                                      "AccessPointRemoved",
                                      testobj, SLOT(slotAccessPointRemoved( QDBusObjectPath ))) ) {
            } else {
                qWarning() << "AccessPointAdded NOT connected";
            }
            if(dbusConnection.connect(NM_DBUS_SERVICE,
                                      devIface.path(),
                                      NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                                      "PropertiesChanged",
                                      testobj,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) ) {
            } else {
                qWarning() << "NOT connect";
            }
        }
        // get the wifi interface state first
        QVariant activeAPPath = devWirelessIface.property("ActiveAccessPoint");
        ident = devWirelessIface.property("HwAddress").toString(); // devWirelessIface.path() //?

        if (accessPointConfigurations.contains(ident)) {
            knownConfigs.removeOne(ident);
        } //else {
            QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
            cpPriv->name = getInterfaceName(devIface);//devIface.property("Interface").toString();
            cpPriv->isValid = true;
            cpPriv->id = ident;
            cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
            cpPriv->hwAddress = devWirelessIface.property("HwAddress").toString();
            cpPriv->serviceInterface = QNetworkInterface::interfaceFromName(devIface.property("Interface").toString());
            if(activeAPPath.value<QDBusObjectPath>().path().length() > 2) {
                cpPriv->state = (cpPriv->state | QNetworkConfiguration::Defined
                                 | QNetworkConfiguration::Discovered
                                 | QNetworkConfiguration::Active );
            } else {
                cpPriv->state = (cpPriv->state | QNetworkConfiguration::Undefined);
            }

            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
            accessPointConfigurations.insert(ident, ptr);
            if (!firstUpdate) {
                QNetworkConfiguration item;
                item.d = ptr;
                emit configurationAdded(item);
            }
//        }

        QDBusReply<QList<QDBusObjectPath> > reply = devWirelessIface.call("GetAccessPoints");
        if ( reply.isValid() ) {
            QList<QDBusObjectPath> list = reply.value();
            foreach(QDBusObjectPath path, list) {
                ////////////// AccessPoints
                accessPointAdded( devIface.path(), path);
            }
        }
        updateServiceNetworks(devIface);
    } else {
        qWarning() << "wireless interface is not valid";
    }
    if (!firstUpdate&& !updating) {
        emit configurationUpdateComplete();
        }
}

QString QNetworkConfigurationManagerPrivate::getInterfaceName(QDBusInterface &devIface)
{
    QString newname;
    QVariant v = devIface.property("State");
    if (v.toUInt() == NM_DEVICE_STATE_ACTIVATED) {
        QString path = devIface.property("Ip4Config").value<QDBusObjectPath>().path();
        QDBusInterface ipIface(NM_DBUS_SERVICE,
                               path,
                               NM_DBUS_INTERFACE_IP4_CONFIG,
                               dbusConnection);
        if (ipIface.isValid()) {
            newname = ipIface.property("Domains").toStringList().join(" ");
        }
    }
    //fallback to interface name
    if(newname.isEmpty())
        newname = devIface.property("Interface").toString();
    return newname;
}

void QNetworkConfigurationManagerPrivate::updateServiceNetworks(QDBusInterface &devIface)
{
    QVariant vs = devIface.property("State");

    QList<QString> knownConfigs = accessPointConfigurations.keys();

    QString domain;
    if(vs.toUInt() == NM_DEVICE_STATE_ACTIVATED) {
        domain = getInterfaceName(devIface);
    }
    if (!snapConfigurations.contains("Internet Service Network"/*domain*/)) {
        QNetworkConfigurationPrivate* spPriv = new QNetworkConfigurationPrivate();
        spPriv->name = "Internet";//domain;//devIface.property("Interface").toString();
        spPriv->isValid = true;
        spPriv->id = "Internet Service Network";//domain;
        spPriv->state =  QNetworkConfiguration::Defined;
        spPriv->serviceInterface = QNetworkInterface::interfaceFromName(devIface.property("Interface").toString());
        spPriv->type = QNetworkConfiguration::ServiceNetwork;

        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> sptr(spPriv);
        snapConfigurations.insert(spPriv->id, sptr);
        if (!firstUpdate) {
            QNetworkConfiguration item;
            item.d = sptr;
            emit configurationAdded(item);
        }
    }

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
}
#endif

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

QStringList QNetworkConfigurationManagerPrivate::getActiveConnectionsPaths(QDBusInterface &iface)
{
    QStringList aCons;
    QVariant prop = iface.property("ActiveConnections");
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
    QDBusInterface devIface(NM_DBUS_SERVICE,
                            iPath,
                            NM_DBUS_INTERFACE_DEVICE,
                            dbusConnection);

    if (devIface.isValid()) {

        QDBusInterface devWirelessIface(NM_DBUS_SERVICE,
                                        iPath,
                                        NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                                        dbusConnection);
        if (devWirelessIface.isValid()) {

            QList<QString> knownConfigs = accessPointConfigurations.keys();

            QVariant activeAPPath = devWirelessIface.property("ActiveAccessPoint");

            QDBusInterface accessPointIface(NM_DBUS_SERVICE,
                                            path.path(),
                                            NM_DBUS_INTERFACE_ACCESS_POINT,
                                            dbusConnection);
            if (accessPointIface.isValid()) {

                if(dbusConnection.connect(NM_DBUS_SERVICE,
                                          path.path(),
                                          NM_DBUS_INTERFACE_ACCESS_POINT,
                                          "PropertiesChanged",
                                          testobj,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) ) {
                } else {
                    qWarning() << "NOT connect";
                }

                QString ident = accessPointIface.path();
                QVariant v = devIface.property("State");
                quint32 vState = v.toUInt();

                bool addIt = true;

                QString ssid = accessPointIface.property("Ssid").toString();
                QString hwAddy = accessPointIface.property("HwAddress").toString();
                QString sInterface = devIface.property("Interface").toString();
                //                foreach(QString oldIface, knownConfigs) {
                //                    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv
                //                            = accessPointConfigurations.value(oldIface);
                //                    if(priv->name == ssid /*&& priv->hwAddress == hwAddy*/) { //weed out duplicate ssid's ??
                //                        knownConfigs.removeOne(oldIface);
                //                        addIt = false;
                //                        break;
                //                    }
                //                }

                if(addIt) {
                    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
                    cpPriv->name = ssid;
                    cpPriv->isValid = true;
                    cpPriv->id = ident;
                    cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
                    cpPriv->hwAddress = accessPointIface.property("HwAddress").toString();
                    cpPriv->serviceInterface
                            = QNetworkInterface::interfaceFromName( devIface.property("Interface").toString());

                    bool knownSsid = false;
                    if(knownSsids.contains(cpPriv->name)) {
                        knownSsid = true;
                    }
                    cpPriv->state = getAPState(vState, knownSsids.contains(cpPriv->name));

                    if(activeAPPath.value<QDBusObjectPath>().path() == accessPointIface.path()) {
                        cpPriv->state = ( cpPriv->state | QNetworkConfiguration::Active);
                    }
                        if(accessPointIface.property("Flags" ).toUInt() == NM_802_11_AP_FLAGS_PRIVACY)
                            cpPriv->purpose = QNetworkConfiguration::Private;
                        else
                            cpPriv->purpose = QNetworkConfiguration::Public;

                        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
                        accessPointConfigurations.insert(ident, ptr);
                        if (!firstUpdate) {
                            QNetworkConfiguration item;
                            item.d = ptr;
                            emit configurationAdded(item);
                        }
                    }
                }
            } // end devWirelessIface
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

QNetworkConfiguration::StateFlags QNetworkConfigurationManagerPrivate::getAPState(qint32 vState, bool isKnown)
{
    QNetworkConfiguration::StateFlags state = QNetworkConfiguration::Undefined;

    // this is the state of the wifi device interface
    if(isKnown)
        state = ( QNetworkConfiguration::Defined);

    switch(vState) { //device interface state, not AP state
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

void QNetworkConfigurationManagerPrivate::propertiesChanged( const QString & path, QMap<QString,QVariant> map)
{
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();
        if( i.key() == "State") { //only applies to device interfaces
            updateAccessPointState(path, i.value().toUInt());
        }
        /*else if( i.key() == "ActiveAccessPoint") {
            qWarning()  << __FUNCTION__ << i.key() << ": " << i.value().value<QDBusObjectPath>().path();
        } else if( i.key() == "Strength")
            qWarning()  << __FUNCTION__ << i.key() << ": " << i.value().toUInt();*/
        //   else
        //            qWarning()  << __FUNCTION__ << i.key() << ": " << i.value();
    }
}

void QNetworkConfigurationManagerPrivate::updateAccessPointState(const QString &deviceInterfacePath, quint32 state)
{
    bool updated = false;
    switch(state) {
    case NM_DEVICE_STATE_ACTIVATED:
        {
            QDBusInterface devWirelessIface(NM_DBUS_SERVICE,
                                            deviceInterfacePath,
                                            NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                                            dbusConnection);
            if (devWirelessIface.isValid()) {
                QVariant activeAPPath = devWirelessIface.property("ActiveAccessPoint");
                QDBusInterface accessPointIface(NM_DBUS_SERVICE,
                                                activeAPPath.value<QDBusObjectPath>().path(),
                                                NM_DBUS_INTERFACE_ACCESS_POINT,
                                                dbusConnection);
                if (accessPointIface.isValid()) {
                    updateState(activeAPPath.value<QDBusObjectPath>().path(), state);
                    currentActiveAP = activeAPPath.value<QDBusObjectPath>().path();
                    updated = true;
                }
            }
        }
        break;
    case NM_DEVICE_STATE_DISCONNECTED:
    case NM_DEVICE_STATE_UNAVAILABLE:
    case NM_DEVICE_STATE_FAILED:
        {
           updateDeviceInterfaceState(deviceInterfacePath, state); //update device interface state just for good measure
            QDBusInterface devWirelessIface(NM_DBUS_SERVICE,
                                            deviceInterfacePath,
                                            NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                                            dbusConnection);
            if (devWirelessIface.isValid()) {
                if(!currentActiveAP.isEmpty() ){
                    updateState(currentActiveAP, state);
                    updated = true;
                    currentActiveAP = "";
                } else {
                    updateConfigurations();
//                    QDBusReply<QList<QDBusObjectPath> > reply = devWirelessIface.call("GetAccessPoints");
//                    if ( reply.isValid() ) {
//                        QList<QDBusObjectPath> list = reply.value();
//                        foreach(QDBusObjectPath aPath, list) {
//                            QDBusInterface accessPointIface(NM_DBUS_SERVICE,
//                                                            aPath.path(),
//                                                            NM_DBUS_INTERFACE_ACCESS_POINT,
//                                                            dbusConnection);
//                            if (accessPointIface.isValid()) {
//                                updateState(accessPointIface.path(), state);
//                                updated = true;
//                            }
//                        }
//                    }
                }
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
                priv->state = ( QNetworkConfiguration::Undefined);
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

#endif

QT_END_NAMESPACE
