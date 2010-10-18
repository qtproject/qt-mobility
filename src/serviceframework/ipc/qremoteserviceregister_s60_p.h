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

#ifndef QREMOTESERVICEREGISTER_S60_P_H
#define QREMOTESERVICEREGISTER_S60_P_H

//#define QT_SFW_SYMBIAN_IPC_DEBUG

#include "qremoteserviceregister.h"
#include "qremoteserviceregister_p.h"
//#include "qremoteserviceclassregister.h"
#include "qservicepackage_p.h"
#include "qservice.h"
#include <e32base.h>

#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
#include <QDebug>
#endif

#include <QQueue>

QTM_BEGIN_NAMESPACE

class ServiceMessageListener;

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
class QRemoteServiceRegisterSymbianPrivate;

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
    virtual ~RServiceSession();
    TInt Connect(); 
    void Close();
    TVersion Version() const;
    void SendServicePackage(const QServicePackage& aPackage);
    
 public:
    // 255 bytes seems to cover a lot of test cases in house
    // this size might need to be increased to avoid a lot
    // of context switches
    TBuf8<255> iMessageFromServer; 
    TPckgBuf<TInt> iSize; // TPckgBuf type can be used directly as IPC parameter

    void setListener(ServiceMessageListener* listener);

public slots:
     void ipcFailure(QService::UnrecoverableIPCError);
    
signals:
    void Disconnected();
    void errorUnrecoverableIPCFault(QService::UnrecoverableIPCError);

protected:
    void ListenForPackages(TRequestStatus& aStatus);
    void CancelListenForPackages();
    
private:
    TInt StartServer();

private: 
    TIpcArgs iArgs; // These two are used in actively listening to server
    TError iError;
    QString iServerAddress;
    ServiceMessageListener* iListener;
    
    friend class ServiceMessageListener;
};

// needed for creating server thread.
const TUint KDefaultHeapSize = 0x10000;

class CServiceProviderServer : public CPolicyServer
    {
    public:
        CServiceProviderServer(QRemoteServiceRegisterSymbianPrivate* aOwner);
        CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

    public:

        void IncreaseSessions();
        void DecreaseSessions();

        void setSecurityFilter(QRemoteServiceRegister::SecurityFilter filter);

    protected:
        virtual TCustomResult CustomSecurityCheckL(const RMessage2 &,TInt &,TSecurityInfo &);

    private:

        int iSessionCount;
        QRemoteServiceRegisterSymbianPrivate* iOwner;
        QRemoteServiceRegister::SecurityFilter iFilter;
    };

class CServiceProviderServerSession : public CSession2
    {
    public:
        static CServiceProviderServerSession* NewL(CServiceProviderServer& aServer);
        static CServiceProviderServerSession* NewLC(CServiceProviderServer& aServer);
        virtual ~CServiceProviderServerSession();
        void ServiceL(const RMessage2& aMessage);
        void SetParent(SymbianServerEndPoint* aOwner);
        void SendServicePackageL(const QServicePackage& aPackage);

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
        QByteArray iBlockData;
        int iTotalSize;
    };


class QRemoteServiceRegisterSymbianPrivate: public QRemoteServiceRegisterPrivate
{
    Q_OBJECT

public:
    QRemoteServiceRegisterSymbianPrivate(QObject* parent);
    void publishServices(const QString& ident );
    static QObject* proxyForService(const QRemoteServiceRegister::Entry& entry, const QString& location);
    void processIncoming(CServiceProviderServerSession* session);

    virtual QRemoteServiceRegister::SecurityFilter setSecurityFilter(QRemoteServiceRegister::SecurityFilter filter);

private:
    CServiceProviderServer *m_server;
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
    QByteArray iByteArray;
};

QTM_END_NAMESPACE

#endif // QREMOTESERVICEREGISTER_S60_P_H
