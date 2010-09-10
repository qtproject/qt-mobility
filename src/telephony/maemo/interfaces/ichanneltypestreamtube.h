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

#ifndef ICHANNELTYPESTREAMTUBE_H
#define ICHANNELTYPESTREAMTUBE_H

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
        class IChannelTypeStreamTube : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Channel.Type.StreamTube";
            }

            IChannelTypeStreamTube(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelTypeStreamTube(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelTypeStreamTube(DBus::DBusProxy *proxy);
            explicit IChannelTypeStreamTube(const DBus::Interfaces::IChannel& mainInterface);
            IChannelTypeStreamTube(const DBus::Interfaces::IChannel& mainInterface, QObject* parent);

            Q_PROPERTY(QString Service READ Service)
            inline QString Service() const
            {
                return qvariant_cast<QString>(internalPropGet("Service"));
            }

            Q_PROPERTY(DBus::Interfaces::SupportedSocketMap SupportedSocketTypes READ SupportedSocketTypes)
            inline DBus::Interfaces::SupportedSocketMap SupportedSocketTypes() const
            {
                return qvariant_cast<DBus::Interfaces::SupportedSocketMap>(internalPropGet("SupportedSocketTypes"));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<> Offer(uint addresstype, const QDBusVariant& address, uint accesscontrol, const QVariantMap& parameters)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(addresstype) << QVariant::fromValue(address) << QVariant::fromValue(accesscontrol) << QVariant::fromValue(parameters);
                return asyncCallWithArgumentList(QLatin1String("Offer"), argumentList);
            }
            inline QDBusPendingReply<QDBusVariant> Accept(uint addresstype, uint accesscontrol, const QDBusVariant& accesscontrolparam)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QDBusVariant>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(addresstype) << QVariant::fromValue(accesscontrol) << QVariant::fromValue(accesscontrolparam);
                return asyncCallWithArgumentList(QLatin1String("Accept"), argumentList);
            }

        Q_SIGNALS:
            void NewRemoteConnection(uint handle, const QDBusVariant& connectionParam, uint connectionID);
            void NewLocalConnection(uint connectionID);
            void ConnectionClosed(uint connectionID, const QString& error, const QString& message);

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }
}
Q_DECLARE_METATYPE(DBus::Interfaces::IChannelTypeStreamTube*)

#endif //ICHANNELTYPESTREAMTUBE_H
