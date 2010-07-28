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
