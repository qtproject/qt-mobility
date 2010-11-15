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
#include "symbian/llcpsockettype1_symbian.h"
#include "symbian/llcpsockettype2_symbian.h"
#include "symbian/qnearfieldutility_symbian.h" 

QTM_BEGIN_NAMESPACE


#define Q_CHECK_LLCPTYPE(function, type)  do { \
if ((type) != m_socketType) \
    { \
    qWarning(#function" was called by a" \
             " socket other than "#type""); \
    return; \
    } } while (0)

#define Q_CHECK_SOCKET_HANDLER2(function)  do { \
if (m_symbianSocketType2 == NULL) \
    { \
    qWarning("Socket Type1 Handler is NULL in "#function); \
    return; \
    } } while (0)




QLlcpSocketPrivate::QLlcpSocketPrivate()
   : m_symbianSocketType1(NULL),
     m_symbianSocketType2(NULL),
     m_socketType(connectionUnknown),
     m_port(-1)
{
    // lazy initializtion according to the llcp tranporation type
}

QLlcpSocketPrivate::~QLlcpSocketPrivate()
{
    delete m_symbianSocketType1;
    delete m_symbianSocketType2;
}

void QLlcpSocketPrivate::connectToService(QNearFieldTarget *target, const QString &serviceUri)
{
    Q_UNUSED(target);   
    Q_CHECK_LLCPTYPE(QLlcpSocketPrivate::connectToService(),connectionType2);  
    
    if (!m_symbianSocketType2)
        {
        QT_TRAP_THROWING(m_symbianSocketType2 = CLlcpSocketType2::NewL(this));
        }
     
    TPtrC8 serviceName = QNFCNdefUtility::FromQStringToTptrC8(serviceUri);
    m_symbianSocketType2->ConnectToServiceL(serviceName);
    m_socketType = connectionType2;
}

//TODO
/*  API miss connection-less conectionToHost */
void QLlcpSocketPrivate::connectToHost(QNearFieldTarget *target, const quint8 portNum)
{
    Q_UNUSED(target);   
    Q_CHECK_LLCPTYPE(QLlcpSocketPrivate::connectToHost(),connectionType1);  
    
    m_port = portNum;
}
   

void QLlcpSocketPrivate::disconnectFromService()
{
    Q_CHECK_LLCPTYPE(QLlcpSocketPrivate::disconnectFromService(),connectionType2); 
    if (m_symbianSocketType2)
        {
        m_symbianSocketType2->DisconnectFromService();
        }
    else
        {
        emit error(QLlcpSocket::UnknownSocketError);
        }
}

void QLlcpSocketPrivate::disconnectFromHost()
{
    Q_CHECK_LLCPTYPE(QLlcpSocketPrivate::disconnectFromService(),connectionType2); 
    Q_CHECK_SOCKET_HANDLER2(QLlcpSocketPrivate::disconnectFromHost());
    m_state->DisconnectFromHost();
}


bool QLlcpSocketPrivate::bind(quint8 port)
{
    Q_CHECK_LLCPTYPE( QLlcpSocketPrivate::bind(),connectionType1); 
    
    if (!m_symbianSocketType1)
        {
        QT_TRAP_THROWING(m_symbianSocketType1 = CLlcpSocketType1::NewL(*this));
        }
    return m_symbianSocketType1->Bind(port);
}

/*!
    Returns true if at least one datagram is waiting to be read;
    otherwise returns false.
*/
bool QLlcpSocketPrivate::hasPendingDatagrams() const
{
    Q_CHECK_LLCPTYPE(QLlcpSocketPrivate::hasPendingDatagrams(),connectionType1); 
    bool val = false;
    if (m_symbianSocketType1)
        {
        val = m_symbianSocketType1->HasPendingDatagrams();
        }
    return val;
}

/*!
    Returns the size of the first pending connectionless datagram. If there is
    no datagram available, this function returns -1.
*/
qint64 QLlcpSocketPrivate::pendingDatagramSize() const
{
    Q_CHECK_LLCPTYPE(QLlcpSocketPrivate::pendingDatagramSize(), connectionType1); 
    int val = -1;
    if (m_symbianSocketType1)
        {
        val = m_symbianSocketType1->PendingDatagramSize();
        }    
    return -1;
}

qint64 QLlcpSocketPrivate::writeDatagram(const char *data, qint64 size)
{
    Q_CHECK_LLCPTYPE(QLlcpSocketPrivate::writeDatagram(), connectionType2);
    Q_UNUSED(data);
    Q_UNUSED(size);

    return -1;
}

