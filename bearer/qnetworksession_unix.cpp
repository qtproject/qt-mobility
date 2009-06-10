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

#include "qnetworksession_p.h"
#include <qnetworkconfiguration.h>
#include <QFile>

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

#include <QNetworkInterface>
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    static QDBusConnection dbusConnection = QDBusConnection::systemBus();
    static QDBusInterface iface(NM_DBUS_SERVICE, NM_DBUS_PATH, NM_DBUS_INTERFACE, dbusConnection);
#endif

QT_BEGIN_NAMESPACE
static bool NetworkManagerAvailable()
{
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    if (dbusConnection.isConnected()) {
        QDBusConnectionInterface *dbiface = dbusConnection.interface();
        QDBusReply<bool> reply = dbiface->isServiceRegistered("org.freedesktop.NetworkManager");
        if (reply.isValid())
            return reply.value();
    }
#endif
    return false;
}


quint64 QNetworkSessionPrivate::sentData() const
{
    quint64 result = 0;
    if( state == QNetworkSession::Connected ) {
        QFile tx("/sys/class/net/"+q->interface().name()+"/statistics/tx_bytes");
        if(tx.exists() && tx.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&tx);
            in >> result;
            tx.close();
        }
    }
    return result;
}

quint64 QNetworkSessionPrivate::receivedData() const
{
    quint64 result = 0;
    if( state == QNetworkSession::Connected ) {

        QFile rx("/sys/class/net/"+q->interface().name()+"/statistics/rx_bytes");
        if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&rx);
            in >> result;
            rx.close();
        }
    }
    return result;
}

quint64 QNetworkSessionPrivate::activeTime() const
{
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    if (startTime.isNull()) {
        return 0;
    }
    if(state == QNetworkSession::Connected )
        return startTime.secsTo(QDateTime::currentDateTime());
#endif
    return 0;
}


void QNetworkSessionPrivate::syncStateWithInterface()
{
    if(!NetworkManagerAvailable()) {
        //TODO

    } else {
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
        updateNetworkConfigurations();
#endif
    }
}

void QNetworkSessionPrivate::open()
{
    if(!NetworkManagerAvailable()) {
        //TODO
    } else {
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
        activateNmSession();
#endif
    } //end NetworkManager
}

void QNetworkSessionPrivate::close()
{
    stop();
}

void QNetworkSessionPrivate::stop()
{
    if(!NetworkManagerAvailable()) {
        //TODO
    } else {
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
        deactivateNmSession();
#endif
    }
}

void QNetworkSessionPrivate::migrate()
{
    qFatal("Function not implemented at %s.", __FUNCTION__);
}

void QNetworkSessionPrivate::accept()
{
    qFatal("Function not implemented at %s.", __FUNCTION__);
}

void QNetworkSessionPrivate::ignore()
{
    qFatal("Function not implemented at %s.", __FUNCTION__);
}

void QNetworkSessionPrivate::reject()
{
    qFatal("Function not implemented at %s.", __FUNCTION__);
}

QNetworkInterface QNetworkSessionPrivate::currentInterface() const
{
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    return QNetworkInterface::interfaceFromName(getCurrentInterfaceName());
#else
    return QNetworkInterface();
#endif
}



QVariant QNetworkSessionPrivate::property(const QString& key)
{
    if (!publicConfig.isValid())
        return QVariant();

    if (key == "ActiveConfigurationIdentifier") {
        if (!isActive) {
            return QString();
        } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork){
            return actualConfig.identifier();
        } else {
            return publicConfig.identifier();
        }
    }
    return QVariant();
}

QString QNetworkSessionPrivate::bearerName() const
{
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    return currentBearerName;
#else
    return QString();
#endif
}

QString QNetworkSessionPrivate::errorString() const
{
    QString errorStr;
    switch(q->error()) {
    case QNetworkSession::RoamingError:
        errorStr = QObject::tr("Roaming error");
        break;
    case QNetworkSession::SessionAbortedError:
        errorStr = QObject::tr("Session aborted by user or system");
        break;
    default:
    case QNetworkSession::UnknownSessionError:
        errorStr = QObject::tr("Unidentified Error");
        break;
    }
    return errorStr;
}

