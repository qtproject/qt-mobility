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

#include "tpsessionchannel_p.h"
#include "telepathyhelpers_maemo6_p.h"


/**
 * \class TpSessionChannel
 * \headerfile <tpsessionchannel.h>
 *
 *
 * When you start chat session or call with your buddy, channel is established with your buddy.
 * TpSessionChannel represents this connection. TpSession account makes automatically channel when
 * you send message to your buddy's address. If you send successive messages to same buddy with
 * TpSessionAccount, it automatically reuses existing connection.
 */
/**
 * \fn void TpSessionChannel::channelReady(TpSessionChannel *);
 *
 * Emitted when the channel becomes ready
 *
 * \param  TpSessionChannel  pointer to channel become ready
 */
/**
 * \fn void TpSessionChannel::channelDestroyed(TpSessionChannel *);
 *
 * Emitted when the channel is destroyed
 *
 * \param  TpSessionChannel  pointer to channel destroyed. The pointer is only for referenc to remove
 * it from some possible places where it could be stored. It is not guaranteed to point any more valid TpSessionChannel object
 */
/**
 * \fn void TpSessionChannel::messageReceived(const Tp::ReceivedMessage &,TpSessionConnection *);
 *
 * Emitted when any of Account Managers recived message
 *
 * \param  Tp::ReceivedMessage  Message received
 * \param  TpSessionChannel  pointer to channel received message
 */
/**
  * \fn void TpSessionChannel::messageSent(const Tp::Message &,Tp::MessageSendingFlags, const QString &,TpSessionChannel *);
  *
  * \param Tp::Message message sent
  */

/**
 * Construct a new TpSessionChannel object. This constructor is called by TpSessionAccount class when
 * new channel is created . It is not inended to be used stand alone
 * This varient with connection and contact as parameter is intented for creationg new connection from origginator side to your peer
 *
 *
 * \param conn connection where this channel is created
 * \param contact  Contacto to your peer to establish channel
 */


TpSessionChannel::TpSessionChannel(Tp::ConnectionPtr conn, const Tp::ContactPtr &contact)
{
    QDEBUG_FUNCTION_BEGIN
    QVariantMap request;
    qDebug() << "TpSessionChannel::TpSessionChannel" << "contact.id() " << contact->id();

    request.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".ChannelType"),
                   TELEPATHY_INTERFACE_CHANNEL_TYPE_TEXT);
    request.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandleType"),
                   (uint) Tp::HandleTypeContact);
    request.insert(QLatin1String(TELEPATHY_INTERFACE_CHANNEL ".TargetHandle"),
                   contact->handle()[0]);

    connect(conn->ensureChannel(request),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onChannelCreated(Tp::PendingOperation*)));

    peerContact = contact;

    QDEBUG_FUNCTION_END
}

/**
 * Construct a new TpSessionChannel object. This constructor is called by TpSessionAccount class when
 * new channel is created . It is not inended to be used stand alone
 * This varient with connection only parameter is intented for receiving new connection from your peer
 *
 *
 * \param conn connection where this channel is created
 */


TpSessionChannel::TpSessionChannel(Tp::TextChannelPtr ch)
{
    QDEBUG_FUNCTION_BEGIN

    qDebug() << "TpSessionChannel::TpSessionChannel" << "path " << ch->objectPath();
    channel = ch;
    connect(channel->becomeReady(Tp::TextChannel::FeatureMessageQueue | Tp::TextChannel::FeatureMessageSentSignal),
            SIGNAL(finished(Tp::PendingOperation *)),
            SLOT(onChannelReady(Tp::PendingOperation *)));

    QDEBUG_FUNCTION_END
}

void TpSessionChannel::onChannelCreated(Tp::PendingOperation *op)
{
    QDEBUG_FUNCTION_BEGIN
    qDebug() << "TpSessionChannel::onOutgoingChannelCreated" ;
    if (op->isError()) {
        qWarning() << "Connection cannot become connected" ;
        return;
    }
    Tp::PendingChannel *pc = qobject_cast<Tp::PendingChannel *>(op);

    channel = Tp::TextChannel::create(pc->connection(), pc->objectPath(), pc->immutableProperties());

    connect(channel->becomeReady(Tp::TextChannel::FeatureMessageQueue | Tp::TextChannel::FeatureMessageSentSignal),
            SIGNAL(finished(Tp::PendingOperation*)),
            SLOT(onChannelReady(Tp::PendingOperation*)));

    QDEBUG_FUNCTION_END
}

void TpSessionChannel::onChannelReady(Tp::PendingOperation *op)
{
    QDEBUG_FUNCTION_BEGIN
    Q_UNUSED(op);

    qDebug() << "TpSessionChannel::onChannelReady type=" << channel->channelType() << "path " << channel->objectPath() <<
    "initiatorContact=" << (channel->initiatorContact() ? channel->initiatorContact()->id() : "NULL") ;
    ;
    connect(channel.data(),
            SIGNAL(messageReceived(const Tp::ReceivedMessage &)),
            SLOT(onMessageReceived(const Tp::ReceivedMessage &)));
    connect(channel.data(),
            SIGNAL(messageSent(const Tp::Message &, Tp::MessageSendingFlags, const QString &)),
            SLOT(onMessageSent(const Tp::Message &, Tp::MessageSendingFlags, const QString &)));
    connect(channel.data(), SIGNAL(destroyed(QObject *)), SLOT(onChannelDestroyed(QObject *)));
    emit channelReady(this);
    peerContact = channel->initiatorContact();

    QList<Tp::ReceivedMessage> queuedMessages = channel->messageQueue();

    foreach (const Tp::ReceivedMessage &message, queuedMessages) {
        qDebug()  << "received " << message.text();
        emit messageReceived(message, this);
    }

    QDEBUG_FUNCTION_END
}

/**
 *  Send message to to ths channel
 *
 *
 * \param message   message to send
 */

bool TpSessionChannel::sendMessage(const QString &message)
{
    QDEBUG_FUNCTION_BEGIN
    bool retVal = !channel->send(message)->isError();
    QDEBUG_FUNCTION_END
    return retVal;	
}

void TpSessionChannel::onMessageReceived(const Tp::ReceivedMessage &msg)
{
    QDEBUG_FUNCTION_BEGIN
    qDebug() << "TpSessionChannel::onMessageReceived " << msg.text();
    emit messageReceived(msg, this);
    QDEBUG_FUNCTION_END
};

void TpSessionChannel::onMessageSent(const Tp::Message &msg, Tp::MessageSendingFlags sflags, const QString &flags)
{
    QDEBUG_FUNCTION_BEGIN

    qDebug() << "TpSessionChannel::onMessageSent";
    emit messageSent(msg, sflags, flags, this);

    QDEBUG_FUNCTION_END
};

/**
 *  Get id ( address of your peer )
 *
 *
 * \returns your peer id/address or empty QString
 */
QString TpSessionChannel::peerId() const
{
    QDEBUG_FUNCTION_BEGIN
    return peerContact ? peerContact->id() : "";
    QDEBUG_FUNCTION_END
}

void TpSessionChannel::onChannelDestroyed(QObject *obj)
{
    QDEBUG_FUNCTION_BEGIN
    Q_UNUSED(obj);

    qDebug() << "TpSessionChannel::onChannelDestroyed";
    //TpSessionChannel *call = (TpSessionChannel *) obj;
    emit channelDestroyed(this);
    QDEBUG_FUNCTION_END
}

