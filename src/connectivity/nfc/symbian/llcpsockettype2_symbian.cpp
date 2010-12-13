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
#include "llcpsockettype2_symbian.h"

#include <QDebug>

#define LOG1 qDebug() <<__FUNCTION__<<": "<<__LINE__<<":"<<  __FILE__;
#define LOG qDebug()<<"CLlcpSocketType2::"<<__FUNCTION__<<": "<<__LINE__ <<"Begine";
#define LOGEND qDebug()<<"CLlcpSocketType2::"<<__FUNCTION__<<": "<<__LINE__ <<"End";
#define LOG2(a) qDebug()<<"CLlcpSocketType2::"<<__FUNCTION__<<": "<<__LINE__<<a;
// will obslete with API updated
const TInt KInterestingSsap = 35;//TODO replace this with URI

/*!
    CLlcpSocketType2::ContructL()
*/
void CLlcpSocketType2::ConstructL()
    {
    LOG
    User::LeaveIfError(iNfcServer.Open());
    iLlcp = CLlcpProvider::NewL( iNfcServer );
    iWait = new (ELeave) CActiveSchedulerWait;
    LOG
    }

/*!
    CLlcpSocketType2::CLlcpSocketType2()
*/
CLlcpSocketType2::CLlcpSocketType2(MLlcpConnOrientedTransporter* aTransporter, QtMobility::QLlcpSocketPrivate* aCallback)
    : iLlcp( NULL ),
      iTransporter(aTransporter),
      iWaitStatus(ENone),
      iCallback(aCallback)
    {
    }

