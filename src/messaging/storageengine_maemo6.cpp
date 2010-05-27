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

#include "storageengine_maemo6_p.h"
#include "telepathyhelpers_maemo6_p.h"
#include "qmessageservice_maemo6_p.h"
#include "qmessagefolder_p.h"
#include "qmessagefolderid_p.h"


Q_GLOBAL_STATIC(StorageEngine, storageEngine);

StorageEngine* StorageEngine::instance()
{
    return storageEngine();
}

StorageEngine::StorageEngine(QMessageService *service, QObject *parent) :
        QObject(parent), m_service(service), m_sync(false),
        m_ready(false), m_state(QMessageService::InactiveState)
{
    QDEBUG_FUNCTION_BEGIN
    m_messagesMap.clear();
    // set the model mode
    m_SMSModel.setQueryMode(CommHistoryModel::AsyncQuery);
    connect(&m_SMSModel, SIGNAL(modelReady()), SLOT(onModelReady()));

    connect(&m_SMSModel, SIGNAL(eventsAdded(const QList<CommHistory::Event> &)),
	    this, SLOT(eventsAdded(const QList<CommHistory::Event> &)));
    connect(&m_SMSModel, SIGNAL(eventsUpdated(const QList<CommHistory::Event> &)),
	    this, SLOT(eventsUpdated(const QList<CommHistory::Event> &)));
    connect(&m_SMSModel, SIGNAL(eventDeleted(int)), this, SLOT(eventDeleted(int)));

    QDEBUG_FUNCTION_END
}

bool StorageEngine::initialize(bool sync)
{
   qDebug() << __PRETTY_FUNCTION__ ;

   // read events from tracker database
   return  updateSMSModel(sync);
}

QMessageService::State StorageEngine::state()
{    
    return m_state;
}

void StorageEngine::setService(QMessageService *service)
{
    // onModelReady() uses m_service to access QMessageServicePrivate
    m_service = service;
}

void StorageEngine::debugMessage(QMessage &message)
{
    // just prints content of main message`s fields
    QDEBUG_FUNCTION_BEGIN
    qDebug() << "id:" << message.id().toString() << "type:" << message.type() << "size:" << message.size() << "status:" << message.status() << "priority:" << message.priority();
    qDebug() << "AccountId:" << message.parentAccountId().toString() << "StantardFolder" << message.standardFolder() << "parenFolderId:" << message.parentFolderId().toString();
    qDebug() << "Date:" << message.date() << "receivedDate:" << message.receivedDate() << "Subject:" << message.subject();
    qDebug() << "From:" << message.from().addressee();
    qDebug() << "To:" << (message.to().isEmpty() ? "**none**" : message.to().first().addressee());
    qDebug() << "Body:" << message.textContent();
    QDEBUG_FUNCTION_END
}

void StorageEngine::debugEvent(Event &event)
{
    QDEBUG_FUNCTION_BEGIN
    // just prints content of main event`s fields
    // TODO: add more fields
    qDebug() << "id: " << event.id() << "contactId" << event.contactId() << "contactName: " << event.contactName();    
    QDEBUG_FUNCTION_END
}

/**
 *
 * Converts message to event.
 *
 */
