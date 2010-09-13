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

#ifndef MESSAGE_H
#define MESSAGE_H
 
#include "qmobilityglobal.h"
#include <QtDBus>
#include <QDBusObjectPath>

#define INCOMINGCALL_OBJECT_PATH "/org/freedesktop/Telepathy/Connection/ring/tel/ring/incoming"


QT_BEGIN_HEADER

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
public:
    QList<Channels> channelslist;
};

Q_DECLARE_METATYPE(ChannelsArray)

QT_END_HEADER

#endif // MESSAGE_H
