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
#include <smsclnt.h>
#include <smuthdr.h>        // CSmsHeader
#include <rsendas.h>
#include <rsendasmessage.h>
#include <f32file.h>
#include <mmsconst.h>
#include <utf.h>            // CnvUtfConverter

#include "qmessageservice.h"
#include "qmessageservice_symbian_p.h"
#include "qmtmengine_symbian_p.h"
#include "qmessage_symbian_p.h"
#include "messagingutil_p.h"
#include "maemohelpers_p.h" // contains non-meamo specific helpers for messaging
#ifdef FREESTYLEMAILUSED
#include "qfsengine_symbian_p.h"
#endif
#ifdef FREESTYLENMAILUSED
#include "qfsnmengine_symbian_p.h"
#endif

QTM_BEGIN_NAMESPACE

using namespace SymbianHelpers;

QMessageServicePrivate::QMessageServicePrivate(QMessageService* parent)
 : q_ptr(parent),
   _state(QMessageService::InactiveState),
   _active(false),
   _pendingRequestCount(0)
{
}
      
QMessageServicePrivate::~QMessageServicePrivate()
{
}

bool QMessageServicePrivate::sendSMS(QMessage &message)
{
    return CMTMEngine::instance()->sendSMS(message);
}

bool QMessageServicePrivate::sendMMS(QMessage &message)
{
    return CMTMEngine::instance()->sendMMS(message);
}

bool QMessageServicePrivate::sendEmail(QMessage &message)
{
    switch (idType(message.parentAccountId())) {
        case EngineTypeFreestyle:
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
            return CFSEngine::instance()->sendEmail(message);
#else
            return false;
#endif
            break;
        case EngineTypeMTM:
        default:
            return CMTMEngine::instance()->sendEmail(message);
            break;
    }
}

bool QMessageServicePrivate::show(const QMessageId& id)
{
    switch (idType(id)) {
        case EngineTypeFreestyle:
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
            return CFSEngine::instance()->showMessage(id);
#else
            return false;
#endif
            break;
        case EngineTypeMTM:
        default:
            return CMTMEngine::instance()->showMessage(id);
            break;
    }
}

bool QMessageServicePrivate::compose(const QMessage &message)
{
    switch (idType(message.parentAccountId())) {
        case EngineTypeFreestyle:
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
            return CFSEngine::instance()->composeMessage(message);
#else
            return false;
#endif
            break;
        case EngineTypeMTM:
        default:
            return CMTMEngine::instance()->composeMessage(message);
            break;
    }
}

bool QMessageServicePrivate::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    if (_pendingRequestCount > 0) {
        return false;
    }
    _pendingRequestCount = 0;
    _active = true;
    _filter = filter;
    _sortOrder = sortOrder;
    _limit = limit;
    _offset = offset;
    _filtered = true;
    _sorted = true;

    _pendingRequestCount++;
    CMTMEngine::instance()->queryMessages((QMessageServicePrivate&)*this, filter, sortOrder, 0, 0);

#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
    _pendingRequestCount++;
    CFSEngine::instance()->queryMessages((QMessageServicePrivate&)*this, filter, sortOrder, 0, 0);
#endif

    return _active;
}

bool QMessageServicePrivate::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    if (_pendingRequestCount > 0) {
        return false;
    }
    _pendingRequestCount = 0;
    _active = true;
    _filter = filter;
    _sortOrder = sortOrder;
    _limit = limit;
    _offset = offset;
    _filtered = true;
    _sorted = true;
    
    _pendingRequestCount++;
    CMTMEngine::instance()->queryMessages((QMessageServicePrivate&)*this, filter, body, matchFlags, sortOrder, 0, 0);

#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
    _pendingRequestCount++;
    CFSEngine::instance()->queryMessages((QMessageServicePrivate&)*this, filter, body, matchFlags, sortOrder, 0, 0);
#endif

    return _active;
}