Event StorageEngine::eventFromMessage(const QMessage &message) const
{
   QDEBUG_FUNCTION_BEGIN
   Event event;

   //event.setDeleted(false);

   switch(message.type())
   {
   case QMessage::Sms:
            event.setType(Event::SMSEvent);
            break;
   case QMessage::InstantMessage:
            event.setType( Event::IMEvent);
            break;
   default:
           qWarning() << __PRETTY_FUNCTION__ << "wrong type";
           return event;
   }

   switch (message.standardFolder())
   {
   case QMessage::InboxFolder:
       event.setParentId(INBOX);
       event.setDirection(Event::Inbound);
       event.setIsDraft(false);
       break;
   case QMessage::OutboxFolder:
       event.setParentId(OUTBOX);
       event.setDirection(Event::Outbound);
       event.setIsDraft(false);
       break;
   case QMessage::SentFolder:
       event.setParentId(SENT);
       event.setDirection(Event::Outbound);
       event.setIsDraft(false);
       break;
   case QMessage::DraftsFolder:
       event.setParentId(DRAFT);
       event.setDirection(Event::Outbound);
       event.setIsDraft(true);
       break;
   default:
       event.setParentId(0);
       event.setDirection(Event::UnknownDirection);
       event.setIsDraft(true);
   }

   // depends on direction
   event.setRemoteUid(event.direction() == Event::Inbound ? message.from().addressee() : MessagingHelper::addressListToString(message.to()));
   event.setLocalUid(event.direction() == Event::Inbound ? MessagingHelper::addressListToString(message.to()) : message.from().addressee());

   // message body
   event.setFreeText(message.textContent());
   /*
     TODO:
   libcommhistory does not process time setting correctly
   event.setStartTime(message.date());
   event.setEndTime(message.receivedDate());
   */
   bool ok = false;
   int eId = message.id().toString().toInt(&ok);

   if (ok || (message.id().toString().length() == 0))
        event.setId(eId);
   else
       qWarning() << __PRETTY_FUNCTION__ << "Cannot convert event ID " << message.id().toString() << " to int";

   QDEBUG_FUNCTION_END
   return event;
}

/**
 *
 * Converts event to message.
 *
 */
QMessage StorageEngine::messageFromEvent(const Event &ev) const
{
    QDEBUG_FUNCTION_BEGIN
    QMessage message;

    if (!ev.isValid())
        return message;

    switch(ev.type())
    {
    case Event::SMSEvent:
             message.setType(QMessage::Sms);
             break;
    case Event::IMEvent:
             message.setType(QMessage::InstantMessage);
             break;
    default:
             message.setType(QMessage::NoType);
            qWarning() << __PRETTY_FUNCTION__ << "wrong type";
    }

    message.setParentAccountId(QMessageAccount::defaultAccount(QMessage::Sms));

    if (ev.isRead()) {
        message.setStatus(QMessage::Read);
    };

    message.setPriority(QMessage::NormalPriority);

    message.setDate(ev.startTime());
    message.setReceivedDate(ev.endTime());

    switch (ev.parentId())
    {
    case INBOX:
        QMessagePrivate::setStandardFolder(message, QMessage:: InboxFolder);
        QMessagePrivate::setParentFolderId(message, QMessageFolderId(FOLDER_ID_INBOX));
        break;
    case OUTBOX:
        QMessagePrivate::setStandardFolder(message, QMessage:: OutboxFolder);
        QMessagePrivate::setParentFolderId(message, QMessageFolderId(FOLDER_ID_OUTBOX));
        break;
    case DRAFT:
        QMessagePrivate::setStandardFolder(message, QMessage:: DraftsFolder);
        QMessagePrivate::setParentFolderId(message, QMessageFolderId(FOLDER_ID_DRAFTS));
        break;
    case SENT:
        QMessagePrivate::setStandardFolder(message, QMessage:: SentFolder);
        QMessagePrivate::setParentFolderId(message, QMessageFolderId(FOLDER_ID_SENT));
        break;
    default:
        QMessagePrivate::setStandardFolder(message, QMessage:: TrashFolder);
        QMessagePrivate::setParentFolderId(message, QMessageFolderId(FOLDER_ID_TRASH));
    }

    if (ev.direction() == Event::Inbound) {
        message.setFrom(QMessageAddress(QMessageAddress::Phone, ev.remoteUid()));
        message.setTo(QMessageAddress(QMessageAddress::Phone, ev.localUid()));
    } else {
        message.setFrom(QMessageAddress(QMessageAddress::Phone, ev.localUid()));
        message.setTo(MessagingHelper::stringToAddressList(ev.remoteUid()));
    }

    message.setBody(QString(ev.freeText()));

    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_id = QMessageId(QString::number(ev.id()));
    privateMessage->_modified = false;   

    QDEBUG_FUNCTION_END
    return message;
}

/**
 *
 * Updates model in synchronous mode if requested by flag updateModel and returns the result of applied filtering.
 *
 */
