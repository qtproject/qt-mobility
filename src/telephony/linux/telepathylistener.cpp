/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "telepathylistener.h"
#include "telepathy.h"
#include "dbusadaptor.h"
#include "dbusinterface.h"
#include "message.h"
#include <QDebug>

#define DBUS_SERVICE "org.freedesktop.Telepathy.Connection.Interface.Requests"
#define DBUS_PATH "/org/freedesktop/Telepathy/Connection/ring/tel/ring"

QTM_BEGIN_NAMESPACE

TelepathyListener::TelepathyListener(QObject* parent) :
        QObject(parent)
        ,ptelepathy(0)
        ,ptelepathyAdaptor(0)
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
    ptelepathy = new Telepathy();
    ptelepathyAdaptor = new TelepathyAdaptor(ptelepathy);
    prequestInterface = new org::freedesktop::Telepathy::Connection::Interface::Requests(DBUS_SERVICE, DBUS_PATH, connection);
    connect(prequestInterface, SIGNAL(NewChannels(ChannelsArray)), SLOT(newChannelsSlot(ChannelsArray)));
}
 
TelepathyListener::~TelepathyListener()
{
    //cleanup
    if (ptelepathyAdaptor)
        delete ptelepathyAdaptor;
    if (ptelepathy)
        delete ptelepathy;
    if(prequestInterface)
        delete prequestInterface;
}

void TelepathyListener::newChannelsSlot(const ChannelsArray& channelsarray)
{
    qDebug() << "TelepathyListener::newChannelsSlot";
    emit NewChannels(channelsarray);
}

#include "moc_telepathylistener.cpp"

QTM_END_NAMESPACE
