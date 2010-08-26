/*
 * This file is part of TelepathyQt4
 *
 * Copyright (C) 2008 Collabora Ltd. <http://www.collabora.co.uk/>
 * Copyright (C) 2008 Nokia Corporation
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

#ifndef _TelepathyQt4_types_h_HEADER_GUARD_
#define _TelepathyQt4_types_h_HEADER_GUARD_


#include "maemo/interfaces/cli-types.h"
#include "maemo/shared-ptr.h"

#include <QtDBus/QDBusVariant>
#include <QtCore/QSharedPointer>

namespace Tp
{
void registerTypes();

class AbstractClient;
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

typedef SharedPtr<AbstractClient> AbstractClientPtr;
typedef SharedPtr<Channel> ChannelPtr;
typedef SharedPtr<ChannelDispatchOperation> ChannelDispatchOperationPtr;
typedef SharedPtr<ChannelRequest> ChannelRequestPtr;
typedef SharedPtr<ClientObject> ClientObjectPtr;
typedef SharedPtr<ClientRegistrar> ClientRegistrarPtr;
typedef SharedPtr<Connection> ConnectionPtr;
typedef SharedPtr<FileTransferChannel> FileTransferChannelPtr;
typedef SharedPtr<IncomingFileTransferChannel> IncomingFileTransferChannelPtr;
typedef SharedPtr<OutgoingFileTransferChannel> OutgoingFileTransferChannelPtr;
typedef SharedPtr<RoomListChannel> RoomListChannelPtr;
typedef SharedPtr<MediaContent> MediaContentPtr;
typedef SharedPtr<MediaStream> MediaStreamPtr;
typedef SharedPtr<StreamedMediaChannel> StreamedMediaChannelPtr;
typedef SharedPtr<TextChannel> TextChannelPtr;

} // Tp

#endif
