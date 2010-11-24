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


#define Q_CHECK_LLCPTYPE(function, type)  do { \
if ((type) != m_socketType) \
    { \
    qWarning(#function" was called by a" \
             " socket other than "#type""); \
    return; \
    } } while (0)

#define Q_CHECK_LLCPTYPE_PARA_3(function, type, returnValue)  do { \
if ((type) != m_socketType) \
    { \
    qWarning(#function" was called by a" \
             " socket other than "#type""); \
    return returnValue; \
    } } while (0)


#define Q_CHECK_SOCKET_HANDLER1(function)  do { \
if (m_symbianSocketType1 == NULL) \
    { \
    qWarning("Socket Type1 Handler is NULL in "#function); \
    return; \
    } } while (0)

#define Q_CHECK_SOCKET_HANDLER2(function)  do { \
if (m_symbianSocketType2 == NULL) \
    { \
    qWarning("Socket Type2 Handler is NULL in "#function); \
    return; \
    } } while (0)


QLlcpSocketPrivate::QLlcpSocketPrivate()
   : m_symbianSocketType1(NULL),
     m_symbianSocketType2(NULL),
     m_socketType(connectionUnknown)
{
    m_state = QLLCPUnconnected::Instance(this);
}


QLlcpSocketPrivate::~QLlcpSocketPrivate()
{
    delete m_symbianSocketType1;
    delete m_symbianSocketType2;
    delete m_state;
}

/*!
    Construct the socket and set it as listening state from llcp server side 
*/
QLlcpSocketPrivate::QLlcpSocketPrivate(CLlcpSocketType2* socketType2_symbian)
    : m_symbianSocketType1(NULL),
      m_symbianSocketType2(socketType2_symbian),
      m_socketType(connectionType2)
{
    m_state = QLLCPListen::Instance(this);
}


void QLlcpSocketPrivate::connectToService(QNearFieldTarget *target, const QString &serviceUri)
{
    if( connectionType1 == m_socketType){
       return;
    }
    else if (connectionUnknown == m_socketType){
       m_socketType = connectionType2;
    }

    m_state->ConnectToService(target,serviceUri);
}

void QLlcpSocketPrivate::disconnectFromService()
{
    Q_CHECK_LLCPTYPE(QLlcpSocketPrivate::disconnectFromService(),connectionType2); 
    Q_CHECK_SOCKET_HANDLER2(QLlcpSocketPrivate::disconnectFromService());
    m_state->DisconnectFromService();
}

void QLlcpSocketPrivate::invokeConnected() 
{
    m_state->ConnectToServiceComplete();
    emit connected();
}

void QLlcpSocketPrivate::invokeReadyRead()
{
    emit readyRead();
}

void QLlcpSocketPrivate::invokeBytesWritten(qint64 bytes)
{
    emit bytesWritten(bytes);
}

void QLlcpSocketPrivate::invokeStateChanged(QLlcpSocket::State socketState)
{
    emit stateChanged(socketState);
}

void QLlcpSocketPrivate::invokeError() const
{
    emit error(QLlcpSocket::UnknownSocketError);
}

void QLlcpSocketPrivate::invokeDisconnected()
{
    emit disconnected();
}

/*!
    Only used at connectless mode, create type1 socket if necessary.
*/
bool QLlcpSocketPrivate::bind(quint8 port)
{
    if( connectionType2 == m_socketType) {
       return false;
    }
    else if (connectionUnknown == m_socketType) {
       m_socketType = connectionType1;
    }

    return m_state->Bind(port);
}

/*!
    Returns true if at least one datagram is waiting to be read;
    otherwise returns false.
*/
bool QLlcpSocketPrivate::hasPendingDatagrams() const
{
    bool val = false;
    Q_CHECK_LLCPTYPE_PARA_3(QLlcpSocketPrivate::hasPendingDatagrams(),connectionType1,val);
    if (m_symbianSocketType1) {
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
    int val = -1;
    Q_CHECK_LLCPTYPE_PARA_3(QLlcpSocketPrivate::pendingDatagramSize(), connectionType1, val);
    if (m_symbianSocketType1)
    {
        val = m_symbianSocketType1->PendingDatagramSize();
    }
    return -1;
}

CLlcpSocketType1* QLlcpSocketPrivate::newSocketType1()
{
    QT_TRAP_THROWING(m_symbianSocketType1 = CLlcpSocketType1::NewL(*this));
    return m_symbianSocketType1;
}

CLlcpSocketType2* QLlcpSocketPrivate::newSocketType2()
{
    QT_TRAP_THROWING(m_symbianSocketType2 = CLlcpSocketType2::NewL(this));
    return m_symbianSocketType2;
}

/*!
    Connection-Oriented Style
*/
qint64 QLlcpSocketPrivate::writeDatagram(const char *data, qint64 size)
{
    qint64 val = -1;
    Q_CHECK_LLCPTYPE_PARA_3(QLlcpSocketPrivate::writeDatagram(),connectionType2, val);
    val = m_state->WriteDatagram(data,size);

    return val;
}

/*!
    Connection-Oriented Style
*/
qint64 QLlcpSocketPrivate::writeDatagram(const QByteArray &datagram)
{
   return writeDatagram(datagram.constData(),datagram.size());
}

/*!
    Used for Both Connection-Less & Connection-Oriented Style
    As for Connection-Oriented mode, can not pass in the port & target parameters.
*/
qint64 QLlcpSocketPrivate::readDatagram(char *data, qint64 maxSize,
                                        QNearFieldTarget **target, quint8 *port)
{
    qint64 val = -1;
    if ( (target != NULL && *target != NULL) || port != 0){
       Q_CHECK_LLCPTYPE_PARA_3(QLlcpSocketPrivate::writeDatagram(),connectionType1, val);
    }

    val = m_state->ReadDatagram(data,maxSize,target,port);

    if (val < 0) {
       emit error(QLlcpSocket::UnknownSocketError);
    }
    return val;
}

qint64 QLlcpSocketPrivate::writeDatagram(const char *data, qint64 size,
                                         QNearFieldTarget *target, quint8 port)
{ 
    if( connectionType2 == m_socketType){
       return -1;
    }
    else if (connectionUnknown == m_socketType){
       m_socketType = connectionType1;
    }

    qint64 val = m_state->WriteDatagram(data,size,target,port);
    if (val < 0) {
        emit error(QLlcpSocket::UnknownSocketError);
    }

    return val;
}

qint64 QLlcpSocketPrivate::writeDatagram(const QByteArray &datagram,
                                         QNearFieldTarget *target, quint8 port)
{
    return writeDatagram(datagram.constData(),datagram.size(),target,port);
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

bool QLlcpSocketPrivate::waitForReadyRead(int msecs)
{
    return m_state->WaitForReadyRead(msecs);
}

bool QLlcpSocketPrivate::waitForBytesWritten(int msecs)
{
    return m_state->WaitForBytesWritten(msecs);
}

bool QLlcpSocketPrivate::waitForConnected(int msecs)
{
    return m_state->WaitForConnected(msecs);
}

bool QLlcpSocketPrivate::waitForDisconnected(int msecs)
{
    return m_state->WaitForDisconnected(msecs);
}

void QLlcpSocketPrivate::changeState(QLLCPSocketState* state)
{
    if (m_state)
    {
       delete m_state;
    }
    m_state = state;
}

#include "moc_qllcpsocket_symbian_p.cpp"

QTM_END_NAMESPACE
