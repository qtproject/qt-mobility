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


#define Q_CHECK_VALID_LLCPTYPE(type)  { \
if ((type) != m_socketType) \
    { \
    emit error(QLlcpSocket::UnknownSocketError); \
    return; \
    } \
}

QLlcpSocketPrivate::QLlcpSocketPrivate()
   : m_symbianSocketType1(NULL),
     m_symbianSocketType2(NULL),
     m_socketType(connectionUnknown),
     m_readDatagramStarted(false),
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
    
    Q_CHECK_VALID_LLCPTYPE(connectionType2);  
    
    if (!m_symbianSocketType2)
        {
        QT_TRAP_THROWING(m_symbianSocketType2 = CLlcpSocketType2::NewL(this));
        }
     
    TPtrC8 serviceName = QNFCNdefUtility::FromQStringToTptrC8(serviceUri);
    m_symbianSocketType2->ConnectToService(serviceName);
    m_socketType = connectionType2;
}


void QLlcpSocketPrivate::disconnectFromService()
{
    Q_CHECK_VALID_LLCPTYPE(connectionType2); 
    if (m_symbianSocketType2)
        {
        m_symbianSocketType2->DisconnectFromService();
        }
    else
        {
        emit error(QLlcpSocket::UnknownSocketError);
        }
}

bool QLlcpSocketPrivate::bind(quint8 port)
{
    Q_CHECK_VALID_LLCPTYPE(connectionType1); 
    
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
    Q_CHECK_VALID_LLCPTYPE(connectionType1); 
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
    Q_CHECK_VALID_LLCPTYPE(connectionType1); 
    int val = -1;
    if (m_symbianSocketType1)
        {
        val = m_symbianSocketType1->PendingDatagramSize();
        }    
    return -1;
}

qint64 QLlcpSocketPrivate::writeDatagram(const char *data, qint64 size)
{
    Q_CHECK_VALID_LLCPTYPE(connectionType2);
    Q_UNUSED(data);
    Q_UNUSED(size);

    return -1;
}

qint64 QLlcpSocketPrivate::writeDatagram(const QByteArray &datagram)
{
    Q_CHECK_VALID_LLCPTYPE(connectionType2);
    Q_UNUSED(datagram);

    return -1;
}

qint64 QLlcpSocketPrivate::readDatagram(char *data, qint64 maxSize,
                                        QNearFieldTarget **target, quint8 *port)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    Q_UNUSED(target);
    m_port = *port;
    qint64 val = readData(data,maxSize);
   
    return val;
}

qint64 QLlcpSocketPrivate::writeDatagram(const char *data, qint64 size,
                                         QNearFieldTarget *target, quint8 port)
{
    Q_UNUSED(target);
    
    Q_CHECK_VALID_LLCPTYPE(connectionType1);
    
    m_port = port;
    qint64 val = writeData(data,size);

    return val;
}

qint64 QLlcpSocketPrivate::writeDatagram(const QByteArray &datagram,
                                         QNearFieldTarget *target, quint8 port)
{
    Q_UNUSED(target);
    Q_CHECK_VALID_LLCPTYPE(connectionType1);
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
    qint64 val = -1;
    if (connectionType1 == m_socketType && m_symbianSocketType1)
        {
           if (m_readDatagramStarted)
               {
               QT_TRAP_THROWING(m_symbianSocketType1 = CLlcpSocketType1::NewL(*this));
               TPtr8 ptr((TUint8*)data, (TInt)maxlen, (TInt)maxlen );
               val = m_symbianSocketType1->ReceiveData(ptr);           
               }
           else
               {
               m_symbianSocketType1->StartReadDatagram(maxlen);    
               m_readDatagramStarted = true;
               }
         }
        
    return val;
}

qint64 QLlcpSocketPrivate::writeData(const char *data, qint64 len)
{
    qint64 val = -1;
    if (connectionType1 == m_socketType)
       {
        if (m_symbianSocketType1 == NULL)
            {
            QT_TRAP_THROWING(m_symbianSocketType1 = CLlcpSocketType1::NewL(*this));
            }   
    
        TPtrC8 myDescriptor((const TUint8*)data, len);
        qint64 val = m_symbianSocketType1->StartWriteDatagram(myDescriptor, m_port);
       }
    return val;
}


void QLlcpSocketPrivate::invokeBytesWritten(qint64 bytes)
{
    emit bytesWritten(bytes);
}

void QLlcpSocketPrivate::invokeReadyRead()
{
    emit readyRead();    
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
#include "moc_qllcpsocket_symbian_p.cpp"

QTM_END_NAMESPACE
