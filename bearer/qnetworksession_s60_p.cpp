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
      isActive(false), ipConnectionNotifier(0), m_activeTime(0), iS60Error(KErrNone),
      iALREnabled(0)
{
    if (iOpenCLibrary.Load(_L("libc")) == KErrNone) {
        iDynamicSetdefaultif = (TOpenCSetdefaultifFunction)iOpenCLibrary.Lookup(564);
    }

    TRAP(iS60Error, iConnectionMonitor.ConnectL());
}

QNetworkSessionPrivate::~QNetworkSessionPrivate()
{
    close();
    if (publicConfig.type() != QNetworkConfiguration::ServiceNetwork) {
        iConnectionMonitor.CancelNotifications();
    }
    iConnectionMonitor.Close();
    iOpenCLibrary.Close();
}

void QNetworkSessionPrivate::syncStateWithInterface()
{
    if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        return;
    }

    TRAP(iS60Error, iConnectionMonitor.NotifyEventL(*this));
    if (iS60Error != KErrNone) {
        return;
    }

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
            if (status.Int() == KErrNone && apId == publicConfig.d.data()->numericId) {
                newState(QNetworkSession::Connected);
                return;
            }            
        }
    }
    
    if ((publicConfig.d.data()->state & QNetworkConfiguration::Discovered) == 
        QNetworkConfiguration::Discovered) {
        newState(QNetworkSession::Disconnected);
    } else {
        newState(QNetworkSession::NotAvailable);
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
    return QNetworkSession::UnknownSessionError;
}

void QNetworkSessionPrivate::open()
{
    if (isActive) {
        return;
    }

    TInt iS60Error = iSocketServ.Connect();
    if (iS60Error == KErrNone) {
        iS60Error = iConnection.Open(iSocketServ);
        ipConnectionNotifier = new ConnectionProgressNotifier(*this,iConnection);
        if (iS60Error == KErrNone) {
            if (publicConfig.type() != QNetworkConfiguration::ServiceNetwork) {
                iConnectionMonitor.CancelNotifications();
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
                                    isActive = true;
                                    if (iDynamicSetdefaultif) {
                                        // Use name of the IAP to set default IAP
                                        QByteArray nameAsByteArray = publicConfig.name().toUtf8(); 
                                        ifreq ifr;
                                        strcpy(ifr.ifr_name, nameAsByteArray.constData());
                                
                                        iS60Error = iDynamicSetdefaultif(&ifr);
                                    }
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
                    CActiveScheduler::Add(this);    
                    iConnection.Start(pref, iStatus);
                    isActive = true;
                    SetActive();
                    newState(QNetworkSession::Connecting);
                }
            } else if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
                TConnSnapPref snapPref(publicConfig.d.data()->numericId);
                //TCommDbConnPref pref;
                //pref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
                //pref.SetDirection(ECommDbConnectionDirectionOutgoing);
                //pref.SetIapId(0);
                //pref.SetNetId(snapPref.Snap());
                CActiveScheduler::Add(this);    
                iConnection.Start(snapPref, iStatus);
                isActive = true;
                SetActive();
                newState(QNetworkSession::Connecting);
            }
        } else {
            // Could not open RConnection
            newState(QNetworkSession::Invalid);
            iSocketServ.Close();
        }
    } else {
        // Could not open RSocketServ
        newState(QNetworkSession::Invalid);
    }
}

void QNetworkSessionPrivate::close()
{
    if (!isActive) {
        return;
    }
    isActive = false;

    iS60Error = KErrNone;
    Cancel();
    if (this->IsAdded()) {
        this->Deque();
    }
#ifdef SNAP_FUNCTIONALITY_AVAILABLE    
    if (iMobility) {
        delete iMobility;
        iMobility = NULL;     
    }
#endif    
    
    if (ipConnectionNotifier) {
        delete ipConnectionNotifier;
        ipConnectionNotifier = NULL;
    }
    iConnection.Close();
    iSocketServ.Close();
    if (iDynamicSetdefaultif) {
        iS60Error = iDynamicSetdefaultif(0);
    }
    
    if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        newState(QNetworkSession::Disconnected);
    } else {
        syncStateWithInterface();
    }
}

