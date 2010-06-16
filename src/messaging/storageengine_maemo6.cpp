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
#include "maemo6helpers_p.h"

#include "qmessageservice_maemo6_p.h"
#include "qmessagefolder_p.h"
#include "qmessagefolderid_p.h"
#include "qmessagefilter_p.h"
#include "qmessage_p.h"

#include <QTimer>

namespace 
{

QString prefixForEvent(const Event &event)
{
    QString prefix;

    switch(event.type())
    {
    case Event::SMSEvent:
             prefix = "SMS_";
             break;
    case Event::IMEvent:
             prefix = "IMS_";
             break;
    default:
            prefix = "";
            qWarning() << __PRETTY_FUNCTION__ << "wrong type";
    }

    return prefix;
}

struct MessageCounter
{
    MessageCounter(const QMessageFilter &filter)
	: _filter(filter)
	, _count(0)
     {
	 _privateFilter = QMessageFilterPrivate::implementation(_filter); 
     }

    void operator()(const Event &event)
    {
	if (_filter.isEmpty()) {
	    _count++;
	} else {
	    const QMessage message = StorageEngine::messageFromEvent(event);
	    if (_privateFilter->filter(message)) {
		_count++;
	    }
	}
    }

    QMessageFilter _filter;
    const QMessageFilterPrivate *_privateFilter;
    int _count;
};

struct MessageFilter
{
    MessageFilter(const QMessageFilter &filter)
	: _filter(filter)
     {
	 _privateFilter = QMessageFilterPrivate::implementation(_filter); 
     }

    void operator()(const Event &event)
    {
	if (_filter.isEmpty()) {
            _ids << QMessageId(prefixForEvent(event) + QString::number(event.id()));
	} else {
	    const QMessage message = StorageEngine::messageFromEvent(event);
	    if (_privateFilter->filter(message)) {
		_ids << message.id();
	    }
	}
    }

    QMessageFilter _filter;
    const QMessageFilterPrivate *_privateFilter;
    QMessageIdList _ids;
};

struct MessageFilterAndBodySearcher
{
    MessageFilterAndBodySearcher(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchflags) 
	: _filter(filter)
	, _body(body)
	, _matchFlags(matchflags)
     {
	 _privateFilter = QMessageFilterPrivate::implementation(_filter); 
     }

    void operator()(const Event &event)
    {
	const QMessage message = StorageEngine::messageFromEvent(event);
        if (_privateFilter->filter(message)) {
            const QString text = message.textContent();

            if (text.length() >= _body.length()) {
                bool found = false;

                if (_matchFlags & QMessageDataComparator::MatchCaseSensitive)
                    found = text.contains(_body, Qt::CaseSensitive);
                else
                    found = text.contains(_body, Qt::CaseInsensitive);

                if (found)
                    _ids << message.id();
            }
        }
    }

    QMessageFilter _filter;
    QString _body;
    QMessageDataComparator::MatchFlags _matchFlags;
    const QMessageFilterPrivate *_privateFilter;
    QMessageIdList _ids;
};

}

Q_GLOBAL_STATIC(StorageEngine, storageEngine);

StorageEngine* StorageEngine::instance()
{
    return storageEngine();
}

StorageEngine::StorageEngine(QObject *parent) 
	: QObject(parent)
	, m_sync(true)
        , m_ready(false)
	, m_error(QMessageManager::NoError)
{
    QDEBUG_FUNCTION_BEGIN

    m_SMSModel.setQueryMode(CommHistoryModel::AsyncQuery);
    connect(&m_SMSModel, SIGNAL(modelReady()), SLOT(onModelReady()));

    connect(&m_SMSModel, SIGNAL(eventsAdded(const QList<CommHistory::Event> &)),
	    this, SLOT(eventsAdded(const QList<CommHistory::Event> &)));
    connect(&m_SMSModel, SIGNAL(eventsUpdated(const QList<CommHistory::Event> &)),
	    this, SLOT(eventsUpdated(const QList<CommHistory::Event> &)));
    connect(&m_SMSModel, SIGNAL(eventDeleted(int)), this, SLOT(eventDeleted(int)));
    
    if (m_SMSModel.getEvents() && m_sync)
	m_loop.exec();
    
    QDEBUG_FUNCTION_END
}

