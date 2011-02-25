/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QThread>
#include <QMutex>
#include <QCache>
#include "qdeclarativemessagemodel.h"
#include "qdeclarativemessagefilter.h"
#include <qdeclarativeinfo.h>
#include <qmessagecontentcontainer.h>
#include <QDebug>

#define CACHE_SIZE 100

QTM_USE_NAMESPACE

class MessageModelWorker : public QObject
{
    Q_OBJECT
public:
    MessageModelWorker();

    QMessage getMessage(const QMessageId &id) {
        QMutexLocker locker(&mutex);
        if (messageCache.contains(id)) {
            return *messageCache.object(id); 
        } else {
            if (!messageIdRequests.contains(id)) {
                messageIdRequests.append(id);
                if (messageIdRequests.count() == 1)
                    QMetaObject::invokeMethod(this, "getRequested", Qt::QueuedConnection);
            } else {
                messageIdRequests.removeAll(id);
                messageIdRequests.append(id);
            }
        }

        return QMessage();
    }

    void requestBody(const QMessageId &id) {
        QMutexLocker locker(&mutex);
        messageBodyRequest = id;
        QMetaObject::invokeMethod(this, "getRequested", Qt::QueuedConnection);
    }

public slots:
    void setFilter(QMessageFilter filter, QMessageSortOrder sort, int limit);
    void getBody(const QMessageId &id);
    void show(const QMessageId &id);
    void remove(const QMessageId &id);

signals:
    void messagesFound(const QMessageIdList&);
    void serviceProgressChanged(uint, uint);
    void messageAdded(const QMessageId&);
    void messageRemoved(const QMessageId&);
    void messageUpdated(const QMessageId&);

private slots:
    void updateFilter();
    void getRequested();
    void serviceStateChanged(QMessageService::State);
    void messageAdded(const QMessageId&,const QMessageManager::NotificationFilterIdSet);
    void messageRemoved(const QMessageId&,const QMessageManager::NotificationFilterIdSet);
    void messageUpdated(const QMessageId&,const QMessageManager::NotificationFilterIdSet);

private:
    void init();
    QMessage *cacheMessage(const QMessageId &id) {
        QMutexLocker locker(&mutex);
        if (!messageCache.contains(id)) {
            locker.unlock();
            QMessage *msg = new QMessage(id);
            locker.relock();
            messageCache.insert(id, msg);
        }
        return messageCache.object(id);
    }
    void removeFromCache(const QMessageId &id) {
        QMutexLocker locker(&mutex);
        messageCache.remove(id);
    }

private:
    QMessageManager *manager;
    QMessageService *service;
    QMessageFilter mFilter;
    QMessageSortOrder mSort;
    bool updatePending : 1;
    bool initialized : 1;
    int mLimit;
    QMessageIdList messageIds;
    QMessageManager::NotificationFilterId notifierId;
    QMessageId serviceId;
    enum QueryAction { None, Query, RetrieveBody };
    QueryAction queryAction;
    QCache<QMessageId,QMessage> messageCache;
    QMessageIdList messageIdRequests;
    QMessageId messageBodyRequest;
    QMutex mutex;
};

MessageModelWorker::MessageModelWorker()
    : manager(0), service(0), updatePending(false), initialized(false)
    , notifierId(-1), queryAction(None), messageCache(CACHE_SIZE)
{
}

void MessageModelWorker::init()
{
    if (!initialized) {
        initialized = true;
        manager = new QMessageManager(this);
        service = new QMessageService(this);
        QObject::connect(service, SIGNAL(stateChanged(QMessageService::State)),
                this, SLOT(serviceStateChanged(QMessageService::State)));
        QObject::connect(service, SIGNAL(messagesFound(const QMessageIdList&)),
                this, SIGNAL(messagesFound(const QMessageIdList&)));
        QObject::connect(service, SIGNAL(progressChanged(uint,uint)),
                this, SIGNAL(serviceProgressChanged(uint,uint)));
        QObject::connect(manager, SIGNAL(messageAdded(const QMessageId&,const QMessageManager::NotificationFilterIdSet)),
                this, SLOT(messageAdded(const QMessageId&,const QMessageManager::NotificationFilterIdSet)));
        QObject::connect(manager, SIGNAL(messageRemoved(const QMessageId&,const QMessageManager::NotificationFilterIdSet)),
                this, SLOT(messageRemoved(const QMessageId&,const QMessageManager::NotificationFilterIdSet)));
        QObject::connect(manager, SIGNAL(messageUpdated(const QMessageId&,const QMessageManager::NotificationFilterIdSet)),
                this, SLOT(messageUpdated(const QMessageId&,const QMessageManager::NotificationFilterIdSet)));
    }
}

