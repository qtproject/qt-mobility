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
#include <QDebug>

#include "qmessageservice.h"
#include "qmessageservice_maemo6_p.h"

#include "maemo6helpers_p.h"
#include "storageengine_maemo6_p.h"
#include "telepathyengine_maemo6_p.h"

QTM_BEGIN_NAMESPACE

QMessageServicePrivate::QMessageServicePrivate(QMessageService *parent)
    : q_ptr(parent),
   _qmfService(new QMFService(parent)),
   _state(QMessageService::InactiveState),
   _error(QMessageManager::NoError),
   _active(false), _actionId(-1),
   _pendingRequestCount(0)
{

}

QMessageServicePrivate::~QMessageServicePrivate()
{
    delete _qmfService;
}

QMessageServicePrivate* QMessageServicePrivate::implementation(const QMessageService &service)
{
    return service.d_ptr;
}

bool QMessageServicePrivate::queryMessages(QMessageService &messageService,
                                           const QMessageFilter &filter,
                                           const QMessageSortOrder &sortOrder,
                                           uint limit, uint offset,
                                           EnginesToCall enginesToCall)
{
    if (_active) {
        return false;
    }

    _filter = filter;
    MessagingHelper::handleNestedFiltersFromMessageFilter(_filter);

    _ids.clear();
    _sorted = true;
    _filtered = true;

    _active = true;
    _error = QMessageManager::NoError;

    _pendingRequestCount = 0;
    if (enginesToCall & EnginesToCallQMF) {
	if (_qmfService->queryMessages(_filter, sortOrder, limit, offset)) {
            _pendingRequestCount++;
        }
    }

    if (enginesToCall & EnginesToCallTelepathy) {
        if (StorageEngine::instance()->queryMessages(&messageService, _filter, sortOrder, limit, offset)) {
            _pendingRequestCount++;
	}
    }

    if (_pendingRequestCount > 0) {
        _sortOrder = sortOrder;
        _limit = limit;
        _offset = offset;

        _state = QMessageService::ActiveState;
        emit messageService.stateChanged(_state);
    } else {
        setFinished(false);
    }

    return _active;
}

bool QMessageServicePrivate::queryMessages(QMessageService &messageService,
                                           const QMessageFilter &filter,
                                           const QString &body,
                                           QMessageDataComparator::MatchFlags matchFlags,
                                           const QMessageSortOrder &sortOrder,
                                           uint limit, uint offset,
                                           EnginesToCall enginesToCall)
{
    if (_active) {
        return false;
    }

    _filter = filter;
    MessagingHelper::handleNestedFiltersFromMessageFilter(_filter);

    _ids.clear();
    _sorted = true;
    _filtered = true;

    _active = true;
    _error = QMessageManager::NoError;

    _pendingRequestCount = 0;
    if (enginesToCall & EnginesToCallQMF) {
	if (_qmfService->queryMessages(_filter, body, matchFlags,
				       sortOrder, limit, offset)) {
            _pendingRequestCount++;
        } 
    }

    if (enginesToCall & EnginesToCallTelepathy) {

        if (StorageEngine::instance()->queryMessages(&messageService, _filter, body, matchFlags,
                                       sortOrder, limit, offset)) {
            _pendingRequestCount++;
	}
    }

    if (_pendingRequestCount > 0) {
        _sortOrder = sortOrder;
        _limit = limit;
        _offset = offset;

        _state = QMessageService::ActiveState;
        emit stateChanged(_state);
    } else {
        setFinished(false);
    }

    return _active;
}

bool QMessageServicePrivate::countMessages(QMessageService &messageService,
                                           const QMessageFilter &filter,
                                           EnginesToCall enginesToCall)
{
    if (_active) {
        return false;
    }

    QMessageFilter handledFilter = filter;
    MessagingHelper::handleNestedFiltersFromMessageFilter(handledFilter);

    _count = 0;

    _active = true;
    _error = QMessageManager::NoError;

    _pendingRequestCount = 0;
    if (enginesToCall & EnginesToCallQMF) {
        if (_qmfService->countMessages(handledFilter)) {
            _pendingRequestCount++;
        }
    }

    if (enginesToCall & EnginesToCallTelepathy) {
        if (StorageEngine::instance()->countMessages(&messageService, handledFilter)) {
                _pendingRequestCount++;
	}
    }

    if (_pendingRequestCount > 0) {
        _state = QMessageService::ActiveState;
        emit stateChanged(_state);
    } else {
        setFinished(false);
    }

    return _active;
}

