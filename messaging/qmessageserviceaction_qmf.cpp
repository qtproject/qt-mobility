/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageserviceaction.h"
#include "qmfhelpers_p.h"

#include <qmailserviceaction.h>

#include <QTimer>

using namespace QmfHelpers;

class QMessageServiceActionPrivate : public QObject
{
    Q_OBJECT

public:
    QMessageServiceActionPrivate();

    QMailTransmitAction _transmit;
    QMailRetrievalAction _retrieval;
    QMailServiceAction *_active;
    QMessageStore::ErrorCode _error;

signals:
    void activityChanged(QMessageServiceAction::Activity);

protected slots:
    void activityChanged(QMailServiceAction::Activity a);
    void completed();
};

QMessageServiceActionPrivate::QMessageServiceActionPrivate()
    : QObject(),
      _active(0),
      _error(QMessageStore::NoError)
{
}

void QMessageServiceActionPrivate::activityChanged(QMailServiceAction::Activity a)
{
    emit activityChanged(convert(a));
}

void QMessageServiceActionPrivate::completed()
{
    emit activityChanged(convert(QMailServiceAction::Successful));
}

QMessageServiceAction::QMessageServiceAction(QObject *parent)
    : QObject(parent),
      d_ptr(new QMessageServiceActionPrivate)
{
    connect(d_ptr, SIGNAL(activityChanged(QMessageServiceAction::Activity)), 
            this, SIGNAL(activityChanged(QMessageServiceAction::Activity)));
}

QMessageServiceAction::~QMessageServiceAction()
{
    delete d_ptr;
}

bool QMessageServiceAction::queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key);
    Q_UNUSED(sortKey);
    Q_UNUSED(limit);
    Q_UNUSED(offset);
    return false; // stub
}

bool QMessageServiceAction::queryMessages(const QString &body, const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(body);
    Q_UNUSED(key);
    Q_UNUSED(sortKey);
    Q_UNUSED(limit);
    Q_UNUSED(offset);
    return false; // stub
}

bool QMessageServiceAction::send(const QMessage &message, const QMessageAccountId &accountId)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    if (!message.id().isValid()) {
        d_ptr->_error = QMessageStore::InvalidId;
        qWarning() << "Invalid message ID";
        return false;
    }
    if (message.parentAccountId() != accountId) {
        d_ptr->_error = QMessageStore::InvalidId;
        qWarning() << "Invalid message account ID";
        return false;
    }
    if (!message.parentFolderId().isValid()) {
        d_ptr->_error = QMessageStore::InvalidId;
        qWarning() << "Invalid message folder ID";
        return false;
    }

    const QMailMessage *m(convert(&message));

    if (!(m->status() & QMailMessage::Outbox)) {
        QMailMessage msg(m->id());
        msg.setStatus(QMailMessage::Outbox, true);
        if (!QMailStore::instance()->updateMessage(&msg)) {
            d_ptr->_error = QMessageStore::FrameworkFault;
            qWarning() << "Unable to mark message as outgoing";
            return false;
        }
    }

    d_ptr->_active = &d_ptr->_transmit;
    d_ptr->_transmit.transmitMessages(convert(accountId));
    return true;
}

bool QMessageServiceAction::compose(const QMessage &message)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    // TODO: To be implemented by integrator
    d_ptr->_error = QMessageStore::NotYetImplemented;
    qWarning() << "QMessageServiceAction::compose not yet implemented";
    return false;

    Q_UNUSED(message)
}

bool QMessageServiceAction::retrieveHeader(const QMessageId& id)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    QMailMessageId messageId(convert(id));
    if (!messageId.isValid()) {
        d_ptr->_error = QMessageStore::InvalidId;
        qWarning() << "Invalid message ID";
        return false;
    }

    // Operation is not relevant to QMF - meta data retrieval always includes header information
    d_ptr->_active = 0;
    QTimer::singleShot(0, d_ptr, SLOT(completed()));
    return true;
}

bool QMessageServiceAction::retrieveBody(const QMessageId& id)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    QMailMessageId messageId(convert(id));
    if (!messageId.isValid()) {
        d_ptr->_error = QMessageStore::InvalidId;
        qWarning() << "Invalid message ID";
        return false;
    }

    d_ptr->_active = &d_ptr->_retrieval;
    d_ptr->_retrieval.retrieveMessages(QMailMessageIdList() << messageId, QMailRetrievalAction::Content);
    return true;
}

bool QMessageServiceAction::retrieve(const QMessageContentContainerId& id)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    QMailMessagePart::Location location(convert(id));
    if (!location.isValid()) {
        d_ptr->_error = QMessageStore::InvalidId;
        qWarning() << "Invalid message part location";
        return false;
    }
    
    d_ptr->_active = &d_ptr->_retrieval;
    d_ptr->_retrieval.retrieveMessagePart(location);
    return true;
}

bool QMessageServiceAction::show(const QMessageId& id)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    // TODO: To be implemented by integrator
    d_ptr->_error = QMessageStore::NotYetImplemented;
    qWarning() << "QMessageServiceAction::show not yet implemented";
    return false;

    Q_UNUSED(id)
}

bool QMessageServiceAction::exportUpdates(const QMessageAccountId &id)
{
    if (d_ptr->_active && ((d_ptr->_active->activity() == QMailServiceAction::Pending) || (d_ptr->_active->activity() == QMailServiceAction::Pending))) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = 0;

    QMailAccountId accountId(convert(id));
    if (!accountId.isValid()) {
        d_ptr->_error = QMessageStore::InvalidId;
        qWarning() << "Account ID is not valid";
        return false;
    }

    d_ptr->_active = &d_ptr->_retrieval;
    d_ptr->_retrieval.exportUpdates(accountId);
    return true;
}

QMessageServiceAction::Activity QMessageServiceAction::activity() const
{
    if (d_ptr->_active) {
        return convert(d_ptr->_active->activity());
    }

    return Successful;
}

void QMessageServiceAction::cancelOperation()
{
    if (d_ptr->_active) {
        d_ptr->_active->cancelOperation();
    }
}

QMessageStore::ErrorCode QMessageServiceAction::lastError() const
{
    if (d_ptr->_active) {
        if (d_ptr->_active->activity() == QMailServiceAction::Failed) {
            // TODO: who knows?
            return QMessageStore::FrameworkFault;
        }
    } else {
        return d_ptr->_error;
    }

    return QMessageStore::NoError;
}

#include "qmessageserviceaction_qmf.moc"