QNetworkSession::SessionError QNetworkSessionPrivate::error() const
{
    return QNetworkSession::UnknownSessionError;
}


#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)

QString QNetworkSessionPrivate::getActiveConnectionPath()
{
    QString interface;
    if(publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        foreach (const QNetworkConfiguration &config, publicConfig.children()) {
            if ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active)
                interface = QNetworkInterface::interfaceFromName(getCurrentInterfaceName(config.name())).hardwareAddress().toLower().replace(":","_");
        }
    } else {
        interface = currentInterface().hardwareAddress().toLower().replace(":","_");
    }

    QString connPath;
    QVariant prop = iface.property("ActiveConnections");
    QList<QDBusObjectPath> connections = prop.value<QList<QDBusObjectPath> >();
    foreach(QDBusObjectPath path, connections) {
        QDBusInterface conDetails(NM_DBUS_SERVICE,
                                  path.path(),
                                  NM_DBUS_INTERFACE_ACTIVE_CONNECTION,
                                  dbusConnection);

        if (conDetails.isValid()) {

            QList<QDBusObjectPath> devices = conDetails.property("Devices").value<QList<QDBusObjectPath> >();
            foreach(QDBusObjectPath devpath, devices) {
                QString str = devpath.path();
                qWarning() << "find" << str << interface;
                if( str.contains(interface)) {
                    return path.path();
                }
            }
        }
    }
    return QString();
}

QString QNetworkSessionPrivate::getConnectionPath(const QString &name)
{
    QString configName;

    if(name.isEmpty())
        configName = q->configuration().name();
    else
        configName = name;

    QString connectionIdent = q->configuration().identifier();
    QString interface = currentInterface().hardwareAddress().toLower();
    QString devicePath = "/org/freedesktop/Hal/devices/net_" + interface.replace(":","_");

    QStringList connectionServices;
    connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
    connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;

    qDBusRegisterMetaType<SettingsMap>();

    QString connPath;

    foreach (QString service, connectionServices) {
        QDBusInterface allCons(service,
                               NM_DBUS_PATH_SETTINGS,
                               NM_DBUS_IFACE_SETTINGS,
                               dbusConnection);
        if (allCons.isValid()) {
            QDBusReply<QList<QDBusObjectPath> > reply = allCons.call("ListConnections");
            if ( reply.isValid() ) {
                QList<QDBusObjectPath> list = reply.value();
                foreach(QDBusObjectPath path, list) {
                    QDBusInterface sysIface(service,
                                            path.path(),
                                            NM_DBUS_IFACE_SETTINGS_CONNECTION,
                                            dbusConnection);
                    if (sysIface.isValid()) {


                       QDBusReply< SettingsMap > rep = sysIface.call("GetSettings");
                        if(rep.isValid()) {
                            QMap< QString, QMap<QString,QVariant> > map = rep.value();
                            QMap< QString, QMap<QString,QVariant> >::const_iterator i = map.find("connection");
                            while (i != map.end() && i.key() == "connection") {
                                QMap<QString,QVariant> innerMap = i.value();
                                QMap<QString,QVariant>::const_iterator ii = innerMap.find("id");
                                while (ii != innerMap.end() && ii.key() == "id") {
                                    if(ii.value().toString() == configName) {
                                        connPath = path.path();
                                        return connPath;
                                    }
                                    ii++;
                                }
                                i++;
                            }
                        }
                    } else {
                        qWarning() << "not valid";
                    }
                } // end foreach
            } else {
                qWarning() << "ListConnections not validd" << service;
            }

        } else {
            qWarning() << "not vvalid" << service;
        }
    }
    return QString();
}