void QMessageServicePrivate::setFinished(bool successful)
{
    if (!successful && _pendingRequestCount > 0) {
        _pendingRequestCount--;
    }

    if (_pendingRequestCount == 0) {
        if (!successful && (_error == QMessageManager::NoError)) {
            // We must report an error of some sort
            _error = QMessageManager::RequestIncomplete;
        }

        _state = QMessageService::FinishedState;
        emit q_ptr->stateChanged(_state);
        _active = false;
    }
}

void QMessageServicePrivate::stateChanged(QMessageService::State state)
{
    _state = state;
    emit q_ptr->stateChanged(state);
}

void QMessageServicePrivate::cancel()
{
    if (!_active) 
	return;
    
    _active = false;
    _error = QMessageManager::NoError;
    _pendingRequestCount = 0;

    QObjectList deleteList;
    foreach (QObject *child, q_ptr->children()) {
	if (qobject_cast<ServiceQuery *>(child) || qobject_cast<SendRequest *>(child)) {
	    deleteList << child;
	}
    }
    qDeleteAll(deleteList);
    
    _qmfService->cancel();

    stateChanged(QMessageService::CanceledState);
}

void QMessageServicePrivate::messagesFound(const QMessageIdList &ids, bool isFiltered, bool isSorted)
{
    if (!_active)
	return;

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

        qDebug() << __PRETTY_FUNCTION__ << "emiting messagesFound()" ;
        emit q_ptr->messagesFound(_ids);

        setFinished(true);

        _ids.clear();
        _filter = QMessageFilter();
        _sortOrder = QMessageSortOrder();
    }
}

void QMessageServicePrivate::messagesCounted(int count)
{
    if (!_active)
	return;

    _pendingRequestCount--;
    _count += count;
    if (_pendingRequestCount == 0) {
        emit q_ptr->messagesCounted(_count);
        setFinished(true);
        _count = 0;
    }
}

bool QMessageServicePrivate::compose(const QMessage &message)
{
    if (_active) {
        return false;
    }

    _active = true;
    _error = QMessageManager::NoError;

    bool retVal = true;
    _state = QMessageService::ActiveState;
    emit stateChanged(_state);

    if (message.type() == QMessage::Sms) {
        retVal = StorageEngine::instance()->compose(message);
    } else if (message.type() == QMessage::Mms) {
        _error = QMessageManager::NotYetImplemented; //TODO:
        qWarning() << "QMessageService::compose not yet implemented for MMS";
        retVal = false;
    } else if (message.type() == QMessage::Email) {
        retVal = _qmfService->compose(message);
    }

    setFinished(retVal);
    return retVal;
}

bool QMessageServicePrivate::show(const QMessageId &id)
{
    if (_active) {
        return false;
    }

    if (!id.isValid()) {
        _error = QMessageManager::InvalidId;
        return false;
    }

    _active = true;
    _error = QMessageManager::NoError;

    bool retVal = true;
    _state = QMessageService::ActiveState;
    emit stateChanged(_state);

    if (id.toString().startsWith("QMF_")) {
        retVal = _qmfService->show(id);
    } else if(id.toString().startsWith("SMS_")) {
        retVal = StorageEngine::instance()->show(id);
    }
    else {
        retVal = false;
    }

    setFinished(retVal);
    return retVal;
}

void QMessageServicePrivate::progressChanged(uint value, uint total)
{
    emit q_ptr->progressChanged(value, total);
}

QMessageService::QMessageService(QObject *parent)
    : QObject(parent),
      d_ptr(new QMessageServicePrivate(this))
{
}

QMessageService::~QMessageService()
{
}

bool QMessageService::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    return d_ptr->queryMessages(*this, filter, sortOrder, limit, offset);
}

