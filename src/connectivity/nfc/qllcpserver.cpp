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

#include "qllcpserver.h"

#if defined(QT_SIMULATOR)
#include "qllcpserver_simulator_p.h"
#elif defined(Q_OS_SYMBIAN)
#include "qllcpserver_symbian_p.h"
#else
#include "qllcpserver_p.h"
#endif

QTM_BEGIN_NAMESPACE

/*!
    \class QLlcpServer
    \brief The QLlcpServer class provides an NFC LLCP socket based server.

    \ingroup connectivity-nfc
    \inmodule QtConnectivity

    This class makes it possible to accept incoming LLCP socket connections.

    Call listen() to have the server start listening for incoming connections on a specified port.
    The newConnection() signal is then emitted each time a client connects to the server.

    Call nextPendingConnection() to accept the pending connection as a connected QLlcpSocket. The
    function returns a pointer to a QLlcpSocket that can be used for communicating with the client.

    If an error occurs, serverError() returns the type of error, and errorString() can be called to
    get a human readable description of what happened.

    When listening for connections, the port which the server is listening on is available through
    serverPort().

    Calling close() makes QLlcpServer stop listening for incoming connections.
*/

/*!
    \enum QLlcpServer::Error

    This enum describes the errors that can occur. The most recent error can be retrieved through a
    call to serverError().

    \value UnknownSocketError   An unidentified error has occurred.
*/

/*!
    \fn QLlcpServer::newConnection()

    This signal is emitted every time a new connection is available.

    \sa hasPendingConnections(), nextPendingConnection()
*/

/*!
    Constructs a new NFC LLCP server with \a parent.
*/
QLlcpServer::QLlcpServer(QObject *parent)
:   QObject(parent), d_ptr(new QLlcpServerPrivate)
{
}

/*!
    Destroys the NFC LLCP server.
*/
QLlcpServer::~QLlcpServer()
{
    delete d_ptr;
}

/*!
    Tells the server to listen for incoming connections on \a serviceUri. If the server is
    currently listening then it will return false. Returns true on success; otherwise returns
    false.

    serviceUri() will return the \a serviceUri that is passed into listen.

    serverPort() will return the port that is assigned to the server.

    \sa serverPort(), isListening(), close()
*/
bool QLlcpServer::listen(const QString &serviceUri)
{
    Q_D(QLlcpServer);

    return d->listen(serviceUri);
}

/*!
    Returns true if the server is listening for incoming connections; otherwise returns false.
*/
bool QLlcpServer::isListening() const
{
    Q_D(const QLlcpServer);

    return d->isListening();
}

/*!
    Stops listening for incoming connections.
*/
void QLlcpServer::close()
{
    Q_D(QLlcpServer);

    d->close();
}

/*!
    Returns the LLCP service URI that the server is listening on.
*/
QString QLlcpServer::serviceUri() const
{
    Q_D(const QLlcpServer);

    return d->serviceUri();
}

/*!
    Returns the LLCP port associated with the service URI that the server is listening on.

    \note This call is not supported on all platforms and will return 0 on these platforms.
*/
quint8 QLlcpServer::serverPort() const
{
    Q_D(const QLlcpServer);

    return d->serverPort();
}

/*!
    Returns true if the server has a pending connection; otherwise returns false.

    \sa nextPendingConnection()
*/
bool QLlcpServer::hasPendingConnections() const
{
    Q_D(const QLlcpServer);

    return d->hasPendingConnections();
}

/*!
    Returns the next pending connection as a connected QLlcpSocket object.

    The socket is created as a child of the server, which means that it is automatically deleted
    when the QLlcpServer object is destroyed. It is still a good idea to delete the object
    explicitly when you are done with it, to avoid wasting memory.

    0 is returned if this function is called when there are no pending connections.

    \sa hasPendingConnections(), newConnection()
*/
QLlcpSocket *QLlcpServer::nextPendingConnection()
{
    Q_D(QLlcpServer);

    return d->nextPendingConnection();
}

/*!
    Returns the last error that occurred.
*/
QLlcpServer::Error QLlcpServer::serverError() const
{
    Q_D(const QLlcpServer);

    return d->serverError();
}

#include "moc_qllcpserver.cpp"

QTM_END_NAMESPACE
