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

#include "qnetworkconfigmanager_s60_p.h"

#include <commdb.h>
#include <aputils.h>
#include <apaccesspointitem.h>
#include <apdatahandler.h>
#include <aputils.h> 
#include <cdbcols.h>
#include <d32dbms.h>

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    #include <cmdestination.h>
    #include <cmconnectionmethod.h>
    #include <cmconnectionmethoddef.h>
#endif

QT_BEGIN_NAMESPACE

QNetworkConfigurationManagerPrivate::QNetworkConfigurationManagerPrivate()
    : QObject(0), CActive(CActive::EPriorityStandard), capFlags(0), iFirstUpdate(true), iS60Error(KErrNone)
{
    TRAP(iS60Error, ipCommsDB = CCommsDatabase::NewL(EDatabaseTypeIAP));
    if (iS60Error != KErrNone) {
        return;
    }
    registerPlatformCapabilities();

    TRAP(iS60Error, iConnectionMonitor.ConnectL());
    if (iS60Error == KErrNone) {
        iS60Error = iConnectionMonitor.NotifyEventL(*this);
    }

#ifdef SNAP_FUNCTIONALITY_AVAILABLE    
    TRAP(iS60Error, iCmManager.OpenL());
    if (iS60Error == KErrNone) {
        iCmManagerOpen = ETrue;
    } else {
        iCmManagerOpen = EFalse;    
    }
#endif

    updateConfigurations();
    updateStatesToSnaps();
}

QNetworkConfigurationManagerPrivate::~QNetworkConfigurationManagerPrivate() 
{
    QList<QString> configIdents = snapConfigurations.keys();
    foreach(QString oldIface, configIdents) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(oldIface);
        priv->isValid = false;
        priv->id.clear();
    }

    configIdents = accessPointConfigurations.keys();
    foreach(QString oldIface, configIdents) {
         QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(oldIface);
        priv->isValid = false;
        priv->id.clear();
    }

    iConnectionMonitor.CancelNotifications();
    iConnectionMonitor.Close();
    
    Cancel();
    
#ifdef SNAP_FUNCTIONALITY_AVAILABLE    
    iCmManager.Close();
#endif
}


void QNetworkConfigurationManagerPrivate::registerPlatformCapabilities()
{
    capFlags |= QNetworkConfigurationManager::BearerManagement;
    capFlags |= QNetworkConfigurationManager::SystemSessionSupport;
    capFlags |= QNetworkConfigurationManager::ApplicationLevelRoaming;
    capFlags |= QNetworkConfigurationManager::ForcedRoaming;
    capFlags |= QNetworkConfigurationManager::DataStatistics;
}

void QNetworkConfigurationManagerPrivate::performAsyncConfigurationUpdate()
{
    //TODO: Change following updateAvailableAccessPoints call to asynchronous
    updateConfigurations();
    iUpdatingConfigurations = ETrue;
    updateAvailableAccessPoints();
    updateActiveAccessPoints();
    updateStatesToSnaps();
    iUpdatingConfigurations = EFalse; 
    emit this->configurationUpdateComplete();
}

void QNetworkConfigurationManagerPrivate::updateConfigurations()
{
    iUpdatingConfigurations = ETrue; 
    ipCommsDB->CancelRequestNotification();
    if (this->IsAdded()) {
        this->Deque();
    }
    TRAP(iS60Error, updateConfigurationsL());
    // Start monitoring changes to IAPs and/or SNAPs in Symbian CommsDB
    CActiveScheduler::Add(this);
    ipCommsDB->RequestNotification(iStatus);
    SetActive();
    iUpdatingConfigurations = EFalse; 
}