void QNetworkSessionPrivate::deviceStateChanged(quint32 devstate)
{
    QNetworkSession::State newState = QNetworkSession::Invalid;
    switch(devstate) {
    case NM_DEVICE_STATE_UNKNOWN: // 0
    case NM_DEVICE_STATE_UNMANAGED: // 1
        newState = QNetworkSession::Invalid; // 0
        currentConnectionPath = "";
        isActive = false;
        break;
    case NM_DEVICE_STATE_UNAVAILABLE: // 2
        newState = QNetworkSession::NotAvailable; // 1
        currentConnectionPath = "";
        isActive = false;
        break;
    case NM_DEVICE_STATE_DISCONNECTED: // 3
        newState = QNetworkSession::Disconnected; // 5
        currentConnectionPath = "";
        isActive = false;
        break;
    case NM_DEVICE_STATE_PREPARE: // 4
    case NM_DEVICE_STATE_CONFIG: // 5
    case NM_DEVICE_STATE_NEED_AUTH: // 6
    case NM_DEVICE_STATE_IP_CONFIG: // 7
        newState = QNetworkSession::Connecting; // 2
        isActive = false;
        break;
    case NM_DEVICE_STATE_ACTIVATED: // 8
        startTime = QDateTime::currentDateTime();
        newState = QNetworkSession::Connected; // 3
        isActive = true;
        emit quitPendingWaitsForOpened();
        emit q->newConfigurationActivated();
        break;
    case NM_DEVICE_STATE_FAILED: // 9
        {
            if(triedServiceConnection == -1) {
                emit q->error(QNetworkSession::SessionAbortedError);
                newState = QNetworkSession::NotAvailable; // 1
                isActive = false;
            } else {
                if(publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
                    activateNmSession();
                }
            }
            //        newState = QNetworkSession::Closing; // 4
        }
        break;
    };

    if( newState != state) {
        state = newState;
        emit q->stateChanged(state);
    }
}

QString QNetworkSessionPrivate::getBearerName(quint32 type)
{
    QString name;
    switch(type) {
            case  DEVICE_TYPE_UNKNOWN:
        break;
            case DEVICE_TYPE_802_3_ETHERNET:
        name = "Ethernet";
        break;
            case DEVICE_TYPE_802_11_WIRELESS:
        name = "WLAN";
        break;
            case DEVICE_TYPE_GSM:
        name = "2G";
        break;
            case DEVICE_TYPE_CDMA:
        name = "CDMA2000";
        break;
    };
    return name;
}

