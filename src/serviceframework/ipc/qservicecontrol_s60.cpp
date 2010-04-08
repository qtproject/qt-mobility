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

#include "qservicecontrol_s60_p.h"
#include "ipcendpoint_p.h"
#include "objectendpoint_p.h"
#include <QTimer>

/* IPC based on Symbian Client-Server framework
 * This module implements the Symbian specific IPC mechanisms and related control.
 * IPC is based on Symbian Client-Server architecture.
 */

QTM_BEGIN_NAMESPACE

#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
void printServicePackage(const QServicePackage& package)
{
    qDebug() << "QServicePackage packageType  : " << package.d->packageType;
    qDebug() << "QServicePackage QUuid        : " << package.d->messageId;
    qDebug() << "QServicePackage responseType : " << package.d->responseType;
}
#endif


class SymbianClientEndPoint: public QServiceIpcEndPoint
{
    Q_OBJECT
public:
    SymbianClientEndPoint(RServiceSession* session, QObject* parent = 0)
        : QServiceIpcEndPoint(parent), session(session)
    {
        Q_ASSERT(session);
        qDebug() << "Symbian IPC endpoint created.";
        // TODO does not exist / work / may be useless.
        // connect(session, SIGNAL(ReadyRead()), this, SLOT(readIncoming()));
        // connect(session, SIGNAL(Disconnected()), this, SIGNAL(disconnected()));
    }

    ~SymbianClientEndPoint()
    {
    qDebug() << "Symbian IPC client endpoint destroyed.";
    }

    void PackageReceived(QServicePackage package)
    {
        qDebug() << "GTR SymbianClientEndPoint: package received. Enqueueing and emiting ReadyRead()";
        incoming.enqueue(package);
        emit readyRead();
    }

protected:
    void flushPackage(const QServicePackage& package)
    {
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
        qDebug() << "SymbianClientEndPoint::flushPackage() for package: ";
        printServicePackage(package);
#endif
        session->SendServicePackage(package);
    }

private:
    RServiceSession *session;
};


class SymbianServerEndPoint: public QServiceIpcEndPoint
{
    Q_OBJECT
public:
    SymbianServerEndPoint(CServiceProviderServerSession* session, QObject* parent = 0)
        : QServiceIpcEndPoint(parent), session(session)
    {
        qDebug() << "Symbian IPC server endpoint created.";
        Q_ASSERT(session);
        // CBase derived classes cannot inherit from QObject,
        // hence manual ownershipping instead of Qt hierarchy.
        session->SetParent(this);
    }

    ~SymbianServerEndPoint()
    {
        qDebug() << "Symbian IPC server endpoint destroyed.";
    }

    void packageReceived(QServicePackage package)
    {
        qDebug() << "GTR SymbianServerEndPoint::packageReceived, putting to queue and emiting readyread.";
        incoming.enqueue(package);
        emit readyRead();
    }

protected:
    void flushPackage(const QServicePackage& package)
    {
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
        qDebug() << "SymbianServerEndPoint::flushPackage() for package: ";
        printServicePackage(package);
#endif
        session->SendServicePackage(package);
    }

private:
    CServiceProviderServerSession *session;
};

QServiceControlPrivate::QServiceControlPrivate(QObject *parent)
    : QObject(parent)
{
}

void QServiceControlPrivate::publishServices(const QString &ident)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "QServiceControlPrivate::publishServices() for ident: " << ident;
#endif
    qDebug("OTR TODO change publishServices to to return value ");
    // Create service side of the Symbian Client-Server architecture.
    CServiceProviderServer *server = new CServiceProviderServer(this);
    TPtrC serviceIdent(reinterpret_cast<const TUint16*>(ident.utf16()));
    TInt err = server->Start(serviceIdent);
    if (err != KErrNone) {
        qDebug() << "RTR server->Start() failed, TODO return false.";
    } else {
        qDebug("GTR QServiceControlPrivate::server providing service started successfully");
    }
}

void QServiceControlPrivate::processIncoming(CServiceProviderServerSession* newSession)
{
    qDebug("GTR Processing incoming session creation.");
    // Create service provider-side endpoints.
    SymbianServerEndPoint* ipcEndPoint = new SymbianServerEndPoint(newSession);
    ObjectEndPoint* endPoint = new ObjectEndPoint(ObjectEndPoint::Service, ipcEndPoint, this);
}