void QNetworkConfigurationManagerPrivate::updateConfigurationsL()
{
    QList<QString> knownConfigs = accessPointConfigurations.keys();
    QList<QString> knownSnapConfigs = snapConfigurations.keys();
    
#ifdef SNAP_FUNCTIONALITY_AVAILABLE    
    // S60 version is >= Series60 3rd Edition Feature Pack 2
    
    if (!iCmManagerOpen) {
        return;
    }

    TInt error = KErrNone;
    QNetworkConfigurationPrivate* cpPriv = NULL;
    QNetworkConfigurationPrivate* cpPriv2 = NULL;
    
    // Loop through all IAPs
    RArray<TUint32> connectionMethods; // IAPs
    CleanupClosePushL(connectionMethods);
    iCmManager.ConnectionMethodL(connectionMethods);
    RCmConnectionMethod connectionMethod;
    for(int i = 0; i < connectionMethods.Count(); i++) {
        connectionMethod = iCmManager.ConnectionMethodL(connectionMethods[i]);
        CleanupClosePushL(connectionMethod);
        TUint32 iapId = connectionMethod.GetIntAttributeL(CMManager::ECmIapId);
        QString ident = QString::number(qHash(iapId));
        if (accessPointConfigurations.contains(ident)) {
            knownConfigs.removeOne(ident);
        } else {
            TRAP(error, cpPriv = configFromConnectioMethodL(connectionMethod));
            if (error == KErrNone) {
                QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
                accessPointConfigurations.insert(cpPriv->id, ptr);
                if (!iFirstUpdate) {
                    QNetworkConfiguration item;
                    item.d = ptr;
                    emit configurationAdded(item);
                }
            }
        }
        CleanupStack::PopAndDestroy(&connectionMethod);
    }
    CleanupStack::PopAndDestroy(&connectionMethods);
    
    // Loop through all SNAPs
    bool iapFound;
    HBufC *pName = NULL;
    RArray<TUint32> destinations;
    CleanupClosePushL(destinations);
    iCmManager.AllDestinationsL(destinations);
    RCmDestination destination;
    for(int i = 0; i < destinations.Count(); i++) {
        destination = iCmManager.DestinationL(destinations[i]);
        CleanupClosePushL(destination);
        QString ident = QString::number(qHash(destination.Id()+1000)); //TODO: Check if it's ok to add 1000 SNAP Id to prevent SNAP ids overlapping IAP ids
        if (snapConfigurations.contains(ident)) {
            knownSnapConfigs.removeOne(ident);
        } else {
            QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
            CleanupStack::PushL(cpPriv);
    
            pName = destination.NameLC();
            cpPriv->name = QString::fromUtf16(pName->Ptr(),pName->Length());
            CleanupStack::PopAndDestroy(pName);
            pName = NULL;
    
            cpPriv->isValid = true;
            cpPriv->id = ident;
            cpPriv->numericId = destination.Id();
            cpPriv->state = QNetworkConfiguration::Defined;
            cpPriv->type = QNetworkConfiguration::ServiceNetwork;

            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
            snapConfigurations.insert(ident, ptr);
            if (!iFirstUpdate) {
                QNetworkConfiguration item;
                item.d = ptr;
                emit configurationAdded(item);
            }
            
            CleanupStack::Pop(cpPriv);
        }
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> privSNAP = snapConfigurations.value(ident);
            
        for (int j=0; j < destination.ConnectionMethodCount(); j++) {
            connectionMethod = destination.ConnectionMethodL(j);
            CleanupClosePushL(connectionMethod);
            
            TUint32 iapId = connectionMethod.GetIntAttributeL(CMManager::ECmIapId);
            QString iface = QString::number(qHash(iapId));
            // Check that IAP can be found from accessPointConfigurations list
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(iface);
            if (priv.data() == 0) {
                TRAP(error, cpPriv2 = configFromConnectioMethodL(connectionMethod));
                if (error == KErrNone) {
                    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv2);
                    accessPointConfigurations.insert(cpPriv2->id, ptr);
                    if (!iFirstUpdate) {
                        QNetworkConfiguration item;
                        item.d = ptr;
                        emit configurationAdded(item);
                    }
                    privSNAP->serviceNetworkMembers.append(ptr);
                }
            } else {
                knownConfigs.removeOne(iface);
                // Check that IAP can be found from related SNAP's configuration list
                iapFound = false;
                for (int i=0; i<privSNAP->serviceNetworkMembers.count(); i++) {
                    if (privSNAP->serviceNetworkMembers[i]->numericId == iapId) {
                        iapFound = true;
                        break;
                    }
                }
                if (!iapFound) {
                    privSNAP->serviceNetworkMembers.append(priv);
                }
            }
            
            CleanupStack::PopAndDestroy(&connectionMethod);
        }
        CleanupStack::PopAndDestroy(&destination);
    }
    CleanupStack::PopAndDestroy(&destinations);
    
