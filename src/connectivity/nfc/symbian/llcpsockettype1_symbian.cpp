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

#include "nearfieldutility_symbian.h"
#include "llcpsockettype1_symbian.h"
#include "debug.h"


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
    : iWaitStatus(ENone),
      iPortBinded(EFalse),
      iCallback(aCallback)
    {
    }

/*!
    CLlcpSocketPrivate::ContructL()
*/
void CLlcpSocketType1::ConstructL()
    {
    User::LeaveIfError(iNfcServer.Open());
    iLlcp = CLlcpProvider::NewL( iNfcServer );
    iWait = new (ELeave) CActiveSchedulerWait;
    }

/*!
    Destroys the LLCP socket.
*/
CLlcpSocketType1::~CLlcpSocketType1()
    {
    BEGIN
    // Destroy connection
    Cleanup();
    
    if ( iLlcp )
        {
        iLlcp->StopListeningConnLessRequest(iLocalPort);
        delete iLlcp;
        }
    iNfcServer.Close();
    delete iTimer;
    delete iWait;

    END
    }

/*!
    Cancel the Receive/Transfer and destroy the local/remote connection.
*/
void CLlcpSocketType1::Cleanup()
    {  
    BEGIN
    // Deleting connection
    if ( iConnection )
        {
        delete iConnection;
        iConnection = NULL;
        } 
    END
    }

/*!
    Start to listen the port as given, set as local port which is used to read datagram
*/
TBool CLlcpSocketType1::Bind(TUint8 aPortNum)
    {
    BEGIN
    TBool bindOK = EFalse;
    if ( !iPortBinded )
        {
        TInt error = KErrNone;
        TRAP( error, iLlcp->StartListeningConnLessRequestL(*this,aPortNum));
        if (KErrNone == error)
            {
            iPortBinded = ETrue;
            iLocalPort = aPortNum;
            bindOK = ETrue;
            }
        }   
    END
    return bindOK;
    }
 
/*!
    Sends the datagram at aData  to the service that this socket is connected to.
    Returns the number of bytes sent on success; otherwise return -1;
*/
TInt CLlcpSocketType1::StartWriteDatagram(const TDesC8& aData,TUint8 aPortNum)
    {
    BEGIN
    TInt val = -1;

    if (iConnection != NULL && iRemotePort != aPortNum)
        {
        return val;
        }
    
    if (KErrNone == CreateConnection(aPortNum))
        {
        TInt error = KErrNone;
        QT_TRYCATCH_ERROR(error , iConnection->TransferL(*this, aData));
        
        if (KErrNone == error)
            {
            iCallback.m_writeDatagramRefCount++;
            val =  0;
            }
        }
    END
    return val; 
    }


TInt CLlcpSocketType1::ReadDatagram(TDes8& aData, TUint8& aRemotePortNum)
    {
    BEGIN
    aRemotePortNum = iRemotePort;
    TInt val = ReadDatagram(aData);
    END
    return val;
    }

TInt CLlcpSocketType1::ReadDatagram(TDes8& aData)
    {
    BEGIN
    TInt readSize = -1;
    if (NULL != iConnection)
        {
        readSize = iConnection->ReceiveDataFromBuf(aData);

        // Start receiving data again
        TInt error = KErrNone;
        error = iConnection->Receive(*this);
        if (KErrNone != error)
            {
            readSize =  -1;
            }
        }
    END
    return readSize;
    }

TBool CLlcpSocketType1::HasPendingDatagrams() const
    {
    BEGIN
    TBool val = EFalse;
    if (NULL != iConnection)
        {
        val = iConnection->HasPendingDatagrams();
        } 
    END
    return val;
    }

TInt64 CLlcpSocketType1::PendingDatagramSize() const
    {
    BEGIN
    TInt64 val = -1;
    if (NULL != iConnection)
        {
        val = iConnection->PendingDatagramSize();
        } 
    END
    return val;
    }

/*!
    Call back from MLlcpConnLessListener
*/
void CLlcpSocketType1::FrameReceived( MLlcpConnLessTransporter* aConnection )
    {
    BEGIN
    iRemotePort = aConnection->SsapL();
    qDebug() << "FrameReceived " << iRemotePort;
    CreateConnection(aConnection);
    END
    }