QObject* QServiceControlPrivate::proxyForService(const QRemoteServiceIdentifier &typeId, const QString &location)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "QServiceControlPrivate::proxyForService for location: " << location;
#endif
    // Create client-side session for the IPC and connect it to the service
    // provide. If service provider is not up, it will be started.
    // Connecting is tried few times in a loop, because if service starting is
    // done at device startup, everything may not be ready yet.
    RServiceSession *session = new RServiceSession(location.section(':', 1, 1));
    int err = session->Connect();
    int i = 0;
    while (err != KErrNone) {
        qDebug() << "QServiceControlPrivate::proxyForService Connecting in loop: " << i;
        if (i > 10) {
            qWarning() << "QtSFW failed to connect to service provider.";
            return NULL;
        }
        User::After(50);
        err = session->Connect();
        i++;
    }

    // Create IPC endpoint. In practice binds the communication session and abstracting
    // class presenting the IPC endoint.
    SymbianClientEndPoint* ipcEndPoint = new SymbianClientEndPoint(session);
    // Create an active message solicitor, which listens messages from server
    ServiceMessageListener* messageListener = new ServiceMessageListener(session, ipcEndPoint);
    // Create object endpoint, which handles the metaobject protocol.
    ObjectEndPoint* endPoint = new ObjectEndPoint(ObjectEndPoint::Client, ipcEndPoint);
    QObject *proxy = endPoint->constructProxy(typeId);
    QObject::connect(proxy, SIGNAL(destroyed()), endPoint, SLOT(deleteLater()));
    return proxy;
}

RServiceSession::RServiceSession(QString address)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "RServiceSession() for address: " << address;
#endif
    iServerAddress = address;
}

void RServiceSession::Close()
{
    qDebug() << "RServiceSession close()";
    RSessionBase::Close();
}

TInt RServiceSession::Connect()
{
    qDebug() << "RServiceSession Connect()";
    TInt err = StartServer();
    if (err == KErrNone) {
        qDebug() << "GTR StartServer() successful, Creating session.";
        TPtrC serviceAddressPtr(reinterpret_cast<const TUint16*>(iServerAddress.utf16()));
        err = CreateSession(serviceAddressPtr, Version());
    }
    return err;
}

TVersion RServiceSession::Version() const
{
    qDebug() << "RServiceSession Version()";
    return TVersion(KServerMajorVersionNumber, KServerMinorVersionNumber, KServerBuildVersionNumber);
}

void RServiceSession::SendServicePackage(const QServicePackage& aPackage)
{
    // Serialize the package into byte array, wrap it in descriptor,
    // and send to service provider.
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << aPackage;
    qDebug() << "Size of package sent from client to server: " << block.count();
    TPtrC8 ptr8((TUint8*)(block.constData()), block.size());
    TIpcArgs args(&ptr8, &iError);
    SendReceive(EServicePackage, args);
    qDebug() << "OTR TODO SendPackage error handling, error code received: " << iError();
}

bool RServiceSession::MessageAvailable()
{
    qDebug("RServiceSession::MessageAvailable OTR TODO");
    return false;
}

// StartServer() checks if the service is already published by someone (i.e. can be found
// from Kernel side). If not, it will start the process that provides the service.
TInt RServiceSession::StartServer()
{
    qDebug() << "RServiceSession::StartServer()";
    TInt ret = KErrNone;
    TPtrC serviceAddressPtr(reinterpret_cast<const TUint16*>(iServerAddress.utf16()));
    TFindServer findServer(serviceAddressPtr);
    TFullName name;
    // Looks up from Kernel-side if there are active servers with the given name.
    // If not found, a process providing the service needs to be started.
    // TODO how to map service address to executable file name.
    if (findServer.Next(name) != KErrNone) {
#ifdef __WINS__
        qWarning("WINS Support for QSFW OOP not implemented.");
#else
        qDebug() << "RServiceSession::StartServer() GTR Service not found from Kernel. Starting a process.";
        qDebug() << "RServiceSession::StartServer() OTR TODO hard coded to start test service: " << "qservicemanager_ipc_service";
        TRequestStatus status;
        RProcess serviceServerProcess;
        _LIT(KServiceProviderServer, "qservicemanager_ipc_service");
        ret = serviceServerProcess.Create(KServiceProviderServer, KNullDesC);
        if (ret != KErrNone) {
            qDebug() << "RTR RProcess::Create failed";
            return ret;
        }
        // Point of synchronization. Waits until the started process calls
        // counterpart of this function (quits wait also if process dies / panics).
        serviceServerProcess.Rendezvous(status);
        if (status != KRequestPending) {
            qDebug() << "RTR Service Server Process Rendezvous() failed, killing process.";
            serviceServerProcess.Kill(KErrNone);
            serviceServerProcess.Close();
            return KErrGeneral;
        } else {
            qDebug() << "GTR Service Server Process Rendezvous() successful, resuming process.";
            serviceServerProcess.Resume();
        }
        User::WaitForRequest(status);
        if (status != KErrNone){
            qDebug("RTR Process Resume failed.");
            serviceServerProcess.Close();
            return status.Int();
        }
        // Free the handle to the process (RHandleBase function, does not 'close process')
        serviceServerProcess.Close();
#endif // __WINS__
    } else {
        qDebug() << "RServiceSession::StartServer() GTR Service found from Kernel, no need to start process.";
    }
    return ret;
}

