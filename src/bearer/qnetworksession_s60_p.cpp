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

#include <qmobilityglobal.h>
#include "qnetworksession_s60_p.h"
#include "qnetworkconfiguration_s60_p.h"
#include "qnetworkconfigmanager_s60_p.h"
#include <es_enum.h>
#include <es_sock.h>
#include <in_sock.h>
#include <stdapis/sys/socket.h>
#include <stdapis/net/if.h>

QTM_BEGIN_NAMESPACE

QNetworkSessionPrivate::QNetworkSessionPrivate()
    : CActive(CActive::EPriorityUserInput), state(QNetworkSession::Invalid),
      isOpen(false), iDynamicUnSetdefaultif(0), ipConnectionNotifier(0),
      iHandleStateNotificationsFromManager(false), iFirstSync(true), iStoppedByUser(false),
      iClosedByUser(false), iDeprecatedConnectionId(0), iError(QNetworkSession::UnknownSessionError),
      iALREnabled(0), iConnectInBackground(false)
{
    CActiveScheduler::Add(this);

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    iMobility = NULL;
#endif
    // Try to load "Open C" dll dynamically and
    // try to attach to unsetdefaultif function dynamically.
    // This is to avoid build breaks with old OpenC versions.
    if (iOpenCLibrary.Load(_L("libc")) == KErrNone) {
        iDynamicUnSetdefaultif = (TOpenCUnSetdefaultifFunction)iOpenCLibrary.Lookup(597);
    }
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
    qDebug() << "QNS this : " << QString::number((uint)this) << " - ";
    if (iDynamicUnSetdefaultif)
        qDebug() << "dynamic setdefaultif() resolution succeeded. ";
    else
        qDebug() << "dynamic setdefaultif() resolution failed. ";
#endif

    TRAP_IGNORE(iConnectionMonitor.ConnectL());
}

QNetworkSessionPrivate::~QNetworkSessionPrivate()
{
    isOpen = false;

    // Cancel Connection Progress Notifications first.
    // Note: ConnectionNotifier must be destroyed before Canceling RConnection::Start()
    //       => deleting ipConnectionNotifier results RConnection::CancelProgressNotification()
    delete ipConnectionNotifier;
    ipConnectionNotifier = NULL;

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    if (iMobility) {
        delete iMobility;
        iMobility = NULL;
    }
#endif

    // Cancel possible RConnection::Start()
    Cancel();
    iSocketServ.Close();
    
    // Close global 'Open C' RConnection
    // Clears also possible unsetdefaultif() flags.
    setdefaultif(0);
    
    iConnectionMonitor.Close();
    iOpenCLibrary.Close();
}

void QNetworkSessionPrivate::configurationStateChanged(TUint32 accessPointId, TUint32 connMonId, QNetworkSession::State newState)
{
    if (iHandleStateNotificationsFromManager) {
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
        qDebug() << "QNS this : " << QString::number((uint)this) << " - "
                << "configurationStateChanged from manager for IAP : " << QString::number(accessPointId)
                << "configurationStateChanged connMon ID : " << QString::number(connMonId)
                << " : to a state: " << newState
                << " whereas my current state is: " << state;
#endif
        if (connMonId == iDeprecatedConnectionId) {
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
            qDebug() << "QNS this : " << QString::number((uint)this) << " - "
                    << "however status update from manager ignored because it related to already closed connection.";
#endif
            return;
        }
        this->newState(newState, accessPointId);
    }
}

void QNetworkSessionPrivate::configurationRemoved(const QNetworkConfiguration& config)
{
    if (!publicConfig.d.data()) {
        return;
    }
    if (config.d.data()->numericId == publicConfig.d.data()->numericId) {
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
        qDebug() << "QNS this : " << QString::number((uint)this) << " - "
                 << "configurationRemoved IAP: " << QString::number(publicConfig.d.data()->numericId) << " : going to State: Invalid";
#endif
        this->newState(QNetworkSession::Invalid, publicConfig.d.data()->numericId);
    }
}

