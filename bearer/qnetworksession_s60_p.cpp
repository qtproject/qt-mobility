/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qnetworksession_s60_p.h"
#include "qnetworkconfiguration_s60_p.h"
#include <es_enum.h>
#include <stdapis/sys/socket.h>
#include <stdapis/net/if.h>

QNetworkSessionPrivate::QNetworkSessionPrivate()
    : CActive(CActive::EPriorityStandard), state(QNetworkSession::Disconnected),
      isActive(false), ipConnectionNotifier(0), iError(QNetworkSession::UnknownSessionError),
      iALREnabled(0)
{
    CActiveScheduler::Add(this);
    
    // Try to load "Open C" dll dynamically and
    // try to attach to setdefaultif function dynamically.
    if (iOpenCLibrary.Load(_L("libc")) == KErrNone) {
        iDynamicSetdefaultif = (TOpenCSetdefaultifFunction)iOpenCLibrary.Lookup(564);
    }

    TRAP_IGNORE(iConnectionMonitor.ConnectL());
}

QNetworkSessionPrivate::~QNetworkSessionPrivate()
{
    close(false);
    delete ipConnectionNotifier;

    iConnectionMonitor.CancelNotifications();
    iConnectionMonitor.Close();

    iOpenCLibrary.Close();
}

void QNetworkSessionPrivate::syncStateWithInterface()
{
    // Start monitoring changes in IAP states
    TRAP_IGNORE(iConnectionMonitor.NotifyEventL(*this));

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
                    if (newState(QNetworkSession::Connected, apId)) {
                        return;
                    }
                }
            }
        }
    }

    if (state != QNetworkSession::Connected) {
        // There were no open connections to used IAP or SNAP
        if ((publicConfig.d.data()->state & QNetworkConfiguration::Discovered) ==
            QNetworkConfiguration::Discovered) {
            newState(QNetworkSession::Disconnected);
        } else {
            newState(QNetworkSession::NotAvailable);
        }
    }
}


QNetworkInterface QNetworkSessionPrivate::currentInterface() const
{
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

QString QNetworkSessionPrivate::errorString() const
{
    //TODO
    //must return translated string
    return QString();
}

QNetworkSession::SessionError QNetworkSessionPrivate::error() const
{
    return iError;
}

void QNetworkSessionPrivate::open()
{
    if (isActive) {
        return;
    }

    // Cancel notifications from RConnectionMonitor
    // => RConnection::ProgressNotification will be used for IAP/SNAP monitoring
    iConnectionMonitor.CancelNotifications();
    
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
        // Search through existing connections.
        // If there is already connection which matches to given IAP
        // try to attach to existing connection.
        TBool connected(EFalse);
        TConnectionInfoBuf connInfo;
        TUint count;
        if (iConnection.EnumerateConnections(count) == KErrNone) {
            for (TUint i=1; i<=count; i++) {
                // Note: GetConnectionInfo expects 1-based index.
                if (iConnection.GetConnectionInfo(i, connInfo) == KErrNone) {
                    if (connInfo().iIapId == publicConfig.d.data()->numericId) {
                        if (iConnection.Attach(connInfo, RConnection::EAttachTypeNormal) == KErrNone) {
                            connected = ETrue;
                            startTime = QDateTime::currentDateTime();
                            if (iDynamicSetdefaultif) {
                                // Use name of the IAP to set default IAP
                                QByteArray nameAsByteArray = publicConfig.name().toUtf8();
                                ifreq ifr;
                                strcpy(ifr.ifr_name, nameAsByteArray.constData());

                                error = iDynamicSetdefaultif(&ifr);
                            }
                            isActive = true;
                            emit quitPendingWaitsForOpened();
                            break;
                        }
                    }
                }
            }
        }
        if (!connected) {
            TCommDbConnPref pref;
            pref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
            pref.SetIapId(publicConfig.d.data()->numericId);
            iConnection.Start(pref, iStatus);
            isActive = true;
            if (!IsActive()) {
                SetActive();
            }
            newState(QNetworkSession::Connecting);
        }
    } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        TConnSnapPref snapPref(publicConfig.d.data()->numericId);
        iConnection.Start(snapPref, iStatus);
        isActive = true;
        if (!IsActive()) {
            SetActive();
        }
        newState(QNetworkSession::Connecting);
    }
    
    if (error != KErrNone) {
        isActive = false;
        iError = QNetworkSession::UnknownSessionError;
        emit q->error(iError);
        if (ipConnectionNotifier) {
            ipConnectionNotifier->StopNotifications();
        }
        syncStateWithInterface();    
    }
}

