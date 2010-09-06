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

#ifndef IACCOUNTMANAGER_H
#define IACCOUNTMANAGER_H

#include <QtCore/QtGlobal>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

#include "maemo/interfaces/itypes.h"
#include "maemo/interfaces/iabstract.h"
#include "maemo/dbusproxy.h"

namespace DBus
{
    namespace Interfaces
    {
        class IAccountManager : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.AccountManager";
            }

            IAccountManager(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IAccountManager(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IAccountManager(DBus::DBusProxy *proxy);

            Q_PROPERTY(QStringList Interfaces READ Interfaces)
            inline QStringList Interfaces() const
            {
                return qvariant_cast<QStringList>(internalPropGet("Interfaces"));
            }

            Q_PROPERTY(DBus::Interfaces::ObjectPathList ValidAccounts READ ValidAccounts)
            inline DBus::Interfaces::ObjectPathList ValidAccounts() const
            {
                return qvariant_cast<DBus::Interfaces::ObjectPathList>(internalPropGet("ValidAccounts"));
            }

            Q_PROPERTY(DBus::Interfaces::ObjectPathList InvalidAccounts READ InvalidAccounts)
            inline DBus::Interfaces::ObjectPathList InvalidAccounts() const
            {
                return qvariant_cast<DBus::Interfaces::ObjectPathList>(internalPropGet("InvalidAccounts"));
            }

            Q_PROPERTY(QStringList SupportedAccountProperties READ SupportedAccountProperties)

            inline QStringList SupportedAccountProperties() const
            {
                return qvariant_cast<QStringList>(internalPropGet("SupportedAccountProperties"));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<QDBusObjectPath> CreateAccount(const QString& connectionManager, const QString& protocol, const QString& displayName, const QVariantMap& parameters, const QVariantMap& properties)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QDBusObjectPath>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }


                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(connectionManager) << QVariant::fromValue(protocol) << QVariant::fromValue(displayName) << QVariant::fromValue(parameters) << QVariant::fromValue(properties);
                return asyncCallWithArgumentList(QLatin1String("CreateAccount"), argumentList);
            }

        Q_SIGNALS:
            void AccountRemoved(const QDBusObjectPath& account);
            void AccountValidityChanged(const QDBusObjectPath& account, bool valid);

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }//Interfaces
}//DBus
Q_DECLARE_METATYPE(DBus::Interfaces::IAccountManager*)

#endif //IACCOUNTMANAGER_H