void RServiceSession::ListenForPackages(TRequestStatus& aStatus)
{
    qDebug("GTR RServiceSession::ListenForPackages()");
    iArgs.Set(0, &iMessageFromServer);
    iArgs.Set(1, &iState); // TODO Not sure if needed
    iArgs.Set(2, &iError); // TODO Not sure if needed
    // TODO needs to be enough room for response, currently is not
    // and also the possibility for error codes needs to be analyzed -
    // is there need for communicating errors here.
    SendReceive(EPackageRequest, iArgs, aStatus);
}

void RServiceSession::CancelListenForPackages()
{
    qDebug("RServiceSession::ListenForPackages");
    SendReceive(EPackageRequestCancel, TIpcArgs(NULL));
}

CServiceProviderServer::CServiceProviderServer(QServiceControlPrivate* aOwner)
    : CServer2(EPriorityNormal), iSessionCount(0), iOwner(aOwner)
{
    qDebug("CServiceProviderServer constructor");
    Q_ASSERT(aOwner);
}

CSession2* CServiceProviderServer::NewSessionL(const TVersion &aVersion, const RMessage2 &aMessage) const
{
    qDebug("CServiceProviderServer::NewSessionL()");
    if (!User::QueryVersionSupported(TVersion(KServerMajorVersionNumber,
                                              KServerMinorVersionNumber, KServerBuildVersionNumber), aVersion))
    {
        User::Leave(KErrNotSupported);
    }
    CServiceProviderServerSession* session = CServiceProviderServerSession::NewL(*const_cast<CServiceProviderServer*>(this));
    iOwner->processIncoming(session);
    return session;
}

void CServiceProviderServer::IncreaseSessions()
{
    iSessionCount++;
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "CServiceProviderServer incremented session count to: " << iSessionCount;
#endif
}

void CServiceProviderServer::DecreaseSessions()
{
    iSessionCount--;
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "CServiceProviderServer decremented session count to: " << iSessionCount;
#endif
}

CServiceProviderServerSession *CServiceProviderServerSession::NewL(CServiceProviderServer &aServer)
{
    qDebug("CServiceProviderServerSession::NewL()");
    CServiceProviderServerSession *self = CServiceProviderServerSession::NewLC(aServer);
    CleanupStack::Pop(self);
    return self;
}

