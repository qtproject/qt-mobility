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

#include "qremoteservicecontrol_s60_p.h"
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
  if(package.d) {
    qDebug() << "QServicePackage packageType  : " << package.d->packageType;
    qDebug() << "QServicePackage QUuid        : " << package.d->messageId;
    qDebug() << "QServicePackage responseType : " << package.d->responseType;
  }
  else {
    qDebug() << "Invalid ServicePackage" << " LEAVING"; 
    User::Leave(KErrCorrupt);
  }
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
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG        
        qDebug() << "Symbian IPC endpoint created. 255 buffer v3";
#endif
        // TODO does not exist / work / may be useless.
        // connect(session, SIGNAL(ReadyRead()), this, SLOT(readIncoming()));
        connect(session, SIGNAL(Disconnected()), this, SIGNAL(disconnected()));
    }

    ~SymbianClientEndPoint()
    {
    qDebug() << "Symbian IPC client endpoint destroyed.";
    }

    void PackageReceived(QServicePackage package)
    {
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
        qDebug() << "GTR SymbianClientEndPoint: package received. Enqueueing and emiting ReadyRead()";
#endif
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
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
        qDebug() << "Symbian IPC server endpoint created.";
#endif
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
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG      
        qDebug() << "GTR SymbianServerEndPoint::packageReceived, putting to queue and emiting readyread.";
#endif        
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
        TRAPD(err, session->SendServicePackageL(package));
        if(err != KErrNone){
          qDebug() << "Failed to send request: " << err;
        }
    }

private:
    CServiceProviderServerSession *session;
};

QRemoteServiceControlPrivate::QRemoteServiceControlPrivate(QObject *parent)
    : QObject(parent)
{
}

void QRemoteServiceControlPrivate::publishServices(const QString &ident)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "QRemoteServiceControlPrivate::publishServices() for ident: " << ident;
    qDebug("OTR TODO change publishServices to to return value ");
#endif    
    // Create service side of the Symbian Client-Server architecture.
    CServiceProviderServer *server = new CServiceProviderServer(this);
    TPtrC serviceIdent(reinterpret_cast<const TUint16*>(ident.utf16()));
    TInt err = server->Start(serviceIdent);
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG    
    if (err != KErrNone) {
        qDebug() << "RTR server->Start() failed, TODO return false.";
    } else {
        qDebug("GTR QRemoteServiceControlPrivate::server providing service started successfully");
    }
#endif
}

void QRemoteServiceControlPrivate::processIncoming(CServiceProviderServerSession* newSession)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG  
    qDebug("GTR Processing incoming session creation.");
#endif
    // Create service provider-side endpoints.
    SymbianServerEndPoint* ipcEndPoint = new SymbianServerEndPoint(newSession);
    ObjectEndPoint* endPoint = new ObjectEndPoint(ObjectEndPoint::Service, ipcEndPoint, this);
}

QObject* QRemoteServiceControlPrivate::proxyForService(const QRemoteServiceIdentifier &typeId, const QString &location)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "QRemoteServiceControlPrivate::proxyForService for location: " << location;
#endif
    // Create client-side session for the IPC and connect it to the service
    // provide. If service provider is not up, it will be started.
    // Connecting is tried few times in a loop, because if service starting is
    // done at device startup, everything may not be ready yet.
    RServiceSession *session = new RServiceSession(location.section(':', 1, 1));
    int err = session->Connect();
    int i = 0;
    while (err != KErrNone) {
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG      
        qDebug() << "QRemoteServiceControlPrivate::proxyForService Connecting in loop: " << i;
#endif        
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
    QObject::connect(session, SIGNAL(errorUnrecoverableIPCFault(QService::UnrecoverableIPCError)),
        proxy, SIGNAL(errorUnrecoverableIPCFault(QService::UnrecoverableIPCError)));
    return proxy;
}

RServiceSession::RServiceSession(QString address) 
: iSize(0)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "RServiceSession() for address: " << address;
#endif
    iServerAddress = address;
}

void RServiceSession::Close()
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "RServiceSession close()";
#endif
    RSessionBase::Close();
}

TInt RServiceSession::Connect()
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "RServiceSession Connect()";
#endif
    TInt err = StartServer();
    if (err == KErrNone) {
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
        qDebug() << "GTR StartServer() successful, Creating session.";
#endif
        TPtrC serviceAddressPtr(reinterpret_cast<const TUint16*>(iServerAddress.utf16()));
        err = CreateSession(serviceAddressPtr, Version());
    }
    return err;
}

TVersion RServiceSession::Version() const
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "RServiceSession Version()";
#endif
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
    TInt err = SendReceive(EServicePackage, args);
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG    
    qDebug() << "OTR SendPackage error handling, error code received: " << err;