int StorageEngine::countMessagesSync(const QMessageFilter & filter, bool updateModel)
{
    QDEBUG_FUNCTION_BEGIN
    int count = 0;

    if (updateModel) {
        updateSMSModel(true);
    }

    if (filter.isEmpty())
        count =  m_messagesMap.size();
    else {
        QMessageIdList idList = filterInternalList(filter);
        count = idList.size();
    }

    QDEBUG_FUNCTION_END
    return count;
}

/**
 *
 * Async call to count messages from event`s database. Slot onModelReady() is called when model is updated.
 * Filtering is applied in the slot after model update.
 *
 * \param  filter
 */
bool StorageEngine::countMessages(const QMessageFilter &filter)
{
    QDEBUG_FUNCTION_BEGIN
    bool ret = updateSMSModel();

    if (ret) {
        m_queryType = QueryCount;
        m_filter = filter;
    }
    else
        m_queryType = NoQuery;

    QDEBUG_FUNCTION_END
    return ret;
}

/**
 *
 * Updates model in synchronous mode if requested by flag updateModel and returns the result of applied filtering and searching.
 *
 */
QMessageIdList StorageEngine::queryMessagesSync(const QMessageFilter &filter, const QString &body,
                                  QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder,
                                  uint limit, uint offset, bool updateModel)
{
    QDEBUG_FUNCTION_BEGIN;
    Q_UNUSED(sortOrder);
    Q_UNUSED(limit);
    Q_UNUSED(offset);

    if (updateModel)
        updateSMSModel(true);

    if ((m_ready == true) && (m_state == QMessageService::FinishedState))
    {
        QMessageIdList ids = filterAndSearchInternalList(filter, body, matchFlags);
        // sorting will be applied in void QMessageServicePrivate::messagesFound()
        // limit and offset will be applied in void QMessageServicePrivate::messagesFound()

        QDEBUG_FUNCTION_END
        return ids;
    } else {
        QDEBUG_FUNCTION_END
        return QMessageIdList();
    }
}

/**
 *
 * Updates SMS model and m_messagesMap in async or sync mode depending on flag sync.
 * Does not emit stateChanged() in sync mode.
 *
 */
bool StorageEngine::updateSMSModel(bool sync)
{
    bool ret = false;

    if ((m_state == QMessageService::ActiveState))
        return false;

    ret = m_SMSModel.getEvents();
    if (ret)
        m_state = QMessageService::ActiveState;
    else
        m_state = QMessageService::CanceledState;


    if (ret && sync) {
        m_sync = true;
        m_queryType = NoQuery;
        m_loop.exec();
    }
    else
        emit stateChanged(m_state);

    return ret;
}

/**
 *
 * Async call to query messages from event`s database. Slot onModelReady() is called when model is updated.
 * Filtering and searching are applied in the slot after the whole model update.
 *
 *
 */
bool StorageEngine::queryMessages(const QMessageFilter &filter, const QString &body,
                                  QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder,
                                  uint limit, uint offset)
{
    QDEBUG_FUNCTION_BEGIN            
    bool ret = updateSMSModel();

    if (ret) {
        m_queryType = QueryAndSearch;

        m_filter = filter;
        m_body = body;
        m_matchFlags = matchFlags;
        m_sortOrder = sortOrder;
        m_limit = limit;
        m_offset = offset;
    }
    else
        m_queryType = NoQuery;

    QDEBUG_FUNCTION_END
    return ret;
}

/**
 *
 * Updates model in synchronous mode if requested by flag updateModel and returns the result of applied filtering.
 *
 */
QMessageIdList StorageEngine::queryMessagesSync(const QMessageFilter & filter, const QMessageSortOrder &sortOrder, uint limit, uint offset, bool updateModel)
{
    QDEBUG_FUNCTION_BEGIN;
    Q_UNUSED(sortOrder);
    Q_UNUSED(limit);
    Q_UNUSED(offset);

    if (updateModel)
        updateSMSModel(true);

    if ((m_ready == true) && (m_state != QMessageService::ActiveState))
    {
        QMessageIdList ids = filterInternalList(filter);
        // sorting will be applied in void QMessageServicePrivate::messagesFound()
        // limit and offset will be applied in void QMessageServicePrivate::messagesFound()

        QDEBUG_FUNCTION_END
        return ids;
    } else {
        QDEBUG_FUNCTION_END
        return QMessageIdList();
    }
}

