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

#include "socketrequestor_p.h"

#include <QtCore/QMutex>
#include <QtCore/QHash>
#include <QtDBus/QDBusObjectPath>
#include <QtCore/QSocketNotifier>

#include <QtCore/QDebug>

#include <dbus/dbus.h>

QTM_BEGIN_NAMESPACE

struct WatchNotifier
{
    DBusWatch *watch;
    QSocketNotifier *readNotifier;
    QSocketNotifier *writeNotifier;
};

class SocketRequestorPrivate : public QObject
{
    Q_OBJECT

public:
    SocketRequestorPrivate();
    ~SocketRequestorPrivate();

    DBusHandlerResult messageFilter(DBusConnection *connection, DBusMessage *message);
    void addWatch(DBusWatch *watch);

    void registerObject(const QString &path, SocketRequestor *object);
    void unregisterObject(const QString &path);

    Q_INVOKABLE void sendRequestAccess(const QString &adaptor, const QString &path,
                                       const QString &kind);
    Q_INVOKABLE void sendCancelAccessRequest(const QString &adaptor, const QString &path,
                                             const QString &kind);

private:
    bool parseAccessFailed(DBusMessage *message, SocketRequestor *socketRequestor);
    bool parseAccessGranted(DBusMessage *message, SocketRequestor *socketRequestor);
    bool parseAccept(DBusMessage *message, SocketRequestor *socketRequestor);
    bool parseConnect(DBusMessage *message, SocketRequestor *socketRequestor);
    bool parseSocket(DBusMessage *message, SocketRequestor *socketRequestor);

private slots:
    void socketRead();
    void socketWrite();

private:
    QMutex m_mutex;
    DBusConnection *m_dbusConnection;
    QHash<QString, SocketRequestor *> m_dbusObjects;
    QList<WatchNotifier> m_watchNotifiers;
};

Q_GLOBAL_STATIC(SocketRequestorPrivate, socketRequestorPrivate)

static DBusHandlerResult dbusFilter(DBusConnection *connection, DBusMessage *message,
                                    void *userData)
{
    SocketRequestorPrivate *s = static_cast<SocketRequestorPrivate *>(userData);
    return s->messageFilter(connection, message);
}

static dbus_bool_t dbusWatch(DBusWatch *watch, void *data)
{
    SocketRequestorPrivate *s = static_cast<SocketRequestorPrivate *>(data);
    s->addWatch(watch);

    return true;
}

SocketRequestorPrivate::SocketRequestorPrivate()
{
    DBusError error;
    dbus_error_init(&error);
    m_dbusConnection = dbus_bus_get_private(DBUS_BUS_SYSTEM, &error);
    dbus_connection_add_filter(m_dbusConnection, &dbusFilter, this, 0);
    dbus_connection_set_watch_functions(m_dbusConnection, dbusWatch, 0, 0, this, 0);
    dbus_error_free(&error);
}

SocketRequestorPrivate::~SocketRequestorPrivate()
{
    dbus_connection_close(m_dbusConnection);
    dbus_connection_unref(m_dbusConnection);
}

DBusHandlerResult SocketRequestorPrivate::messageFilter(DBusConnection *connection,
                                                        DBusMessage *message)
{
    QMutexLocker locker(&m_mutex);

    foreach (const WatchNotifier &watchNotifier, m_watchNotifiers)
        watchNotifier.writeNotifier->setEnabled(true);

    if (connection != m_dbusConnection)
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    QString path = QString::fromUtf8(dbus_message_get_path(message));

    SocketRequestor *socketRequestor = m_dbusObjects.value(path);

    if (!socketRequestor)
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    bool handled;

    if (dbus_message_is_method_call(message, "com.nokia.nfc.AccessRequestor", "AccessFailed"))
        handled = parseAccessFailed(message, socketRequestor);
    else if (dbus_message_is_method_call(message, "com.nokia.nfc.AccessRequestor", "AccessGranted"))
        handled = parseAccessGranted(message, socketRequestor);
    else if (dbus_message_is_method_call(message, "com.nokia.nfc.LLCPRequestor", "Accept"))
        handled = parseAccept(message, socketRequestor);
    else if (dbus_message_is_method_call(message, "com.nokia.nfc.LLCPRequestor", "Connect"))
        handled = parseConnect(message, socketRequestor);
    else if (dbus_message_is_method_call(message, "com.nokia.nfc.LLCPRequestor", "Socket"))
        handled = parseSocket(message, socketRequestor);
    else
        handled = false;

    if (!handled)
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    DBusMessage *reply = dbus_message_new_method_return(message);
    quint32 serial;
    dbus_connection_send(connection, reply, &serial);

    return DBUS_HANDLER_RESULT_HANDLED;
}