bool QMessageServicePrivate::countMessages(const QMessageFilter &filter)
{
    if (_pendingRequestCount > 0) {
        return false;
    }
    _pendingRequestCount = 0;
    _active = true;
    _count = 0;

    _pendingRequestCount++;
    CMTMEngine::instance()->countMessages((QMessageServicePrivate&)*this, filter);

#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
    _pendingRequestCount++;
    CFSEngine::instance()->countMessages((QMessageServicePrivate&)*this, filter);
#endif
    return _active;
}

bool QMessageServicePrivate::retrieve(const QMessageId &messageId, const QMessageContentContainerId &id)
{
    switch (idType(messageId)) {
        case EngineTypeFreestyle:
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
            return CFSEngine::instance()->retrieve(*this, messageId, id);
#else
            return false;
#endif
            break;
        case EngineTypeMTM:
        default:
            return CMTMEngine::instance()->retrieve(*this, messageId, id);
            break;
    }
}

bool QMessageServicePrivate::retrieveBody(const QMessageId& id)
{
    switch (idType(id)) {
        case EngineTypeFreestyle:
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
            return CFSEngine::instance()->retrieveBody(*this, id);
#else
            return false;
#endif
            break;
        case EngineTypeMTM:
        default:
            return CMTMEngine::instance()->retrieveBody(*this, id);
            break;
    }
}

bool QMessageServicePrivate::retrieveHeader(const QMessageId& id)
{
    switch (idType(id)) {
        case EngineTypeFreestyle:
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
            return CFSEngine::instance()->retrieveHeader(*this, id);
#else
            return false;
#endif
            break;
        case EngineTypeMTM:
        default:
            return CMTMEngine::instance()->retrieveHeader(*this, id);
            break;
    }
}

void QMessageServicePrivate::messagesFound(const QMessageIdList &ids, bool isFiltered, bool isSorted)
{
    _pendingRequestCount--;

    if (!isFiltered) {
        _filtered = false;
    }

    if (!isSorted) {
        _sorted = false;
    } else {
        if ((ids.count() > 0) && (_ids.count() > 0)) {
            _sorted = false;
        }
    }

    _ids.append(ids);

    if (_pendingRequestCount == 0) {
        if (!_filtered) {
            MessagingHelper::filterMessages(_ids, _filter);
        }
        if (!_sorted) {
            MessagingHelper::orderMessages(_ids, _sortOrder);
        }
        MessagingHelper::applyOffsetAndLimitToMessageIdList(_ids, _limit, _offset);

        emit q_ptr->messagesFound(_ids);

        setFinished(true);

        _ids.clear();
        _filter = QMessageFilter();
        _sortOrder = QMessageSortOrder();
    }
}

void QMessageServicePrivate::messagesCounted(int count)
{
    _pendingRequestCount--;

    _count += count;

    if (_pendingRequestCount == 0) {

        emit q_ptr->messagesCounted(_count);

        setFinished(true);

        _count = 0;
    }
}

bool QMessageServicePrivate::exportUpdates(const QMessageAccountId &id)
{
    switch (idType(id)) {
            case EngineTypeFreestyle:
#if defined(FREESTYLEMAILUSED) || defined(FREESTYLENMAILUSED)
                return CFSEngine::instance()->exportUpdates(id);
#else
                return false;
#endif
            case EngineTypeMTM:
            default:
                return CMTMEngine::instance()->exportUpdates(*this, id);
    }
}

void QMessageServicePrivate::setFinished(bool successful)
{
    if (!successful && (_error == QMessageManager::NoError)) {
        // We must report an error of some sort
        _error = QMessageManager::RequestIncomplete;
    }

    _state = QMessageService::FinishedState;
    emit q_ptr->stateChanged(_state);
    _active = false;
}

QMessageService::QMessageService(QObject *parent)
    : QObject(parent),
    d_ptr(new QMessageServicePrivate(this))
{
	connect(d_ptr, SIGNAL(stateChanged(QMessageService::State)), this, SIGNAL(stateChanged(QMessageService::State)));
	connect(d_ptr, SIGNAL(messagesFound(const QMessageIdList&)), this, SIGNAL(messagesFound(const QMessageIdList&)));
    //connect(d_ptr, SIGNAL(messagesCounted(int)), this, SIGNAL(messagesCounted(int)));
	connect(d_ptr, SIGNAL(progressChanged(uint, uint)), this, SIGNAL(progressChanged(uint, uint)));
}

