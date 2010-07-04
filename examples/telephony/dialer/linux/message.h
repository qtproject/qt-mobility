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

#ifndef MESSAGE_H
#define MESSAGE_H
 
#include <QtDBus>
#include <QDBusObjectPath>

#define INCOMINGCALL_OBJECT_PATH "/org/freedesktop/Telepathy/Connection/ring/tel/ring/incoming"

class Channels
{
public:
    Channels();
    Channels(const QString& path, const QString &message);
    Channels(const Channels &other);
    Channels& operator=(const Channels &other);
    ~Channels();
 
    friend QDBusArgument &operator<<(QDBusArgument &argument, const Channels &channels);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, Channels &channels);
 
    //register Channels with the Qt type system
    static void registerMetaType();
private:
    void initIncomingCallData();
public:
    QDBusObjectPath m_objPath;
    QVariantMap map;
    QString initiatorID;
};
Q_DECLARE_METATYPE(Channels)

class ChannelsArray
{
public:
    ChannelsArray();
    ChannelsArray(const QString& path, const QString &message);
    ChannelsArray(const ChannelsArray &other);
    ChannelsArray& operator=(const ChannelsArray &other);
    ~ChannelsArray();

    friend QDBusArgument &operator<<(QDBusArgument &argument, const ChannelsArray &channelsarray);
    friend const QDBusArgument &operator>>(const QDBusArgument &argument, ChannelsArray &channelsarray);

    //register Channels with the Qt type system
    static void registerMetaType();
private:
    QList<Channels> channelslist;
};
Q_DECLARE_METATYPE(ChannelsArray)

 
#endif // MESSAGE_H
