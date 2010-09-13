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

#ifndef ICHANNELGROUP_H
#define ICHANNELGROUP_H

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
        class IChannelGroup : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Channel.Interface.Group";
            }
            IChannelGroup(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelGroup(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelGroup(DBus::DBusProxy *proxy);
            explicit IChannelGroup(const DBus::Interfaces::IChannel& mainInterface);
            IChannelGroup(const DBus::Interfaces::IChannel& mainInterface, QObject* parent);

            Q_PROPERTY(uint GroupFlags READ GroupFlags)
            inline uint GroupFlags() const
            {
                return qvariant_cast<uint>(internalPropGet("GroupFlags"));
            }

            Q_PROPERTY(DBus::Interfaces::HandleOwnerMap HandleOwners READ HandleOwners)
            inline DBus::Interfaces::HandleOwnerMap HandleOwners() const
            {
                return qvariant_cast<DBus::Interfaces::HandleOwnerMap>(internalPropGet("HandleOwners"));
            }

            Q_PROPERTY(DBus::Interfaces::LocalPendingInfoList LocalPendingMembers READ LocalPendingMembers)
            inline DBus::Interfaces::LocalPendingInfoList LocalPendingMembers() const
            {
                return qvariant_cast<DBus::Interfaces::LocalPendingInfoList>(internalPropGet("LocalPendingMembers"));
            }

            Q_PROPERTY(DBus::Interfaces::UIntList Members READ Members)
            inline DBus::Interfaces::UIntList Members() const
            {
                return qvariant_cast<DBus::Interfaces::UIntList>(internalPropGet("Members"));
            }

            Q_PROPERTY(DBus::Interfaces::UIntList RemotePendingMembers READ RemotePendingMembers)
            inline DBus::Interfaces::UIntList RemotePendingMembers() const
            {
                return qvariant_cast<DBus::Interfaces::UIntList>(internalPropGet("RemotePendingMembers"));
            }

            Q_PROPERTY(uint SelfHandle READ SelfHandle)
            inline uint SelfHandle() const
            {
                return qvariant_cast<uint>(internalPropGet("SelfHandle"));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<> AddMembers(const DBus::Interfaces::UIntList& contacts, const QString& message)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(contacts) << QVariant::fromValue(message);
                return asyncCallWithArgumentList(QLatin1String("AddMembers"), argumentList);
            }
            inline QDBusPendingReply<DBus::Interfaces::UIntList, DBus::Interfaces::UIntList, DBus::Interfaces::UIntList> GetAllMembers()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::UIntList, DBus::Interfaces::UIntList, DBus::Interfaces::UIntList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("GetAllMembers"));
            }
            inline QDBusPendingReply<uint> GetGroupFlags()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<uint>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("GetGroupFlags"));
            }
            inline QDBusPendingReply<DBus::Interfaces::UIntList> GetHandleOwners(const DBus::Interfaces::UIntList& handles)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::UIntList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(handles);
                return asyncCallWithArgumentList(QLatin1String("GetHandleOwners"), argumentList);
            }
            inline QDBusPendingReply<DBus::Interfaces::UIntList> GetLocalPendingMembers()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::UIntList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("GetLocalPendingMembers"));
            }
            inline QDBusPendingReply<DBus::Interfaces::LocalPendingInfoList> GetLocalPendingMembersWithInfo()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::LocalPendingInfoList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("GetLocalPendingMembersWithInfo"));
            }
            inline QDBusPendingReply<DBus::Interfaces::UIntList> GetMembers()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::UIntList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("GetMembers"));
            }
            inline QDBusPendingReply<DBus::Interfaces::UIntList> GetRemotePendingMembers()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::UIntList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("GetRemotePendingMembers"));
            }
            inline QDBusPendingReply<uint> GetSelfHandle()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<uint>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("GetSelfHandle"));
            }
            inline QDBusPendingReply<> RemoveMembers(const DBus::Interfaces::UIntList& contacts, const QString& message)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(contacts) << QVariant::fromValue(message);
                return asyncCallWithArgumentList(QLatin1String("RemoveMembers"), argumentList);
            }
            inline QDBusPendingReply<> RemoveMembersWithReason(const DBus::Interfaces::UIntList& contacts, const QString& message, uint reason)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(contacts) << QVariant::fromValue(message) << QVariant::fromValue(reason);
                return asyncCallWithArgumentList(QLatin1String("RemoveMembersWithReason"), argumentList);
            }

        Q_SIGNALS:
            void HandleOwnersChanged(const DBus::Interfaces::HandleOwnerMap& added, const DBus::Interfaces::UIntList& removed);
            void SelfHandleChanged(uint selfHandle);
            void GroupFlagsChanged(uint added, uint removed);
            void MembersChanged(const QString& message, const DBus::Interfaces::UIntList& added, const DBus::Interfaces::UIntList& removed, const DBus::Interfaces::UIntList& localPending, const DBus::Interfaces::UIntList& remotePending, uint actor, uint reason);
            void MembersChangedDetailed(const DBus::Interfaces::UIntList& added, const DBus::Interfaces::UIntList& removed, const DBus::Interfaces::UIntList& localPending, const DBus::Interfaces::UIntList& remotePending, const QVariantMap& details);

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }
}
Q_DECLARE_METATYPE(DBus::Interfaces::IChannelGroup*)

#endif //ICHANNELGROUP_H
