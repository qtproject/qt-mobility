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

#ifndef QREMOTESERVICECONTROL_S60_P_H
#define QREMOTESERVICECONTROL_S60_P_H

#define QT_SFW_SYMBIAN_IPC_DEBUG

#include "qremoteservicecontrol.h"
#include "qremoteserviceclassregister.h"
#include "qservicepackage_p.h"
#include <e32base.h>

#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
#include <QDebug>
#endif

#include <QQueue>

QTM_BEGIN_NAMESPACE

const TUint KServerMajorVersionNumber = 1;
const TUint KServerMinorVersionNumber = 0;
const TUint KServerBuildVersionNumber = 0;

enum TServiceProviderRequest
{
    EServicePackage = 1,
    EPackageRequest = 2,
    EPackageRequestCancel = 3,
    EPackageRequestComplete = 4
};


// Forward declarations
class ObjectEndPoint;
class CServiceProviderServerSession;
class CServiceProviderServer;
class SymbianServerEndPoint;
class SymbianClientEndPoint;

// Type definitions
typedef TPckgBuf<TInt> TError; 

#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
void printServicePackage(const QServicePackage& package);
#endif


// Internal class handling the actual communication with the service provider.
// Communication is based on standard Symbian client-server architecture.
class RServiceSession : public QObject, public RSessionBase
{
    Q_OBJECT
public: 
    RServiceSession(QString address);
    TInt Connect(); 
    void Close();
    TVersion Version() const;
    bool MessageAvailable();
    void SendServicePackage(const QServicePackage& aPackage);

    void ListenForPackages(TRequestStatus& aStatus); // TODO protected friend?
    void CancelListenForPackages();

 public:
    TBuf8<1550> iMessageFromServer; // TODO not necessarily enough (was 255)
    TPckgBuf<TInt> iState; // TPckgBuf type can be used directly as IPC parameter

signals:
    // void ReadyRead();
    void Disconnected(); // TODO not sure if this should be done like others

private:
    TInt StartServer();

private: 
    TIpcArgs iArgs; // These two are used in actively listening to server
    TError iError;
    QString iServerAddress;
};

// needed for creating server thread.
const TUint KDefaultHeapSize = 0x10000;

class CServiceProviderServer : public CServer2
    {
    public:
        CServiceProviderServer(QRemoteServiceControlPrivate* aOwner);
        CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

    public:

        void IncreaseSessions();
        void DecreaseSessions();

    private:

        int iSessionCount;
        QRemoteServiceControlPrivate* iOwner;
    };

class CServiceProviderServerSession : public CSession2
    {
    public:
        static CServiceProviderServerSession* NewL(CServiceProviderServer& aServer);
        static CServiceProviderServerSession* NewLC(CServiceProviderServer& aServer);
        virtual ~CServiceProviderServerSession();
        void ServiceL(const RMessage2& aMessage);
        void SetParent(SymbianServerEndPoint* aOwner);
        void SendServicePackage(const QServicePackage& aPackage);

        TInt HandleServicePackageL(const RMessage2& aMessage);
        void HandlePackageRequestL(const RMessage2& aMessage);
        void HandlePackageRequestCancelL(const RMessage2& aMessage);

    private:
        CServiceProviderServerSession(CServiceProviderServer& aServer);
        void ConstructL(); 

    private:
        CServiceProviderServer& iServer;
        SymbianServerEndPoint* iOwner;
        QByteArray* iByteArray;
        RMessage2 iMsg; // For replying pending service package requests
        QQueue<QServicePackage> iPendingPackageQueue;
        TBool iPendingPackageRequest;
    };


class QRemoteServiceControlPrivate: public QObject
{
    Q_OBJECT

public:
    QRemoteServiceControlPrivate(QObject* parent);
    void publishServices(const QString& ident );
    static QObject* proxyForService(const QRemoteServiceIdentifier& typeId, const QString& location);
    void processIncoming(CServiceProviderServerSession* session);
};

// A helper class that actively listens for serviceprovider messages.
// Needed because Symbian server cannot send messages without active request
// from the client.
class ServiceMessageListener : public CActive
{
public:
    ServiceMessageListener(RServiceSession* aSession, SymbianClientEndPoint* aOwner);
    ~ServiceMessageListener();

protected:
    void StartListening();
    // from CActive baseclass
    void DoCancel();
    void RunL();

private:
    RServiceSession* iClientSession;
    SymbianClientEndPoint* iOwnerEndPoint;
};

QTM_END_NAMESPACE

#endif // QREMOTESERVICECONTROL_S60_P_H