#endif
    if(err != KErrNone){
      enum QService::UnrecoverableIPCError e  = QService::ErrorUnknown;
      switch(err){
      case KErrServerTerminated:
        e = QService::ErrorServiceNoLongerAvailable;
        break;
      case KErrNoMemory:
      case KErrServerBusy: // if the slots are full, something is really wrong        
        e = QService::ErrorOutofMemory;
        break;
      }
      qDebug() << "OTR SendPackage error handling, errorUnrecoverableIPCFault emitting2: " << e;
      emit errorUnrecoverableIPCFault(e);
    }
}

bool RServiceSession::MessageAvailable()
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
  // TODO look into this
    qDebug("RServiceSession::MessageAvailable OTR TODO");
#endif
    return false;
}

// StartServer() checks if the service is already published by someone (i.e. can be found
// from Kernel side). If not, it will start the process that provides the service.
TInt RServiceSession::StartServer()
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "RServiceSession::StartServer()";
#endif
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
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
        qDebug() << "RServiceSession::StartServer() GTR Service not found from Kernel. Starting a process.";
        qDebug() << "RServiceSession::StartServer() OTR TODO hard coded to start test service: " << "qservicemanager_ipc_service";
#endif
        TRequestStatus status;
        RProcess serviceServerProcess;
        _LIT(KServiceProviderServer, "qservicemanager_ipc_service");
        ret = serviceServerProcess.Create(KServiceProviderServer, KNullDesC);
        if (ret != KErrNone) {
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
            qDebug() << "RTR RProcess::Create failed";
#endif
            return ret;
        }
        // Point of synchronization. Waits until the started process calls
        // counterpart of this function (quits wait also if process dies / panics).
        serviceServerProcess.Rendezvous(status);
        if (status != KRequestPending) {
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
            qDebug() << "RTR Service Server Process Rendezvous() failed, killing process.";
#endif
            serviceServerProcess.Kill(KErrNone);
            serviceServerProcess.Close();
            return KErrGeneral;
        } else {
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG          
            qDebug() << "GTR Service Server Process Rendezvous() successful, resuming process.";
#endif
            serviceServerProcess.Resume();
        }
        User::WaitForRequest(status);
        if (status != KErrNone){
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
            qDebug("RTR Process Resume failed.");
#endif            
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
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "GTR RServiceSession::ListenForPackages(), iSize: " << iSize();
#endif
    iArgs.Set(0, &iMessageFromServer);
    // Total Size of returned messaage,which might differ from the amount of data in iMessageFromServer
    iArgs.Set(1, &iSize); 
    iArgs.Set(2, &iError); // TODO Not sure if needed
    // TODO needs to be enough room for response, currently is not
    // and also the possibility for error codes needs to be analyzed -
    // is there need for communicating errors here.
    SendReceive(EPackageRequest, iArgs, aStatus);
}

void RServiceSession::CancelListenForPackages()
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG  
    qDebug("RServiceSession::CancelListenForPackages");
#endif
    TInt err = SendReceive(EPackageRequestCancel, TIpcArgs(NULL));
    if(err != KErrNone){
      enum QService::UnrecoverableIPCError e = QService::ErrorUnknown;
      switch(err){
      case KErrServerTerminated:        
        e = QService::ErrorServiceNoLongerAvailable;
        break;
      case KErrNoMemory:
      case KErrServerBusy: // if the slots are full, something is really wrong        
        e = QService::ErrorOutofMemory;
        break;
      }
      qDebug() << "RServiceSession::CancelListenForPackages failed with error: " << e;
      emit errorUnrecoverableIPCFault(e);
    }
}

void RServiceSession::ipcFailure(QService::UnrecoverableIPCError err)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG  
    qDebug("RServiceSession::ipcFailure ipc Fault reported");
#endif
  emit errorUnrecoverableIPCFault(err);
}

CServiceProviderServer::CServiceProviderServer(QRemoteServiceControlPrivate* aOwner)
    : CServer2(EPriorityNormal), iSessionCount(0), iOwner(aOwner)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug("CServiceProviderServer constructor");
#endif
    Q_ASSERT(aOwner);
}

CSession2* CServiceProviderServer::NewSessionL(const TVersion &aVersion, const RMessage2 &aMessage) const
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug("CServiceProviderServer::NewSessionL()");
#endif
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
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug("CServiceProviderServerSession::NewL()");
#endif
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
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug("CServiceProviderServerSession constructor");
#endif
    iServer.IncreaseSessions();
}

void CServiceProviderServerSession::ConstructL()
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug("OTR CServiceProviderServerSession::ConstructL() TODO empty");
#endif
    iTotalSize = 0;    // No data
    iBlockData.clear();// clear the buffer
}

CServiceProviderServerSession::~CServiceProviderServerSession()
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug("CServiceProviderServerSession destructor");
#endif
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
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG      
        qDebug("OTR TODO HandleServicePackageL. Message read failed.");
#endif
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
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug("HandlePackageRequestL(). Setting pending true and storing message.");
#endif
    iMsg = aMessage;
    iPendingPackageRequest = ETrue;
    if(!iPendingPackageQueue.isEmpty())
      SendServicePackageL(iPendingPackageQueue.dequeue());
}