void QNetworkSessionPrivate::syncStateWithInterface()
{
    if (!publicConfig.d) {
        return;
    }

    if (iFirstSync && publicConfig.d.data()) {
        QObject::connect(((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager), SIGNAL(configurationStateChanged(TUint32, TUint32, QNetworkSession::State)),
                         this, SLOT(configurationStateChanged(TUint32, TUint32, QNetworkSession::State)));
        // Listen to configuration removals, so that in case the configuration
        // this session is based on is removed, session knows to enter Invalid -state.
        QObject::connect(((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager),
                         SIGNAL(configurationRemoved(QNetworkConfiguration)),
                         this, SLOT(configurationRemoved(QNetworkConfiguration)));
    }
    // Start listening IAP state changes from QNetworkConfigurationManagerPrivate
    iHandleStateNotificationsFromManager = true;    

    // Check open connections to see if there is already
    // an open connection to selected IAP or SNAP
    TUint count;
    TRequestStatus status;
    iConnectionMonitor.GetConnectionCount(count, status);
    User::WaitForRequest(status);
    if (status.Int() != KErrNone) {
        return;
    }

    TUint numSubConnections;
    TUint connectionId;
    for (TUint i = 1; i <= count; i++) {
        TInt ret = iConnectionMonitor.GetConnectionInfo(i, connectionId, numSubConnections);
        if (ret == KErrNone) {
            TUint apId;
            iConnectionMonitor.GetUintAttribute(connectionId, 0, KIAPId, apId, status);
            User::WaitForRequest(status);
            if (status.Int() == KErrNone) {
                TInt connectionStatus;
                iConnectionMonitor.GetIntAttribute(connectionId, 0, KConnectionStatus, connectionStatus, status);
                User::WaitForRequest(status);
                if (connectionStatus == KLinkLayerOpen) {
                    if (state != QNetworkSession::Closing) {
                        if (newState(QNetworkSession::Connected, apId)) {
                            return;
                        }
                    }
                }
            }
        }
    }

    if (state != QNetworkSession::Connected) {
        if ((publicConfig.d.data()->state & QNetworkConfiguration::Discovered) ==
            QNetworkConfiguration::Discovered) {
            newState(QNetworkSession::Disconnected);
        } else {
            newState(QNetworkSession::NotAvailable);
        }
    }
}

QNetworkInterface QNetworkSessionPrivate::interface(TUint iapId) const
{
    QString interfaceName;

    TSoInetInterfaceInfo ifinfo;
    TPckg<TSoInetInterfaceInfo> ifinfopkg(ifinfo);
    TSoInetIfQuery ifquery;
    TPckg<TSoInetIfQuery> ifquerypkg(ifquery);
 
    // Open dummy socket for interface queries
    RSocket socket;
    TInt retVal = socket.Open(iSocketServ, _L("udp"));
    if (retVal != KErrNone) {
        return QNetworkInterface();
    }
 
    // Start enumerating interfaces
    socket.SetOpt(KSoInetEnumInterfaces, KSolInetIfCtrl);
    while(socket.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, ifinfopkg) == KErrNone) {
        ifquery.iName = ifinfo.iName;
        TInt err = socket.GetOpt(KSoInetIfQueryByName, KSolInetIfQuery, ifquerypkg);
        if(err == KErrNone && ifquery.iZone[1] == iapId) { // IAP ID is index 1 of iZone
            if(ifinfo.iAddress.Address() > 0) {
                interfaceName = QString::fromUtf16(ifinfo.iName.Ptr(),ifinfo.iName.Length());
                break;
            }
        }
    }
 
    socket.Close();
 
    if (interfaceName.isEmpty()) {
        return QNetworkInterface();
    }
 
    return QNetworkInterface::interfaceFromName(interfaceName);
}

QNetworkInterface QNetworkSessionPrivate::currentInterface() const
{
    if (!publicConfig.isValid() || state != QNetworkSession::Connected) {
        return QNetworkInterface();
    }
    
    return activeInterface;
}

QVariant QNetworkSessionPrivate::sessionProperty(const QString& key) const
{
    if (key == "ConnectInBackground") {
        return QVariant(iConnectInBackground);
    }
    return QVariant();
}

void QNetworkSessionPrivate::setSessionProperty(const QString& key, const QVariant& value)
{
    // Valid value means adding property, invalid means removing it.
    if (key == "ConnectInBackground") {
        if (value.isValid()) {
            iConnectInBackground = value.toBool();
        } else {
            iConnectInBackground = EFalse;
        }
    }
}

QString QNetworkSessionPrivate::errorString() const
{
    switch (iError) {
    case QNetworkSession::UnknownSessionError:
        return tr("Unknown session error.");
    case QNetworkSession::SessionAbortedError:
        return tr("The session was aborted by the user or system.");
    case QNetworkSession::OperationNotSupportedError:
        return tr("The requested operation is not supported by the system.");
    case QNetworkSession::InvalidConfigurationError:
        return tr("The specified configuration cannot be used.");
    case QNetworkSession::RoamingError:
        return tr("Roaming was aborted or is not possible.");
    }
 
    return QString();
}

QNetworkSession::SessionError QNetworkSessionPrivate::error() const
{
    return iError;
}

void QNetworkSessionPrivate::open()
{
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
        qDebug() << "QNS this : " << QString::number((uint)this) << " - "
                << "open() called, session state is: " << state << " and isOpen is: "
                << isOpen;
#endif
    if (isOpen || (state == QNetworkSession::Connecting)) {
        return;
    }
    
    // Stop handling IAP state change signals from QNetworkConfigurationManagerPrivate
    // => RConnection::ProgressNotification will be used for IAP/SNAP monitoring
    iHandleStateNotificationsFromManager = false;

    // Configuration may have been invalidated after session creation by platform
    // (e.g. configuration has been deleted).
    if (!publicConfig.isValid()) {
        newState(QNetworkSession::Invalid);
        iError = QNetworkSession::InvalidConfigurationError;
        emit q->error(iError);
        return;
    }
    // If opening a undefined configuration, session emits error and enters
    // NotAvailable -state. Note that we will try ones in 'defined' state to avoid excessive
    // need for WLAN scans (via updateConfigurations()), because user may have walked
    // into a WLAN range, but periodic background scan has not occurred yet -->
    // we don't want to force application to make frequent updateConfigurations() calls
    // to be able to try if e.g. home WLAN is available.
    if (publicConfig.state() == QNetworkConfiguration::Undefined) {
        newState(QNetworkSession::NotAvailable);
        iError = QNetworkSession::InvalidConfigurationError;
        emit q->error(iError);
        return;
    }
    // Clear possible previous states
    iStoppedByUser = false;
    iClosedByUser = false;
    iDeprecatedConnectionId = 0;

    TInt error = iSocketServ.Connect();
    if (error != KErrNone) {
        // Could not open RSocketServ
        newState(QNetworkSession::Invalid);
        iError = QNetworkSession::UnknownSessionError;
        emit q->error(iError);
        syncStateWithInterface();    
        return;
    }
    
    error = iConnection.Open(iSocketServ);
    if (error != KErrNone) {
        // Could not open RConnection
        iSocketServ.Close();
        newState(QNetworkSession::Invalid);
        iError = QNetworkSession::UnknownSessionError;
        emit q->error(iError);
        syncStateWithInterface();    
        return;
    }
    
    // Use RConnection::ProgressNotification for IAP/SNAP monitoring
    // (<=> ConnectionProgressNotifier uses RConnection::ProgressNotification)
    if (!ipConnectionNotifier) {
        ipConnectionNotifier = new ConnectionProgressNotifier(*this,iConnection);
    }
    if (ipConnectionNotifier) {
        ipConnectionNotifier->StartNotifications();
    }
    
    if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
#ifdef OCC_FUNCTIONALITY_AVAILABLE
            // With One Click Connectivity (Symbian^3 onwards) it is possible
            // to connect silently, without any popups.
            TConnPrefList pref;
            TExtendedConnPref prefs;
            prefs.SetIapId(publicConfig.d.data()->numericId);
            if (iConnectInBackground) {
                prefs.SetNoteBehaviour( TExtendedConnPref::ENoteBehaviourConnSilent );
            }
            pref.AppendL(&prefs);
#else
            TCommDbConnPref pref;
            pref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
            pref.SetIapId(publicConfig.d.data()->numericId);
#endif
            iConnection.Start(pref, iStatus);
            if (!IsActive()) {
                SetActive();
            }
            newState(QNetworkSession::Connecting);
    } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
#ifdef OCC_FUNCTIONALITY_AVAILABLE
        // On Symbian^3 if service network is not reachable, it triggers a UI (aka EasyWLAN) where
        // user can create new IAPs. To detect this, we need to store the number of IAPs
        // there was before connection was started.
        iKnownConfigsBeforeConnectionStart = ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->accessPointConfigurations.keys();
        TConnPrefList snapPref;
        TExtendedConnPref prefs;
        prefs.SetSnapId(publicConfig.d.data()->numericId);
        if (iConnectInBackground) {
            prefs.SetNoteBehaviour( TExtendedConnPref::ENoteBehaviourConnSilent );
        }
        snapPref.AppendL(&prefs);
#else
        TConnSnapPref snapPref(publicConfig.d.data()->numericId);
#endif
        iConnection.Start(snapPref, iStatus);
        if (!IsActive()) {
            SetActive();
        }
        newState(QNetworkSession::Connecting);
    } else if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
        iKnownConfigsBeforeConnectionStart = ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->accessPointConfigurations.keys();
        iConnection.Start(iStatus);
        if (!IsActive()) {
            SetActive();
        }
        newState(QNetworkSession::Connecting);
    }
 
    if (error != KErrNone) {
        isOpen = false;
        iError = QNetworkSession::UnknownSessionError;
        emit q->error(iError);
        if (ipConnectionNotifier) {
            ipConnectionNotifier->StopNotifications();
        }
        syncStateWithInterface();    
    }
}

