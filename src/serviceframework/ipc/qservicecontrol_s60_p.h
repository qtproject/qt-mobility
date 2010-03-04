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

#ifndef QSERVICECONTROL_S60_P_H
#define QSERVICECONTROL_S60_P_H

#define QT_SFW_SYMBIAN_IPC_DEBUG

#include "qservicecontrol.h"
#include "qservicetyperegister.h"
#include <e32base.h>

#ifdef QT_SFW_SYMBIAN_IPC_DEBUG
#include <QDebug>
#endif

QTM_BEGIN_NAMESPACE

// Need to migrate to clientservercommon.h
const TUint KServerMajorVersionNumber = 1;
const TUint KServerMinorVersionNumber = 0;
const TUint KServerBuildVersionNumber = 0;

// Forward declarations
class ObjectEndPoint;
// Type definitions
typedef TPckgBuf<TInt> TError; 

// Internal class handling the actual communication with the service provider.
// Communication is based on standard Symbian client-server architecture.
class RServiceSession : public RSessionBase
{

public: 
    RServiceSession(QString address);
    TInt Connect(); 
    void Close();
    TVersion Version() const;

 public:
    TPckgBuf<TInt> iState(); // TPckgBuf type can be used directly as IPC parameter

private:
    TInt StartServer();

private: 
    TIpcArgs iArgs;
    TError iError;
    QString iServerAddress;
};

// needed for creating server thread.
const TUint KDefaultHeapSize = 0x10000;

class CServiceProviderServer : public CServer2
    {
    public:
        CServiceProviderServer();
        CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

    public:

        void IncreaseSessions();
        void DecreaseSessions();

    private:
        int iSessionCount;
    };

class CServiceProviderServerSession : public CSession2
    {
    public:
        static CServiceProviderServerSession* NewL(CServiceProviderServer& aServer);
        static CServiceProviderServerSession* NewLC(CServiceProviderServer& aServer);
        virtual ~CServiceProviderServerSession();
        void ServiceL(const RMessage2& aMessage);

    private:
        CServiceProviderServerSession(CServiceProviderServer& aServer);
        void ConstructL(); 

    private:
        CServiceProviderServer& iServer;
        QByteArray* iByteArray;
        RMessage2 iMsg;
    };


class QServiceControlPrivate: public QObject
{
    Q_OBJECT

public:
    QServiceControlPrivate(QObject* parent);
    void publishServices(const QString& ident );
    static QObject* proxyForService(const QServiceTypeIdent& typeId, const QString& location);
};

QTM_END_NAMESPACE

#endif // QSERVICECONTROL_S60_P_H