/*!
    Call back from MLlcpReadWriteCb
*/
void CLlcpSocketType1::ReceiveComplete(TInt aError)
    {  
    BEGIN
    TInt err = KErrNone;
    if (KErrNone == aError)
        {
        QT_TRYCATCH_ERROR(err,iCallback.invokeReadyRead());
        }
    else
        {
        QT_TRYCATCH_ERROR(err,iCallback.invokeError());
        }
    Q_UNUSED(err);
    END
    }

/*!
    Call back from MLlcpReadWriteCb
*/
void CLlcpSocketType1::WriteComplete(TInt aError, TInt aSize)
    {
    BEGIN
    if (iWaitStatus == EWaitForBytesWritten)
        {
        StopWaitNow(EWaitForBytesWritten);
        }

    TInt err = KErrNone;
    if (KErrNone == aError) {
        iCallback.m_writeDatagramRefCount--;
        QT_TRYCATCH_ERROR(err,iCallback.invokeBytesWritten(aSize));
        }
    else {
        QT_TRYCATCH_ERROR(err,iCallback.invokeError());
        }

    if ( err == aError && iConnection != NULL
         && iConnection->HasQueuedWrittenDatagram())
         {
         iConnection->TransferQueued(*this);
         }

    END
    }

void CLlcpSocketType1::StopWaitNow(TWaitStatus aWaitStatus)
    {
    BEGIN
    if ( iWaitStatus == aWaitStatus )
        {
        if (iWait->IsStarted())
            {
            iWait->AsyncStop();
            }
        if (iTimer)//stop the timer
            {
            delete iTimer;
            iTimer = NULL;
            }
        }   
    END
    }

/*!
    Creating MLlcpConnLessTransporter object if  connection type connectionless,
    Creating Creating wrapper for local peer connection.
*/
TInt CLlcpSocketType1::CreateConnection(TUint8 portNum)
    {
    BEGIN
    TInt error = KErrNone;
    MLlcpConnLessTransporter* connType1 = NULL;
    
    if ( iConnection )
        {
        return error;
        }

    TRAP( error, connType1 = iLlcp->CreateConnLessTransporterL( portNum ) );
   
    if ( error == KErrNone)
        {
          iRemotePort = portNum;
          qDebug() << "CreateConnection " << iRemotePort;
          error = CreateConnection(connType1);
        }
    END
    return error;        
    }

TInt CLlcpSocketType1::CreateConnection(MLlcpConnLessTransporter* aConnection)
    {
    BEGIN
    TInt error = KErrNone;
    
    if (NULL == aConnection)
        return KErrNotFound;

     // Only accepting one incoming remote connection
     if ( !iConnection )
         { 
         // Creating wrapper for connection. 
         TRAP( error, iConnection = COwnLlcpConnectionWrapper::NewL(aConnection ) );
         if ( error == KErrNone )
             {
             iConnection->Receive(*this);
             }
         }
     END
     return error;  
    }


TBool CLlcpSocketType1::WaitForBytesWritten(TInt aMilliSeconds)
    {
    BEGIN_END
    return WaitForOperationReady(EWaitForBytesWritten, aMilliSeconds);
    }

TBool CLlcpSocketType1::WaitForOperationReady(TWaitStatus aWaitStatus,TInt aMilliSeconds)
    {
    BEGIN
    TBool ret = EFalse;
    if (iWaitStatus != ENone || iWait->IsStarted())
        {
        return ret;
        }
    iWaitStatus = aWaitStatus;

    if (iTimer)
        {
        delete iTimer;
        iTimer = NULL;
        }
    if (aMilliSeconds > 0)
        {
        TRAPD(err, iTimer = CLlcpTimer::NewL(*iWait));
        if (err != KErrNone)
            {
            return ret;
            }
        iTimer->Start(aMilliSeconds);
        }
    iWait->Start();

    //control is back here when iWait->AsyncStop() is called by the timer or the callback function
    iWaitStatus = ENone;

    if (!iTimer)
        {
        //iTimer == NULL means this CActiveSchedulerWait
        //AsyncStop is fired by the call back of ReadyRead
        ret = ETrue;
        }
    else
        {
        delete iTimer;
        iTimer = NULL;
        }

    END
    return ret;
    }

  
/*!
    Construct the wrapper for connectionLess transport.
*/
COwnLlcpConnectionWrapper* COwnLlcpConnectionWrapper::NewL( MLlcpConnLessTransporter* aConnection )
    {
    COwnLlcpConnectionWrapper* self = COwnLlcpConnectionWrapper::NewLC(aConnection );
    CleanupStack::Pop( self );
    return self;
    }
   