void MessageModelWorker::setFilter(QMessageFilter filter, QMessageSortOrder sort, int limit)
{
    mutex.lock();
    mFilter = filter;
    mSort = sort;
    mLimit = limit;
    mutex.unlock();
    if (!updatePending) {
        updatePending = true;
        QMetaObject::invokeMethod(this, "updateFilter", Qt::QueuedConnection);
    }
}

void MessageModelWorker::updateFilter()
{
    init();
    queryAction = Query;
    if (notifierId != -1)
        manager->unregisterNotificationFilter(notifierId);

    mutex.lock();
    service->queryMessages(mFilter, mSort, mLimit);
    mutex.unlock();

    notifierId = manager->registerNotificationFilter(mFilter);
    updatePending = false;
}

void MessageModelWorker::getRequested()
{
    init();
    QMessageIdList ids;
    mutex.lock();
    for (int i = 0; i < 3 && messageIdRequests.count(); i++)
        ids.append(messageIdRequests.takeLast());
    if (messageIdRequests.count())
        QMetaObject::invokeMethod(this, "getRequested", Qt::QueuedConnection);
    mutex.unlock();

    for (int idx = 0; idx < ids.count(); ++idx) {
        QMessageId id = ids.at(idx);
        cacheMessage(id);
        emit messageUpdated(id);
    }

    QMessageId bodyId;
    mutex.lock();
    bodyId = messageBodyRequest;
    messageBodyRequest = QMessageId();
    mutex.unlock();

    if (bodyId.isValid())
        getBody(bodyId);
}

void MessageModelWorker::getBody(const QMessageId &id)
{
    QMessage message(*cacheMessage(id));

    QMessageContentContainer b = message.find(message.bodyId());
    if (!b.isContentAvailable()) {
//        qDebug() << "loading content" << id.toString();
        if (serviceId != id) {
            serviceId = id;
            if (service->state() == QMessageService::ActiveState) {
                service->cancel();
            } else {
//                qDebug() << "getting body";
                queryAction = MessageModelWorker::RetrieveBody;
                service->retrieveBody(id);
            }
        }
    } else {
        emit messageUpdated(serviceId);
    }
}

void MessageModelWorker::show(const QMessageId &id)
{
    init();
    service->show(id);
}

void MessageModelWorker::remove(const QMessageId &id)
{
    init();
    manager->removeMessage(id);
}

void MessageModelWorker::serviceStateChanged(QMessageService::State newState)
{
//    qDebug() << "state" << newState;
    switch (newState) {
        case QMessageService::FinishedState:
            if (queryAction == MessageModelWorker::RetrieveBody) {
//                qDebug() << "body retrieved";
                cacheMessage(serviceId);
                emit messageUpdated(serviceId);
            }
            queryAction = MessageModelWorker::None;
            break;
        case QMessageService::CanceledState:
            if (queryAction == MessageModelWorker::RetrieveBody) {
//                qDebug() << "cancelled previous, getting body";
                service->retrieveBody(serviceId);
            }
            break;
        default:
            break;
    } 
}

void MessageModelWorker::messageAdded(const QMessageId &id,const QMessageManager::NotificationFilterIdSet)
{
    if (!updatePending) {
        updatePending = true;
        QMetaObject::invokeMethod(this, "updateFilter", Qt::QueuedConnection);
    }
    emit messageAdded(id);
}

void MessageModelWorker::messageRemoved(const QMessageId &id,const QMessageManager::NotificationFilterIdSet)
{
    removeFromCache(id);
    emit messageRemoved(id);
}

void MessageModelWorker::messageUpdated(const QMessageId &id,const QMessageManager::NotificationFilterIdSet)
{
    removeFromCache(id);
    cacheMessage(id);
    emit messageUpdated(id);
}

//============================================================================

