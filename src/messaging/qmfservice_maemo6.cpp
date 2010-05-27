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
#include "qmfservice_maemo6_p.h"
#include "qmfhelpers_maemo6_p.h"
#include "qmessageservice_maemo6_p.h"

#include <QTimer>
#include <qmessage_p.h>


QTM_BEGIN_NAMESPACE

namespace {

struct TextPartSearcher
{
    QString _search;
    Qt::CaseSensitivity _cs;

    TextPartSearcher(const QString &searchText, Qt::CaseSensitivity cs) 
	: _search(searchText)
	, _cs(cs) 
    {}

    bool operator()(const QMailMessagePart &part)
    {
        if (part.contentType().type().toLower() == "text") {
            if (part.body().data().contains(_search, _cs)) {
                // We have located some matching text - stop the traversal
                return false;
            }
        }

        return true;
    }
};

}

using namespace QmfHelpers;

QMFService::QMFService(QMessageService *service, QObject *parent)
    : QObject(parent),
      m_service(service),
      m_active(0),
      m_activeStoreAction(false),
      m_limit(0),
      m_offset(0)

{
    // Ensure the message store is initialized
    QMessageManager();

    connect(&m_transmit, SIGNAL(activityChanged(QMailServiceAction::Activity)), this, SLOT(transmitActivityChanged(QMailServiceAction::Activity)));
    connect(&m_transmit, SIGNAL(statusChanged(QMailServiceAction::Status)), this, SLOT(statusChanged(QMailServiceAction::Status)));

    connect(&m_retrieval, SIGNAL(activityChanged(QMailServiceAction::Activity)), this, SLOT(retrievalActivityChanged(QMailServiceAction::Activity)));
    connect(&m_retrieval, SIGNAL(statusChanged(QMailServiceAction::Status)), this, SLOT(statusChanged(QMailServiceAction::Status)));
}

QMFService::~QMFService()
{
}

bool QMFService::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    if (isBusy()) {
        return false;
    }

    m_active = 0;
    m_activeStoreAction = true;

    stateChanged(QMessageService::ActiveState);

    m_candidateIds = QMailStore::instance()->queryMessages(convert(filter), convert(sortOrder), limit, offset);

    QMessageManager::Error error = convert(QMailStore::instance()->lastError());
    setError(error);

    if (error == QMessageManager::NoError) {
        m_lastFilter = QMessageFilter();
        m_lastOrdering = QMessageSortOrder();
        m_match = QString();
        m_limit = static_cast<int>(limit);
        m_offset = 0;
        m_matchingIds.clear();
        QTimer::singleShot(0, this, SLOT(reportMatchingIds()));
        return true;
    }

    return false;
}

bool QMFService::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    if (body.isEmpty()) {
        return queryMessages(filter, sortOrder, limit, offset);
    }

    if (isBusy()) {
        return false;
    }

    m_active = 0;
    m_activeStoreAction = true;
    stateChanged(QMessageService::ActiveState);

    if ((filter == m_lastFilter) && (sortOrder == m_lastOrdering) 
	&& (body == m_match) && (matchFlags == m_matchFlags)) {
        // This is a continuation of the last search
        m_limit = static_cast<int>(limit);
        m_offset = static_cast<int>(offset);
        QTimer::singleShot(0, this, SLOT(findMatchingIds()));
        return true;
    }

    // Find all messages to perform the body search on
    m_candidateIds = QMailStore::instance()->queryMessages(convert(filter), convert(sortOrder));

    QMessageManager::Error error = convert(QMailStore::instance()->lastError());
    setError(error);

    if (error == QMessageManager::NoError) {
        m_lastFilter = filter;
        m_lastOrdering = sortOrder;
        m_match = body;
        m_limit = static_cast<int>(limit);
        m_offset = static_cast<int>(offset);
	m_matchFlags = matchFlags;
        m_matchingIds.clear();
        QTimer::singleShot(0, this, SLOT(findMatchingIds()));
        return true;
    }

    return false;
}

