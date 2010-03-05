/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessageservice.h"
#include "qmfhelpers_p.h"

#include <qmailserviceaction.h>

#include <QTimer>
#include <qmessage_p.h>


QTM_BEGIN_NAMESPACE

namespace {

struct TextPartSearcher
{
    QString _search;

    TextPartSearcher(const QString &searchText) : _search(searchText) {}

    bool operator()(const QMailMessagePart &part)
    {
        if (part.contentType().type().toLower() == "text") {
            if (part.body().data().contains(_search, Qt::CaseInsensitive)) {
                // We have located some matching text - stop the traversal
                return false;
            }
        }

        return true;
    }
};

}

using namespace QmfHelpers;

class QMessageServicePrivate : public QObject
{
    Q_OBJECT

public:
    QMessageServicePrivate();

    QMailTransmitAction _transmit;
    QMailRetrievalAction _retrieval;
    QMailServiceAction *_active;
    QMessageManager::Error _error;
    bool _activeStoreAction;

    QList<QMessageId> _matchingIds;
    QList<QMailMessageId> _candidateIds;
    QMessageFilter _lastFilter;
    QMessageSortOrder _lastOrdering;
    QString _match;
    int _limit;
    int _offset;

    QMailMessageIdList _transmitIds;
    
    bool isBusy() const;

signals:
    void stateChanged(QMessageService::State);
    void progressChanged(uint, uint);
    void messagesFound(const QMessageIdList&);
    void messagesCounted(int);

protected slots:
    void transmitActivityChanged(QMailServiceAction::Activity a);
    void retrievalActivityChanged(QMailServiceAction::Activity a);
    void statusChanged(const QMailServiceAction::Status &s);
    void completed();
    void reportMatchingIds();
    void reportMatchingCount();
    void findMatchingIds();
    void testNextMessage();

private:
    bool messageMatch(const QMailMessageId &messageid);
};

QMessageServicePrivate::QMessageServicePrivate()
    : QObject(),
      _active(0),
      _error(QMessageManager::NoError),
      _activeStoreAction(false),
      _limit(0),
      _offset(0)
{
    connect(&_transmit, SIGNAL(activityChanged(QMailServiceAction::Activity)), this, SLOT(transmitActivityChanged(QMailServiceAction::Activity)));
    connect(&_transmit, SIGNAL(statusChanged(QMailServiceAction::Status)), this, SLOT(statusChanged(QMailServiceAction::Status)));

    connect(&_retrieval, SIGNAL(activityChanged(QMailServiceAction::Activity)), this, SLOT(retrievalActivityChanged(QMailServiceAction::Activity)));
    connect(&_retrieval, SIGNAL(statusChanged(QMailServiceAction::Status)), this, SLOT(statusChanged(QMailServiceAction::Status)));
}

bool QMessageServicePrivate::isBusy() const
{
    if ((_active && ((_active->activity() == QMailServiceAction::Pending) || (_active->activity() == QMailServiceAction::InProgress))) ||
        _activeStoreAction) {
        qWarning() << "Action is currently busy";
        return true;
    }

    return false;
}

void QMessageServicePrivate::transmitActivityChanged(QMailServiceAction::Activity a)
{
    if (a == QMailServiceAction::Successful) {
        if (!_transmitIds.isEmpty()) {
            // If these messages were transmitted, we need to move them to Sent folder
            QMailMessageKey filter(QMailMessageKey::id(_transmitIds));
            foreach (const QMailMessageId &id, QMailStore::instance()->queryMessages(filter & QMailMessageKey::status(QMailMessage::Sent))) {
                QMessage message(convert(id));

                QMessagePrivate::setStandardFolder(message,QMessage::SentFolder);
                if (!QMessageManager().updateMessage(&message)) {
                    qWarning() << "Unable to mark message as sent!";
                }
            }

            _transmitIds.clear();
        }
    } else if (a == QMailServiceAction::Failed) {
        _transmitIds.clear();

        if (_error == QMessageManager::NoError) {
            // We may have failed due to some part of the request remaining incomplete
            _error = QMessageManager::RequestIncomplete;
        }
    }

    emit stateChanged(convert(a));
}