class QDeclarativeMessageModelPrivate
{
public:
    QDeclarativeMessageModelPrivate(QDeclarativeMessageModel *model)
        : q(model), filter(0), sortKey(QDeclarativeMessageModel::Timestamp)
        , sortOrder(QDeclarativeMessageModel::AscendingOrder)
        , componentCompleted(false)
        , updatePending(false), limit(0)
    {
        worker = new MessageModelWorker;
        worker->moveToThread(&workerThread);
        QObject::connect(worker, SIGNAL(messagesFound(const QMessageIdList&)),
                q, SLOT(messagesFound(const QMessageIdList&)));
        QObject::connect(worker, SIGNAL(messageAdded(const QMessageId&)),
                q, SIGNAL(messageAdded()));
        QObject::connect(worker, SIGNAL(messageRemoved(const QMessageId&)),
                q, SLOT(messageRemoved(const QMessageId&)));
        QObject::connect(worker, SIGNAL(messageUpdated(const QMessageId&)),
                q, SLOT(messageUpdated(const QMessageId&)));
        workerThread.start(QThread::IdlePriority);
    }
    ~QDeclarativeMessageModelPrivate()
    {
        workerThread.quit();
        workerThread.wait();
        delete worker;
    }

    void updateFilter();

    QDeclarativeMessageModel *q;
    QDeclarativeMessageFilterBase *filter;
    QDeclarativeMessageModel::SortKey sortKey;
    QDeclarativeMessageModel::SortOrder sortOrder;
    QMessageIdList messageIds;
    bool componentCompleted : 1;
    bool updatePending : 1;
    QMessage lastMessage;
    int limit;
    QThread workerThread;
    MessageModelWorker *worker;
};

void QDeclarativeMessageModelPrivate::updateFilter()
{
    if (!componentCompleted)
        return;
    QMessageSortOrder sort;
    switch (sortKey) {
    case QDeclarativeMessageModel::Priority:
        sort = QMessageSortOrder::byPriority(Qt::SortOrder(sortOrder));
        break;
    case QDeclarativeMessageModel::Sender:
        sort = QMessageSortOrder::bySender(Qt::SortOrder(sortOrder));
        break;
    case QDeclarativeMessageModel::Size:
        sort = QMessageSortOrder::bySize(Qt::SortOrder(sortOrder));
        break;
    case QDeclarativeMessageModel::StatusRead:
        sort = QMessageSortOrder::byStatus(QMessage::Read, Qt::SortOrder(sortOrder));
        break;
    case QDeclarativeMessageModel::StatusIncoming:
        sort = QMessageSortOrder::byStatus(QMessage::Incoming, Qt::SortOrder(sortOrder));
        break;
    case QDeclarativeMessageModel::Subject:
        sort = QMessageSortOrder::bySubject(Qt::SortOrder(sortOrder));
        break;
    case QDeclarativeMessageModel::Timestamp:
        sort = QMessageSortOrder::byTimeStamp(Qt::SortOrder(sortOrder));
        break;
    case QDeclarativeMessageModel::ReceptionTimestamp:
        sort = QMessageSortOrder::byReceptionTimeStamp(Qt::SortOrder(sortOrder));
        break;
    case QDeclarativeMessageModel::Recipients:
        sort = QMessageSortOrder::byRecipients(Qt::SortOrder(sortOrder));
        break;
    case QDeclarativeMessageModel::Type:
        sort = QMessageSortOrder::byType(Qt::SortOrder(sortOrder));
        break;
    }

    if (filter)
        worker->setFilter(filter->filter(), sort, limit);
    else
        worker->setFilter(QMessageFilter(), sort, limit);

    updatePending = false;
}