/*!
    Construct the new wrapper for connectionLess transport.
*/
COwnLlcpConnectionWrapper* COwnLlcpConnectionWrapper::NewLC(MLlcpConnLessTransporter* aConnection )
    {
    COwnLlcpConnectionWrapper* self = new (ELeave) COwnLlcpConnectionWrapper(aConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
   
/*!
    Constructor
*/
COwnLlcpConnectionWrapper::COwnLlcpConnectionWrapper(  MLlcpConnLessTransporter* aConnection )
    :  iConnection( aConnection )
    {
    }
    
/*!
    ConstructL
*/
void COwnLlcpConnectionWrapper::ConstructL()
    {
    // Create the transmitter AO
    iSenderAO = CLlcpSenderType1::NewL(iConnection);
     // Create the receiver AO
    iReceiverAO = CLlcpReceiverType1::NewL(iConnection);
    }
    
/*!
    Destroy the new wrapper for connectionLess transport.
*/
COwnLlcpConnectionWrapper::~COwnLlcpConnectionWrapper()
    {
    BEGIN
    iSendBufArray.ResetAndDestroy();
    iSendBufArray.Close();
    
    if ( iConnection )
        {
        delete iConnection;
        iConnection = NULL;
        }
    
    delete iSenderAO;
    delete iReceiverAO;
    END
    }
  
/*!
    Send data from queued buffer
*/
bool COwnLlcpConnectionWrapper::TransferQueued(MLlcpReadWriteCb& aLlcpSendCb)
    {
    BEGIN
    bool ret = false;
    if (iSendBufArray.Count() == 0 || iSenderAO->IsActive())
        return ret;
    
    HBufC8* bufRef = iSendBufArray[0];
    if (NULL == bufRef)
        return ret;

    TPtrC8 ptr(bufRef->Ptr(), bufRef->Length());
    if(iSenderAO->Transfer(aLlcpSendCb, ptr) == KErrNone)
        {
        ret = true;
        }
    iSendBufArray.Remove( 0 );
    delete bufRef;
    bufRef = NULL;
    
    END
    return ret;
    }

/*!
    Send data from local peer to remote peer via connectionLess transport
*/
TInt COwnLlcpConnectionWrapper::TransferL(MLlcpReadWriteCb& aLlcpSendCb, const TDesC8& aData )
    {  
    BEGIN
    TInt error = KErrNone;
    // Pass message into transmiter AO
    if (!iSenderAO->IsActive())
        {
        error = iSenderAO->Transfer(aLlcpSendCb, aData);
        }
    else if (aData.Length() > 0)
        {
        HBufC8* buf = HBufC8::NewLC(aData.Length());
        buf->Des().Copy(aData );
        error = iSendBufArray.Append(buf);
        CleanupStack::Pop( buf );
        }
    END
    return error;
    }


/*!
 *  Trigger the receiver AO to start receive datagram
    Receive data from remote peer to local peer via connectionLess transport
*/
TInt COwnLlcpConnectionWrapper::Receive(MLlcpReadWriteCb& aLlcpReceiveCb)
    {
    BEGIN
    TInt error = KErrInUse;
    // Pass message on to transmit AO
    if (!iReceiverAO->IsActive())
        {
        error = iReceiverAO->Receive(aLlcpReceiveCb);
        }

    END
    return error;
    }

/*!
    Retrieve data from the buffer of the connection less socket
*/
TInt COwnLlcpConnectionWrapper::ReceiveDataFromBuf(TDes8& aData)
    {
    return iReceiverAO->ReceiveDataFromBuf(aData);
    }

bool COwnLlcpConnectionWrapper::HasPendingDatagrams() const
    {
    return iReceiverAO->HasPendingDatagrams();
    }

bool COwnLlcpConnectionWrapper::HasQueuedWrittenDatagram() const
    {
    bool hasData = iSendBufArray.Count() > 0 ? true : false;
    return hasData;
    }
   
TInt64 COwnLlcpConnectionWrapper::PendingDatagramSize() const
    {
    return iReceiverAO->PendingDatagramSize();
    }

/*!
    Start of implementation of Sender AO & Receiver AO for connection less mode (type1)
*/

/*!
    Constructor
*/
CLlcpSenderType1::CLlcpSenderType1(MLlcpConnLessTransporter* aConnection) 
                       : CActive(CActive::EPriorityStandard), iConnection(aConnection)
    {
    }

/*!
    Constructor
*/
CLlcpSenderType1* CLlcpSenderType1::NewL(MLlcpConnLessTransporter* iConnection)
    {
    CLlcpSenderType1* self = new(ELeave) CLlcpSenderType1(iConnection);
    CActiveScheduler::Add(self);
    return self;
    }

/*!
    Destructor
*/
CLlcpSenderType1::~CLlcpSenderType1()
    {
    BEGIN
    Cancel();   // Cancel ANY outstanding request at time of destruction
    iTransmitBuf.Close();
    END
    }

TInt CLlcpSenderType1::Transfer(MLlcpReadWriteCb& cb, const TDesC8& aData)
    {
    BEGIN
    TInt error = KErrNone;

    // Copying data to internal buffer.
    iTransmitBuf.Zero();
    error = iTransmitBuf.ReAlloc( aData.Length() );

    if ( error == KErrNone )
      {
       iTransmitBuf.Append( aData );

       // Sending data
       iConnection->Transmit( iStatus, iTransmitBuf );
       SetActive();
       iSendObserver = &cb;
      }
    else
      {
       error = KErrNoMemory;
      }

    END
    return error;
    }

void CLlcpSenderType1::RunL(void)
    {
    BEGIN
    TInt error = iStatus.Int();
    // Call back functions of notifying the llcp sender completed.
    if (iSendObserver)
       {
       iSendObserver->WriteComplete(error,iTransmitBuf.Length());
       }
    END
    }

void CLlcpSenderType1::DoCancel(void)
    {
    BEGIN
    // Cancel any outstanding write request on iSocket at this time.
    iConnection->TransmitCancel();
    END
    }

/*!
    Start of implementation of Receiver AO for connection less mode (type1) - CLlcpReceiverType1
*/

/*!
    Constructor
*/
CLlcpReceiverType1::CLlcpReceiverType1(MLlcpConnLessTransporter* aConnection) 
                       : CActive(CActive::EPriorityStandard), iConnection(aConnection)
    {
    }

/*!
    Constructor
*/
CLlcpReceiverType1* CLlcpReceiverType1::NewL(MLlcpConnLessTransporter* aConnection)
    {
    CLlcpReceiverType1* self = new(ELeave) CLlcpReceiverType1(aConnection);
    CActiveScheduler::Add(self);
    return self;
    }


/*!
    Set active for the receiver AO 
    Receive complete callback function "cb" registed
*/
TInt CLlcpReceiverType1::Receive(MLlcpReadWriteCb& cb)
    {
    BEGIN
    TInt error = KErrNone;
    TInt length = 0;
    length = iConnection->SupportedDataLength();
    iReceiveBuf.Zero();
    if (length > 0)
        error = iReceiveBuf.ReAlloc( length );

    if ( error == KErrNone )
        {
        iConnection->Receive( iStatus, iReceiveBuf );
        SetActive();
        // Register the call back function which will complete in RunL
        iReceiveObserver = &cb;
        }

    END
    return error;
    }
    
void CLlcpReceiverType1::RunL(void)
    {
    BEGIN
    TInt error = iStatus.Int();
     // Call back functions of notifying the llcp receiver completed.
    if (iReceiveObserver)
       {
       iReceiveObserver->ReceiveComplete(error);
       }          
    END
    }

CLlcpReceiverType1::~CLlcpReceiverType1()
    {
    BEGIN
    // cancel ANY outstanding request at time of destruction
    Cancel();
    iReceiveBuf.Close();
    END
    }

TInt CLlcpReceiverType1::ReceiveDataFromBuf(TDes8& aData)
    {
    BEGIN
    if (iReceiveBuf.Size() == 0)
        return 0;

    TInt requiredLength = aData.Length();
    TInt bufLength =  iReceiveBuf.Length();

    TInt readLength = requiredLength < bufLength ? requiredLength : bufLength;

    TPtrC8 ptr(iReceiveBuf.Ptr(),readLength);
    aData.Append(ptr);

    //Empty the buffer as long as receive data request issued.
    iReceiveBuf.Zero();
    END
    return readLength;
    }

bool CLlcpReceiverType1::HasPendingDatagrams() const
    {
    return iReceiveBuf.Size() > 0 ? true : false;
    }
   
TInt64 CLlcpReceiverType1::PendingDatagramSize() const
    {
    return iReceiveBuf.Size();
    }

void CLlcpReceiverType1::DoCancel(void)
    {
    BEGIN
    // Cancel any outstanding write request on iSocket at this time.
    iConnection->ReceiveCancel();
    END
    }
