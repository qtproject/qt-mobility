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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qnetworksession_p.h"
#include <qnetworkconfiguration.h>
#include <QFile>
#include <QMutex>

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

//////
class QNetworkSessionManagerPrivate : public QObject
{
    Q_OBJECT

public:
    QNetworkSessionManagerPrivate(QObject *parent = 0);
    ~QNetworkSessionManagerPrivate();

    void increment(const QNetworkConfiguration &config);
    void decrement(const QNetworkConfiguration &config);
    void clear(const QNetworkConfiguration &config);

    unsigned int referenceCount(const QNetworkConfiguration &config);

private:

Q_SIGNALS:
    void forcedSessionClose(const QNetworkConfiguration &config);
    void startConfiguration(const QNetworkConfiguration &config);
    void stopConfiguration(const QNetworkConfiguration &config);

private:
    QMutex lock;
    QMap<QString, unsigned int> refCount;
};

#include "qnetworksession_unix.moc"

Q_GLOBAL_STATIC(QNetworkSessionManagerPrivate, sessionManager);

QNetworkSessionManagerPrivate::QNetworkSessionManagerPrivate(QObject *parent)
:   QObject(parent)
{
}

QNetworkSessionManagerPrivate::~QNetworkSessionManagerPrivate()
{
}

void QNetworkSessionManagerPrivate::increment(const QNetworkConfiguration &config)
{
    QMutexLocker locker(&lock);

    ++refCount[config.identifier()];
//    qWarning() << __FUNCTION__ << refCount[config.identifier()] << config.name() << config.state();

    if ((config.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active &&
        (config.state() & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered) {
        emit startConfiguration(config);
    }
}

void QNetworkSessionManagerPrivate::decrement(const QNetworkConfiguration &config)
{
    QMutexLocker locker(&lock);

if (refCount[config.identifier()] > 0) {
        --refCount[config.identifier()];
//qWarning() << __FUNCTION__ << refCount[config.identifier()] << config.name()<<config.state() ;

        if (refCount[config.identifier()] == 0 &&
            (config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            emit stopConfiguration(config);
        }
    }
}

void QNetworkSessionManagerPrivate::clear(const QNetworkConfiguration &config)
{
    QMutexLocker locker(&lock);

    if (refCount[config.identifier()] != 0) {
        refCount[config.identifier()] = 0;
//qWarning() << __FUNCTION__ << refCount[config.identifier()] << config.name()<<config.state() ;

        if ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active)
            emit stopConfiguration(config);

        emit forcedSessionClose(config);
    }
}

unsigned int QNetworkSessionManagerPrivate::referenceCount(const QNetworkConfiguration &config)
{
    QMutexLocker locker(&lock);

    return refCount[config.identifier()];
}

///////////
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
    state = QNetworkSession::Invalid;
    lastError = QNetworkSession::UnknownSessionError;

    connect(sessionManager(), SIGNAL(forcedSessionClose(QNetworkConfiguration)),
            this, SLOT(deactivateNmSession(QNetworkConfiguration)));
    connect(sessionManager(), SIGNAL(startConfiguration(QNetworkConfiguration)),
            this, SLOT(activateNmSession(QNetworkConfiguration)));
    connect(sessionManager(), SIGNAL(stopConfiguration(QNetworkConfiguration)),
            this, SLOT(deactivateNmSession(QNetworkConfiguration)));

    if(!NetworkManagerAvailable()) {
        //TODO

    } else {
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
        updateNetworkConfigurations();
        if ((publicConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            sessionManager()->increment(publicConfig);
        }
#endif
    }
}

void QNetworkSessionPrivate::open()
{
    if (!NetworkManagerAvailable()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit q->error(lastError);
    } else {
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
        if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
            lastError = QNetworkSession::OperationNotSupportedError;
            emit q->error(lastError);
            return;
        }

        if (!isActive) {
            if ((publicConfig.state() & QNetworkConfiguration::Discovered) !=
                QNetworkConfiguration::Discovered) {
                lastError =QNetworkSession::OperationNotSupportedError;
                emit q->error(lastError);
                return;
            }
        }
            // increment session count
            sessionManager()->increment(publicConfig);
#endif
    } //end NetworkManager
}

void QNetworkSessionPrivate::close()
{
    if (!NetworkManagerAvailable()) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit q->error(lastError);
    } else {
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
        if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
            lastError = QNetworkSession::OperationNotSupportedError;
            emit q->error(lastError);
            return;
        }
        if (isActive) {
            // decrement session count
            QString APPAth = publicConfig.identifier();

            sessionManager()->decrement(publicConfig);

            if(APPAth.contains("AccessPoint")) {
                disconnect(accessPointIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                        this,SLOT(propertiesChanged( const QString &, QMap<QString,QVariant>)));
            }
            if(sessionManager()->referenceCount(publicConfig) > 0)
                isActive = false;
            emit q->sessionClosed();
        } else {
            qWarning() << __FUNCTION__ << "session is not active";
        }
#endif
    }
}

