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

#ifndef CLIENTOBSERVERADAPTOR_H
#define CLIENTOBSERVERADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

#include "clientbaseobserver.h"
#include "types.h"

namespace DBus
{
    class ClientObserverAdaptor : public QDBusAbstractAdaptor
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Telepathy.Client.Observer")
        Q_CLASSINFO("D-Bus Introspection", ""
    "  <interface name=\"org.freedesktop.Telepathy.Client.Observer\" >\n"
    "    <property name=\"ObserverChannelFilter\" type=\"aa{sv}\" access=\"read\" />\n"
    "    <property name=\"Recover\" type=\"b\" access=\"read\" />\n"
    "    <method name=\"ObserveChannels\" >\n"
    "      <arg name=\"Account\" type=\"o\" direction=\"in\" />\n"
    "      <arg name=\"Connection\" type=\"o\" direction=\"in\" />\n"
    "      <arg name=\"Channels\" type=\"a(oa{sv})\" direction=\"in\" />\n"
    "      <arg name=\"Dispatch_Operation\" type=\"o\" direction=\"in\" />\n"
    "      <arg name=\"Requests_Satisfied\" type=\"ao\" direction=\"in\" />\n"
    "      <arg name=\"Observer_Info\" type=\"a{sv}\" direction=\"in\" />\n"
    "    </method>\n"
    "  </interface>\n"
            "")

        Q_PROPERTY(DBus::Interfaces::ChannelClassList ObserverChannelFilter READ ObserverChannelFilter)

    public:
        ClientObserverAdaptor(
                const QDBusConnection &bus,
                ClientBaseObserver *client,
                QObject *parent);
        virtual ~ClientObserverAdaptor();

    public: // Properties
        inline DBus::Interfaces::ChannelClassList ObserverChannelFilter() const
        {
            return mClient->observerChannelFilter();
        }

        inline bool Recover() const
        {
            return mClient->shouldRecover();
        }

    public Q_SLOTS: // Methods
        void ObserveChannels(const QDBusObjectPath &account,
                const QDBusObjectPath &connection,
                const DBus::Interfaces::ChannelDetailsList &channels,
                const QDBusObjectPath &dispatchOperation,
                const DBus::Interfaces::ObjectPathList &requestsSatisfied,
                const QVariantMap &observerInfo,
                const QDBusMessage &message);

    private:
        QDBusConnection mBus;
        ClientBaseObserver *mClient;
    };

} // DBus

Q_DECLARE_METATYPE(DBus::ClientObserverAdaptor*)

#endif //CLIENTOBSERVERADAPTOR_H
