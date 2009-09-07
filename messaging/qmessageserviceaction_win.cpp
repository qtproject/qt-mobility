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
#include <mapix.h>
#include <objbase.h>
#include <mapiutil.h>
#include <QDebug>
#include <QTimer>
#include "winhelpers_p.h"

using namespace WinHelpers;

class QMessageServiceActionPrivate : public QObject
{
    Q_OBJECT

public:
    QMessageServiceActionPrivate(QMessageServiceAction* parent);

    bool init();
    void shutdown();

public:
    QMessageServiceAction* q_ptr;
    bool _active;
    QMessageStore::ErrorCode _error;
    QMessageIdList _candidateIds;

public slots:
    void completed();
    void reportMatchingIds();

signals:
    void activityChanged(QMessageServiceAction::Activity);
    void messagesFound(const QMessageIdList&);
    void progressChanged(uint, uint);
};

void QMessageServiceActionPrivate::completed()
{
    _active = false;
    emit activityChanged(QMessageServiceAction::Successful);
}

void QMessageServiceActionPrivate::reportMatchingIds()
{
    emit messagesFound(_candidateIds);
    completed();
}



bool QMessageServiceActionPrivate::init()
{
#ifndef QT_NO_THREAD
    // Note MAPIINIT is ignored on Windows Mobile but used on Outlook 2007 see
    // msdn ms862621 vs cc842343
    MAPIINIT_0 MAPIINIT = { 0, MAPI_MULTITHREAD_NOTIFICATIONS };
    if (MAPIInitialize(&MAPIINIT) == S_OK)
    {
        return true;
    }
#else
    if (MAPIInitialize(0) == S_OK)
    {
        return true;
    }
#endif
    return false;
}

void QMessageServiceActionPrivate::shutdown()
{
    MAPIUninitialize();
}

QMessageServiceActionPrivate::QMessageServiceActionPrivate(QMessageServiceAction* parent)
    :q_ptr(parent),
     _active(false)
{
}

QMessageServiceAction::QMessageServiceAction(QObject *parent)
    : QObject(parent),
    d_ptr(new QMessageServiceActionPrivate(this))
{
    d_ptr->init();
    connect(d_ptr, SIGNAL(activityChanged(QMessageServiceAction::Activity)),
        this, SIGNAL(activityChanged(QMessageServiceAction::Activity)));
    connect(d_ptr, SIGNAL(messagesFound(const QMessageIdList&)),
        this, SIGNAL(messagesFound(const QMessageIdList&)));
    connect(d_ptr, SIGNAL(progressChanged(uint, uint)),
        this, SIGNAL(progressChanged(uint, uint)));
}

QMessageServiceAction::~QMessageServiceAction()
{
    d_ptr->shutdown();
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
    d_ptr->_error = QMessageStore::instance()->lastError();

    if (d_ptr->_error == QMessageStore::NoError) {
        QTimer::singleShot(0, d_ptr, SLOT(reportMatchingIds()));
        return true;
    }
    return false;
}

bool QMessageServiceAction::queryMessages(const QString &body, QMessageDataComparator::Options options, const QMessageFilter &filter, const QMessageOrdering &ordering, uint limit, uint offset) const
{
    Q_UNUSED(body);
    Q_UNUSED(options)
    Q_UNUSED(filter);
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

bool QMessageServiceAction::countMessages(const QString &body, QMessageDataComparator::Options options, const QMessageFilter &filter, uint limit) const
{
    Q_UNUSED(body);
    Q_UNUSED(options);
    Q_UNUSED(filter);
    Q_UNUSED(limit);
    return false;
}

bool QMessageServiceAction::send(QMessage &message)
{
    Q_UNUSED(message)
    return false; // stub
}

bool QMessageServiceAction::compose(const QMessage &message)
{
    Q_UNUSED(message)

    //login to a MAPI session

    QMessageStore::ErrorCode lastError = QMessageStore::NoError;
    MapiSessionPtr mapiSession(MapiSession::createSession(&lastError, false));
    if(lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to open session with default profile";
        return false;
    }

    //open default store

    MapiStorePtr mapiStore = mapiSession->defaultStore(&lastError);

    if(lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to access default store";
        return false;
    }

    //get the default inbox folder

    MapiFolderPtr inboxFolder = mapiStore->receiveFolder(&lastError);

    if(lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to load default receive folder";
        return false;
    }

    //get the drafts folder

    MapiFolderPtr draftsFolder = inboxFolder->subFolder(&lastError, MapiFolder::Drafts, *mapiStore);

    if(lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to load a drafts folder";
        return false;
    }

    //create blank message in drafts

    LPMESSAGE draftMessage = draftsFolder->createMessage(&lastError);

    if(lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to create blank message in outbox";
        return false;
    }

    //show draft message

    mapiSession->showForm(draftMessage,draftsFolder->folder(),mapiStore->store());

   if(draftMessage) draftMessage->Release(); draftMessage = 0;

   return true;
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
    Q_UNUSED(id)
    return false; // stub
}

bool QMessageServiceAction::exportUpdates(const QMessageAccountId &id)
{
    Q_UNUSED(id)
    return false; // stub
}

QMessageServiceAction::Activity QMessageServiceAction::activity() const
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

#include "qmessageserviceaction_win.moc"
