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

#include "message.h"
 
Channels::Channels() :
        m_objPath("/")
        , initiatorID("")
{
    map.clear();
}
 
Channels::Channels(const QString &path, const QString &text) :
        m_objPath(path)
        , initiatorID(text)
{
    if(path == INCOMINGCALL_OBJECT_PATH)
        m_objPath.setPath(path + text);
    initIncomingCallData();
}
 
Channels::Channels(const Channels &other) :
        m_objPath(other.m_objPath)
{
    map = other.map;
}
 
Channels& Channels::operator=(const Channels &other)
{
    m_objPath = other.m_objPath;
    map = other.map;
    return *this;
}
 
Channels::~Channels()
{
}
 
void Channels::registerMetaType()
{
    qRegisterMetaType<Channels>("Channels");
 
    qDBusRegisterMetaType<Channels>();
}

void Channels::initIncomingCallData()
{
    map.empty();
    if(m_objPath.path().indexOf(INCOMINGCALL_OBJECT_PATH) == 0)
    {
        map.insert("org.freedesktop.Telepathy.Channel.InitiatorID", QVariant(initiatorID));
        map.insert("org.freedesktop.Telepathy.Channel.TargetHandleType", QVariant((uint)(1)));
        map.insert("org.freedesktop.Telepathy.Channel.TargetHandle", QVariant((uint)(6)));
        map.insert("org.freedesktop.Telepathy.Channel.TargetID", QVariant(initiatorID));
        map.insert("org.freedesktop.Telepathy.Channel.Requested", QVariant(false));
        map.insert("org.freedesktop.Telepathy.Channel.Type.StreamedMedia.ImmutableStreams", QVariant(true));
        map.insert("org.freedesktop.Telepathy.Channel.ChannelType", QVariant("org.freedesktop.Telepathy.Channel.Type.StreamedMedia"));
        map.insert("org.freedesktop.Telepathy.Channel.InitiatorHandle", QVariant((uint)(6)));

        QStringList list;
        list << "org.freedesktop.Telepathy.Channel.Interface.DTMF";
        list << "org.freedesktop.Telepathy.Channel.Interface.Hold";
        list << "com.Nokia.Telepathy.Channel.Interface.DialStrings";
        list << "org.freedesktop.Telepathy.Channel.Interface.Group";
        list << "org.freedesktop.Telepathy.Channel.Interface.CallState";
        list << "com.nokia.Telepathy.Channel.Interface.Emergency";
        list << "com.nokia.Telepathy.Channel.Interface.ConferenceMember";

        map.insert("org.freedesktop.Telepathy.Channel.Interfaces", QVariant(list));

        map.insert("org.freedesktop.Telepathy.Channel.Type.StreamedMedia.InitialAudio", QVariant(true));
        map.insert("org.freedesktop.Telepathy.Channel.Type.StreamedMedia.InitialVideo", QVariant(false));
    }
}

QDBusArgument &operator<<(QDBusArgument &argument, const Channels& channels)
{
    argument.beginStructure();
    argument << channels.m_objPath;
    //argument << channels.m_path;
    //argument << channels.m_text;
    argument << channels.map;
    argument.endStructure();

    return argument;
}
 
const QDBusArgument &operator>>(const QDBusArgument &argument, Channels &channels)
{
    argument.beginStructure();
    argument >> channels.m_objPath;
    //argument >> channels.m_path;
    //argument >> channels.m_text;
    argument >> channels.map;
    argument.endStructure();

    return argument;
}


ChannelsArray::ChannelsArray()
{
}

ChannelsArray::ChannelsArray(const ChannelsArray& other)
{
    channelslist.clear();
    channelslist.append((other.channelslist));
}

ChannelsArray::ChannelsArray(const QString& path, const QString &message)
{
    channelslist.clear();
    Channels chs(path, message);
    channelslist.push_back(chs);
}

ChannelsArray& ChannelsArray::operator=(const ChannelsArray& other)
{
    channelslist.clear();
    channelslist.append((other.channelslist));
    return *this;
}

ChannelsArray::~ChannelsArray()
{
}

void ChannelsArray::registerMetaType()
{
    qRegisterMetaType<ChannelsArray>("ChannelsArray");

    qDBusRegisterMetaType<ChannelsArray>();
}

QDBusArgument &operator<<(QDBusArgument &argument, const ChannelsArray& channelsarray)
{
    argument.beginArray(qMetaTypeId<Channels>());
    for(int i = 0; i < channelsarray.channelslist.length(); ++i)
        argument << channelsarray.channelslist.at(i);

    argument.endArray();

    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, ChannelsArray &channelsarray)
{
    argument.beginArray();
    channelsarray.channelslist.clear();
    while(!argument.atEnd()){
        Channels chs;
        argument >> chs;
        channelsarray.channelslist.append(chs);
    }
    argument.endArray();

    return argument;
}