qint64 QLlcpSocketPrivate::writeDatagram(const QByteArray &datagram)
{
    Q_CHECK_LLCPTYPE(QLlcpSocketPrivate::writeDatagram(),connectionType2);
    Q_UNUSED(datagram);

    return -1;
}

qint64 QLlcpSocketPrivate::readDatagram(char *data, qint64 maxSize,
                                        QNearFieldTarget **target, quint8 *port)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    Q_UNUSED(target);
    
    qint64 val = -1;
    if (connectionType1 == m_socketType && m_symbianSocketType1)
        {
        QT_TRAP_THROWING(m_symbianSocketType1 = CLlcpSocketType1::NewL(*this));
        TPtr8 ptr((TUint8*)data, (TInt)maxSize, (TInt)maxSize );
        val = m_symbianSocketType1->ReadDatagram(ptr);           
        }
    
    //m_port = *port;
    if (val < 0)
        {
        emit error(QLlcpSocket::UnknownSocketError);   
        }
   
    return val;
}

qint64 QLlcpSocketPrivate::writeDatagram(const char *data, qint64 size,
                                         QNearFieldTarget *target, quint8 port)
{
    Q_UNUSED(target);
    
    Q_CHECK_LLCPTYPE(QLlcpSocketPrivate::writeDatagram(),connectionType1);
    
    qint64 val = -1;
    if (connectionType1 == m_socketType)
       {
        if (m_symbianSocketType1 == NULL)
            {
            QT_TRAP_THROWING(m_symbianSocketType1 = CLlcpSocketType1::NewL(*this));
            }   
    
        TPtrC8 myDescriptor((const TUint8*)data, size);
        val = m_symbianSocketType1->StartWriteDatagram(myDescriptor, m_port);
       }
    
    //m_port = port;

    return val;
}

qint64 QLlcpSocketPrivate::writeDatagram(const QByteArray &datagram,
                                         QNearFieldTarget *target, quint8 port)
{
    Q_UNUSED(target);
    Q_CHECK_LLCPTYPE(QLlcpSocketPrivate::writeDatagram(),connectionType1);
    
    if (m_symbianSocketType1 == NULL)
        {
        QT_TRAP_THROWING(m_symbianSocketType1 = CLlcpSocketType1::NewL(*this));
        }    
    
    const TDesC8& myDescriptor = QNFCNdefUtility::FromQByteArrayToTPtrC8(datagram);
    qint64 val = m_symbianSocketType1->StartWriteDatagram(myDescriptor, port);
    return val;
}

QLlcpSocket::Error QLlcpSocketPrivate::error() const
{
    return QLlcpSocket::UnknownSocketError;
}

QLlcpSocket::State QLlcpSocketPrivate::state() const
{
    return QLlcpSocket::UnconnectedState;
}

qint64 QLlcpSocketPrivate::readData(char *data, qint64 maxlen)
{
    return readDatagram(data,maxlen);
}

qint64 QLlcpSocketPrivate::writeData(const char *data, qint64 len)
{
    return writeDatagram(data,len);
}


void QLlcpSocketPrivate::invokeBytesWritten(qint64 bytes)
{
    emit bytesWritten(bytes);
}

void QLlcpSocketPrivate::invokeReadyRead()
{
    emit readyRead();    
}

void QLlcpSocketPrivate::invokeError()
{
    emit error(QLlcpSocket::UnknownSocketError);    
}

void QLlcpSocketPrivate::invokeStateChange(QLlcpSocket::State socketState)
{
    emit stateChanged(socketState);
}


bool QLlcpSocketPrivate::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);

    return false;
}

bool QLlcpSocketPrivate::waitForBytesWritten(int msecs)
{
    Q_UNUSED(msecs);

    return false;
}

bool QLlcpSocketPrivate::waitForConnected(int msecs)
{
    Q_UNUSED(msecs);

    return false;
}

bool QLlcpSocketPrivate::waitForDisconnected(int msecs)
{
    Q_UNUSED(msecs);

    return false;
}

void QLlcpSocketPrivate::ChangeState(QLLCPState* state)
    {
    m_state = state;
    }

///////////////////// State Machine 
QLLCPState::QLLCPState(QLlcpSocketPrivate* aSocket)
      :m_socket(aSocket)
    {}

void QLLCPState::ChangeState(QLlcpSocketPrivate*t,QLLCPState *s )
    {
    t->ChangeState(s);
    }

