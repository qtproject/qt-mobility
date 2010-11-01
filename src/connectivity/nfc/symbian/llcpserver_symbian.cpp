
/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
#include "llcpserver_symbian.h"
#include "llcpsocket_symbian.h"
#include "../qllcpserver_symbian_p.h"
// TODO
// will obslete with API updated
const TInt KInterestingSsap = 35;

/*!
    CLlcpServer::NewL()
*/
CLlcpServer* CLlcpServer::NewL()
    {
    CLlcpServer* self = CLlcpServer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

/*!
    CLlcpServer::NewLC()
*/
CLlcpServer* CLlcpServer::NewLC()
    {
    CLlcpServer* self = new (ELeave) CLlcpServer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/*!
    CLlcpServer::CLlcpServer()
*/
CLlcpServer::CLlcpServer()
     :iLlcp( NULL ),
     iSocketListening(EFalse)
    {
    }

/*!
    CLlcpServer::ContructL()
*/
void CLlcpServer::ConstructL()
    {  
    iLlcp = CLlcpProvider::NewL( iNfcServer );
    }

/*!
    Destroys the LLCP socket.
*/
CLlcpServer::~CLlcpServer()
    { 
    if ( iLlcp )
        {
        delete iLlcp;
        iLlcp = NULL;
        }
    iLlcpSocketArray.Close();  
    iServiceName.Close();
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
    CLlcpSocketType2 *llcpSocket = iLlcpSocketArray[0];
    if (iLlcpSocketArray.Count() > 0)
        {
        iLlcpSocketArray.Remove(0);
        }
       return llcpSocket;
    }

bool CLlcpServer::hasPendingConnections() const
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
bool CLlcpServer::Listen( const TDesC8& aServiceName)
    {
    TInt error = KErrNone; 

    if (error == KErrNone)
        {
        // TODO
        // will updated to
        // iLlcp->StartListeningConnOrientedRequestL( *this, aServiceName );
        iServiceName = aServiceName;
        TRAP(error,iLlcp->StartListeningConnOrientedRequestL( *this, KInterestingSsap ));         
        }

    error == KErrNone ? iSocketListening = ETrue : iSocketListening = EFalse;
    return iSocketListening;
    }


bool CLlcpServer::isListening() const
    {
    return iSocketListening;
    }

/*!
    Call back from MLlcpConnOrientedListener
*/
void CLlcpServer::RemoteConnectRequest( MLlcpConnOrientedTransporter* aConnection )
    {   
    TInt error = KErrNone;
       
    // create remote connection for the iLlcpsocket
    aConnection->AcceptConnectRequest();

    CLlcpSocketType2 *llcpSocket = NULL;
    TRAP(error,llcpSocket = CLlcpSocketType2::NewL());
    // Creating wrapper for connection. 
    if (KErrNone == error)
        {
        TRAP(error,llcpSocket->CreateLocalConnection(iServiceName));
        if (KErrNone != error)
            {
            //TODO emit error
            }
        TRAP(error,llcpSocket->CreateRemoteConnection(aConnection));
        if (KErrNone != error)
            {
            //TODO emit error
            }
        iLlcpSocketArray.Append(llcpSocket);
        }
    else
        {
        //TODO emit errors
        }
    //TODO  The newConnection() signal is then emitted each time a client connects to the server.

    }


