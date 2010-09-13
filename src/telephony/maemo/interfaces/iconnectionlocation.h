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

#ifndef ICONNECTIONLOCATION_H
#define ICONNECTIONLOCATION_H

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
        class IConnectionLocation : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Connection.Interface.Location";
            }

            IConnectionLocation(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IConnectionLocation(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IConnectionLocation(DBus::DBusProxy *proxy);
            explicit IConnectionLocation(const DBus::Interfaces::IConnection& mainInterface);
            IConnectionLocation(const DBus::Interfaces::IConnection& mainInterface, QObject* parent);

            Q_PROPERTY(DBus::Interfaces::UIntList LocationAccessControlTypes READ LocationAccessControlTypes)
            inline DBus::Interfaces::UIntList LocationAccessControlTypes() const
            {
                return qvariant_cast<DBus::Interfaces::UIntList>(internalPropGet("LocationAccessControlTypes"));
            }

            Q_PROPERTY(DBus::Interfaces::RichPresenceAccessControl LocationAccessControl READ LocationAccessControl WRITE setLocationAccessControl)
            inline DBus::Interfaces::RichPresenceAccessControl LocationAccessControl() const
            {
                return qvariant_cast<DBus::Interfaces::RichPresenceAccessControl>(internalPropGet("LocationAccessControl"));
            }
            inline void setLocationAccessControl(const DBus::Interfaces::RichPresenceAccessControl& newValue)
            {
                internalPropSet("LocationAccessControl", QVariant::fromValue(newValue));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<DBus::Interfaces::ContactLocations> GetLocations(const DBus::Interfaces::UIntList& contacts)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::ContactLocations>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(contacts);
                return asyncCallWithArgumentList(QLatin1String("GetLocations"), argumentList);
            }
            inline QDBusPendingReply<QVariantMap> RequestLocation(uint contact)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QVariantMap>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(contact);
                return asyncCallWithArgumentList(QLatin1String("RequestLocation"), argumentList);
            }
            inline QDBusPendingReply<> SetLocation(const QVariantMap& location)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(location);
                return asyncCallWithArgumentList(QLatin1String("SetLocation"), argumentList);
            }

        Q_SIGNALS:
            void LocationUpdated(uint contact, const QVariantMap& location);

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }//Interfaces
}//DBus
Q_DECLARE_METATYPE(DBus::Interfaces::IConnectionLocation*)

#endif //ICONNECTIONLOCATION_H