CLlcpSocketType2* CLlcpSocketType2::NewL(QtMobility::QLlcpSocketPrivate* aCallback)
    {
    CLlcpSocketType2* self = new (ELeave) CLlcpSocketType2(NULL,aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CLlcpSocketType2* CLlcpSocketType2::NewL(MLlcpConnOrientedTransporter* aTransporter, QtMobility::QLlcpSocketPrivate* aCallback)
    {
    CLlcpSocketType2* self = new (ELeave) CLlcpSocketType2(aTransporter,aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/*!
    Destroys the LLCP socket.
*/
CLlcpSocketType2::~CLlcpSocketType2()
    {
    Cleanup();
    delete iLlcp;
    iNfcServer.Close();
    delete iWait;
    delete iTimer;
    iReceiveBufArray.ResetAndDestroy();
    }

/*!
    Connects to the service identified by the URI \a serviceUri (on \a target).
*/
TInt CLlcpSocketType2::ConnectToService( const TDesC8& aServiceName)
    {
    TInt err = KErrNone;
    TRAP( err, ConnectToServiceL(aServiceName) );
    return err;
    }

void CLlcpSocketType2::ConnectToServiceL( const TDesC8& aServiceName)
    {
    LOG
    if ( !iConnecter && !iTransporter)
        {
        iTransporter = iLlcp->CreateConnOrientedTransporterL( KInterestingSsap );
        iConnecter = CLlcpConnecterAO::NewL( *iTransporter, *this );
        iConnecter->ConnectL( aServiceName );
        }
    LOG
    }

/*!
    Disconnects the socket.
*/
TInt CLlcpSocketType2::DisconnectFromService()
    {
    TInt err = KErrNone;
    TRAP( err, DisconnectFromServiceL() );
    return err;
    }

void CLlcpSocketType2::DisconnectFromServiceL()
    {
    if (iSender && iSender->IsActive())
        {
        WaitForBytesWritten(-1);
        }
    if (iConnecter)
        {
        iConnecter->DisconnectL();
        }
    Cleanup();
    }

/*!
    Sends the datagram at aData  to the service that this socket is connected to.
    Returns the number of bytes sent on success; otherwise return -1;
*/
TInt CLlcpSocketType2::StartWriteDatagram(const TDesC8& aData)
    {
    LOG
    TInt val = -1;
    if (!iTransporter)
        {
        LOG
        return KErrNotReady;
        }

    if (!iSender)
        {
        TRAPD(err,iSender = CLlcpSenderAO::NewL(*iTransporter, *this));
        if (err != KErrNone)
            return val;
        }
    TInt error = KErrNone;
    //asynchronous transfer
    error = iSender->Send( aData);
    if (KErrNone == error)
        {
        val =  0;
        }
    LOGEND
    return val;
    }

TBool CLlcpSocketType2::ReceiveData(TDes8& aData)
    {
    //fetch data from internal buffer
    //TODO lock the buffer
    LOG
    TBool ret = EFalse;
    HBufC8* buf = NULL;
    TInt extBufferLength = aData.Length();
    TInt extBufferMaxLength = aData.MaxLength();
    while ( iReceiveBufArray.Count() > 0 )
        {
        buf = iReceiveBufArray[ 0 ];
        if (buf->Length() - iBufferOffset <=  extBufferMaxLength - extBufferLength )
            {
            TPtrC8 ptr(buf->Ptr() + iBufferOffset, buf->Length() - iBufferOffset);
            aData.Append( ptr );
            iReceiveBufArray.Remove( 0 );
            extBufferLength += buf->Length() - iBufferOffset;
            delete buf;
            buf = NULL;
            iBufferOffset = 0;
            }
        else
            {
            TPtrC8 ptr(buf->Ptr() + iBufferOffset, extBufferMaxLength - extBufferLength);
            aData.Append( ptr );
            iBufferOffset += extBufferMaxLength - extBufferLength;
            ret = ETrue;
            break;
            }
        ret = ETrue;
        }
    //TODO unlock the buffer
    LOGEND
    return ret;
    }

TInt64 CLlcpSocketType2::BytesAvailable()
    {
    //TODO lock the buffer
    LOG
    TInt64 ret = 0;
    for (TInt i = 0; i < iReceiveBufArray.Count(); ++i)
        {
        HBufC8* buf = iReceiveBufArray[ i ];
        if (!buf)
            {
                continue;
            }
        if ( i == 0)
            {
            ret += buf->Length() - iBufferOffset;
            }
        else
            {
            ret += buf->Length();
            }
        }
    //TODO unlock the buffer
    LOGEND
    return ret;
    }

TBool CLlcpSocketType2::WaitForOperationReady(TWaitStatus aWaitStatus,TInt aMilliSeconds)
    {
    LOG
    TBool ret = EFalse;
    if (iWaitStatus != ENone || iWait->IsStarted())
        {
        LOG
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
    LOGEND
    return ret;
    }

/**
 * Blocks until data is available for reading and the readyRead()
 * signal has been emitted, or until msecs milliseconds have
 * passed. If msecs is -1, this function will not time out.
 * Returns true if data is available for reading; otherwise
 * returns false (if the operation timed out or if an error
 * occurred).
 */
TBool CLlcpSocketType2::WaitForReadyRead(TInt aMilliSeconds)
    {
    return WaitForOperationReady(EWaitForReadyRead, aMilliSeconds);
    }

TBool CLlcpSocketType2::WaitForBytesWritten(TInt aMilliSeconds)
    {
    return WaitForOperationReady(EWaitForBytesWritten, aMilliSeconds);
    }
TBool CLlcpSocketType2::WaitForConnected(TInt aMilliSeconds)
    {
    return WaitForOperationReady(EWaitForConnected, aMilliSeconds);
    }
TBool CLlcpSocketType2::WaitForDisconnected(TInt /*aMilliSeconds*/)
    {
    return ETrue;//disconnect is a sync method
    }

RPointerArray<HBufC8>& CLlcpSocketType2::GetAndLockBuffer()
    {
    //TODO lock the buffer
    return iReceiveBufArray;
    }
void CLlcpSocketType2::UnlockBuffer()
    {
    //TODO unlock buffer
    }
void CLlcpSocketType2::AttachCallbackHandler(QtMobility::QLlcpSocketPrivate* aCallback)
    {
    iCallback = aCallback;
    if (iTransporter && iTransporter->IsConnected())//has connected llcp transporter
        {
        LOG2("A server llcp type2 socket");
        if (!iReceiver)
            {
            TRAPD(err,iReceiver = CLlcpReceiverAO::NewL( *iTransporter, *this ));
            if (err != KErrNone)
                {
                return;
                }
            }
        iReceiver->StartReceiveDatagram();
        }
    }
/*!
    Cancel the Receive/Transfer and destroy the local/remote connection.
*/
void CLlcpSocketType2::Cleanup()
    {
    LOG
    if (iConnecter)
        {
        delete iConnecter;
        iConnecter = NULL;
        }
    if (iSender)
        {
        delete iSender;
        iSender = NULL;
        }
    if (iReceiver)
        {
        delete iReceiver;
        iReceiver = NULL;
        }
    if (iTransporter)
        {
        delete iTransporter;
        iTransporter = NULL;
        }
    LOGEND
    }
void CLlcpSocketType2::Error(QtMobility::QLlcpSocket::Error /*aSocketError*/)
    {
    //emit error
    if ( iCallback )
        {
        TRAP_IGNORE(
                QT_TRYCATCH_LEAVING(iCallback->invokeError());
        );
        }
    }
void CLlcpSocketType2::StateChangedL(QtMobility::QLlcpSocket::State aSocketState)
    {
    LOG
    if (aSocketState == QtMobility::QLlcpSocket::ConnectedState && iWaitStatus == EWaitForConnected)
        {
        StopWaitNow(EWaitForConnected);
        }

    if (aSocketState == QtMobility::QLlcpSocket::ConnectedState)
        {
        if ( iCallback)
            {
            QT_TRYCATCH_LEAVING(iCallback->invokeConnected());
            }
        if (!iReceiver)
            {
            iReceiver = CLlcpReceiverAO::NewL( *iTransporter, *this );
            }
        User::LeaveIfError(iReceiver->StartReceiveDatagram());
        }

    if (aSocketState == QtMobility::QLlcpSocket::ClosingState && iCallback)
        {
        QT_TRYCATCH_LEAVING(iCallback->invokeDisconnected());
        }

    LOGEND
    }

void CLlcpSocketType2::StopWaitNow(TWaitStatus aWaitStatus)
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
void CLlcpSocketType2::ReadyRead()
    {
    LOG
    if (iWaitStatus == EWaitForReadyRead)
        {
        StopWaitNow(EWaitForReadyRead);
        }
    //emit readyRead()
    if ( iCallback )
        {
        TRAP_IGNORE(
              QT_TRYCATCH_LEAVING(iCallback->invokeReadyRead());
        );
        }
    LOGEND
    }
void CLlcpSocketType2::BytesWritten(qint64 aBytes)
    {
    LOG
    if (iWaitStatus == EWaitForBytesWritten)
        {
        StopWaitNow(EWaitForBytesWritten);
        }
    //emit bytesWritten signal;
    if ( iCallback )
        {
        TRAP_IGNORE(
              QT_TRYCATCH_LEAVING(iCallback->invokeBytesWritten(aBytes));
        );
        }
    LOGEND
    }

// connecter implementation
CLlcpConnecterAO* CLlcpConnecterAO::NewL( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket )
    {
    CLlcpConnecterAO* self = new (ELeave) CLlcpConnecterAO( aConnection, aSocket );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CLlcpConnecterAO::CLlcpConnecterAO( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket )
    : CActive( EPriorityStandard ),
      iConnection( aConnection ),
      iSocket( aSocket ),
      iConnState( ENotConnected )
    {
    }
/*!
    ConstructL
*/
void CLlcpConnecterAO::ConstructL()
    {
    CActiveScheduler::Add( this );
    if ( iConnection.IsConnected() )
        {
        iConnState = EConnected;
        // Starting listening disconnect event
        iConnection.WaitForDisconnection( iStatus );
        SetActive();
        }
    }

/*!
 * Destructor.
 */
CLlcpConnecterAO::~CLlcpConnecterAO()
    {
    Cancel();
    }
/*!
 * Connect to remote peer as given service uri.
 */
void CLlcpConnecterAO::ConnectL(const TDesC8& /*aServiceName*/)
    {
    LOG
    if ( iConnState == ENotConnected )
        {
        // Starting connecting if is in idle state
        iConnection.Connect( iStatus );
        SetActive();
        iConnState = EConnecting;
        //emit connecting signal
        iSocket.StateChangedL(QtMobility::QLlcpSocket::ConnectingState);
        }
    LOGEND
    }

/*!
 * Disonnect with remote peer.
 */
void CLlcpConnecterAO::DisconnectL()
    {
    if ( iConnState == ENotConnected )
        {
        return;
        }
    else if ( iConnState == EConnecting )
        {
        Cancel();//cancel Connect
        }
    else if ( iConnState == EConnected )
        {
        Cancel();//cancel WaitForDisconnection
        iConnection.Disconnect();
        }
        iConnState = ENotConnected;

        //emit QAbstractSocket::ClosingState;
        iSocket.StateChangedL(QtMobility::QLlcpSocket::ClosingState);
    }
void CLlcpConnecterAO::RunL()
  {
    LOG
  TInt error = iStatus.Int();

  switch ( iConnState )
    {
    // Handling connecting request
    case EConnecting:
      {
      if ( error == KErrNone )
        {
          LOG2("Connected to LLCP server");
        // Updating state
        iConnState = EConnected;
        //emit connected signal
        iSocket.StateChangedL(QtMobility::QLlcpSocket::ConnectedState);
        // Starting listening disconnect event
        iConnection.WaitForDisconnection( iStatus );
        SetActive();
        }
      else
        {
          LOG2("!!Failed to Connected to LLCP server");
        //KErrNotSupported when remote peer has lost from the near field.
        iConnState = ENotConnected;
        //emit error signal
        iSocket.Error(QtMobility::QLlcpSocket::UnknownSocketError);
        }
      }
      break;
    case EConnected:
      {
      //handling disconnect event
      if ( error == KErrNone )
        {
        // Updating state
        iConnState = ENotConnected;
        //emit disconnected signal
        iSocket.StateChangedL(QtMobility::QLlcpSocket::ClosingState);
        }
      else
        {
        iConnState = ENotConnected;
        //emit error signal
        iSocket.Error(QtMobility::QLlcpSocket::UnknownSocketError);
        }
      }
      break;
    default:
            {
            // Do nothing
            }
      break;
    }
  LOGEND
  }
void CLlcpConnecterAO::DoCancel()
  {
    switch ( iConnState )
        {
        case EConnecting:
            {
            iConnection.ConnectCancel();
            iConnState = ENotConnected;
            }
            break;

        case EConnected:
            {
            iConnection.WaitForDisconnectionCancel();
            iConnState = ENotConnected;
            }
            break;

        default:
            {
            // Do nothing
            }
            break;
        }
  }
//sender AO implementation

CLlcpSenderAO* CLlcpSenderAO::NewL( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket )
    {
    CLlcpSenderAO* self = new (ELeave) CLlcpSenderAO( aConnection, aSocket );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CLlcpSenderAO::CLlcpSenderAO( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket )
    : CActive( EPriorityStandard ),
      iConnection( aConnection ),
      iSocket( aSocket )
    {
    }
/*!
    ConstructL
*/
void CLlcpSenderAO::ConstructL()
    {
    CActiveScheduler::Add( this );
    if ( !iConnection.IsConnected() )
        {
        User::Leave(KErrArgument);
        }
    }

/*!
 * Destructor.
 */
CLlcpSenderAO::~CLlcpSenderAO()
    {
    Cancel();
    iSendBuf0.Close();
    iSendBuf1.Close();
    }
/*!
 * Transfer given data to remote device.
 */
TInt CLlcpSenderAO::Send( const TDesC8& aData )
    {
    LOG
    TInt error = KErrNone;
    if ( !IsActive() )
        {
        // Copying data to internal buffer.
        iSendBuf0.Zero();
        error = iSendBuf0.ReAlloc( aData.Length() );

        if ( error == KErrNone )
          {
          iSendBuf0.Append( aData );

          // Sending data
          iConnection.Transmit( iStatus, iSendBuf0 );
          SetActive();
          iCurrentBuffer = EBuffer0;
          }
        }
    else
      {
        if (iCurrentBuffer == EBuffer0)
            {
            error = iSendBuf1.ReAlloc( iSendBuf1.Length() + aData.Length() );
            if (error == KErrNone)
                {
                iSendBuf1.Append(aData);
                }
            }
        else
            {
            error = iSendBuf0.ReAlloc( iSendBuf0.Length() + aData.Length() );
            if (error == KErrNone)
                {
                iSendBuf0.Append(aData);
                }
            }
      }
    LOGEND
    return error;
    }

void CLlcpSenderAO::RunL()
    {
    LOG
    TInt error = iStatus.Int();
    if ( error == KErrNone )
        {
        if (iCurrentBuffer == EBuffer0)
            {
            iSocket.BytesWritten(iSendBuf0.Length());
            iSendBuf0.Zero();
            if(iSendBuf1.Length() > 0)
                {
                iCurrentBuffer = EBuffer1;
                iConnection.Transmit( iStatus, iSendBuf1 );
                SetActive();
                }
            }
        else //buffer1
            {
            iSocket.BytesWritten(iSendBuf1.Length());
            iSendBuf1.Zero();
            if(iSendBuf0.Length() > 0)
                {
                iCurrentBuffer = EBuffer0;
                iConnection.Transmit( iStatus, iSendBuf0 );
                SetActive();
                }
            }
        }
    else
        {
        //emit error() signal
        iSocket.Error(QtMobility::QLlcpSocket::UnknownSocketError);
        }
    LOGEND
    }
void CLlcpSenderAO::DoCancel()
    {
    if (IsActive())
        {
        iConnection.TransmitCancel();
        }
    }
//receiver implementation
CLlcpReceiverAO* CLlcpReceiverAO::NewL( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket )
    {
    CLlcpReceiverAO* self = new (ELeave) CLlcpReceiverAO( aConnection, aSocket );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CLlcpReceiverAO::CLlcpReceiverAO( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket )
    : CActive( EPriorityStandard ),
      iConnection( aConnection ),
      iSocket( aSocket )
    {
    }
/*!
    ConstructL
*/
void CLlcpReceiverAO::ConstructL()
    {
    CActiveScheduler::Add( this );
    if ( !iConnection.IsConnected() )
        {
        User::Leave(KErrArgument);
        }
    }

/*!
 * Destructor.
 */
CLlcpReceiverAO::~CLlcpReceiverAO()
    {
    Cancel();
    iReceiveBuf.Close();
    }

TInt CLlcpReceiverAO::StartReceiveDatagram()
  {
    LOG
    if (IsActive())
        {
        return KErrInUse;
        }
    TInt length = 0;
    TInt error = KErrNone;
    length = iConnection.SupportedDataLength();

     if ( length > 0 )
         {
         iReceiveBuf.Zero();
         error = iReceiveBuf.ReAlloc( length );

         if ( error == KErrNone )
             {
             iConnection.Receive( iStatus, iReceiveBuf );
             SetActive();
             }
         }
     else
         {
         // if length is 0 or negative, LLCP link is destroyed.
         error = KErrNotReady;
         }
     LOGEND
     return error;
  }

void CLlcpReceiverAO::RunL()
  {
    LOG
  TInt error = iStatus.Int();
  if ( error == KErrNone )
    {
    //append to buffer
    HBufC8* buf = NULL;
    buf = HBufC8::NewLC( iReceiveBuf.Length() );
    buf->Des().Copy( iReceiveBuf );
    iSocket.GetAndLockBuffer().AppendL( buf );
    iSocket.UnlockBuffer();
    CleanupStack::Pop( buf );

    //emit readyRead() signal
    iSocket.ReadyRead();
    //resend the Receive request to NFC server
    if (StartReceiveDatagram() != KErrNone)
      {
      iSocket.Error(QtMobility::QLlcpSocket::UnknownSocketError);
      }
    }
    else if ( error == KErrCancel )
    {
        //just omit the KErrCancel
        qDebug("CLlcpReceiverAO::RunL() iStatus = KErrCancel");
    }
    else
    {
    //emit error() signal
    iSocket.Error(QtMobility::QLlcpSocket::UnknownSocketError);
    }
  LOGEND
  }
void CLlcpReceiverAO::DoCancel()
  {
  if (IsActive())
    {
    iConnection.ReceiveCancel();
    }
  }
//EOF