void QNetworkSessionPrivate::updateNetworkConfigurations()
{
    triedServiceConnection = -1;
    nmDBusObj = new QNmDBusHelper;
    state = QNetworkSession::Invalid;

    if (!iface.isValid()) {
        qWarning() << "Could not find NetworkManager";
        emit q->error(QNetworkSession::SessionAbortedError);
        return;
    }

    connect(nmDBusObj,SIGNAL(pathForStateChanged(const QString &, quint32)),
            this, SLOT(updateDeviceInterfaceState(const QString&, quint32)));

    currentBearerName == "";

    if(publicConfig.identifier().contains( NM_DBUS_PATH_ACCESS_POINT)) {

        QString APPAth = publicConfig.identifier();
        QDBusInterface accessPointIface(NM_DBUS_SERVICE,
                                        APPAth,
                                        NM_DBUS_INTERFACE_ACCESS_POINT,
                                        dbusConnection);
        if (accessPointIface.isValid()) {
            currentBearerName = "WLAN";
//            connect(nmDBusObj, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
//                    this,SLOT(propertiesChanged( const QString &, QMap<QString,QVariant>)));
//            if(dbusConnection.connect(NM_DBUS_SERVICE,
//                                      APPAth,
//                                      NM_DBUS_INTERFACE_ACCESS_POINT,
//                                      "PropertiesChanged",
//                                      nmDBusObj,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) ) {
//            }
        } else
            qWarning() << "NOT VALID";
    } else if(publicConfig.type() != QNetworkConfiguration::ServiceNetwork) {
        // this is device interface
        QDBusReply<QList<QDBusObjectPath> > reply = iface.call("GetDevices");
        if ( reply.isValid() ) {
            QList<QDBusObjectPath> devicesList = reply.value();

            foreach(QDBusObjectPath devicePath, devicesList) {
                QDBusInterface devIface(NM_DBUS_SERVICE,
                                        devicePath.path(),
                                        NM_DBUS_INTERFACE_DEVICE,
                                        dbusConnection);
                if (devIface.isValid()) {
                    QVariant v = devIface.property("DeviceType");

                    if(v.toUInt() == DEVICE_TYPE_802_11_WIRELESS) {
                        QDBusInterface devWirelessIface(NM_DBUS_SERVICE,
                                                        devIface.path(),
                                                        NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                                                        dbusConnection);
                        if (devWirelessIface.isValid()) {
                            if(devWirelessIface.property("HwAddress").toString() == publicConfig.identifier()) {

                                currentBearerName = getBearerName(DEVICE_TYPE_802_11_WIRELESS);

                                currentConnectionPath = getConnectionPath();
                                v = devIface.property("State");
                                deviceStateChanged(v.toUInt());
                                if(dbusConnection.connect(NM_DBUS_SERVICE,
                                                          devicePath.path(),
                                                          NM_DBUS_INTERFACE_DEVICE,
                                                          "StateChanged",
                                                          nmDBusObj,SLOT(deviceStateChanged(quint32)))) {
                                }
                                break;
                            }
                        }
                    } else if(v.toUInt() == DEVICE_TYPE_802_3_ETHERNET) {

                        QDBusInterface devWiredIface(NM_DBUS_SERVICE,
                                                     devIface.path(),
                                                     NM_DBUS_INTERFACE_DEVICE_WIRED,
                                                     dbusConnection);
                        if (devWiredIface.isValid()) {
                            if(devWiredIface.property("HwAddress").toString() == publicConfig.identifier()) {

                                currentBearerName = getBearerName( DEVICE_TYPE_802_3_ETHERNET);
                                currentConnectionPath = getConnectionPath();

                                v = devIface.property("State");
                                deviceStateChanged(v.toUInt());
//                                if(dbusConnection.connect(NM_DBUS_SERVICE,
//                                                          devicePath.path(),
//                                                          NM_DBUS_INTERFACE_DEVICE,
//                                                          "StateChanged",
//                                                          nmDBusObj,SLOT(deviceStateChanged(quint32)))) {
//                                }
                                break;
                            }
                        }
                    }
                } // end devIface
            }
        }
    } else {
        //service network
        //  QNetworkSession::State oldState = state;

        if (!publicConfig.isValid()) {
            state = QNetworkSession::Invalid;
        } else if ((publicConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            state = QNetworkSession::Connected;
        } else if ((publicConfig.state() & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered) {
            state = QNetworkSession::Disconnected;
        } else if ((publicConfig.state() & QNetworkConfiguration::Defined) == QNetworkConfiguration::Defined) {
            state = QNetworkSession::NotAvailable;
        } else if ((publicConfig.state() & QNetworkConfiguration::Undefined) == QNetworkConfiguration::Undefined) {
            state = QNetworkSession::NotAvailable;
        }
    }
    if ((publicConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
        deviceStateChanged(8);
        // start time for new sessions
    }
    setActiveTimeStamp();

}

void QNetworkSessionPrivate::activateNmSession()
{
    bool ok = false;
    currentBearerName == "";

    QString connPath;
    QString connectionIdent;
    QString interface;
    if(publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        // get list of connections. active first connection
        foreach (const QNetworkConfiguration &config, publicConfig.children()) {
            if ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active)
                continue;
            if ((config.state() & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered) {
                connPath = getConnectionPath(config.name());
                qint32 numCon = connPath.section('/', 4, 4, QString::SectionSkipEmpty).toInt();
                if(numCon < triedServiceConnection) { //if rolled around stop this crazy thing!
                    emit q->error(QNetworkSession::SessionAbortedError);
                    return;
                }
                if(triedServiceConnection != numCon) {
                    connectionIdent = config.identifier();
                    interface = QNetworkInterface::interfaceFromName(getCurrentInterfaceName(config.name())).hardwareAddress().toLower();
                    break;
                }
            }
        }
    } else if(publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
        triedServiceConnection = -1;
        connPath = getConnectionPath();
        connectionIdent = q->configuration().identifier();
        interface = currentInterface().hardwareAddress().toLower();
    }

    if(connPath.isEmpty()) {
        qWarning() << "No known connection configuration";
        emit q->error(QNetworkSession::SessionAbortedError);
        return;
    }

    if (!iface.isValid()) {
        qWarning() << "Could not find NetworkManager";
        emit q->error(QNetworkSession::SessionAbortedError);
        return;
    }

    QString devicePath = "/org/freedesktop/Hal/devices/net_" + interface.replace(":","_");

    QDBusInterface devIface(NM_DBUS_SERVICE,
                            devicePath,
                            NM_DBUS_INTERFACE_DEVICE,
                            dbusConnection);
    QVariant v;
    if (devIface.isValid()) {

        v = devIface.property("DeviceType");
        currentBearerName = getBearerName(v.toUInt());

        v = devIface.property("State");

        if( v.toUInt() != NM_DEVICE_STATE_ACTIVATED) {
            if(devIface.property("DeviceType").toUInt() == DEVICE_TYPE_802_11_WIRELESS) {
                QDBusInterface devWirelessIface(NM_DBUS_SERVICE,
                                                devIface.path(),
                                                NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                                                dbusConnection);
                if (devWirelessIface.isValid()) {

//                    connect(nmDBusObj, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
//                            this,SLOT(propertiesChanged( const QString &, QMap<QString,QVariant>)));
//
//                    if(dbusConnection.connect(NM_DBUS_SERVICE,
//                                              devIface.path(),
//                                              NM_DBUS_INTERFACE_DEVICE_WIRELESS,
//                                              "PropertiesChanged",
//                                              nmDBusObj,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) ) {
//                    } else {
//                        qWarning() << "NOT connect";
//                    }
                    ok = true;
                } else {
                    qWarning() << "devWirelessIface is not valid" << devIface.path();
                }
            } else { // Wired interface
                QDBusInterface devWiredIface(NM_DBUS_SERVICE,
                                             devIface.path(),
                                             NM_DBUS_INTERFACE_DEVICE_WIRED,
                                             dbusConnection);
                if (devWiredIface.isValid()) {

                    if(devWiredIface.property("Carrier").toBool()) {
                        ok = true;
                    }
                }
            }

            if(ok) {
                activateConnection(iface, connPath, devIface.path());
            } else {
                qWarning() << "NOT OK";
                emit q->error(QNetworkSession::SessionAbortedError);
            }
        } else {
            emit q->newConfigurationActivated();
        }
    } else {
        qWarning() << "not VALID";
        emit q->error(QNetworkSession::SessionAbortedError);
    }
}

void QNetworkSessionPrivate::activateConnection(QDBusInterface &iface, const QString & connPath, const QString &devPath)
{
if(!dbusConnection.connect(NM_DBUS_SERVICE,
                               devPath,
                               NM_DBUS_INTERFACE_DEVICE,
                               "StateChanged",
                               nmDBusObj,SLOT(deviceStateChanged(quint32)))) {
        qWarning() << __FUNCTION__ << "XXXXXXXXXX dbus connect NOT successful" <<  devPath;
    }

    QStringList connectionServices;
    connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
    connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;

    QDBusObjectPath connectionPath(connPath);
    QDBusObjectPath devicePath(devPath);

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

                    if(path == connectionPath) {

                        triedServiceConnection = connectionPath.path().section('/', 4, 4, QString::SectionSkipEmpty).toInt();

                        state = QNetworkSession::Connecting;
                        emit q->stateChanged(state);

                        connect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
                                this, SLOT(configChanged(QNetworkConfiguration)));

                        QDBusPendingCall pendingCall = iface.asyncCall("ActivateConnection",
                                                                       QVariant(service),
                                                                       QVariant::fromValue(connectionPath),
                                                                       QVariant::fromValue(devicePath),
                                                                       QVariant::fromValue(connectionPath));

                        QDBusPendingCallWatcher *callWatcher = new QDBusPendingCallWatcher(pendingCall, this);

                        QObject::connect(callWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                                         this, SLOT(slotActivationFinished(QDBusPendingCallWatcher*)));
                    }
                }
            }
        }
    }
}

