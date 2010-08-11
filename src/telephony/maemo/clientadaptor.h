/*
 * This file is part of TelepathyQt4
 *
 * Copyright (C) 2009 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2009 Nokia Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef CLIENTADAPTOR_H
#define CLIENTADAPTOR_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

#include "abstractclient.h"
#include "types.h"

namespace Tp
{

class PendingOperation;

class ClientAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Telepathy.Client")
    Q_CLASSINFO("D-Bus Introspection", ""
"  <interface name=\"org.freedesktop.Telepathy.Client\" >\n"
"    <property name=\"Interfaces\" type=\"as\" access=\"read\" />\n"
"  </interface>\n"
        "")

    Q_PROPERTY(QStringList Interfaces READ Interfaces)

public:
    ClientAdaptor(const QStringList &interfaces, QObject *parent);
    virtual ~ClientAdaptor();

public: // Properties
    inline QStringList Interfaces() const
    {
        return mInterfaces;
    }

private:
    QStringList mInterfaces;
};

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

    Q_PROPERTY(Tp::ChannelClassList ObserverChannelFilter READ ObserverChannelFilter)

public:
    ClientObserverAdaptor(
            const QDBusConnection &bus,
            AbstractClientObserver *client,
            QObject *parent);
    virtual ~ClientObserverAdaptor();

public: // Properties
    inline Tp::ChannelClassList ObserverChannelFilter() const
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
            const Tp::ChannelDetailsList &channels,
            const QDBusObjectPath &dispatchOperation,
            const Tp::ObjectPathList &requestsSatisfied,
            const QVariantMap &observerInfo,
            const QDBusMessage &message);

private:
    QDBusConnection mBus;
    AbstractClientObserver *mClient;
};

} // Tp

Q_DECLARE_METATYPE(Tp::ClientAdaptor*)
Q_DECLARE_METATYPE(Tp::ClientObserverAdaptor*)

#endif //CLIENTADAPTOR_H