#else
    // S60 version is < Series60 3rd Edition Feature Pack 2
    if (!ipCommsDB) {
        return;
    }
    CCommsDbTableView* pDbTView = ipCommsDB->OpenTableLC(TPtrC(IAP));

    // Loop through all IAPs
    TUint32 apId = 0;
    TInt retVal = pDbTView->GotoFirstRecord();
    while (retVal == KErrNone) {
        pDbTView->ReadUintL(TPtrC(COMMDB_ID), apId);
        QString ident = QString::number(qHash(apId));
        if (accessPointConfigurations.contains(ident)) {
            knownConfigs.removeOne(ident);
        } else {
            QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
            if (readNetworkConfigurationValuesFromCommsDb(apId, cpPriv)) {
                QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> ptr(cpPriv);
                accessPointConfigurations.insert(ident, ptr);
                if (!iFirstUpdate) {
                    QNetworkConfiguration item;
                    item.d = ptr;
                    emit configurationAdded(item);
                }
            } else {
                delete cpPriv;
            }
        }
        retVal = pDbTView->GotoNextRecord();
    }
    CleanupStack::PopAndDestroy(pDbTView);
#endif
    updateActiveAccessPoints();
    
    foreach (QString oldIface, knownConfigs) {
        //remove non existing IAP
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.take(oldIface);
        priv->isValid = false;
        if (!iFirstUpdate) {
            QNetworkConfiguration item;
            item.d = priv;
            emit configurationRemoved(item);
        }
        // Remove non existing IAP from SNAPs
        QList<QString> snapConfigIdents = snapConfigurations.keys();
        foreach (QString iface, snapConfigIdents) {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv2 = snapConfigurations.value(iface);
            // => Check if one of the IAPs of the SNAP is active
            for (int i=0; i<priv2->serviceNetworkMembers.count(); i++) {
                if (priv2->serviceNetworkMembers[i]->numericId == priv->numericId) {
                    priv2->serviceNetworkMembers.removeAt(i);
                    break;
                }
            }
        }    
    }
    foreach (QString oldIface, knownSnapConfigs) {
        //remove non existing SNAPs
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = snapConfigurations.take(oldIface);
        priv->isValid = false;
        if (!iFirstUpdate) {
            QNetworkConfiguration item;
            item.d = priv;
            emit configurationRemoved(item);
        }
    }

    if (iFirstUpdate) {
        iFirstUpdate = false;
    }
}

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
QNetworkConfigurationPrivate* QNetworkConfigurationManagerPrivate::configFromConnectioMethodL(
        RCmConnectionMethod& connectionMethod)
{
    QNetworkConfigurationPrivate* cpPriv = new QNetworkConfigurationPrivate();
    CleanupStack::PushL(cpPriv);
    
    TUint32 iapId = connectionMethod.GetIntAttributeL(CMManager::ECmIapId);
    QString ident = QString::number(qHash(iapId));
    
    HBufC *pName = connectionMethod.GetStringAttributeL(CMManager::ECmName);
    CleanupStack::PushL(pName);
    cpPriv->name = QString::fromUtf16(pName->Ptr(),pName->Length());
    CleanupStack::PopAndDestroy(pName);
    pName = NULL;
    
    TUint32 bearerId = connectionMethod.GetIntAttributeL(CMManager::ECmCommsDBBearerType);
    switch (bearerId) {
        case KCommDbBearerCSD:
            cpPriv->bearer = QNetworkConfigurationPrivate::Bearer2G;
            break;
        case KCommDbBearerWcdma:
            cpPriv->bearer = QNetworkConfigurationPrivate::BearerWCDMA;
            break;
        case KCommDbBearerLAN:
            cpPriv->bearer = QNetworkConfigurationPrivate::BearerEthernet;
            break;
        case KCommDbBearerCdma2000:
            cpPriv->bearer = QNetworkConfigurationPrivate::BearerCDMA2000;
            break;
        case KCommDbBearerVirtual:
            cpPriv->bearer = QNetworkConfigurationPrivate::BearerUnknown;
            break;
        case KCommDbBearerPAN:
            cpPriv->bearer = QNetworkConfigurationPrivate::BearerUnknown;
            break;
        case KCommDbBearerWLAN:
            cpPriv->bearer = QNetworkConfigurationPrivate::BearerWLAN;
            break;
        default:
            cpPriv->bearer = QNetworkConfigurationPrivate::BearerUnknown;
            break;
    }
    
    cpPriv->state |= QNetworkConfiguration::Defined;
    TBool isConnected = connectionMethod.GetBoolAttributeL(CMManager::ECmConnected);
    if (isConnected) {
        cpPriv->state = QNetworkConfiguration::Active;
    }
    
    cpPriv->isValid = true;
    cpPriv->id = ident;
    cpPriv->numericId = iapId;
    cpPriv->type = QNetworkConfiguration::InternetAccessPoint;
    
    CleanupStack::Pop(cpPriv);
    return cpPriv;
}
#endif