void QNetworkSessionPrivate::forcedSessionClose(const QNetworkConfiguration &config)
{
    if (publicConfig == config) {
        isActive = false;

        sessionManager()->decrement(publicConfig);
        emit q->sessionClosed();

        lastError = QNetworkSession::SessionAbortedError;
        emit q->error(lastError);
    }
}

void QNetworkSessionPrivate::stop()
{
    if(!NetworkManagerAvailable()) {
        //TODO
    } else {
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)

    if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        lastError = QNetworkSession::OperationNotSupportedError;
        emit q->error(lastError);
        return;
    }
            // force the session reference count to 0
        QString APPAth = publicConfig.identifier();

        sessionManager()->clear(publicConfig);
        if(APPAth.contains("AccessPoint")) {
            disconnect(accessPointIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                    this,SLOT(propertiesChanged( const QString &, QMap<QString,QVariant>)));
        }
        isActive = false;
        emit q->sessionClosed();
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
    //if session is not active, netierh is the interface valid.
    if (!publicConfig.isValid() ||
        (publicConfig.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active )
        return QNetworkInterface();
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
            return serviceConfig.identifier();
        } else {
            return publicConfig.identifier();
        }
    } else if (key == "SessionReferenceCount") {
        return QVariant::fromValue(sessionManager()->referenceCount(publicConfig));
    }
    return QVariant();
}

QString QNetworkSessionPrivate::bearerName() const
{
    if (!publicConfig.isValid())
        return QString();
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
            }
        }
    }
    return QString();
}

void QNetworkSessionPrivate::deviceStateChanged(quint32 devstate)
{
//    qWarning() << __FUNCTION__ << devstate << publicConfig.name()<<
//            (((publicConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) ? "Active" : "Not Active");
    updateStateFromActiveConfig();
//    QNetworkSession::State newState = QNetworkSession::Invalid;
//    switch(devstate) {
//    case NM_DEVICE_STATE_UNKNOWN: // 0
//    case NM_DEVICE_STATE_UNMANAGED: // 1
//        newState = QNetworkSession::Invalid; // 5
//        currentConnectionPath = "";
//        break;
//    case NM_DEVICE_STATE_UNAVAILABLE: // 2
//        newState = QNetworkSession::NotAvailable; // 5
//        currentConnectionPath = "";
//        break;
//    case NM_DEVICE_STATE_DISCONNECTED: // 3
//        newState = QNetworkSession::Disconnected; // 5
//        currentConnectionPath = "";
//        break;
//    case NM_DEVICE_STATE_PREPARE: // 4
//    case NM_DEVICE_STATE_CONFIG: // 5
//    case NM_DEVICE_STATE_NEED_AUTH: // 6
//    case NM_DEVICE_STATE_IP_CONFIG: // 7
//        newState = QNetworkSession::Connecting; // 2
//        break;
//    case NM_DEVICE_STATE_ACTIVATED: // 8
//        newState = QNetworkSession::Connected; // 3
//        isActive = true;
////        emit quitPendingWaitsForOpened();
////        emit q->newConfigurationActivated();
//        break;
//    case NM_DEVICE_STATE_FAILED: // 9
//        newState = QNetworkSession::NotAvailable; // 1
//        break;
//    };
//
//
//    if( newState != state) {
//        state = newState;
//        emit q->stateChanged(state);
//
//    }
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
    iface = new QNetworkManagerInterface();
    QString ok;
    if(publicConfig.isValid()) {
        if(publicConfig.identifier().contains( NM_DBUS_PATH_ACCESS_POINT)) {
            activeConfig = publicConfig;
            QString APPAth = publicConfig.identifier();
            accessPointIface = new QNetworkManagerInterfaceAccessPoint(APPAth);
            currentBearerName = "WLAN";
            accessPointIface->setConnections();
            connect(accessPointIface, SIGNAL(propertiesChanged(const QString &,QMap<QString,QVariant>)),
                    this,SLOT(propertiesChanged( const QString &, QMap<QString,QVariant>)));
            updateStateFromServiceNetwork();
        } else if(publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
            activeConfig = publicConfig;

            QString devicePath = "/org/freedesktop/Hal/devices/net_"
                                 + publicConfig.d->serviceInterface.hardwareAddress().replace(":","_").toLower();
            devIface = new QNetworkManagerInterfaceDevice( devicePath);

            currentBearerName = getBearerName(devIface->deviceType());
            currentConnectionPath = getConnectionPath();
            updateStateFromServiceNetwork();

//            deviceStateChanged(devIface->state());
            devIface->setConnections();
            connect(devIface,SIGNAL(stateChanged(const QString &, quint32)),
                    this, SLOT(updateDeviceInterfaceState(const QString&, quint32)));
        } else {
            //service network
            serviceConfig = publicConfig;
            updateStateFromServiceNetwork();

            if(publicConfig.isValid()) {
                if ((publicConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
                    deviceStateChanged(8);
                    // start time for new sessions
                }
            }
        }
        setActiveTimeStamp();
    }
}

void QNetworkSessionPrivate::activateNmSession(const QNetworkConfiguration &config)
{
    if (publicConfig == config) {
        bool ok = false;

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
                    connectionIdent = config.identifier();
                    break;
                }
            }
        } else if(publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
            connPath = getConnectionPath();
            connectionIdent = q->configuration().identifier();
        }

        interface = QNetworkInterface::interfaceFromName( publicConfig.d->serviceInterface.name()).hardwareAddress().toLower();

        if(connPath.isEmpty()) {
            qWarning() << "No known connection configuration";
            emit q->error(QNetworkSession::SessionAbortedError);
            return;
        }

        QString devicePath = "/org/freedesktop/Hal/devices/net_" + interface.replace(":","_");
        devIface = new QNetworkManagerInterfaceDevice(devicePath);
        currentBearerName = getBearerName(devIface->deviceType());

        if( devIface->state() != NM_DEVICE_STATE_ACTIVATED) {
            if(devIface->deviceType() == DEVICE_TYPE_802_11_WIRELESS) {

                devWirelessIface = new QNetworkManagerInterfaceDeviceWireless(devIface->connectionInterface()->path());
                devWirelessIface->setConnections();
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
            }
        } else {
            emit q->newConfigurationActivated();
        }
    }
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
//                triedServiceConnection = connectionPath.path().section('/', 4, 4, QString::SectionSkipEmpty).toInt();

                state = QNetworkSession::Connecting;
                emit q->stateChanged(state);

                connect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
                        this, SLOT(configChanged(QNetworkConfiguration)));

                iface = new QNetworkManagerInterface();
                iface->setConnections();
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
    } else {
        QDBusObjectPath result = reply.value();
        currentConnectionPath = result.path();
    }
}

