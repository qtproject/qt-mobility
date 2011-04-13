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

#include "qllcpsocket_maemo6_p.h"

#include "manager_interface.h"
#include "maemo6/adapter_interface_p.h"
#include "maemo6/socketrequestor_p.h"

#include <QtCore/QSocketNotifier>
#include <QtCore/QAtomicInt>

#include <errno.h>
#include <signal.h>

using namespace com::nokia::nfc;

QTM_BEGIN_NAMESPACE

static QAtomicInt requestorId = 0;
static const char * const requestorBasePath = "/com/nokia/nfc/llcpclient/";

QLlcpSocketPrivate::QLlcpSocketPrivate(QLlcpSocket *q)
:   q_ptr(q),
    m_connection(QDBusConnection::connectToBus(QDBusConnection::SystemBus, QUuid::createUuid())),
    m_socketRequestor(0), m_readNotifier(0),
    m_state(QLlcpSocket::UnconnectedState), m_error(QLlcpSocket::UnknownSocketError)
{
}

QLlcpSocketPrivate::QLlcpSocketPrivate(const QDBusConnection &connection, int readFd)
:   q_ptr(0), m_connection(connection), m_socketRequestor(0),
    m_readFd(readFd),
    m_state(QLlcpSocket::ConnectedState), m_error(QLlcpSocket::UnknownSocketError)
{
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

        connect(m_socketRequestor, SIGNAL(accessFailed(QDBusObjectPath,QString,QString)),
                this, SLOT(AccessFailed(QDBusObjectPath,QString,QString)));
        connect(m_socketRequestor, SIGNAL(accessGranted(QDBusObjectPath,QString)),
                this, SLOT(AccessGranted(QDBusObjectPath,QString)));
        connect(m_socketRequestor, SIGNAL(accept(QDBusVariant,QDBusVariant,int,QVariantMap)),
                this, SLOT(Accept(QDBusVariant,QDBusVariant,int,QVariantMap)));
        connect(m_socketRequestor, SIGNAL(connect(QDBusVariant,QDBusVariant,int,QVariantMap)),
                this, SLOT(Connect(QDBusVariant,QDBusVariant,int,QVariantMap)));
        connect(m_socketRequestor, SIGNAL(socket(QDBusVariant,QDBusVariant,int,QVariantMap)),
                this, SLOT(Socket(QDBusVariant,QDBusVariant,int,QVariantMap)));
    }

    if (m_socketRequestor) {
        m_serviceUri = serviceUri;

        QString accessKind(QLatin1String("device.llcp.co.client:") + serviceUri);

        m_socketRequestor->requestAccess(m_requestorPath, accessKind);
    } else {
        setSocketError(QLlcpSocket::SocketResourceError);

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

    if (m_socketRequestor) {
        QString accessKind(QLatin1String("device.llcp.co.client:") + m_serviceUri);

        Manager manager(QLatin1String("com.nokia.nfc"), QLatin1String("/"), m_connection);
        QDBusObjectPath defaultAdapterPath = manager.DefaultAdapter();

        m_socketRequestor->cancelAccessRequest(m_requestorPath, accessKind);
    }

    m_state = QLlcpSocket::UnconnectedState;
    q->setOpenMode(QIODevice::NotOpen);
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
    return !m_datagrams.isEmpty();
}

qint64 QLlcpSocketPrivate::pendingDatagramSize() const
{
    if (m_datagrams.isEmpty())
        return -1;

    return m_datagrams.first().length();
}

qint64 QLlcpSocketPrivate::writeDatagram(const char *data, qint64 size)
{
    return writeData(data, size);
}

qint64 QLlcpSocketPrivate::writeDatagram(const QByteArray &datagram)
{
    if (uint(datagram.length()) > m_properties.value(QLatin1String("RemoteMIU"), 128).toUInt())
        return -1;

    return writeData(datagram.constData(), datagram.size());
}

