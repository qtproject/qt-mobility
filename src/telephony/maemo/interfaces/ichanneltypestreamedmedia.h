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

#ifndef ICHANNELTYPESTREAMEDMEDIA_H
#define ICHANNELTYPESTREAMEDMEDIA_H

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
        class IChannelTypeStreamedMedia : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Channel.Type.StreamedMedia";
            }

            IChannelTypeStreamedMedia(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelTypeStreamedMedia(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelTypeStreamedMedia(DBus::DBusProxy *proxy);
            explicit IChannelTypeStreamedMedia(const DBus::Interfaces::IChannel& mainInterface);
            IChannelTypeStreamedMedia(const DBus::Interfaces::IChannel& mainInterface, QObject* parent);

            Q_PROPERTY(bool InitialAudio READ InitialAudio)
            inline bool InitialAudio() const
            {
                return qvariant_cast<bool>(internalPropGet("InitialAudio"));
            }

            Q_PROPERTY(bool InitialVideo READ InitialVideo)
            inline bool InitialVideo() const
            {
                return qvariant_cast<bool>(internalPropGet("InitialVideo"));
            }

            Q_PROPERTY(bool ImmutableStreams READ ImmutableStreams)
            inline bool ImmutableStreams() const
            {
                return qvariant_cast<bool>(internalPropGet("ImmutableStreams"));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<DBus::Interfaces::MediaStreamInfoList> ListStreams()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::MediaStreamInfoList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("ListStreams"));
            }
            inline QDBusPendingReply<> RemoveStreams(const DBus::Interfaces::UIntList& streams)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(streams);
                return asyncCallWithArgumentList(QLatin1String("RemoveStreams"), argumentList);
            }
            inline QDBusPendingReply<> RequestStreamDirection(uint streamID, uint streamDirection)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(streamID) << QVariant::fromValue(streamDirection);
                return asyncCallWithArgumentList(QLatin1String("RequestStreamDirection"), argumentList);
            }
            inline QDBusPendingReply<DBus::Interfaces::MediaStreamInfoList> RequestStreams(uint contactHandle, const DBus::Interfaces::UIntList& types)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::MediaStreamInfoList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(contactHandle) << QVariant::fromValue(types);
                return asyncCallWithArgumentList(QLatin1String("RequestStreams"), argumentList);
            }

        Q_SIGNALS:
            void StreamAdded(uint streamID, uint contactHandle, uint streamType);
            void StreamDirectionChanged(uint streamID, uint streamDirection, uint pendingFlags);
            void StreamError(uint streamID, uint errorCode, const QString& message);
            void StreamRemoved(uint streamID);
            void StreamStateChanged(uint streamID, uint streamState);

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }
}
Q_DECLARE_METATYPE(DBus::Interfaces::IChannelTypeStreamedMedia*)

#endif //ICHANNELTYPESTREAMEDMEDIA_H
