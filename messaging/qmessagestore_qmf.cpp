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
#include "qmessagestore.h"
#include "qmfhelpers_p.h"

#include <qmailstore.h>


using namespace QmfHelpers;

class QMessageStorePrivate
{
public:
    QMessageStorePrivate() : _store(QMailStore::instance()), _error(QMessageStore::NoError) {}

    QMailStore *_store;
    QMessageStore::ErrorCode _error;

    Q_SCOPED_STATIC_DECLARE(QMessageStore,storeInstance);
};

Q_SCOPED_STATIC_DEFINE(QMessageStore,QMessageStorePrivate,storeInstance);

QMessageStore::QMessageStore(QObject *parent)
    : QObject(parent),
      d_ptr(new QMessageStorePrivate)
{
    Q_ASSERT(instance() != 0);
}

QMessageStore::~QMessageStore()
{
    delete d_ptr;
}

QMessageStore::ErrorCode QMessageStore::lastError() const
{
    if (d_ptr->_error != QMessageStore::NoError) {
        return d_ptr->_error;
    }

    return convert(d_ptr->_store->lastError());
}

QMessageIdList QMessageStore::queryMessages(const QMessageFilterKey &key, const QMessageSortKey &sortKey, uint limit, uint offset) const
{
    if (key.options() != 0) {
        d_ptr->_error = QMessageStore::NotYetImplemented;
        return QMessageIdList();
    }
    
    d_ptr->_error = QMessageStore::NoError;
    return convert(d_ptr->_store->queryMessages(convert(key), convert(sortKey)));

    // TODO: Extend QMF to support limit/offset
    Q_UNUSED(limit)
    Q_UNUSED(offset)
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderIdList QMessageStore::queryFolders(const QMessageFolderFilterKey &key, const QMessageFolderSortKey &sortKey, uint limit, uint offset) const
{
    if (key.options() != 0) {
        d_ptr->_error = QMessageStore::NotYetImplemented;
        return QMessageFolderIdList();
    }
    
    d_ptr->_error = QMessageStore::NoError;
    return convert(d_ptr->_store->queryFolders(convert(key), convert(sortKey)));

    // TODO: Extend QMF to support limit/offset
    Q_UNUSED(limit)
    Q_UNUSED(offset)
}
#endif

QMessageAccountIdList QMessageStore::queryAccounts(const QMessageAccountFilterKey &key, const QMessageAccountSortKey &sortKey, uint limit, uint offset) const
{
    if (key.options() != 0) {
        d_ptr->_error = QMessageStore::NotYetImplemented;
        return QMessageAccountIdList();
    }
    
    d_ptr->_error = QMessageStore::NoError;
    return convert(d_ptr->_store->queryAccounts(convert(key), convert(sortKey)));

    // TODO: Extend QMF to support limit/offset
    Q_UNUSED(limit)
    Q_UNUSED(offset)
}

int QMessageStore::countMessages(const QMessageFilterKey& key) const
{
    if (key.options() != 0) {
        d_ptr->_error = QMessageStore::NotYetImplemented;
        return 0;
    }
    
    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->countMessages(convert(key));
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
int QMessageStore::countFolders(const QMessageFolderFilterKey& key) const
{
    if (key.options() != 0) {
        d_ptr->_error = QMessageStore::NotYetImplemented;
        return 0;
    }
    
    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->countFolders(convert(key));
}
#endif

int QMessageStore::countAccounts(const QMessageAccountFilterKey& key) const
{
    if (key.options() != 0) {
        d_ptr->_error = QMessageStore::NotYetImplemented;
        return 0;
    }
    
    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->countAccounts(convert(key));
}

bool QMessageStore::removeMessage(const QMessageId& id, QMessageStore::RemovalOption option)
{
    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->removeMessage(convert(id), convert(option));
}

bool QMessageStore::removeMessages(const QMessageFilterKey& key, QMessageStore::RemovalOption option)
{
    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->removeMessages(convert(key), convert(option));
}

bool QMessageStore::addMessage(QMessage *m)
{
    QMailMessage msg(convert(*m));

    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->addMessage(&msg);
}

bool QMessageStore::updateMessage(QMessage *m)
{
    QMailMessage msg(convert(*m));

    d_ptr->_error = QMessageStore::NoError;
    return d_ptr->_store->updateMessage(&msg);
}

QMessage QMessageStore::message(const QMessageId& id) const
{
    d_ptr->_error = QMessageStore::NoError;
    return convert(d_ptr->_store->message(convert(id)));
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolder QMessageStore::folder(const QMessageFolderId& id) const
{
    d_ptr->_error = QMessageStore::NoError;
    return convert(d_ptr->_store->folder(convert(id)));
}
#endif

QMessageAccount QMessageStore::account(const QMessageAccountId& id) const
{
    d_ptr->_error = QMessageStore::NoError;
    return convert(d_ptr->_store->account(convert(id)));
}

void QMessageStore::setMaximumWorkingMemory(uint maximumBytes)
{
    // TODO: implement memory constraints
    Q_UNUSED(maximumBytes)
}

uint QMessageStore::maximumWorkingMemory()
{
    // TODO: implement memory constraints
    return 0; // stub
}

QMessageStore* QMessageStore::instance()
{
    return QMessageStorePrivate::storeInstance();
}
    
void QMessageStore::startNotifications(const QMessageFilterKey &key)
{
    if (key.options() != 0) {
        d_ptr->_error = QMessageStore::NotYetImplemented;
        return;
    }
    
    // TODO: implement notifications
    Q_UNUSED(key)    
}

void QMessageStore::stopNotifications()
{
    // TODO: implement notifications
}

