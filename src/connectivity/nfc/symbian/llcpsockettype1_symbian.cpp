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

#include <QDebug>
#define LOG1 qDebug() <<__FUNCTION__<<": "<<__LINE__<<":"<<  __FILE__;
#define LOG qDebug()<<"CLlcpSocketType1::"<<__FUNCTION__<<": "<<__LINE__ <<"Begine";
#define LOGEND qDebug()<<"CLlcpSocketType1::"<<__FUNCTION__<<": "<<__LINE__ <<"End";
#define LOG2(a) qDebug()<<"CLlcpSocketType1::"<<__FUNCTION__<<": "<<__LINE__<<a;

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
      iConnection(NULL),
      iWait(NULL),
      iTimer(NULL),
      iWaitStatus(ENone),
      iConnLessStarted(EFalse),
      iCallback(aCallback),
      iRemotePortNum(-1)
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

    // Destroy connection
    Cleanup();
    
    if ( iLlcp )
        {
        delete iLlcp;
        iLlcp = NULL;
        }
    iNfcServer.Close();
    if (iTimer)
        {
        delete iTimer;
        iTimer = NULL;
        }
    if ( iWait )
        {
        delete iWait;
        iWait = NULL;
        }
    }

/*!
    Cancel the Receive/Transfer and destroy the local/remote connection.
*/
void CLlcpSocketType1::Cleanup()
    {    
    // Deleting connection
    if ( iConnection )
        {
        iConnection->TransferCancel();
        iConnection->ReceiveCancel();
        delete iConnection;
        iConnection = NULL;
        }  
    }