/**
 *
 * Converts message to event.
 *
 */
Event StorageEngine::eventFromMessage(const QMessage &message)
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
   event.setIsRead(message.status() & QMessage::Read);

   bool ok = false;
   int eId = message.id().toString().mid(4).toInt(&ok);

   if (!ok)
       eId = message.id().toString().toInt(&ok);

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
QMessage StorageEngine::messageFromEvent(const Event &ev)
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

    QMessage::StatusFlags status(0);
    if (ev.isRead()) {
	status |= QMessage::Read;
    }
    if (ev.isDeleted()) {
	status |= QMessage::Removed;
    }
    
    message.setStatus(status);
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

    privateMessage->_id = QMessageId(prefixForEvent(ev) + QString::number(ev.id()));
    privateMessage->_modified = false;   

    QDEBUG_FUNCTION_END
    return message;
}

QMessageManager::Error StorageEngine::error() const
{
    return m_error;
}

int StorageEngine::countMessagesSync(const QMessageFilter &filter)
{
    QDEBUG_FUNCTION_BEGIN
	
    m_error = QMessageManager::NoError;

    QMessageFilterPrivate *pf = QMessageFilterPrivate::implementation(filter);
    if (pf->_field == QMessageFilterPrivate::None && 
	pf->_filterList.count() == 0 &&
	pf->_notFilter) {
	return 0;
    }

    MessageCounter counter(filter);
    foreachEvent<MessageCounter>(counter);

    QDEBUG_FUNCTION_END
    return counter._count;
}

QMessageIdList StorageEngine::queryMessagesSync(const QMessageFilter &filter, const QString &body,
						QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder,
						uint limit, uint offset)
{
    QDEBUG_FUNCTION_BEGIN;
    Q_UNUSED(sortOrder);
    Q_UNUSED(limit);
    Q_UNUSED(offset);

    m_error = QMessageManager::NoError;

    QMessageFilterPrivate *pf = QMessageFilterPrivate::implementation(filter);
    if (pf->_field == QMessageFilterPrivate::None && 
	pf->_filterList.count() == 0 &&
	pf->_notFilter) {
	return QMessageIdList();
    }

    MessageFilterAndBodySearcher searcher(filter, body, matchFlags);
    foreachEvent<MessageFilterAndBodySearcher>(searcher);

    QDEBUG_FUNCTION_END

    return searcher._ids;
}

QMessageIdList StorageEngine::queryMessagesSync(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    QDEBUG_FUNCTION_BEGIN
    Q_UNUSED(sortOrder);
    Q_UNUSED(limit);
    Q_UNUSED(offset);

    m_error = QMessageManager::NoError;

    QMessageFilterPrivate *pf = QMessageFilterPrivate::implementation(filter);
    if (pf->_field == QMessageFilterPrivate::None && 
	pf->_filterList.count() == 0 &&
	pf->_notFilter) {
	return QMessageIdList();
    }
    
    MessageFilter searcher(filter);
    foreachEvent<MessageFilter>(searcher);
    
    QDEBUG_FUNCTION_END

    return searcher._ids;
}

bool StorageEngine::countMessages(QMessageService *service, const QMessageFilter &filter)
{
    QDEBUG_FUNCTION_BEGIN

    m_error = QMessageManager::NoError;    

    ServiceQuery *query = new ServiceQuery(service, filter);
    QTimer::singleShot(0, query, SLOT(doQuery()));

    QDEBUG_FUNCTION_END
    return true;
}

bool StorageEngine::queryMessages(QMessageService *service, const QMessageFilter &filter, const QString &body,
				  QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder,
				  uint limit, uint offset)
{
    QDEBUG_FUNCTION_BEGIN;

    m_error = QMessageManager::NoError;

    ServiceQuery *query = new ServiceQuery(service, filter, body, matchFlags, 
					   sortOrder, limit, offset);
    QTimer::singleShot(0, query, SLOT(doQuery()));

    QDEBUG_FUNCTION_END
    return true;
}

bool StorageEngine::queryMessages(QMessageService *service, const QMessageFilter &filter, 
				  const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    QDEBUG_FUNCTION_BEGIN

    m_error = QMessageManager::NoError;
	
    ServiceQuery *query = new ServiceQuery(service, filter, sortOrder, limit, offset);
    QTimer::singleShot(0, query, SLOT(doQuery()));

    QDEBUG_FUNCTION_END
    return true;
}

