/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qllcpsocket_meego_p.h"

#include "manager_interface.h"
#include "meego/adapter_interface_p.h"
#include "meego/socketrequestor_p.h"

#include <QtCore/QSocketNotifier>
#include <QtCore/QAtomicInt>

#include <errno.h>
#include <signal.h>

using namespace com::nokia::nfc;

QTM_BEGIN_NAMESPACE

static void qt_ignore_sigpipe()
{
    // Set to ignore SIGPIPE once only.
    static QBasicAtomicInt atom = Q_BASIC_ATOMIC_INITIALIZER(0);
    if (atom.testAndSetRelaxed(0, 1)) {
        struct sigaction noaction;
        memset(&noaction, 0, sizeof(noaction));
        noaction.sa_handler = SIG_IGN;
        ::sigaction(SIGPIPE, &noaction, 0);
    }
}

static QAtomicInt requestorId = 0;
static const char * const requestorBasePath = "/com/nokia/nfc/llcpclient/";

QLlcpSocketPrivate::QLlcpSocketPrivate(QLlcpSocket *q)
:   q_ptr(q),
    m_connection(QDBusConnection::connectToBus(QDBusConnection::SystemBus, QUuid::createUuid())),
    m_socketRequestor(0), m_readNotifier(0),
    m_state(QLlcpSocket::UnconnectedState), m_error(QLlcpSocket::UnknownSocketError)
{
    qt_ignore_sigpipe();
}

QLlcpSocketPrivate::QLlcpSocketPrivate(const QDBusConnection &connection, int readFd, int writeFd)
:   q_ptr(0), m_connection(connection), m_socketRequestor(0),
    m_readFd(readFd), m_writeFd(writeFd),
    m_state(QLlcpSocket::ConnectedState), m_error(QLlcpSocket::UnknownSocketError)
{
    qt_ignore_sigpipe();

    m_readNotifier = new QSocketNotifier(m_readFd, QSocketNotifier::Read, this);
    connect(m_readNotifier, SIGNAL(activated(int)), this, SLOT(_q_readNotify()));
}

QLlcpSocketPrivate::~QLlcpSocketPrivate()
{
    delete m_readNotifier;
}

void QLlcpSocketPrivate::connectToService(QNearFieldTarget *target, const QString &serviceUri)
{
    Q_UNUSED(target);

    Q_Q(QLlcpSocket);

    m_state = QLlcpSocket::ConnectingState;
    emit q->stateChanged(m_state);

    if (m_requestorPath.isEmpty()) {
        m_requestorPath = QLatin1String(requestorBasePath) +
                          QString::number(requestorId.fetchAndAddOrdered(1));
    }

    Manager manager(QLatin1String("com.nokia.nfc"), QLatin1String("/"), m_connection);
    QDBusObjectPath defaultAdapterPath = manager.DefaultAdapter();

    if (!m_socketRequestor) {
        m_socketRequestor = new SocketRequestor(defaultAdapterPath.path(), this);

        connect(m_socketRequestor, SIGNAL(accessFailed(QDBusObjectPath,QString)),
                this, SLOT(AccessFailed(QDBusObjectPath,QString)));
        connect(m_socketRequestor, SIGNAL(accessGranted(QDBusObjectPath,QString)),
                this, SLOT(AccessGranted(QDBusObjectPath,QString)));
        connect(m_socketRequestor, SIGNAL(accept(QDBusVariant,QDBusVariant,int,int)),
                this, SLOT(Accept(QDBusVariant,QDBusVariant,int,int)));
        connect(m_socketRequestor, SIGNAL(connect(QDBusVariant,QDBusVariant,int,int)),
                this, SLOT(Connect(QDBusVariant,QDBusVariant,int,int)));
        connect(m_socketRequestor, SIGNAL(socket(QDBusVariant,QDBusVariant,int,int)),
                this, SLOT(Socket(QDBusVariant,QDBusVariant,int,int)));
    }

    if (m_socketRequestor) {
        m_serviceUri = serviceUri;

        QString accessKind(QLatin1String("device.llcp.co.client:") + serviceUri);

        m_socketRequestor->requestAccess(m_requestorPath, accessKind);
    } else {
        m_error = QLlcpSocket::UnknownSocketError;
        emit q->error();

        m_state = QLlcpSocket::UnconnectedState;
        emit q->stateChanged(m_state);
    }
}

void QLlcpSocketPrivate::disconnectFromService()
{
    Q_Q(QLlcpSocket);

    m_state = QLlcpSocket::ClosingState;
    emit q->stateChanged(m_state);

    delete m_readNotifier;
    m_readNotifier = 0;
    ::close(m_readFd);
    m_readFd = -1;
    ::close(m_writeFd);
    m_writeFd = -1;

    if (m_socketRequestor) {
        QString accessKind(QLatin1String("device.llcp.co.client:") + m_serviceUri);

        Manager manager(QLatin1String("com.nokia.nfc"), QLatin1String("/"), m_connection);
        QDBusObjectPath defaultAdapterPath = manager.DefaultAdapter();

        m_socketRequestor->cancelAccessRequest(m_requestorPath, accessKind);
    }

    m_state = QLlcpSocket::UnconnectedState;
    emit q->stateChanged(m_state);
    emit q->disconnected();
}

bool QLlcpSocketPrivate::bind(quint8 port)
{
    Q_UNUSED(port);

    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return false;
}

bool QLlcpSocketPrivate::hasPendingDatagrams() const
{
    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return false;
}

qint64 QLlcpSocketPrivate::pendingDatagramSize() const
{
    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return -1;
}

