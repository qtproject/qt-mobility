/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QNETWORKCONFIGURATIONMANAGERPRIVATE_H
#define QNETWORKCONFIGURATIONMANAGERPRIVATE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qnetworkconfigmanager.h>
#include <qnetworkconfiguration_s60_p.h>
#include <qnetworksession.h>

#include <QHash>
#include <rconnmon.h>
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    #include <cmmanager.h>
#endif

// Uncomment and compile QtBearer to gain detailed state tracing
// #define QT_BEARERMGMT_SYMBIAN_DEBUG

#define QT_BEARERMGMT_CONFIGURATION_SNAP_PREFIX "S_"
#define QT_BEARERMGMT_CONFIGURATION_IAP_PREFIX  "I_"

class CCommsDatabase;
class QEventLoop;

QT_BEGIN_NAMESPACE
class QTimer;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QNetworkSessionPrivate;
class AccessPointsAvailabilityScanner;

class QNetworkConfigurationManagerPrivate : public QObject, public CActive, public MConnectionMonitorObserver
{
    Q_OBJECT

public:
    QNetworkConfigurationManagerPrivate();
    virtual ~QNetworkConfigurationManagerPrivate(); 

    QNetworkConfiguration defaultConfiguration();
    void performAsyncConfigurationUpdate();

Q_SIGNALS:
    void configurationAdded(const QNetworkConfiguration& config);
    void configurationRemoved(const QNetworkConfiguration& config);
    void configurationUpdateComplete();
    void configurationChanged(const QNetworkConfiguration& config);
    void onlineStateChanged(bool isOnline);
    
    void configurationStateChanged(TUint32 accessPointId, TUint32 connMonId,
                                   QNetworkSession::State newState);
    
public Q_SLOTS:
    void updateConfigurations();
    void delayedConfigurationUpdate();

private:
    void registerPlatformCapabilities();
    void updateStatesToSnaps();
    bool changeConfigurationStateTo(QExplicitlySharedDataPointer<QNetworkConfigurationPrivate>& sharedData,
                                    QNetworkConfiguration::StateFlags newState);
    bool changeConfigurationStateAtMinTo(QExplicitlySharedDataPointer<QNetworkConfigurationPrivate>& sharedData,
                                         QNetworkConfiguration::StateFlags newState);
    bool changeConfigurationStateAtMaxTo(QExplicitlySharedDataPointer<QNetworkConfigurationPrivate>& sharedData,
                                          QNetworkConfiguration::StateFlags newState);
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    void updateMobileBearerToConfigs(TConnMonBearerInfo bearerInfo);
    QNetworkConfigurationPrivate* configFromConnectionMethodL(RCmConnectionMethod& connectionMethod);
#else
    bool readNetworkConfigurationValuesFromCommsDb(
            TUint32 aApId, QNetworkConfigurationPrivate* apNetworkConfiguration);
    void readNetworkConfigurationValuesFromCommsDbL(
            TUint32 aApId, QNetworkConfigurationPrivate* apNetworkConfiguration);
#endif    
    
    void updateConfigurationsL();
    void updateActiveAccessPoints();
    void updateAvailableAccessPoints();
    void accessPointScanningReady(TBool scanSuccessful, TConnMonIapInfo iapInfo);
    void startCommsDatabaseNotifications();
    void stopCommsDatabaseNotifications();
    void updateConfigurationsAfterRandomTime();

    QNetworkConfiguration defaultConfigurationL();
    TBool GetS60PlatformVersion(TUint& aMajor, TUint& aMinor) const;
    void startMonitoringIAPData(TUint32 aIapId);
    QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> dataByConnectionId(TUint aConnectionId);

protected:
    // From CActive
    void RunL();
    void DoCancel();
    
private:
    // MConnectionMonitorObserver
    void EventL(const CConnMonEventBase& aEvent);
    // For QNetworkSessionPrivate to indicate about state changes
    void configurationStateChangeReport(TUint32 accessPointId,
                                   QNetworkSession::State newState);

public: // Data
    //this table contains an up to date list of all configs at any time.
    //it must be updated if configurations change, are added/removed or
    //the members of ServiceNetworks change
    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > accessPointConfigurations;
    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > snapConfigurations;
    QHash<QString, QExplicitlySharedDataPointer<QNetworkConfigurationPrivate> > userChoiceConfigurations;
    QNetworkConfigurationManager::Capabilities capFlags;
    
private: // Data
    bool               iFirstUpdate; 
    CCommsDatabase*    ipCommsDB;
    RConnectionMonitor iConnectionMonitor;

    TBool              iWaitingCommsDatabaseNotifications;
    TBool              iOnline;
    TBool              iInitOk;
    TBool              iUpdateGoingOn;
    TBool              iUpdatePending;
    TUint              iTimeToWait;

    AccessPointsAvailabilityScanner* ipAccessPointsAvailabilityScanner;
    
    friend class QNetworkSessionPrivate;
    friend class AccessPointsAvailabilityScanner;

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    RCmManager iCmManager;
#endif
};

class AccessPointsAvailabilityScanner : public CActive
{
public:
    AccessPointsAvailabilityScanner(QNetworkConfigurationManagerPrivate& owner,
                                   RConnectionMonitor& connectionMonitor); 
    ~AccessPointsAvailabilityScanner();

    void StartScanning();
    
protected: // From CActive
    void RunL();
    void DoCancel();

private: // Data
    QNetworkConfigurationManagerPrivate& iOwner;
    RConnectionMonitor& iConnectionMonitor;
    TConnMonIapInfoBuf iIapBuf;
};

QTM_END_NAMESPACE

#endif //QNETWORKCONFIGURATIONMANAGERPRIVATE_H
