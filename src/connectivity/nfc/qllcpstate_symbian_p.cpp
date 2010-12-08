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
#include "qllcpsocket_symbian_p.h"
#include "qllcpstate_symbian_p.h"
#include "symbian/llcpsockettype1_symbian.h"
#include "symbian/llcpsockettype2_symbian.h"
#include "symbian/nearfieldutility_symbian.h"

QTM_BEGIN_NAMESPACE

QLLCPSocketState::QLLCPSocketState(QLlcpSocketPrivate* aSocket)
      :m_socket(aSocket)
{}

/*!
    Bind can only be proceeded under the unconnected state.
*/
bool QLLCPUnconnected::Bind(quint8 port)
{
    CLlcpSocketType1* socketHandler =  m_socket->socketType1Instance();
    bool isOK = socketHandler->Bind(port);
    if (isOK)
    {
        m_socket->changeState(m_socket->getBindState());
        emit m_socket->invokeStateChanged(QLlcpSocket::BoundState);
    }
    return isOK;
}

/*!
    Connection-Less Mode
*/
bool QLLCPUnconnected::WaitForBytesWritten(int msecs)
{
   bool isOK = false;

   CLlcpSocketType1* socketHandler =  m_socket->socketType1Instance();
   if (socketHandler != NULL)
   {
       isOK = socketHandler->WaitForBytesWritten(msecs);
   }
   return isOK;
}

/*!
    Connection-Less Mode, same behaviour as QLLCPUnconnected::WaitForBytesWritten
*/
bool QLLCPBind::WaitForBytesWritten(int msecs)
{
   bool isOK = false;
   CLlcpSocketType1* socketHandler = m_socket->socketType1Handler();
   if (socketHandler != NULL)
   {
       isOK = socketHandler->WaitForBytesWritten(msecs);
   }
   return isOK;
}

/*!
    Connection-Less Mode
*/
bool QLLCPBind::WaitForReadyRead(int msecs)
{
    bool isOK = false;
    CLlcpSocketType1* socketHandler = m_socket->socketType1Handler();
    if (socketHandler != NULL && socketHandler->HasPendingDatagrams())
    {
       isOK = true;
    }

    return isOK;
}


/*!
    Connection-Less Mode
*/
qint64 QLLCPUnconnected::WriteDatagram(const char *data, qint64 size,
                                       QNearFieldTarget *target, quint8 port)
{
    qint64 val = -1;

    CLlcpSocketType1* socketHandler =  m_socket->socketType1Instance();

    TPtrC8 myDescriptor((const TUint8*)data, size);
    val = socketHandler->StartWriteDatagram(myDescriptor, port);

    return val;
}

/*!
    Connection-Less Mode
*/
qint64 QLLCPBind::WriteDatagram(const char *data, qint64 size,
                              QNearFieldTarget *target, quint8 port)
{
    qint64 val = -1;
    CLlcpSocketType1* socketHandler = m_socket->socketType1Handler();
    if (socketHandler != NULL)
    {
        TPtrC8 myDescriptor((const TUint8*)data, size);
        val = socketHandler->StartWriteDatagram(myDescriptor, port);
    }

    return val;
}


/*!
    Connection-Oriented Mode - Client side socket write
*/
qint64 QLLCPConnected::WriteDatagram(const char *data, qint64 size)
{
    qint64 val = -1;
    CLlcpSocketType2* socketHandler = m_socket->socketType2Handler();
    if (socketHandler != NULL)
    {
        TPtrC8 myDescriptor((const TUint8*)data, size);
        val = socketHandler->StartWriteDatagram(myDescriptor);
    }

    return val;
}

/*!
    Connection-Oriented Mode
*/
bool QLLCPConnected::WaitForBytesWritten(int msecs)
{
   bool isOK = false;
   CLlcpSocketType2* socketHandler = m_socket->socketType2Handler();
   if (socketHandler != NULL)
   {
       isOK = socketHandler->WaitForBytesWritten(msecs);
   }
   return isOK;
}

/*!
    Connection-Oriented Mode
*/
bool QLLCPConnected::WaitForReadyRead(int msecs)
{
    bool isOK = false;
    CLlcpSocketType2* socketHandler = m_socket->socketType2Handler();
    if (socketHandler != NULL)
    {
        isOK = socketHandler->WaitForReadyRead(msecs);
    }
    return isOK;
}