bool QMFService::countMessages(const QMessageFilter &filter)
{
    if (isBusy()) {
        return false;
    }
    m_active = 0;
    m_activeStoreAction = true;

    m_candidateIds = QMailStore::instance()->queryMessages(convert(filter));

    QMessageManager::Error error = convert(QMailStore::instance()->lastError());
    setError(error);

    if (error == QMessageManager::NoError) {
        m_lastFilter = QMessageFilter();
        m_lastOrdering = QMessageSortOrder();
        m_match = QString();
        m_limit = 0;
        m_offset = 0;
        m_matchingIds.clear();
        QTimer::singleShot(0, this, SLOT(reportMatchingCount()));
        return true;
    }

    return false;
}

bool QMFService::send(QMessage &message)
{
    if (isBusy()) {
        return false;
    }
    m_active = 0;

    if (!message.parentAccountId().isValid()) {
        // Attach to the default account
        message.setParentAccountId(QMessageAccount::defaultAccount(message.type()));
        if (!message.parentAccountId().isValid()) {
            setError(QMessageManager::InvalidId);
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
    QMessagePrivate::setStandardFolder(message, QMessage::OutboxFolder);

    QMailMessage msg(convert(message));

    // Ensure that the from address is added
    if (msg.from().isNull()) {
        QMailAccount account(msg.parentAccountId());
        msg.setFrom(account.fromAddress());
    }

    // Mark this message as outgoing
    msg.setStatus(QMailMessage::Outbox, true);

    if (msg.id().isValid()) {
        // Update the message
        if (!QMailStore::instance()->updateMessage(&msg)) {
            setError(QMessageManager::FrameworkFault);
            qWarning() << "Unable to mark message as outgoing";
            return false;
        }
    } else {
        // Add this message to the store
        if (!QMailStore::instance()->addMessage(&msg)) {
            setError(QMessageManager::FrameworkFault);
            qWarning() << "Unable to store message for transmission";
            return false;
        }
	// Update message
	QMessagePrivate *p = QMessagePrivate::implementation(message);
	p->_id = convert(msg.id());
	p->_contentIdentifier = msg.contentIdentifier();
	p->_contentScheme = msg.contentScheme();
    }

    m_transmitIds = QMailStore::instance()->queryMessages(QMailMessageKey::status(QMailMessage::Outbox) & QMailMessageKey::parentAccountId(msg.parentAccountId()));

    setError(QMessageManager::NoError);
    m_active = &m_transmit;
    m_transmit.transmitMessages(msg.parentAccountId());
    return true;
}

bool QMFService::compose(const QMessage &message)
{
    if (isBusy()) {
        return false;
    }
    m_active = 0;
    
    // TODO: To be implemented by integrator
    setError(QMessageManager::NotYetImplemented);
    qWarning() << "QMFService::compose not yet implemented";
    return false;

    Q_UNUSED(message)
}

bool QMFService::retrieveHeader(const QMessageId& id)
{
    if (isBusy()) {
        return false;
    }
    m_active = 0;

    QMailMessageId messageId(convert(id));
    if (!messageId.isValid()) {
        setError(QMessageManager::InvalidId);
        qWarning() << "Invalid message ID";
        return false;
    }

    // Operation is not relevant to QMF - meta data retrieval always includes header information
    setError(QMessageManager::NoError);
    m_active = 0;
    QTimer::singleShot(0, this, SLOT(completed()));
    return true;
}

bool QMFService::retrieveBody(const QMessageId& id)
{
    if (isBusy()) {
        return false;
    }
    m_active = 0;

    QMailMessageId messageId(convert(id));
    if (!messageId.isValid()) {
        setError(QMessageManager::InvalidId);
        qWarning() << "Invalid message ID";
        return false;
    }

    setError(QMessageManager::NoError);
    m_active = &m_retrieval;
    m_retrieval.retrieveMessages(QMailMessageIdList() << messageId, QMailRetrievalAction::Content);
    return true;
}

bool QMFService::retrieve(const QMessageId &messageId, const QMessageContentContainerId& id)
{
    if (isBusy()) {
        return false;
    }
    m_active = 0;

    QMailMessagePart::Location location(convert(id));

    if (!location.isValid()) {
        setError(QMessageManager::InvalidId);
        qWarning() << "Invalid message part location";
        return false;
    }
    
    setError(QMessageManager::NoError);
    m_active = &m_retrieval;
    m_retrieval.retrieveMessagePart(location);
    return true;

    Q_UNUSED(messageId)
}

bool QMFService::show(const QMessageId& id)
{
    if (isBusy()) {
        return false;
    }
    m_active = 0;
    // TODO: To be implemented by integrator
    setError(QMessageManager::NotYetImplemented);
    qWarning() << "QMFService::show not yet implemented";
    return false;

    Q_UNUSED(id)
}

bool QMFService::exportUpdates(const QMessageAccountId &id)
{
    if (isBusy()) {
        return false;
    }
    m_active = 0;

    QMailAccountId accountId(convert(id));
    if (!accountId.isValid()) {
        setError(QMessageManager::InvalidId);
        qWarning() << "Account ID is not valid";
        return false;
    }

    setError(QMessageManager::NoError);
    m_active = &m_retrieval;
    m_retrieval.exportUpdates(accountId);
    return true;
}

QMessageService::State QMFService::state() const
{
    if (m_active) {
        return convert(m_active->activity());
    } else if(m_activeStoreAction) {
        return QMessageService::ActiveState;
    }
    return QMessageService::FinishedState;
}

void QMFService::cancel()
{
    if (m_active) {
        m_active->cancelOperation();
    } else if (m_activeStoreAction) {
        m_activeStoreAction = false;
    }
}

bool QMFService::isBusy() const
{
    if ((m_active && ((m_active->activity() == QMailServiceAction::Pending) || (m_active->activity() == QMailServiceAction::InProgress))) ||
        m_activeStoreAction) {
        qWarning() << "Action is currently busy";
        return true;
    }
    return false;
}

void QMFService::transmitActivityChanged(QMailServiceAction::Activity a)
{
    if (a == QMailServiceAction::Successful) {
        if (!m_transmitIds.isEmpty()) {
            // If these messages were transmitted, we need to move them to Sent folder
            QMailMessageKey filter(QMailMessageKey::id(m_transmitIds));
            foreach (const QMailMessageId &id, QMailStore::instance()->queryMessages(filter & QMailMessageKey::status(QMailMessage::Sent))) {
                QMessage message(convert(id));

                QMessagePrivate::setStandardFolder(message, QMessage::SentFolder);
                if (!QMessageManager().updateMessage(&message)) {
                    qWarning() << "Unable to mark message as sent!";
                }
            }
            m_transmitIds.clear();
        }
    } else if (a == QMailServiceAction::Failed) {
        m_transmitIds.clear();
	if (error() == QMessageManager::NoError) {
            // We may have failed due to some part of the request remaining incomplete
            setError(QMessageManager::RequestIncomplete);
        }
    }
    stateChanged(convert(a));
}

void QMFService::statusChanged(const QMailServiceAction::Status &s)
{
    if (s.errorCode != QMailServiceAction::Status::ErrNoError) {
        qWarning() << QString("Service error %1: \"%2\"").arg(s.errorCode).arg(s.text);
        if (s.errorCode == QMailServiceAction::Status::ErrNotImplemented) {
            setError(QMessageManager::NotYetImplemented);
        } else if ((s.errorCode == QMailServiceAction::Status::ErrNonexistentMessage) ||
                   (s.errorCode == QMailServiceAction::Status::ErrEnqueueFailed) ||
                   (s.errorCode == QMailServiceAction::Status::ErrInvalidAddress) ||
                   (s.errorCode == QMailServiceAction::Status::ErrInvalidData)) {
            setError(QMessageManager::ConstraintFailure);
        } else {
            setError(QMessageManager::FrameworkFault);
        }
    }
}

void QMFService::retrievalActivityChanged(QMailServiceAction::Activity a)
{
    if ((a == QMailServiceAction::Failed) && (error() == QMessageManager::NoError)) {
        // We may have failed due to some part of the request remaining incomplete
        setError(QMessageManager::RequestIncomplete);
    }
    stateChanged(convert(a));
}

void QMFService::completed()
{
    m_activeStoreAction = false;
    stateChanged(QMessageService::FinishedState);
}

bool QMFService::messageMatch(const QMailMessageId &messageId)
{
    if (m_match.isEmpty()) {
        return true;
    }

    const QMailMessage candidate(messageId);
    if (candidate.id().isValid()) {
	Qt::CaseSensitivity cs = (m_matchFlags & QMessageDataComparator::MatchCaseSensitive) ? Qt::CaseSensitive : Qt::CaseInsensitive;
        // Search only messages or message parts that are of type 'text/*'
        if (candidate.hasBody()) {
            if (candidate.contentType().type().toLower() == "text") {
		if (candidate.body().data().contains(m_match, cs))
                    return true;
            }
        } else if (candidate.multipartType() != QMailMessage::MultipartNone) {
            // Search all 'text/*' parts within this message
            TextPartSearcher searcher(m_match, cs);
            if (candidate.foreachPart<TextPartSearcher&>(searcher) == false) {
                // We found a matching part in the message
                return true;
            }
        }
    }

    return false;
}

void QMFService::reportMatchingIds()
{
    QMessageServicePrivate *p = QMessageServicePrivate::implementation(*m_service);
    // QMessageServicePrivate emits QMessageService::messagesFound() signal 
    p->messagesFound(convert(m_candidateIds), true, true);
    m_activeStoreAction = false;
}

void QMFService::reportMatchingCount()
{
    QMessageServicePrivate *p = QMessageServicePrivate::implementation(*m_service);
    // QMessageServicePrivate emits QMessageService::messagesCounted() signal 
    p->messagesCounted(m_candidateIds.count());
    m_activeStoreAction = false;
}

void QMFService::matchQueryCompleted()
{
    QMessageServicePrivate *p = QMessageServicePrivate::implementation(*m_service);
    // QMessageServicePrivate emits QMessageService::messagesFound() signal 
    p->messagesFound(m_resultIds, true, true);
    m_activeStoreAction = false;
    m_resultIds.clear();
}

void QMFService::findMatchingIds()
{
    int required = ((m_offset + m_limit) - m_matchingIds.count());

    // Are any of the existing IDs part of the result set?
    if (required < m_limit) {
	m_resultIds = m_matchingIds.mid(m_offset, m_limit);
    }

    if ((required > 0 || m_limit == 0) && !m_candidateIds.isEmpty()) {
        QTimer::singleShot(0, this, SLOT(testNextMessage()));
    } else {
	matchQueryCompleted();
    }
}

void QMFService::testNextMessage()
{
    int required = ((m_offset + m_limit) - m_matchingIds.count());
    if (required > 0 || m_limit==0) {
        QMailMessageId messageId(m_candidateIds.takeFirst());
        if (messageMatch(messageId)) {
            m_matchingIds.append(convert(messageId));
            --required;

            if (required < m_limit) {
                // This is within the result set
		m_resultIds << m_matchingIds.last();
	    }
        }

        if ((required > 0 || m_limit == 0) && !m_candidateIds.isEmpty()) {
            QTimer::singleShot(0, this, SLOT(testNextMessage()));
            return;
        }
    }
    matchQueryCompleted();
}

QMessageManager::Error QMFService::error() const
{
    return m_service->error();
}

void QMFService::setError(QMessageManager::Error error) const
{
    QMessageServicePrivate *p = QMessageServicePrivate::implementation(*m_service);
    p->_error = error;
}

void QMFService::stateChanged(QMessageService::State state) const
{
    QMessageServicePrivate *p = QMessageServicePrivate::implementation(*m_service);
    // QMessageServicePrivate emits QMessageService::stateChanged() signal 
    p->stateChanged(state);
}


#include "moc_qmfservice_maemo6_p.cpp"

QTM_END_NAMESPACE