bool CLlcpSocketType1::Bind(TInt8 aPortNum)
    {

    bool bindOK = ETrue;
    TInt error = KErrNone;
    if ( !iConnLessStarted )
        {
        // remote connection created at frame received
        TRAP( error, iLlcp->StartListeningConnLessRequestL(*this,aPortNum ));
        if (KErrNone == error)
            {
            iConnLessStarted = ETrue;
            }
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
    
    if (KErrNone == CreateConnection(aPortNum))
        {
        TRAP_IGNORE(
             QT_TRYCATCH_LEAVING( error = iConnection->TransferL(*this, aData));
         );
        
        if (KErrNone == error)
            {
            val =  0;
            }
        }
    return val; 
    }


TInt CLlcpSocketType1::ReadDatagram(TDes8& aData, TInt8& aRemotePortNum)
    {
    aRemotePortNum = iRemotePortNum;
    TInt val = ReadDatagram(aData);
    return val;
    }

TInt CLlcpSocketType1::ReadDatagram(TDes8& aData)
    {
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

    return readSize;
    }

bool CLlcpSocketType1::HasPendingDatagrams() const
    {
    bool val = false;
    if (NULL != iConnection)
        {
        val = iConnection->HasPendingDatagrams();
        } 
    return val;
    }

TInt64 CLlcpSocketType1::PendingDatagramSize() const
    {
    TInt64 val = -1;
    if (NULL != iConnection)
        {
        val = iConnection->PendingDatagramSize();
        } 
    return val;
    }

/*!
    Call back from MLlcpConnLessListener
*/
void CLlcpSocketType1::FrameReceived( MLlcpConnLessTransporter* aConnection )
    {
    iRemotePortNum = aConnection->DsapL();
    CreateConnection(aConnection);
    qDebug() << " CLlcpSocketType1::FrameReceived after: " << iRemotePortNum;
    }

/*!
    Call back from MLlcpReadWriteCb
*/
void CLlcpSocketType1::ReceiveComplete(TInt aError)
    {  

    qDebug() << " CLlcpSocketType1::ReceiveComplete " << aError;
    TRAP_IGNORE(
        if (KErrNone == aError)
            {
            QT_TRYCATCH_LEAVING(iCallback.invokeReadyRead());
            }
        else
            {
            QT_TRYCATCH_LEAVING(iCallback.invokeError());
            }
    ); // End of TRAP_IGNORE
    }

/*!
    Call back from MLlcpReadWriteCb
*/
void CLlcpSocketType1::WriteComplete(TInt aError, TInt aSize)
    {
    LOG
    if (iWaitStatus == EWaitForBytesWritten)
        {
        StopWaitNow(EWaitForBytesWritten);
        }
    TRAP_IGNORE(
        if (KErrNone == aError) {
            QT_TRYCATCH_LEAVING(iCallback.invokeBytesWritten(aSize));
            }
        else {
            QT_TRYCATCH_LEAVING(iCallback.invokeError());
            }
    
        if ( iConnection != NULL && iConnection->HasQueuedWrittenDatagram())
             {
             qDebug() << "TransferQueued transfer ";
             QT_TRYCATCH_LEAVING(iConnection->TransferQueuedL(*this));
             }
    );
    LOGEND
    }

void CLlcpSocketType1::StopWaitNow(TWaitStatus aWaitStatus)
    {
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
    }

/*!
    Creating MLlcpConnLessTransporter object if  connection type connectionless,
    Creating Creating wrapper for local peer connection.
*/
TInt CLlcpSocketType1::CreateConnection(TInt8 portNum)
    {
    TInt error = KErrNone;
    MLlcpConnLessTransporter* connType1 = NULL;
    
    if ( iConnection )
        return error;

   TRAP( error, connType1 = iLlcp->CreateConnLessTransporterL( portNum ) );
   
    if ( error == KErrNone )
        {
        error = CreateConnection(connType1);
        }
    return error;        
    }

TInt CLlcpSocketType1::CreateConnection(MLlcpConnLessTransporter* aConnection)
    {
    TInt error = KErrNone;
    
    if (NULL == aConnection)
        return KErrNotFound;

     // Only accepting one incoming remote connection
     if ( !iConnection )
         { 
         // Creating wrapper for connection. 
         TRAP( error, iConnection = COwnLlcpConnLess::NewL(aConnection ) );
         if ( error != KErrNone )
             {
             delete aConnection;
             aConnection = NULL;
             }
         else
             {
             iConnection->Receive(*this);
             }
         }
     
     return error;  
    }


TBool CLlcpSocketType1::WaitForBytesWritten(TInt aMilliSeconds)
    {
    return WaitForOperationReady(EWaitForBytesWritten, aMilliSeconds);
    }

TBool CLlcpSocketType1::WaitForOperationReady(TWaitStatus aWaitStatus,TInt aMilliSeconds)
    {
    qDebug() << "WaitForOperationReady: " << iWait;
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

   qDebug() <<"WaitForOperationReady finished" << iTimer;
    return ret;
    }

  
/*!
    Construct the wrapper for connectionLess transport.
*/
COwnLlcpConnLess* COwnLlcpConnLess::NewL( MLlcpConnLessTransporter* aConnection )
    {
    COwnLlcpConnLess* self = COwnLlcpConnLess::NewLC(aConnection );
    CleanupStack::Pop( self );
    return self;
    }
   
/*!
    Construct the new wrapper for connectionLess transport.
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
    :  iConnection( aConnection )
    {
    }
    
/*!
    ConstructL
*/
void COwnLlcpConnLess::ConstructL()
    {
    // Create the receiver AO
    iSenderAO = CLlcpSenderType1::NewL(iConnection);
    // Create the transmitter AO
    iReceiverAO = CLlcpReceiverType1::NewL(iConnection);
    }
    
/*!
    Destroy the new wrapper for connectionLess transport.
*/
COwnLlcpConnLess::~COwnLlcpConnLess()
    {
    iSendBufArray.ResetAndDestroy();
    
    if ( iConnection )
        {
        delete iConnection;
        iConnection = NULL;
        }
    
    delete iSenderAO;
    delete iReceiverAO;
    }
  

/*!
    Send data from queued buffer
*/
bool COwnLlcpConnLess::TransferQueuedL(MLlcpReadWriteCb& aLlcpSendCb)
    {
    LOG
    bool ret = false;
    if (iSendBufArray.Count() == 0 || iSenderAO->IsActive())
        return ret;
    
    HBufC8* bufRef = iSendBufArray[0];
    //HBufC8* buf = HBufC8::NewLC(bufRef->Length());
    //buf->Des().Copy( *bufRef );
    //iSocket.GetAndLockBuffer().AppendL( buf );
    TPtrC8 ptr(bufRef->Ptr(), bufRef->Length());
    if(iSenderAO->Transfer(aLlcpSendCb, ptr) == KErrNone)
        {
        ret = true;
        }
    iSendBufArray.Remove( 0 );
    
    LOGEND
    return ret;
    }

/*!
    Send data from local peer to remote peer via connectionLess transport
*/
TInt COwnLlcpConnLess::TransferL(MLlcpReadWriteCb& aLlcpSendCb, const TDesC8& aData )
    {  
    LOG
    TInt error = KErrNone;
    // Pass message on to transmit AO
    if (!iSenderAO->IsActive())
        {
        error = iSenderAO->Transfer(aLlcpSendCb, aData);
        }
    else if (aData.Length() > 0)
        {
        qDebug() << "TransferL buffered";
        HBufC8* buf = HBufC8::NewLC(aData.Length());
        buf->Des().Copy(aData );
        error = iSendBufArray.Append(buf);
        CleanupStack::Pop( buf );
        qDebug() << "TransferL buffered finished" << error;
        }
    
    //David debug // do sth
    qDebug() << "David check ok " << aData.Length();
    
    //LOGEND
    return error;
    }

/*!
    Cancel data transfer from local peer to remote peer via connectionLess transport
*/
void COwnLlcpConnLess::TransferCancel()
    {
    if (iSenderAO->IsActive())
        {
        iSenderAO->Cancel();
        }   
    }
   
/*!
    Cancel data receive from local peer to remote peer via connectionLess transport
*/
void COwnLlcpConnLess::ReceiveCancel()
    {
    if (iReceiverAO->IsActive())
        {
        iReceiverAO->Cancel();
        }  
    }

/*!
 *  Trigger the receive AO to start receive datagram
    Receive data from remote peer to local peer via connectionLess transport
*/
TInt COwnLlcpConnLess::Receive(MLlcpReadWriteCb& aLlcpReceiveCb)
    {
    TInt error = KErrNone;
    // Pass message on to transmit AO
    if (!iReceiverAO->IsActive())
        {
        error = iReceiverAO->Receive(aLlcpReceiveCb);
        }
    else
        {
        error = KErrInUse;
        }
    return error;
    }

/*!
    Retrieve data from the buffer of the connection less socket
*/
TInt COwnLlcpConnLess::ReceiveDataFromBuf(TDes8& aData)
    {
    return iReceiverAO->ReceiveDataFromBuf(aData);
    }

bool COwnLlcpConnLess::HasPendingDatagrams() const
    {
    return iReceiverAO->HasPendingDatagrams();
    }

bool COwnLlcpConnLess::HasQueuedWrittenDatagram() const
    {
    bool hasData = iSendBufArray.Count() > 0 ? true : false;
    return hasData;
    }
   
TInt64 COwnLlcpConnLess::PendingDatagramSize() const
    {
    return iReceiverAO->PendingDatagramSize();
    }

/*!
    Start of implementation of Sender AO for connection less mode (type1) - CLlcpSenderType1
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
    Cancel();   // cancel ANY outstanding request at time of destruction
    iTransmitBuf.Close();
    }

TInt CLlcpSenderType1::Transfer(MLlcpReadWriteCb& cb, const TDesC8& aData)
    {
     
    TInt error = KErrNone;
    if (!IsActive())
        {           
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
        }
    else
         {
         error = KErrInUse;
         }    
    return error;
    }

void CLlcpSenderType1::RunL(void)
    {
    TInt error = iStatus.Int();

    qDebug() << "CLlcpSenderType1::RunL: " << iTransmitBuf.Length();
    MLlcpReadWriteCb* cb = iSendObserver;

    // Call back functions of notifying the llcp sender completed.
    if (cb)
       {
       cb->WriteComplete(error,iTransmitBuf.Length());
       }

    }

void CLlcpSenderType1::DoCancel(void)
    {
    // Cancel any outstanding write request on iSocket at this time.
    iConnection->TransmitCancel();
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
CLlcpReceiverType1* CLlcpReceiverType1::NewL(MLlcpConnLessTransporter* iConnection)
    {
    CLlcpReceiverType1* self = new(ELeave) CLlcpReceiverType1(iConnection);
    CActiveScheduler::Add(self);
    return self;
    }


/*!
    Set active for the receiver AO 
    Receive complete callback function "cb" registed
*/
TInt CLlcpReceiverType1::Receive(MLlcpReadWriteCb& cb)
    {
    TInt error = KErrNone;
    if (!IsActive())
        {     
        TInt length = 0;
        length = iConnection->SupportedDataLength();
        iReceiveBuf.Zero();
        error = iReceiveBuf.ReAlloc( length );
          
        if ( error == KErrNone )
            {
            iConnection->Receive( iStatus, iReceiveBuf );
            SetActive();
            // Register the call back function which will complete in RunL
            iReceiveObserver = &cb;          
            }
       }
    return error;
    }
    
void CLlcpReceiverType1::RunL(void)
    {
    TInt error = iStatus.Int();
    qDebug() << "receiverTyp1::RUNL bufSize:" << iReceiveBuf.Length();
    MLlcpReadWriteCb* cb = iReceiveObserver;
    if ( error == KErrNone )
       {
        iReceiveObserver = NULL;           
       }  
     // Call back functions of notifying the llcp receiver completed.
    if (cb)
       {
       cb->ReceiveComplete(error);
       }            
    }

CLlcpReceiverType1::~CLlcpReceiverType1()
    {
    // cancel ANY outstanding request at time of destruction
    Cancel();
    iReceiveBuf.Close();
    }

TInt CLlcpReceiverType1::ReceiveDataFromBuf(TDes8& aData)
    {
    if (iReceiveBuf.Size() == 0)
        return 0;

    TInt requiredLength = aData.MaxLength();
    TInt bufLength =  iReceiveBuf.Length();

    TInt readLength = requiredLength < bufLength ? requiredLength : bufLength;

    TPtrC8 ptr(iReceiveBuf.Ptr(),readLength);
    aData.Append(ptr);

    //Empty the buffer as long as receive data request issued.
    iReceiveBuf.Zero();

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
    // Cancel any outstanding write request on iSocket at this time.
    iConnection->ReceiveCancel();
    }