TUint QNetworkSessionPrivate::iapClientCount(TUint aIAPId) const
{
    TRequestStatus status;
    TUint connectionCount;
    iConnectionMonitor.GetConnectionCount(connectionCount, status);
    User::WaitForRequest(status);
    if (status.Int() == KErrNone) {
        for (TUint i = 1; i <= connectionCount; i++) {
            TUint connectionId;
            TUint subConnectionCount;
            iConnectionMonitor.GetConnectionInfo(i, connectionId, subConnectionCount);
            TUint apId;
            iConnectionMonitor.GetUintAttribute(connectionId, subConnectionCount, KIAPId, apId, status);
            User::WaitForRequest(status);
            if (apId == aIAPId) {
                TConnMonClientEnumBuf buf;
                iConnectionMonitor.GetPckgAttribute(connectionId, 0, KClientInfo, buf, status);
                User::WaitForRequest(status);
                if (status.Int() == KErrNone) {
                    return buf().iCount;
                }
            }
        }
    }
    return 0;
}

void QNetworkSessionPrivate::close(bool allowSignals)
{
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
    qDebug() << "QNS this : " << QString::number((uint)this) << " - "
            << "close() called, session state is: " << state << " and isOpen is : "
            << isOpen;
#endif

    if (!isOpen && state != QNetworkSession::Connecting) {
        return;
    }
    // Mark this session as closed-by-user so that we are able to report
    // distinguish between stop() and close() state transitions
    // when reporting.
    iClosedByUser = true;
    isOpen = false;

#ifndef OCC_FUNCTIONALITY_AVAILABLE
    // On Symbian^3 we need to keep track of active configuration longer
    // in case of empty-SNAP-triggered EasyWLAN.
    activeConfig = QNetworkConfiguration();
#endif
    serviceConfig = QNetworkConfiguration();
    
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    if (iMobility) {
        delete iMobility;
        iMobility = NULL;
    }
#endif

    if (ipConnectionNotifier && !iHandleStateNotificationsFromManager) {
        ipConnectionNotifier->StopNotifications();
        // Start handling IAP state change signals from QNetworkConfigurationManagerPrivate
        iHandleStateNotificationsFromManager = true;
    }
    
    Cancel(); // closes iConnection
    iSocketServ.Close();
    
    // Close global 'Open C' RConnection. If OpenC supports,
    // close the defaultif for good to avoid difficult timing
    // and bouncing issues of network going immediately back up
    //  because of e.g. select() thread etc.
    if (iDynamicUnSetdefaultif) {
        iDynamicUnSetdefaultif();
    } else {
        setdefaultif(0);
    }

    // If UserChoice, go down immediately. If some other configuration,
    // go down immediately if there is no reports expected from the platform;
    // in practice Connection Monitor is aware of connections only after
    // KFinishedSelection event, and hence reports only after that event, but
    // that does not seem to be trusted on all Symbian versions --> safest
    // to go down.
    if (publicConfig.type() == QNetworkConfiguration::UserChoice || state == QNetworkSession::Connecting) {
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
    qDebug() << "QNS this : " << QString::number((uint)this) << " - "
            << "going Disconnected right away. Deprecating connection monitor ID: " << publicConfig.d.data()->connectionId;
#endif

        // The connection has gone down, and processing of status updates must be
        // stopped. Depending on platform, there may come 'connecting/connected' states
        // considerably later (almost a second). Connection id is an increasing
        // number, so this does not affect next _real_ 'conneting/connected' states.
        iDeprecatedConnectionId = publicConfig.d.data()->connectionId;
        newState(QNetworkSession::Closing);
        newState(QNetworkSession::Disconnected);
    }
    if (allowSignals) {
        emit q->closed();
    }
}

