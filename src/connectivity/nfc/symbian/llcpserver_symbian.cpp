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

#include "llcpserver_symbian.h"
#include "llcpsockettype2_symbian.h"
#include "../qllcpserver_symbian_p.h"

// TODO
// will obslete with API updated
const TInt KInterestingSsap = 35;

/*!
    CLlcpServer::NewL()
*/
CLlcpServer* CLlcpServer::NewL(QtMobility::QLlcpServerPrivate& aCallback)
    {
    CLlcpServer* self = CLlcpServer::NewLC(aCallback);
    CleanupStack::Pop( self );
    return self;
    }

/*!
    CLlcpServer::NewLC()
*/
CLlcpServer* CLlcpServer::NewLC(QtMobility::QLlcpServerPrivate& aCallback)
    {
    CLlcpServer* self = new (ELeave) CLlcpServer(aCallback);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/*!
    CLlcpServer::CLlcpServer()
*/
CLlcpServer::CLlcpServer(QtMobility::QLlcpServerPrivate& aCallback)
     :iLlcp( NULL ),
     iSocketListening(EFalse),
     iCallback(aCallback)
    {
    }

/*!
    CLlcpServer::ContructL()
*/
void CLlcpServer::ConstructL()
    {
    User::LeaveIfError(iNfcServer.Open());
    iLlcp = CLlcpProvider::NewL( iNfcServer );
    }

/*!
    Destroys the LLCP socket.
*/
CLlcpServer::~CLlcpServer()
    {
    delete iLlcp;
    iLlcpSocketArray.ResetAndDestroy();
    iLlcpSocketArray.Close();
    iServiceName.Close();
    iNfcServer.Close();
    }

/*!
    Returns the next pending connection as a connected CLlcpSocketType2
    object.

    The socket is created as a child of the server, which means that
    it is automatically deleted when the CLlcpServer object is
    destroyed. It is still a good idea to delete the object
    explicitly when you are done with it, to avoid wasting memory.
*/
CLlcpSocketType2* CLlcpServer::nextPendingConnection()
    {
    // take first element
    CLlcpSocketType2 *llcpSocket = NULL;
    if (iLlcpSocketArray.Count() > 0)
        {
        llcpSocket = iLlcpSocketArray[0];
        iLlcpSocketArray.Remove(0);
        }
    return llcpSocket;
    }

TBool CLlcpServer::hasPendingConnections() const
    {
    return iLlcpSocketArray.Count() > 0 ? ETrue: EFalse;
    }

const TDesC8&  CLlcpServer::serviceUri() const
    {
    return iServiceName;
    }

/*!
    Listen to the LLCP Socket by the URI \a serviceUri .
*/
TBool CLlcpServer::Listen( const TDesC8& aServiceName)
    {
    TInt error = KErrNone;

    // TODO
    // will updated to
    // iLlcp->StartListeningConnOrientedRequestL( *this, aServiceName );
    iServiceName = aServiceName;
    TRAP(error,iLlcp->StartListeningConnOrientedRequestL( *this, KInterestingSsap ));

    error == KErrNone ? iSocketListening = ETrue : iSocketListening = EFalse;
    return iSocketListening;
    }

void CLlcpServer::StopListening( )
    {
    // TODO
    // will updated to
    //TRAP(error,iLlcp->StopListeningConnOrientedRequest(iServiceName));
    iLlcp->StopListeningConnOrientedRequest( KInterestingSsap );

    iSocketListening = EFalse;
    }


TBool CLlcpServer::isListening() const
    {
    return iSocketListening;
    }

/*!
    Call back from MLlcpConnOrientedListener
*/
void CLlcpServer::RemoteConnectRequest( MLlcpConnOrientedTransporter* aConnection )
    {
    if (aConnection == NULL)
        return;

    TInt error = KErrNone;

    // create remote connection for the iLlcpsocket
    aConnection->AcceptConnectRequest();

    CLlcpSocketType2 *llcpSocket = NULL;
    TRAP(error,llcpSocket = CLlcpSocketType2::NewL(aConnection));
    // Creating wrapper for connection.
    if (KErrNone == error)
        {
        iLlcpSocketArray.Append(llcpSocket);
        //The newConnection() signal is then emitted each time a client connects to the server.
        TRAP_IGNORE(
                QT_TRYCATCH_LEAVING(iCallback.invokeNewConnection()));
        }
    else
        {
        //emit errors
        TRAP_IGNORE(
                QT_TRYCATCH_LEAVING(iCallback.invokeError()));
        }

    }
//EOF
