
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
#include "llcpsocket_symbian.h"

// TODO
// will obslete with API updated
const TInt KInterestingSsap = 35;

/*!
    CLlcpSocketPrivate::NewL()
*/
CLlcpSocketPrivate* CLlcpSocketPrivate::NewL()
    {
    CLlcpSocketPrivate* self = CLlcpSocketPrivate::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

/*!
    CLlcpSocketPrivate::NewLC()
*/
CLlcpSocketPrivate* CLlcpSocketPrivate::NewLC()
    {
    CLlcpSocketPrivate* self = new (ELeave) CLlcpSocketPrivate();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/*!
    CLlcpSocketPrivate::CLlcpSocketPrivate()
*/
CLlcpSocketPrivate::CLlcpSocketPrivate( )
    : iLlcp( NULL ),
    iRemoteConnection(NULL),
    iConnLessStarted(EFalse),
    iConnOrientedStarted(EFalse),
    {
    }

/*!
    CLlcpSocketPrivate::ContructL()
*/
void CLlcpSocketPrivate::ConstructL()
    {  
    iLlcp = CLlcpProvider::NewL( iNfcServer );

    if ( !iConnLessStarted )
        {
        // TODO
        // will updated to
        // iLlcp->StartListeningConnOrientedRequestL( *this, aServiceName );
        iLlcp->StopListeningConnLessRequest(KInterestingSsap );
        
        iConnLessStarted = EFalse;
        
        CreateLocalConnection(CLlcpSocketPrivate::ConnectionLess);
        }
    }

/*!
    Destroys the LLCP socket.
*/
CLlcpSocketPrivate::~CLlcpSocketPrivate()
    {
    Cleanup();
    
    if ( iLlcp )
        {
        delete iLlcp;
        iLlcp = NULL;
        }
    }


/*!
    Connects to the service identified by the URI \a serviceUri (on \a target).
*/
void CLlcpSocketPrivate::ConnectToService( const TDesC8& aServiceName)
    {
    if ( iConnLessStarted )
        {
        // TODO
        // will updated to
        // iLlcp->StartListeningConnOrientedRequestL( *this, aServiceName );
        iLlcp->StopListeningConnLessRequest(KInterestingSsap );
        
        iConnLessStarted = EFalse;
        }
    
    
    if ( !iConnOrientedStarted )
        {
        // TODO
        // will updated to
        // iLlcp->StartListeningConnOrientedRequestL( *this, aServiceName );
        iLlcp->StartListeningConnOrientedRequestL( *this, KInterestingSsap );  
        iConnOrientedStarted = ETrue;
        
        CreateLocalConnection(CLlcpSocketPrivate::ConnectionOriented);

        }
    }

 
/*!
    Creating MLlcpConnLessTransporter object if  connection type connectionless,
    Creating MLlcpConnOrientedTransporter object if  connection type connectionOriented,
    Creating Creating wrapper for local peer connection.
*/
void CLlcpSocketPrivate::CreateLocalConnection(ConnType aType)
    {
    TInt error = KErrNone;
    MLlcpConnLessTransporter* connType1 = NULL;
    MLlcpConnOrientedTransporter *connType2 = NULL;
    
    if ( iLocalConnection )
        return;
        
    if (CLlcpSocketPrivate::ConnectionLess == aType)
        {
        TRAP( error, connType1 = iLlcp->CreateConnLessTransporterL( KInterestingSsap ) );
        }
    else
        {
        TRAP( error, connType2 = iLlcp->CreateConnOrientedTransporterL( KInterestingSsap ) );
        }
    
    if ( error == KErrNone )
        {
        // Creating wrapper for connection. 
        if (CLlcpSocketPrivate::ConnectionLess == aType)
            {
            TRAP( error, iLocalConnection = 
                    COwnLlcpConnLess::NewL( connType1 )) ;              
            }
        else
            {
            TRAP( error, iLocalConnection = 
                    COwnLlcpConnOriented::NewL( connType2));    
            }
        
        if ( error != KErrNone )
            {
            delete conn;
            }
        }
    return;        
    }

void CLlcpSocketPrivate::CreateRemoteConnection(MLlcpConnLessTransporter* aConnection)
    {
    TInt error = KErrNone;
     
     // Only accepting one incoming remote connection
     if ( !iRemoteConnection ||typeid(iRemoteConnection) != typeid(COwnLlcpConnLess*) )
         {
         if (iRemoteConnection)
             // current remote connection is not ConnOriented, need to switch
             {
             Cleanup();
             }              
         
         // Creating wrapper for connection. 
         TRAP( error, iRemoteConnection = COwnLlcpConnLess::NewL( aConnection ) );
         if ( error == KErrNone )
             {
             // Start receiving data
             iRemoteConnection->Receive();
             }
         else
             {
             delete aConnection;
             }
         }
     else
         {
         delete aConnection;
         }    
    }

/*!
    Creating Creating wrapper for remote peer connection of which type is connection oriented.
*/
void CLlcpSocketPrivate::CreateRemoteConnection(MLlcpConnOrientedTransporter* aConnection)
    {
      TInt error = KErrNone;      
          
      // Only accepting one incoming remote connection
      if ( !iRemoteConnection || typeid(iRemoteConnection) != typeid(COwnLlcpConnOriented*)  )
          {
          aConnection->AcceptConnectRequest();
          
          if (iRemoteConnection)
              // current remote connection is not ConnOriented, need to switch
              {
              Cleanup();
              }
          
          // Creating wrapper for connection. 
          TRAP( error, iRemoteConnection = COwnLlcpConnOriented::NewL( aConnection ) );
          if ( error == KErrNone )
              {
              // Start receiving data
              iRemoteConnection->Receive();
              }
          else
              {
              delete aConnection;
              }
          }
      else
          {
          // Decline connection request and delete connection object
          aConnection->DeclineConnectRequest();
          delete aConnection;
          }
    }

 
/*!
    Sends the datagram at aData  to the service that this socket is connected to.
    Returns the number of bytes sent on success; otherwise return -1;
*/
TInt CLlcpSocketPrivate::WriteDatagram(const TDesC8& aData)
    {
    TInt val = -1;
    TInt error = KErrNone;

    error = iRemoteConnection->Transfer( aData);
    if (KErrNone == error)
        {
        val =  aData.Size();
        }
    return val; 
    }

/*!
    Disconnects the socket.
*/
void CLlcpSocketPrivate::DisconnectFromService()
    { 
    if ( iConnOrientedStarted )
        {
        // Stopping to listen LLCP link
        iLlcp->RemoveLlcpLinkListener();
        
        // Stopping to listen SSAP 35.
        iLlcp->StopListeningConnLessRequest( KInterestingSsap );
        iConnOrientedStarted = EFalse;
        
        Cleanup();
        } 
    }


/*!
    Cancel the Receive/Transfer and destroy the local/remote connection.
*/
void CLlcpSocketPrivate::Cleanup()
    {    
    if ( iRemoteConnection )
        {
        iRemoteConnection->ReceiveCancel();
        iRemoteConnection->TransferCancel()();
        
        delete iRemoteConnection;
        iRemoteConnection = NULL;
        }   
    
    if ( iLocalConnection )
        {
        iLocalConnection->ReceiveCancel();
        iLocalConnection->TransferCancel()();
        
        delete iLocalConnection;
        iLocalConnection = NULL;
        }        
    }
 


/*!
    Call back from MLlcpConnOrientedListener
*/
void CLlcpSocketPrivate::RemoteConnectRequest( MLlcpConnOrientedTransporter* aConnection )
    {  
    CreateRemoteConnection(aConnection);
    }

/*!
    Call back from MLlcpConnLessListener
*/
void CLlcpSocketPrivate::FrameReceived( MLlcpConnLessTransporter* aConnection )
    {
    CreateRemoteConnection(aConnection);
    }

   
/*!
    Construct a new wrapper for connectionLess transport.
*/
COwnLlcpConnLess* COwnLlcpConnLess::NewL( MLlcpConnLessTransporter* aConnection )
    {
    COwnLlcpConnLess* self = COwnLlcpConnLess::NewLC( aConnection );
    CleanupStack::Pop( self );
    return self;
    }
   
/*!
    Construct a new wrapper for connectionLess transport.
*/
COwnLlcpConnLess* COwnLlcpConnLess::NewLC( MLlcpConnLessTransporter* aConnection )
    {
    COwnLlcpConnLess* self = new (ELeave) COwnLlcpConnLess( aConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
   
/*!
    Constructor
*/
COwnLlcpConnLess::COwnLlcpConnLess( MLlcpConnLessTransporter* aConnection )
    : CActive( EPriorityStandard ),
      iConnection( aConnection ),
      iActionState( EIdle )
    {
    }
    
/*!
    ConstructL
*/
void COwnLlcpConnLess::ConstructL()
    {
    CActiveScheduler::Add( this );
    }
    
/*!
    Destroy the new wrapper for connectionLess transport.
*/
COwnLlcpConnLess::~COwnLlcpConnLess()
    {
    Cancel();
    
    if ( iConnection )
        {
        delete iConnection;
        iConnection = NULL;
        }
        
    iTransmitBuf.Close();
    iReceiveBuf.Close();
    }
   
/*!
    Send data from local peer to remote peer via connectionLess transport
*/
TInt COwnLlcpConnLess::Transfer( const TDesC8& aData )
    {
    TInt error = KErrNone;
    
    // Copying data to internal buffer. 
    iTransmitBuf.Zero();
    error = iTransmitBuf.ReAlloc( aData.Length() );
    
    if ( error == KErrNone )
        {
        iTransmitBuf.Append( aData );
        
        if ( iActionState == EIdle )
            {
            // Sending data
            iConnection->Transmit( iStatus, iTransmitBuf );
            SetActive();
            iActionState = ETransmitting;
            }
        else
            {
            // Already sending or receiving data
            error = KErrInUse;
            }
        }
        
    return error;
    }

/*!
    Cancel data transfer from local peer to remote peer via connectionLess transport
*/
void COwnLlcpConnLess::TransferCancel()
    {
    Cancel();
    }
   
/*!
    Receive data from remote peer to local peer via connectionLess transport
*/
TInt COwnLlcpConnLess::Receive( )
    {
    TInt error = KErrNone;
    
    if ( iActionState == EIdle )
        {
        TInt length = 0;
        length = iConnection->SupportedDataLength();
        
        if ( length > 0 )
            {
            iReceiveBuf.Zero();
            error = iReceiveBuf.ReAlloc( length );
            
            if ( error == KErrNone )
                {
                iConnection->Receive( iStatus, iReceiveBuf );
                SetActive();
                iActionState = EReceiving;
                }
            }
        else
            {
            // if length is 0 or negative, LLCP link is destroyed.
            error = KErrNotReady;
            }
        }
    else
        {
        // Connection is already connecting or transfering data, 
        // cannot start receiving.
        error = KErrInUse;
        }
    
    return error;
    }

/*!
    Cancel data receive from local peer to remote peer via connectionLess transport
*/
void COwnLlcpConnLess::ReceiveCancel()
    {
    Cancel();
    }
   

void COwnLlcpConnLess::RunL()
    {
    }

    
void COwnLlcpConnLess::DoCancel()
    {
    TInt error = iStatus.Int();
    
    switch ( iActionState )
        {
        case EReceiving:
            {
            iConnection->ReceiveCancel();
            }
            break;
        
        case ETransmitting:
            {
            iConnection->TransmitCancel();
            }
            break;
        
        default:
            {
            // Do nothing.
            }
            break;
        }
    
    iActionState = EIdle;
    }



COwnLlcpConnOriented* COwnLlcpConnOriented::NewL( MLlcpConnOrientedTransporter* aConnection )
    {
    COwnLlcpConnOriented* self = COwnLlcpConnOriented::NewLC( aConnection );
    CleanupStack::Pop( self );
    return self;
    }
   
// -----------------------------------------------------------------------------
// CMyOwnLlcpApplication::NewLC()
// -----------------------------------------------------------------------------
//
COwnLlcpConnOriented* COwnLlcpConnOriented::NewLC( MLlcpConnOrientedTransporter* aConnection )
    {
    COwnLlcpConnOriented* self = new (ELeave) COwnLlcpConnOriented( aConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
   

COwnLlcpConnOriented::COwnLlcpConnOriented( MLlcpConnOrientedTransporter* aConnection )
    : CActive( EPriorityStandard ),
      iConnection( aConnection ),
      iClientStatus( NULL ),
      iConnState( ENotConnected ),
      iActionState( EIdle )
    {
    }
    
/*!
    ConstructL
*/
void COwnLlcpConnOriented::ConstructL()
    {
    CActiveScheduler::Add( this );
    
    if ( iConnection->IsConnected() )
        {
        iConnState = EConnected;
        }
    }
    

/*!
 * Destructor.
 */
COwnLlcpConnOriented::~COwnLlcpConnOriented()
    {
    Cancel();
    
    if ( iConnection )
        {
        delete iConnection;
        iConnection = NULL;
        }
        
    iTransmitBuf.Close();
    iReceiveBuf.Close();
    }
   
/*!
    Transfer data from local peer to remote peer via connection oriented transport
*/
TInt COwnLlcpConnOriented::Transfer( /*TRequestStatus& aStatus,*/ const TDesC8& aData )
    {
    TInt error = KErrNone;
    TRequestStatus aStatus = KRequestPending;
    TRequestStatus* status = &aStatus; // local
    
    // Copying data to internal buffer. 
    iTransmitBuf.Zero();
    error = iTransmitBuf.ReAlloc( aData.Length() );
    
    if ( error == KErrNone )
        {
        iTransmitBuf.Append( aData );
        
        if ( iConnState == ENotConnected )
            {
            // Starting connecting if is in idle state
            iConnection->Connect( iStatus );
            SetActive();
            iConnState = EConnecting;
            
            iClientStatus = status;
            }
        else if ( iConnState == EConnected && iActionState == EIdle )
            {
            // Sending data
            iConnection->Transmit( iStatus, iTransmitBuf );
            SetActive();
            iActionState = ETransmitting;
            
            iClientStatus = status;
            }
        else
            {
            // iConnState == EConnecting, cannot do nothing
            error = KErrInUse;
            }
        }
        
    if ( error != KErrNone )
        {
        User::RequestComplete( status, error );
        }
    
    return error;
    }

   
/*!
    Cancel the data transfer from local peer to remote peer via connection oriented transport
*/
void COwnLlcpConnOriented::TransferCancel()
    {
    Cancel();
    }
   

/*!
    Receive data from remote peer to local peer via connection oriented transport
*/
TInt COwnLlcpConnOriented::Receive()
    {
    TInt error = KErrNone;
    
    if ( iConnState == EConnected )
        {
        if ( iActionState == EIdle )
            {
            TInt length = 0;
            length = iConnection->SupportedDataLength();
            
            if ( length > 0 )
                {
                iReceiveBuf.Zero();
                error = iReceiveBuf.ReAlloc( length );
                
                if ( error == KErrNone )
                    {
                    iConnection->Receive( iStatus, iReceiveBuf );
                    SetActive();
                    iActionState = EReceiving;
                    }
                }
            else
                {
                // if length is 0 or negative, LLCP link is destroyed.
                error = KErrNotReady;
                }
            }
        else
            {
            // Connection is already connecting or transfering data, 
            // cannot start receiving.
            error = KErrInUse;
            }
        }
    else
        {
        error = KErrNotReady;
        }
    
    return error;
    }

/*!
    Cancel data receive from local peer to remote peer via connection oriented transport
*/
void COwnLlcpConnOriented::ReceiveCancel()
    {
    Cancel();
    }
   

void COwnLlcpConnOriented::RunL()
    {
    TInt error = iStatus.Int();
    
    switch ( iConnState )
        {
        // Handling connecting request
        case EConnecting:
            {
            if ( error == KErrNone )
                {
                // Updating state
                iConnState = EConnected;
                
                // Sending data if there is data to send
                if ( iTransmitBuf.Length() > 0 )
                    {
                    iConnection->Transmit( iStatus, iTransmitBuf );
                    SetActive();
                    iActionState = ETransmitting;
                    }
                }
            else
                {
                if ( iClientStatus )
                    {
                    User::RequestComplete( iClientStatus, error );
                    iClientStatus = NULL;
                    }
                
                iConnState = ENotConnected;
                }
            }
            break;
        // End of handling connecting request
        
        // Handling transfer or receive requests
        case EConnected:
            {
            if ( iActionState == EReceiving && error == KErrNone )
                {
                iActionState = EIdle;
                }
            else if ( iActionState == ETransmitting )
                {
                // Data transfered/received, notifying client
                if ( iClientStatus )
                    {
                    User::RequestComplete( iClientStatus, error );
                    iClientStatus = NULL;
                    }
                
                iActionState = EIdle;
                }
            else
                {
                // Do nothing
                }
                
            }
            break;
        // End of handling transfer or receive requests
        
        default:
            {
            // Do nothing
            }
            break;
        }
    }

    
void COwnLlcpConnOriented::DoCancel()
    {
    TInt error = iStatus.Int();
    
    switch ( iConnState )
        {
        case EConnecting:
            {
            iConnection->ConnectCancel();
            iConnState = ENotConnected;
            }
            break;
        
        case EConnected:
            {
            switch ( iActionState )
                {
                case ETransmitting:
                    {
                    iConnection->TransmitCancel();
                    }
                    break;
                    
                case EReceiving:
                    {
                    iConnection->ReceiveCancel();
                    }
                    break;
                    
                default:
                    {
                    // Do nothing.
                    }
                    break;
                }
            }
            break;
        
        default:
            {
            // Do nothing
            }
            break;
        }
    
    if ( iClientStatus )
        {
        User::RequestComplete( iClientStatus, KErrCancel );
        iClientStatus = NULL;
        }
    }