qint64 QLlcpSocketPrivate::readDatagram(char *data, qint64 maxSize,
                                        QNearFieldTarget **target, quint8 *port)
{
    Q_UNUSED(target);
    Q_UNUSED(port);

    return readData(data, maxSize);
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
    if (m_datagrams.isEmpty())
        return 0;

    const QByteArray datagram = m_datagrams.takeFirst();
    qint64 size = qMin(maxlen, qint64(datagram.length()));
    memcpy(data, datagram.constData(), size);
    return size;
}

qint64 QLlcpSocketPrivate::writeData(const char *data, qint64 len)
{
    Q_Q(QLlcpSocket);

    qint64 miu = m_properties.value(QLatin1String("RemoteMIU"), 128).toUInt();
    qint64 current = 0;

    while (current < len) {
        ssize_t wrote = ::write(m_readFd, data + current, qMin(miu, len - current));
        if (wrote == -1) {
            if (current)
                emit q->bytesWritten(current);

            setSocketError(QLlcpSocket::RemoteHostClosedError);
            q->disconnectFromService();
            return -1;
        }
        current += wrote;
    }

    if (current)
        emit q->bytesWritten(current);

    return current;
}

qint64 QLlcpSocketPrivate::bytesAvailable() const
{
    qint64 available = 0;
    foreach (const QByteArray &datagram, m_datagrams)
        available += datagram.length();
    return available;
}

bool QLlcpSocketPrivate::canReadLine() const
{
    foreach (const QByteArray &datagram, m_datagrams) {
        if (datagram.contains('\n'))
            return true;
    }

    return false;
}

bool QLlcpSocketPrivate::waitForReadyRead(int msec)
{
    if (bytesAvailable())
        return true;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_readFd, &fds);

    timeval timeout;
    timeout.tv_sec = msec / 1000;
    timeout.tv_usec = (msec % 1000) * 1000;

    // timeout can not be 0 or else select will return an error.
    if (0 == msec)
        timeout.tv_usec = 1000;

    int result = -1;
    // on Linux timeout will be updated by select, but _not_ on other systems.
    QElapsedTimer timer;
    timer.start();
    while (!bytesAvailable() && (-1 == msec || timer.elapsed() < msec)) {
        result = ::select(m_readFd + 1, &fds, 0, 0, &timeout);
        if (result > 0)
            _q_readNotify();

        if (-1 == result && errno != EINTR) {
            setSocketError(QLlcpSocket::UnknownSocketError);
            break;
        }
    }

    return bytesAvailable();
}

bool QLlcpSocketPrivate::waitForBytesWritten(int msec)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_readFd, &fds);

    timeval timeout;
    timeout.tv_sec = msec / 1000;
    timeout.tv_usec = (msec % 1000) * 1000;

    // timeout can not be 0 or else select will return an error.
    if (0 == msec)
        timeout.tv_usec = 1000;

    int result = -1;
    // on Linux timeout will be updated by select, but _not_ on other systems.
    QElapsedTimer timer;
    timer.start();
    while (-1 == msec || timer.elapsed() < msec) {
        result = ::select(m_readFd + 1, 0, &fds, 0, &timeout);
        if (result > 0)
            return true;
        if (-1 == result && errno != EINTR) {
            setSocketError(QLlcpSocket::UnknownSocketError);
            return false;
        }
    }

    // timeout expired
    return false;
}

bool QLlcpSocketPrivate::waitForConnected(int msecs)
{
    if (m_state != QLlcpSocket::ConnectingState)
        return m_state == QLlcpSocket::ConnectedState;

    QElapsedTimer timer;
    timer.start();
    while (m_state == QLlcpSocket::ConnectingState && (msecs == -1 || timer.elapsed() < msecs)) {
        if (!m_socketRequestor->waitForDBusSignal(qMax(msecs - timer.elapsed(), qint64(0)))) {
            setSocketError(QLlcpSocket::UnknownSocketError);
            break;
        }
    }

    // Possibly not needed.
    QCoreApplication::sendPostedEvents(this, QEvent::MetaCall);

    return m_state == QLlcpSocket::ConnectedState;
}

