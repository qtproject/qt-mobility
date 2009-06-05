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
#ifndef QNETWORKSESSIONPRIVATE_H
#define QNETWORKSESSIONPRIVATE_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QLibrary class.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include "qnetworksession.h"

#include <QDateTime>

#include <e32base.h>
#include <CommDbConnPref.h>
#include <es_sock.h>
#include <rconnmon.h>
#ifdef SNAP_FUNCTIONALITY_AVAILABLE
    #include <comms-infras/cs_mobility_apiext.h>
#endif

typedef int(*TOpenCSetdefaultifFunction)(const struct ifreq*);

QT_BEGIN_NAMESPACE

class ConnectionProgressNotifier;

#ifdef SNAP_FUNCTIONALITY_AVAILABLE
class QNetworkSessionPrivate : public CActive, public MMobilityProtocolResp, public MConnectionMonitorObserver
#else
class QNetworkSessionPrivate : public CActive, public MConnectionMonitorObserver
#endif
{
public:
    QNetworkSessionPrivate(); 
    ~QNetworkSessionPrivate();
    
    //called by QNetworkSession constructor and ensures
    //that the state is immediately updated (w/o actually opening
    //a session). Also this function should take care of 
    //notification hooks to discover future state changes.
    void syncStateWithInterface();

    QNetworkInterface currentInterface() const;
    QVariant property(const QString& key);
    QString bearerName() const;
    
    void setALREnabled(bool enabled);

    void open();
    void close();
    void stop();
    void migrate();
    void accept();
    void ignore();
    void reject();

    QString errorString() const; //must return translated string
    QNetworkSession::SessionError error() const;
    quint64 sentData() const;
    quint64 receivedData() const;
    quint64 activeTime() const;
    
    /**
     * Creates a connection to SNAP using ALR
     */
    void ConnectALRL();
    
#ifdef SNAP_FUNCTIONALITY_AVAILABLE    
public: // From MMobilityProtocolResp
    void PreferredCarrierAvailable(TAccessPointInfo aOldAPInfo,
                                   TAccessPointInfo aNewAPInfo,
                                   TBool aIsUpgrade,
                                   TBool aIsSeamless);

    void NewCarrierActive(TAccessPointInfo aNewAPInfo, TBool aIsSeamless);

    void Error(TInt aError);
#endif    

Q_SIGNALS:
    //releases any pending waitForOpened() calls
    void quitPendingWaitsForOpened();
    
protected: // From CActive
    void RunL();
    void DoCancel();
    
private: // MConnectionMonitorObserver
    void EventL(const CConnMonEventBase& aEvent);
    
private:    
    void newState(QNetworkSession::State newState);
    void handleSymbianConnectionStatusChange(TInt aConnectionStatus, TInt aError);
    QNetworkConfiguration activeConfiguration() const;

private: // data
    //the config set on QNetworkSession
    mutable QNetworkConfiguration publicConfig;

    //in case SNAP this one may be different to publicConfig
    mutable QNetworkConfiguration actualConfig;

    QNetworkSession::State state;
    bool isActive;

    QNetworkSession* q;
    QDateTime startTime;

    RLibrary iOpenCLibrary;
    TOpenCSetdefaultifFunction iDynamicSetdefaultif;

    RSocketServ iSocketServ;
    mutable RConnection iConnection;
    mutable RConnectionMonitor iConnectionMonitor;
    ConnectionProgressNotifier* ipConnectionNotifier;
#ifdef SNAP_FUNCTIONALITY_AVAILABLE    
    CActiveCommsMobilityApiExt* iMobility;
#endif    
    
    quint64 m_activeTime;
    
    TInt iS60Error;
    TInt iALREnabled;
    TBool iALRUpgradingConnection;

    friend class QNetworkSession;
    friend class ConnectionProgressNotifier;
};

class ConnectionProgressNotifier : public CActive
{
public:
    ConnectionProgressNotifier(QNetworkSessionPrivate& owner, RConnection& connection); 
    ~ConnectionProgressNotifier();
    
protected: // From CActive
    void RunL();
    void DoCancel();

private: // Data
    QNetworkSessionPrivate& owner;
    RConnection& iConnection;
    TNifProgressBuf iProgress;
};

QT_END_NAMESPACE

#endif //QNETWORKSESSIONPRIVATE_H

