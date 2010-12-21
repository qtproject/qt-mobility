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

#include "debug.h"

// will obslete with API updated
const TInt KInterestingSsap = 35;//TODO replace this with URI

/*!
    CLlcpSocketType2::ContructL()
*/
void CLlcpSocketType2::ConstructL()
    {
    BEGIN
    User::LeaveIfError(iNfcServer.Open());
    iLlcp = CLlcpProvider::NewL( iNfcServer );
    iWait = new (ELeave) CActiveSchedulerWait;
    END
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
    BEGIN
    CLlcpSocketType2* self = new (ELeave) CLlcpSocketType2(NULL,aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    END
    return self;
    }

CLlcpSocketType2* CLlcpSocketType2::NewL(MLlcpConnOrientedTransporter* aTransporter, QtMobility::QLlcpSocketPrivate* aCallback)
    {
    BEGIN
    CLlcpSocketType2* self = new (ELeave) CLlcpSocketType2(aTransporter,aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    END
    return self;
    }

/*!
    Destroys the LLCP socket.
*/
CLlcpSocketType2::~CLlcpSocketType2()
    {
    BEGIN
    Cleanup();
    delete iLlcp;
    iNfcServer.Close();
    delete iWait;
    delete iTimer;
    iReceiveBufArray.ResetAndDestroy();
    END
    }

/*!
    Connects to the service identified by the URI \a serviceUri (on \a target).
*/
TInt CLlcpSocketType2::ConnectToService( const TDesC8& aServiceName)
    {
    BEGIN
    TInt err = KErrNone;
    TRAP( err, ConnectToServiceL(aServiceName) );
    END
    return err;
    }

void CLlcpSocketType2::ConnectToServiceL( const TDesC8& aServiceName)
    {
    BEGIN
    if ( !iConnecter && !iTransporter)
        {
        iTransporter = iLlcp->CreateConnOrientedTransporterL( KInterestingSsap );
        iConnecter = CLlcpConnecterAO::NewL( *iTransporter, *this );
        iConnecter->ConnectL( aServiceName );
        }
    END
    }

/*!
    Disconnects the socket.
*/
TInt CLlcpSocketType2::DisconnectFromService()
    {
    BEGIN
    TInt err = KErrNone;
    TRAP( err, DisconnectFromServiceL() );
    END
    return err;
    }

void CLlcpSocketType2::DisconnectFromServiceL()
    {
    BEGIN
    if (iSender && iSender->IsActive())
        {
        WaitForBytesWritten(-1);
        }
    if (iConnecter)
        {
        iConnecter->DisconnectL();
        }
    Cleanup();
    END
    }

/*!
    Sends the datagram at aData  to the service that this socket is connected to.
    Returns the number of bytes sent on success; otherwise return -1;
*/
TInt CLlcpSocketType2::StartWriteDatagram(const TDesC8& aData)
    {
    BEGIN
    TInt val = -1;
    if (!iTransporter)
        {
        END
        return KErrNotReady;
        }

    if (!iSender)
        {
        TRAPD(err,iSender = CLlcpSenderAO::NewL(*iTransporter, *this));
        if (err != KErrNone)
            {
            END
            return val;
            }
        }
    TInt error = KErrNone;
    //asynchronous transfer
    error = iSender->Send( aData);
    if (KErrNone == error)
        {
        val =  0;
        }
    END
    return val;
    }

TBool CLlcpSocketType2::ReceiveData(TDes8& aData)
    {
    //fetch data from internal buffer
    BEGIN
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
    END
    return ret;
    }

TInt64 CLlcpSocketType2::BytesAvailable()
    {
    BEGIN
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
    END
    return ret;
    }

TBool CLlcpSocketType2::WaitForOperationReady(TWaitStatus aWaitStatus,TInt aMilliSeconds)
    {
    BEGIN
    TBool ret = EFalse;
    if (iWaitStatus != ENone || iWait->IsStarted())
        {
        END
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
            END
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
    BEGIN
    END
    return WaitForOperationReady(EWaitForReadyRead, aMilliSeconds);
    }

TBool CLlcpSocketType2::WaitForBytesWritten(TInt aMilliSeconds)
    {
    BEGIN
    END
    return WaitForOperationReady(EWaitForBytesWritten, aMilliSeconds);
    }
TBool CLlcpSocketType2::WaitForConnected(TInt aMilliSeconds)
    {
    BEGIN
    END
    return WaitForOperationReady(EWaitForConnected, aMilliSeconds);
    }
TBool CLlcpSocketType2::WaitForDisconnected(TInt /*aMilliSeconds*/)
    {
    BEGIN
    END
    return ETrue;//disconnect is a sync method
    }

void CLlcpSocketType2::AttachCallbackHandler(QtMobility::QLlcpSocketPrivate* aCallback)
    {
    BEGIN
    iCallback = aCallback;
    if (iTransporter && iTransporter->IsConnected())//has connected llcp transporter
        {
        LOG("A server llcp type2 socket");
        if (!iReceiver)
            {
            TRAPD(err,iReceiver = CLlcpReceiverAO::NewL( *iTransporter, *this ));
            if (err != KErrNone)
                {
                END
                return;
                }
            }
        if (iReceiver->StartReceiveDatagram() != KErrNone)
          {
          Error(QtMobility::QLlcpSocket::UnknownSocketError);
          }
        }
    END
    }
/*!
    Cancel the Receive/Transfer and destroy the local/remote connection.
*/
void CLlcpSocketType2::Cleanup()
    {
    BEGIN
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
    END
    }
void CLlcpSocketType2::Error(QtMobility::QLlcpSocket::Error /*aSocketError*/)
    {
    BEGIN
    //emit error
    if ( iCallback )
        {
        TInt error = KErrNone;
        QT_TRYCATCH_ERROR(error,iCallback->invokeError());
        //can do nothing if there is an error,so just ignore it
        Q_UNUSED(error);
        }
    END
    }
void CLlcpSocketType2::StateChangedL(QtMobility::QLlcpSocket::State aSocketState)
    {
    BEGIN
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

    END
    }

void CLlcpSocketType2::StopWaitNow(TWaitStatus aWaitStatus)
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
void CLlcpSocketType2::ReadyRead()
    {
    BEGIN
    if (iWaitStatus == EWaitForReadyRead)
        {
        StopWaitNow(EWaitForReadyRead);
        }
    //emit readyRead()
    if ( iCallback )
        {
        TInt error = KErrNone;
        QT_TRYCATCH_ERROR(error,iCallback->invokeReadyRead());
        //can do nothing if there is an error,so just ignore it
        Q_UNUSED(error);
        }
    END
    }
void CLlcpSocketType2::BytesWritten(qint64 aBytes)
    {
    BEGIN
    if (iWaitStatus == EWaitForBytesWritten)
        {
        StopWaitNow(EWaitForBytesWritten);
        }
    //emit bytesWritten signal;
    if ( iCallback )
        {
        TInt error = KErrNone;
        QT_TRYCATCH_ERROR(error,iCallback->invokeBytesWritten(aBytes));
        //can do nothing if there is an error,so just ignore it
        Q_UNUSED(error);
        }
    END
    }

// connecter implementation
CLlcpConnecterAO* CLlcpConnecterAO::NewL( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket )
    {
    BEGIN
    CLlcpConnecterAO* self = new (ELeave) CLlcpConnecterAO( aConnection, aSocket );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    END
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
    BEGIN
    CActiveScheduler::Add( this );
    if ( iConnection.IsConnected() )
        {
        LOG("a LLCP server side socket");
        iConnState = EConnected;
        // Starting listening disconnect event
        iConnection.WaitForDisconnection( iStatus );
        SetActive();
        }
    END
    }

/*!
 * Destructor.
 */
CLlcpConnecterAO::~CLlcpConnecterAO()
    {
    BEGIN
    Cancel();
    END
    }
/*!
 * Connect to remote peer as given service uri.
 */
void CLlcpConnecterAO::ConnectL(const TDesC8& /*aServiceName*/)
    {
    BEGIN
    if ( iConnState == ENotConnected )
        {
        // Starting connecting if is in idle state
        iConnection.Connect( iStatus );
        SetActive();
        iConnState = EConnecting;
        //emit connecting signal
        iSocket.StateChangedL(QtMobility::QLlcpSocket::ConnectingState);
        }
    END
    }

/*!
 * Disonnect with remote peer.
 */
void CLlcpConnecterAO::DisconnectL()
    {
    BEGIN
    if ( iConnState == ENotConnected )
        {
        END
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
    END
    }
void CLlcpConnecterAO::RunL()
    {
    BEGIN
    TInt error = iStatus.Int();

    switch ( iConnState )
        {
        // Handling connecting request
        case EConnecting:
            {
            if ( error == KErrNone )
                {
                LOG("Connected to LLCP server");
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
                LOG("!!Failed to Connected to LLCP server");
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
                LOG("Disconnected event received");
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
    END
    }
void CLlcpConnecterAO::DoCancel()
    {
    BEGIN
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
    END
    }
//sender AO implementation

CLlcpSenderAO* CLlcpSenderAO::NewL( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket )
    {
    BEGIN
    CLlcpSenderAO* self = new (ELeave) CLlcpSenderAO( aConnection, aSocket );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    END
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
    BEGIN
    CActiveScheduler::Add( this );
    if ( !iConnection.IsConnected() )
        {
        User::Leave(KErrArgument);
        }
    END
    }

/*!
 * Destructor.
 */
CLlcpSenderAO::~CLlcpSenderAO()
    {
    BEGIN
    Cancel();
    iSendBuf0.Close();
    iSendBuf1.Close();
    END
    }
/*!
 * Transfer given data to remote device.
 */
TInt CLlcpSenderAO::Send( const TDesC8& aData )
    {
    BEGIN
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
    END
    return error;
    }

void CLlcpSenderAO::RunL()
    {
    BEGIN
    TInt error = iStatus.Int();
    if ( error == KErrNone )
        {
        if (iCurrentBuffer == EBuffer0)
            {
            LOG("Buffer0 write finished");
            iSocket.BytesWritten(iSendBuf0.Length());
            iSendBuf0.Zero();
            if(iSendBuf1.Length() > 0)
                {
                LOG("Start send buffer1 after buffer0 write finished");
                iCurrentBuffer = EBuffer1;
                iConnection.Transmit( iStatus, iSendBuf1 );
                SetActive();
                }
            }
        else //buffer1
            {
            LOG("Buffer1 write finished");
            iSocket.BytesWritten(iSendBuf1.Length());
            iSendBuf1.Zero();
            if(iSendBuf0.Length() > 0)
                {
                LOG("Start send buffer0 after buffer1 write finished");
                iCurrentBuffer = EBuffer0;
                iConnection.Transmit( iStatus, iSendBuf0 );
                SetActive();
                }
            }
        }
    else
        {
        LOG("iStatus.Int() = "<<error);
        //emit error() signal
        iSocket.Error(QtMobility::QLlcpSocket::UnknownSocketError);
        }
    END
    }
void CLlcpSenderAO::DoCancel()
    {
    BEGIN
    if (IsActive())
        {
        iConnection.TransmitCancel();
        }
    END
    }
//receiver implementation
CLlcpReceiverAO* CLlcpReceiverAO::NewL( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket )
    {
    BEGIN
    CLlcpReceiverAO* self = new (ELeave) CLlcpReceiverAO( aConnection, aSocket );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    END
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
    BEGIN
    CActiveScheduler::Add( this );
    if ( !iConnection.IsConnected() )
        {
        User::Leave(KErrArgument);
        }
    END
    }

/*!
 * Destructor.
 */
CLlcpReceiverAO::~CLlcpReceiverAO()
    {
    BEGIN
    Cancel();
    iReceiveBuf.Close();
    END
    }

TInt CLlcpReceiverAO::StartReceiveDatagram()
    {
    BEGIN
    if (IsActive())
        {
        END
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
    END
    return error;
    }

void CLlcpReceiverAO::RunL()
    {
    BEGIN
    TInt error = iStatus.Int();
    if ( error == KErrNone )
        {
        //append to buffer
        HBufC8* buf = NULL;
        buf = HBufC8::NewLC( iReceiveBuf.Length() );
        buf->Des().Copy( iReceiveBuf );
        iSocket.iReceiveBufArray.AppendL( buf );
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
        LOG(" iStatus = KErrCancel");
        }
    else
        {
        //emit error() signal
        iSocket.Error(QtMobility::QLlcpSocket::UnknownSocketError);
        }
    END
    }
void CLlcpReceiverAO::DoCancel()
    {
    BEGIN
    if (IsActive())
        {
        iConnection.ReceiveCancel();
        }
    END
    }
//EOF