void QNetworkSessionPrivate::stop()
{
    if (!isActive) {
        return;
    }
    isActive = false;

    iS60Error = KErrNone;
    Cancel();
    if (this->IsAdded()) {
        this->Deque();
    }
#ifdef SNAP_FUNCTIONALITY_AVAILABLE    
    if (iMobility) {
        delete iMobility;
        iMobility = NULL;     
    }
#endif    
    
    if (ipConnectionNotifier) {
        delete ipConnectionNotifier;
        ipConnectionNotifier = NULL;
    }
    iConnection.Stop(RConnection::EStopAuthoritative);
    iConnection.Close();
    iSocketServ.Close();
    if (iDynamicSetdefaultif) {
        iS60Error = iDynamicSetdefaultif(0);
    }
    
    if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        newState(QNetworkSession::Disconnected);
    } else {
        syncStateWithInterface();
    }
}

void QNetworkSessionPrivate::migrate()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    iMobility->MigrateToPreferredCarrier();
    newState(QNetworkSession::Roaming);
#endif
}

void QNetworkSessionPrivate::ignore()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    iMobility->IgnorePreferredCarrier();
    if (!iALRUpgradingConnection) {
        newState(QNetworkSession::Disconnected);
    } else {
        newState(QNetworkSession::Connected);
    }
#endif
}

void QNetworkSessionPrivate::accept()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    iS60Error = KErrNone;

    iMobility->NewCarrierAccepted();
    actualConfig = activeConfiguration();
    if (iDynamicSetdefaultif) {
        // Use name of the IAP to set default IAP
        QByteArray nameAsByteArray = actualConfig.name().toUtf8(); 
        ifreq ifr;
        strcpy(ifr.ifr_name, nameAsByteArray.constData());

        iS60Error = iDynamicSetdefaultif(&ifr);
    }
    newState(QNetworkSession::Connected);
#endif    
}

void QNetworkSessionPrivate::reject()
{
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    iMobility->NewCarrierRejected();
    if (!iALRUpgradingConnection) {
        newState(QNetworkSession::Disconnected);
    } else {
        newState(QNetworkSession::Connected);
    }
#endif
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
            QList<QNetworkConfiguration> subConfigurations = publicConfig.children();
            for (int i = 0; i < subConfigurations.count(); i++ ) {
                if (subConfigurations[i].state() == QNetworkConfiguration::Active) {
                    config = subConfigurations[i];
                    break;
                } else if (!config.isValid() && subConfigurations[i].state() == QNetworkConfiguration::Discovered) {
                    config = subConfigurations[i];
                }
            }
            if (!config.isValid()) {
                config = subConfigurations[0];
            }
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

void QNetworkSessionPrivate::setALREnabled(bool enabled)
{
    if (enabled) {
        iALREnabled++;
    } else {
        iALREnabled--;  
    }
}

quint64 QNetworkSessionPrivate::sentData() const
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

    TUint uplinkData = 0;
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
                    TUint ulData;
                    iConnectionMonitor.GetUintAttribute(connectionId, 0, KUplinkData, ulData, status );
                    User::WaitForRequest(status);
                    if (status.Int() == KErrNone) {
                        uplinkData += ulData;
                    }
                }
            }            
        }
    }
    
    return uplinkData;
}

quint64 QNetworkSessionPrivate::receivedData() const
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
    
    TUint downlinkData = 0;
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
                    TUint dlData;
                    iConnectionMonitor.GetUintAttribute(connectionId, 0, KDownlinkData, dlData, status );
                    User::WaitForRequest(status);
                    if (status.Int() == KErrNone) {
                    downlinkData += dlData;
                    }
                }
            }            
        }
    }
    
    return downlinkData;
}

quint64 QNetworkSessionPrivate::activeTime() const
{
    if (!isActive || startTime.isNull()) {
        return 0;
    }
    return startTime.secsTo(QDateTime::currentDateTime());
}

QNetworkConfiguration QNetworkSessionPrivate::activeConfiguration() const
{
    _LIT(KSetting, "IAP\\Id");
    TUint32 iapId = 0;
    iConnection.GetIntSetting(KSetting, iapId);
    
    if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
        QList<QNetworkConfiguration> children = publicConfig.children();
        for (int i=0; i < children.count(); i++) {
            if (children[i].d.data()->numericId == iapId) {
                return children[i]; 
            }
        }
    }
    
    return publicConfig;
}

void QNetworkSessionPrivate::RunL()
{
    iS60Error = KErrNone;

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
        
                iS60Error = iDynamicSetdefaultif(&ifr);
            }
#ifdef SNAP_FUNCTIONALITY_AVAILABLE            
            if (publicConfig.type() == QNetworkConfiguration::ServiceNetwork) {
                // Activate ALR monitoring
                iMobility = CActiveCommsMobilityApiExt::NewL(iConnection, *this);
            }
