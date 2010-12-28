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

#include "symbian/debug.h"
QTM_BEGIN_NAMESPACE

QLlcpServerPrivate::QLlcpServerPrivate(QLlcpServer *q)
    :q_ptr(q)
{
    BEGIN
    QT_TRAP_THROWING(m_symbianbackend = CLlcpServer::NewL(*this));
    END
}
QLlcpServerPrivate::~QLlcpServerPrivate()
{
    BEGIN
    close();
    delete m_symbianbackend;
    END
}

bool QLlcpServerPrivate::listen(const QString &serviceUri)
{
    BEGIN
    TPtrC wide(static_cast<const TUint16*>(serviceUri.utf16()),serviceUri.length());
    RBuf8 narrow;

    TInt val = narrow.CreateMax(wide.Length());
    if( val != KErrNone)
    {
        END
        return false;
    }
    narrow.Copy(wide);

    bool ret =  m_symbianbackend->Listen(narrow);

    narrow.Close();
    END
    return ret;
}

bool QLlcpServerPrivate::isListening() const
{
    BEGIN
    END
    return m_symbianbackend->isListening();
}

/*!
    Stops listening for incoming connections.
*/
void QLlcpServerPrivate::close()
{
    BEGIN
    m_symbianbackend->StopListening();
    qDeleteAll(m_pendingConnections);
    m_pendingConnections.clear();
    END
}

QString QLlcpServerPrivate::serviceUri() const
{
    BEGIN
    const TDesC8& theDescriptor= m_symbianbackend->serviceUri();

    RBuf wide;
    TInt val = wide.CreateMax(theDescriptor.Length());
    if( val != KErrNone)
    {
        END
        return false;
    }
    wide.Copy(theDescriptor);
    QString ret = QString::fromUtf16(wide.Ptr(),wide.Length());
    LOG("QLlcpServerPrivate::serviceUri() ret="<<ret);
    wide.Close();
    END
    return ret;

}

quint8 QLlcpServerPrivate::serverPort() const
{
    BEGIN
    END
    return 0;
}

bool QLlcpServerPrivate::hasPendingConnections() const
{
    BEGIN
    END
    return m_symbianbackend->hasPendingConnections();
}

void QLlcpServerPrivate::invokeNewConnection()
{
    BEGIN
    Q_Q(QLlcpServer);
    emit q->newConnection();
    END
}

QLlcpSocket *QLlcpServerPrivate::nextPendingConnection()
{
    BEGIN
    QLlcpSocket* qSocket  = NULL;
    CLlcpSocketType2* socket_symbian = m_symbianbackend->nextPendingConnection();
    if (socket_symbian)
    {
        QLlcpSocketPrivate *qSocket_p = new QLlcpSocketPrivate(socket_symbian);
        qSocket = new QLlcpSocket(qSocket_p,NULL);
        qSocket_p->attachCallbackHandler(qSocket);
        socket_symbian->AttachCallbackHandler(qSocket_p);
        m_pendingConnections.append(qSocket);
    }
    END
    return qSocket;
}

QLlcpServer::Error QLlcpServerPrivate::serverError() const
{
    BEGIN
    END
    return QLlcpServer::UnknownSocketError;
}

QTM_END_NAMESPACE
//EOF

