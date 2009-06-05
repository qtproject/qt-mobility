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

QT_BEGIN_NAMESPACE
//typedef QMap< QString, QMap<QString,QVariant> > SettingsMap;
//1Q_DECLARE_METATYPE(SettingsMap)

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


quint64 QNetworkSessionPrivate::sentData() const
{
    QFile tx("/sys/class/net/"+q->interface().name()+"/statistics/tx_bytes");
    quint64 result = 0;
    if(tx.exists() && tx.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&tx);
        in >> result;
        tx.close();
    }
    return result;
}

quint64 QNetworkSessionPrivate::receivedData() const
{
    QFile rx("/sys/class/net/"+q->interface().name()+"/statistics/rx_bytes");
    quint64 result = 0;
    if(rx.exists() && rx.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&rx);
        in >> result;
        rx.close();
    }
    return result;
}

quint64 QNetworkSessionPrivate::activeTime() const
{
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
    if (startTime.isNull()) {
        return 0;
    }
    return startTime.secsTo(QDateTime::currentDateTime());
#else
    return 0;
#endif

}

void QNetworkSessionPrivate::syncStateWithInterface()
{
    if(!NetworkManagerAvailable()) {
        //TODO

    } else {
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
//        qWarning() << "New config session" << __FUNCTION__;
    nmDBusObj = new QNmDBusHelper;

    connect(nmDBusObj,SIGNAL(pathForStateChanged(const QString &, quint32)),
            this, SLOT(updateDeviceInterfaceState(const QString&, quint32)));

        currentBearerName == "";
        QDBusInterface iface(NM_DBUS_SERVICE,
                             NM_DBUS_PATH,
                             NM_DBUS_INTERFACE,
                             dbusConnection);
        if (!iface.isValid()) {
            qWarning() << "Could not find NetworkManager";
            return;
        }

        if(publicConfig.identifier().contains( NM_DBUS_PATH_ACCESS_POINT)) {
            QString APPAth = publicConfig.identifier();
            QDBusInterface accessPointIface(NM_DBUS_SERVICE,
                                            APPAth,
                                            NM_DBUS_INTERFACE_ACCESS_POINT,
                                            dbusConnection);
            if (accessPointIface.isValid()) {
                currentBearerName = "WLAN";
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
                                        if(dbusConnection.connect(NM_DBUS_SERVICE,
                                                                  devicePath.path(),
                                                                  NM_DBUS_INTERFACE_DEVICE,
                                                                  "StateChanged",
                                                                  nmDBusObj,SLOT(deviceStateChanged(quint32)))) {
                                        }
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
            // start time for new sessions
            startTime = QDateTime::currentDateTime();
            isActive = true;
        }
#endif
    }
}

void QNetworkSessionPrivate::open()
{
    if(!NetworkManagerAvailable()) {
        //TODO
    } else {
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
        bool ok = false;
        currentBearerName == "";
        if(publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
        QString connPath = getConnectionPath();
        if(connPath.isEmpty()) {
            qWarning() << "No known connection configuration";
            emit q->error(QNetworkSession::SessionAbortedError);
            return;
        }
        QDBusInterface iface(NM_DBUS_SERVICE,
                             NM_DBUS_PATH,
                             NM_DBUS_INTERFACE,
                             dbusConnection);
        if (!iface.isValid()) {
            qWarning() << "Could not find NetworkManager";
            emit q->error(QNetworkSession::SessionAbortedError);
            return;
        }

        QString connectionIdent = q->configuration().identifier();
        QString interface = currentInterface().hardwareAddress().toLower();
        QString devicePath = "/org/freedesktop/Hal/devices/net_" + interface.replace(":","_");

//        qWarning() << connectionIdent << interface;

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

                if(ok)
                    if(!dbusConnection.connect(NM_DBUS_SERVICE,
                                               devicePath,
                                               NM_DBUS_INTERFACE_DEVICE,
                                               "StateChanged",
                                              nmDBusObj,SLOT(deviceStateChanged(quint32)))) {
                    qWarning() << "XXXXXXXXXX dbus connect NOT successful" <<  devicePath;
                }

                if(ok) {
                    QStringList connectionServices;
                    connectionServices << NM_DBUS_SERVICE_SYSTEM_SETTINGS;
                    connectionServices << NM_DBUS_SERVICE_USER_SETTINGS;

                    QDBusObjectPath connectionPath(connPath);
                    QDBusObjectPath devicePath(devIface.path());

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
        } else if(publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {

            } else {
            // invalid type
            }
#endif
    } //end NetworkManager
}

#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
void QNetworkSessionPrivate::slotActivationFinished(QDBusPendingCallWatcher* openCall)
{
    QDBusPendingReply<QDBusObjectPath> reply = *openCall;
    if (reply.isError()) {
        emit q->error(QNetworkSession::UnknownSessionError);
    } else {
        QDBusObjectPath result = reply.value();
        currentConnectionPath = result.path();
    }
}
#endif

void QNetworkSessionPrivate::close()
{
    if(!NetworkManagerAvailable()) {
    //TODO

    } else {
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
        stop();
#endif
    }
}

void QNetworkSessionPrivate::stop()
{
    if(!NetworkManagerAvailable()) {
        //TODO
    } else {
#if !defined(QT_NO_DBUS) && !defined(Q_OS_MAC)
        QString activeConnectionPath = getActiveConnectionPath();
        if (!activeConnectionPath.isEmpty()) {
            QDBusObjectPath dbpath(activeConnectionPath);
            QDBusInterface iface(NM_DBUS_SERVICE,
                                 NM_DBUS_PATH,
                                 NM_DBUS_INTERFACE,
                                 dbusConnection);
            if (!iface.isValid()) {
                emit q->error(QNetworkSession::UnknownSessionError);
                return;
            }
            iface.call("DeactivateConnection", QVariant::fromValue(dbpath));
        } else {
            qWarning() <<"Could not stop. No active path";
        }
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
    QDBusInterface iface(NM_DBUS_SERVICE,
                         NM_DBUS_PATH,
                         NM_DBUS_INTERFACE,
                         dbusConnection); //NetworkManager interface
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
                                return QNetworkInterface::interfaceFromName( devIface.property("Interface").toString());
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

                                            if( accessPointIface.path() == q->configuration().identifier() /*accessPointIface.property("Ssid").toString()*/) {
                                                return QNetworkInterface::interfaceFromName( devIface.property("Interface").toString());
                                            }
                                            i++;
                                        }
                                    }
                                }
                            }
                        }
                    } else if(devIface.property("DeviceType").toInt() == DEVICE_TYPE_802_3_ETHERNET) { //wifi interface
                        if( q->configuration().name() == devIface.property("Interface").toString()) {
                            return QNetworkInterface::interfaceFromName( devIface.property("Interface").toString());
                        }
                    }
                }
            }
        }
    }