qint64 QLlcpSocketPrivate::writeDatagram(const char *data, qint64 size)
{
    Q_UNUSED(data);
    Q_UNUSED(size);

    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return -1;
}

qint64 QLlcpSocketPrivate::writeDatagram(const QByteArray &datagram)
{
    Q_UNUSED(datagram);

    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return -1;
}

qint64 QLlcpSocketPrivate::readDatagram(char *data, qint64 maxSize,
                                        QNearFieldTarget **target, quint8 *port)
{
    Q_UNUSED(data);
    Q_UNUSED(maxSize);
    Q_UNUSED(target);
    Q_UNUSED(port);

    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return -1;
}

qint64 QLlcpSocketPrivate::writeDatagram(const char *data, qint64 size,
                                         QNearFieldTarget *target, quint8 port)
{
    Q_UNUSED(data);
    Q_UNUSED(size);
    Q_UNUSED(target);
    Q_UNUSED(port);

    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return -1;
}

qint64 QLlcpSocketPrivate::writeDatagram(const QByteArray &datagram,
                                         QNearFieldTarget *target, quint8 port)
{
    Q_UNUSED(datagram);
    Q_UNUSED(target);
    Q_UNUSED(port);

    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return -1;
}

QLlcpSocket::SocketError QLlcpSocketPrivate::error() const
{
    return m_error;
}

QLlcpSocket::SocketState QLlcpSocketPrivate::state() const
{
    return m_state;
}

qint64 QLlcpSocketPrivate::readData(char *data, qint64 maxlen)
{
    if (!buffer.isEmpty()) {
        int i = buffer.read(data, maxlen);
        return i;
    }

    return 0;
}

qint64 QLlcpSocketPrivate::writeData(const char *data, qint64 len)
{
    Q_Q(QLlcpSocket);

    qDebug() << Q_FUNC_INFO;

    ssize_t wrote = ::write(m_writeFd, data, len);
    if (wrote == -1) {
        m_error = QLlcpSocket::RemoteHostClosedError;
        emit q->error(m_error);
        q->disconnectFromService();
        return -1;
    } else {
        emit q->bytesWritten(wrote);

        return wrote;
    }
}

qint64 QLlcpSocketPrivate::bytesAvailable() const
{
    return buffer.size();
}

bool QLlcpSocketPrivate::canReadLine() const
{
    return buffer.canReadLine();
}

bool QLlcpSocketPrivate::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);

    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return false;
}

bool QLlcpSocketPrivate::waitForBytesWritten(int msecs)
{
    Q_UNUSED(msecs);

    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return false;
}

bool QLlcpSocketPrivate::waitForConnected(int msecs)
{
    Q_UNUSED(msecs);

    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return false;
}

bool QLlcpSocketPrivate::waitForDisconnected(int msecs)
{
    Q_UNUSED(msecs);

    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return false;
}

void QLlcpSocketPrivate::AccessFailed(const QDBusObjectPath &targetPath, const QString &error)
{
    Q_UNUSED(targetPath);
    Q_UNUSED(error);

    Q_Q(QLlcpSocket);

    m_error = QLlcpSocket::UnknownSocketError;
    emit q->error();

    m_state = QLlcpSocket::UnconnectedState;
    emit q->stateChanged(m_state);
}

void QLlcpSocketPrivate::AccessGranted(const QDBusObjectPath &targetPath,
                                       const QString &accessKind)
{
    Q_UNUSED(targetPath);
    Q_UNUSED(accessKind);
}

void QLlcpSocketPrivate::Accept(const QDBusVariant &lsap, const QDBusVariant &rsap,
                                int readFd, int writeFd)
{
    Q_UNUSED(lsap);
    Q_UNUSED(rsap);
    Q_UNUSED(readFd);
    Q_UNUSED(writeFd);
}

void QLlcpSocketPrivate::Connect(const QDBusVariant &lsap, const QDBusVariant &rsap,
                                 int readFd, int writeFd)
{
    Q_UNUSED(lsap);
    Q_UNUSED(rsap);

    m_readFd = readFd;
    m_writeFd = writeFd;

    m_readNotifier = new QSocketNotifier(m_readFd, QSocketNotifier::Read, this);
    connect(m_readNotifier, SIGNAL(activated(int)), this, SLOT(_q_readNotify()));

    Q_Q(QLlcpSocket);

    q->setOpenMode(QIODevice::ReadWrite);

    m_state = QLlcpSocket::ConnectedState;
    emit q->stateChanged(m_state);
    emit q->connected();
}

void QLlcpSocketPrivate::Socket(const QDBusVariant &lsap, const QDBusVariant &rsap,
                                int readFd, int writeFd)
{
    Q_UNUSED(lsap);
    Q_UNUSED(rsap);
    Q_UNUSED(readFd);
    Q_UNUSED(writeFd);
}

void QLlcpSocketPrivate::_q_readNotify()
{
    Q_Q(QLlcpSocket);

    char *writePointer = buffer.reserve(QPRIVATELINEARBUFFER_BUFFERSIZE);
    int readFromDevice =
        ::read(m_readFd, writePointer, QPRIVATELINEARBUFFER_BUFFERSIZE);

    if (readFromDevice <= 0) {
        m_readNotifier->setEnabled(false);

        m_error = QLlcpSocket::RemoteHostClosedError;
        emit q->error(m_error);

        q->disconnectFromService();
    } else if (readFromDevice > 0) {
        buffer.chop(QPRIVATELINEARBUFFER_BUFFERSIZE - (readFromDevice < 0 ? 0 : readFromDevice));

        emit q->readyRead();
    }
}

#include "moc_qllcpsocket_meego_p.cpp"

QTM_END_NAMESPACE