/*!
    Connection-Oriented Mode
*/
bool QLLCPConnected::WaitForDisconnected(int msecs)
{
    bool isOK = false;
    CLlcpSocketType2* socketHandler = m_socket->socketType2Handler();
    if (socketHandler != NULL)
    {
        isOK = socketHandler->WaitForDisconnected(msecs);
        m_socket->changeState(m_socket->getUnconnectedState());
        m_socket->invokeStateChanged(QLlcpSocket::UnconnectedState);
    }
    return isOK;
}

/*!
    Connection-Less Mode
*/
qint64 QLLCPBind::ReadDatagram(char *data, qint64 maxSize,
                               QNearFieldTarget **target, quint8 *port)
{
    Q_UNUSED(target);
    qint64 val = -1;
    CLlcpSocketType1* socketHandler = m_socket->socketType1Handler();
    if (socketHandler != NULL)
    {
        TPtr8 ptr((TUint8*)data, (TInt)maxSize);
        if (port == NULL){
           val = socketHandler->ReadDatagram(ptr);
        }
        else {
           quint8 portNum = *port;
           val = socketHandler->ReadDatagram(ptr, (TInt8&)portNum);
        }
    }

    return val;
}


/*!
    Connection-Oriented Mode - Client side socket read
*/
qint64 QLLCPConnected::ReadDatagram(char *data, qint64 maxSize,QNearFieldTarget **target, quint8 *port )
{
    Q_UNUSED(target);
    Q_UNUSED(port);
    qint64 val = -1;
    CLlcpSocketType2* socketHandler = m_socket->socketType2Handler();
    if (socketHandler != NULL)
    {
        // The length of the descriptor is set to zero
        // and its maximum length is set to maxSize
        TPtr8 ptr((TUint8*)data, (TInt)maxSize);

        bool ret = socketHandler->ReceiveData(ptr);
        if(ret)
            val = ptr.Length();

    }

    return val;
}

/*!
    Connection-Oriented Mode
*/
void QLLCPClosing::DisconnectFromService()
{
    CLlcpSocketType2* socketHandler = m_socket->socketType2Handler();
    if (socketHandler && socketHandler->DisconnectFromService() == KErrNone )
    {
        m_socket->changeState(m_socket->getUnconnectedState());
        m_socket->invokeStateChanged(QLlcpSocket::UnconnectedState);

        // only emit disconnected if we were connected before
        m_socket->invokeDisconnected();
    }
    else
    {
       m_socket->invokeError();
    }

}

/*!
    Connection-Oriented Mode
*/
void QLLCPConnected::DisconnectFromService()
{
   CLlcpSocketType2* socketHandler = m_socket->socketType2Handler();
   if (socketHandler)
   {
       m_socket->changeState(m_socket->getClosingState());
       m_socket->invokeStateChanged(QLlcpSocket::ClosingState);
       // If there is pending data waiting to be written,
       // Symbian backend Socket will wait until all data has been written
       if (socketHandler->DisconnectFromService() == KErrNone)
       {
          m_socket->changeState(m_socket->getUnconnectedState());
          m_socket->invokeStateChanged(QLlcpSocket::UnconnectedState);
       }
       else
       {
          m_socket->invokeError();
       }
   }
   else
   {
      m_socket->invokeError();
   }
   return;
}

/*!
    Connection-Oriented Mode
*/
void QLLCPConnecting::DisconnectFromService()
{
   CLlcpSocketType2* socketHandler = m_socket->socketType2Handler();
   if (NULL != socketHandler)
   {
       if (socketHandler->DisconnectFromService() != KErrNone)
       {
          m_socket->invokeError();
       }
       m_socket->changeState(m_socket->getUnconnectedState());
       m_socket->invokeStateChanged(QLlcpSocket::UnconnectedState);
   }
   else
   {
      m_socket->invokeError();
   }
   return;
}

/*!
    Connection-Oriented Mode
*/
void QLLCPUnconnected::DisconnectFromService()
{
   qWarning("QLlcpSocket::disconnectToHost() called when already disconnected");
   return;
}

/*!
    Connection-Oriented Mode
*/
void QLLCPConnecting::ConnectToServiceComplete()
    {
    m_socket->changeState(m_socket->getConnectedState());
    m_socket->invokeStateChanged(QLlcpSocket::ConnectedState);
    }

