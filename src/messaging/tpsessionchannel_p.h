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

#ifndef TPSESSIONCHANNEL_H
#define TPSESSIONCHANNEL_H

#include <QObject>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/Message>
#include <TelepathyQt4/PendingChannel>
#include <TelepathyQt4/ChannelRequest>
#include <TelepathyQt4/Channel>
#include <TelepathyQt4/TextChannel>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/Connection>

class TpSessionChannel : public QObject
{
    Q_OBJECT
public:
    TpSessionChannel(Tp::TextChannelPtr);
    TpSessionChannel(Tp::ConnectionPtr conn, const Tp::ContactPtr &contact);
    bool sendMessage(const QString &message);
    QString peerId() const;
signals:
    void channelReady(TpSessionChannel *);
    void channelDestroyed(TpSessionChannel *);
    void messageReceived(const Tp::ReceivedMessage &, TpSessionChannel *);
    void messageSent(const Tp::Message &, Tp::MessageSendingFlags, const QString &, TpSessionChannel *);
public slots:
    void onChannelCreated(Tp::PendingOperation *op);
    void onChannelReady(Tp::PendingOperation *op);
    void onChannelDestroyed(QObject *);
    void onMessageReceived(const Tp::ReceivedMessage &);
    void onMessageSent(const Tp::Message &, Tp::MessageSendingFlags, const QString &);
public:
    Tp::ContactPtr peerContact;
    Tp::TextChannelPtr channel;
};

#endif // TPSESSIONCHANNEL_H