void QNetworkSessionPrivate::stop()
{
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
    qDebug() << "QNS this : " << QString::number((uint)this) << " - "
            << "stop() called, session state is: " << state << " and isOpen is : "
            << isOpen;
#endif
    if (!isOpen &&
        publicConfig.isValid() &&
        publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
    qDebug() << "QNS this : " << QString::number((uint)this) << " - "
            << "since session is not open, using RConnectionMonitor to stop() the interface";
#endif
        iStoppedByUser = true;
        // If the publicConfig is type of IAP, enumerate through connections at
        // connection monitor. If publicConfig is active in that list, stop it.
        // Otherwise there is nothing to stop. Note: because this QNetworkSession is not open,
        // activeConfig is not usable.
        TUint count;
        TRequestStatus status;
        iConnectionMonitor.GetConnectionCount(count, status);
        User::WaitForRequest(status);
        if (status.Int() != KErrNone) {
            return;
        }
        TUint numSubConnections; // Not used but needed by GetConnectionInfo i/f
        TUint connectionId;
        for (TUint i = 1; i <= count; ++i) {
            // Get (connection monitor's assigned) connection ID
            TInt ret = iConnectionMonitor.GetConnectionInfo(i, connectionId, numSubConnections);            
            if (ret == KErrNone) {
                // See if connection Id matches with our Id. If so, stop() it.
                if (publicConfig.d.data()->connectionId == connectionId) {
                    ret = iConnectionMonitor.SetBoolAttribute(connectionId,
                                                              0, // subConnectionId don't care
                                                              KConnectionStop,
                                                              ETrue);
                }
            }
            // Enter disconnected state right away since the session is not even open.
            // Symbian^3 connection monitor does not emit KLinkLayerClosed when
            // connection is stopped via connection monitor.
            newState(QNetworkSession::Disconnected);
        }
    } else if (isOpen) {
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
    qDebug() << "QNS this : " << QString::number((uint)this) << " - "
            << "since session is open, using RConnection to stop() the interface";
#endif
        // Since we are open, use RConnection to stop the interface
        isOpen = false;
        iStoppedByUser = true;
        newState(QNetworkSession::Closing);
        if (ipConnectionNotifier) {
            ipConnectionNotifier->StopNotifications();
            // Start handling IAP state change signals from QNetworkConfigurationManagerPrivate
            iHandleStateNotificationsFromManager = true;
        }
        iConnection.Stop(RConnection::EStopAuthoritative);
        isOpen = true;
        close(false);
        emit q->closed();
    }
}

void QNetworkSessionPrivate::migrate()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    if (iMobility) {
        // Close global 'Open C' RConnection. If openC supports, use the 'heavy'
        // version to block all subsequent requests.
        if (iDynamicUnSetdefaultif) {
            iDynamicUnSetdefaultif();
        } else {
            setdefaultif(0);
        }
        // Start migrating to new IAP
        iMobility->MigrateToPreferredCarrier();
    }
#endif
}

void QNetworkSessionPrivate::ignore()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    if (iMobility) {
        iMobility->IgnorePreferredCarrier();

        if (!iALRUpgradingConnection) {
            newState(QNetworkSession::Disconnected);
        } else {
            newState(QNetworkSession::Connected,iOldRoamingIap);
        }
    }
#endif
}

void QNetworkSessionPrivate::accept()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    if (iMobility) {
        iMobility->NewCarrierAccepted();

        QNetworkConfiguration newActiveConfig = activeConfiguration(iNewRoamingIap);

        // Use name of the new IAP to open global 'Open C' RConnection
        QByteArray nameAsByteArray = newActiveConfig.name().toUtf8();
        ifreq ifr;
        memset(&ifr, 0, sizeof(struct ifreq));
        strcpy(ifr.ifr_name, nameAsByteArray.constData());
        setdefaultif(&ifr);

        newState(QNetworkSession::Connected, iNewRoamingIap);
    }
#endif
}

void QNetworkSessionPrivate::reject()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    if (iMobility) {
        iMobility->NewCarrierRejected();

        if (!iALRUpgradingConnection) {
            newState(QNetworkSession::Disconnected);
        } else {
            QNetworkConfiguration newActiveConfig = activeConfiguration(iOldRoamingIap);

            // Use name of the old IAP to open global 'Open C' RConnection
            QByteArray nameAsByteArray = newActiveConfig.name().toUtf8();
            ifreq ifr;
            memset(&ifr, 0, sizeof(struct ifreq));
            strcpy(ifr.ifr_name, nameAsByteArray.constData());
            setdefaultif(&ifr);

            newState(QNetworkSession::Connected, iOldRoamingIap);
        }
    }
#endif
}

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
void QNetworkSessionPrivate::PreferredCarrierAvailable(TAccessPointInfo aOldAPInfo,
                                                       TAccessPointInfo aNewAPInfo,
                                                       TBool aIsUpgrade,
                                                       TBool aIsSeamless)
{
    iOldRoamingIap = aOldAPInfo.AccessPoint();
    iNewRoamingIap = aNewAPInfo.AccessPoint();
    newState(QNetworkSession::Roaming);
    if (iALREnabled > 0) {
        iALRUpgradingConnection = aIsUpgrade;
        QList<QNetworkConfiguration> configs = publicConfig.children();
        for (int i=0; i < configs.count(); i++) {
            if (configs[i].d.data()->numericId == aNewAPInfo.AccessPoint()) {
                // Any slot connected to the signal might throw an std::exception,
                // which must not propagate into Symbian code (this function is a callback
                // from platform). We could convert exception to a symbian Leave, but since the
                // prototype of this function bans this (no trailing 'L'), we just catch
                // and drop.
                QT_TRY {
                    emit q->preferredConfigurationChanged(configs[i],aIsSeamless);
                }
                QT_CATCH (std::exception&) {}
            }
        }
    } else {
        migrate();
    }
}

