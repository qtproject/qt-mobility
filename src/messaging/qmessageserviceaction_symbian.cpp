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

#include "qmessageserviceaction.h"
#include "qmessageserviceaction_symbian_p.h"
#include "qmtmengine_symbian_p.h"


QTM_BEGIN_NAMESPACE

QMessageServiceActionPrivate::QMessageServiceActionPrivate(QMessageServiceAction* parent)
 : q_ptr(0),
   _state(QMessageServiceAction::Pending),
   _active(false)
{
	Q_UNUSED(parent);
}
      
QMessageServiceActionPrivate::~QMessageServiceActionPrivate()
{
}

bool QMessageServiceActionPrivate::sendSMS(QMessage &message)
{
	return CMTMEngine::instance()->sendSMS(message);
}

bool QMessageServiceActionPrivate::sendMMS(QMessage &message)
{
	CMTMEngine* mtmEngine = CMTMEngine::instance();
	bool retVal = mtmEngine->storeMMS(message, KMsvGlobalOutBoxIndexEntryId);
	if (retVal){
		mtmEngine->sendMMS();
	}
	return retVal;
}

bool QMessageServiceActionPrivate::sendEmail(QMessage &message)
{
	CMTMEngine* mtmEngine = CMTMEngine::instance();
	bool retVal = mtmEngine->storeEmail(message, KMsvGlobalOutBoxIndexEntryId);
	if (retVal){
		mtmEngine->sendEmail(message);
	}
	return retVal;
}

bool QMessageServiceActionPrivate::show(const QMessageId& id)
{
	return CMTMEngine::instance()->showMessage(id);
}

bool QMessageServiceActionPrivate::compose(const QMessage &message)
{
	return CMTMEngine::instance()->composeMessage(message);
}

bool QMessageServiceActionPrivate::queryMessages(const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    return CMTMEngine::instance()->queryMessages((QMessageServiceActionPrivate&)*this, filter, ordering, limit, offset);
}

bool QMessageServiceActionPrivate::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    return CMTMEngine::instance()->queryMessages((QMessageServiceActionPrivate&)*this, filter, body, options, ordering, limit, offset);
}

bool QMessageServiceActionPrivate::countMessages(const QMessageFilter &filter)
{
    return CMTMEngine::instance()->countMessages((QMessageServiceActionPrivate&)*this, filter);
}

bool QMessageServiceActionPrivate::retrieve(const QMessageId &messageId, const QMessageContentContainerId &id)
{
	return CMTMEngine::instance()->retrieve(messageId, id);
}

bool QMessageServiceActionPrivate::retrieveBody(const QMessageId& id)
{
	return CMTMEngine::instance()->retrieveBody(id);
}

bool QMessageServiceActionPrivate::retrieveHeader(const QMessageId& id)
{
	return CMTMEngine::instance()->retrieveHeader(id);
}

QMessageServiceAction::QMessageServiceAction(QObject *parent)
    : QObject(parent),
    d_ptr(new QMessageServiceActionPrivate(this))
{
	connect(d_ptr, SIGNAL(stateChanged(QMessageServiceAction::State)), this, SIGNAL(stateChanged(QMessageServiceAction::State)));
	connect(d_ptr, SIGNAL(messagesFound(const QMessageIdList&)), this, SIGNAL(messagesFound(const QMessageIdList&)));
    connect(d_ptr, SIGNAL(messagesCounted(int)), this, SIGNAL(messagesCounted(int)));
	connect(d_ptr, SIGNAL(progressChanged(uint, uint)), this, SIGNAL(progressChanged(uint, uint)));
}

QMessageServiceAction::~QMessageServiceAction()
{
}

bool QMessageServiceAction::queryMessages(const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset)
{
    if (d_ptr->_active) {
        return false;
    }
    
    d_ptr->_active = true;
    d_ptr->_lastError = QMessageManager::NoError;
    
    if (d_ptr->queryMessages(filter, ordering, limit, offset)) {
        d_ptr->_state = QMessageServiceAction::InProgress;
        emit stateChanged(d_ptr->_state);
    } else {
        d_ptr->_state = QMessageServiceAction::Failed;
        emit stateChanged(d_ptr->_state);
    
        d_ptr->_active = false;
    }
    
    return d_ptr->_active;
}

bool QMessageServiceAction::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset)
{
    if (d_ptr->_active) {
        return false;
    }

    d_ptr->_active = true;
    d_ptr->_lastError = QMessageManager::NoError;

    if (d_ptr->queryMessages(filter, body, options, ordering, limit, offset)) {
        d_ptr->_state = QMessageServiceAction::InProgress;
        emit stateChanged(d_ptr->_state);
    } else {
        d_ptr->_state = QMessageServiceAction::Failed;
        emit stateChanged(d_ptr->_state);
    
        d_ptr->_active = false;
    }

    return d_ptr->_active;
}

bool QMessageServiceAction::countMessages(const QMessageFilter &filter)
{
    if (d_ptr->_active) {
        return false;
    }
    
    d_ptr->_active = true;
    d_ptr->_lastError = QMessageManager::NoError;
    
    if (d_ptr->countMessages(filter)) {
        d_ptr->_state = QMessageServiceAction::InProgress;
        emit stateChanged(d_ptr->_state);
    } else {
        d_ptr->_state = QMessageServiceAction::Failed;
        emit stateChanged(d_ptr->_state);
    
        d_ptr->_active = false;
    }
    
    return d_ptr->_active;
}