/*!
    \qmlclass MessageModel QDeclarativeMessageModel
    \brief The MessageModel element provides access to messages.

    \ingroup qml-messaging

    This element is part of the \bold{QtMobility.messaging 1.1} module.

    MessageModel provides a model of messages from the message store.
    The contents of the model can be specified via a \l filter, and sorted
    via the \l sortBy and \l sortOrder properties.  The model is automatically
    updated if the contents of the store change.

    The following roles are supported by the model:

    \table
    \header \o Role \o type
    \row \o subject \o string
    \row \o type
        \o enumeration
            \list
            \o MessageFilter.Mms
            \o MessageFilter.Sms
            \o MessageFilter.Email
            \o MessageFilter.InstantMessage
            \endlist
    \row \o size \o int
    \row \o sender \o string
    \row \o to \o List<string>
    \row \o date \o Date
    \row \o receivedDate \o Date
    \row \o priority
        \o enumeration
            \list
            \o MessageFilter.HighPriority
            \o MessageFilter.NormalPriority
            \o MessageFilter.LowPriority
            \endlist
    \row \o body \o string
    \row \o ready \o bool
    \endtable

    \bold Note: since the body can be large and may need to be fetched from
    the server it is recommended that viewing the body be a user action,
    and the body role not be bound to until needed.  If the body has not yet been
    downloaded, it will be requested. \c body will remain undefined until it
    has been retrieved.

    The \c ready role is true if the message meta data has been retrieved; otherwise
    false.  Since MessageModel threaded, all messages are initially not ready, and will
    become ready once the data is available.

    The following example displays the subject, sender and date of all messages
    sorted in descending order by timestamp:

    \qml
    import Qt 4.7
    import QtMobility.messaging 1.1

    Rectangle {
        width: 320
        height: 480
        ListView {
            id: list
            anchors.fill: parent
            model: MessageModel {
                sortBy: MessageModel.Timestamp
                sortOrder: MessageModel.DescendingOrder
            }
            delegate: Item {
                id: wrapper
                height: 32; width: list.width
                Text { id: subjText; text: subject; font.pixelSize: 13; x: 3 }
                Text {
                    text: sender; color: "gray"; font.pixelSize: 9
                    x: 3; width: parent.width-100;
                    anchors.top: subjText.bottom; anchors.topMargin: 3
                    elide: Text.ElideRight
                }
                Text {
                    text: date; color: "gray"; font.pixelSize: 9
                    anchors.right: parent.right
                    anchors.top: subjText.bottom; anchors.topMargin: 3
                }
            }
        }
    }
    \endqml

    \sa MessageFilter
*/
QDeclarativeMessageModel::QDeclarativeMessageModel(QObject *parent)
    : QAbstractListModel(parent), d(new QDeclarativeMessageModelPrivate(this))
{
    QHash<int, QByteArray> roleNames;
    roleNames[SubjectRole] = "subject";
    roleNames[SenderRole] = "sender";
    roleNames[ToRole] = "to";
    roleNames[SizeRole] = "size";
    roleNames[TypeRole] = "type";
    roleNames[DateRole] = "date";
    roleNames[ReceivedDateRole] = "receivedDate";
    roleNames[BodyRole] = "body";
    roleNames[PriorityRole] = "priority";
    roleNames[ReadyRole] = "ready";
    setRoleNames(roleNames);
}

QDeclarativeMessageModel::~QDeclarativeMessageModel()
{
    delete d;
}

/*!
    \qmlproperty MessageFilter MessageModel::filter

    The MessageFilter specifying the messages to provide.
*/
QDeclarativeMessageFilterBase *QDeclarativeMessageModel::filter() const
{
    return d->filter;
}

void QDeclarativeMessageModel::setFilter(QDeclarativeMessageFilterBase *filter)
{
    if (filter == d->filter)
        return;

    d->filter = filter;
    scheduleUpdate();
    emit filterChanged();
}

/*!
    \qmlproperty enumeration MessageModel::sortBy

    Specifies the role to sort by:

    \list
    \o MessageModel.Priority
    \o MessageModel.Sender
    \o MessageModel.Size
    \o MessageModel.StatusRead
    \o MessageModel.StatusIncoming
    \o MessageModel.Subject
    \o MessageModel.Timestamp (default)
    \o MessageModel.ReceptionTimestamp
    \o MessageModel.Recipients
    \o MessageModel.Type
    \endlist
*/
QDeclarativeMessageModel::SortKey QDeclarativeMessageModel::sortBy() const
{
    return d->sortKey;
}

void QDeclarativeMessageModel::setSortBy(QDeclarativeMessageModel::SortKey k)
{
    if (k == d->sortKey)
        return;
    d->sortKey = k;
    scheduleUpdate();
    emit sortByChanged();
}


/*!
    \qmlproperty enumeration MessageModel::sortOrder

    Specifies the sort order:

    \list
    \o MessageModel.AscendingOrder
    \o MessageModel.DescendingOrder
    \endlist
*/
QDeclarativeMessageModel::SortOrder QDeclarativeMessageModel::sortOrder() const
{
    return d->sortOrder;
}

void QDeclarativeMessageModel::setSortOrder(QDeclarativeMessageModel::SortOrder o)
{
    if (o == d->sortOrder)
        return;
    d->sortOrder = o;
    scheduleUpdate();
    emit sortOrderChanged();
}

/*!
    \qmlproperty int MessageModel::count
    Holds the number of messages matching the filter.

    If \l limit is set then there will be at most \l limit messages.
*/

/*!
    \qmlproperty int MessageModel::limit
    Holds the maximum number of messages to retrieve.

    A value of zero (default) will retrieve all messages.
*/
int QDeclarativeMessageModel::limit() const
{
    return d->limit;
}

void QDeclarativeMessageModel::setLimit(int l)
{
    if (l == d->limit)
        return;
    d->limit = l;
    scheduleUpdate();
    emit limitChanged();
}

void QDeclarativeMessageModel::componentComplete()
{
    d->componentCompleted = true;
    d->updateFilter();
}

