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

#ifndef ICHANNELTYPEFILETRANSFER_H
#define ICHANNELTYPEFILETRANSFER_H

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
        class IChannelTypeFileTransfer : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Channel.Type.FileTransfer";
            }

            IChannelTypeFileTransfer(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelTypeFileTransfer(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IChannelTypeFileTransfer(DBus::DBusProxy *proxy);
            explicit IChannelTypeFileTransfer(const DBus::Interfaces::IChannel& mainInterface);
            IChannelTypeFileTransfer(const DBus::Interfaces::IChannel& mainInterface, QObject* parent);

            Q_PROPERTY(uint State READ State)
            inline uint State() const
            {
                return qvariant_cast<uint>(internalPropGet("State"));
            }

            Q_PROPERTY(QString ContentType READ ContentType)
            inline QString ContentType() const
            {
                return qvariant_cast<QString>(internalPropGet("ContentType"));
            }

            Q_PROPERTY(QString Filename READ Filename)
            inline QString Filename() const
            {
                return qvariant_cast<QString>(internalPropGet("Filename"));
            }

            Q_PROPERTY(qulonglong Size READ Size)
            inline qulonglong Size() const
            {
                return qvariant_cast<qulonglong>(internalPropGet("Size"));
            }

            Q_PROPERTY(uint ContentHashType READ ContentHashType)
            inline uint ContentHashType() const
            {
                return qvariant_cast<uint>(internalPropGet("ContentHashType"));
            }

            Q_PROPERTY(QString ContentHash READ ContentHash)
            inline QString ContentHash() const
            {
                return qvariant_cast<QString>(internalPropGet("ContentHash"));
            }

            Q_PROPERTY(QString Description READ Description)
            inline QString Description() const
            {
                return qvariant_cast<QString>(internalPropGet("Description"));
            }

            Q_PROPERTY(qlonglong Date READ Date)
            inline qlonglong Date() const
            {
                return qvariant_cast<qlonglong>(internalPropGet("Date"));
            }

            Q_PROPERTY(DBus::Interfaces::SupportedSocketMap AvailableSocketTypes READ AvailableSocketTypes)
            inline DBus::Interfaces::SupportedSocketMap AvailableSocketTypes() const
            {
                return qvariant_cast<DBus::Interfaces::SupportedSocketMap>(internalPropGet("AvailableSocketTypes"));
            }

            Q_PROPERTY(qulonglong TransferredBytes READ TransferredBytes)
            inline qulonglong TransferredBytes() const
            {
                return qvariant_cast<qulonglong>(internalPropGet("TransferredBytes"));
            }

            Q_PROPERTY(qulonglong InitialOffset READ InitialOffset)
            inline qulonglong InitialOffset() const
            {
                return qvariant_cast<qulonglong>(internalPropGet("InitialOffset"));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<QDBusVariant> AcceptFile(uint addressType, uint accessControl, const QDBusVariant& accessControlParam, qulonglong offset)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QDBusVariant>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(addressType) << QVariant::fromValue(accessControl) << QVariant::fromValue(accessControlParam) << QVariant::fromValue(offset);
                return asyncCallWithArgumentList(QLatin1String("AcceptFile"), argumentList);
            }
            inline QDBusPendingReply<QDBusVariant> ProvideFile(uint addressType, uint accessControl, const QDBusVariant& accessControlParam)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<QDBusVariant>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(addressType) << QVariant::fromValue(accessControl) << QVariant::fromValue(accessControlParam);
                return asyncCallWithArgumentList(QLatin1String("ProvideFile"), argumentList);
            }

        Q_SIGNALS:
            void FileTransferStateChanged(uint state, uint reason);
            void TransferredBytesChanged(qulonglong count);
            void InitialOffsetDefined(qulonglong initialOffset);

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }
}
Q_DECLARE_METATYPE(DBus::Interfaces::IChannelTypeFileTransfer*)

#endif //ICHANNELTYPEFILETRANSFER_H