void QNetworkSessionPrivate::NewCarrierActive(TAccessPointInfo /*aNewAPInfo*/, TBool /*aIsSeamless*/)
{
    if (iALREnabled > 0) {
        QT_TRY {
            emit q->newConfigurationActivated();
        }
        QT_CATCH (std::exception&) {}
    } else {
        accept();
    }
}

void QNetworkSessionPrivate::Error(TInt /*aError*/)
{
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
    qDebug() << "QNS this : " << QString::number((uint)this) << " - "
            << "roaming Error() occured, isOpen is: " << isOpen;
#endif
    if (isOpen) {
        isOpen = false;
        activeConfig = QNetworkConfiguration();
        serviceConfig = QNetworkConfiguration();
        iError = QNetworkSession::RoamingError;
        emit q->error(iError);
        Cancel();
        if (ipConnectionNotifier) {
            ipConnectionNotifier->StopNotifications();
        }
        QT_TRY {
            syncStateWithInterface();
            // In some cases IAP is still in Connected state when
            // syncStateWithInterface(); is called
            // => Following call makes sure that Session state
            //    changes immediately to Disconnected.
            newState(QNetworkSession::Disconnected);
            emit q->closed();
        }
        QT_CATCH (std::exception&) {}
    } else if (iStoppedByUser) {
        // If the user of this session has called the stop() and
        // configuration is based on internet SNAP, this needs to be
        // done here because platform might roam.
        QT_TRY {
            newState(QNetworkSession::Disconnected);
        }
        QT_CATCH (std::exception&) {}
    }
}
#endif

void QNetworkSessionPrivate::setALREnabled(bool enabled)
{
    if (enabled) {
        iALREnabled++;
    } else {
        iALREnabled--;
    }
}

QNetworkConfiguration QNetworkSessionPrivate::bestConfigFromSNAP(const QNetworkConfiguration& snapConfig) const
{
    QNetworkConfiguration config;
    QList<QNetworkConfiguration> subConfigurations = snapConfig.children();
    for (int i = 0; i < subConfigurations.count(); i++ ) {
        if (subConfigurations[i].state() == QNetworkConfiguration::Active) {
            config = subConfigurations[i];
            break;
        } else if (!config.isValid() && subConfigurations[i].state() == QNetworkConfiguration::Discovered) {
            config = subConfigurations[i];
        }
    }
    if (!config.isValid() && subConfigurations.count() > 0) {
        config = subConfigurations[0];
    }
    return config;
}

quint64 QNetworkSessionPrivate::bytesWritten() const
{
    return transferredData(KUplinkData);
}

quint64 QNetworkSessionPrivate::bytesReceived() const
{
    return transferredData(KDownlinkData);
}

quint64 QNetworkSessionPrivate::transferredData(TUint dataType) const
{
    if (!publicConfig.isValid()) {
        return 0;
    }
    
    QNetworkConfiguration config;
    if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
        if (serviceConfig.isValid()) {
            config = serviceConfig;
        } else {
            if (activeConfig.isValid()) {
                config = activeConfig;
            }
        }
    } else {
        config = publicConfig;
    }

    if (!config.isValid()) {
        return 0;
    }
    
    TUint count;
    TRequestStatus status;
    iConnectionMonitor.GetConnectionCount(count, status);
    User::WaitForRequest(status);
    if (status.Int() != KErrNone) {
        return 0;
    }
    
    TUint transferredData = 0;
    TUint numSubConnections;
    TUint connectionId;
    bool configFound;
    for (TUint i = 1; i <= count; i++) {
        TInt ret = iConnectionMonitor.GetConnectionInfo(i, connectionId, numSubConnections);
        if (ret == KErrNone) {
            TUint apId;
            iConnectionMonitor.GetUintAttribute(connectionId, 0, KIAPId, apId, status);
            User::WaitForRequest(status);
            if (status.Int() == KErrNone) {
                configFound = false;
                if (config.type() == QNetworkConfiguration::ServiceNetwork) {
                    QList<QNetworkConfiguration> configs = config.children();
                    for (int i=0; i < configs.count(); i++) {
                        if (configs[i].d.data()->numericId == apId) {
                            configFound = true;
                            break;
                        }
                    }
                } else if (config.d.data()->numericId == apId) {
                    configFound = true;
                }
                if (configFound) {
                    TUint tData;
                    iConnectionMonitor.GetUintAttribute(connectionId, 0, dataType, tData, status );
                    User::WaitForRequest(status);
                    if (status.Int() == KErrNone) {
                    transferredData += tData;
                    }
                }
            }
        }
    }
    
    return transferredData;
}

quint64 QNetworkSessionPrivate::activeTime() const
{
    if (!isOpen || startTime.isNull()) {
        return 0;
    }
    return startTime.secsTo(QDateTime::currentDateTime());
}

