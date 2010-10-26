
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
    CLlcpSocketType1::NewL()
*/
CLlcpSocketType1* CLlcpSocketType1::NewL()
    {
    CLlcpSocketType1* self = CLlcpSocketType1::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

/*!
    CLlcpSocketType1::NewLC()
*/
CLlcpSocketType1* CLlcpSocketType1::NewLC()
    {
    CLlcpSocketType1* self = new (ELeave) CLlcpSocketType1();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/*!
    CLlcpSocketType1::CLlcpSocketType1()
*/
CLlcpSocketType1::CLlcpSocketType1( )
    : iLlcp( NULL ),
    iRemoteConnection(NULL),
    iConnLessStarted(EFalse)
    {
    }

/*!
    CLlcpSocketPrivate::ContructL()
*/
void CLlcpSocketType1::ConstructL()
    {  
    iLlcp = CLlcpProvider::NewL( iNfcServer );
    }

/*!
    Destroys the LLCP socket.
*/
CLlcpSocketType1::~CLlcpSocketType1()
    {
    Cleanup();
    
    if ( iLlcp )
        {
        delete iLlcp;
        iLlcp = NULL;
        }
    }


/*!
    Cancel the Receive/Transfer and destroy the local/remote connection.
*/
void CLlcpSocketType1::Cleanup()
    {    
    // Deleting local connection
    if ( iLocalConnection )
        {
        iLocalConnection->TransferCancel();
        
        delete iLocalConnection;
        iLocalConnection = NULL;
        }
        
    // Deleting remote connection
    if ( iRemoteConnection )
        {
        iRemoteConnection->ReceiveCancel();
        
        delete iRemoteConnection;
        iRemoteConnection = NULL;
        }    
    }


bool CLlcpSocketType1::Bind(TInt8 portNum)
    {
    TInt error = KErrNone;
    if ( !iConnLessStarted )
        {
        // remote connection created at frame received
        TRAP( error, iLlcp->StartListeningConnLessRequestL(*this,portNum ));
        iConnLessStarted = ETrue;
        }   
    if ( error == KErrNone )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
 
/*!
    Creating MLlcpConnLessTransporter object if  connection type connectionless,
    Creating Creating wrapper for local peer connection.
*/
void CLlcpSocketType1::CreateLocalConnection(TInt8 portNum)
    {
    TInt error = KErrNone;
    MLlcpConnLessTransporter* connType1 = NULL;
    
    if ( iLocalConnection )
        return;
        
   TRAP( error, connType1 = iLlcp->CreateConnLessTransporterL( portNum ) );

    
    if ( error == KErrNone )
        {
        TRAP( error, iLocalConnection = 
                    COwnLlcpConnLess::NewL( connType1 )) ;              
        
        if ( error != KErrNone )
            {
            delete iLocalConnection;
            }
        }
    return;        
    }

void CLlcpSocketType1::CreateRemoteConnection(MLlcpConnLessTransporter* aConnection)
    {
    TInt error = KErrNone;
     
     // Only accepting one incoming remote connection
     if ( !iRemoteConnection )
         { 
         // Creating wrapper for connection. 
         TRAP( error, iRemoteConnection = COwnLlcpConnLess::NewL( aConnection ) );
         if ( error != KErrNone )
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
    Sends the datagram at aData  to the service that this socket is connected to.
    Returns the number of bytes sent on success; otherwise return -1;
*/
TInt CLlcpSocketType1::WriteDatagram(const TDesC8& aData,TInt8 portNum)
    {
    TInt val = -1;
    TInt error = KErrNone;
    
    CreateLocalConnection(portNum);
    error = iLocalConnection->Transfer( aData);
    if (KErrNone == error)
        {
        val =  0;
        }
    return val; 
    }


TInt CLlcpSocketType1::ReadDatagram( TDes8& aData )
    {
    TInt val = -1;
    TInt error = KErrNone;
    
    // Start receiving data again
    error = iRemoteConnection->Receive(aData);
    if (KErrNone == error)
        {
        val =  0;
        }
    return val;    
    }
    

/*!
    Call back from MLlcpConnLessListener
*/
void CLlcpSocketType1::FrameReceived( MLlcpConnLessTransporter* aConnection )
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
TInt COwnLlcpConnLess::Receive( TDes8& aData)
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
    TInt error = iStatus.Int();
    
    switch ( iActionState )
        {
        
        case EReceiving:
            {
            //TODO emit readyRead signals
          
            if ( error == KErrNone )
                {
                iActionState = EIdle;
                }
            }
            break;
        
        case ETransmitting:
            {
            //TODO emit bytesWritten signals
            if ( error == KErrNone )
                {
                iActionState = EIdle;
                }
            }
            break;
            
        default:
            {
            // Do nothing.
            }
            break;
        }    
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

TInt COwnLlcpConnOriented::Connect(const TDesC8& aServiceName)
    {
    TInt error = KErrNone;
    TRequestStatus aStatus = KRequestPending;
    TRequestStatus* status = &aStatus; // local   
      
    if ( iConnState == ENotConnected )
        {
        // Starting connecting if is in idle state
        iConnection->Connect( iStatus );
        SetActive();
        iConnState = EConnecting;
        
        iClientStatus = status;
        }    
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



//  ############# CLlcpSocketType2 #######################

/*!
    CLlcpSocketPrivate::ContructL()
*/
void CLlcpSocketType2::ConstructL()
    {  
    iLlcp = CLlcpProvider::NewL( iNfcServer );

    if ( !iConnOrientedStarted )
        {
        CreateLocalConnection();    
        iConnOrientedStarted = EFalse;
        }
    }

/*!
    Creating MLlcpConnOrientedTransporter object if  connection type connectionOriented,
    Creating Creating wrapper for local peer connection.
*/
void CLlcpSocketType2::CreateLocalConnection()
    {
    TInt error = KErrNone;
    MLlcpConnOrientedTransporter *connType2 = NULL;
    
    if ( iLocalConnection )
        return;
        
    TRAP( error, connType2 = iLlcp->CreateConnOrientedTransporterL( KInterestingSsap ) );
    
    if ( error == KErrNone )
        {
            TRAP( error, iLocalConnection = 
                    COwnLlcpConnOriented::NewL( connType2));    
        
        if ( error != KErrNone )
            {
            delete iLocalConnection;
            }
        }
    return;        
    }


/*!
    Creating Creating wrapper for remote peer connection of which type is connection oriented.
*/
void CLlcpSocketType2::CreateRemoteConnection(MLlcpConnOrientedTransporter* aConnection)
    {
      TInt error = KErrNone;      
          
      // Only accepting one incoming remote connection
      if ( !iRemoteConnection  )
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
    Connects to the service identified by the URI \a serviceUri (on \a target).
*/
void CLlcpSocketType2::ConnectToService( const TDesC8& aServiceName)
    {
    iLocalConnection->Connect(aServiceName);
    }

COwnLlcpConnOriented* CLlcpSocketType2::RemoteConnection() const 
    {
    return iRemoteConnection;
    }

void CLlcpSocketType2::CreateRemoteConnectionL(MLlcpConnOrientedTransporter* aConnection)
    {
    if (!iRemoteConnection)
        {
        iRemoteConnection = COwnLlcpConnOriented::NewL( aConnection );
        }
    }

/*!
    Disconnects the socket.
*/
void CLlcpSocketType2::DisconnectFromService()
    { 
    if ( iConnOrientedStarted )
        {

        // Stopping to listen SSAP 35.
        iLlcp->StopListeningConnLessRequest( KInterestingSsap );
        iConnOrientedStarted = EFalse;
        
        Cleanup();
        } 
    }


/*!
    Cancel the Receive/Transfer and destroy the local/remote connection.
*/
void CLlcpSocketType2::Cleanup()
    {    
    if ( iRemoteConnection )
        {
        iRemoteConnection->ReceiveCancel();
        iRemoteConnection->TransferCancel();
        
        delete iRemoteConnection;
        iRemoteConnection = NULL;
        }   
    
    if ( iLocalConnection )
        {
        iLocalConnection->ReceiveCancel();
        iLocalConnection->TransferCancel();
        
        delete iLocalConnection;
        iLocalConnection = NULL;
        }        
    }
