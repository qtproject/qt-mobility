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

#ifndef TYPES_H
#define TYPES_H

#include <QtDBus/QDBusVariant>

#include "maemo/sharedpointer.h"
#include "maemo/interfaces/itypes.h"

namespace DBus
{
    void registerTypes();

    class ClientBase;
    class Channel;
    class ChannelDispatchOperation;
    class ChannelRequest;
    class ClientObject;
    class ClientRegistrar;
    class Connection;
    class FileTransferChannel;
    class IncomingFileTransferChannel;
    class OutgoingFileTransferChannel;
    class RoomListChannel;
    class MediaContent;
    class MediaStream;
    class StreamedMediaChannel;
    class TextChannel;

    struct SUSocketAddress
    {
        QString address;
        uint port;
    };

    bool operator==(const SUSocketAddress& v1, const SUSocketAddress& v2);
    inline bool operator!=(const SUSocketAddress& v1, const SUSocketAddress& v2)
    {
        return !operator==(v1, v2);
    }
    QDBusArgument& operator<<(QDBusArgument& arg, const SUSocketAddress& val);
    const QDBusArgument& operator>>(const QDBusArgument& arg, SUSocketAddress& val);

    typedef SharedPointer<ClientBase> ClientBasePtr;
    typedef SharedPointer<Channel> ChannelPtr;
    typedef SharedPointer<ChannelDispatchOperation> ChannelDispatchOperationPtr;
    typedef SharedPointer<ChannelRequest> ChannelRequestPtr;
    typedef SharedPointer<ClientObject> ClientObjectPtr;
    typedef SharedPointer<ClientRegistrar> ClientRegistrarPtr;
    typedef SharedPointer<Connection> ConnectionPtr;
    typedef SharedPointer<FileTransferChannel> FileTransferChannelPtr;
    typedef SharedPointer<IncomingFileTransferChannel> IncomingFileTransferChannelPtr;
    typedef SharedPointer<OutgoingFileTransferChannel> OutgoingFileTransferChannelPtr;
    typedef SharedPointer<RoomListChannel> RoomListChannelPtr;
    typedef SharedPointer<MediaContent> MediaContentPtr;
    typedef SharedPointer<MediaStream> MediaStreamPtr;
    typedef SharedPointer<StreamedMediaChannel> StreamedMediaChannelPtr;
    typedef SharedPointer<TextChannel> TextChannelPtr;

} // DBus

Q_DECLARE_METATYPE(DBus::SUSocketAddress)

#endif//TYPES_H
