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
    iConnLessStarted(EFalse),
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
    // Deleting connection
    if ( iConnection )
        {
        iConnection->TransferCancel();
        iConnection->ReceiveCancel();
        delete iConnection;
        iConnection = NULL;
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
    
    if (KErrNone == CreateConnection(aPortNum))
        {
        error = iConnection->Transfer(*this, aData);
        if (KErrNone == error)
            {
            val =  0;
            }
        }
    return val; 
    }


TInt CLlcpSocketType1::ReadDatagram(TPtr8 &aTPtr)
    {
    TInt val = -1;
    if (NULL == iConnection)
        {
        iConnection->ReceiveDataFromBuf(aTPtr);
        
        // Start receiving data again      
        TInt error = KErrNone;
        error = iConnection->Receive(*this, aTPtr.Length());
        if (KErrNone == error)
            {
            val =  0;
            }        
        }

    return val;    
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
    CreateConnection(aConnection);
    }

/*!
    Call back from MLlcpReadWriteCb
*/
void CLlcpSocketType1::ReceiveComplete(TInt aError)
    {  
    TRAP_IGNORE(
        if (KErrNone == aError)
            {
            QT_TRYCATCH_LEAVING(iCallback.invokeReadyRead());
            }
        else
            {
            QT_TRYCATCH_LEAVING(iCallback.invokeError());
            }
    );
    }

/*!
    Call back from MLlcpReadWriteCb
*/
void CLlcpSocketType1::WriteComplete(TInt aError, TInt aSize)
    {
    if (iWaitStatus == EWaitForBytesWritten)
        {
        StopWaitNow(EWaitForBytesWritten);
        }
    
    TRAP_IGNORE(
        if (KErrNone == aError)
            {
            QT_TRYCATCH_LEAVING(iCallback.invokeBytesWritten(aSize));
            }
        else
            {
            QT_TRYCATCH_LEAVING(iCallback.invokeError());
            }
    );
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
         }
     
     return error;  
    }


TBool CLlcpSocketType1::WaitForBytesWritten(TInt aMilliSeconds)
    {
    return WaitForOperationReady(EWaitForBytesWritten, aMilliSeconds);
    }

TBool CLlcpSocketType1::WaitForOperationReady(TWaitStatus aWaitStatus,TInt aMilliSeconds)
    {
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
    return ret;
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
    if ( iConnection )
        {
        delete iConnection;
        iConnection = NULL;
        }
    
    delete iSenderAO;
    delete iReceiverAO;
    }
   
/*!
    Send data from local peer to remote peer via connectionLess transport
*/
TInt COwnLlcpConnLess::Transfer(MLlcpReadWriteCb& aLlcpSendCb, const TDesC8& aData )
    {  
    TInt error = KErrNone;
    // Pass message on to transmit AO
    if (!iSenderAO->IsActive())
        {
        error = iSenderAO->Transfer(aLlcpSendCb, aData);
        }
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
    Receive data from remote peer to local peer via connectionLess transport
*/
TInt COwnLlcpConnLess::Receive(MLlcpReadWriteCb& aLlcpReceiveCb, TInt64 aMaxSize)
    {
    TInt error = KErrNone;
    // Pass message on to transmit AO
    if (!iReceiverAO->IsActive())
        {
        error = iReceiverAO->Receive(aLlcpReceiveCb, aMaxSize);
        }
    return error;
    
    }

void COwnLlcpConnLess::ReceiveDataFromBuf(TPtr8 &aTPtr) 
    {   
    iReceiverAO->ReceiveDataFromBuf(aTPtr);
    }

bool COwnLlcpConnLess::HasPendingDatagrams() const
    {
    return iReceiverAO->HasPendingDatagrams();
    }
   
TInt64 COwnLlcpConnLess::PendingDatagramSize() const
    {
    return iReceiverAO->PendingDatagramSize();
    }



///////////////////////////////////////////////////////////////

// Sender AO implementation (class CLlcpSenderType1)
CLlcpSenderType1::CLlcpSenderType1(MLlcpConnLessTransporter* aConnection) 
                       : CActive(CActive::EPriorityStandard), iConnection(aConnection)
    {
    }

CLlcpSenderType1* CLlcpSenderType1::NewL(MLlcpConnLessTransporter* iConnection)
    {
    CLlcpSenderType1* self = new(ELeave) CLlcpSenderType1(iConnection);
    CActiveScheduler::Add(self);
    return self;
    }

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
        
        // Having issued the request for incoming data, ensure you mark this active object 
        // as having an outstanding request.
        SetActive();
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
    MLlcpReadWriteCb* cb = iSendObserver;
    if ( error == KErrNone )
       {
       iSendObserver = NULL;           
       }  
    //emit bytesWritten signals
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


///////////////////////////////////////////////////////////////

// Receiver AO implementation (class CLlcpSenderType1)
// Sender AO implementation (class CLlcpSenderType1)
CLlcpReceiverType1::CLlcpReceiverType1(MLlcpConnLessTransporter* aConnection) 
                       : CActive(CActive::EPriorityStandard), iConnection(aConnection)
    {
    }


CLlcpReceiverType1* CLlcpReceiverType1::NewL(MLlcpConnLessTransporter* iConnection)
    {
    CLlcpReceiverType1* self = new(ELeave) CLlcpReceiverType1(iConnection);
    CActiveScheduler::Add(self);
    return self;
    }

TInt CLlcpReceiverType1::Receive(MLlcpReadWriteCb& cb,TInt64 aMaxSize)
    {
    TInt error = KErrNone;
    if (!IsActive())
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
              iReceiveObserver = &cb;
              }
          }
     else
          {
          error = KErrInUse;
          }
        }
    return error;
    }
    
void CLlcpReceiverType1::RunL(void)
    {
    TInt error = iStatus.Int();
    MLlcpReadWriteCb* cb = iReceiveObserver;
    if ( error == KErrNone )
       {
        iReceiveObserver = NULL;           
       }  
    //emit bytesWritten signals
    if (cb)
       {
       cb->ReceiveComplete(error);
       }            
    }

CLlcpReceiverType1::~CLlcpReceiverType1()
    {
    Cancel();   // cancel ANY outstanding request at time of destruction
    iReceiveBuf.Close();
    }



void CLlcpReceiverType1::ReceiveDataFromBuf(TPtr8 &aTPtr) 
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
        }
    else
        {
        TInt i = 0;
        for (; i < readLength; i++)
            {
            aTPtr[i] = iReceiveBuf[i];
            }   
        //iReceiveBuf.Delete(0,readLength);
        }
    //Empty the buffer.
    iReceiveBuf.Zero(); 
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
