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
#include "winhelpers_p.h"

using namespace WinHelpers;

class QMessageServiceActionPrivate : public QObject
{
public:
    QMessageServiceActionPrivate(QMessageServiceAction* parent);

    bool init();
    void shutdown();

public:
    QMessageServiceAction* q_ptr;

};

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
:
q_ptr(parent)
{
}

QMessageServiceAction::QMessageServiceAction(QObject *parent)
    : QObject(parent),
    d_ptr(new QMessageServiceActionPrivate(this))
{
    d_ptr->init();
}

QMessageServiceAction::~QMessageServiceAction()
{
    d_ptr->shutdown();
      delete d_ptr; d_ptr = 0;
}

bool QMessageServiceAction::queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(key);
    Q_UNUSED(sortKey);
    Q_UNUSED(limit);
    Q_UNUSED(offset);
    return false; // stub
}

bool QMessageServiceAction::queryMessages(const QString &body, QMessageDataComparator::Options options, const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
{
    Q_UNUSED(body);
    Q_UNUSED(options)
    Q_UNUSED(key);
    Q_UNUSED(sortKey);
    Q_UNUSED(limit);
    Q_UNUSED(offset);
    return false; // stub
}

bool QMessageServiceAction::countMessages(const QMessageFilterKey &key, uint limit) const
{
    Q_UNUSED(key);
    Q_UNUSED(limit);
    return false;
}

bool QMessageServiceAction::countMessages(const QString &body, QMessageDataComparator::Options options, const QMessageFilterKey &key, uint limit) const
{
    Q_UNUSED(body);
    Q_UNUSED(options);
    Q_UNUSED(key);
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

    //login to a MAPI session

    QMessageStore::ErrorCode lastError = QMessageStore::NoError;
    MapiSession mapiSession(&lastError,true);

    if(lastError != QMessageStore::NoError)
    {
        qWarning() << "Unable to open session with default profile";
        return false;
    }

    //open default store

    MapiStorePtr mapiStore = mapiSession.findStore(&lastError);

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

    MapiFolderPtr draftsFolder = inboxFolder->subFolder(MapiFolder::Drafts,&lastError);

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

    mapiSession.showForm(draftMessage,draftsFolder->folder(),mapiStore->store());

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