void QDeclarativeMessageModel::scheduleUpdate()
{
    if (!d->componentCompleted)
        return;
    if (!d->updatePending) {
        d->updatePending = true;
        QMetaObject::invokeMethod(this, "updateFilter", Qt::QueuedConnection);
    }
}

void QDeclarativeMessageModel::messagesFound(const QMessageIdList &ids)
{
//    qDebug() << "found messages";
    beginResetModel();
    d->messageIds = ids;
    d->lastMessage = QMessage();
    endResetModel();
    emit countChanged();
}

void QDeclarativeMessageModel::serviceProgressChanged(uint value, uint total)
{
    /*
    if (total) {
        qDebug() << "progress" << qreal(value)/total;
    } else {
        qDebug() << "No progress available";
    }
    */
}

int QDeclarativeMessageModel::rowCount(const QModelIndex &) const
{
    return d->messageIds.count();
}

QVariant QDeclarativeMessageModel::data(const QModelIndex &index, int role) const
{
    QVariant rv;
    int idx = index.row();
    if (d->lastMessage.id() != d->messageIds.at(idx))
        d->lastMessage = d->worker->getMessage(d->messageIds.at(idx));
    if (!d->lastMessage.id().isValid()) {
        if (role == ReadyRole)
            rv = false;
        else
            rv = QString();
        return rv;
    }

    switch (role) {
    case SubjectRole:
        rv = d->lastMessage.subject();
        break;
    case SizeRole:
        rv = d->lastMessage.size();
        break;
    case TypeRole:
        rv = d->lastMessage.type();
        break;
    case SenderRole:
        rv = d->lastMessage.from().addressee();
        break;
    case ToRole: {
            QStringList to;
            foreach (QMessageAddress addr, d->lastMessage.to())
                to += addr.addressee();
            rv = to;
        }
        break;
    case DateRole:
        rv = d->lastMessage.date();
        break;
    case ReceivedDateRole:
        rv = d->lastMessage.receivedDate();
        break;
    case BodyRole:
//        qDebug() << "!!!!!body " << d->lastMessage.bodyId().toString();
        if (d->lastMessage.bodyId().isValid()) {
            QMessageContentContainer b = d->lastMessage.find(d->lastMessage.bodyId());
            if (b.isContentAvailable()) {
                rv = b.textContent();
            } else {
                d->worker->requestBody(d->lastMessage.id());
            }
        } else {
            // Why is the body id sometimes invalid?
            rv = QString();
        }
        break;
    case PriorityRole:
        rv = d->lastMessage.priority();
        break;
    case ReadyRole:
        rv = true;
        break;
    default:
        break;
    }
    return rv;
}

/*!
    \qmlmethod MessageModel::showMessage(index)

    Displays the message at \a index using the system message client.
*/
void QDeclarativeMessageModel::showMessage(int index) const
{
    if (index < 0 || index >= d->messageIds.count())
        return;
    QMetaObject::invokeMethod(d->worker, "show", Qt::QueuedConnection, Q_ARG(QMessageId,d->messageIds.at(index)));
}

/*!
    \qmlmethod MessageModel::removeMessage(index)

    Remove the message at \a index from the mail store and the originating server (if applicable).
*/
void QDeclarativeMessageModel::removeMessage(int index)
{
    if (index < 0 || index >= d->messageIds.count())
        return;
    QMetaObject::invokeMethod(d->worker, "remove", Qt::QueuedConnection, Q_ARG(QMessageId,d->messageIds.at(index)));
}

/*!
    \qmlsignal MessageModel::messageAdded()

    This handler is called when a message that matches the filter criteria
    is added to the store.  The model will be updated shortly and will include
    the new message.
*/

void QDeclarativeMessageModel::messageUpdated(const QMessageId &id)
{
    for (int i = 0; i < d->messageIds.count(); ++i) {
        if (d->messageIds.at(i) == id) {
            QModelIndex idx = index(i, 0);
            emit dataChanged(idx, idx);
            break;
        }
    }
}

void QDeclarativeMessageModel::messageRemoved(const QMessageId &id)
{
    for (int i = 0; i < d->messageIds.count(); ++i) {
        if (d->messageIds.at(i) == id) {
            beginRemoveRows(QModelIndex(), i, i);
            d->messageIds.removeAt(i);
            endRemoveRows();
            break;
        }
    }
}

void QDeclarativeMessageModel::updateFilter()
{
    d->updateFilter();
}

#include "qdeclarativemessagemodel.moc"