QNetworkConfiguration QNetworkSessionPrivate::activeConfiguration(TUint32 iapId) const
{
    if (iapId == 0) {
        _LIT(KSetting, "IAP\\Id");
        iConnection.GetIntSetting(KSetting, iapId);
    }

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        // Try to search IAP from the used SNAP using IAP Id
        QList<QNetworkConfiguration> children = publicConfig.children();
        for (int i=0; i < children.count(); i++) {
            if (children[i].d.data()->numericId == iapId) {
                return children[i];
            }
        }

        // Given IAP Id was not found from the used SNAP
        // => Try to search matching IAP using mappingName
        //    mappingName contains:
        //      1. "Access point name" for "Packet data" Bearer
        //      2. "WLAN network name" (= SSID) for "Wireless LAN" Bearer
        //      3. "Dial-up number" for "Data call Bearer" or "High Speed (GSM)" Bearer
        //    <=> Note: It's possible that in this case reported IAP is
        //              clone of the one of the IAPs of the used SNAP
        //              => If mappingName matches, clone has been found
        QNetworkConfiguration pt;
        pt.d = ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->accessPointConfigurations.value(QT_BEARERMGMT_CONFIGURATION_IAP_PREFIX+QString::number(qHash(iapId)));
        if (pt.d) {
            for (int i=0; i < children.count(); i++) {
                if (children[i].d.data()->mappingName == pt.d.data()->mappingName) {
                    return children[i];
                }
            }
        } else {
#ifdef OCC_FUNCTIONALITY_AVAILABLE
            // On Symbian^3 (only, not earlier or Symbian^4) if the SNAP was not reachable, it triggers
            // user choice type of activity (EasyWLAN). As a result, a new IAP may be created, and
            // hence if was not found yet. Therefore update configurations and see if there is something new.
            // 1. Update knowledge from the databases.
            ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->updateConfigurations();
            // 2. Check if new configuration was created during connection creation
            QList<QString> knownConfigs = ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->accessPointConfigurations.keys();
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
            qDebug() << "QNS this : " << QString::number((uint)this) << " - "
                    << "opened configuration was not known beforehand, looking for new.";
#endif
            if (knownConfigs.count() > iKnownConfigsBeforeConnectionStart.count()) {
                // Configuration count increased => new configuration was created
                // => Search new, created configuration
                QString newIapId;
                for (int i=0; i < iKnownConfigsBeforeConnectionStart.count(); i++) {
                    if (knownConfigs[i] != iKnownConfigsBeforeConnectionStart[i]) {
                        newIapId = knownConfigs[i];
                        break;
                    }
                }
                if (newIapId.isEmpty()) {
                    newIapId = knownConfigs[knownConfigs.count()-1];
                }
                pt.d = ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->accessPointConfigurations.value(newIapId);
                if (pt.d) {
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
                    qDebug() << "QNS this : " << QString::number((uint)this) << " - "
                            << "new configuration was found, name, IAP id: " << pt.name() << pt.identifier();
#endif
                    return pt;
                }
            }
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
            qDebug() << "QNS this : " << QString::number((uint)this) << " - "
                    << "configuration was not found, returning invalid.";
#endif
#endif // OCC_FUNCTIONALITY_AVAILABLE
            // Given IAP Id was not found from known IAPs array
            return QNetworkConfiguration();
        }
        // Matching IAP was not found from used SNAP
        // => IAP from another SNAP is returned
        //    (Note: Returned IAP matches to given IAP Id)
        return pt;
    }
#endif
    if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
        if (publicConfig.d.data()->manager) {
            QNetworkConfiguration pt;
            // Try to found User Selected IAP from known IAPs (accessPointConfigurations)
            pt.d = ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->accessPointConfigurations.value(QT_BEARERMGMT_CONFIGURATION_IAP_PREFIX+QString::number(qHash(iapId)));
            if (pt.d) {
                return pt;
            } else {
                // Check if new (WLAN) IAP was created in IAP/SNAP dialog
                // 1. Sync internal configurations array to commsdb first
                ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->updateConfigurations();
                // 2. Check if new configuration was created during connection creation
                QList<QString> knownConfigs = ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->accessPointConfigurations.keys();
                if (knownConfigs.count() > iKnownConfigsBeforeConnectionStart.count()) {
                    // Configuration count increased => new configuration was created
                    // => Search new, created configuration
                    QString newIapId;
                    for (int i=0; i < iKnownConfigsBeforeConnectionStart.count(); i++) {
                        if (knownConfigs[i] != iKnownConfigsBeforeConnectionStart[i]) {
                            newIapId = knownConfigs[i];
                            break;
                        }
                    }
                    if (newIapId.isEmpty()) {
                        newIapId = knownConfigs[knownConfigs.count()-1];
                    }
                    pt.d = ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->accessPointConfigurations.value(newIapId);
                    if (pt.d) {
                        return pt;
                    }
                }
            }
        }
        return QNetworkConfiguration();
    }

    return publicConfig;
}

void QNetworkSessionPrivate::RunL()
{
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
    qDebug() << "QNS this : " << QString::number((uint)this) << " - "
            << "RConnection::RunL with status code: " << iStatus.Int();
#endif
    TInt statusCode = iStatus.Int();

    switch (statusCode) {
        case KErrNone: // Connection created successfully
            {
            TInt error = KErrNone;
            QNetworkConfiguration newActiveConfig = activeConfiguration();
            if (!newActiveConfig.isValid()) {
                error = KErrGeneral;
            } else {
                // Use name of the IAP to open global 'Open C' RConnection
                ifreq ifr;
                memset(&ifr, 0, sizeof(struct ifreq));
                QByteArray nameAsByteArray = newActiveConfig.name().toUtf8();
                strcpy(ifr.ifr_name, nameAsByteArray.constData());
                error = setdefaultif(&ifr);
            }
            
            if (error != KErrNone) {
                isOpen = false;
                iError = QNetworkSession::UnknownSessionError;
                QT_TRYCATCH_LEAVING(emit q->error(iError));
                Cancel();
                if (ipConnectionNotifier) {
                    ipConnectionNotifier->StopNotifications();
                }
                QT_TRYCATCH_LEAVING(syncStateWithInterface());
                return;
            }
 
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
            if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
                // Activate ALR monitoring
                iMobility = CActiveCommsMobilityApiExt::NewL(iConnection, *this);
            }
#endif
            isOpen = true;
            activeConfig = newActiveConfig;
            activeInterface = interface(activeConfig.d.data()->numericId);
            if (publicConfig.type() == QNetworkConfiguration::UserChoice) {
                QNetworkConfiguration pt;
                pt.d = activeConfig.d.data()->serviceNetworkPtr;
                serviceConfig = pt;
            }
            
            startTime = QDateTime::currentDateTime();

            QT_TRYCATCH_LEAVING({
                    newState(QNetworkSession::Connected);
                    emit quitPendingWaitsForOpened();
                });
            }
            break;
        case KErrNotFound: // Connection failed
            isOpen = false;
            activeConfig = QNetworkConfiguration();
            serviceConfig = QNetworkConfiguration();
            iError = QNetworkSession::InvalidConfigurationError;
            QT_TRYCATCH_LEAVING(emit q->error(iError));
            Cancel();
            if (ipConnectionNotifier) {
                ipConnectionNotifier->StopNotifications();
            }
            QT_TRYCATCH_LEAVING(syncStateWithInterface());
            break;
        case KErrCancel: // Connection attempt cancelled
        case KErrAlreadyExists: // Connection already exists
        default:
            isOpen = false;
            activeConfig = QNetworkConfiguration();
            serviceConfig = QNetworkConfiguration();
            if (publicConfig.state() == QNetworkConfiguration::Undefined ||
                publicConfig.state() == QNetworkConfiguration::Defined) {
                iError = QNetworkSession::InvalidConfigurationError;
            } else {
                iError = QNetworkSession::UnknownSessionError;
            }
            QT_TRYCATCH_LEAVING(emit q->error(iError));
            Cancel();
            if (ipConnectionNotifier) {
                ipConnectionNotifier->StopNotifications();
            }
            QT_TRYCATCH_LEAVING(syncStateWithInterface());
            break;
    }
}