void CServiceProviderServerSession::HandlePackageRequestCancelL(const RMessage2& /*aMessage*/)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug("HandlePackageRequestCancelL");
#endif
    if (iPendingPackageRequest) {
        iMsg.Complete(KErrCancel);
        iPendingPackageRequest = EFalse;
    }
}

void CServiceProviderServerSession::SendServicePackageL(const QServicePackage& aPackage)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug("CServiceProviderServerSession:: SendServicePackage for package: ");
#endif
    printServicePackage(aPackage);
    if (iPendingPackageRequest) {
        if(iBlockData.isEmpty()){
          // Serialize the package        
          QDataStream out(&iBlockData, QIODevice::WriteOnly);
          out.setVersion(QDataStream::Qt_4_6);
          out << aPackage;
          iTotalSize = iBlockData.size();
        }
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
        qDebug() << "Size of package sent from server to client: " << iBlockData.count();               
        qDebug() << "Size of buffer from client: " << iMsg.GetDesMaxLength(0);
#endif
        
        int size = iBlockData.size();
        if(size > iMsg.GetDesMaxLength(0)){
          size = iMsg.GetDesMaxLength(0);
          // enequeue the package so we send the  next chunk
          // when the next request comes through
          iPendingPackageQueue.enqueue(aPackage); 
        }
        TPtrC8 ptr8((TUint8*)(iBlockData.constData()), size);      
        iMsg.WriteL(0, ptr8);
        iBlockData.remove(0, size);
//#ifdef QT_SFW_SYMBIAN_IPC_DEBUG        
//        if(status == KErrOverflow){
//          qDebug() << "OTR Server to client, got overflow, sending 0 bytes";
//        }
//        else if(status != KErrNone){
//          qDebug() << "OTR SendServicePackage: error code from send: " << status;
//        }
//#endif
        TPckgBuf<TInt> totalSize(iTotalSize);
        iMsg.WriteL(1,totalSize);                
        iMsg.Complete(EPackageRequestComplete);
        iPendingPackageRequest = EFalse;
    } else {
        iPendingPackageQueue.enqueue(aPackage);
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG        
        qWarning() << "RTR SendServicePackage: service package from server to client queued - no pending receive request.";
#endif
    }
}

ServiceMessageListener::ServiceMessageListener(RServiceSession* aSession, SymbianClientEndPoint* aOwner)
    : CActive(EPriorityNormal),
    iClientSession(aSession),
    iOwnerEndPoint(aOwner)
{
    Q_ASSERT(iClientSession);
    Q_ASSERT(iOwnerEndPoint);
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG    
    qDebug("ServiceMessageListener constructor");
#endif
    CActiveScheduler::Add(this);
    StartListening();
}

ServiceMessageListener::~ServiceMessageListener()
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug("ServiceMessageListener destructor");
#endif
    Cancel();
}

void ServiceMessageListener::StartListening()
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug("ServiceMessageListener::StartListening");
#endif
    iClientSession->ListenForPackages(iStatus);
    SetActive();
}

void ServiceMessageListener::DoCancel()
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG  
    qDebug("ServiceMessageListener::DoCancel");
#endif
    iClientSession->CancelListenForPackages();
}

void ServiceMessageListener::RunL()
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "ServiceMessageListener::RunL for iStatus.Int(should be 4): " << iStatus.Int();
#endif
    if (iStatus.Int() == EPackageRequestComplete) {
        // Client side has received a service package from server. Pass it onwards and
        // issue new pending request.
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
        qDebug() << "RunL length of the package received client side is: " << iClientSession->iMessageFromServer.Length();
#endif
        if(iClientSession->iMessageFromServer.Length() == 0){
          // we received 0 bytes from the other side, 
          // normally because it tried to write more bytes 
          // than were in the TDes
          User::Leave(KErrTooBig); 
        }
                
        iByteArray.append((const char*)iClientSession->iMessageFromServer.Ptr(),
                             iClientSession->iMessageFromServer.Length());
        if(iByteArray.length() >= iClientSession->iSize()){        
          QDataStream in(iByteArray);
          iByteArray.clear();
          QServicePackage results;
          in >> results;
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
          qDebug() << "ServiceMessageListener Reproduced service package: ";
          printServicePackage(results);
#endif
          iOwnerEndPoint->PackageReceived(results);
        }          
        StartListening();
    }
    else if(iStatus.Int() < 0){
      TInt s = iStatus.Int();
      switch(s){
      case KErrServerTerminated:
        iClientSession->ipcFailure(QService::ErrorServiceNoLongerAvailable);
        break;
      case KErrServerBusy:
      case KErrNoMemory:
        iClientSession->ipcFailure(QService::ErrorOutofMemory);
        break;
      }
    }
}

#include "moc_qremoteservicecontrol_s60_p.cpp"
#include "qremoteservicecontrol_s60.moc"
QTM_END_NAMESPACE