/**
 *
 * Async call to query messages from event`s database. Slot onModelReady() is called when model is updated.
 * Filtering is applied in the slot after the whole model update.
 *
 *
 */
bool StorageEngine::queryMessages(const QMessageFilter & filter, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    QDEBUG_FUNCTION_BEGIN
    bool ret = updateSMSModel();

    if (ret) {
        m_queryType = QueryAndFilter;

        m_filter = filter;
        m_sortOrder = sortOrder;
        m_limit = limit;
        m_offset = offset;
    }
    else
        m_queryType = NoQuery;

    QDEBUG_FUNCTION_END
    return ret;
}

/**
 *
 * Filter and search m_messagesMap. Do not update it. Sync call.
 *
 *
 */
QMessageIdList StorageEngine::filterAndSearchInternalList(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags)
{
    QDEBUG_FUNCTION_BEGIN
    QMessageIdList idList;
    QList<QMessage> messageList = m_messagesMap.values();
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(filter);

    for (int i = 0; i < messageList.size(); ++i) {
        QMessage message = messageList.at(i);

        if (pf->filter(message)) {
            QString text = message.subject();

            if (text.length() >= body.length()) {
                bool found = false;

                if (matchFlags & QMessageDataComparator::MatchCaseSensitive)
                    found = text.contains(body, Qt::CaseSensitive);
                else
                    found = text.contains(body, Qt::CaseInsensitive);

                if (found)
                    idList.append(message.id());
            }
        }
    }

    QDEBUG_FUNCTION_END
    return idList;
}

/**
 *
 * Filter m_messagesMap. Do not update it. Sync call.
 *
 *
 */
QMessageIdList StorageEngine::filterInternalList(const QMessageFilter &filter)
{
    QDEBUG_FUNCTION_BEGIN
    QMessageIdList idList;
    QList<QMessage> messageList = m_messagesMap.values();
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(filter);

    for (int i = 0; i < messageList.size(); ++i) {
        QMessage message = messageList.at(i);

        if (pf->filter(message)) {            
          idList.append(message.id());
        };
    }

    QDEBUG_FUNCTION_END
    return idList;
}

/**
 *
 * Slot that is called when m_SMSModel is updated. Updates m_messagesMap.
 * If model update was caused by async query (queryMessages(), countMessages()), apply filtering and search and pass processing to QMessageServicePrivate.
 * In case of sync queries (queryMessagesSync(), countMessagesSync()) just quits loop m_loop.
 *
 */
void StorageEngine::onModelReady()
{
    QDEBUG_FUNCTION_BEGIN;

    if (m_queryType == AddQuery || m_queryType == RemoveQuery)
    {
        qDebug() << __PRETTY_FUNCTION__ << "m_queryType == AddQuery || m_queryType == RemoveQuery";
        qDebug() << __PRETTY_FUNCTION__ << "m_sync: " << m_sync;
        if (m_sync)
        {
            m_sync = false;
            m_loop.quit();
        }
        return;
    }

    if ( !updateMessagesMap() )
                qWarning() << __PRETTY_FUNCTION__ << "Error in messages map update";

    m_ready = true;
    m_state = QMessageService::FinishedState;

    if (m_queryType != NoQuery)
    {        
        QMessageServicePrivate *p = QMessageServicePrivate::implementation(*m_service);        

        if (!p) {
            qWarning() <<  __PRETTY_FUNCTION__ << "Cannot get QMessageServicePrivate";
            return;
        }

        switch (m_queryType)
        {
        case QueryAndFilter: {
                QMessageIdList ids = queryMessagesSync(m_filter, m_sortOrder, m_limit, m_offset, false);
                p->messagesFound(ids, true, false);
                break;
            }
        case QueryAndSearch: {
                QMessageIdList ids = queryMessagesSync(m_filter, m_body, m_matchFlags, m_sortOrder, m_limit, m_offset, false);
                p->messagesFound(ids, true, false);
                break;
            }
        case QueryCount: {
                int counter = countMessagesSync(m_filter, false);
                p->messagesCounted(counter);
                break;
            }
        case NoQuery:
            break;
        default:
            emit stateChanged(m_state);
        }
    }

    qDebug() << __PRETTY_FUNCTION__ << "m_sync: " << m_sync;
    if (m_sync)
    {
        m_sync = false;
        m_loop.quit();
    }    
    QDEBUG_FUNCTION_END
}