QNetworkConfiguration QNetworkConfigurationManagerPrivate::defaultConfiguration()
{
    QNetworkConfiguration config;
    TRAP(iS60Error, config = defaultConfigurationL());
    return config;
}

QNetworkConfiguration QNetworkConfigurationManagerPrivate::defaultConfigurationL()
{
    QNetworkConfiguration item;
    QList<QString> knownConfigs = accessPointConfigurations.keys();

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    // Check Default Connection
    TCmDefConnValue defaultConnectionValue;
    iCmManager.ReadDefConnL(defaultConnectionValue);
    if (defaultConnectionValue.iType == ECmDefConnDestination) {
        QString iface = QString::number(qHash(defaultConnectionValue.iId+1000));
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = snapConfigurations.value(iface);
        if (priv.data() != 0) {
            item.d = priv;
        }
    } else if (defaultConnectionValue.iType == ECmDefConnConnectionMethod) {
        QString iface = QString::number(qHash(defaultConnectionValue.iId));
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(iface);
        if (priv.data() != 0) {
            item.d = priv;
        }
    } 
#endif
    
    return item;
}

bool QNetworkConfigurationManagerPrivate::readNetworkConfigurationValuesFromCommsDb(
        TUint32 aApId, QNetworkConfigurationPrivate* apNetworkConfiguration)
{
    iS60Error = KErrNone;
    TRAP(iS60Error, readNetworkConfigurationValuesFromCommsDbL(aApId,apNetworkConfiguration));
    if (iS60Error != KErrNone) {
        return false;        
    }
    return true;
}

void QNetworkConfigurationManagerPrivate::readNetworkConfigurationValuesFromCommsDbL(
        TUint32 aApId, QNetworkConfigurationPrivate* apNetworkConfiguration)
{
    CApDataHandler* pDataHandler = CApDataHandler::NewLC(*ipCommsDB); 
    CApAccessPointItem* pAPItem = CApAccessPointItem::NewLC(); 
    TBuf<KCommsDbSvrMaxColumnNameLength> name;
    
    CApUtils* pApUtils = CApUtils::NewLC(*ipCommsDB);
    TUint32 apId = pApUtils->WapIdFromIapIdL(aApId);
    
    pDataHandler->AccessPointDataL(apId,*pAPItem);
    pAPItem->ReadTextL(EApIapName, name);
    if (name.Compare(_L("Easy WLAN")) == 0) {
        // "Easy WLAN" won't be accepted to the Configurations list
        User::Leave(KErrNotFound);
    }
    
    QString ident = QString::number(qHash(aApId));
    
    apNetworkConfiguration->name = QString::fromUtf16(name.Ptr(),name.Length());
    apNetworkConfiguration->isValid = true;
    apNetworkConfiguration->id = ident;
    apNetworkConfiguration->numericId = aApId;
    apNetworkConfiguration->state = (QNetworkConfiguration::Defined);
    apNetworkConfiguration->type = QNetworkConfiguration::InternetAccessPoint;
    switch (pAPItem->BearerTypeL()) {
        case EApBearerTypeCSD:      
            apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::Bearer2G;
            break;
        case EApBearerTypeGPRS:
            apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::Bearer2G;
            break;
        case EApBearerTypeHSCSD:
            apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::BearerHSPA;
            break;
        case EApBearerTypeCDMA:
            apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::BearerCDMA2000;
            break;
        case EApBearerTypeWLAN:
            apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::BearerWLAN;
            break;
        case EApBearerTypeLAN:
            apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::BearerEthernet;
            break;
        case EApBearerTypeLANModem:
            apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::BearerEthernet;
            break;
        default: apNetworkConfiguration->bearer = QNetworkConfigurationPrivate::BearerUnknown;
    }
    
    CleanupStack::PopAndDestroy(pApUtils);
    CleanupStack::PopAndDestroy(pAPItem);
    CleanupStack::PopAndDestroy(pDataHandler);
}