CServiceProviderServerSession *CServiceProviderServerSession::NewLC(CServiceProviderServer &aServer)
{
    CServiceProviderServerSession* self = new (ELeave) CServiceProviderServerSession(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CServiceProviderServerSession::CServiceProviderServerSession(CServiceProviderServer &aServer)
    : iServer(aServer)
{
    qDebug("CServiceProviderServerSession constructor");
    iServer.IncreaseSessions();
}

void CServiceProviderServerSession::ConstructL()
{
    qDebug("OTR CServiceProviderServerSession::ConstructL() TODO empty");
}

CServiceProviderServerSession::~CServiceProviderServerSession()
{
    qDebug("CServiceProviderServerSession destructor");
    iServer.DecreaseSessions();
    delete iByteArray;
}

void CServiceProviderServerSession::ServiceL(const RMessage2 &aMessage)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "CServiceProviderServerSession::ServiceL for message: " << aMessage.Function();
#endif
    // TODO leaving needs to be analyzed if OK
    switch (aMessage.Function()) 
    {
    case EServicePackage:
        User::LeaveIfError(HandleServicePackageL(aMessage));
        aMessage.Complete(KErrNone);
        break;
    case EPackageRequest:
        //User::LeaveIfError(HandlePackageRequestL(aMessage));
        // Message is completed only when we have signals to transfer.
        HandlePackageRequestL(aMessage);
        break;
    case EPackageRequestCancel:
        //User::LeaveIfError(HandlePackageRequestCancelL(aMessage));
        HandlePackageRequestCancelL(aMessage);
        // TODO
        break;
    }
}

TInt CServiceProviderServerSession::HandleServicePackageL(const RMessage2& aMessage)
{
    TInt ret = KErrNone;
    // Reproduce the serialized data.
    HBufC8* servicePackageBuf8 = HBufC8::New(aMessage.GetDesLength(0));
    if (!servicePackageBuf8) {
        return KErrNoMemory;
    }

    TPtr8 ptrToBuf(servicePackageBuf8->Des());
    TRAP(ret, aMessage.ReadL(0, ptrToBuf));
    if (ret != KErrNone) {
        qDebug("OTR TODO HandleServicePackageL. Message read failed.");
        //iDb->lastError().setError(DBError::UnknownError);
        //aMessage.Write(1, LastErrorCode());
        delete servicePackageBuf8;
        return ret;
    }

    QByteArray byteArray((const char*)ptrToBuf.Ptr(), ptrToBuf.Length());
    QDataStream in(byteArray);
    QServicePackage results;
    in >> results;

#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "CServiceProviderServerSession Reproduced service package: ";
    printServicePackage(results);
#endif
    iOwner->packageReceived(results);
    return ret;
}

void CServiceProviderServerSession::SetParent(SymbianServerEndPoint *aOwner)
{
    iOwner = aOwner;
}

void CServiceProviderServerSession::HandlePackageRequestL(const RMessage2& aMessage)
{
    qDebug("HandlePackageRequestL(). Setting pending true and storing message.");
    iMsg = aMessage;
    iPendingPackageRequest = ETrue;
}

void CServiceProviderServerSession::HandlePackageRequestCancelL(const RMessage2& /*aMessage*/)
{
    qDebug("HandlePackageRequestCancelL");
    if (iPendingPackageRequest) {
        iMsg.Complete(KErrCancel);
        iPendingPackageRequest = EFalse;
    }
}

void CServiceProviderServerSession::SendServicePackage(const QServicePackage& aPackage)
{
    qDebug("CServiceProviderServerSession:: SendServicePackage for package: ");
    printServicePackage(aPackage);
    if (iPendingPackageRequest) {
        // Serialize the package
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_4_6);
        out << aPackage;
        qDebug() << "Size of package sent from server to client: " << block.count();
        TPtrC8 ptr8((TUint8*)(block.constData()), block.size());

        TPckgBuf<TInt> state(0);
        iMsg.Write(0, ptr8);
        // iMsg.Write(1, state); // TODO not sure if needed
        // iMsg.Write(2, ); // TODO not sure if needed
        iMsg.Complete(EPackageRequestComplete);
        iPendingPackageRequest = EFalse;
    } else {
        qWarning("RTR SendServicePackage: service package from server to client dropped - no pending receive request.");
    }
}

ServiceMessageListener::ServiceMessageListener(RServiceSession* aSession, SymbianClientEndPoint* aOwner)
    : CActive(EPriorityNormal),
    iClientSession(aSession),
    iOwnerEndPoint(aOwner)
{
    Q_ASSERT(iClientSession);
    Q_ASSERT(iOwnerEndPoint);
    qDebug("ServiceMessageListener constructor");
    CActiveScheduler::Add(this);
    StartListening();
}

ServiceMessageListener::~ServiceMessageListener()
{
    qDebug("ServiceMessageListener destructor");
    Cancel();
}

void ServiceMessageListener::StartListening()
{
    qDebug("ServiceMessageListener::StartListening");
    iClientSession->ListenForPackages(iStatus);
    SetActive();
}

void ServiceMessageListener::DoCancel()
{
    qDebug("ServiceMessageListener::DoCancel");
    iClientSession->CancelListenForPackages();
}

void ServiceMessageListener::RunL()
{
    qDebug() << "ServiceMessageListener::RunL for iStatus.Int(should be 4): " << iStatus.Int();
    if (iStatus.Int() == EPackageRequestComplete) {
        // Client side has received a service package from server. Pass it onwards and
        // issue new pending request.
        qDebug() << "RunL length of the package received client side is: " << iClientSession->iMessageFromServer.Length();
        QByteArray byteArray((const char*)iClientSession->iMessageFromServer.Ptr(),
                             iClientSession->iMessageFromServer.Length());
        QDataStream in(byteArray);
        QServicePackage results;
        in >> results;

#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
        qDebug() << "ServiceMessageListener Reproduced service package: ";
        printServicePackage(results);
#endif
        iOwnerEndPoint->PackageReceived(results);
        StartListening();
    }
}

#include "moc_qservicecontrol_s60_p.cpp"
#include "qservicecontrol_s60.moc"
QTM_END_NAMESPACE
