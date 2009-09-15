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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageserviceaction.h"
#include "qmessageserviceaction_p.h"
#include <mapix.h>
#include <objbase.h>
#include <mapiutil.h>
#include <QDebug>
#include <QTimer>
#include "winhelpers_p.h"
#include "qmessagestore.h"
#include "qmessageid_p.h"
#include "qmessagefolderid_p.h"
#include "qmessageaccountid_p.h"
#include "qmessage_p.h"
#include "qmessagestore_p.h"

//TODO make all functions non-blocking and asynchronous

using namespace WinHelpers;

void QMessageServiceActionPrivate::completed()
{
    _active = false;
    emit stateChanged(QMessageServiceAction::Successful);
}

void QMessageServiceActionPrivate::reportMatchingIds()
{
    emit messagesFound(_candidateIds);
    completed();
}

QMessageServiceActionPrivate::QMessageServiceActionPrivate(QMessageServiceAction* parent)
    :q_ptr(parent),
     _active(false),
     _state(QMessageServiceAction::Pending)
{
}

bool QMessageServiceActionPrivate::send(const QMessage& message, bool showComposer)
{
    QMessage outgoing(message);

    if(!outgoing.parentAccountId().isValid())
    {
        qWarning() << "No valid account set for message, attempting to use default for message type...";

        QMessageAccountId defaultAccountId = QMessageAccount::defaultAccount(message.type());
        if(!defaultAccountId.isValid())
        {
            _lastError = QMessageStore::ConstraintFailure;
            qWarning() << "No default account available for message type.";
            return false;
        }

        outgoing.setParentAccountId(defaultAccountId);
    }

    MapiSessionPtr mapiSession(MapiSession::createSession(&_lastError,true));

    if (_lastError != QMessageStore::NoError)
    {
        qWarning() << "Could not create seesion";
        return false;
    }

    MapiStorePtr mapiStore = mapiSession->findStore(&_lastError, outgoing.parentAccountId(),false);

    if(mapiStore.isNull() || _lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to get store for the account";
        return false;
    }

    MapiFolderPtr mapiFolder;

    QMessageFolder folder(outgoing.parentFolderId());

    if(folder.id().isValid())
        mapiFolder = mapiStore->findFolder(&_lastError, QMessageFolderIdPrivate::folderRecordKey(folder.id()));
    else
        mapiFolder = mapiStore->findFolder(&_lastError, outgoing.standardFolder());

    //QMessagePrivate::setStandardFolder(outgoing,QMessage::DraftsFolder); //TODO default ATM

    if( mapiFolder.isNull() || _lastError != QMessageStore::NoError ) {
        qWarning() << "Unable to get folder for the message";
        return false;
    }

    IMessage* mapiMessage = mapiFolder->createMessage(outgoing,mapiSession,&_lastError, true);

    if(!mapiMessage || _lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to create MAPI message from source";
        mapiRelease(mapiMessage);
        return false;
    }

    if(showComposer)
    {
        if(FAILED(mapiSession->showForm(mapiMessage,mapiFolder->folder(),mapiStore->store())))
        {
            qWarning() << "MAPI ShowForm failed.";
            _lastError = QMessageStore::FrameworkFault;
            mapiRelease(mapiMessage);
            return false;
        }
    }
    else
    {
        if(FAILED(mapiMessage->SubmitMessage(0)))
        {
            qWarning() << "MAPI SubmitMessage failed.";
            _lastError = QMessageStore::FrameworkFault;
            mapiRelease(mapiMessage);
            return false;
        }
    }

    if(!mapiSession->flushQueues())
        qWarning() << "MAPI flush queues failed.";

    mapiRelease(mapiMessage);

    return true;
}

bool QMessageServiceActionPrivate::show(const QMessageId& messageId)
{
    if(!messageId.isValid())
    {
        _lastError = QMessageStore::InvalidId;
        qWarning() << "Invalid message id";
        return false;
    }

    MapiSessionPtr mapiSession(MapiSession::createSession(&_lastError,true));
    if (_lastError != QMessageStore::NoError)
    {
        qWarning() << "Could not create seesion";
        return false;
    }

    //messageId -> IMapiStore
    //messageId -> IMapiFolder
    //messageId -> IMessage

    MapiEntryId entryId = QMessageIdPrivate::entryId(messageId);
    MapiRecordKey folderRecordKey = QMessageIdPrivate::folderRecordKey(messageId);
    MapiRecordKey storeRecordKey = QMessageIdPrivate::storeRecordKey(messageId);

    MapiStorePtr mapiStore = mapiSession->findStore(&_lastError,QMessageAccountIdPrivate::from(storeRecordKey));

    if(mapiStore.isNull() || _lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to get store for the message";
        return false;
    }

    MapiFolderPtr mapiFolder = mapiStore->openFolder(&_lastError,folderRecordKey);

    if( mapiFolder.isNull() || _lastError != QMessageStore::NoError ) {
        qWarning() << "Unable to get folder for the message";
        return false;
    }

    IMessage* mapiMessage = mapiFolder->openMessage(&_lastError,entryId);

    if(!mapiMessage || _lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to create MAPI message from source";
        mapiRelease(mapiMessage);
        return false;
    }

    if(FAILED(mapiSession->showForm(mapiMessage,mapiFolder->folder(),mapiStore->store())))
    {
        qWarning() << "MAPI ShowForm failed.";
        _lastError = QMessageStore::FrameworkFault;
        mapiRelease(mapiMessage);
        return false;
    }

    mapiRelease(mapiMessage);

    return true;
}

