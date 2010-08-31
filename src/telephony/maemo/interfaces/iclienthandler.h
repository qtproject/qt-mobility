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

#ifndef ICLIENTHANDLER_H
#define ICLIENTHANDLER_H

#include <QtCore/QtGlobal>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

#include "maemo/interfaces/itypes.h"
#include "maemo/interfaces/iabstract.h"
#include "maemo/interfaces/iclient.h"
#include "maemo/dbusproxy.h"

namespace DBus
{
    namespace Interfaces
    {
        class IClientHandler : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Client.Handler";
            }

            IClientHandler(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IClientHandler(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            ~IClientHandler();
            IClientHandler(DBus::DBusProxy *proxy);
            explicit IClientHandler(const DBus::Interfaces::IClient& mainInterface);
            IClientHandler(const DBus::Interfaces::IClient& mainInterface, QObject* parent);

            Q_PROPERTY(DBus::Interfaces::ChannelClassList HandlerChannelFilter READ HandlerChannelFilter)
            inline DBus::Interfaces::ChannelClassList HandlerChannelFilter() const
            {
                return qvariant_cast<DBus::Interfaces::ChannelClassList>(internalPropGet("HandlerChannelFilter"));
            }

            Q_PROPERTY(bool BypassApproval READ BypassApproval)
            inline bool BypassApproval() const
            {
                return qvariant_cast<bool>(internalPropGet("BypassApproval"));
            }

            Q_PROPERTY(QStringList Capabilities READ Capabilities)
            inline QStringList Capabilities() const
            {
                return qvariant_cast<QStringList>(internalPropGet("Capabilities"));
            }

            Q_PROPERTY(DBus::Interfaces::ObjectPathList HandledChannels READ HandledChannels)
            inline DBus::Interfaces::ObjectPathList HandledChannels() const
            {
                return qvariant_cast<DBus::Interfaces::ObjectPathList>(internalPropGet("HandledChannels"));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<> HandleChannels(const QDBusObjectPath& account, const QDBusObjectPath& connection, const DBus::Interfaces::ChannelDetailsList& channels, const DBus::Interfaces::ObjectPathList& requestsSatisfied, qulonglong userActionTime, const QVariantMap& handlerInfo)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(account) << QVariant::fromValue(connection) << QVariant::fromValue(channels) << QVariant::fromValue(requestsSatisfied) << QVariant::fromValue(userActionTime) << QVariant::fromValue(handlerInfo);
                return asyncCallWithArgumentList(QLatin1String("HandleChannels"), argumentList);
            }

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }//Interfaces
}//DBus
Q_DECLARE_METATYPE(DBus::Interfaces::IClientHandler*)

#endif //ICLIENTHANDLER_H
