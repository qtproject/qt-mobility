/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtDBus>

#include "dbusproxy.h"
#include "maemo/constants.h"

namespace DBus
{
    struct DBusProxy::Private
    {
        QDBusConnection dbusConnection;
        QString busName;
        QString objectPath;
        QString invalidationReason;
        QString invalidationMessage;

        Private(const QDBusConnection &, const QString &, const QString &);
    };

    DBusProxy::Private::Private(const QDBusConnection &dbusConnection,
                const QString &busName, const QString &objectPath)
        : dbusConnection(dbusConnection),
          busName(busName),
          objectPath(objectPath)
    {
    }

    DBusProxy::DBusProxy(const QDBusConnection &dbusConnection,
            const QString &busName, const QString &path, QObject *parent)
        : QObject(parent),
          mPriv(new Private(dbusConnection, busName, path))
    {
        if (!dbusConnection.isConnected()) {
            invalidate(QLatin1String(TELEPATHY_ERROR_DISCONNECTED),
                    QLatin1String("DBus connection disconnected"));
        }
    }

    DBusProxy::~DBusProxy()
    {
        delete mPriv;
    }

    QDBusConnection DBusProxy::dbusConnection() const
    {
        return mPriv->dbusConnection;
    }

    QString DBusProxy::objectPath() const
    {
        return mPriv->objectPath;
    }

    QString DBusProxy::busName() const
    {
        return mPriv->busName;
    }

    void DBusProxy::setBusName(const QString &busName)
    {
        mPriv->busName = busName;
    }

    bool DBusProxy::isValid() const
    {
        return mPriv->invalidationReason.isEmpty();
    }

    QString DBusProxy::invalidationReason() const
    {
        return mPriv->invalidationReason;
    }

    QString DBusProxy::invalidationMessage() const
    {
        return mPriv->invalidationMessage;
    }

    void DBusProxy::invalidate(const QString &reason, const QString &message)
    {
        if (!isValid()) {
            qDebug().nospace() << "Already invalidated by "
                << mPriv->invalidationReason
                << ", not replacing with " << reason
                << " \"" << message << "\"";
            return;
        }

        Q_ASSERT(!reason.isEmpty());

        qDebug().nospace() << "proxy invalidated: " << reason
            << ": " << message;
        qDebug() << "- bus: " << mPriv->busName;
        qDebug() << "- path" << mPriv->objectPath;

        mPriv->invalidationReason = reason;
        mPriv->invalidationMessage = message;

        Q_ASSERT(!isValid());

        // Defer emitting the invalidated signal until we next
        // return to the mainloop.
        QTimer::singleShot(0, this, SLOT(emitInvalidated()));
    }

    void DBusProxy::invalidate(const QDBusError &error)
    {
        invalidate(error.name(), error.message());
    }

    void DBusProxy::emitInvalidated()
    {
        Q_ASSERT(!isValid());

        emit invalidated(this, mPriv->invalidationReason, mPriv->invalidationMessage);
    }
} // DBus
