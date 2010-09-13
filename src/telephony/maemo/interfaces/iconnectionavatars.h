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

#ifndef ICONNECTIONAVATARS_H
#define ICONNECTIONAVATARS_H

#include <QtCore/QtGlobal>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

#include "maemo/interfaces/itypes.h"
#include "maemo/interfaces/iabstract.h"
#include "maemo/interfaces/iconnection.h"
#include "maemo/dbusproxy.h"

namespace DBus
{
    namespace Interfaces
    {
        class IConnectionAvatars : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Connection.Interface.Avatars";
            }

            IConnectionAvatars(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IConnectionAvatars(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IConnectionAvatars(DBus::DBusProxy *proxy);
            explicit IConnectionAvatars(const DBus::Interfaces::IConnection& mainInterface);
            IConnectionAvatars(const DBus::Interfaces::IConnection& mainInterface, QObject* parent);

            Q_PROPERTY(QStringList SupportedAvatarMIMETypes READ SupportedAvatarMIMETypes)
            inline QStringList SupportedAvatarMIMETypes() const
            {
                return qvariant_cast<QStringList>(internalPropGet("SupportedAvatarMIMETypes"));
            }

            Q_PROPERTY(uint MinimumAvatarHeight READ MinimumAvatarHeight)
            inline uint MinimumAvatarHeight() const
            {
                return qvariant_cast<uint>(internalPropGet("MinimumAvatarHeight"));
            }

            Q_PROPERTY(uint MinimumAvatarWidth READ MinimumAvatarWidth)
            inline uint MinimumAvatarWidth() const
            {
                return qvariant_cast<uint>(internalPropGet("MinimumAvatarWidth"));
            }

            Q_PROPERTY(uint RecommendedAvatarHeight READ RecommendedAvatarHeight)
            inline uint RecommendedAvatarHeight() const
            {
                return qvariant_cast<uint>(internalPropGet("RecommendedAvatarHeight"));
            }

            Q_PROPERTY(uint RecommendedAvatarWidth READ RecommendedAvatarWidth)
            inline uint RecommendedAvatarWidth() const
            {
                return qvariant_cast<uint>(internalPropGet("RecommendedAvatarWidth"));
            }

            Q_PROPERTY(uint MaximumAvatarHeight READ MaximumAvatarHeight)
            inline uint MaximumAvatarHeight() const
            {
                return qvariant_cast<uint>(internalPropGet("MaximumAvatarHeight"));
            }

            Q_PROPERTY(uint MaximumAvatarWidth READ MaximumAvatarWidth)
            inline uint MaximumAvatarWidth() const
            {
                return qvariant_cast<uint>(internalPropGet("MaximumAvatarWidth"));
            }

            Q_PROPERTY(uint MaximumAvatarBytes READ MaximumAvatarBytes)
            inline uint MaximumAvatarBytes() const
            {
                return qvariant_cast<uint>(internalPropGet("MaximumAvatarBytes"));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<QStringList, ushort, ushort, ushort, ushort, uint> GetAvatarRequirements()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QStringList, ushort, ushort, ushort, ushort, uint>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("GetAvatarRequirements"));
            }
            inline QDBusPendingReply<QStringList> GetAvatarTokens(const DBus::Interfaces::UIntList& contacts)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QStringList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(contacts);
                return asyncCallWithArgumentList(QLatin1String("GetAvatarTokens"), argumentList);
            }
            inline QDBusPendingReply<DBus::Interfaces::AvatarTokenMap> GetKnownAvatarTokens(const DBus::Interfaces::UIntList& contacts)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::AvatarTokenMap>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(contacts);
                return asyncCallWithArgumentList(QLatin1String("GetKnownAvatarTokens"), argumentList);
            }
            inline QDBusPendingReply<QByteArray, QString> RequestAvatar(uint contact)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QByteArray, QString>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(contact);
                return asyncCallWithArgumentList(QLatin1String("RequestAvatar"), argumentList);
            }
            inline QDBusPendingReply<> RequestAvatars(const DBus::Interfaces::UIntList& contacts)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(contacts);
                return asyncCallWithArgumentList(QLatin1String("RequestAvatars"), argumentList);
            }
            inline QDBusPendingReply<QString> SetAvatar(const QByteArray& avatar, const QString& MIMEType)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QString>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(avatar) << QVariant::fromValue(MIMEType);
                return asyncCallWithArgumentList(QLatin1String("SetAvatar"), argumentList);
            }
            inline QDBusPendingReply<> ClearAvatar()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("ClearAvatar"));
            }

        Q_SIGNALS:
            void AvatarUpdated(uint contact, const QString& newAvatarToken);
            void AvatarRetrieved(uint contact, const QString& token, const QByteArray& avatar, const QString& type);

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }//Interfaces
}//DBus
Q_DECLARE_METATYPE(DBus::Interfaces::IConnectionAvatars*)

#endif //ICONNECTIONAVATARS_H
