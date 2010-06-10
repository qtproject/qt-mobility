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

#ifndef STORAGEENGINE_H
#define STORAGEENGINE_H

#include <QObject>
#include <QEventLoop>
#include <QMap>

#include <CommHistory/event.h>
#include <messaging-service/messagingif.h>
#include "smsmodel_maemo6_p.h"

#include "qmessage.h"
#include "qmessagefilter.h"
#include "qmessagesortorder.h"
#include "qmessageservice.h"

#define STANDARD_FOLDERS_COUNT  5 //  InboxFolder, OutboxFolder, DraftsFolder,SentFolder, TrashFolder

QTM_USE_NAMESPACE
using namespace RTComCommHistory;

class StorageEngine : public QObject
{
    Q_OBJECT

public:

    explicit StorageEngine(QObject *parent = 0);
    static StorageEngine* instance();

    int countMessagesSync(const QMessageFilter &filter = QMessageFilter());
    QMessageIdList queryMessagesSync(const QMessageFilter &filter = QMessageFilter(), const QMessageSortOrder &sortOrder = QMessageSortOrder(),
				     uint limit = 0, uint offset = 0);
    QMessageIdList queryMessagesSync(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags = 0,
				     const QMessageSortOrder &sortOrder = QMessageSortOrder(), uint limit = 0, uint offset = 0);


    bool countMessages(QMessageService *service, const QMessageFilter &filter = QMessageFilter());
    bool queryMessages(QMessageService *service, const QMessageFilter &filter = QMessageFilter(), const QMessageSortOrder &sortOrder = QMessageSortOrder(),
		       uint limit = 0, uint offset = 0);
    bool queryMessages(QMessageService *service, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags = 0,
		       const QMessageSortOrder &sortOrder = QMessageSortOrder(), uint limit = 0, uint offset = 0);

    QMessage message(const QMessageId &id) const;    
    bool removeMessage(const QMessageId &id);
    bool addMessage(QMessage &message);
    bool updateMessage(QMessage &message);

    bool compose(const QMessage & message);
    bool show(const QMessageId &id);

    QMessageFolder folder(const QMessageFolderId &id);
    int countFolders(const QMessageFolderFilter &filter);
    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset);

    QMessageManager::Error error() const;

    void registerNotificationFilter(QMessageManager::NotificationFilterId, const QMessageFilter &filter);
    void unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId);

    static QMessage messageFromEvent(const Event &event);
    static Event eventFromMessage(const QMessage &message);

signals:
    void messageAdded(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageRemoved(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageUpdated(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);

private slots:
    void onModelReady();
    void eventsAdded(const QList<CommHistory::Event> &events);
    void eventsUpdated(const QList<CommHistory::Event> &events);
    void eventDeleted(int id);

private:

    QMessageIdList filterInternalList(const QMessageFilter &filter);
    QMessageIdList filterAndSearchInternalList(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags);
    void processFilters(const QList<CommHistory::Event> &events, void (StorageEngine::*signal)(const QMessageId &, const QMessageManager::NotificationFilterIdSet &));

    template <typename Func>
    void foreachEvent(Func &f) const;

    MessagingIf  *m_pMessagingIf;
    bool m_sync;
    bool m_ready;
    
    mutable QMessageManager::Error m_error;
    
    SMSModel m_SMSModel;
    QEventLoop m_loop;

    typedef QMap<QMessageManager::NotificationFilterId, QMessageFilter> NotificationFilterMap;
    NotificationFilterMap m_filters;

};

template <typename Func>
void StorageEngine::foreachEvent(Func &f) const
{
    int rowCount = m_SMSModel.rowCount();
    for (int row = 0; row < rowCount; ++row) {
        QModelIndex index = m_SMSModel.index(row, 0);
	if (index.isValid()) {
	    const Event event = m_SMSModel.event(index);
	    if (!event.isDeleted()) {
		f(event);
	    }
	}
    }
}

class ServiceQuery : public QObject
{
    Q_OBJECT

public:

    enum QueryType 
    {
	CountQuery = 0,
	MessageQuery,
	MessageAndBodySearchQuery
    };

    ServiceQuery(QMessageService *service, const QMessageFilter &filter);
    ServiceQuery(QMessageService *service, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset);
    ServiceQuery(QMessageService *service, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, 
		 const QMessageSortOrder &sortOrder, uint limit, uint offset);
    ~ServiceQuery();
											  
public slots:
    void doQuery();

private slots:
    void sortMessages();
    void searchBody();
    void completed();
 
private:

    const QueryType _type;
    QMessageService *_service;

    QMessageFilter _filter;
    QString _body;
    QMessageDataComparator::MatchFlags _matchFlags;
    QMessageSortOrder _sortOrder;
    uint _limit;
    uint _offset;

    int _count;
    QMessageIdList _ids;
};


#endif // STORAGEENGINE_H




