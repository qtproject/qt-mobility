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
#include "modestengine_maemo_p.h"
#include "telepathyengine_maemo_p.h"
#include <QDebug>

QTM_BEGIN_NAMESPACE

class QMessageServicePrivate : public QObject
{
public:
    QMessageServicePrivate(QMessageService* parent);
    ~QMessageServicePrivate();

    void setFinished(bool successful);

public:
    QMessageService* q_ptr;
    QMessageService::State _state;
    bool _active;
    QMessageManager::Error _error;
};

QMessageServicePrivate::QMessageServicePrivate(QMessageService* parent)
 : q_ptr(parent),
   _state(QMessageService::InactiveState),
   _active(false)
{
}

QMessageServicePrivate::~QMessageServicePrivate()
{
}

QMessageService::QMessageService(QObject *parent)
 : QObject(parent),
   d_ptr(new QMessageServicePrivate(this))
{
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

    if (ModestEngine::instance()->queryMessages(*this, filter, sortOrder, limit, offset)) {
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

    if (ModestEngine::instance()->queryMessages(*this, filter, body, matchFlags, sortOrder, limit, offset)) {
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

    if (ModestEngine::instance()->countMessages(*this, filter)) {
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

    // Check message type
    if(message.type() == QMessage::AnyType || message.type() == QMessage::NoType) {
        d_ptr->_error = QMessageManager::ConstraintFailure;
        retVal = false;
    }

    QMessageAccountId accountId = message.parentAccountId();
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
        if (!(account.messageTypes() & message.type())) {
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

    QMessage outgoing(message);

    // Set default account if unset
    if (!outgoing.parentAccountId().isValid()) {
        outgoing.setParentAccountId(accountId);
    }

    if (retVal) {
        if (account.messageTypes() & QMessage::Sms) {
            retVal = TelepathyEngine::instance()->sendMessage(message);
        } else if (account.messageTypes() & QMessage::Xmpp) {
            retVal = TelepathyEngine::instance()->sendMessage(message);
        } else if (account.messageTypes() & QMessage::Mms) {
            d_ptr->_error = QMessageManager::NotYetImplemented;
            qWarning() << "QMessageService::send not yet implemented for MMS";
            retVal = false;
        } else if (account.messageTypes() & QMessage::Email) {
            retVal = ModestEngine::instance()->sendEmail(message);
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

    if (message.type() == QMessage::Sms) {
        d_ptr->_error = QMessageManager::NotYetImplemented; //TODO:
        qWarning() << "QMessageService::compose not yet implemented for SMS";
        retVal = false;
    } else if (message.type() == QMessage::Mms) {
        d_ptr->_error = QMessageManager::NotYetImplemented; //TODO:
        qWarning() << "QMessageService::compose not yet implemented for MMS";
        retVal = false;
    } else if (message.type() == QMessage::Email) {
        retVal = ModestEngine::instance()->composeEmail(message);
    }

    d_ptr->setFinished(retVal);
    return retVal;
}

bool QMessageService::retrieveHeader(const QMessageId& id)
{
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageService::retrieveBody(const QMessageId& id)
{
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageService::retrieve(const QMessageId &messageId, const QMessageContentContainerId& id)
{
    Q_UNUSED(messageId)
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageService::show(const QMessageId& id)
{
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageService::exportUpdates(const QMessageAccountId &id)
{
    Q_UNUSED(id)
    return false; // stub
}

QMessageService::State QMessageService::state() const
{
    return InactiveState; // stub
}

void QMessageService::cancel()
{
}

QMessageManager::Error QMessageService::error() const
{
    return QMessageManager::NoError;
}

QTM_END_NAMESPACE