/**
 *
 * Updates m_messagesMap using just updated m_SMSModel.  Sync call.
 *
 */
bool StorageEngine::updateMessagesMap()
{
    QDEBUG_FUNCTION_BEGIN
    bool ret = false;

    int rowsCount = m_SMSModel.rowCount();
    int rowIndex = 0;

    qDebug() << __PRETTY_FUNCTION__ << "rowsCount: " << rowsCount;

    for (rowIndex = 0; rowIndex < rowsCount; rowIndex++)
    {
        QModelIndex modelIndex = m_SMSModel.index(rowIndex, 0);

        if (!modelIndex.isValid())
        {
            qWarning() << __PRETTY_FUNCTION__ << "Invalid index";
            return false;
        }
        else
        {
            Event event = m_SMSModel.event(modelIndex);

            if (!event.isValid())
            {
                qWarning() << __PRETTY_FUNCTION__ << "Invalid event ";
                return false;
            }
            else if (!event.isDeleted()) {
                QMessage message = messageFromEvent(event);

                m_messagesMap.insert(message.id(), message);
                debugMessage(message);
                ret = true;
            }
        }
    }

    QDEBUG_FUNCTION_END
    return ret;
}

/**
 *
 * Returns message from m_messagesMap by id. Sync call.
 *
 */
QMessage StorageEngine::message(const QMessageId& id)
{
    if (id.isValid()) {

        return m_messagesMap.value(id, QMessage());
    }

    return QMessage();
}

/**
 *
 * Removes message m_messagesMap, m_SMSModel and events database by id.  Sync call.
 *
 */
bool StorageEngine::removeMessage(const QMessageId& id)
{
    bool ret = false;

    if (id.isValid())
    {
        bool isConverted = false;
        int iId = id.toString().toInt(&isConverted);
        if (isConverted && (iId > 0 )) {
            ret = m_SMSModel.deleteEvent(iId) && m_messagesMap.remove(id);
            if (!ret)
                qWarning() << __PRETTY_FUNCTION__ << "Cannot removeMessage";
            else
                qDebug() << __PRETTY_FUNCTION__ << "Removed successfully";
        }
    }

   return ret;
}

/**
 *
 * Adds message to  m_messagesMap, m_SMSModel and events database.  Sync call.
 *
 */
bool StorageEngine::addMessage(QMessage &message)
{
    bool ret = false;
    Event event = eventFromMessage(message);

    if (m_SMSModel.addEvent(event , false, true)) {
            QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
            privateMessage->_id = QMessageId(QString::number(event.id()));
            privateMessage->_modified = false;

            m_messagesMap.insert(message.id(), message);
            qDebug() << __PRETTY_FUNCTION__ << "Message added to store. new id = " << message.id().toString();
            ret = true;
    }
    else
        qWarning() << __PRETTY_FUNCTION__ << "Cannot add message";

    return ret;
}

/**
 *
 * Updates existing message data in m_messagesMap, m_SMSModel and tracker database.
 *
 */
bool StorageEngine::updateMessage(QMessage &message)
{
    QDEBUG_FUNCTION_BEGIN
    bool ret = false;
    QMessageId id(message.id());

    if (id.isValid() && m_messagesMap.contains(id)) {
	Event event = eventFromMessage(message);
	ret = m_SMSModel.modifyEvent(event);
	if (ret) {
	    QMessage &mapMessage = m_messagesMap[id];
	    mapMessage = message;
	    QMessagePrivate *p = QMessagePrivate::implementation(mapMessage);
	    p->_modified = false;
	}
    }

    QDEBUG_FUNCTION_END
    return ret;
}

