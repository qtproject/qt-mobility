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

#include "qmessageglobal.h"
#include "qmessagemanager.h"
#include "qmessageaccount.h"
#include "qmessageaccountid.h"
#include "qmessageaccount_p.h"
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"
#include "qmessageservice.h"
#include "qmessage.h"
#include "qmessagemanager.h"
#include "maemo6helpers_p.h"
#include "qmessagestore_p.h"
#include "qmessage_p.h"
#include "qmessagefilter.h"
#include "qmessagefilter_p.h"
#include "qmessagefolderfilter_p.h"
#include <QThread>
#include <QObject>
#include <QEventLoop>

#include <CommHistory/event.h>
#include "smsmodel_maemo6_p.h"


#define STANDARD_FOLDERS_COUNT  5 //  InboxFolder, OutboxFolder, DraftsFolder,SentFolder, TrashFolder

QTM_USE_NAMESPACE
using namespace RTComCommHistory;


class StorageEngine : public QObject
{
    Q_OBJECT

public:

    enum QueryType {
        NoQuery = 0,
        QueryCount,
        QueryAndFilter,
        QueryAndSearch,
        AddQuery,
        RemoveQuery
    };

    StorageEngine(QMessageService *service = 0, QObject *parent = 0);
    static StorageEngine* instance();
    bool initialize(bool sync = true);
    void setService(QMessageService *service);

    bool updateSMSModel(bool sync = false);
    bool countMessages (const QMessageFilter & filter = QMessageFilter()); // for QMessageService
    int countMessagesSync(const QMessageFilter & filter = QMessageFilter(), bool updateModel = true); // for QMessageManager

    bool queryMessages(const QMessageFilter & filter = QMessageFilter(), const QMessageSortOrder & sortOrder = QMessageSortOrder(),
                       uint limit = 0, uint offset = 0);
    QMessageIdList queryMessagesSync(const QMessageFilter & filter = QMessageFilter(), const QMessageSortOrder & sortOrder = QMessageSortOrder(),
                                     uint limit = 0, uint offset = 0, bool updateModel = true);

    bool queryMessages(const QMessageFilter & filter, const QString & body, QMessageDataComparator::MatchFlags matchFlags = 0, const QMessageSortOrder & sortOrder = QMessageSortOrder(), uint limit = 0, uint offset = 0);
    QMessageIdList queryMessagesSync(const QMessageFilter & filter, const QString & body, QMessageDataComparator::MatchFlags matchFlags = 0,
                                     const QMessageSortOrder & sortOrder = QMessageSortOrder(), uint limit = 0, uint offset = 0, bool updateModel = true);

    QMessage message(const QMessageId& id);    
    bool removeMessage(const QMessageId& id);
    bool addMessage(QMessage &message);
    bool updateMessage(QMessage &message);
    
    QMessageFolder folder(const QMessageFolderId& id);
    int countFolders(const QMessageFolderFilter& filter);
    QMessageFolderIdList queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset);

    QMessage messageFromEvent(const Event &event) const;
    Event eventFromMessage(const QMessage &message) const;

    QMessageService::State state();

    void registerNotificationFilter(QMessageManager::NotificationFilterId, const QMessageFilter &filter);
    void unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId);


signals:
    void messagesCounted(int count);
    void messagesFound(const QMessageIdList &ids);
    void stateChanged(QMessageService::State newState);

    void messageAdded(const QMessageId &id, const QMessageManager::NotificationFilterIdSet & matchingFilterIds);
    void messageRemoved(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &matchingFilterIds);
    void messageUpdated(const QMessageId &id, const QMessageManager::NotificationFilterIdSet & matchingFilterIds);


private slots:
    void onModelReady();
    void eventsAdded(const QList<CommHistory::Event> &events);
    void eventsUpdated(const QList<CommHistory::Event> &events);
    void eventDeleted(int id);

private:
    bool updateMessagesMap();
    QMessageIdList filterInternalList(const QMessageFilter &filter);
    QMessageIdList filterAndSearchInternalList(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags);
    void processFilters(const QList<CommHistory::Event> &events, void (StorageEngine::*signal)(const QMessageId &, const QMessageManager::NotificationFilterIdSet &));

    void debugMessage(QMessage &message);
    void debugEvent(Event &event);

    QMessageService                     *m_service;
    bool                                m_sync;
    bool                                m_ready;
    QMessageService::State              m_state;
    SMSModel                            m_SMSModel;
    QMap<QMessageId,QMessage>           m_messagesMap;
    QEventLoop                          m_loop;
    QueryType                           m_queryType;

    QMessageFilter                      m_filter;
    QString                             m_body;
    QMessageDataComparator::MatchFlags  m_matchFlags;
    QMessageSortOrder                   m_sortOrder;
    uint                                m_limit;
    uint                                m_offset;

    typedef QMap<QMessageManager::NotificationFilterId, QMessageFilter> NotificationFilterMap;
    NotificationFilterMap m_filters;

};


#endif // STORAGEENGINE_H