/**
 * Slot that is called when m_SMSModel is ready.
 */
void StorageEngine::onModelReady()
{
    QDEBUG_FUNCTION_BEGIN;

    m_ready = true;
    if (m_sync) {
        m_sync = false;
        m_loop.quit();
    }    

    QDEBUG_FUNCTION_END
}

/**
 *
 * Returns message from m_SMSModel by id. Sync call.
 *
 */
QMessage StorageEngine::message(const QMessageId &id) const
{
    if (id.isValid()) {
	m_error = QMessageManager::NoError;
        int eventId = id.toString().mid(4).toInt();
	QModelIndex index = m_SMSModel.findEvent(eventId);
	if (index.isValid()) {
	    const Event event = m_SMSModel.event(index);
	    return messageFromEvent(event);
	}
    } else {
	m_error = QMessageManager::InvalidId;
    }

    return QMessage();
}

/**
 *
 * Removes message m_SMSModel and events database by id. Sync call.
 *
 */
bool StorageEngine::removeMessage(const QMessageId &id)
{
    QDEBUG_FUNCTION_BEGIN
    bool ret = false;

    if (id.isValid()) {
        bool isConverted = false;
        int iId = id.toString().mid(4).toInt(&isConverted);
        if (isConverted && iId > 0) {
            ret = m_SMSModel.deleteEvent(iId);
            if (ret) {
		m_error = QMessageManager::NoError;
		qDebug() << __PRETTY_FUNCTION__ << "Removed successfully";
            } else {
		m_error = QMessageManager::FrameworkFault;
                qWarning() << __PRETTY_FUNCTION__ << "Cannot removeMessage";             
	    }
        } else {
            qWarning() << __PRETTY_FUNCTION__ << "Invalid ID";
	    m_error = QMessageManager::InvalidId;
	}
    } else {	
        qWarning() << __PRETTY_FUNCTION__ << "Invalid ID";
	m_error = QMessageManager::InvalidId;
    }

   QDEBUG_FUNCTION_END
   return ret;
}

/**
 *
 * Adds message to m_SMSModel and events database.  Sync call.
 *
 */
bool StorageEngine::addMessage(QMessage &message)
{
    bool ret = false;
    Event event = eventFromMessage(message);

    if (m_SMSModel.addEvent(event)) {
	QMessagePrivate *privateMessage = QMessagePrivate::implementation(message);
        privateMessage->_id = QMessageId(prefixForEvent(event) + QString::number(event.id()));
	privateMessage->_modified = false;
	qDebug() << __PRETTY_FUNCTION__ << "Message added to store. new id = " << message.id().toString();
	m_error = QMessageManager::NoError;
	ret = true;
    }  else {
        qWarning() << __PRETTY_FUNCTION__ << "Cannot add message";
	m_error = QMessageManager::FrameworkFault;
    }
    return ret;
}

/**
 *
 * Updates existing message data in m_SMSModel and tracker database.
 *
 */