void QNetworkSessionPrivate::deactivateNmSession(const QNetworkConfiguration &config)
{
    if (publicConfig == config) {
        QString activeConnectionPath = getActiveConnectionPath();
        if (!activeConnectionPath.isEmpty()) {
            QDBusObjectPath dbpath(activeConnectionPath);
//            QNetworkManagerInterface * iface;
//            iface = new QNetworkManagerInterface();
            iface->deactivateConnection(dbpath);
//            disconnect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
//                       this, SLOT(configChanged(QNetworkConfiguration)));
        } else {
            qWarning() <<"Could not stop. No active path";
        }
    }
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
                    isActive = (publicConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active;
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
        updateStateFromServiceNetwork();
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
    if (serviceConfig.isValid() && (config == serviceConfig || config == activeConfig)) {
        disconnect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
                   this, SLOT(configChanged(QNetworkConfiguration)));
        updateStateFromServiceNetwork();
    } else {
        updateStateFromActiveConfig();
    }
}

void QNetworkSessionPrivate::updateStateFromActiveConfig()
{
    QNetworkSession::State oldState = state;

    bool newActive = false;

    if (!activeConfig.isValid()) {
        state = QNetworkSession::Invalid;
    } else if ((activeConfig.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
        state = QNetworkSession::Connected;
        newActive = true;
    } else if ((activeConfig.state() & QNetworkConfiguration::Discovered) == QNetworkConfiguration::Discovered) {
        state = QNetworkSession::Disconnected;
    } else if ((activeConfig.state() & QNetworkConfiguration::Defined) == QNetworkConfiguration::Defined) {
        state = QNetworkSession::NotAvailable;
    } else if ((activeConfig.state() & QNetworkConfiguration::Undefined) == QNetworkConfiguration::Undefined) {
        state = QNetworkSession::NotAvailable;
    }

    bool oldActive = isActive;
    isActive = newActive;

    if (!oldActive && isActive)
        emit quitPendingWaitsForOpened();
    if (oldActive && !isActive)
        emit q->sessionClosed();

    if (oldState != state)
        emit q->stateChanged(state);
}

void QNetworkSessionPrivate::updateStateFromServiceNetwork()
{
    connect(&manager, SIGNAL(configurationChanged(QNetworkConfiguration)),
            this, SLOT(configChanged(QNetworkConfiguration)));

    foreach (const QNetworkConfiguration &config, publicConfig.children()) {
        if ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {

            QString hwAddress = config.d->serviceInterface.hardwareAddress();
            QString devicePath = "/org/freedesktop/Hal/devices/net_" + hwAddress.replace(":","_").toLower();
            QNetworkManagerInterfaceDevice *devIfaceL;
            devIfaceL = new QNetworkManagerInterfaceDevice( devicePath);
            currentBearerName = getBearerName(devIfaceL->deviceType());
            break;
        }
    }

    QNetworkSession::State oldState = state;

    foreach (const QNetworkConfiguration &config, serviceConfig.children()) {
        if ((config.state() & QNetworkConfiguration::Active) != QNetworkConfiguration::Active)
            continue;

        if (activeConfig != config) {
            activeConfig = config;
            emit q->newConfigurationActivated();
        }

        state = QNetworkSession::Connected;
        if (state != oldState)
            emit q->stateChanged(state);

        return;
    }
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

//    // No active configurations found, must be disconnected.
//    state = QNetworkSession::Disconnected;
//    if (state != oldState)
//        emit q->stateChanged(state);
}


#endif

QT_END_NAMESPACE