bool SocketRequestorPrivate::parseAccessFailed(DBusMessage *message, SocketRequestor *socketRequestor)
{
    Q_UNUSED(message);

    // m_mutex is locked in messageFilter()

    QMetaObject::invokeMethod(socketRequestor, "accessFailed",
                              Q_ARG(QDBusObjectPath, QDBusObjectPath()),
                              Q_ARG(QString, QString()));
    return true;
}

bool SocketRequestorPrivate::parseAccessGranted(DBusMessage *message, SocketRequestor *socketRequestor)
{
    Q_UNUSED(message);

    // m_mutex is locked in messageFilter()

    QMetaObject::invokeMethod(socketRequestor, "accessGranted",
                              Q_ARG(QDBusObjectPath, QDBusObjectPath()),
                              Q_ARG(QString, QString()));
    return true;
}

bool SocketRequestorPrivate::parseAccept(DBusMessage *message, SocketRequestor *socketRequestor)
{
    // m_mutex is locked in messageFilter()

    DBusMessageIter args;

    if (!dbus_message_iter_init(message, &args))
        return false;

    if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_VARIANT)
        return false;

    if (!dbus_message_iter_next(&args))
        return false;

    if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_VARIANT)
        return false;

    if (!dbus_message_iter_next(&args))
        return false;

    if (dbus_message_iter_get_arg_type(&args) != int('h'))
        return false;

    int readFd;

    dbus_message_iter_get_basic(&args, &readFd);

    if (!dbus_message_iter_next(&args))
        return false;

    if (dbus_message_iter_get_arg_type(&args) != int('h'))
        return false;

    int writeFd;

    dbus_message_iter_get_basic(&args, &writeFd);

    QMetaObject::invokeMethod(socketRequestor, "accept", Q_ARG(QDBusVariant, QDBusVariant()),
                              Q_ARG(QDBusVariant, QDBusVariant()),
                              Q_ARG(int, readFd), Q_ARG(int, writeFd));

    return true;
}

bool SocketRequestorPrivate::parseConnect(DBusMessage *message, SocketRequestor *socketRequestor)
{
    // m_mutex is locked in messageFilter()

    DBusMessageIter args;

    if (!dbus_message_iter_init(message, &args))
        return false;

    if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_VARIANT)
        return false;

    if (!dbus_message_iter_next(&args))
        return false;

    if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_VARIANT)
        return false;

    if (!dbus_message_iter_next(&args))
        return false;

    if (dbus_message_iter_get_arg_type(&args) != int('h'))
        return false;

    int readFd;

    dbus_message_iter_get_basic(&args, &readFd);

    if (!dbus_message_iter_next(&args))
        return false;

    if (dbus_message_iter_get_arg_type(&args) != int('h'))
        return false;

    int writeFd;

    dbus_message_iter_get_basic(&args, &writeFd);

    QMetaObject::invokeMethod(socketRequestor, "connect", Q_ARG(QDBusVariant, QDBusVariant()),
                              Q_ARG(QDBusVariant, QDBusVariant()),
                              Q_ARG(int, readFd), Q_ARG(int, writeFd));

    return true;
}

bool SocketRequestorPrivate::parseSocket(DBusMessage *message, SocketRequestor *socketRequestor)
{
    // m_mutex is locked in messageFilter()

    DBusMessageIter args;

    if (!dbus_message_iter_init(message, &args))
        return false;

    if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_VARIANT)
        return false;

    if (!dbus_message_iter_next(&args))
        return false;

    if (dbus_message_iter_get_arg_type(&args) != DBUS_TYPE_VARIANT)
        return false;

    if (!dbus_message_iter_next(&args))
        return false;

    if (dbus_message_iter_get_arg_type(&args) != int('h'))
        return false;

    int readFd;

    dbus_message_iter_get_basic(&args, &readFd);

    if (!dbus_message_iter_next(&args))
        return false;

    if (dbus_message_iter_get_arg_type(&args) != int('h'))
        return false;

    int writeFd;

    dbus_message_iter_get_basic(&args, &writeFd);

    QMetaObject::invokeMethod(socketRequestor, "socket", Q_ARG(QDBusVariant, QDBusVariant()),
                              Q_ARG(QDBusVariant, QDBusVariant()),
                              Q_ARG(int, readFd), Q_ARG(int, writeFd));

    return true;
}

void SocketRequestorPrivate::addWatch(DBusWatch *watch)
{
    QMutexLocker locker(&m_mutex);

    int fd = dbus_watch_get_unix_fd(watch);

    WatchNotifier watchNotifier;
    watchNotifier.watch = watch;

    watchNotifier.readNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(watchNotifier.readNotifier, SIGNAL(activated(int)), this, SLOT(socketRead()));
    watchNotifier.writeNotifier = new QSocketNotifier(fd, QSocketNotifier::Write, this);
    connect(watchNotifier.writeNotifier, SIGNAL(activated(int)), this, SLOT(socketWrite()));

    m_watchNotifiers.append(watchNotifier);
}