bool QMessageService::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset)
{
    return d_ptr->queryMessages(*this, filter, body, matchFlags, sortOrder, limit, offset);
}

bool QMessageService::countMessages(const QMessageFilter &filter)
{
    return d_ptr->countMessages(*this, filter);
}

bool QMessageService::send(QMessage &message)
{
    if (d_ptr->_active) {
	qWarning() << "error: QMessageService is active";
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
            } else if (account.messageTypes() & QMessage::InstantMessage) {
                msgType = QMessage::InstantMessage;
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
                qDebug() << __FUNCTION__ << "Cannot obtain default account";
                retVal = false;
            }
        }
    }

    QMessageAccount account(accountId);
    qDebug() << " message.type(): " <<  message.type() << " msgType: "<< msgType;

    if (retVal) {
        // Check account/message type compatibility
        if (!(account.messageTypes() & message.type()) && (msgType == QMessage::NoType)) {
            qDebug() << __FUNCTION__ << "Account does not support SMS sending";
            d_ptr->_error = QMessageManager::ConstraintFailure;
            retVal = false;
        }
    }

    if (retVal) {
        // Check recipients
        QMessageAddressList recipients = message.to() + message.bcc() + message.cc();
        if (recipients.isEmpty()) {
            d_ptr->_error = QMessageManager::ConstraintFailure;
            retVal = false;
        }
    }

    if (retVal) {
        QMessage outgoing(message);

        // Set default account if unset
        if (!outgoing.parentAccountId().isValid()) {
            outgoing.setParentAccountId(accountId);
        }

        if (account.messageTypes() & QMessage::Sms) {
	    retVal = TelepathyEngine::instance()->sendMessage(message, this);
        } else if (account.messageTypes() & QMessage::InstantMessage) {
            retVal = TelepathyEngine::instance()->sendMessage(message, this);
        } else if (account.messageTypes() & QMessage::Mms) {
            d_ptr->_error = QMessageManager::NotYetImplemented;
            qWarning() << "QMessageService::send not yet implemented for MMS";
            retVal = false;
        } else if (account.messageTypes() & QMessage::Email) {
            retVal = d_ptr->_qmfService->send(message);
	}
    }

    if (!retVal)
	d_ptr->setFinished(retVal);

    return retVal;
}

bool QMessageService::compose(const QMessage &message)
{
    return d_ptr->compose(message);
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

    // QMFService might emit signals so this value must be true
    d_ptr->_active = true;

    bool retVal(false);
    // This operation is only relevant to email implementation
    if (id.toString().startsWith("QMF_")) {
	retVal = d_ptr->_qmfService->retrieveHeader(id);
    }

    if (!retVal) // No longer active
	d_ptr->_active = false;
    
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

    // QMFService might emit signals so this value must be true
    d_ptr->_active = true;

    bool retVal(false);
    // This operation is only relevant to email implementation
    if (id.toString().startsWith("QMF_")) {
	retVal = d_ptr->_qmfService->retrieveBody(id);
    }

    if (!retVal)  // No longer active
	d_ptr->_active = false;
    
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

    // QMFService might emit signals so this value must be true
    d_ptr->_active = true;

    bool retVal(false);
    // This operation is only relevant to email implementation
    if (messageId.toString().startsWith("QMF_")) {
	retVal = d_ptr->_qmfService->retrieve(messageId, id);
    }

    if (!retVal) // No longer active
	d_ptr->_active = false;
    
    return retVal;
}

bool QMessageService::show(const QMessageId& id)
{
    return d_ptr->show(id);
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

    // QMFService might emit signals so this value must be true
    d_ptr->_active = true;

    bool retVal(false);
    // This operation is only relevant to email implementation
    if (id.toString().startsWith("QMF_")) {
        retVal = d_ptr->_qmfService->exportUpdates(id);
    }

    if (!retVal) // No longer active
	d_ptr->_active = false;

    return retVal;
}

QMessageService::State QMessageService::state() const
{
    return d_ptr->_state;
}

void QMessageService::cancel()
{
    d_ptr->cancel();
}

QMessageManager::Error QMessageService::error() const
{
    return d_ptr->_error;
}

QTM_END_NAMESPACE