// Bind can only be proceeded under the unconnectted state.
void QLLCPType1Unconnected::Bind(quint8 port)
{
    m_socket->bind(port);
    ChangeState(m_socket, QLLCPType1Unconnected::Instance(m_socket));
    m_socket->invokeStateChange(QLlcpSocket::UnconnectedState);
}

// simple return, already unconnected
void QLLCPType1Unconnected::DisconnectFromHost()
{
    return;
}

void QLLCPType1Bind::Transmit(const char *data, qint64 size,QNearFieldTarget *target, quint8 port)
{
    m_socket->writeDatagram(data, size,target,port);
}

qint64 QLLCPType1Bind::readDatagram(char *data, qint64 maxSize, QNearFieldTarget **target, quint8 *port)
{
    return m_socket->readDatagram(data, maxSize,target,port);
}

void QLLCPType1Connected::DisconnectFromHost()
{
    ChangeState(m_socket, QLLCPType1Closing::Instance(m_socket));
    m_socket->invokeStateChange(QLlcpSocket::ClosingState);
    m_socket->disconnectFromHost();
}

void QLLCPType1Closing::DisconnectFromHost()
{
    ChangeState(m_socket, QLLCPType1Unconnected::Instance(m_socket));
    m_socket->invokeStateChange(QLlcpSocket::UnconnectedState);
    
    CLlcpSocketType2* socket = m_socket->socketType2Handler();
    socket->DisconnectFromService();    
}

// Constructors
QLLCPType1Unconnected::QLLCPType1Unconnected(QLlcpSocketPrivate* aSocket)
    :QLLCPState(aSocket)
{}

QLLCPType1LookUp::QLLCPType1LookUp(QLlcpSocketPrivate* aSocket)
    :QLLCPState(aSocket)
{}

QLLCPType1Bind::QLLCPType1Bind(QLlcpSocketPrivate* aSocket)
    :QLLCPState(aSocket)
{}

QLLCPType1Connecting::QLLCPType1Connecting(QLlcpSocketPrivate* aSocket)
    :QLLCPState(aSocket)
{}

QLLCPType1Connected::QLLCPType1Connected(QLlcpSocketPrivate* aSocket)
    :QLLCPState(aSocket)
{}

QLLCPType1Closing::QLLCPType1Closing(QLlcpSocketPrivate* aSocket)
    :QLLCPState(aSocket)
{}


QLLCPState* QLLCPType1Unconnected::m_instance = NULL;
QLLCPState* QLLCPType1Bind::m_instance = NULL;
QLLCPState* QLLCPType1LookUp::m_instance = NULL;
QLLCPState* QLLCPType1Connecting::m_instance = NULL;
QLLCPState* QLLCPType1Connected::m_instance = NULL;
QLLCPState* QLLCPType1Closing::m_instance = NULL;

QLLCPState* QLLCPType1Unconnected::Instance(QLlcpSocketPrivate* aSocket)
{
   if (m_instance == NULL)
   {
       m_instance = new QLLCPType1Unconnected(aSocket);
   }
   return m_instance;
}

QLLCPState* QLLCPType1Bind::Instance(QLlcpSocketPrivate* aSocket)
{
   if (m_instance == NULL)
   {
       m_instance = new QLLCPType1Bind(aSocket);
   }
   return m_instance;
}
      
QLLCPState* QLLCPType1LookUp::Instance(QLlcpSocketPrivate* aSocket)
{
   if (m_instance == NULL)
   {
       m_instance = new QLLCPType1LookUp(aSocket);
   }
   return m_instance;
}

QLLCPState* QLLCPType1Connecting::Instance(QLlcpSocketPrivate* aSocket)
{
   if (m_instance == NULL)
   {
       m_instance = new QLLCPType1Connecting(aSocket);
   }
   return m_instance;
}
      
QLLCPState* QLLCPType1Connected::Instance(QLlcpSocketPrivate* aSocket)
{
   if (m_instance == NULL)
   {
       m_instance = new QLLCPType1Connected(aSocket);
   }
   return m_instance;
}

QLLCPState* QLLCPType1Closing::Instance(QLlcpSocketPrivate* aSocket)
{
   if (m_instance == NULL)
   {
       m_instance = new QLLCPType1Closing(aSocket);
   }
   return m_instance;
}

#include "moc_qllcpsocket_symbian_p.cpp"

QTM_END_NAMESPACE