QMessageService::~QMessageService()
{
}

bool QMessageService::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    if (d_ptr->_active) {
        return false;
    }
    
    d_ptr->_active = true;
    d_ptr->_error = QMessageManager::NoError;
    
    if (d_ptr->queryMessages(filter, sortOrder, limit, offset)) {
        d_ptr->_state = QMessageService::ActiveState;
        emit stateChanged(d_ptr->_state);
    } else {
        d_ptr->setFinished(false);
    }
    
    return d_ptr->_active;
}

bool QMessageService::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    if (d_ptr->_active) {
        return false;
    }

    d_ptr->_active = true;
    d_ptr->_error = QMessageManager::NoError;

    if (d_ptr->queryMessages(filter, body, matchFlags, sortOrder, limit, offset)) {
        d_ptr->_state = QMessageService::ActiveState;
        emit stateChanged(d_ptr->_state);
    } else {
        d_ptr->setFinished(false);
    }

    return d_ptr->_active;
}

bool QMessageService::countMessages(const QMessageFilter &filter)
{
    if (d_ptr->_active) {
        return false;
    }
    
    d_ptr->_active = true;
    d_ptr->_error = QMessageManager::NoError;
    
    if (d_ptr->countMessages(filter)) {
        d_ptr->_state = QMessageService::ActiveState;
        emit stateChanged(d_ptr->_state);
    } else {
        d_ptr->setFinished(false);
    }
    
    return d_ptr->_active;
}

bool QMessageService::send(QMessage &message)
{
	if (d_ptr->_active) {
		return false;
	}
	
	d_ptr->_active = true;
	d_ptr->_error = QMessageManager::NoError;
	
    bool retVal = true;	
    
    d_ptr->_state = QMessageService::ActiveState;
    emit stateChanged(d_ptr->_state);
    
    QMessageAccountId accountId = message.parentAccountId();
    QMessage::Type msgType = QMessage::NoType;

    // Check message type
    if (message.type() == QMessage::AnyType || message.type() == QMessage::NoType) {
        QMessage::TypeFlags types = QMessage::NoType;
        if (accountId.isValid()) {
            // ParentAccountId was defined => Message type can be read
            // from parent account
            QMessageAccount account = QMessageAccount(accountId);
            QMessage::TypeFlags types = account.messageTypes();
            if (types & QMessage::Sms) {
                msgType = QMessage::Sms;
            } else if (types & QMessage::Mms) {
                msgType = QMessage::Mms;
            } else if (types & QMessage::Email) {
                msgType = QMessage::Email;
            }
        }
        if (msgType == QMessage::NoType) {
            d_ptr->_error = QMessageManager::ConstraintFailure;
            retVal = false;
        }
    }

    if (retVal) {
        // Check account
        if (!accountId.isValid()) {
            accountId = QMessageAccount::defaultAccount(message.type());
            if (!accountId.isValid()) {
                d_ptr->_error = QMessageManager::InvalidId;
                retVal = false;
            }
        }
    }

    QMessageAccount account(accountId);
    if (retVal) {
        // Check account/message type compatibility
        if (!(account.messageTypes() & message.type()) && (msgType == QMessage::NoType)) {
            d_ptr->_error = QMessageManager::ConstraintFailure;
            retVal = false;
        }
    }
    
    if (retVal) {
        // Check recipients
        QMessageAddressList recipients = message.to() + message.bcc() + message.cc();
        if (recipients.isEmpty()) {
            d_ptr->_error = QMessageManager::ConstraintFailure;
            return false;
        }
    }
    
    if (retVal) {
        // Set default account if unset
        if (!message.parentAccountId().isValid()) {
            message.setParentAccountId(accountId);
        }
        
        if (message.type() == QMessage::AnyType || message.type() == QMessage::NoType) {
            message.setType(msgType);
        }

        if (account.messageTypes() & QMessage::Sms) {
            retVal = d_ptr->sendSMS(message);
        } else if (account.messageTypes() & QMessage::Mms) {
            retVal = d_ptr->sendMMS(message);
        } else if (account.messageTypes() & QMessage::Email) {
            retVal = d_ptr->sendEmail(message);
        }
    }
    
    d_ptr->setFinished(retVal);
    return retVal;
}