bool QMessageServiceAction::send(QMessage &message)
{
	if (d_ptr->_active) {
		return false;
	}
	
	d_ptr->_active = true;
	d_ptr->_lastError = QMessageManager::NoError;
	
    bool retVal = true;	
    
    d_ptr->_state = QMessageServiceAction::InProgress;
    emit stateChanged(d_ptr->_state);
    
    // Check message type
    if(message.type() == QMessage::AnyType || message.type() == QMessage::NoType) {
        d_ptr->_lastError = QMessageManager::ConstraintFailure;
        retVal = false;
    }

    QMessageAccountId accountId = message.parentAccountId();
    if (retVal) {
        // Check account
        if (!accountId.isValid()) {
            accountId = QMessageAccount::defaultAccount(message.type());
            if (!accountId.isValid()) {
                d_ptr->_lastError = QMessageManager::InvalidId;
                retVal = false;
            }
        }
    }

    if (retVal) {
        // Check account/message type compatibility
        QMessageAccount account(accountId);
        if (!(account.messageTypes() & message.type())) {
            d_ptr->_lastError = QMessageManager::ConstraintFailure;
            retVal = false;
        }
    }
    
    if (retVal) {
        // Check recipients
        QMessageAddressList recipients = message.to() + message.bcc() + message.cc();
        if (recipients.isEmpty()) {
            d_ptr->_lastError = QMessageManager::ConstraintFailure;
            return false;
        }
    }
    
    QMessage outgoing(message);

    // Set default account if unset
    if (!outgoing.parentAccountId().isValid()) {
        outgoing.setParentAccountId(accountId);
    }
    
    if (retVal) {
        if (message.type() == QMessage::Sms) {
            retVal = d_ptr->sendSMS(outgoing);
        } else if (message.type() == QMessage::Mms) {
            retVal = d_ptr->sendMMS(outgoing);
        } else if (message.type() == QMessage::Email) {
            retVal = d_ptr->sendEmail(outgoing);
        }
    }
    
    d_ptr->_state = retVal ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = false;
    
    return retVal;
}

bool QMessageServiceAction::compose(const QMessage &message)
{
	if (d_ptr->_active) {
		return false;
	}
	
	d_ptr->_active = true;
	d_ptr->_lastError = QMessageManager::NoError;
	
	bool retVal = true;
	d_ptr->_state = QMessageServiceAction::InProgress;
	emit stateChanged(d_ptr->_state);
	
	retVal = d_ptr->compose(message);
	
    d_ptr->_state = retVal ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = false;
    
    return retVal;
}

bool QMessageServiceAction::retrieveHeader(const QMessageId& id)
{
	if (d_ptr->_active) {
		return false;
	}
	
	d_ptr->_active = true;
	d_ptr->_lastError = QMessageManager::NoError;
	
	bool retVal = true;
	d_ptr->_state = QMessageServiceAction::InProgress;
	emit stateChanged(d_ptr->_state);

	retVal = d_ptr->retrieveHeader(id);
	
    d_ptr->_state = retVal ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = false;
    
    return retVal;
}

bool QMessageServiceAction::retrieveBody(const QMessageId& id)
{
	if (d_ptr->_active) {
		return false;
	}
	
	d_ptr->_active = true;
	d_ptr->_lastError = QMessageManager::NoError;
	
	bool retVal = true;
	d_ptr->_state = QMessageServiceAction::InProgress;
	emit stateChanged(d_ptr->_state);

	retVal = d_ptr->retrieveBody(id);
	
    d_ptr->_state = retVal ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = false;
    
    return retVal;
}

bool QMessageServiceAction::retrieve(const QMessageId &messageId, const QMessageContentContainerId& id)
{
	if (d_ptr->_active) {
		return false;
	}
	
	d_ptr->_active = true;
	d_ptr->_lastError = QMessageManager::NoError;

	bool retVal = true;
	d_ptr->_state = QMessageServiceAction::InProgress;
	emit stateChanged(d_ptr->_state);

	retVal = d_ptr->retrieve(messageId, id);
	
    d_ptr->_state = retVal ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = false;
    
    return retVal;
}

bool QMessageServiceAction::show(const QMessageId& id)
{
	if (d_ptr->_active) {
		return false;
	}
	
	d_ptr->_active = true;
	d_ptr->_lastError = QMessageManager::NoError;
	
	bool retVal = true;
	d_ptr->_state = QMessageServiceAction::InProgress;
	emit stateChanged(d_ptr->_state);

    retVal = d_ptr->show(id);
    
    d_ptr->_state = retVal ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = false;
    
    return retVal;
}

bool QMessageServiceAction::exportUpdates(const QMessageAccountId &/*id*/)
{
	//Not supported
	return true;
}

QMessageServiceAction::State QMessageServiceAction::state() const
{
	return d_ptr->_state;
}

void QMessageServiceAction::cancelOperation()
{
}

QMessageManager::ErrorCode QMessageServiceAction::lastError() const
{
    return d_ptr->_lastError;
}

#include "moc_qmessageserviceaction_symbian_p.cpp"
QTM_END_NAMESPACE