void QNetworkConfigurationManagerPrivate::updateActiveAccessPoints()
{
    bool online = false;
    QList<QString> inactiveConfigs = accessPointConfigurations.keys();

    TRequestStatus status;
    TUint connectionCount;
    iConnectionMonitor.GetConnectionCount(connectionCount, status);
    User::WaitForRequest(status);
    
    // Go through all connections and set state of related IAPs to Active
    TUint connectionId;
    TUint subConnectionCount;
    TUint apId;
    if (status.Int() == KErrNone) {
        for (TUint i = 1; i <= connectionCount; i++) {
            iConnectionMonitor.GetConnectionInfo(i, connectionId, subConnectionCount);
            iConnectionMonitor.GetUintAttribute(connectionId, subConnectionCount, KIAPId, apId, status);
            User::WaitForRequest(status);
            QString ident = QString::number(qHash(apId));
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(ident);
            if (priv.data()) {
                online = true;
                inactiveConfigs.removeOne(ident);
                changeConfigurationState(priv, QNetworkConfiguration::Active);
            }
        }
    }

    // Make sure that state of rest of the IAPs won't be Active
    foreach (QString iface, inactiveConfigs) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(iface);
        if (priv.data()) {
            ensureMaxConfigurationState(priv, QNetworkConfiguration::Discovered);
        }
    }

    if (iOnline != online) {
        iOnline = online;
        emit this->onlineStateChanged(iOnline);
    }
}

void QNetworkConfigurationManagerPrivate::updateAvailableAccessPoints()
{
    QList<QString> unavailableConfigs = accessPointConfigurations.keys();

    TRequestStatus status;
    TPckgBuf<TConnMonNetworkNames> pkgNetworks;
    TConnMonIapInfoBuf iapBuf;
    iConnectionMonitor.GetPckgAttribute(EBearerIdAll, 0, KIapAvailability, iapBuf, status);
    User::WaitForRequest(status) ;
    
    // Set state of returned IAPs to Discovered
    // if state is not already Active
    if (status.Int() == KErrNone) {
        for(TUint i=0; i<iapBuf().iCount; i++) {
            QString ident = QString::number(qHash(iapBuf().iIap[i].iIapId));
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(ident);
            if (priv.data()) {
                unavailableConfigs.removeOne(ident);
                if (priv.data()->state < QNetworkConfiguration::Active) {
                    changeConfigurationState(priv, QNetworkConfiguration::Discovered);
                }
            }
        }
    }

    // Make sure that state of rest of the IAPs won't be Discovered
    foreach (QString iface, unavailableConfigs) {
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(iface);
        if (priv.data()) {
            ensureMaxConfigurationState(priv, QNetworkConfiguration::Defined);
        }
    }
}

void QNetworkConfigurationManagerPrivate::updateStatesToSnaps()
{
    // Go through SNAPs and set correct state to SNAPs
    bool discovered;
    bool active;
    QList<QString> snapConfigIdents = snapConfigurations.keys();
    foreach (QString iface, snapConfigIdents) {
        discovered = false;
        active = false;
        QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = snapConfigurations.value(iface);
        // => Check if one of the IAPs of the SNAP is discovered
        for (int i=0; i<priv->serviceNetworkMembers.count(); i++)
        {
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv2 = priv->serviceNetworkMembers[i];
            if ((priv->serviceNetworkMembers[i]->state & QNetworkConfiguration::Active) 
                    == QNetworkConfiguration::Active) {
                active = true;
                break;
            } else if ((priv->serviceNetworkMembers[i]->state & QNetworkConfiguration::Discovered) 
                        == QNetworkConfiguration::Discovered) {
                discovered = true;
            }
        }
        if (active) {
            changeConfigurationState(priv, QNetworkConfiguration::Active);
        } else if (discovered) {
            changeConfigurationState(priv, QNetworkConfiguration::Discovered);
        } else {
            ensureMaxConfigurationState(priv, QNetworkConfiguration::Defined);
        }
    }    
}

bool QNetworkConfigurationManagerPrivate::changeConfigurationState(QExplicitlySharedDataPointer<QNetworkConfigurationPrivate>& sharedData,
                                                                     QNetworkConfiguration::StateFlags newState)
{
    if ((sharedData.data()->state & newState) != newState) {
        sharedData.data()->state |= newState;
        QNetworkConfiguration item;
        item.d = sharedData;
        if (!iFirstUpdate) {
            emit configurationChanged(item);
        }
        return true;
    }
    return false;
}

