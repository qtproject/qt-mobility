
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
    :iLlcpSocket(NULL),
     iLlcp( NULL ),
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
    if ( iLlcpSocket )
        {
        delete iLlcpSocket;
        iLlcpSocket = NULL;
        }    
    }


CLlcpSocketType2* CLlcpServer::nextPendingConnection()
    {
       return iLlcpSocket;
    }

bool CLlcpServer::hasPendingConnections() const
    {
    iLlcpSocket != NULL ? ETrue: EFalse;
    }

/*!
    Listen to the LLCP Socket by the URI \a serviceUri .
*/
void CLlcpServer::Listen( const TDesC8& aServiceName)
    {
    TInt error = KErrNone; 
    TRAP(error,iLlcpSocket = CLlcpSocketType2::NewL());
    if (error == KErrNone)
        {
        // TODO
        // will updated to
        // iLlcp->StartListeningConnOrientedRequestL( *this, aServiceName );
        TRAP(error,iLlcp->StartListeningConnOrientedRequestL( *this, KInterestingSsap ));         
        }

    error == KErrNone ? iSocketListening = ETrue : iSocketListening = EFalse;
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
    if( !iLlcpSocket->RemoteConnection())
        {
        aConnection->AcceptConnectRequest();
         
        // Creating wrapper for connection. 
        iLlcpSocket->CreateRemoteConnection(aConnection);
        }
    
    //TODO  The newConnection() signal is then emitted each time a client connects to the server.

    }