bool StorageEngine::updateMessage(QMessage &message)
{
    QDEBUG_FUNCTION_BEGIN
    bool ret = false;
    QMessageId id(message.id());

    if (id.isValid()) {
	Event event = eventFromMessage(message);
	ret = m_SMSModel.modifyEvent(event);
	m_error = ret ? QMessageManager::NoError : QMessageManager::FrameworkFault;
    } else {
	m_error = QMessageManager::InvalidId;
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
    
    if (!id.isValid()) {
	m_error = QMessageManager::InvalidId;
	return folder;
    }

    m_error = QMessageManager::NoError;
    if (name.startsWith(FOLDER_PREFIX_SMS)) {
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

    m_error = QMessageManager::NoError;

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

    m_error = QMessageManager::NoError;

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
    qDebug() << "StorageEngine::eventsAdded";
    processFilters(events, &StorageEngine::messageAdded);
}

void StorageEngine::eventsUpdated(const QList<CommHistory::Event> &events)
{
    qDebug() << "StorageEngine::eventsUpdated";
    processFilters(events, &StorageEngine::messageUpdated);
}

void StorageEngine::eventDeleted(int id)
{
    QMessageId messageId("SMS_" + QString::number(id));
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
	qDebug() << "StorageEngine::processFilters:" << "added/updated" << event.id();
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

bool StorageEngine::compose(const QMessage &message)
{
    QDEBUG_FUNCTION_BEGIN
    m_error = QMessageManager::NoError;    

    QStringList contacts = MessagingHelper::stringListFromAddressList(message.to());

    qDebug() << __PRETTY_FUNCTION__ << contacts << message.textContent();
    MessagingIf l_pMessagingIf;
    l_pMessagingIf.showSmsEditor(contacts, message.textContent(), QString());

    QDEBUG_FUNCTION_END
    return true;
}

bool StorageEngine::show(const QMessageId &id)
{
    QDEBUG_FUNCTION_BEGIN
    bool ret = false;
    m_error = QMessageManager::NoError;

    if (id.isValid()) {
        QMessage message(id);
        qDebug() << "after message";
        ret = compose(message);
    }
    else {
        qWarning() << __PRETTY_FUNCTION__ << "ERROR: InvalidId";
        m_error = QMessageManager::InvalidId;
    }

    QDEBUG_FUNCTION_END
    return ret;
}

ServiceQuery::ServiceQuery(QMessageService *service, const QMessageFilter &filter)
    : QObject(service)
    , _type(CountQuery)
    , _service(service)
    , _filter(filter)
    , _count(0)
{

}
	
ServiceQuery::ServiceQuery(QMessageService *service, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset)
    : QObject(service)
    , _type(MessageQuery)
    , _service(service)
    , _filter(filter)
    , _sortOrder(sortOrder)
    , _limit(limit)
    , _offset(offset)
    , _count(0)
    
{

}

ServiceQuery::ServiceQuery(QMessageService *service, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, 
			   const QMessageSortOrder &sortOrder, uint limit, uint offset)
    : QObject(service)
    , _type(MessageAndBodySearchQuery)
    , _service(service)
    , _filter(filter)
    , _body(body)
    , _matchFlags(matchFlags)
    , _sortOrder(sortOrder)
    , _limit(limit)
    , _offset(offset)
    , _count(0)
{

}

ServiceQuery::~ServiceQuery()
{
    qDebug() << "ServiceQuery::~ServiceQuery()";
}

void ServiceQuery::doQuery()
{
    if (_type == CountQuery) {
	_count = StorageEngine::instance()->countMessagesSync(_filter);
	QTimer::singleShot(0, this, SLOT(completed()));
    } else {
	_ids = StorageEngine::instance()->queryMessagesSync(_filter);
	if (_ids.count()) {
	    if (_type == MessageQuery || _body.isEmpty()) {
		QTimer::singleShot(0, this, SLOT(sortMessages()));
	    } else {
		QTimer::singleShot(0, this, SLOT(searchBody()));
	    }
	} else {
	    QTimer::singleShot(0, this, SLOT(completed()));
	}
    }
}

void ServiceQuery::sortMessages()
{
    MessagingHelper::orderMessages(_ids, _sortOrder);
    MessagingHelper::applyOffsetAndLimitToMessageIdList(_ids, _limit, _offset);
    QTimer::singleShot(0, this, SLOT(completed()));
}

void ServiceQuery::searchBody()
{
    QMessageIdList ids(_ids);
    
    _ids.clear();
    
    foreach (const QMessageId &id, ids) {
	const QMessage message(id);
	const QString text = message.textContent();
	if (text.length() >= _body.length()) {
	    bool found = false;

	    if (_matchFlags & QMessageDataComparator::MatchCaseSensitive)
		found = text.contains(_body, Qt::CaseSensitive);
	    else
		found = text.contains(_body, Qt::CaseInsensitive);
	    
	    if (found)
		_ids << message.id();
	}
    }

    if (_ids.count()) {
	QTimer::singleShot(0, this, SLOT(sortMessages()));
    } else {
	QTimer::singleShot(0, this, SLOT(completed()));
    }
}

void ServiceQuery::completed()
{
    QMessageServicePrivate *p = QMessageServicePrivate::implementation(*_service);
    if (_type == CountQuery)
	p->messagesCounted(_count);
    else
	p->messagesFound(_ids, true, true);
    deleteLater();
}

#include "moc_storageengine_maemo6_p.cpp"