bool QLlcpSocketPrivate::waitForDisconnected(int msec)
{
    if (m_state == QLlcpSocket::UnconnectedState)
        return true;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_readFd, &fds);

    timeval timeout;
    timeout.tv_sec = msec / 1000;
    timeout.tv_usec = (msec % 1000) * 1000;

    // timeout can not be 0 or else select will return an error.
    if (0 == msec)
        timeout.tv_usec = 1000;

    int result = -1;
    // on Linux timeout will be updated by select, but _not_ on other systems.
    QElapsedTimer timer;
    timer.start();
    while (m_state != QLlcpSocket::UnconnectedState && (-1 == msec || timer.elapsed() < msec)) {
        result = ::select(m_readFd + 1, &fds, 0, 0, &timeout);
        if (result > 0)
            _q_readNotify();

        if (-1 == result && errno != EINTR) {
            setSocketError(QLlcpSocket::UnknownSocketError);
            break;
        }
    }

    return m_state == QLlcpSocket::UnconnectedState;
}

void QLlcpSocketPrivate::AccessFailed(const QDBusObjectPath &targetPath, const QString &kind,
                                      const QString &error)
{
    Q_UNUSED(targetPath);
    Q_UNUSED(kind);
    Q_UNUSED(error);

    Q_Q(QLlcpSocket);

    setSocketError(QLlcpSocket::SocketAccessError);

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
                                int readFd, const QVariantMap &properties)
{
    Q_UNUSED(lsap);
    Q_UNUSED(rsap);
    Q_UNUSED(readFd);
    Q_UNUSED(properties);
}

void QLlcpSocketPrivate::Connect(const QDBusVariant &lsap, const QDBusVariant &rsap,
                                 int readFd, const QVariantMap &properties)
{
    Q_UNUSED(lsap);
    Q_UNUSED(rsap);

    m_readFd = readFd;

    m_readNotifier = new QSocketNotifier(m_readFd, QSocketNotifier::Read, this);
    connect(m_readNotifier, SIGNAL(activated(int)), this, SLOT(_q_readNotify()));

    m_properties = properties;

    Q_Q(QLlcpSocket);

    q->setOpenMode(QIODevice::ReadWrite);

    m_state = QLlcpSocket::ConnectedState;
    emit q->stateChanged(m_state);
    emit q->connected();
}

void QLlcpSocketPrivate::Socket(const QDBusVariant &lsap, const QDBusVariant &rsap,
                                int readFd, const QVariantMap &properties)
{
    Q_UNUSED(lsap);
    Q_UNUSED(rsap);
    Q_UNUSED(readFd);
    Q_UNUSED(properties);
}

void QLlcpSocketPrivate::_q_readNotify()
{
    Q_Q(QLlcpSocket);

    QByteArray datagram;
    datagram.resize(m_properties.value(QLatin1String("LocalMIU"), 128).toUInt());

    int readFromDevice = ::read(m_readFd, datagram.data(), datagram.size());
    if (readFromDevice <= 0) {
        m_readNotifier->setEnabled(false);

        setSocketError(QLlcpSocket::RemoteHostClosedError);

        q->disconnectFromService();
        q->setOpenMode(QIODevice::NotOpen);
    } else {
        m_datagrams.append(datagram.left(readFromDevice));
        emit q->readyRead();
    }
}

void QLlcpSocketPrivate::setSocketError(QLlcpSocket::SocketError socketError)
{
    Q_Q(QLlcpSocket);

    QLatin1String c("QLlcpSocket");

    m_error = socketError;
    switch (socketError) {
    case QLlcpSocket::UnknownSocketError:
        q->setErrorString(QLlcpSocket::tr("%1: Unknown error %2").arg(c).arg(errno));
        break;
    case QLlcpSocket::RemoteHostClosedError:
        q->setErrorString(QLlcpSocket::tr("%1: Remote closed").arg(c));
        break;
    case QLlcpSocket::SocketAccessError:
        q->setErrorString(QLlcpSocket::tr("%1: Socket access error"));
        break;
    case QLlcpSocket::SocketResourceError:
        q->setErrorString(QLlcpSocket::tr("%1: Socket resource error"));
        break;
    }

    emit q->error(m_error);
}

#include "moc_qllcpsocket_maemo6_p.cpp"

QTM_END_NAMESPACE
