/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include "message.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimerEvent>
#include <QTimer>

Message::Message(QObject *parent) :
        QObject(parent)
{
    m_tid = 0;
    // QMessageService class provides the interface for requesting messaging service operations
    m_service = new QMessageService(this);
    QObject::connect(m_service, SIGNAL(stateChanged(QMessageService::State)), this, SLOT(stateChanged(QMessageService::State)));
    QObject::connect(m_service, SIGNAL(messagesFound(const QMessageIdList&)), this, SLOT(messagesFound(const QMessageIdList&)));

    // QMessageManager class represents the main interface for storage and
    // retrieval of messages, folders and accounts in the system message store
    m_manager = new QMessageManager(this);
    QObject::connect(m_manager, SIGNAL(messageAdded(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)),
                     this, SLOT(messageAdded(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)));
    QObject::connect(m_manager, SIGNAL(messageRemoved(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)),
                     this, SLOT(messageRemoved(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)));
    QObject::connect(m_manager, SIGNAL(messageUpdated(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)),
                     this, SLOT(messageUpdated(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)));

    // Register SMS in inbox folder notificationfilter
    m_notifFilterSet.insert(m_manager->registerNotificationFilter(QMessageFilter::byType(
                                QMessage::Sms)));

#ifdef Q_OS_SYMBIAN
#ifdef __WINS__
    m_notifFilterSet.insert(m_manager->registerNotificationFilter(QMessageFilter::byStandardFolder(
                                QMessage::DraftsFolder)));
#else
    m_notifFilterSet.insert(m_manager->registerNotificationFilter(QMessageFilter::byStandardFolder(QMessage::InboxFolder)));
#endif
#else
    m_notifFilterSet.insert(m_manager->registerNotificationFilter(QMessageFilter::byStandardFolder(QMessage::InboxFolder)));
#endif

}

Message::~Message()
{
}

void Message::waitBeforeHandleMessage(const QMessageId& id)
{
    // We have to sleep a while to let SMS message to be ready for reading
    if (m_tid == 0) {
        m_messageId = id;
        m_tid = startTimer(500);
    }
}

void Message::timerEvent(QTimerEvent* event)
{
    if (m_tid == event->timerId()) {
        this->killTimer(m_tid);
        m_tid = 0;
        processIncomingSMS(m_messageId);
    }
}

void Message::messageAdded(const QMessageId& id,
                           const QMessageManager::NotificationFilterIdSet& matchingFilterIds)
{
    if (matchingFilterIds.contains(m_notifFilterSet))
        waitBeforeHandleMessage(id);
}

void Message::messageRemoved(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)
{
    // None
}

void Message::messageUpdated(const QMessageId&, const QMessageManager::NotificationFilterIdSet&)
{
    // None
}

void Message::checkMessages()
{
    QMessageFilter smsFilter(QMessageFilter::byType(QMessage::Sms));
#ifdef Q_OS_SYMBIAN
#ifdef __WINS__
    QMessageFilter folderFilter(QMessageFilter::byStandardFolder(QMessage::DraftsFolder));
#else
    QMessageFilter folderFilter(QMessageFilter::byStandardFolder(QMessage::InboxFolder));
#endif
#else
    QMessageFilter folderFilter(QMessageFilter::byStandardFolder(QMessage::InboxFolder));
#endif
    m_service->queryMessages(smsFilter & folderFilter);
}

void Message::messagesFound(const QMessageIdList &ids)
{
    foreach(const QMessageId& id, ids) {
        processIncomingSMS(id);
    }
}

void Message::processIncomingSMS(const QMessageId& id)
{
    QMessage message = m_manager->message(id);
    QString txt = message.textContent();

    if (txt.length() > 10) {
        if (txt.mid(0, 4).contains("RES:", Qt::CaseSensitive)) {
            // Your friend send his location to your
            QString s = txt.right(txt.length() - 4);
            int space = s.indexOf(" ");
            QString friend_latitudeStr = s.left(space);
            QString friend_longitudeStr = s.right(space);

            // Application to foreground
            emit raise();

            // SMS friend location received
            emit friendLocationSMSReceived(friend_latitudeStr, friend_longitudeStr);
            // Remove message from inbox
            m_manager->removeMessage(id);
        } else if (txt.mid(0, 4).contains("REQ:", Qt::CaseSensitive)) {
            // Your friend send location request to you
            // and
            // also his location
            QString s = txt.right(txt.length() - 4);
            int space = s.indexOf(" ");
            QString friend_latitudeStr = s.left(space);
            QString friend_longitudeStr = s.right(space + 1);

            // Application to foreground
            emit raise();

            // friend location SMS received
            emit friendLocationSMSReceived(friend_latitudeStr, friend_longitudeStr);

            // Friend ask your location
            QMessageAddress from = message.from();
            if (from.type() == QMessage::Sms) {
                emit friendAskLocationSMS(from.recipient());
            }
            // Remove message from inbox
            m_manager->removeMessage(id);
        }
    }
}

bool Message::accountId(QMessageAccountId& id, const QMessage::Type& type)
{
    bool ret = false;
    foreach(const QMessageAccountId& accountId, m_manager->queryAccounts()) {
        QMessageAccount account(accountId);
        if (account.messageTypes() & type) {
            id = accountId;
            ret = true;
            break;
        }
    }
    return ret;
}

bool Message::sendLocationSMS(QString typeStr, QGeoPositionInfo& position, QString phoneNumber)
{
    // Send SMS
    if (!position.isValid()) {
        return false;
    }

    QGeoCoordinate coordinate = position.coordinate();

    QMessage smsMessage;
    smsMessage.setType(QMessage::Sms);

    // NOTE: finding QMessageAccountId is not necessary on latest QtMobility Messaging module
    QMessageAccountId aId;
    if (!accountId(aId, QMessage::Sms)) {
        return false;
    }
    smsMessage.setParentAccountId(aId);

    smsMessage.setTo(QMessageAddress(QMessageAddress::Phone, phoneNumber));

    QString bodyText;
    bodyText += typeStr;
    bodyText += QString().setNum(coordinate.latitude());
    bodyText += " ";
    bodyText += QString().setNum(coordinate.longitude());
    smsMessage.setBody(bodyText);

    // Send SMS
    return m_service->send(smsMessage);
}

void Message::stateChanged(QMessageService::State s)
{
    emit sendingSMSFailed(s);
}