#endif
    return QNetworkInterface();
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
    QString interface = currentInterface().hardwareAddress().toLower().replace(":","_");
        QDBusInterface iface(NM_DBUS_SERVICE,
                             NM_DBUS_PATH,
                             NM_DBUS_INTERFACE,
                             dbusConnection);
        if (!iface.isValid()) {
            qWarning() << "Could not find NetworkManager";
            return "";
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
                    if( str.contains(interface)) {
                        return path.path();
                    }
                }
            }
        }
    return QString();
}

QString QNetworkSessionPrivate::getConnectionPath()
{
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
                        QDBusMessage r = sysIface.call("GetSettings");
                        QDBusReply< SettingsMap > rep = sysIface.call("GetSettings");

                        QMap< QString, QMap<QString,QVariant> > map = rep.value();
                        QList<QString> list = map.keys();

                        foreach (QString key, list) {
                            QMap<QString,QVariant> innerMap = map[key];
//                                       qWarning() << "       Key: " << key;
                            QMap<QString,QVariant>::const_iterator i = innerMap.constBegin();

                            while (i != innerMap.constEnd()) {
                                QString k = i.key();
  //                   qWarning() << "          Key: " << k << " Entry: " << i.value();
                                if(k == "id") {
//                                    qWarning() << q->configuration().name();
                                    if(i.value().toString() == q->configuration().name()) {
//                                             qWarning() <<"Connection path"<< path.path();
//                                             qWarning() <<"XXXXXXXXXX" << i.value().toString();
                                        connPath = path.path();
                                        return connPath;
                                    }
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
//        qWarning() << "        emit quitPendingWaitsForOpened()" << publicConfig.identifier();
        emit quitPendingWaitsForOpened();
        emit q->newConfigurationActivated();
        break;
    case NM_DEVICE_STATE_FAILED: // 9
        emit q->error(QNetworkSession::SessionAbortedError);
        newState = QNetworkSession::NotAvailable; // 1
        isActive = false;
//        newState = QNetworkSession::Closing; // 4
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

void QNetworkSessionPrivate::updateDeviceInterfaceState(const QString &path, quint32 nmState)
{
    Q_UNUSED(path);
//   qWarning() << __FUNCTION__ << path << nmState << publicConfig.identifier();
   deviceStateChanged(nmState);
}

#endif

QT_END_NAMESPACE
