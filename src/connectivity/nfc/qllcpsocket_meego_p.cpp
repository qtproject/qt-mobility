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
#include "accessrequestor_adaptor.h"
#include "llcprequestor_adaptor.h"

using namespace com::nokia::nfc;

QTM_BEGIN_NAMESPACE

static int requestorId = 0;
static const char * const requestorBasePath = "/com/nokia/nfc/llcprequestor/";

QLlcpSocketPrivate::QLlcpSocketPrivate(QLlcpSocket *q)
:   q_ptr(q),
    m_connection(QDBusConnection::connectToBus(QDBusConnection::SystemBus, QUuid::createUuid()))
{
}

void QLlcpSocketPrivate::connectToService(QNearFieldTarget *target, const QString &serviceUri)
{
    qDebug() << Q_FUNC_INFO << target << serviceUri;

    const QString requestorPath = QLatin1String(requestorBasePath) + QString::number(++requestorId);

    // change state to waiting for touch

    m_accessRequestor = new AccessRequestorAdaptor(this);
    m_llcpRequestor = new LLCPRequestorAdaptor(this);

    if (!QDBusConnection::sessionBus().registerObject(requestorPath, this)) {
        qDebug() << "failed to register dbus service";
        delete m_llcpRequestor;
        m_llcpRequestor = 0;
        delete m_accessRequestor;
        m_accessRequestor = 0;

        // emit error

        // change state to Unconnected
        return;
    }

    qDebug() << Q_FUNC_INFO << "registered object at" << requestorPath;
    QString accessKind(QLatin1String("device.llcp.co.client:") + serviceUri);

    Manager manager(QLatin1String("com.nokia.nfc"), QLatin1String("/"), m_connection);
    QDBusObjectPath defaultAdapterPath = manager.DefaultAdapter();

    Adapter adapter(QLatin1String("com.nokia.nfc"), defaultAdapterPath.path(), m_connection);

    qDebug() << Q_FUNC_INFO << "requesting access";
    adapter.RequestAccess(QDBusObjectPath(requestorPath), accessKind);
}

void QLlcpSocketPrivate::disconnectFromService()
{
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

QLlcpSocket::Error QLlcpSocketPrivate::error() const
{
    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return QLlcpSocket::UnknownSocketError;
}

QLlcpSocket::State QLlcpSocketPrivate::state() const
{
    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return QLlcpSocket::UnconnectedState;
}

qint64 QLlcpSocketPrivate::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);

    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return -1;
}

qint64 QLlcpSocketPrivate::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return -1;
}

bool QLlcpSocketPrivate::bytesAvailable() const
{
    qDebug() << Q_FUNC_INFO << "is unimplemented";

    return 0;
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
    qDebug() << Q_FUNC_INFO << "is unimplemented";
}

void QLlcpSocketPrivate::AccessGranted(const QDBusObjectPath &targetPath,
                                       const QStringList &accessKind)
{
    qDebug() << Q_FUNC_INFO << "is unimplemented";
}

void QLlcpSocketPrivate::Accept(const QDBusObjectPath &targetPath, const QDBusVariant &lsap,
                                const QDBusVariant &rsap, int fd)
{
    qDebug() << Q_FUNC_INFO << "is unimplemented";
}

void QLlcpSocketPrivate::Connect(const QDBusObjectPath &targetPath, const QDBusVariant &lsap,
                                 const QDBusVariant &rsap, int fd)
{
    qDebug() << Q_FUNC_INFO << "is unimplemented";
}

void QLlcpSocketPrivate::Socket(const QDBusObjectPath &targetPath, const QDBusVariant &lsap,
                                const QDBusVariant &rsap, int fd)
{
    qDebug() << Q_FUNC_INFO << "is unimplemented";
}

#include "moc_qllcpsocket_meego_p.cpp"

QTM_END_NAMESPACE