/*!
    Connection-Oriented Mode
*/
bool QLLCPConnecting::WaitForConnected(int msecs)
{
    bool isOK = false;
    CLlcpSocketType2* socketHandler = m_socket->socketType2Handler();
    if (socketHandler != NULL)
    {
        isOK = socketHandler->WaitForConnected(msecs);
    }
    return isOK;
}

/*!
    Connection-Oriented Mode
*/
void QLLCPUnconnected::ConnectToService(QNearFieldTarget *target, const QString &serviceUri)
{

    CLlcpSocketType2* socketHandler =  m_socket->socketType2Instance();
    TPtrC8 serviceName = QNFCNdefUtility::FromQStringToTptrC8(serviceUri);

    if (socketHandler && KErrNone == socketHandler->ConnectToService(serviceName) )
    {
        m_socket->changeState(m_socket->getConnectingState());
        m_socket->invokeStateChanged(QLlcpSocket::ConnectingState);
    }
    else
    {
        m_socket->invokeError();
    }
}

/*!
    Connection-Oriented Mode
*/
void QLLCPConnecting::ConnectToService(QNearFieldTarget *target, const QString &serviceUri)
    {
    Q_UNUSED(target);
    Q_UNUSED(serviceUri);
    m_socket->invokeError();
    qWarning("QLlcpSocket::connectToService() called when already connecting");
    return;
    }

/*!
    Connection-Oriented Mode
*/
void QLLCPConnected::ConnectToService(QNearFieldTarget *target, const QString &serviceUri)
    {
    Q_UNUSED(target);
    Q_UNUSED(serviceUri);
    qWarning("QLlcpSocket::connectToService() called when already connected");
    return;
    }

/*!
    Connection-Oriented Mode
*/
void QLLCPClosing::ConnectToService(QNearFieldTarget *target, const QString &serviceUri)
    {
    Q_UNUSED(target);
    Q_UNUSED(serviceUri);
    qWarning("QLlcpSocket::connectToService() called when already connected");
    return;
    }

/*!
    Constructors
*/
QLLCPUnconnected::QLLCPUnconnected(QLlcpSocketPrivate* aSocket)
    :QLLCPSocketState(aSocket)
{}


QLLCPBind::QLLCPBind(QLlcpSocketPrivate* aSocket)
    :QLLCPSocketState(aSocket)
{}

QLLCPConnecting::QLLCPConnecting(QLlcpSocketPrivate* aSocket)
    :QLLCPSocketState(aSocket)
{}

QLLCPConnected::QLLCPConnected(QLlcpSocketPrivate* aSocket)
    :QLLCPSocketState(aSocket)
{}

QLLCPClosing::QLLCPClosing(QLlcpSocketPrivate* aSocket)
    :QLLCPSocketState(aSocket)
{}

qint64 QLLCPSocketState::ReadDatagram(char *data, qint64 maxSize,
        QNearFieldTarget **target, quint8 *port)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    Q_UNUSED(target);
    Q_UNUSED(port);
    m_socket->invokeError();
    return -1;
}

/*!
    State base class default implementation
*/
qint64 QLLCPSocketState::WriteDatagram(const char *data, qint64 size,
                                         QNearFieldTarget *target, quint8 port)
{
    Q_UNUSED(data);
    Q_UNUSED(size);
    Q_UNUSED(target);
    Q_UNUSED(port);
    m_socket->invokeError();
    return -1;
}

qint64 QLLCPSocketState::WriteDatagram(const char *data, qint64 size)
{
    Q_UNUSED(data);
    Q_UNUSED(size);
    m_socket->invokeError();
    return -1;
}

bool QLLCPSocketState::Bind(quint8 port)
{
    m_socket->invokeError();
    return false;
}

 bool QLLCPSocketState::WaitForBytesWritten(int msecs)
 {
    Q_UNUSED(msecs);
    return false;
 }

 bool QLLCPSocketState::WaitForReadyRead(int msecs)
 {
    Q_UNUSED(msecs);
    return false;
 }

bool QLLCPSocketState::WaitForConnected(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

bool QLLCPSocketState::WaitForDisconnected(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

void QLLCPSocketState::ConnectToService(QNearFieldTarget *target, const QString &serviceUri)
{
   Q_UNUSED(target);
   Q_UNUSED(serviceUri);
   m_socket->invokeError();
}

void QLLCPSocketState::ConnectToServiceComplete()
{
   m_socket->invokeError();
}

void QLLCPSocketState::DisconnectFromService()
{
   m_socket->invokeError();
}


QTM_END_NAMESPACE
