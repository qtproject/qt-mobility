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
 _state(QMessageServiceAction::Pending)
{
	Q_UNUSED(parent);
}
      
QMessageServiceActionPrivate::~QMessageServiceActionPrivate()
{
}

bool QMessageServiceActionPrivate::sendSMS(QMessage &message)
{
	return  CMTMEngine::instance()->sendSMS(message);
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

bool QMessageServiceActionPrivate::retrieve(const QMessageContentContainerId& id)
{
	return CMTMEngine::instance()->retrieve(id);
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
	connect(d_ptr, SIGNAL(progressChanged(uint, uint)), this, SIGNAL(progressChanged(uint, uint)));
}

QMessageServiceAction::~QMessageServiceAction()
{
}

bool QMessageServiceAction::queryMessages(const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset)
{
    return d_ptr->queryMessages(filter, ordering, limit, offset);
}

bool QMessageServiceAction::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset)
{
    return d_ptr->queryMessages(filter, body, options, ordering, limit, offset);
}

bool QMessageServiceAction::countMessages(const QMessageFilter &filter)
{
    Q_UNUSED(filter);
    return false;
}

bool QMessageServiceAction::send(QMessage &message)
{
    bool retVal = true;	
    
    d_ptr->_state = QMessageServiceAction::InProgress;
    emit stateChanged(d_ptr->_state);
    
    if (message.type() == QMessage::NoType){
		QMessageAccount account = QMessageAccount(message.parentAccountId());
		QMessage::TypeFlags types = account.messageTypes();
		if (types == QMessage::Email){
			message.setType(QMessage::Email);
		}
		else if (types == QMessage::Mms){
			message.setType(QMessage::Mms);
		}
		if (types == QMessage::Sms){
			message.setType(QMessage::Sms);
		}
    } 
    
    if (message.type() == QMessage::Sms) {
        retVal = d_ptr->sendSMS(message);
    } else if (message.type() == QMessage::Mms) {
        retVal = d_ptr->sendMMS(message);
    } else if (message.type() == QMessage::Email) {
		retVal = d_ptr->sendEmail(message);
    }
    
    d_ptr->_state = retVal ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);
    
    return retVal;
}

bool QMessageServiceAction::compose(const QMessage &message)
{
	bool retVal = true;
	d_ptr->_state = QMessageServiceAction::InProgress;
	emit stateChanged(d_ptr->_state);
	
	retVal = d_ptr->compose(message);
	
    d_ptr->_state = retVal ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);
    
    return retVal;
}

bool QMessageServiceAction::retrieveHeader(const QMessageId& id)
{
	return d_ptr->retrieveHeader(id);
}

bool QMessageServiceAction::retrieveBody(const QMessageId& id)
{
	return d_ptr->retrieveBody(id);
}

bool QMessageServiceAction::retrieve(const QMessageId &messageId, const QMessageContentContainerId& id)
{
    Q_UNUSED(messageId)
	return d_ptr->retrieve(id);
}

bool QMessageServiceAction::show(const QMessageId& id)
{
    return d_ptr->show(id);
}

bool QMessageServiceAction::exportUpdates(const QMessageAccountId &id)
{
    Q_UNUSED(id)
    return false; // stub
}

QMessageServiceAction::State QMessageServiceAction::state() const
{
    return Pending; // stub
}

void QMessageServiceAction::cancelOperation()
{
}

QMessageStore::ErrorCode QMessageServiceAction::lastError() const
{
    return QMessageStore::NoError;
}

#include "moc_qmessageserviceaction_symbian_p.cpp"
QTM_END_NAMESPACE