void QNetworkSessionPrivate::close(bool signalWhenCloseIsReady)
{
    if (!isActive) {
        return;
    }
    isActive = false;

    Cancel();
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    if (iMobility) {
        delete iMobility;
        iMobility = NULL;
    }
#endif

    if (ipConnectionNotifier) {
        ipConnectionNotifier->StopNotifications();
    }
    iConnection.Close();
    iSocketServ.Close();
    if (iDynamicSetdefaultif) {
        iDynamicSetdefaultif(0);
    }

    syncStateWithInterface();
    if (signalWhenCloseIsReady) {
        emit q->sessionClosed();
    }
}

void QNetworkSessionPrivate::stop()
{
    if (!isActive) {
        return;
    }
    isActive = false;
    iConnection.Stop(RConnection::EStopAuthoritative);
    newState(QNetworkSession::Disconnected);
    close();
}

void QNetworkSessionPrivate::migrate()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    iMobility->MigrateToPreferredCarrier();
#endif
}

void QNetworkSessionPrivate::ignore()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    iMobility->IgnorePreferredCarrier();
    if (!iALRUpgradingConnection) {
        newState(QNetworkSession::Disconnected);
    } else {
        newState(QNetworkSession::Connected,iOldRoamingIap);
    }
#endif
}

void QNetworkSessionPrivate::accept()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    iMobility->NewCarrierAccepted();
    if (iDynamicSetdefaultif) {
        // Use name of the IAP to set default IAP
        QByteArray nameAsByteArray = actualConfig.name().toUtf8();
        ifreq ifr;
        strcpy(ifr.ifr_name, nameAsByteArray.constData());

        iDynamicSetdefaultif(&ifr);
    }
    newState(QNetworkSession::Connected, iNewRoamingIap);
#endif
}

void QNetworkSessionPrivate::reject()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    iMobility->NewCarrierRejected();
    if (!iALRUpgradingConnection) {
        newState(QNetworkSession::Disconnected);
    } else {
        newState(QNetworkSession::Connected, iOldRoamingIap);
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
                emit q->preferredConfigurationChanged(configs[i],aIsSeamless);
            }
        }
    } else {
        migrate();
    }
}

void QNetworkSessionPrivate::NewCarrierActive(TAccessPointInfo /*aNewAPInfo*/, TBool /*aIsSeamless*/)
{
    if (iALREnabled > 0) {
        emit q->newConfigurationActivated();
    } else {
        accept();
    }
}

void QNetworkSessionPrivate::Error(TInt /*aError*/)
{
    if (isActive) {
        isActive = false;
        iError = QNetworkSession::RoamingError;
        emit q->error(iError);
        Cancel();
        if (ipConnectionNotifier) {
            ipConnectionNotifier->StopNotifications();
        }
        syncStateWithInterface();    
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

QString QNetworkSessionPrivate::bearerName() const
{
    QNetworkConfiguration config;
    if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
        config = publicConfig;
    } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        if (actualConfig.isValid()) {
            config = actualConfig;
        } else {
            config = bestConfigFromSNAP(publicConfig);
        }
    }
    if (!config.isValid()) {
        return QString("Unknown");
    }

    switch (config.d.data()->bearer) {
        case QNetworkConfigurationPrivate::BearerEthernet:  return QString("Ethernet");
        case QNetworkConfigurationPrivate::BearerWLAN:      return QString("WLAN");
        case QNetworkConfigurationPrivate::Bearer2G:        return QString("2G");
        case QNetworkConfigurationPrivate::BearerCDMA2000:  return QString("CDMA2000");
        case QNetworkConfigurationPrivate::BearerWCDMA:     return QString("WCDMA");
        case QNetworkConfigurationPrivate::BearerHSPA:      return QString("HSPA");
        case QNetworkConfigurationPrivate::BearerBluetooth: return QString("Bluetooth");
        case QNetworkConfigurationPrivate::BearerWiMAX:     return QString("WiMAX");
        default: return QString();
    }
}

quint64 QNetworkSessionPrivate::sentData() const
{
    return transferredData(KUplinkData);
}

quint64 QNetworkSessionPrivate::receivedData() const
{
    return transferredData(KDownlinkData);
}

