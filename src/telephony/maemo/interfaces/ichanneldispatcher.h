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

#ifndef ICHANNELDISPATCHER_H
#define ICHANNELDISPATCHER_H

#include <QtCore/QtGlobal>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

#include "maemo/interfaces/itypes.h"
#include "maemo/interfaces/iabstract.h"
#include "maemo/dbusproxy.h"

namespace DBus
{
    namespace Interfaces
    {
        class IChannelDispatcher : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.ChannelDispatcher";
            }

            IChannelDispatcher(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelDispatcher(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelDispatcher(DBus::DBusProxy *proxy);

            Q_PROPERTY(QStringList Interfaces READ Interfaces)
            inline QStringList Interfaces() const
            {
                return qvariant_cast<QStringList>(internalPropGet("Interfaces"));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<QDBusObjectPath> CreateChannel(const QDBusObjectPath& account, const QVariantMap& requestedProperties, qlonglong userActionTime, const QString& preferredHandler)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QDBusObjectPath>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(account) << QVariant::fromValue(requestedProperties) << QVariant::fromValue(userActionTime) << QVariant::fromValue(preferredHandler);
                return asyncCallWithArgumentList(QLatin1String("CreateChannel"), argumentList);
            }
            inline QDBusPendingReply<QDBusObjectPath> EnsureChannel(const QDBusObjectPath& account, const QVariantMap& requestedProperties, qlonglong userActionTime, const QString& preferredHandler)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QDBusObjectPath>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(account) << QVariant::fromValue(requestedProperties) << QVariant::fromValue(userActionTime) << QVariant::fromValue(preferredHandler);
                return asyncCallWithArgumentList(QLatin1String("EnsureChannel"), argumentList);
            }

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }//Interfaces
}//DBus
Q_DECLARE_METATYPE(DBus::Interfaces::IChannelDispatcher*)

#endif //ICHANNELDISPATCHER_H