void QMessageServicePrivate::statusChanged(const QMailServiceAction::Status &s)
{
    if (s.errorCode != QMailServiceAction::Status::ErrNoError) {
        qWarning() << QString("Service error %1: \"%2\"").arg(s.errorCode).arg(s.text);

        if (s.errorCode == QMailServiceAction::Status::ErrNotImplemented) {
            _error = QMessageManager::NotYetImplemented;
        } else if ((s.errorCode == QMailServiceAction::Status::ErrNonexistentMessage) ||
                   (s.errorCode == QMailServiceAction::Status::ErrEnqueueFailed) ||
                   (s.errorCode == QMailServiceAction::Status::ErrInvalidAddress) ||
                   (s.errorCode == QMailServiceAction::Status::ErrInvalidData)) {
            _error = QMessageManager::ConstraintFailure;
        } else {
            _error = QMessageManager::FrameworkFault;
        }
    }
}

void QMessageServicePrivate::retrievalActivityChanged(QMailServiceAction::Activity a)
{
    if ((a == QMailServiceAction::Failed) && (_error == QMessageManager::NoError)) {
        // We may have failed due to some part of the request remaining incomplete
        _error = QMessageManager::RequestIncomplete;
    }

    emit stateChanged(convert(a));
}

void QMessageServicePrivate::completed()
{
    _activeStoreAction = false;
    emit stateChanged(QMessageService::FinishedState);
}

bool QMessageServicePrivate::messageMatch(const QMailMessageId &messageId)
{
    if (_match.isEmpty()) {
        return true;
    }

    const QMailMessage candidate(messageId);
    if (candidate.id().isValid()) {
        // Search only messages or message parts that are of type 'text/*'
        if (candidate.hasBody()) {
            if (candidate.contentType().type().toLower() == "text") {
                if (candidate.body().data().contains(_match, Qt::CaseInsensitive))
                    return true;
            }
        } else if (candidate.multipartType() != QMailMessage::MultipartNone) {
            // Search all 'text/*' parts within this message
            TextPartSearcher searcher(_match);
            if (candidate.foreachPart<TextPartSearcher&>(searcher) == false) {
                // We found a matching part in the message
                return true;
            }
        }
    }

    return false;
}

void QMessageServicePrivate::reportMatchingIds()
{
    emit messagesFound(convert(_candidateIds));
    completed();
}

void QMessageServicePrivate::reportMatchingCount()
{
    emit messagesCounted(_candidateIds.count());
    completed();
}

void QMessageServicePrivate::findMatchingIds()
{
    int required = ((_offset + _limit) - _matchingIds.count());

    // Are any of the existing IDs part of the result set?
    if (required < _limit) {
        emit messagesFound(_matchingIds.mid(_offset, _limit));
    }

    if ((required > 0 || _limit == 0) && !_candidateIds.isEmpty()) {
        QTimer::singleShot(0, this, SLOT(testNextMessage()));
    } else {
        completed();
    }
}

void QMessageServicePrivate::testNextMessage()
{
    int required = ((_offset + _limit) - _matchingIds.count());
    if (required > 0 || _limit==0) {
        QMailMessageId messageId(_candidateIds.takeFirst());
        if (messageMatch(messageId)) {
            _matchingIds.append(convert(messageId));
            --required;

            if (required < _limit) {
                // This is within the result set
                emit messagesFound(QMessageIdList() << _matchingIds.last());
            }
        }

        if ((required > 0 || _limit == 0) && !_candidateIds.isEmpty()) {
            QTimer::singleShot(0, this, SLOT(testNextMessage()));
            return;
        }
    }

    completed();
}

QMessageService::QMessageService(QObject *parent)
    : QObject(parent),
      d_ptr(new QMessageServicePrivate)
{
    // Ensure the message store is initialized
    QMessageManager();

    connect(d_ptr, SIGNAL(stateChanged(QMessageService::State)), 
            this, SIGNAL(stateChanged(QMessageService::State)));
    connect(d_ptr, SIGNAL(messagesFound(QMessageIdList)), 
            this, SIGNAL(messagesFound(QMessageIdList)));
    connect(d_ptr, SIGNAL(messagesCounted(int)), 
            this, SIGNAL(messagesCounted(int)));
    connect(d_ptr, SIGNAL(progressChanged(uint, uint)), 
            this, SIGNAL(progressChanged(uint, uint)));
}

QMessageService::~QMessageService()
{
    delete d_ptr;
}

