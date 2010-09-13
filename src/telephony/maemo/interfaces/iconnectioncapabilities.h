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

#ifndef ICONNECTIONCAPABILITIES_H
#define ICONNECTIONCAPABILITIES_H

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
        class IConnectionCapabilities : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Connection.Interface.Capabilities";
            }

            IConnectionCapabilities(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IConnectionCapabilities(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IConnectionCapabilities(DBus::DBusProxy *proxy);
            explicit IConnectionCapabilities(const DBus::Interfaces::IConnection& mainInterface);
            IConnectionCapabilities(const DBus::Interfaces::IConnection& mainInterface, QObject* parent);

        public Q_SLOTS:
            inline QDBusPendingReply<DBus::Interfaces::CapabilityPairList> AdvertiseCapabilities(const DBus::Interfaces::CapabilityPairList& add, const QStringList& remove)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::CapabilityPairList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(add) << QVariant::fromValue(remove);
                return asyncCallWithArgumentList(QLatin1String("AdvertiseCapabilities"), argumentList);
            }
            inline QDBusPendingReply<DBus::Interfaces::ContactCapabilityList> GetCapabilities(const DBus::Interfaces::UIntList& handles)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::ContactCapabilityList>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(handles);
                return asyncCallWithArgumentList(QLatin1String("GetCapabilities"), argumentList);
            }

        Q_SIGNALS:
            void CapabilitiesChanged(const DBus::Interfaces::CapabilityChangeList& caps);

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }//Interfaces
}//DBus
Q_DECLARE_METATYPE(DBus::Interfaces::IConnectionCapabilities*)

#endif //ICONNECTIONCAPABILITIES_H
