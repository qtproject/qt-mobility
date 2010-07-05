/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "dialer.h"
#include "telepathy.h"
#include "dbusadaptor.h"
#include "dbusinterface.h"
#include "message.h"

#define DBUS_SERVICE "org.freedesktop.Telepathy.Connection.Interface.Requests"
#define DBUS_PATH "/org/freedesktop/Telepathy/Connection/ring/tel/ring"


Dialer::Dialer()
    : ptelepathy(0),
    ptelepathyAdaptor(0)
{
    /*
      Register the Channels type first thing, so Qt knows how to handle
      it before an Adaptor/Interface is even constructed.

      It should be ok to register it further down, as long as no
      Channels marshaling or unmarshaling takes place, but this is
      definitely the safest way of doing things.
    */
    Channels::registerMetaType();
    ChannelsArray::registerMetaType();

    QDBusConnection connection = QDBusConnection::sessionBus();
    if (!connection.interface()->isServiceRegistered(DBUS_SERVICE))
    {
        ptelepathy = new Telepathy();
        ptelepathyAdaptor = new TelepathyAdaptor(ptelepathy);

        if (!connection.registerService(DBUS_SERVICE))
        {
            qFatal("Could not register service!");
        }

        if (!connection.registerObject(DBUS_PATH, ptelepathy))
        {
            qFatal("Could not register Telepathy object!");
        }
    }

    prequestInterface = new org::freedesktop::Telepathy::Connection::Interface::Requests(DBUS_SERVICE, DBUS_PATH, connection);

}

Dialer::~Dialer()
{
    if (ptelepathy)
    {
        delete ptelepathy;
    }
    if (ptelepathyAdaptor)
    {
        delete ptelepathyAdaptor;
    }
    if(prequestInterface)
    {
        delete prequestInterface;
    }
}

void Dialer::dial(QString number)
{
    ChannelsArray channelsarray(INCOMINGCALL_OBJECT_PATH, number);
    prequestInterface->createNewChannels(channelsarray);
}
