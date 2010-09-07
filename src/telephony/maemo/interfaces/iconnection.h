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

#ifndef ICONNECTION_H
#define ICONNECTION_H

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
        class IConnection : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Connection";
            }

            IConnection(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IConnection(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IConnection(DBus::DBusProxy *proxy);
            ~IConnection();
            Q_PROPERTY(QStringList Interfaces READ Interfaces)
            inline QStringList Interfaces() const
            {
                return qvariant_cast<QStringList>(internalPropGet("Interfaces"));
            }

            Q_PROPERTY(uint SelfHandle READ SelfHandle)
            inline uint SelfHandle() const
            {
                return qvariant_cast<uint>(internalPropGet("SelfHandle"));
            }

            Q_PROPERTY(uint Status READ Status)
            inline uint Status() const
            {
                return qvariant_cast<uint>(internalPropGet("Status"));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<> Connect()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("Connect"));
            }
            inline QDBusPendingReply<> Disconnect()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("Disconnect"));
            }
            inline QDBusPendingReply<QStringList> GetInterfaces()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QStringList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("GetInterfaces"));
            }
            inline QDBusPendingReply<QString> GetProtocol()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QString>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("GetProtocol"));
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
            inline QDBusPendingReply<uint> GetStatus()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<uint>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("GetStatus"));
            }
            inline QDBusPendingReply<> HoldHandles(uint handleType, const DBus::Interfaces::UIntList& handles)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(handleType) << QVariant::fromValue(handles);
                return asyncCallWithArgumentList(QLatin1String("HoldHandles"), argumentList);
            }
            inline QDBusPendingReply<QStringList> InspectHandles(uint handleType, const DBus::Interfaces::UIntList& handles)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QStringList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(handleType) << QVariant::fromValue(handles);
                return asyncCallWithArgumentList(QLatin1String("InspectHandles"), argumentList);
            }
            inline QDBusPendingReply<DBus::Interfaces::ChannelInfoList> ListChannels()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::ChannelInfoList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("ListChannels"));
            }
            inline QDBusPendingReply<> ReleaseHandles(uint handleType, const DBus::Interfaces::UIntList& handles)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(handleType) << QVariant::fromValue(handles);
                return asyncCallWithArgumentList(QLatin1String("ReleaseHandles"), argumentList);
            }
            inline QDBusPendingReply<QDBusObjectPath> RequestChannel(const QString& type, uint handleType, uint handle, bool suppressHandler)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QDBusObjectPath>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(type) << QVariant::fromValue(handleType) << QVariant::fromValue(handle) << QVariant::fromValue(suppressHandler);
                return asyncCallWithArgumentList(QLatin1String("RequestChannel"), argumentList);
            }
            inline QDBusPendingReply<DBus::Interfaces::UIntList> RequestHandles(uint handleType, const QStringList& identifiers)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::UIntList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(handleType) << QVariant::fromValue(identifiers);
                return asyncCallWithArgumentList(QLatin1String("RequestHandles"), argumentList);
            }

        Q_SIGNALS:
            void SelfHandleChanged(uint selfHandle);
            void NewChannel(const QDBusObjectPath& objectPath, const QString& channelType, uint handleType, uint handle, bool suppressHandler);
            void ConnectionError(const QString& error, const QVariantMap& details);
            void StatusChanged(uint status, uint reason);

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }//Interfaces
}//DBus
Q_DECLARE_METATYPE(DBus::Interfaces::IConnection*)

#endif //ICONNECTION_H
