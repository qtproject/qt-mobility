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

#include "llcpsockettype2_symbian.h"

// TODO
// will obslete with API updated
const TInt KInterestingSsap = 35;

/*!
    CLlcpSocketType2::ContructL()
*/
void CLlcpSocketType2::ConstructL()
    {
	User::LeaveIfError(iNfcServer.Open());
    iLlcp = CLlcpProvider::NewL( iNfcServer );
    }

/*!
    CLlcpSocketType2::CLlcpSocketType2()
*/
CLlcpSocketType2::CLlcpSocketType2(MLlcpConnOrientedTransporter* aTransporter, QtMobility::QLlcpSocketPrivate* aCallback)
    : iLlcp( NULL ),
      iTransporter(aTransporter),
    iCallback(aCallback)
    {
    }


CLlcpSocketType2* CLlcpSocketType2::NewL(QtMobility::QLlcpSocketPrivate* aCallback)
    {
    CLlcpSocketType2* self = new (ELeave) CLlcpSocketType2(NULL,aCallback);
    self->ConstructL();
    return self;
    }

CLlcpSocketType2* CLlcpSocketType2::NewL(MLlcpConnOrientedTransporter* aTransporter, QtMobility::QLlcpSocketPrivate* aCallback)
	{
    CLlcpSocketType2* self = new (ELeave) CLlcpSocketType2(aTransporter,aCallback);
    self->ConstructL();
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
    }

/*!
    Connects to the service identified by the URI \a serviceUri (on \a target).
*/
void CLlcpSocketType2::ConnectToServiceL( const TDesC8& aServiceName)
    {    
	if ( !iConnecter && !iTransporter)
		{
		iTransporter = iLlcp->CreateConnOrientedTransporterL( KInterestingSsap );
		iConnecter = CLlcpConnecterAO::NewL( *iTransporter, *this );
		iConnecter->Connect( aServiceName );
		}
    }

/*!
    Disconnects the socket.
*/
void CLlcpSocketType2::DisconnectFromService()
    {
    if (iConnecter)
        {
		iConnecter->Disconnect();
        }
    Cleanup();
    }

/*!
    Sends the datagram at aData  to the service that this socket is connected to.
    Returns the number of bytes sent on success; otherwise return -1;
*/
TInt CLlcpSocketType2::StartWriteDatagramL(const TDesC8& aData)
    {

    if (!iTransporter)
    	{
		return KErrNotReady;
    	}
    
    if (!iSender)
    	{
		iSender = CLlcpSenderAO::NewL(*iTransporter, *this);
    	}
    TInt val = -1;
    TInt error = KErrNone;
    //asynchronous transfer
    error = iSender->Send( aData);
    if (KErrNone == error)
        {
        val =  0;
        }
    return val; 
    }

bool CLlcpSocketType2::ReceiveData(TDesC8& aData)
    {
	//TODO fetch data from cache pool
      return EFalse;
    }


/*!
    Cancel the Receive/Transfer and destroy the local/remote connection.
*/
void CLlcpSocketType2::Cleanup()
    {
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
    }
void CLlcpSocketType2::Error(QtMobility::QLlcpSocket::Error socketError)
	{
	//TODO emit error
	}
void CLlcpSocketType2::StateChanged(QtMobility::QLlcpSocket::State socketState)
	{
	//TODO emit stateChange
	}
void CLlcpSocketType2::ReadyRead()
	{
	//TODO emit readyRead()
	}
void CLlcpSocketType2::BytesWritten(qint64 bytes)
	{
	//TODO emit bytesWritten();
	}
// connecter implementation
CLlcpConnecterAO* CLlcpConnecterAO::NewL( MLlcpConnOrientedTransporter& aConnection, CLlcpSocketType2& aSocket )
	{
	CLlcpConnecterAO* self = new (ELeave) CLlcpConnecterAO( aConnection, aSocket );
	self->ConstructL();
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
void CLlcpConnecterAO::Connect(const TDesC8& /*aServiceName*/)
    {
    if ( iConnState == ENotConnected )
        {
        // Starting connecting if is in idle state
        iConnection.Connect( iStatus );
        SetActive();
        iConnState = EConnecting;
        //emit connecting signal
        iSocket.StateChanged(QtMobility::QLlcpSocket::ConnectingState);
        }
    }

/*!
 * Disonnect with remote peer.
 */
void CLlcpConnecterAO::Disconnect()
    {     
    if ( iConnState != ENotConnected )
        {
        iConnection.Disconnect();
        iConnState = ENotConnected;
        //TODO emit QAbstractSocket::ClosingState;
        iSocket.StateChanged(QtMobility::QLlcpSocket::ClosingState);
        }    
    }
void CLlcpConnecterAO::RunL()
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
				//TODO emit connected signal
				iSocket.StateChanged(QtMobility::QLlcpSocket::ConnectedState);
		        // Starting listening disconnect event
		        iConnection.WaitForDisconnection( iStatus );
		        SetActive();
				}
			else
				{
				//KErrNotSupported when remote peer has lost from the near field. 
				iConnState = ENotConnected;
				//TODO emit error signal
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
				//TODO emit disconnected signal
				iSocket.StateChanged(QtMobility::QLlcpSocket::ClosingState);
				}
			else
				{
				iConnState = ENotConnected;
				//TODO emit error signal
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
	self->ConstructL();
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
    iSendBuf.Close();
    }
/*!
 * Transfer given data to remote device.
 */
TInt CLlcpSenderAO::Send( const TDesC8& aData )
    {
	TInt error = KErrNone;
    if ( !IsActive() )
        {
		// Copying data to internal buffer. 
		iSendBuf.Zero();
		error = iSendBuf.ReAlloc( aData.Length() );
		
		if ( error == KErrNone )
			{
			iSendBuf.Append( aData );
			
			// Sending data
			iConnection.Transmit( iStatus, iSendBuf );
			SetActive();
			}
        }
    else
    	{
		error = KErrInUse;
    	}
    return error;
    }

void CLlcpSenderAO::RunL()
	{
	TInt error = iStatus.Int();
	if ( error == KErrNone )
		{
		//TODO emit bytesWritten(qint64 bytes) signal
		iSocket.BytesWritten(iSendBuf.Length());
		}
	else
		{
		//TODO emit error() signal
		iSocket.Error(QtMobility::QLlcpSocket::UnknownSocketError);
		}
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
	self->ConstructL();
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

TInt CLlcpReceiverAO::StartReceiveDatagram(TInt64 aMaxSize)
	{
	if (IsActive())
		{
		return KErrInUse;
		}
    TInt length = 0;
    TInt error = KErrNone;
    length = iConnection.SupportedDataLength();
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
             iConnection.Receive( iStatus, iReceiveBuf );
             SetActive();
             }
         }
     else
         {
         // if length is 0 or negative, LLCP link is destroyed.
         error = KErrNotReady;
         }
     return error;
	}

void CLlcpReceiverAO::RunL()
	{
	TInt error = iStatus.Int();
	if ( error == KErrNone )
		{
		//TODO append to buffer pool
		//TODO emit readyRead() signal
		iSocket.ReadyRead();
		}
	else
		{
		//TODO emit error() signal
		iSocket.Error(QtMobility::QLlcpSocket::UnknownSocketError);
		}
	}
void CLlcpReceiverAO::DoCancel()
	{
	if (IsActive())
		{
		iConnection.ReceiveCancel();
		}
	}