bool QMessageService::compose(const QMessage &message)
{
	if (d_ptr->_active) {
		return false;
	}
	
	d_ptr->_active = true;
	d_ptr->_error = QMessageManager::NoError;
	
	bool retVal = true;
	d_ptr->_state = QMessageService::ActiveState;
	emit stateChanged(d_ptr->_state);
	
	retVal = d_ptr->compose(message);
	
    d_ptr->setFinished(retVal);
    return retVal;
}

bool QMessageService::retrieveHeader(const QMessageId& id)
{
	if (d_ptr->_active) {
		return false;
	}
	
    if (!id.isValid()) {
        d_ptr->_error = QMessageManager::InvalidId;
        return false;
    }
    
	d_ptr->_active = true;
	d_ptr->_error = QMessageManager::NoError;
	
	bool retVal = true;
	d_ptr->_state = QMessageService::ActiveState;
	emit stateChanged(d_ptr->_state);

	retVal = d_ptr->retrieveHeader(id);
	if (retVal == false) {
	    d_ptr->setFinished(retVal);
	}
	
    return retVal;
}

bool QMessageService::retrieveBody(const QMessageId& id)
{
	if (d_ptr->_active) {
		return false;
	}
	
    if (!id.isValid()) {
        d_ptr->_error = QMessageManager::InvalidId;
        return false;
    }
    
	d_ptr->_active = true;
	d_ptr->_error = QMessageManager::NoError;
	
	bool retVal = true;
	d_ptr->_state = QMessageService::ActiveState;
	emit stateChanged(d_ptr->_state);

	retVal = d_ptr->retrieveBody(id);
	if (retVal == false) {
        d_ptr->setFinished(retVal);
    }
	
    return retVal;
}

bool QMessageService::retrieve(const QMessageId &messageId, const QMessageContentContainerId& id)
{
	if (d_ptr->_active) {
		return false;
	}
	
    if (!messageId.isValid() || !id.isValid()) {
        d_ptr->_error = QMessageManager::InvalidId;
        return false;
    }
    
	d_ptr->_active = true;
	d_ptr->_error = QMessageManager::NoError;

	bool retVal = true;
	d_ptr->_state = QMessageService::ActiveState;
	emit stateChanged(d_ptr->_state);

	retVal = d_ptr->retrieve(messageId, id);
    if (retVal == false) {
        d_ptr->setFinished(retVal);
    }
	
    return retVal;
}

bool QMessageService::show(const QMessageId& id)
{
	if (d_ptr->_active) {
		return false;
	}
	
    if (!id.isValid()) {
        d_ptr->_error = QMessageManager::InvalidId;
        return false;
    }
    
	d_ptr->_active = true;
	d_ptr->_error = QMessageManager::NoError;
	
	bool retVal = true;
	d_ptr->_state = QMessageService::ActiveState;
	emit stateChanged(d_ptr->_state);

    retVal = d_ptr->show(id);
    
    d_ptr->setFinished(retVal);
    return retVal;
}

bool QMessageService::exportUpdates(const QMessageAccountId &id)
{
    if (d_ptr->_active) {
        return false;
    }
    
    if (!id.isValid()) {
        d_ptr->_error = QMessageManager::InvalidId;
        return false;
    }
    
    d_ptr->_active = true;
    d_ptr->_error = QMessageManager::NoError;
    
    bool retVal = true;
    d_ptr->_state = QMessageService::ActiveState;
    emit stateChanged(d_ptr->_state);
    
    retVal = d_ptr->exportUpdates(id);
    if (retVal == false) {
        d_ptr->setFinished(retVal);
    }
    
    return retVal;
}

QMessageService::State QMessageService::state() const
{
	return d_ptr->_state;
}

void QMessageService::cancel()
{
}

QMessageManager::Error QMessageService::error() const
{
    return d_ptr->_error;
}

#include "moc_qmessageservice_symbian_p.cpp"
QTM_END_NAMESPACE