quint64 QNetworkSessionPrivate::transferredData(TUint dataType) const
{
    if (!publicConfig.isValid()) {
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
                if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
                    QList<QNetworkConfiguration> configs = publicConfig.children();
                    for (int i=0; i < configs.count(); i++) {
                        if (configs[i].d.data()->numericId == apId) {
                            configFound = true;
                            break;
                        }
                    }
                } else if (publicConfig.d.data()->numericId == apId) {
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
    if (!isActive || startTime.isNull()) {
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

    if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        QList<QNetworkConfiguration> children = publicConfig.children();
        for (int i=0; i < children.count(); i++) {
            if (children[i].d.data()->numericId == iapId) {
                return children[i];
            }
        }
        return QNetworkConfiguration();
    }

    return publicConfig;
}

void QNetworkSessionPrivate::RunL()
{
    TInt statusCode = iStatus.Int();

    switch (statusCode) {
        case KErrNone: // Connection created succesfully
            actualConfig = activeConfiguration();
            startTime = QDateTime::currentDateTime();

            if (iDynamicSetdefaultif) {
                // Use name of the IAP to set default IAP
                QByteArray nameAsByteArray = actualConfig.name().toUtf8();
                ifreq ifr;
                strcpy(ifr.ifr_name, nameAsByteArray.constData());

                TInt error = iDynamicSetdefaultif(&ifr);
                if (error != KErrNone) {
                    iError = QNetworkSession::UnknownSessionError;
                    emit q->error(iError);
                    Cancel();
                    if (ipConnectionNotifier) {
                        ipConnectionNotifier->StopNotifications();
                    }
                    syncStateWithInterface();
                    return;
                }
            }
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
            if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
                // Activate ALR monitoring
                iMobility = CActiveCommsMobilityApiExt::NewL(iConnection, *this);
            }
#endif
            newState(QNetworkSession::Connected);
            emit quitPendingWaitsForOpened();
            break;
        case KErrNotFound: // Connection failed
        case KErrCancel: // Connection attempt cancelled
        case KErrAlreadyExists: // Connection already exists
        default:
            isActive = false;
            iError = QNetworkSession::UnknownSessionError;
            emit q->error(iError);
            Cancel();
            if (ipConnectionNotifier) {
                ipConnectionNotifier->StopNotifications();
            }
            syncStateWithInterface();
            break;
    }
}

void QNetworkSessionPrivate::DoCancel()
{
    iConnection.Close();
}