bool QNetworkConfigurationManagerPrivate::ensureMaxConfigurationState(QExplicitlySharedDataPointer<QNetworkConfigurationPrivate>& sharedData,
                                                                      QNetworkConfiguration::StateFlags newState)
{
    QNetworkConfiguration::StateFlags oldState = sharedData.data()->state;
    sharedData.data()->state &= newState;
    if (oldState != sharedData.data()->state) {
        QNetworkConfiguration item;
        item.d = sharedData;
        if (!iFirstUpdate) {
            emit configurationChanged(item);
        }
        return true;
    }
    return false;
}

void QNetworkConfigurationManagerPrivate::RunL()
{
    TInt statusCode = iStatus.Int();
    switch (statusCode) {
        case RDbNotifier::EClose: /** The database has been closed. */
            statusCode = 0;
            break;
        case RDbNotifier::EUnlock: /** All read locks have been removed. */
            statusCode = 0;
            break;
        case RDbNotifier::ECommit: /** A transaction has been committed. */ 
            statusCode = 0;
            break;
        case RDbNotifier::ERollback: /** A transaction has been rolled back */
            statusCode = 0;
            break;
        case RDbNotifier::ERecover: /** The database has been recovered */
            statusCode = 0;
            break;
        default:
            statusCode = 0;
            break;
    }
    
    if (!iUpdatingConfigurations) {
        updateConfigurations();
    }
}

void QNetworkConfigurationManagerPrivate::DoCancel()
{
    ipCommsDB->CancelRequestNotification();
}


void QNetworkConfigurationManagerPrivate::EventL(const CConnMonEventBase& aEvent)
{
    switch (aEvent.EventType())
    {
        case EConnMonCreateConnection:
            {
            CConnMonCreateConnection* realEvent;
            realEvent = (CConnMonCreateConnection*) &aEvent;
            TUint subConnectionCount = 0;
            TUint apId;            
            TUint connectionId = realEvent->ConnectionId();
            TRequestStatus status;
            iConnectionMonitor.GetUintAttribute(connectionId, subConnectionCount, KIAPId, apId, status);
            User::WaitForRequest(status);
            QString ident = QString::number(qHash(apId));
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(ident);
            if (priv.data()) {
                if (changeConfigurationState(priv, QNetworkConfiguration::Active)) {
                    updateStatesToSnaps();
                }
                if (!iOnline) {
                    iOnline = true;
                    emit this->onlineStateChanged(iOnline);
                }
            }
            }
            break;
    
        case EConnMonDeleteConnection:
            {
            CConnMonDeleteConnection* realEvent;
            realEvent = (CConnMonDeleteConnection*) &aEvent;
            TUint connectionId = realEvent->ConnectionId();
            TUint subConnectionCount = 0;
            TUint apId;            
            TRequestStatus status;
            iConnectionMonitor.GetUintAttribute(connectionId, subConnectionCount, KIAPId, apId, status);
            User::WaitForRequest(status);
            QString ident = QString::number(qHash(apId));
            QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(ident);
            if (priv.data()) {
                if (ensureMaxConfigurationState(priv, QNetworkConfiguration::Discovered)) {
                    updateStatesToSnaps();
                }
            }
            TUint connectionCount;
            iConnectionMonitor.GetConnectionCount(connectionCount, status);
            User::WaitForRequest(status);
            if (connectionCount == 0) {
                iOnline = false;
                emit this->onlineStateChanged(iOnline);
            }
            }
            break;
    
        case EConnMonIapAvailabilityChange:
            {
            CConnMonIapAvailabilityChange* realEvent;
            realEvent = (CConnMonIapAvailabilityChange*) &aEvent;
            TConnMonIapInfo iaps = realEvent->IapAvailability();
            QList<QString> unDiscoveredConfigs = accessPointConfigurations.keys();
            for ( TUint i = 0; i < iaps.Count(); i++ ) {
                QString ident = QString::number(qHash(iaps.iIap[i].iIapId));
                QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(ident);
                if (priv.data()) {
                    changeConfigurationState(priv, QNetworkConfiguration::Discovered);
                    unDiscoveredConfigs.removeOne(ident);
                }
            }
            foreach (QString iface, unDiscoveredConfigs) {
                QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> priv = accessPointConfigurations.value(iface);
                if (priv.data()) {
                    ensureMaxConfigurationState(priv, QNetworkConfiguration::Defined);
                }
            }
            }
            break;

        default:
            // For unrecognized events
            break;
    }
}

QT_END_NAMESPACE
