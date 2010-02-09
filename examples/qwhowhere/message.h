/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>

// QtMobility API headers
// Messaging
#include <qtmessaging.h>

// Location
#include <qgeopositioninfo.h>

// QtMobility namespace
QTM_USE_NAMESPACE

class Message: public QObject
{
    Q_OBJECT

public:
    Message(QObject *parent = 0);
    ~Message();

    void checkMessages();
    bool sendLocationSMS(QString typeStr, QGeoPositionInfo& position, QString phoneNumber);
    void timerEvent(QTimerEvent *);

private:
    bool accountId(QMessageAccountId& id, const QMessage::Type& type);
    void processIncomingSMS(const QMessageId& id);
    void waitBeforeHandleMessage(const QMessageId& id);

public slots:
    // QMessageService
    void stateChanged(QMessageService::State s);
    void messagesFound(const QMessageIdList &ids);

    // QMessageManager
    void messageAdded(const QMessageId &id,
                      const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageRemoved(const QMessageId &id,
                        const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageUpdated(const QMessageId &id,
                        const QMessageManager::NotificationFilterIdSet &matchingFilterIds);

signals:
    void sendingSMSFailed(int);
    void friendLocationSMSReceived(QString latitude, QString longitude);
    void friendAskLocationSMS(QString friendPhoneNumber);
    void raise();

private:
    QMessageService* m_service;
    QMessageManager* m_manager;
    QMessageManager::NotificationFilterIdSet m_notifFilterSet;
    QMessageId m_messageId;
    int m_tid;
};

#endif // MESSAGE_H