bool QMessageService::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    if (d_ptr->isBusy()) {
        return false;
    }
    d_ptr->_active = 0;
    d_ptr->_activeStoreAction = true;
    emit stateChanged(QMessageService::ActiveState);

    d_ptr->_candidateIds = QMailStore::instance()->queryMessages(convert(filter), convert(sortOrder), limit, offset);
    d_ptr->_error = convert(QMailStore::instance()->lastError());

    if (d_ptr->_error == QMessageManager::NoError) {
        d_ptr->_lastFilter = QMessageFilter();
        d_ptr->_lastOrdering = QMessageSortOrder();
        d_ptr->_match = QString();
        d_ptr->_limit = static_cast<int>(limit);
        d_ptr->_offset = 0;
        d_ptr->_matchingIds.clear();
        QTimer::singleShot(0, d_ptr, SLOT(reportMatchingIds()));
        return true;
    }

    return false;
}

bool QMessageService::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    if (matchFlags) {
        //TODO: Support matchFlags
        return false;
    }

    if (body.isEmpty()) {
        return queryMessages(filter, sortOrder, limit, offset);
    }

    if (d_ptr->isBusy()) {
        return false;
    }

    d_ptr->_active = 0;
    d_ptr->_activeStoreAction = true;
    emit stateChanged(QMessageService::ActiveState);

    if ((filter == d_ptr->_lastFilter) && (sortOrder == d_ptr->_lastOrdering) && (body == d_ptr->_match)) {
        // This is a continuation of the last search
        d_ptr->_limit = static_cast<int>(limit);
        d_ptr->_offset = static_cast<int>(offset);
        QTimer::singleShot(0, d_ptr, SLOT(findMatchingIds()));
        return true;
    }

    // Find all messages to perform the body search on
    d_ptr->_candidateIds = QMailStore::instance()->queryMessages(convert(filter), convert(sortOrder));
    d_ptr->_error = convert(QMailStore::instance()->lastError());

    if (d_ptr->_error == QMessageManager::NoError) {
        d_ptr->_lastFilter = filter;
        d_ptr->_lastOrdering = sortOrder;
        d_ptr->_match = body;
        d_ptr->_limit = static_cast<int>(limit);
        d_ptr->_offset = static_cast<int>(offset);
        d_ptr->_matchingIds.clear();
        QTimer::singleShot(0, d_ptr, SLOT(findMatchingIds()));
        return true;
    }

    return false;
}

bool QMessageService::countMessages(const QMessageFilter &filter)
{
    if (d_ptr->isBusy()) {
        return false;
    }
    d_ptr->_active = 0;
    d_ptr->_activeStoreAction = true;
    
    d_ptr->_candidateIds = QMailStore::instance()->queryMessages(convert(filter));
    d_ptr->_error = convert(QMailStore::instance()->lastError());

    if (d_ptr->_error == QMessageManager::NoError) {
        d_ptr->_lastFilter = QMessageFilter();
        d_ptr->_lastOrdering = QMessageSortOrder();
        d_ptr->_match = QString();
        d_ptr->_limit = 0;
        d_ptr->_offset = 0;
        d_ptr->_matchingIds.clear();
        QTimer::singleShot(0, d_ptr, SLOT(reportMatchingCount()));
        return true;
    }

    return false;
}

bool QMessageService::send(QMessage &message)
{
    if (d_ptr->isBusy()) {
        return false;
    }
    d_ptr->_active = 0;

    if (!message.parentAccountId().isValid()) {
        // Attach to the default account
        message.setParentAccountId(QMessageAccount::defaultAccount(message.type()));
        if (!message.parentAccountId().isValid()) {
            d_ptr->_error = QMessageManager::InvalidId;
            qWarning() << "Invalid message account ID";
            return false;
        }
    }

    // Ensure the message contains a timestamp
    if (!message.date().isValid()) {
        message.setDate(QDateTime::currentDateTime());
    }

    QMessageFolderId existingFolderId(message.parentFolderId());

    // Move the message to the Outbox folder
    QMessagePrivate::setStandardFolder(message,QMessage::OutboxFolder);

    QMailMessage *msg(convert(&message));

    // Ensure that the from address is added
    if (msg->from().isNull()) {
        QMailAccount account(msg->parentAccountId());
        msg->setFrom(account.fromAddress());
    }

    // Mark this message as outgoing
    msg->setStatus(QMailMessage::Outbox, true);

    if (msg->id().isValid()) {
        // Update the message
        if (!QMailStore::instance()->updateMessage(msg)) {
            d_ptr->_error = QMessageManager::FrameworkFault;
            qWarning() << "Unable to mark message as outgoing";
            return false;
        }
    } else {
        // Add this message to the store
        if (!QMailStore::instance()->addMessage(msg)) {
            d_ptr->_error = QMessageManager::FrameworkFault;
            qWarning() << "Unable to store message for transmission";
            return false;
        }
    }

    d_ptr->_transmitIds = QMailStore::instance()->queryMessages(QMailMessageKey::status(QMailMessage::Outgoing) & QMailMessageKey::parentAccountId(msg->parentAccountId()));

    d_ptr->_error = QMessageManager::NoError;
    d_ptr->_active = &d_ptr->_transmit;
    d_ptr->_transmit.transmitMessages(msg->parentAccountId());
    return true;
}

