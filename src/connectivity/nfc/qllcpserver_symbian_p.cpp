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

#include "qllcpserver_symbian_p.h"
#include "symbian/llcpserver_symbian.h"
#include "symbian/llcpsockettype2_symbian.h"
#include "symbian/nearfieldutility_symbian.h"

QTM_BEGIN_NAMESPACE

QLlcpServerPrivate::QLlcpServerPrivate()
    :m_state(QLlcpServerPrivate::UnconnectedState)
{
    QT_TRAP_THROWING(m_symbianbackend = CLlcpServer::NewL(*this));
}


QLlcpServerPrivate::~QLlcpServerPrivate()
{
    delete m_symbianbackend;
}

bool QLlcpServerPrivate::listen(const QString &serviceUri)
{
    m_state = QLlcpServerPrivate::ListeningState;
    TPtrC8 serviceName = QNFCNdefUtility::FromQStringToTptrC8(serviceUri);
    return m_symbianbackend->Listen(serviceName);
}

bool QLlcpServerPrivate::isListening() const
{
    return m_symbianbackend->isListening();
}

/*!
    Stops listening for incoming connections.
*/
void QLlcpServerPrivate::close()
{
    m_state = QLlcpServerPrivate::UnconnectedState;
    m_symbianbackend->StopListening();
}

QString QLlcpServerPrivate::serviceUri() const
{
    const TDesC8& theDescriptor= m_symbianbackend->serviceUri();
    QString serviceName = QNFCNdefUtility::FromDesC8ToQString(theDescriptor);
    return serviceName;
}

quint8 QLlcpServerPrivate::serverPort() const
{
    return 0;
}

bool QLlcpServerPrivate::hasPendingConnections() const
{
    return m_symbianbackend->hasPendingConnections();
}

void QLlcpServerPrivate::invokeNewConnection()
{
    emit newConnection();
}

QLlcpSocket* QLlcpServerPrivate::qllcpsocket(CLlcpSocketType2* socket_symbian)
{
    QLlcpSocketPrivate *qSocket_p = new QLlcpSocketPrivate(socket_symbian);
    //TODO
    //active the following line when Aaron fix it.
    QLlcpSocket* qSocket = NULL;
    //QLlcpSocket* qSocket = new QLlcpSocket(qSocket_p);

    return qSocket;
}

QLlcpSocket *QLlcpServerPrivate::nextPendingConnection()
{
    CLlcpSocketType2* socket_s = m_symbianbackend->nextPendingConnection();
    if (socket_s)
    {
       return qllcpsocket(socket_s);
    }
    return 0;
}

QLlcpServer::Error QLlcpServerPrivate::serverError() const
{
    return QLlcpServer::UnknownSocketError;
}

#include "moc_qllcpserver_symbian_p.cpp"

QTM_END_NAMESPACE