/**
 *
 * Returns instance of QMessageFolder, corresponding to one of standard SMS folders
 *
 */
QMessageFolder StorageEngine::folder(const QMessageFolderId& id)
{
    QMessageFolder folder;
    QString name = id.toString();

    if (name.startsWith(FOLDER_PREFIX_SMS))
    {
       name.remove(0, 4);
       folder = QMessageFolderPrivate::from(id, QMessageAccount::defaultAccount(QMessage::Sms), QMessageFolderId(), name,  QString());
    }

    return folder;
}

/**
 *
 * Returns the constant number od standard SMS folders. Sync call.
 *
 */
int StorageEngine::countFolders(const QMessageFolderFilter& filter)
{
    QMessageFolderIdList result;

    result << QMessageFolderId(FOLDER_ID_INBOX)
           << QMessageFolderId(FOLDER_ID_OUTBOX)
           << QMessageFolderId(FOLDER_ID_DRAFTS)
           << QMessageFolderId(FOLDER_ID_SENT)
           << QMessageFolderId(FOLDER_ID_TRASH);

    MessagingHelper::filterFolders(result, filter);

    return result.size();
}

/**
 *
 * Returns list of standard SMS folders acceptable by filter. Sync call.
 *
 */
QMessageFolderIdList StorageEngine::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset)
{
    Q_UNUSED(sortOrder);
    Q_UNUSED(limit);
    Q_UNUSED(offset);
    QMessageFolderIdList result;

    result << QMessageFolderId(FOLDER_ID_INBOX)
           << QMessageFolderId(FOLDER_ID_OUTBOX)
           << QMessageFolderId(FOLDER_ID_DRAFTS)
           << QMessageFolderId(FOLDER_ID_SENT)
           << QMessageFolderId(FOLDER_ID_TRASH);

    MessagingHelper::filterFolders(result, filter);

    return result;
}


void StorageEngine::eventsAdded(const QList<CommHistory::Event> &events)
{
    processFilters(events, &StorageEngine::messageAdded);
}

void StorageEngine::eventsUpdated(const QList<CommHistory::Event> &events)
{
    processFilters(events, &StorageEngine::messageUpdated);
}

void StorageEngine::eventDeleted(int id)
{
    QMessageId messageId(QString::number(id));
    QMessageManager::NotificationFilterIdSet idSet;

    NotificationFilterMap::const_iterator it = m_filters.begin(), end = m_filters.end();
    for (; it != end; ++it) {
	const QMessageFilter &filter = it.value();
	if (filter.isEmpty())
	    idSet.insert(it.key());
    }

    if (!idSet.isEmpty())
	emit messageRemoved(messageId, idSet);
}

void StorageEngine::registerNotificationFilter(QMessageManager::NotificationFilterId id, const QMessageFilter &filter)
{
    m_filters[id] = filter;
}

void StorageEngine::unregisterNotificationFilter(QMessageManager::NotificationFilterId id)
{
    m_filters.remove(id);
}

void StorageEngine::processFilters(const QList<CommHistory::Event> &events, void (StorageEngine::*signal)(const QMessageId &, const QMessageManager::NotificationFilterIdSet &))
{
    QMap<QMessageId, QMessageManager::NotificationFilterIdSet> matches;
    
    QList<QMessage> messages;
    foreach (const Event &event, events) {
	messages << messageFromEvent(event);
    }

    NotificationFilterMap::const_iterator it = m_filters.begin(), end = m_filters.end();

    for (; it != end; ++it) {
	const QMessageFilter &filter = it.value();
	const QMessageFilterPrivate *pf = QMessageFilterPrivate::implementation(filter);

	foreach (const QMessage &message, messages) {
	    if (filter.isEmpty() || pf->filter(message)) {
		QMessageId id(message.id());
		if (id.isValid())
		    matches[id].insert(it.key());
	    }
	}
    }

    QMap<QMessageId, QMessageManager::NotificationFilterIdSet>::const_iterator mit = matches.begin(), mend = matches.end();
    for ( ; mit != mend; ++mit) {
	emit (this->*signal)(mit.key(), mit.value());
    }
}