bool QMessageService::compose(const QMessage &message)
{
    if (d_ptr->isBusy()) {
        return false;
    }
    d_ptr->_active = 0;

    // TODO: To be implemented by integrator
    d_ptr->_error = QMessageManager::NotYetImplemented;
    qWarning() << "QMessageService::compose not yet implemented";
    return false;

    Q_UNUSED(message)
}

bool QMessageService::retrieveHeader(const QMessageId& id)
{
    if (d_ptr->isBusy()) {
        return false;
    }
    d_ptr->_active = 0;

    QMailMessageId messageId(convert(id));
    if (!messageId.isValid()) {
        d_ptr->_error = QMessageManager::InvalidId;
        qWarning() << "Invalid message ID";
        return false;
    }

    // Operation is not relevant to QMF - meta data retrieval always includes header information
    d_ptr->_error = QMessageManager::NoError;
    d_ptr->_active = 0;
    QTimer::singleShot(0, d_ptr, SLOT(completed()));
    return true;
}

bool QMessageService::retrieveBody(const QMessageId& id)
{
    if (d_ptr->isBusy()) {
        return false;
    }
    d_ptr->_active = 0;

    QMailMessageId messageId(convert(id));
    if (!messageId.isValid()) {
        d_ptr->_error = QMessageManager::InvalidId;
        qWarning() << "Invalid message ID";
        return false;
    }

    d_ptr->_error = QMessageManager::NoError;
    d_ptr->_active = &d_ptr->_retrieval;
    d_ptr->_retrieval.retrieveMessages(QMailMessageIdList() << messageId, QMailRetrievalAction::Content);
    return true;
}

bool QMessageService::retrieve(const QMessageId &messageId, const QMessageContentContainerId& id)
{
    if (d_ptr->isBusy()) {
        return false;
    }
    d_ptr->_active = 0;

    QMailMessagePart::Location location(convert(id));
    if (!location.isValid()) {
        d_ptr->_error = QMessageManager::InvalidId;
        qWarning() << "Invalid message part location";
        return false;
    }
    
    d_ptr->_error = QMessageManager::NoError;
    d_ptr->_active = &d_ptr->_retrieval;
    d_ptr->_retrieval.retrieveMessagePart(location);
    return true;

    Q_UNUSED(messageId)
}

bool QMessageService::show(const QMessageId& id)
{
    if (d_ptr->isBusy()) {
        return false;
    }
    d_ptr->_active = 0;

    // TODO: To be implemented by integrator
    d_ptr->_error = QMessageManager::NotYetImplemented;
    qWarning() << "QMessageService::show not yet implemented";
    return false;

    Q_UNUSED(id)
}

bool QMessageService::exportUpdates(const QMessageAccountId &id)
{
    if (d_ptr->isBusy()) {
        return false;
    }
    d_ptr->_active = 0;

    QMailAccountId accountId(convert(id));
    if (!accountId.isValid()) {
        d_ptr->_error = QMessageManager::InvalidId;
        qWarning() << "Account ID is not valid";
        return false;
    }

    d_ptr->_error = QMessageManager::NoError;
    d_ptr->_active = &d_ptr->_retrieval;
    d_ptr->_retrieval.exportUpdates(accountId);
    return true;
}

QMessageService::State QMessageService::state() const
{
    if (d_ptr->_active) {
        return convert(d_ptr->_active->activity());
    }
    else if(d_ptr->_activeStoreAction)
        return QMessageService::ActiveState;

    return FinishedState;
}

void QMessageService::cancel()
{
    if (d_ptr->_active) {
        d_ptr->_active->cancelOperation();
    }
    else if(d_ptr->_activeStoreAction)
        d_ptr->_activeStoreAction = false;
}

QMessageManager::Error QMessageService::error() const
{
    return d_ptr->_error;
}

#include "qmessageservice_qmf.moc"


QTM_END_NAMESPACE
