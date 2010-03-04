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

/* IPC based on Symbian Client-Server framework
 * This module implements the Symbian specific IPC mechanisms and related control.
 * IPC is based on Symbian Client-Server architecture.
 */

QTM_BEGIN_NAMESPACE

class SymbianClientServerEndPoint: public QServiceIpcEndPoint
{
    Q_OBJECT
public:
    SymbianClientServerEndPoint(RServiceSession* session, QObject* parent = 0)
        : QServiceIpcEndPoint(parent), session(session)
    {
    qDebug() << "Symbian IPC endpoint created.";
    }

    ~SymbianClientServerEndPoint()
    {
    qDebug() << "Symbian IPC endpoint destroyed.";
    }

protected:
    void flushPackage(const QServicePackage& package)
    {
    qDebug() << "OTR flushPackage TODO";
    }

private:
    RServiceSession *session;
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
    CServiceProviderServer *server = new CServiceProviderServer;
    TPtrC serviceIdent(reinterpret_cast<const TUint16*>(ident.utf16()));
    TInt err = server->Start(serviceIdent);
    if (err != KErrNone) {
        qDebug() << "RTR server->Start() failed, TODO return false.";
    } else {
        qDebug("GTR QServiceControlPrivate::server providing service started successfully");
    }
}

QObject* QServiceControlPrivate::proxyForService(const QServiceTypeIdent &typeId, const QString &location)
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
    qDebug("OTR TODO (proxyForService) all following properly:");
    SymbianClientServerEndPoint* ipcEndPoint = new SymbianClientServerEndPoint(session);
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

CServiceProviderServer::CServiceProviderServer()
    : CServer2(EPriorityNormal), iSessionCount(0)
{
    qDebug("CServiceProviderServer constructor");
}

CSession2* CServiceProviderServer::NewSessionL(const TVersion &aVersion, const RMessage2 &aMessage) const
{
    qDebug("CServiceProviderServer::NewSessionL()");
    if (!User::QueryVersionSupported(TVersion(KServerMajorVersionNumber,
            KServerMinorVersionNumber, KServerBuildVersionNumber), aVersion))
        {
        qDebug("RTR NewSessionL() Version not supported");
        User::Leave(KErrNotSupported);
        }
    return CServiceProviderServerSession::NewL(*const_cast<CServiceProviderServer*>(this));
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
    iSessionCount++;
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
    iServer.IncreaseSessions();
    delete iByteArray;
}

void CServiceProviderServerSession::ServiceL(const RMessage2 &aMessage)
{
#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
    qDebug() << "CServiceProviderServerSession::ServiceL for message: " << aMessage.Function();
#endif
    qDebug("OTR TODO, now just completing message in ServiceL without handling it.");
    aMessage.Complete(KErrNone);
}

#include "moc_qservicecontrol_s60_p.cpp"
#include "qservicecontrol_s60.moc"
QTM_END_NAMESPACE
