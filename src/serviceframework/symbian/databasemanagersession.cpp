/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "clientservercommon.h"
#include "databasemanagersession.h"
#include "databasemanagerserver.pan"
#include "databasemanagersignalhandler.h"
#include "servicedatabase_p.h"
#include "databasemanagerserver.h"
    
#include <QFileSystemWatcher>

QTM_BEGIN_NAMESPACE

bool lessThan(const QServiceInterfaceDescriptor &d1,
                                        const QServiceInterfaceDescriptor &d2)
    {
    return (d1.majorVersion() < d2.majorVersion())
            || ( d1.majorVersion() == d2.majorVersion()
            && d1.minorVersion() < d2.minorVersion());
    }

CDatabaseManagerServerSession* CDatabaseManagerServerSession::NewL(CDatabaseManagerServer& aServer)
    {
    CDatabaseManagerServerSession* self = CDatabaseManagerServerSession::NewLC(aServer);
    CleanupStack::Pop(self);
    return self;
    }

CDatabaseManagerServerSession* CDatabaseManagerServerSession::NewLC(CDatabaseManagerServer& aServer)
    {
    CDatabaseManagerServerSession* self = new (ELeave) CDatabaseManagerServerSession(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CDatabaseManagerServerSession::ConstructL()
    {
    iDb = new ServiceDatabase();
    initDbPath();
    iDatabaseManagerSignalHandler = new DatabaseManagerSignalHandler(*this);
    }

CDatabaseManagerServerSession::CDatabaseManagerServerSession(CDatabaseManagerServer& aServer) 
    : iServer(aServer),
      iDatabaseManagerSignalHandler(NULL),
      iDb(NULL),
      m_watcher(NULL)
    {
    iServer.IncreaseSessions();
    }

CDatabaseManagerServerSession::~CDatabaseManagerServerSession()
    {
    iServer.DecreaseSessions();
    delete iDatabaseManagerSignalHandler;
    delete iDb;
    delete iByteArray;
    delete m_watcher;
    }

void CDatabaseManagerServerSession::ServiceL(const RMessage2& aMessage)
    {
    if (aMessage.Function() == ENotifyServiceSignalRequest)
        {
        NotifyServiceSignal(aMessage);
        }
    else
        {
        TRAPD(err, DispatchMessageL(aMessage));
        aMessage.Complete(err);
        }       
    }
	        
void CDatabaseManagerServerSession::DispatchMessageL(const RMessage2& aMessage)
    {
        switch (aMessage.Function())
        {
            case ERegisterServiceRequest:
                User::LeaveIfError(RegisterServiceL(aMessage));
                break;
            case EUnregisterServiceRequest:
                User::LeaveIfError(UnregisterServiceL(aMessage));
                break;
            case EGetInterfacesRequest:
                User::LeaveIfError(InterfacesL(aMessage));
                break;
            case EGetInterfacesSizeRequest:
                User::LeaveIfError(InterfacesSizeL(aMessage));
                break;
            case EGetServiceNamesRequest:
                User::LeaveIfError(ServiceNamesL(aMessage));
                break;
            case EGetServiceNamesSizeRequest:
                User::LeaveIfError(ServiceNamesSizeL(aMessage));
                break;
            case EInterfaceDefaultRequest:
                User::LeaveIfError(InterfaceDefaultL(aMessage));
                break;
            case ESetInterfaceDefault:
                User::LeaveIfError(SetInterfaceDefaultL(aMessage));
                break;
            case ESetInterfaceDefault2:
                User::LeaveIfError(SetInterfaceDefault2L(aMessage));
                break;
            case ESetChangeNotificationsEnabledRequest:
                SetChangeNotificationsEnabled(aMessage);
                break;
            case EInterfaceDefaultSizeRequest:
                User::LeaveIfError(InterfaceDefaultSize(aMessage));
                break;
            case ECancelNotifyServiceSignalRequest:
                User::LeaveIfError(CancelNotifyServiceSignal(aMessage));
                break;
            default:
                aMessage.Panic(KUnknownOpCode, KErrNotSupported);
                break;
        }
    }

TInt CDatabaseManagerServerSession::InterfaceDefaultSize(const RMessage2& aMessage)
    {
    TInt ret;
    HBufC* defaultInterfaceBuf = HBufC::New(aMessage.GetDesLength(0));
    if (!defaultInterfaceBuf)
        return KErrNoMemory;
    
    TPtr ptrToBuf(defaultInterfaceBuf->Des());
    TRAP(ret, aMessage.ReadL(0, ptrToBuf));
    
    
    QString interfaceName = QString::fromUtf16(ptrToBuf.Ptr(), ptrToBuf.Length());   
    QServiceInterfaceDescriptor descriptor;
    descriptor = iDb->interfaceDefault(interfaceName);
    
    iByteArray = new QByteArray();
    
    QDataStream in(iByteArray, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_4_6);
    in << descriptor;

    TPckgBuf<TInt> size(iByteArray->size());
    
    aMessage.Write(1, size);
    aMessage.Write(2, LastErrorCode());
    delete defaultInterfaceBuf;
    
    return ret;
    }

TInt CDatabaseManagerServerSession::InterfaceDefaultL(const RMessage2& aMessage)
    {
    TPtrC8 defaultInterfacePtr8((TUint8*)(iByteArray->constData()), iByteArray->size());
    aMessage.Write(0, defaultInterfacePtr8);
    delete iByteArray;
    iByteArray = NULL;

    return 0;
    }

void CDatabaseManagerServerSession::NotifyServiceSignal(const RMessage2& aMessage)
    {
    iMsg = aMessage;
    iWaitingAsyncRequest = ETrue;
    }

TInt CDatabaseManagerServerSession::CancelNotifyServiceSignal(const RMessage2& /*aMessage*/)
    {
    if (iWaitingAsyncRequest)
        {
        iMsg.Complete(KErrCancel);
        iWaitingAsyncRequest  = EFalse;
        }
    
    return KErrNone;
    }

TInt CDatabaseManagerServerSession::RegisterServiceL(const RMessage2& aMessage)
    {    
    QString securityToken;
    
    TVendorId vendorId = aMessage.VendorId();
    if (vendorId != 0)
    {
        securityToken = QString::number(vendorId);
    }
    else
    {
        securityToken = QString::number(aMessage.SecureId().iId);
    }

    TInt ret;
    HBufC8* serviceMetaDataBuf8 = HBufC8::New(aMessage.GetDesLength(0));
    if (!serviceMetaDataBuf8)
        return KErrNoMemory;

    TPtr8 ptrToBuf(serviceMetaDataBuf8->Des());
    TRAP(ret, aMessage.ReadL(0, ptrToBuf));
    if (ret != KErrNone)
        {
        iDb->lastError().setError(DBError::UnknownError);
        aMessage.Write(1, LastErrorCode());
        delete serviceMetaDataBuf8;
        return ret;
        }

    QByteArray byteArray((const char*)ptrToBuf.Ptr(), ptrToBuf.Length());
    QDataStream out(byteArray);
    ServiceMetaDataResults results;
    out >> results;
    
    iDb->registerService(results, securityToken);
 
    aMessage.Write(1, LastErrorCode());

    delete serviceMetaDataBuf8;
    
    return ret;
    }

TInt CDatabaseManagerServerSession::UnregisterServiceL(const RMessage2& aMessage)
    {
    QString securityToken;
    
    TVendorId vendorId = aMessage.VendorId();
    if (vendorId != 0)
    {
        securityToken = QString::number(vendorId);
    }
    else
    {
        securityToken = QString::number(aMessage.SecureId().iId);
    }
    
    TInt ret;
    HBufC* serviceNameBuf = HBufC::New(aMessage.GetDesLength(0));
    if (!serviceNameBuf)
        return KErrNoMemory;

    TPtr ptrToBuf(serviceNameBuf->Des());
    TRAP(ret, aMessage.ReadL(0, ptrToBuf));
    if (ret != KErrNone)
        {
        iDb->lastError().setError(DBError::UnknownError);
        aMessage.Write(1, LastErrorCode());
        delete serviceNameBuf;
        return ret;
        }

    QString serviceName = QString::fromUtf16(ptrToBuf.Ptr(), ptrToBuf.Length());
    iDb->unregisterService(serviceName, securityToken);
    
    aMessage.Write(1, LastErrorCode());
    delete serviceNameBuf;
    
    return ret;
    }

TInt CDatabaseManagerServerSession::InterfacesSizeL(const RMessage2& aMessage)
    {
    TInt ret;
    HBufC8* buf = HBufC8::New(aMessage.GetDesLength(0));
    if (!buf)
        return KErrNoMemory;
    
    TPtr8 ptrToBuf(buf->Des());
    TRAP(ret, aMessage.ReadL(0, ptrToBuf));
    if (ret != KErrNone)
        {
        iDb->lastError().setError(DBError::UnknownError);
        aMessage.Write(2, LastErrorCode());
        delete buf;
        return ret;
        }
    
    QByteArray byteArray((const char*)ptrToBuf.Ptr(), ptrToBuf.Length());
    QDataStream out(byteArray);
    QServiceFilter filter;
    out >> filter;
    
    QList<QServiceInterfaceDescriptor> interfaces = iDb->getInterfaces(filter);    
    iByteArray = new QByteArray();
    
    QDataStream in(iByteArray, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_4_6);
    in << interfaces;

    TPckgBuf<TInt> size(iByteArray->size());
    aMessage.Write(1, size);
    aMessage.Write(2, LastErrorCode());
    
    delete buf;
    return ret;
    }

TInt CDatabaseManagerServerSession::InterfacesL(const RMessage2& aMessage)
    {
    TPtrC8 interfacesPtr8((TUint8*)(iByteArray->constData()), iByteArray->size());
    aMessage.Write(0, interfacesPtr8);
    delete iByteArray;
    iByteArray = NULL;
    
    return 0;
    }

TInt CDatabaseManagerServerSession::ServiceNamesSizeL(const RMessage2& aMessage)
    {
    TInt ret;
    HBufC* serviceNamesBuf = HBufC::New(aMessage.GetDesLength(0));
    if (!serviceNamesBuf)
        return KErrNoMemory;

    TPtr ptrToBuf (serviceNamesBuf->Des());
    TRAP(ret, aMessage.ReadL(0, ptrToBuf));
    if (ret != KErrNone)
        {
        iDb->lastError().setError(DBError::UnknownError);
        aMessage.Write(2, LastErrorCode());
        delete serviceNamesBuf;
        return ret;
        }

    QString interfaceName = QString::fromUtf16(ptrToBuf.Ptr(), ptrToBuf.Length());
    QStringList serviceNames = iDb->getServiceNames(interfaceName);
    iByteArray = new QByteArray();

    QDataStream in(iByteArray, QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_4_6);
    in << serviceNames;

    TPckgBuf<TInt> size(iByteArray->size());
    aMessage.Write(1, size);
    aMessage.Write(2, LastErrorCode());
    delete serviceNamesBuf;
    
    return ret;
    }

TInt CDatabaseManagerServerSession::ServiceNamesL(const RMessage2& aMessage)
    {
    TPtrC8 ptr8((TUint8*)(iByteArray->constData()), iByteArray->size());
    aMessage.Write(0, ptr8);
    delete iByteArray;
    iByteArray = NULL;
    
    return 0;
    }

TInt CDatabaseManagerServerSession::SetInterfaceDefaultL(const RMessage2& aMessage)
    {
    TInt ret;
    HBufC* serviceNameBuf = HBufC::New(aMessage.GetDesLength(0));
    HBufC* interfaceNameBuf = HBufC::New(aMessage.GetDesLength(1));
    if (!serviceNameBuf || !interfaceNameBuf)
        return KErrNoMemory;

    TPtr ptrToBuf(serviceNameBuf->Des());
    TPtr ptrToBuf2(interfaceNameBuf->Des());
    
    TRAP(ret, aMessage.ReadL(0, ptrToBuf));
    TRAPD(ret2, aMessage.ReadL(1, ptrToBuf2));
    if (ret != KErrNone || ret2 != KErrNone)
        {
        iDb->lastError().setError(DBError::UnknownError);
        aMessage.Write(2, LastErrorCode());
        delete serviceNameBuf;
        delete interfaceNameBuf;
        return (ret == KErrNone) ? ret2 : ret;
        }

    QString serviceName = QString::fromUtf16(ptrToBuf.Ptr(), ptrToBuf.Length());
    QString interfaceName = QString::fromUtf16(ptrToBuf2.Ptr(), ptrToBuf2.Length());
    
    QList<QServiceInterfaceDescriptor> descriptors;
    QServiceFilter filter;
    filter.setServiceName(serviceName);
    filter.setInterface(interfaceName);
    // Nothing should be returned, because we are checking on nonexistent service
    descriptors = iDb->getInterfaces(filter);

    //find the descriptor with the latest version
    int latestIndex = 0;
        for (int i = 1; i < descriptors.count(); ++i) {
            if (lessThan(descriptors[latestIndex], descriptors[i]))
                latestIndex = i;
    }

    if (!descriptors.isEmpty()) {
        iDb->setInterfaceDefault(descriptors[latestIndex]);     
    }
    else {
        aMessage.Write(2, TError(DBError::NotFound));
        delete serviceNameBuf;
        delete interfaceNameBuf;
        return KErrNotFound;
    }

    aMessage.Write(2, LastErrorCode());
    delete serviceNameBuf;
    delete interfaceNameBuf;
    
    return ret;
    }

TInt CDatabaseManagerServerSession::SetInterfaceDefault2L(const RMessage2& aMessage)
    {
    TInt ret;
    HBufC8* interfaceBuf = HBufC8::New(aMessage.GetDesLength(0));
    if (!interfaceBuf)
        return KErrNoMemory;

    TPtr8 ptrToBuf(interfaceBuf->Des());
    TRAP(ret, aMessage.ReadL(0, ptrToBuf));
    if (ret != KErrNone)
        {
        iDb->lastError().setError(DBError::UnknownError);
        aMessage.Write(1, LastErrorCode());
        delete interfaceBuf;
        return ret;
        }

    QByteArray interfaceDescriptorByteArray((const char*)ptrToBuf.Ptr(), ptrToBuf.Length());
    QDataStream out(interfaceDescriptorByteArray);
    QServiceInterfaceDescriptor interfaceDescriptor;
    out >> interfaceDescriptor;
    
    iDb->setInterfaceDefault(interfaceDescriptor);
    aMessage.Write(1, LastErrorCode());
    delete interfaceBuf;
    
    return ret;
    }

void CDatabaseManagerServerSession::SetChangeNotificationsEnabled(const RMessage2& aMessage)
    {
    if (!m_watcher) 
        {
        m_watcher = new QFileSystemWatcher(QStringList() << iDb->databasePath());
        QObject::connect(m_watcher, SIGNAL(fileChanged(QString)),
                iDatabaseManagerSignalHandler, SLOT(databaseChanged(QString)));
        }

    
    if (aMessage.Int0() == 1) // 1 == Notifications enabled 
        {
        m_knownServices.clear();
        m_knownServices = iDb->getServiceNames(QString());
        } 
    else 
        {
        m_watcher->removePath(iDb->databasePath());
        m_knownServices.clear();
        }
    
    aMessage.Write(1, LastErrorCode());
    }

void CDatabaseManagerServerSession::databaseChanged(const QString &path)
    {
    QStringList currentServices = iDb->getServiceNames(QString());

    if (currentServices == m_knownServices)
        return;

    QStringList newServices;
    for (int i=0; i<currentServices.count(); i++) {
        if (!m_knownServices.contains(currentServices[i]))
            newServices << currentServices[i];
    }

    QStringList removedServices;
    for (int i=0; i<m_knownServices.count(); i++) {
        if (!currentServices.contains(m_knownServices[i]))
            removedServices << m_knownServices[i];
    }

    m_knownServices = currentServices;
    for (int i=0; i<newServices.count(); i++)
        ServiceAdded(newServices[i]);
    for (int i=0; i<removedServices.count(); i++)
        ServiceRemoved(removedServices[i]);
    }

TError CDatabaseManagerServerSession::LastErrorCode()
    {
    return TError(iDb->lastError().code());
    }

void CDatabaseManagerServerSession::ServiceAdded(const QString& aServiceName)
    {           
        if (iWaitingAsyncRequest)
            {
            TPckgBuf<TInt> state(0);
            TPtrC str(reinterpret_cast<const TUint16*>(aServiceName.utf16()));
            iMsg.Write(0, str);
            iMsg.Write(1, state);
            iMsg.Write(2, LastErrorCode());
            iMsg.Complete(ENotifySignalComplete); 
            iWaitingAsyncRequest = EFalse;
            }    
    }

void CDatabaseManagerServerSession::ServiceRemoved(const QString& aServiceName)
    {
    if (iWaitingAsyncRequest)
        {
        TPckgBuf<TInt> state(1);
        TPtrC str(reinterpret_cast<const TUint16*>(aServiceName.utf16()));
        iMsg.Write(0, str);
        iMsg.Write(1, state);
        iMsg.Write(2, LastErrorCode());
        iMsg.Complete(ENotifySignalComplete);
        iWaitingAsyncRequest = EFalse;
        }
    }

void CDatabaseManagerServerSession::initDbPath()
    {
    QSettings::Scope settingsScope = QSettings::SystemScope;;
    QString dbIdentifier = "_system";
    ServiceDatabase *db = iDb;
    QSettings settings(QSettings::IniFormat, settingsScope,
            QLatin1String("Nokia"), QLatin1String("QtServiceFramework"));
    QFileInfo fi(settings.fileName());
#ifdef __WINS__
    // In emulator use commmon place for service database
    // instead of server's private directory (server is in thread, so each
    // application gets its own private directory)
    QDir dir;
    QString serviceDbPath = QDir::toNativeSeparators("C:/Data/temp/QtServiceFW");
    dir.mkpath(serviceDbPath);
    if (!dir.cd(serviceDbPath)) {
        qWarning() << "Fatal error: could not create needed path for serviceDatabase: " << serviceDbPath;
        User::Panic(_L("PLAT (emulator)"), 0);
    }
#else
    QDir dir = fi.dir();
#endif
    QString qtVersion(qVersion());
    qtVersion = qtVersion.left(qtVersion.size() -2); //strip off patch version
    QString dbName = QString("QtServiceFramework_") + qtVersion + dbIdentifier + QLatin1String(".db");
    db->setDatabasePath(dir.path() + QDir::separator() + dbName);
    
    openDb();
    }

bool CDatabaseManagerServerSession::openDb()
    {
    if (iDb) {
        if (iDb->isOpen()) {
            return true;
        } else {
            return iDb->open();
        }
    }
    
    return false;
    }

QTM_END_NAMESPACE

// End of File
