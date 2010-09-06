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

#ifndef ICLIENTOBSERVER_H
#define ICLIENTOBSERVER_H

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
        class IClientObserver : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Client.Observer";
            }

            IClientObserver(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IClientObserver(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            ~IClientObserver();
            IClientObserver(DBus::DBusProxy *proxy);
            explicit IClientObserver(const DBus::Interfaces::IClient& mainInterface);
            IClientObserver(const DBus::Interfaces::IClient& mainInterface, QObject* parent);

            Q_PROPERTY(DBus::Interfaces::ChannelClassList ObserverChannelFilter READ ObserverChannelFilter)
            inline DBus::Interfaces::ChannelClassList ObserverChannelFilter() const
            {
                return qvariant_cast<DBus::Interfaces::ChannelClassList>(internalPropGet("ObserverChannelFilter"));
            }

            Q_PROPERTY(bool Recover READ Recover)
            inline bool Recover() const
            {
                return qvariant_cast<bool>(internalPropGet("Recover"));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<> ObserveChannels(const QDBusObjectPath& account, const QDBusObjectPath& connection, const DBus::Interfaces::ChannelDetailsList& channels, const QDBusObjectPath& dispatchOperation, const DBus::Interfaces::ObjectPathList& requestsSatisfied, const QVariantMap& observerInfo)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(account) << QVariant::fromValue(connection) << QVariant::fromValue(channels) << QVariant::fromValue(dispatchOperation) << QVariant::fromValue(requestsSatisfied) << QVariant::fromValue(observerInfo);
                return asyncCallWithArgumentList(QLatin1String("ObserveChannels"), argumentList);
            }

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }//Interfaces
}//DBus
Q_DECLARE_METATYPE(DBus::Interfaces::IClientObserver*)

#endif //ICLIENTOBSERVER_H