void QNetworkSessionPrivate::DoCancel()
{
    iConnection.Close();
}

// Enters newState if feasible according to current state.
// AccessPointId may be given as parameter. If it is zero, state-change is assumed to
// concern this session's configuration. If non-zero, the configuration is looked up
// and checked if it matches the configuration this session is based on.
bool QNetworkSessionPrivate::newState(QNetworkSession::State newState, TUint accessPointId)
{
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
    qDebug() << "QNS this : " << QString::number((uint)this) << " - "
             << "NEW STATE, IAP ID : " << QString::number(accessPointId) << " , newState : " << QString::number(newState);
#endif
    // Make sure that activeConfig is always updated when SNAP is signaled to be
    // connected.
    if (isOpen && publicConfig.type() == QNetworkConfiguration::ServiceNetwork &&
        newState == QNetworkSession::Connected) {
        activeConfig = activeConfiguration(accessPointId);
        activeInterface = interface(activeConfig.d.data()->numericId);
    }

    // Make sure that same state is not signaled twice in a row.
    if (state == newState) {
        return true;
    }

    // Make sure that Connecting state does not overwrite Roaming state
    if (state == QNetworkSession::Roaming && newState == QNetworkSession::Connecting) {
        return false;
    }
    
    // Make sure that Connected state is not reported when Connection is
    // already Closing.
    // Note: Stopping connection results sometimes KLinkLayerOpen
    //       to be reported first (just before KLinkLayerClosed).
    if (state == QNetworkSession::Closing && newState == QNetworkSession::Connected) {
        return false;
    }

    // Make sure that some lagging 'closing' state-changes do not overwrite
    // if we are already disconnected or closed.
    if (state == QNetworkSession::Disconnected && newState == QNetworkSession::Closing) {
        return false;
    }

    bool emitSessionClosed = false;

    // If we abruptly go down and user hasn't closed the session, we've been aborted.
    // Note that session may be in 'closing' state and not in 'connected' state, because
    // depending on platform the platform may report KConfigDaemonStartingDeregistration
    // event before KLinkLayerClosed
    if ((isOpen && state == QNetworkSession::Connected && newState == QNetworkSession::Disconnected) ||
        (isOpen && !iClosedByUser && newState == QNetworkSession::Disconnected)) {
        // Active & Connected state should change directly to Disconnected state
        // only when something forces connection to close (eg. when another
        // application or session stops connection or when network drops
        // unexpectedly).
        isOpen = false;
        activeConfig = QNetworkConfiguration();
        serviceConfig = QNetworkConfiguration();
        iError = QNetworkSession::SessionAbortedError;
        emit q->error(iError);
        Cancel();
        if (ipConnectionNotifier) {
            ipConnectionNotifier->StopNotifications();
        }
        // Start handling IAP state change signals from QNetworkConfigurationManagerPrivate
        iHandleStateNotificationsFromManager = true;
        emitSessionClosed = true; // Emit SessionClosed after state change has been reported
    }

    bool retVal = false;
    if (accessPointId == 0) {
        state = newState;
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
        qDebug() << "QNS this : " << QString::number((uint)this) << " - " << "===> EMIT State changed A to: " << state;
#endif
        emit q->stateChanged(state);
        retVal = true;
    } else {
        if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
            if (publicConfig.d.data()->numericId == accessPointId) {
                state = newState;
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
                qDebug() << "QNS this : " << QString::number((uint)this) << " - " << "===> EMIT State changed B to: " << state;
#endif
                emit q->stateChanged(state);
                retVal = true;
            }
        } else if (publicConfig.type() == QNetworkConfiguration::UserChoice && isOpen) {
            if (activeConfig.d.data()->numericId == accessPointId) {
                state = newState;
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
                qDebug() << "QNS this : " << QString::number((uint)this) << " - " << "===> EMIT State changed C to: " << state;
#endif
                emit q->stateChanged(state);
                retVal = true;
            }
        } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
            QList<QNetworkConfiguration> subConfigurations = publicConfig.children();
            for (int i = 0; i < subConfigurations.count(); i++) {
                if (subConfigurations[i].d.data()->numericId == accessPointId) {
                    if (newState != QNetworkSession::Disconnected) {
                        state = newState;
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
                        qDebug() << "QNS this : " << QString::number((uint)this) << " - " << "===> EMIT State changed D  to: " << state;
#endif
                        emit q->stateChanged(state);
                        retVal = true;
                    } else {
                        QNetworkConfiguration config = bestConfigFromSNAP(publicConfig);
                        if ((config.state() == QNetworkConfiguration::Defined) ||
                            (config.state() == QNetworkConfiguration::Discovered)) {
                            activeConfig = QNetworkConfiguration();
                            state = newState;
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
                            qDebug() << "QNS this : " << QString::number((uint)this) << " - " << "===> EMIT State changed E  to: " << state;
#endif
                            emit q->stateChanged(state);
                            retVal = true;
                        } else if (config.state() == QNetworkConfiguration::Active) {
                            // Connection to used IAP was closed, but there is another
                            // IAP that's active in used SNAP
                            // => Change state back to Connected
                            state =  QNetworkSession::Connected;
                            emit q->stateChanged(state);
                            retVal = true;
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
                            qDebug() << "QNS this : " << QString::number((uint)this) << " - " << "===> EMIT State changed F  to: " << state;
#endif
                        }
                    }
                }
            }
#ifdef OCC_FUNCTIONALITY_AVAILABLE
            // If the retVal is not true here, it means that the status update may apply to an IAP outside of
            // SNAP (session is based on SNAP but follows IAP outside of it), which may occur on Symbian^3 EasyWlan.
            if (retVal == false && activeConfig.d.data() && activeConfig.d.data()->numericId == accessPointId) {
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
                qDebug() << "QNS this : " << QString::number((uint)this) << " - " << "===> EMIT State changed G  to: " << state;
#endif
                if (newState == QNetworkSession::Disconnected) {
                    activeConfig = QNetworkConfiguration();
                }
                state = newState;
                emit q->stateChanged(state);
                retVal = true;
            }
#endif
        }
    }
    
    if (emitSessionClosed) {
        emit q->closed();
    }
    if (state == QNetworkSession::Disconnected) {
        // The connection has gone down, and processing of status updates must be
        // stopped. Depending on platform, there may come 'connecting/connected' states
        // considerably later (almost a second). Connection id is an increasing
        // number, so this does not affect next _real_ 'conneting/connected' states.
        iDeprecatedConnectionId = publicConfig.d.data()->connectionId;
#ifdef OCC_FUNCTIONALITY_AVAILABLE
        // Just in case clear activeConfiguration.
        activeConfig = QNetworkConfiguration();
#endif
    }
    return retVal;
}

