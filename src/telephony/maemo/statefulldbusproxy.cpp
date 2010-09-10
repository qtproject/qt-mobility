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

#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtDBus>

#include "statefulldbusproxy.h"
#include "maemo/constants.h"

namespace DBus
{
    StatefullDBusProxy::StatefullDBusProxy(const QDBusConnection &dbusConnection,
            const QString &busName, const QString &objectPath, QObject *parent)
        : DBusProxy(dbusConnection, busName, objectPath, parent)
    {
        QString uniqueName = busName;

        connect(dbusConnection.interface(), SIGNAL(serviceOwnerChanged(QString, QString, QString)),
                this, SLOT(onServiceOwnerChanged(QString, QString, QString)));

        if (!busName.startsWith(QLatin1String(":"))) {
            // For a stateful interface, it makes no sense to follow name-owner
            // changes, so we want to bind to the unique name.
            QDBusReply<QString> reply = dbusConnection.interface()->serviceOwner(
                    busName);
            if (reply.isValid()) {
                uniqueName = reply.value();
            } else {
                invalidate(reply.error());
            }
        }

        setBusName(uniqueName);
    }

    StatefullDBusProxy::~StatefullDBusProxy()
    {
    }

    void StatefullDBusProxy::onServiceOwnerChanged(const QString &name, const QString &oldOwner, const QString &newOwner)
    {
        if (isValid() && name == busName() && newOwner == QLatin1String("")) {
            invalidate(QLatin1String(TELEPATHY_DBUS_ERROR_NAME_HAS_NO_OWNER),
                    QLatin1String("Name owner lost (service crashed?)"));
        }
    }
} // DBus
