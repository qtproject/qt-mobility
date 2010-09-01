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

#ifndef ICHANNELTYPETUBES_H
#define ICHANNELTYPETUBES_H

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
        class IChannelTypeTubes : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Channel.Type.Tubes";
            }

            IChannelTypeTubes(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelTypeTubes(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelTypeTubes(DBus::DBusProxy *proxy);
            explicit IChannelTypeTubes(const DBus::Interfaces::IChannel& mainInterface);
            IChannelTypeTubes(const DBus::Interfaces::IChannel& mainInterface, QObject* parent);

        public Q_SLOTS:
            inline QDBusPendingReply<DBus::Interfaces::SupportedSocketMap> GetAvailableStreamTubeTypes()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::SupportedSocketMap>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("GetAvailableStreamTubeTypes"));
            }
            inline QDBusPendingReply<DBus::Interfaces::UIntList> GetAvailableTubeTypes()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::UIntList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("GetAvailableTubeTypes"));
            }
            inline QDBusPendingReply<DBus::Interfaces::TubeInfoList> ListTubes()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::TubeInfoList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("ListTubes"));
            }
            inline QDBusPendingReply<uint> OfferDBusTube(const QString& service, const QVariantMap& parameters)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<uint>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(service) << QVariant::fromValue(parameters);
                return asyncCallWithArgumentList(QLatin1String("OfferDBusTube"), argumentList);
            }
            inline QDBusPendingReply<uint> OfferStreamTube(const QString& service, const QVariantMap& parameters, uint addressType, const QDBusVariant& address, uint accessControl, const QDBusVariant& accessControlParam)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<uint>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(service) << QVariant::fromValue(parameters) << QVariant::fromValue(addressType) << QVariant::fromValue(address) << QVariant::fromValue(accessControl) << QVariant::fromValue(accessControlParam);
                return asyncCallWithArgumentList(QLatin1String("OfferStreamTube"), argumentList);
            }
            inline QDBusPendingReply<QString> AcceptDBusTube(uint ID)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QString>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(ID);
                return asyncCallWithArgumentList(QLatin1String("AcceptDBusTube"), argumentList);
            }
            inline QDBusPendingReply<QDBusVariant> AcceptStreamTube(uint ID, uint addressType, uint accessControl, const QDBusVariant& accessControlParam)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QDBusVariant>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(ID) << QVariant::fromValue(addressType) << QVariant::fromValue(accessControl) << QVariant::fromValue(accessControlParam);
                return asyncCallWithArgumentList(QLatin1String("AcceptStreamTube"), argumentList);
            }
            inline QDBusPendingReply<> CloseTube(uint ID)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(ID);
                return asyncCallWithArgumentList(QLatin1String("CloseTube"), argumentList);
            }
            inline QDBusPendingReply<QString> GetDBusTubeAddress(uint ID)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QString>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(ID);
                return asyncCallWithArgumentList(QLatin1String("GetDBusTubeAddress"), argumentList);
            }
            inline QDBusPendingReply<DBus::Interfaces::DBusTubeMemberList> GetDBusNames(uint ID)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::DBusTubeMemberList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(ID);
                return asyncCallWithArgumentList(QLatin1String("GetDBusNames"), argumentList);
            }
            inline QDBusPendingReply<uint, QDBusVariant> GetStreamTubeSocketAddress(uint ID)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<uint, QDBusVariant>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(ID);
                return asyncCallWithArgumentList(QLatin1String("GetStreamTubeSocketAddress"), argumentList);
            }

        Q_SIGNALS:
            void NewTube(uint ID, uint initiator, uint type, const QString& service, const QVariantMap& parameters, uint state);
            void TubeStateChanged(uint ID, uint state);
            void TubeClosed(uint ID);
            void DBusNamesChanged(uint ID, const DBus::Interfaces::DBusTubeMemberList& added, const DBus::Interfaces::UIntList& removed);
            void StreamTubeNewConnection(uint ID, uint handle);

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }
}
Q_DECLARE_METATYPE(DBus::Interfaces::IChannelTypeTubes*)

#endif //ICHANNELTYPETUBES_H