void SocketRequestorPrivate::socketRead()
{
    QMutexLocker locker(&m_mutex);

    QList<DBusWatch *> pendingWatches;

    foreach (const WatchNotifier &watchNotifier, m_watchNotifiers) {
        if (watchNotifier.readNotifier != sender())
            continue;

        pendingWatches.append(watchNotifier.watch);
    }

    DBusConnection *connection = m_dbusConnection;
    locker.unlock();

    foreach (DBusWatch *watch, pendingWatches)
        dbus_watch_handle(watch, DBUS_WATCH_READABLE);

    while (dbus_connection_dispatch(connection) == DBUS_DISPATCH_DATA_REMAINS);
}

void SocketRequestorPrivate::socketWrite()
{
    QMutexLocker locker(&m_mutex);

    QList<DBusWatch *> pendingWatches;

    foreach (const WatchNotifier &watchNotifier, m_watchNotifiers) {
        if (watchNotifier.writeNotifier != sender())
            continue;

        watchNotifier.writeNotifier->setEnabled(false);
        pendingWatches.append(watchNotifier.watch);
    }

    locker.unlock();

    foreach (DBusWatch *watch, pendingWatches)
        dbus_watch_handle(watch, DBUS_WATCH_WRITABLE);
}

void SocketRequestorPrivate::registerObject(const QString &path, SocketRequestor *object)
{
    QMutexLocker locker(&m_mutex);

    m_dbusObjects.insert(path, object);
}

void SocketRequestorPrivate::unregisterObject(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    m_dbusObjects.remove(path);
}

void SocketRequestorPrivate::sendRequestAccess(const QString &adaptor, const QString &path,
                                               const QString &kind)
{
    QMutexLocker locker(&m_mutex);

    foreach (const WatchNotifier &watchNotifier, m_watchNotifiers)
        watchNotifier.writeNotifier->setEnabled(true);

    DBusMessage *message;
    DBusMessageIter args;

    message = dbus_message_new_method_call("com.nokia.nfc", adaptor.toLocal8Bit(),
                                           "com.nokia.nfc.Adapter", "RequestAccess");

    if (!message)
        return;

    dbus_message_iter_init_append(message, &args);
    const char *cdata = path.toUtf8().constData();
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_OBJECT_PATH, &cdata)) {
        dbus_message_unref(message);
        return;
    }

    cdata = kind.toUtf8().constData();
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &cdata)) {
        dbus_message_unref(message);
        return;
    }

    quint32 serial;
    dbus_connection_send(m_dbusConnection, message, &serial);

    dbus_message_unref(message);
}

void SocketRequestorPrivate::sendCancelAccessRequest(const QString &adaptor, const QString &path,
                                                     const QString &kind)
{
    QMutexLocker locker(&m_mutex);

    foreach (const WatchNotifier &watchNotifier, m_watchNotifiers)
        watchNotifier.writeNotifier->setEnabled(true);

    DBusMessage *message;
    DBusMessageIter args;

    message = dbus_message_new_method_call("com.nokia.nfc", adaptor.toLocal8Bit(),
                                           "com.nokia.nfc.Adapter", "CancelAccessRequest");

    if (!message)
        return;

    dbus_message_iter_init_append(message, &args);
    const char *cdata = path.toUtf8().constData();
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_OBJECT_PATH, &cdata)) {
        dbus_message_unref(message);
        return;
    }

    cdata = kind.toUtf8().constData();
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &cdata)) {
        dbus_message_unref(message);
        return;
    }

    quint32 serial;
    dbus_connection_send(m_dbusConnection, message, &serial);

    dbus_message_unref(message);
}

SocketRequestor::SocketRequestor(const QString &adaptor, QObject *parent)
:   QObject(parent), m_adaptor(adaptor)
{


}

SocketRequestor::~SocketRequestor()
{
}

void SocketRequestor::requestAccess(const QString &path, const QString &kind)
{
    SocketRequestorPrivate *s = socketRequestorPrivate();

    s->registerObject(path, this);

    QMetaObject::invokeMethod(s, "sendRequestAccess", Qt::QueuedConnection,
                              Q_ARG(QString, m_adaptor), Q_ARG(QString, path),
                              Q_ARG(QString, kind));
}

void SocketRequestor::cancelAccessRequest(const QString &path, const QString &kind)
{
    SocketRequestorPrivate *s = socketRequestorPrivate();

    s->unregisterObject(path);

    QMetaObject::invokeMethod(s, "sendCancelAccessRequest", Qt::QueuedConnection,
                              Q_ARG(QString, m_adaptor), Q_ARG(QString, path),
                              Q_ARG(QString, kind));
}

#include <moc_socketrequestor_p.cpp>
#include <socketrequestor.moc>

QTM_END_NAMESPACE
