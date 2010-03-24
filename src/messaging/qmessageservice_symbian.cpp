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
#include "symbianhelpers_p.h"
#ifdef FREESTYLEMAILUSED
#include "qfsengine_symbian_p.h"
#endif

QTM_BEGIN_NAMESPACE

QMessageServicePrivate::QMessageServicePrivate(QMessageService* parent)
 : q_ptr(parent),
   _state(QMessageService::InactiveState),
   _active(false)
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
    if (SymbianHelpers::isFreestyleAccount(message.parentAccountId())) {
#ifdef FREESTYLEMAILUSED
        return CFSEngine::instance()->sendEmail(message);
#else
        return false;
#endif
    } else
        return CMTMEngine::instance()->sendEmail(message);
}

bool QMessageServicePrivate::show(const QMessageId& id)
{
    QMessageId fsId = id;
    if (SymbianHelpers::isFreestyleMessage(fsId)) {
#ifdef FREESTYLEMAILUSED
        return CFSEngine::instance()->showMessage(id);
#else
        return false;
#endif
    } else
        return CMTMEngine::instance()->showMessage(id);
}

bool QMessageServicePrivate::compose(const QMessage &message)
{
	return CMTMEngine::instance()->composeMessage(message);
}

bool QMessageServicePrivate::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return CMTMEngine::instance()->queryMessages((QMessageServicePrivate&)*this, filter, sortOrder, limit, offset);
}

bool QMessageServicePrivate::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return CMTMEngine::instance()->queryMessages((QMessageServicePrivate&)*this, filter, body, matchFlags, sortOrder, limit, offset);
}

bool QMessageServicePrivate::countMessages(const QMessageFilter &filter)
{
    return CMTMEngine::instance()->countMessages((QMessageServicePrivate&)*this, filter);
}

bool QMessageServicePrivate::retrieve(const QMessageId &messageId, const QMessageContentContainerId &id)
{
    if (SymbianHelpers::isFreestyleMessage(messageId)) {
#ifdef FREESTYLEMAILUSED
        return CFSEngine::instance()->retrieve(messageId, id);
#else
        return false;
#endif
    } else
        return CMTMEngine::instance()->retrieve(messageId, id);
}

bool QMessageServicePrivate::retrieveBody(const QMessageId& id)
{
    if (SymbianHelpers::isFreestyleMessage(id)) {
#ifdef FREESTYLEMAILUSED
        return CFSEngine::instance()->retrieveBody(id);
#else
        return false;
#endif
    } else
        return CMTMEngine::instance()->retrieveBody(id);
}

bool QMessageServicePrivate::retrieveHeader(const QMessageId& id)
{
    if (SymbianHelpers::isFreestyleMessage(id)) {
#ifdef FREESTYLEMAILUSED
        return CFSEngine::instance()->retrieveHeader(id);
#else
        return false;
#endif
    } else
        return CMTMEngine::instance()->retrieveHeader(id);
}

bool QMessageServicePrivate::exportUpdates(const QMessageAccountId &id)
{
  //  if (SymbianHelpers::isFreestyleMessage(id))
  //      return CFSEngine::instance()->exportUpdates(id);
  //  else
        return CMTMEngine::instance()->exportUpdates(id);
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
    connect(d_ptr, SIGNAL(messagesCounted(int)), this, SIGNAL(messagesCounted(int)));
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
        QMessage outgoing(message);
    
        // Set default account if unset
        if (!outgoing.parentAccountId().isValid()) {
            outgoing.setParentAccountId(accountId);
        }
        
        if (outgoing.type() == QMessage::AnyType || outgoing.type() == QMessage::NoType) {
            outgoing.setType(msgType);
        }

        if (account.messageTypes() & QMessage::Sms) {
            retVal = d_ptr->sendSMS(outgoing);
        } else if (account.messageTypes() & QMessage::Mms) {
            retVal = d_ptr->sendMMS(outgoing);
        } else if (account.messageTypes() & QMessage::Email) {
            retVal = d_ptr->sendEmail(outgoing);
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
	
    d_ptr->setFinished(retVal);
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
	
    d_ptr->setFinished(retVal);
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
	
    d_ptr->setFinished(retVal);
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
    
    d_ptr->setFinished(retVal);
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