#endif            
            newState(QNetworkSession::Connected);
            break;
        case KErrNotFound: // Connection failed
        case KErrCancel: // Connection attempt cancelled
        case KErrAlreadyExists: // Connection already exists
        default:
            close();
            newState(QNetworkSession::Invalid);
            break;
    }
}

void QNetworkSessionPrivate::DoCancel()
{
    TInt result = iConnection.Stop();
    if (result != KErrNone) {
        // Nothing to do...
    }
}

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
void QNetworkSessionPrivate::PreferredCarrierAvailable(TAccessPointInfo /*aOldAPInfo*/,
                                                       TAccessPointInfo aNewAPInfo,
                                                       TBool aIsUpgrade,
                                                       TBool aIsSeamless)
{
    if (iALREnabled > 0) {
        iALRUpgradingConnection = aIsUpgrade;
        QList<QNetworkConfiguration> configs = publicConfig.children();
        for (int i=0; i < configs.count(); i++) {
            if (configs[i].d.data()->numericId == aNewAPInfo.AccessPoint() ) {
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
    
}
#endif

void QNetworkSessionPrivate::newState(QNetworkSession::State newState)
{
    if (state != newState) {
        state = newState;
        emit q->stateChanged(state);
    }
}

void QNetworkSessionPrivate::handleSymbianConnectionStatusChange(TInt aConnectionStatus,
                                                                 TInt aError)
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
                newState(QNetworkSession::Disconnected);
                }
            break;
    
        // Connection failure
        case KConnectionFailure:
            newState(QNetworkSession::Invalid);
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
            newState(QNetworkSession::Connecting);
            break;
    
        // Starting log in
        case KCsdStartingLogIn:
            break;
    
        // Finished login
        case KCsdFinishedLogIn:
            break;
    
        // Connection open
        case KConnectionOpen:
        case KLinkLayerOpen:
            newState(QNetworkSession::Connected);
            break;
            
        // Connection blocked or suspended
        case KDataTransferTemporarilyBlocked:
            break;
    
        // Hangup or GRPS deactivation
        case KConnectionStartingClose:
            newState(QNetworkSession::Closing);
            break;
    
        // Connection closed
        case KConnectionClosed:
        case KLinkLayerClosed:
            newState(QNetworkSession::Disconnected);
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

            TRequestStatus status;
            TUint apId;
            iConnectionMonitor.GetUintAttribute(connectionId, 0, KIAPId, apId, status);
            User::WaitForRequest(status);
            if (status.Int() == KErrNone && apId == publicConfig.d.data()->numericId) {
                handleSymbianConnectionStatusChange(connectionStatus, KErrNone);
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
            if (status.Int() == KErrNone && apId == publicConfig.d.data()->numericId) {
                newState(QNetworkSession::Connected);
            }
            }
            break;
    
        case EConnMonDeleteConnection:
            {
            bool connected = false;
            CConnMonDeleteConnection* realEvent;
            realEvent = (CConnMonDeleteConnection*) &aEvent;
            TUint connectionId = realEvent->ConnectionId();
            TUint apId;            
            TRequestStatus status;
            iConnectionMonitor.GetUintAttribute(connectionId, 0, KIAPId, apId, status);
            User::WaitForRequest(status);
            if (status.Int() == KErrNone && apId == publicConfig.d.data()->numericId) {
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
                        if (status.Int() == KErrNone && apId == publicConfig.d.data()->numericId) {
                            connected = true;
                            break;
                        }            
                    }
                }
            }
            if (!connected) {
                newState(QNetworkSession::Disconnected);
            }
            }
            break;

        default:
            // For unrecognized events
            break;
    }
}

ConnectionProgressNotifier::ConnectionProgressNotifier(QNetworkSessionPrivate& owner, RConnection& connection)
    : CActive(CActive::EPriorityStandard), owner(owner), iConnection(connection)
{
    CActiveScheduler::Add(this);  
    iConnection.ProgressNotification(iProgress, iStatus);
    SetActive();
}

ConnectionProgressNotifier::~ConnectionProgressNotifier()
{
    Cancel();
    iConnection.CancelProgressNotification();
}

void ConnectionProgressNotifier::DoCancel()
{
    iConnection.CancelProgressNotification();
}

void ConnectionProgressNotifier::RunL()
    {
    owner.handleSymbianConnectionStatusChange(iProgress().iStage, iProgress().iError);

    iConnection.ProgressNotification(iProgress, iStatus);
    SetActive();
}

