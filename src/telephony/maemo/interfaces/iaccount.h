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

#ifndef IACCOUNT_H
#define IACCOUNT_H

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
        class IAccount : public DBus::IAbstract
        {
            Q_OBJECT
        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Account";
            }

            IAccount(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IAccount(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IAccount(DBus::DBusProxy *proxy);

            Q_PROPERTY(QStringList Interfaces READ Interfaces)
            inline QStringList Interfaces() const
            {
                return qvariant_cast<QStringList>(internalPropGet("Interfaces"));
            }

            Q_PROPERTY(QString DisplayName READ DisplayName WRITE setDisplayName)
            inline QString DisplayName() const
            {
                return qvariant_cast<QString>(internalPropGet("DisplayName"));
            }
            inline void setDisplayName(const QString& newValue)
            {
                internalPropSet("DisplayName", QVariant::fromValue(newValue));
            }

            Q_PROPERTY(QString Icon READ Icon WRITE setIcon)
            inline QString Icon() const
            {
                return qvariant_cast<QString>(internalPropGet("Icon"));
            }
            inline void setIcon(const QString& newValue)
            {
                internalPropSet("Icon", QVariant::fromValue(newValue));
            }

            Q_PROPERTY(bool Valid READ Valid)
            inline bool Valid() const
            {
                return qvariant_cast<bool>(internalPropGet("Valid"));
            }

            Q_PROPERTY(bool Enabled READ Enabled WRITE setEnabled)
            inline bool Enabled() const
            {
                return qvariant_cast<bool>(internalPropGet("Enabled"));
            }
            inline void setEnabled(bool newValue)
            {
                internalPropSet("Enabled", QVariant::fromValue(newValue));
            }

            Q_PROPERTY(QString Nickname READ Nickname WRITE setNickname)
            inline QString Nickname() const
            {
                return qvariant_cast<QString>(internalPropGet("Nickname"));
            }
            inline void setNickname(const QString& newValue)
            {
                internalPropSet("Nickname", QVariant::fromValue(newValue));
            }

            Q_PROPERTY(QVariantMap Parameters READ Parameters)
            inline QVariantMap Parameters() const
            {
                return qvariant_cast<QVariantMap>(internalPropGet("Parameters"));
            }

            Q_PROPERTY(DBus::Interfaces::SimplePresence AutomaticPresence READ AutomaticPresence WRITE setAutomaticPresence)
            inline DBus::Interfaces::SimplePresence AutomaticPresence() const
            {
                return qvariant_cast<DBus::Interfaces::SimplePresence>(internalPropGet("AutomaticPresence"));
            }
            inline void setAutomaticPresence(const DBus::Interfaces::SimplePresence& newValue)
            {
                internalPropSet("AutomaticPresence", QVariant::fromValue(newValue));
            }

            Q_PROPERTY(bool ConnectAutomatically READ ConnectAutomatically WRITE setConnectAutomatically)
            inline bool ConnectAutomatically() const
            {
                return qvariant_cast<bool>(internalPropGet("ConnectAutomatically"));
            }
            inline void setConnectAutomatically(bool newValue)
            {
                internalPropSet("ConnectAutomatically", QVariant::fromValue(newValue));
            }

            Q_PROPERTY(QDBusObjectPath Connection READ Connection)
            inline QDBusObjectPath Connection() const
            {
                return qvariant_cast<QDBusObjectPath>(internalPropGet("Connection"));
            }

            Q_PROPERTY(uint ConnectionStatus READ ConnectionStatus)
            inline uint ConnectionStatus() const
            {
                return qvariant_cast<uint>(internalPropGet("ConnectionStatus"));
            }

            Q_PROPERTY(uint ConnectionStatusReason READ ConnectionStatusReason)
            inline uint ConnectionStatusReason() const
            {
                return qvariant_cast<uint>(internalPropGet("ConnectionStatusReason"));
            }

            Q_PROPERTY(DBus::Interfaces::SimplePresence CurrentPresence READ CurrentPresence)
            inline DBus::Interfaces::SimplePresence CurrentPresence() const
            {
                return qvariant_cast<DBus::Interfaces::SimplePresence>(internalPropGet("CurrentPresence"));
            }

            Q_PROPERTY(DBus::Interfaces::SimplePresence RequestedPresence READ RequestedPresence WRITE setRequestedPresence)
            inline DBus::Interfaces::SimplePresence RequestedPresence() const
            {
                return qvariant_cast<DBus::Interfaces::SimplePresence>(internalPropGet("RequestedPresence"));
            }
            inline void setRequestedPresence(const DBus::Interfaces::SimplePresence& newValue)
            {
                internalPropSet("RequestedPresence", QVariant::fromValue(newValue));
            }

            Q_PROPERTY(QString NormalizedName READ NormalizedName)
            inline QString NormalizedName() const
            {
                return qvariant_cast<QString>(internalPropGet("NormalizedName"));
            }

            Q_PROPERTY(bool HasBeenOnline READ HasBeenOnline)
            inline bool HasBeenOnline() const
            {
                return qvariant_cast<bool>(internalPropGet("HasBeenOnline"));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<> Remove()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                return asyncCall(QLatin1String("Remove"));
            }

            inline QDBusPendingReply<QStringList> UpdateParameters(const QVariantMap& set, const QStringList& unset)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QStringList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }


                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(set) << QVariant::fromValue(unset);
                return asyncCallWithArgumentList(QLatin1String("UpdateParameters"), argumentList);
            }

            inline QDBusPendingReply<> Reconnect()
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }


                return asyncCall(QLatin1String("Reconnect"));
            }

        Q_SIGNALS:
            void Removed();
            void AccountPropertyChanged(const QVariantMap& properties);

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }//Interfaces
}//DBus
Q_DECLARE_METATYPE(DBus::Interfaces::IAccount*)

#endif // IACCOUNT_H