void QNetworkSessionPrivate::handleSymbianConnectionStatusChange(TInt aConnectionStatus,
                                                                 TInt aError,
                                                                 TUint accessPointId)
{
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
    qDebug() << "QNS this : " << QString::number((uint)this) << " - " << QString::number(accessPointId) << " , status : " << QString::number(aConnectionStatus);
#endif
    switch (aConnectionStatus)
        {
        // Connection unitialised
        case KConnectionUninitialised:
            break;

        // Starting connetion selection
        case KStartingSelection:
            break;

        // Selection finished
        case KFinishedSelection:
            if (aError == KErrNone)
                {
                break;
                }
            else
                {
                // The user pressed e.g. "Cancel" and did not select an IAP
                newState(QNetworkSession::Disconnected,accessPointId);
                }
            break;

        // Connection failure
        case KConnectionFailure:
            newState(QNetworkSession::NotAvailable);
            break;

        // Prepearing connection (e.g. dialing)
        case KPsdStartingConfiguration:
        case KPsdFinishedConfiguration:
        case KCsdFinishedDialling:
        case KCsdScanningScript:
        case KCsdGettingLoginInfo:
        case KCsdGotLoginInfo:
            break;

        case KConfigDaemonStartingRegistration:
        // Creating connection (e.g. GPRS activation)
        case KCsdStartingConnect:
        case KCsdFinishedConnect:
            newState(QNetworkSession::Connecting,accessPointId);
            break;

        // Starting log in
        case KCsdStartingLogIn:
            break;

        // Finished login
        case KCsdFinishedLogIn:
            break;

        // Connection open
        case KConnectionOpen:
            break;

        case KLinkLayerOpen:
            newState(QNetworkSession::Connected,accessPointId);
            break;

        // Connection blocked or suspended
        case KDataTransferTemporarilyBlocked:
            break;

        case KConfigDaemonStartingDeregistration:
        // Hangup or GRPS deactivation
        case KConnectionStartingClose:
            newState(QNetworkSession::Closing,accessPointId);
            break;

        // Connection closed
        case KConnectionClosed:
        case KLinkLayerClosed:
            newState(QNetworkSession::Disconnected,accessPointId);
            // Report manager about this to make sure this event
            // is received by all interseted parties (mediated by
            // manager because it does always receive all events from
            // connection monitor).
#ifdef QT_BEARERMGMT_SYMBIAN_DEBUG
            qDebug() << "QNS this : " << QString::number((uint)this) << " - " << "reporting disconnection to manager.";
#endif
            if (publicConfig.d.data()) {
                ((QNetworkConfigurationManagerPrivate*)publicConfig.d.data()->manager)->configurationStateChangeReport(publicConfig.d.data()->numericId, QNetworkSession::Disconnected);
            }
            break;
        // Unhandled state
        default:
            break;
        }
}

ConnectionProgressNotifier::ConnectionProgressNotifier(QNetworkSessionPrivate& owner, RConnection& connection)
    : CActive(CActive::EPriorityUserInput), iOwner(owner), iConnection(connection)
{
    CActiveScheduler::Add(this);
}

ConnectionProgressNotifier::~ConnectionProgressNotifier()
{
    Cancel();
}

void ConnectionProgressNotifier::StartNotifications()
{
    if (!IsActive()) {
        SetActive();
    }
    iConnection.ProgressNotification(iProgress, iStatus);
}

void ConnectionProgressNotifier::StopNotifications()
{
    Cancel();
}

void ConnectionProgressNotifier::DoCancel()
{
    iConnection.CancelProgressNotification();
}

void ConnectionProgressNotifier::RunL()
{
    if (iStatus == KErrNone) {
        QT_TRYCATCH_LEAVING(iOwner.handleSymbianConnectionStatusChange(iProgress().iStage, iProgress().iError));
    
        SetActive();
        iConnection.ProgressNotification(iProgress, iStatus);
    }
}

#include "moc_qnetworksession_s60_p.cpp"

QTM_END_NAMESPACE