void QNetworkSessionPrivate::slotActivationFinished(QDBusPendingCallWatcher* openCall)
{
    QDBusPendingReply<QDBusObjectPath> reply = *openCall;
    if (reply.isError()) {
        disconnect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
                this, SLOT(configChanged(QNetworkConfiguration)));
        emit q->error(QNetworkSession::UnknownSessionError);
    } else {
        QDBusObjectPath result = reply.value();
        currentConnectionPath = result.path();
    }
}

void QNetworkSessionPrivate::deactivateNmSession()
{
    QString activeConnectionPath = getActiveConnectionPath();
    if (!activeConnectionPath.isEmpty()) {
        QDBusObjectPath dbpath(activeConnectionPath);
        if (!iface.isValid()) {
            emit q->error(QNetworkSession::UnknownSessionError);
            return;
        }
        iface.call("DeactivateConnection", QVariant::fromValue(dbpath));
        disconnect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
                this, SLOT(configChanged(QNetworkConfiguration)));
    } else {
        qWarning() <<"Could not stop. No active path";
    }
}

void QNetworkSessionPrivate::setActiveTimeStamp()
{
    QString connectionIdent = q->configuration().identifier();
    QString interface = currentInterface().hardwareAddress().toLower();
    QString devicePath = "/org/freedesktop/Hal/devices/net_" + interface.replace(":","_");

    QString path;

    if (iface.isValid()) {
        QVariant prop = iface.property("ActiveConnections");
        QList<QDBusObjectPath> connections = prop.value<QList<QDBusObjectPath> >();
        foreach(QDBusObjectPath conpath, connections) {
            QDBusInterface conDetails(NM_DBUS_SERVICE,
                                      conpath.path(),
                                      NM_DBUS_INTERFACE_ACTIVE_CONNECTION,
                                      dbusConnection);
            if (conDetails.isValid()) {
                QVariant prop = conDetails.property("Connection");
                QDBusObjectPath connection = prop.value<QDBusObjectPath>();

                QVariant Sprop = conDetails.property("Devices");
                QList<QDBusObjectPath> so = Sprop.value<QList<QDBusObjectPath> >();
                foreach(QDBusObjectPath device, so) {

                    if(device.path() == devicePath) {
                        path = connection.path();
                    }
                    break;
                }
            }
        }
    }

    QStringList connectionServices;
    connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;
    connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
    qDBusRegisterMetaType<SettingsMap>();

    foreach (QString service, connectionServices) {
        QDBusInterface sysIface(service,
                                path,
                                NM_DBUS_IFACE_SETTINGS_CONNECTION,
                                dbusConnection);
        if (sysIface.isValid()) {
            QDBusReply< SettingsMap > rep = sysIface.call("GetSettings");
            if(rep.isValid()) {
                bool tmOk = false;
                QMap< QString, QMap<QString,QVariant> > map = rep.value();
                QMap< QString, QMap<QString,QVariant> >::const_iterator i = map.find("connection");
                while (i != map.end() && i.key() == "connection") {
                    QMap<QString,QVariant> innerMap = i.value();
                    QMap<QString,QVariant>::const_iterator ii = innerMap.find("id");
                    while (ii != innerMap.end() && ii.key() == "id") {
                        if(ii.value().toString() == q->configuration().name()) {
                            tmOk = true;
                        } else
                            tmOk = false;
                        ii++;
                    }
                    while (ii != innerMap.end() && ii.key() == "timestamp" && tmOk) {
                        startTime = QDateTime::fromTime_t(ii.value().toUInt());
                        ii++;
                        break;
                    }
                    i++;
                }
            } else {
                //                qWarning() << "session GetSettings failed";
                break;
            }
        }
    }
}


