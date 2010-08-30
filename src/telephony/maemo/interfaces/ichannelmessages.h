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

#ifndef ICHANNELMESSAGES_H
#define ICHANNELMESSAGES_H

#include <QtCore/QtGlobal>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

#include "maemo/interfaces/itypes.h"
#include "maemo/interfaces/iabstract.h"
#include "maemo/interfaces/ichannel.h"
#include "maemo/dbusproxy.h"

namespace DBus
{
    namespace Interfaces
    {
        class IChannelMessages : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Channel.Interface.Messages";
            }
            IChannelMessages(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelMessages(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelMessages(DBus::DBusProxy *proxy);
            explicit IChannelMessages(const DBus::Interfaces::IChannel& mainInterface);
            IChannelMessages(const DBus::Interfaces::IChannel& mainInterface, QObject* parent);

            Q_PROPERTY(QStringList SupportedContentTypes READ SupportedContentTypes)
            inline QStringList SupportedContentTypes() const
            {
                return qvariant_cast<QStringList>(internalPropGet("SupportedContentTypes"));
            }

            Q_PROPERTY(uint MessagePartSupportFlags READ MessagePartSupportFlags)
            inline uint MessagePartSupportFlags() const
            {
                return qvariant_cast<uint>(internalPropGet("MessagePartSupportFlags"));
            }

            Q_PROPERTY(DBus::Interfaces::MessagePartListList PendingMessages READ PendingMessages)
            inline DBus::Interfaces::MessagePartListList PendingMessages() const
            {
                return qvariant_cast<DBus::Interfaces::MessagePartListList>(internalPropGet("PendingMessages"));
            }

            Q_PROPERTY(uint DeliveryReportingSupport READ DeliveryReportingSupport)
            inline uint DeliveryReportingSupport() const
            {
                return qvariant_cast<uint>(internalPropGet("DeliveryReportingSupport"));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<QString> SendMessage(const DBus::Interfaces::MessagePartList& message, uint flags)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QString>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(message) << QVariant::fromValue(flags);
                return asyncCallWithArgumentList(QLatin1String("SendMessage"), argumentList);
            }
            inline QDBusPendingReply<DBus::Interfaces::MessagePartContentMap> GetPendingMessageContent(uint messageID, const DBus::Interfaces::UIntList& parts)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::MessagePartContentMap>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(messageID) << QVariant::fromValue(parts);
                return asyncCallWithArgumentList(QLatin1String("GetPendingMessageContent"), argumentList);
            }

        Q_SIGNALS:
            void MessageSent(const DBus::Interfaces::MessagePartList& content, uint flags, const QString& messageToken);
            void PendingMessagesRemoved(const DBus::Interfaces::UIntList& messageIDs);
            void MessageReceived(const DBus::Interfaces::MessagePartList& message);

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }
}
Q_DECLARE_METATYPE(DBus::Interfaces::IChannelMessages*)

#endif //ICHANNELMESSAGES_H
