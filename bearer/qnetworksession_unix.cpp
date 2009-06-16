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
#include <qnetworkmanagerservice_p.h>
#endif

#include <QNetworkInterface>
static QDBusConnection dbusConnection = QDBusConnection::systemBus();

QT_BEGIN_NAMESPACE
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


quint64 QNetworkSessionPrivate::sentData() const
{
    quint64 result = 0;
    QString devFile;
    if( state == QNetworkSession::Connected ) {
        if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
            foreach (const QNetworkConfiguration &config, publicConfig.children()) {
                if ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
                    devFile = config.d->serviceInterface.name();
                }
            }
        } else {
            devFile = q->interface().name();
        }
        QFile tx("/sys/class/net/"+devFile+"/statistics/tx_bytes");
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
    QString devFile;
    if( state == QNetworkSession::Connected ) {
        if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
            foreach (const QNetworkConfiguration &config, publicConfig.children()) {
                if ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
                    devFile = config.d->serviceInterface.name();
                }
            }
        } else {
            devFile = q->interface().name();
        }

        QFile rx("/sys/class/net/"+devFile+"/statistics/rx_bytes");
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
        keepActive = true;
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
        keepActive = false;
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
    return QNetworkInterface::interfaceFromName( publicConfig.d->serviceInterface.name());
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
                interface = QNetworkInterface::interfaceFromName( publicConfig.d->serviceInterface.name()).hardwareAddress().toLower().replace(":","_");
        }
    } else {
        interface = currentInterface().hardwareAddress().toLower().replace(":","_");
    }

    QString connPath;

    QNetworkManagerInterface * ifaceD;
    ifaceD = new QNetworkManagerInterface();
    QList<QDBusObjectPath> connections = ifaceD->activeConnections();
    foreach(QDBusObjectPath path, connections) {
        QNetworkManagerConnectionActive *conDetailsD;
        conDetailsD = new QNetworkManagerConnectionActive( path.path());
        QList<QDBusObjectPath> devices = conDetailsD->devices();
        foreach(QDBusObjectPath devpath, devices) {
            QString str = devpath.path();
            if( str.contains(interface)) {
                return path.path();
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

    qDBusRegisterMetaType<QNmSettingsMap>();

    QString connPath;

    foreach (QString service, connectionServices) {
        QNetworkManagerSettings *settingsiface;
        settingsiface = new QNetworkManagerSettings(service);
        QList<QDBusObjectPath> list = settingsiface->listConnections();
        foreach(QDBusObjectPath path, list) {
            QNetworkManagerSettingsConnection *sysIface;
            sysIface = new QNetworkManagerSettingsConnection(service, path.path());
            QNmSettingsMap map = sysIface->getSettings();
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
//            } else {
//                qWarning() << "ListConnections not validd" << service;
            }

        }/* else {
            qWarning() << "not vvalid" << service;
        }*/
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
        setActiveTimeStamp();
        newState = QNetworkSession::Connected; // 3
        isActive = true;
        emit quitPendingWaitsForOpened();
        emit q->newConfigurationActivated();
        break;
    case NM_DEVICE_STATE_FAILED: // 9
        {
            emit q->error(QNetworkSession::SessionAbortedError);
            newState = QNetworkSession::NotAvailable; // 1
            isActive = false;
        }
        break;
    };
//if(!isActive && keepActive)
//    activateNmSession();
    if( newState != state) {
        state = newState;
        emit q->newConfigurationActivated();
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
    keepActive = false;
    triedServiceConnection = -1;
    state = QNetworkSession::Invalid;
//    if(!iface) {
    iface = new QNetworkManagerInterface();
    currentBearerName == "";
    QString ok;
    if(publicConfig.identifier().contains( NM_DBUS_PATH_ACCESS_POINT)) {
        QString APPAth = publicConfig.identifier();
        accessPointIface = new QNetworkManagerInterfaceAccessPoint(APPAth);
        currentBearerName = "WLAN";
        connect(accessPointIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                this,SLOT(propertiesChanged( const QString &, QMap<QString,QVariant>)));
     //   updateServiceNetworkState();
    }

    if(publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
        // this is device interface
        QList<QDBusObjectPath> devicesList = iface->getDevices();

        foreach(QDBusObjectPath devicePath, devicesList) {
            devIface = new QNetworkManagerInterfaceDevice( devicePath.path());
            QString devInterface = devIface->interface().name();
            
            if(devIface->deviceType() == DEVICE_TYPE_802_11_WIRELESS) {
                devWirelessIface = new QNetworkManagerInterfaceDeviceWireless(devIface->connectionInterface()->path());
                if(devWirelessIface->hwAddress() == publicConfig.identifier()) {

                    currentBearerName = getBearerName(DEVICE_TYPE_802_11_WIRELESS);
                    currentConnectionPath = getConnectionPath();
                    deviceStateChanged(devIface->state());

                    connect(devIface,SIGNAL(stateChanged(const QString &, quint32)),
                            this, SLOT(updateDeviceInterfaceState(const QString&, quint32)));
                    break;
                }
            } else if(devIface->deviceType() == DEVICE_TYPE_802_3_ETHERNET) {

                    devWiredIface = new QNetworkManagerInterfaceDeviceWired(devIface->connectionInterface()->path());
                    if(devWiredIface->hwAddress() == publicConfig.identifier()) {

                        currentBearerName = getBearerName( DEVICE_TYPE_802_3_ETHERNET);
                        currentConnectionPath = getConnectionPath();

                        deviceStateChanged(devIface->state());
                        connect(devIface,SIGNAL(stateChanged(const QString &, quint32)),
                                this, SLOT(updateDeviceInterfaceState(const QString&, quint32)));
                        break;
                    }
            } // end devIface
        }
    } else {
        //service network
        //  QNetworkSession::State oldState = state;
        updateServiceNetworkState();
        if ((publicConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            deviceStateChanged(8);
            // start time for new sessions
        }
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
                    interface = QNetworkInterface::interfaceFromName( publicConfig.d->serviceInterface.name()).hardwareAddress().toLower();
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

//    if (!iface.isValid()) {
//        qWarning() << "Could not find NetworkManager";
//        emit q->error(QNetworkSession::SessionAbortedError);
//        return;
//    }

    QString devicePath = "/org/freedesktop/Hal/devices/net_" + interface.replace(":","_");
//    QNetworkManagerInterfaceDevice *devIface;
    devIface = new QNetworkManagerInterfaceDevice(devicePath);
    currentBearerName = getBearerName(devIface->deviceType());

    if( devIface->state() != NM_DEVICE_STATE_ACTIVATED) {
        if(devIface->deviceType() == DEVICE_TYPE_802_11_WIRELESS) {

            devWirelessIface = new QNetworkManagerInterfaceDeviceWireless(devIface->connectionInterface()->path());

            connect(devWirelessIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                    this,SLOT(propertiesChanged( const QString &, QMap<QString,QVariant>)));

            ok = true;
        } else { // Wired interface
            devWiredIface = new QNetworkManagerInterfaceDeviceWired(devIface->connectionInterface()->path());
            if(devWiredIface->carrier()) {
                ok = true;
            }
        }

        if(ok) {
            activateConnection( connPath, devIface->connectionInterface()->path());
        } else {
            qWarning() << "NOT OK";
            emit q->error(QNetworkSession::SessionAbortedError);
        }
    } else {
        emit q->newConfigurationActivated();
    }
    //    } else {
//        qWarning() << "not VALID";
//        emit q->error(QNetworkSession::SessionAbortedError);
//    }
}

void QNetworkSessionPrivate::activateConnection( const QString & connPath, const QString &devPath)
{
    QStringList connectionServices;
    connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
    connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;

    QDBusObjectPath connectionPath(connPath);
    QDBusObjectPath devicePath(devPath);
    
    foreach (QString service, connectionServices) {
        QNetworkManagerSettings *settingsiface;
        settingsiface = new QNetworkManagerSettings(service);
        QList<QDBusObjectPath> list = settingsiface->listConnections();
        foreach(QDBusObjectPath path, list) {
            if(path == connectionPath) {
                triedServiceConnection = connectionPath.path().section('/', 4, 4, QString::SectionSkipEmpty).toInt();
                
                state = QNetworkSession::Connecting;
                emit q->stateChanged(state);
                
                connect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
                        this, SLOT(configChanged(QNetworkConfiguration)));
                
                iface = new QNetworkManagerInterface();
                connect(iface,SIGNAL(stateChanged(const QString &, quint32)),
                        this, SLOT(updateDeviceInterfaceState(const QString&, quint32)));
                iface->activateConnection(
                        service,
                        connectionPath,
                        devicePath,
                        connectionPath);
                connect(iface, SIGNAL(activationFinished(QDBusPendingCallWatcher*)),
                        this, SLOT(slotActivationFinished(QDBusPendingCallWatcher*)));
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
        QNetworkManagerInterface * iface;
        iface = new QNetworkManagerInterface();
        iface->deactivateConnection(dbpath);
        disconnect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
                   this, SLOT(configChanged(QNetworkConfiguration)));
    } else {
        qWarning() <<"Could not stop. No active path";
    }
    triedServiceConnection = -1;
}

void QNetworkSessionPrivate::setActiveTimeStamp()
{
    QString connectionIdent = q->configuration().identifier();
    QString interface = currentInterface().hardwareAddress().toLower();
    QString devicePath = "/org/freedesktop/Hal/devices/net_" + interface.replace(":","_");

    QString path;
    QNetworkManagerInterface * ifaceD;
    ifaceD = new QNetworkManagerInterface();
    QList<QDBusObjectPath> connections = ifaceD->activeConnections();
    foreach(QDBusObjectPath conpath, connections) {
        QNetworkManagerConnectionActive *conDetails;
        conDetails = new QNetworkManagerConnectionActive(conpath.path());
        QDBusObjectPath connection = conDetails->connection();

        QList<QDBusObjectPath> so = conDetails->devices();
        foreach(QDBusObjectPath device, so) {

            if(device.path() == devicePath) {
                path = connection.path();
            }
            break;
        }
    }

    QStringList connectionServices;
    connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;
    connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
    qDBusRegisterMetaType<QNmSettingsMap>();

    foreach (QString service, connectionServices) {
        QNetworkManagerSettings *settingsiface;
        settingsiface = new QNetworkManagerSettings(service);
        QList<QDBusObjectPath> list = settingsiface->listConnections();
        foreach(QDBusObjectPath path, list) {
            QNetworkManagerSettingsConnection *sysIface;
            sysIface = new QNetworkManagerSettingsConnection(service, path.path());
            QNmSettingsMap map = sysIface->getSettings();

            bool tmOk = false;
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
        }
    }
    if(startTime.isNull())
        startTime = QDateTime::currentDateTime();
}


void QNetworkSessionPrivate::updateDeviceInterfaceState(const QString &path, quint32 nmState)
{
    Q_UNUSED(path);
    deviceStateChanged(nmState);
    if(publicConfig.type() == QNetworkConfiguration::ServiceNetwork)
        updateServiceNetworkState();
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

void QNetworkSessionPrivate::configChanged(const QNetworkConfiguration& config)
{
    if(config.type() == QNetworkConfiguration::ServiceNetwork) {
        disconnect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
                   this, SLOT(configChanged(QNetworkConfiguration)));
        updateServiceNetworkState();
    }
}

void QNetworkSessionPrivate::updateServiceNetworkState()
{
    connect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
            this, SLOT(configChanged(QNetworkConfiguration)));
    foreach (const QNetworkConfiguration &config, publicConfig.children()) {
        if ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            QString devicePath = "/org/freedesktop/Hal/devices/net_" + config.identifier().replace(":","_").toLower();
            QNetworkManagerInterfaceDevice *devIfaceL;
            devIfaceL = new QNetworkManagerInterfaceDevice( devicePath);
            currentBearerName = getBearerName(devIfaceL->deviceType());
            break;
        }
    }
 //   currentConnectionPath = getConnectionPath();

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
    emit q->stateChanged(state);
}


#endif

QT_END_NAMESPACE