QMessageServiceAction::QMessageServiceAction(QObject *parent)
    : QObject(parent),
    d_ptr(new QMessageServiceActionPrivate(this))
{
    connect(d_ptr, SIGNAL(stateChanged(QMessageServiceAction::State)),
        this, SIGNAL(stateChanged(QMessageServiceAction::State)));
    connect(d_ptr, SIGNAL(messagesFound(const QMessageIdList&)),
        this, SIGNAL(messagesFound(const QMessageIdList&)));
    connect(d_ptr, SIGNAL(progressChanged(uint, uint)),
        this, SIGNAL(progressChanged(uint, uint)));
}

QMessageServiceAction::~QMessageServiceAction()
{
    delete d_ptr;
    d_ptr = 0;
}

bool QMessageServiceAction::queryMessages(const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    if (d_ptr->_active) {
        qWarning() << "Action is currently busy";
        return false;
    }
    d_ptr->_active = true;
    d_ptr->_candidateIds = QMessageStore::instance()->queryMessages(filter, ordering, limit, offset);
    d_ptr->_lastError = QMessageStore::instance()->lastError();

    if (d_ptr->_lastError == QMessageStore::NoError) {
        QTimer::singleShot(0, d_ptr, SLOT(reportMatchingIds()));
        return true;
    }
    return false;
}

bool QMessageServiceAction::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    Q_UNUSED(filter);
    Q_UNUSED(body);
    Q_UNUSED(options)
    Q_UNUSED(ordering);
    Q_UNUSED(limit);
    Q_UNUSED(offset);
    return false; // stub
}

bool QMessageServiceAction::countMessages(const QMessageFilter &filter, uint limit) const
{
    Q_UNUSED(filter);
    Q_UNUSED(limit);
    return false;
}

bool QMessageServiceAction::countMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::Options options, uint limit) const
{
    Q_UNUSED(filter);
    Q_UNUSED(body);
    Q_UNUSED(options);
    Q_UNUSED(limit);
    return false;
}

bool QMessageServiceAction::send(QMessage &message)
{
    if(d_ptr->_active) {
        qWarning() << "Action is currently busy";
        return false;
    }

    d_ptr->_state = QMessageServiceAction::InProgress;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = true;

    d_ptr->_lastError = QMessageStore::NoError;

    bool result = d_ptr->send(message);

    d_ptr->_state = result ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);

    d_ptr->_active = false;

    return result;
}

bool QMessageServiceAction::compose(const QMessage &message)
{
    if(d_ptr->_active) {
        qWarning() << "Action is currently busy";
        return false;
    }

    d_ptr->_state = QMessageServiceAction::InProgress;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = true;

    d_ptr->_lastError = QMessageStore::NoError;

    bool result = d_ptr->send(message,true);

    d_ptr->_state = result ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = false;

    return result;
}

bool QMessageServiceAction::retrieveHeader(const QMessageId& id)
{
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageServiceAction::retrieveBody(const QMessageId& id)
{
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageServiceAction::retrieve(const QMessageContentContainerId& id)
{
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageServiceAction::show(const QMessageId& id)
{
    if(d_ptr->_active) {
        qWarning() << "Action is currently busy";
        return false;
    }

    d_ptr->_state = InProgress;
    emit stateChanged(d_ptr->_state);
    d_ptr->_lastError = QMessageStore::NoError;
    d_ptr->_active = true;

    bool result = d_ptr->show(id);

    d_ptr->_state = result ? QMessageServiceAction::Successful : QMessageServiceAction::Failed;
    emit stateChanged(d_ptr->_state);
    d_ptr->_active = false;

    return result;
}

bool QMessageServiceAction::exportUpdates(const QMessageAccountId &id)
{
    Q_UNUSED(id)
    return false; // stub
}

QMessageServiceAction::State QMessageServiceAction::state() const
{
    return d_ptr->_state;
}

void QMessageServiceAction::cancelOperation()
{
}

QMessageStore::ErrorCode QMessageServiceAction::lastError() const
{
    return d_ptr->_lastError;
}