void QNetworkSessionPrivate::updateDeviceInterfaceState(const QString &path, quint32 nmState)
{
    Q_UNUSED(path);
   deviceStateChanged(nmState);
}

void QNetworkSessionPrivate::propertiesChanged( const QString & path, QMap<QString,QVariant> map)
{
    Q_UNUSED(path);
    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext()) {
        i.next();

        if( i.key() == "State") { //only applies to device interfaces
            deviceStateChanged(i.value().toUInt());
        }
        else if( i.key() == "ActiveAccessPoint") {
        }
    }
}

QString QNetworkSessionPrivate::getCurrentInterfaceName(const QString &idName) const
{
    QString id;
    if(publicConfig.type() == QNetworkConfiguration::ServiceNetwork)
        id = idName;
    else
        id = q->configuration().identifier();

    if (iface.isValid()) {
        QDBusReply<QList<QDBusObjectPath> > reply = iface.call("GetDevices");
        if ( reply.isValid() ) {
            QList<QDBusObjectPath> list = reply.value();
            foreach(QDBusObjectPath path, list) {
                QDBusInterface devIface(NM_DBUS_SERVICE,
                                        path.path(),
                                        NM_DBUS_INTERFACE_DEVICE,
                                        dbusConnection); //device interface
                QString ident;
                if (devIface.isValid()) {
                    if(devIface.property("DeviceType").toInt() == DEVICE_TYPE_802_11_WIRELESS) { //wifi interface
                        QDBusInterface devWirelessIface(NM_DBUS_SERVICE,
                                                        devIface.path(),
                                                        NM_DBUS_INTERFACE_DEVICE_WIRELESS,
                                                        dbusConnection);
                        if (devWirelessIface.isValid()) {
                            if( q->configuration().name() == devIface.property("Interface").toString()) {
                                return  devIface.property("Interface").toString();
                            } else {
                                if( q->configuration().identifier().contains("AccessPoint")) {
                                    QDBusInterface accessPointIface(NM_DBUS_SERVICE,
                                                                    q->configuration().identifier(),
                                                                    NM_DBUS_INTERFACE_ACCESS_POINT,
                                                                    dbusConnection);
                                    if (accessPointIface.isValid()) {
                                        return devIface.property("Interface").toString();
                                    }

                                } else {
                                    QDBusReply<QList<QDBusObjectPath> > reply = devWirelessIface.call("GetAccessPoints");
                                    if ( reply.isValid() ) {
                                        QList<QDBusObjectPath> list = reply.value();
                                        int i =0;
                                        foreach(QDBusObjectPath path, list) {
                                            QDBusInterface accessPointIface(NM_DBUS_SERVICE,
                                                                            path.path(),
                                                                            NM_DBUS_INTERFACE_ACCESS_POINT,
                                                                            dbusConnection);
                                            if (accessPointIface.isValid()) {
                                                if( accessPointIface.path() == id ||
                                                    accessPointIface.property("Ssid").toString() == id ) {
                                                    return devIface.property("Interface").toString();
                                                }
                                                i++;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    } else if(devIface.property("DeviceType").toInt() == DEVICE_TYPE_802_3_ETHERNET) { //ethernet interface

                        QDBusInterface devWiredIface(NM_DBUS_SERVICE,
                                                     devIface.path(),
                                                     NM_DBUS_INTERFACE_DEVICE_WIRED,
                                                     dbusConnection);
                        if (devWiredIface.isValid()) {
                            if( q->configuration().identifier() == devWiredIface.property("HwAddress").toString()) {
                                return devIface.property("Interface").toString();
                            }
                        }
                    }
                }
            }
        }
    }
    return QString();
}

void QNetworkSessionPrivate::configChanged(const QNetworkConfiguration& /*config*/)
{
//    qWarning() << __FUNCTION__ << config.identifier() << config.name();
}



#endif

QT_END_NAMESPACE
