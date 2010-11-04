
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

#include "llcpsockettype1_symbian.h"

// TODO
// will obslete with API updated
const TInt KInterestingSsap = 35;

/*!
    CLlcpSocketType1::NewL()
*/
CLlcpSocketType1* CLlcpSocketType1::NewL(QtMobility::QLlcpSocketPrivate& aCallback)
    {
    CLlcpSocketType1* self = CLlcpSocketType1::NewLC(aCallback);
    CleanupStack::Pop( self );
    return self;
    }

/*!
    CLlcpSocketType1::NewLC()
*/
CLlcpSocketType1* CLlcpSocketType1::NewLC(QtMobility::QLlcpSocketPrivate& aCallback)
    {
    CLlcpSocketType1* self = new (ELeave) CLlcpSocketType1(aCallback);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/*!
    CLlcpSocketType1::CLlcpSocketType1()
*/
CLlcpSocketType1::CLlcpSocketType1(QtMobility::QLlcpSocketPrivate& aCallback)
    : iLlcp( NULL ),
    iLocalConnection(NULL),
    iRemoteConnection(NULL),
    iConnLessStarted(EFalse),
    iCallback(aCallback)
    {
    }

/*!
    CLlcpSocketPrivate::ContructL()
*/
void CLlcpSocketType1::ConstructL()
    {  
    iNfcServer.Open();
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
    iNfcServer.Close();
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
    bool bindOK = ETrue;
    TInt error = KErrNone;
    if ( !iConnLessStarted )
        {
        // remote connection created at frame received
        TRAP( error, iLlcp->StartListeningConnLessRequestL(*this,portNum ));
        iConnLessStarted = ETrue;
        }   
    else
        {
        bindOK = EFalse;
        }
    
    if (bindOK)
        {
        error == KErrNone ? bindOK =  ETrue :  bindOK =  EFalse;
        }
    return bindOK;
    }
 
/*!
    Sends the datagram at aData  to the service that this socket is connected to.
    Returns the number of bytes sent on success; otherwise return -1;
*/
TInt CLlcpSocketType1::StartWriteDatagram(const TDesC8& aData,TInt8 aPortNum)
    {
    TInt val = -1;
    TInt error = KErrNone;
    
    if (KErrNone == CreateLocalConnection(aPortNum))
        {
        error = iLocalConnection->Transfer(*this, aData);
        if (KErrNone == error)
            {
            val =  0;
            }
        }
    return val; 
    }


TInt CLlcpSocketType1::StartReadDatagram(TInt64 aMaxSize)
    {
    TInt val = -1;
    if (NULL == iRemoteConnection)
        {
        TInt error = KErrNone;
        
        // Start receiving data again
        error = iRemoteConnection->Receive(*this, aMaxSize);
        if (KErrNone == error)
            {
            val =  0;
            }        
        }

    return val;    
    }

TInt64 CLlcpSocketType1::ReceiveData(TPtr8 &aTPtr)
    {
      if (iRemoteConnection != NULL)
          {
          iRemoteConnection->ReceiveDataFromBuf(aTPtr);
          return aTPtr.Length();
          }
      return -1;
    }

bool CLlcpSocketType1::HasPendingDatagrams() const
    {
    bool val = false;
    if (NULL != iRemoteConnection)
        {
        val = iRemoteConnection->HasPendingDatagrams();
        } 
    return val;
    }

TInt64 CLlcpSocketType1::PendingDatagramSize() const
    {
    TInt64 val = -1;
    if (NULL != iRemoteConnection)
        {
        val = iRemoteConnection->PendingDatagramSize();
        } 
    return val;
    }

/*!
    Call back from MLlcpConnLessListener
*/
void CLlcpSocketType1::FrameReceived( MLlcpConnLessTransporter* aConnection )
    {
    CreateRemoteConnection(aConnection);
    iCallback.invokeReadyRead();
    }

/*!
    Call back from MLlcpReadWriteCb
*/
void CLlcpSocketType1::ReceiveComplete()
    {
    iCallback.invokeReadyRead();
    }

/*!
    Call back from MLlcpReadWriteCb
*/
void CLlcpSocketType1::WriteComplete( TInt aSize)
    {
    iCallback.invokeBytesWritten(aSize);
    }
/*!
    Creating MLlcpConnLessTransporter object if  connection type connectionless,
    Creating Creating wrapper for local peer connection.
*/
TInt CLlcpSocketType1::CreateLocalConnection(TInt8 portNum)
    {
    TInt error = KErrNone;
    MLlcpConnLessTransporter* connType1 = NULL;
    
    if ( iLocalConnection )
        return error;
        
   TRAP( error, connType1 = iLlcp->CreateConnLessTransporterL( portNum ) );
    
    if ( error == KErrNone )
        {
        TRAP( error, iLocalConnection = 
                    COwnLlcpConnLess::NewL(connType1 )) ;              
        
        if ( error != KErrNone )
            {
            delete iLocalConnection;
            iLocalConnection = NULL;
            }
        }
    return error;        
    }

void CLlcpSocketType1::CreateRemoteConnection(MLlcpConnLessTransporter* aConnection)
    {
    if (NULL == aConnection)
        return;
    
    TInt error = KErrNone;
     
     // Only accepting one incoming remote connection
     if ( !iRemoteConnection )
         { 
         // Creating wrapper for connection. 
         TRAP( error, iRemoteConnection = COwnLlcpConnLess::NewL(aConnection ) );
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
    Construct a new wrapper for connectionLess transport.
*/
COwnLlcpConnLess* COwnLlcpConnLess::NewL( MLlcpConnLessTransporter* aConnection )
    {
    COwnLlcpConnLess* self = COwnLlcpConnLess::NewLC(aConnection );
    CleanupStack::Pop( self );
    return self;
    }
   
/*!
    Construct a new wrapper for connectionLess transport.
*/
COwnLlcpConnLess* COwnLlcpConnLess::NewLC(MLlcpConnLessTransporter* aConnection )
    {
    COwnLlcpConnLess* self = new (ELeave) COwnLlcpConnLess(aConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
   
/*!
    Constructor
*/
COwnLlcpConnLess::COwnLlcpConnLess(  MLlcpConnLessTransporter* aConnection )
    : CActive( EPriorityStandard ),
      iConnection( aConnection ),
      iActionState( EIdle ),
      iLlcpReadWriteCb(NULL)
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
TInt COwnLlcpConnLess::Transfer(MLlcpReadWriteCb& aLlcpSendCb, const TDesC8& aData )
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
            iLlcpReadWriteCb = &aLlcpSendCb;
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
    Cancel data receive from local peer to remote peer via connectionLess transport
*/
void COwnLlcpConnLess::ReceiveCancel()
    {
    Cancel();
    }

/*!
    Receive data from remote peer to local peer via connectionLess transport
*/
TInt COwnLlcpConnLess::Receive(MLlcpReadWriteCb& aLlcpReceiveCb, TInt64 aMaxSize)
    {
    TInt error = KErrNone;
    
    if ( iActionState == EIdle )
        {
        TInt length = 0;
        length = iConnection->SupportedDataLength();
        if (length > aMaxSize)
            {
            length = aMaxSize;
            }   
        
        if ( length > 0 )
            {
            iReceiveBuf.Zero();
            error = iReceiveBuf.ReAlloc( length );
            
            if ( error == KErrNone )
                {
                iConnection->Receive( iStatus, iReceiveBuf );
                SetActive();
                iActionState = EReceiving;
                iLlcpReadWriteCb = &aLlcpReceiveCb;
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

void COwnLlcpConnLess::ReceiveDataFromBuf(TPtr8 &aTPtr) 
    {
    if (iReceiveBuf.Size() == 0)
        return;
    
    TInt readLength = aTPtr.Length();
    TInt bufLength =  iReceiveBuf.Length();  
  
    if (readLength >= bufLength)
        {
        // swallow all the data from iReceiveBuf
        for (TInt i=0; i < bufLength; i++)
            {
            aTPtr[i] = iReceiveBuf[i];
            }
        iReceiveBuf.Zero(); 
        }
    else
        {
        TInt i = 0;
        for (; i < readLength; i++)
            {
            aTPtr[i] = iReceiveBuf[i];
            }   
        iReceiveBuf.Delete(0,readLength);
        }

    }

bool COwnLlcpConnLess::HasPendingDatagrams() const
    {
    return iReceiveBuf.Size() > 0 ? true : false;
    }
   
TInt64 COwnLlcpConnLess::PendingDatagramSize() const
    {
    return iReceiveBuf.Size();
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
                MLlcpReadWriteCb* cb = iLlcpReadWriteCb;
                iLlcpReadWriteCb = NULL;
                iActionState = EIdle;
                cb->ReceiveComplete();                
                
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
