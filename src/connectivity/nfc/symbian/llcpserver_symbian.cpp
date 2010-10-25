
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
    :iLlcp( NULL )
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
    
    iPendingConnections.Close();
    if ( iLlcp )
        {
        delete iLlcp;
        iLlcp = NULL;
        }
    }

/*!
    Listen to the LLCP Socket by the URI \a serviceUri .
*/
void CLlcpServer::Listen( const TDesC8& aServiceName)
    {
        // TODO
        // will updated to
        // iLlcp->StartListeningConnOrientedRequestL( *this, aServiceName );
        iLlcp->StartListeningConnOrientedRequestL( *this, KInterestingSsap );  
    }


/*!
    Call back from MLlcpConnOrientedListener
*/
void CLlcpServer::RemoteConnectRequest( MLlcpConnOrientedTransporter* aConnection )
    {  
    }



void CLlcpServer::CreateLocalConnection()
    {
    TInt error = KErrNone;
    MLlcpConnLessTransporter* connType1 = NULL;
    /*
    if ( iLocalConnection )
        return;
        
   TRAP( error, connType1 = iLlcp->CreateConnLessTransporterL( KInterestingSsap ) );

    
    if ( error == KErrNone )
        {
        TRAP( error, iLocalConnection = 
                    COwnLlcpConnLess::NewL( connType1 )) ;              
        
        if ( error != KErrNone )
            {
            delete iLocalConnection;
            }
        }
     */
    return;        
    }