bool QNetworkSessionPrivate::newState(QNetworkSession::State newState, TUint accessPointId)
{
    // Make sure that actualConfig is always updated when SNAP is signaled to be
    // connected.
    if (isActive && publicConfig.type() == QNetworkConfiguration::ServiceNetwork &&
        newState == QNetworkSession::Connected) {
        actualConfig = activeConfiguration(accessPointId);
    }

    // Make sure that same state is not signaled twice in a row.
    if (state == newState) {
        return true;
    }

    // Make sure that Connecting state does not overwrite Roaming state
    if (state == QNetworkSession::Roaming && newState == QNetworkSession::Connecting) {
        return false;
    }

    if (isActive && state == QNetworkSession::Connected && newState == QNetworkSession::Disconnected) {
        isActive = false;
        iError = QNetworkSession::SessionAbortedError;
        emit q->error(iError);
        Cancel();
        if (ipConnectionNotifier) {
            ipConnectionNotifier->StopNotifications();
        }
        // Start monitoring changes in IAP states
        TRAP_IGNORE(iConnectionMonitor.NotifyEventL(*this));
    }

    if (accessPointId == 0) {
        state = newState;
        emit q->stateChanged(state);
        return true;
    }

    if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
        if (publicConfig.d.data()->numericId == accessPointId) {
            state = newState;
            emit q->stateChanged(state);
            return true;
        }
    } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        QList<QNetworkConfiguration> subConfigurations = publicConfig.children();
        for (int i = 0; i < subConfigurations.count(); i++) {
            if (subConfigurations[i].d.data()->numericId == accessPointId) {
                if (newState == QNetworkSession::Connected) {
                    // Make sure that when AccessPoint is reported to be Connected
                    // also state of the related configuration changes to Active.
                    subConfigurations[i].d.data()->state = QNetworkConfiguration::Active;

                    state = newState;
                    emit q->stateChanged(state);
                    return true;
                } else {
                    if (newState == QNetworkSession::Disconnected) {
                        // Make sure that when AccessPoint is reported to be disconnected
                        // also state of the related configuration changes from Active to Defined.
                        subConfigurations[i].d.data()->state = QNetworkConfiguration::Defined;
                    }
                    QNetworkConfiguration config = bestConfigFromSNAP(publicConfig);
                    if ((config.state() == QNetworkConfiguration::Defined) ||
                        (config.state() == QNetworkConfiguration::Discovered)) {
                        state = newState;
                        emit q->stateChanged(state);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

void QNetworkSessionPrivate::handleSymbianConnectionStatusChange(TInt aConnectionStatus,
                                                                 TInt aError,
                                                                 TUint accessPointId)
{
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
                // The user successfully selected an IAP to be used
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

        // Hangup or GRPS deactivation
        case KConnectionStartingClose:
            newState(QNetworkSession::Closing,accessPointId);
            break;

        // Connection closed
        case KConnectionClosed:
            break;

        case KLinkLayerClosed:
            newState(QNetworkSession::Disconnected,accessPointId);
            break;

        // Unhandled state
        default:
            break;
        }
}

void QNetworkSessionPrivate::EventL(const CConnMonEventBase& aEvent)
{
    switch (aEvent.EventType())
    {
        case EConnMonConnectionStatusChange:
            {
            CConnMonConnectionStatusChange* realEvent;
            realEvent = (CConnMonConnectionStatusChange*) &aEvent;

            TUint connectionId = realEvent->ConnectionId();
            TInt connectionStatus = realEvent->ConnectionStatus();

            // Try to Find IAP Id using connection Id
            TUint apId = 0;
            if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
                QList<QNetworkConfiguration> subConfigurations = publicConfig.children();
                for (int i = 0; i < subConfigurations.count(); i++ ) {
                    if (subConfigurations[i].d.data()->connectionId == connectionId) {
                        apId = subConfigurations[i].d.data()->numericId;
                        break;
                    }
                }
            } else if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
                if (publicConfig.d.data()->connectionId == connectionId) {
                    apId = publicConfig.d.data()->numericId;
                }
            }

            if (apId > 0) {
                handleSymbianConnectionStatusChange(connectionStatus, KErrNone, apId);
            }
            }
            break;

        case EConnMonCreateConnection:
            {
            CConnMonCreateConnection* realEvent;
            realEvent = (CConnMonCreateConnection*) &aEvent;
            TUint apId;
            TUint connectionId = realEvent->ConnectionId();
            TRequestStatus status;
            iConnectionMonitor.GetUintAttribute(connectionId, 0, KIAPId, apId, status);
            User::WaitForRequest(status);
            if (status.Int() == KErrNone) {
                // Store connection id to related AccessPoint Configuration
                if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
                    QList<QNetworkConfiguration> subConfigurations = publicConfig.children();
                    for (int i = 0; i < subConfigurations.count(); i++ ) {
                        if (subConfigurations[i].d.data()->numericId == apId) {
                            subConfigurations[i].d.data()->connectionId = connectionId;
                            break;
                        }
                    }
                } else if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
                    if (publicConfig.d.data()->numericId == apId) {
                        publicConfig.d.data()->connectionId = connectionId;
                    }
                }
            }
            }
            break;

        case EConnMonDeleteConnection:
            {
            CConnMonDeleteConnection* realEvent;
            realEvent = (CConnMonDeleteConnection*) &aEvent;
            TUint connectionId = realEvent->ConnectionId();
            // Remove connection id from related AccessPoint Configuration
            if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
                QList<QNetworkConfiguration> subConfigurations = publicConfig.children();
                for (int i = 0; i < subConfigurations.count(); i++ ) {
                    if (subConfigurations[i].d.data()->connectionId == connectionId) {
                        subConfigurations[i].d.data()->connectionId = 0;
                        break;
                    }
                }
            } else if (publicConfig.type() == QNetworkConfiguration::InternetAccessPoint) {
                if (publicConfig.d.data()->connectionId == connectionId) {
                    publicConfig.d.data()->connectionId = 0;
                }
            }
            }
            break;

        default:
            // For unrecognized events
            break;
    }
}

ConnectionProgressNotifier::ConnectionProgressNotifier(QNetworkSessionPrivate& owner, RConnection& connection)
    : CActive(CActive::EPriorityStandard), iOwner(owner), iConnection(connection)
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
        iOwner.handleSymbianConnectionStatusChange(iProgress().iStage, iProgress().iError);
    
        SetActive();
        iConnection.ProgressNotification(iProgress, iStatus);
    }
}

