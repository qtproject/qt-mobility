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
#include "maemohelpers_p.h"
#include "qmessagefilter_p.h"
#include "qmessagesortorder_p.h"
#include "qmessageaccountfilter_p.h"
#include "qmessageaccountsortorder_p.h"
#include "qmessagefolderfilter_p.h"
#include "qmessagefoldersortorder_p.h"
#include "qmessagestore.h"

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(MessagingHelper, messagingHelper);

MessagingHelper* MessagingHelper::instance()
{
    return messagingHelper();
}

MessagingHelper::MessagingHelper()
{
}

MessagingHelper::~MessagingHelper()
{
}


void MessagingHelper::filterAccounts(QMessageAccountIdList& accountIds, const QMessageAccountFilter& filter)
{
    QMessageAccountFilterPrivate* pf = QMessageAccountFilterPrivate::implementation(filter);
    if ((pf->_field == QMessageAccountFilterPrivate::None) &&
        (pf->_filterList.count() == 0)) {
        if (pf->_notFilter) {
            // There is only one filter: empty ~QMessageFilter()
            // => accountIds must be cleared
            accountIds.clear();
            return;
        } else {
            // There is only one filter: empty QMessageFilter()
            // => accountIds list can remain intact
            return;
        }
    }

    if (pf->_valid) {
        QMessageStore* store = QMessageStore::instance();
        for (int i=accountIds.count()-1; i >= 0; i--) {
            QMessageAccount account = store->account(accountIds[i]);
            if (!pf->filter(account)) {
                accountIds.removeAt(i);
            }
        }
    }
}

bool MessagingHelper::accountLessThan(const QMessageAccountId accountId1, const QMessageAccountId accountId2)
{
    QMessageStore* store = QMessageStore::instance();
    return QMessageAccountSortOrderPrivate::lessThan(*messagingHelper()->m_AccountSortOrder,
                                                     store->account(accountId1),
                                                     store->account(accountId2));
}

void MessagingHelper::orderAccounts(QMessageAccountIdList& accountIds, const QMessageAccountSortOrder &sortOrder)
{
    messagingHelper()->m_AccountSortOrder = (QMessageAccountSortOrder*)&sortOrder;
    qSort(accountIds.begin(), accountIds.end(), MessagingHelper::accountLessThan);
}

void MessagingHelper::applyOffsetAndLimitToAccountIdList(QMessageAccountIdList& accountIds, int limit, int offset)
{
    if (offset > 0) {
        if (offset > accountIds.count()) {
            accountIds.clear();
        } else {
            for (int i = 0; i < offset; i++) {
                accountIds.removeFirst();
            }
        }
    }
    if (limit > 0) {
        for (int i = accountIds.count()-1; i >= limit; i--) {
            accountIds.removeAt(i);
        }
    }
}

void MessagingHelper::filterFolders(QMessageFolderIdList& folderIds, const QMessageFolderFilter& filter)
{
    QMessageFolderFilterPrivate* pf = QMessageFolderFilterPrivate::implementation(filter);
    if ((pf->_field == QMessageFolderFilterPrivate::None) &&
        (pf->_filterList.count() == 0)) {
        if (pf->_notFilter) {
            // There is only one filter: empty ~QMessageFilter()
            // => accountIds must be cleared
            folderIds.clear();
            return;
        } else {
            // There is only one filter: empty QMessageFilter()
            // => accountIds list can remain intact
            return;
        }
    }

    if (pf->_valid) {
        QMessageStore* store = QMessageStore::instance();
        for (int i=folderIds.count()-1; i >= 0; i--) {
            QMessageFolder folder = store->folder(folderIds[i]);
            if (!pf->filter(folder)) {
                folderIds.removeAt(i);
            }
        }
    }
}

bool MessagingHelper::folderLessThan(const QMessageFolderId folderId1, const QMessageFolderId folderId2)
{
    QMessageStore* store = QMessageStore::instance();
    return QMessageFolderSortOrderPrivate::lessThan(*messagingHelper()->m_FolderSortOrder,
                                                    store->folder(folderId1),
                                                    store->folder(folderId2));
}

void MessagingHelper::orderFolders(QMessageFolderIdList& folderIds,  const QMessageFolderSortOrder &sortOrder)
{
    messagingHelper()->m_FolderSortOrder = (QMessageFolderSortOrder*)&sortOrder;
    qSort(folderIds.begin(), folderIds.end(), MessagingHelper::folderLessThan);
}

void MessagingHelper::applyOffsetAndLimitToFolderIdList(QMessageFolderIdList& folderIds, int limit, int offset)
{
    if (offset > 0) {
        if (offset > folderIds.count()) {
            folderIds.clear();
        } else {
            for (int i = 0; i < offset; i++) {
                folderIds.removeFirst();
            }
        }
    }
    if (limit > 0) {
        for (int i = folderIds.count()-1; i >= limit; i--) {
            folderIds.removeAt(i);
        }
    }
}

void MessagingHelper::filterMessages(QMessageIdList& messageIds, const QMessageFilter& filter)
{
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(filter);
    if ((pf->_field == QMessageFilterPrivate::None) &&
        (pf->_filterList.count() == 0)) {
        if (pf->_notFilter) {
            // There is only one filter: empty ~QMessageFilter()
            // => accountIds must be cleared
            messageIds.clear();
            return;
        } else {
            // There is only one filter: empty QMessageFilter()
            // => accountIds list can remain intact
            return;
        }
    }

    if (pf->_valid) {
        QMessageStore* store = QMessageStore::instance();
        for (int i=messageIds.count()-1; i >= 0; i--) {
            QMessage message = store->message(messageIds[i]);
            if (!pf->filter(message)) {
                messageIds.removeAt(i);
            }
        }
    }
}

bool MessagingHelper::messageLessThan(const QMessageId messageId1, const QMessageId messageId2)
{
    QMessageStore* store = QMessageStore::instance();
    return QMessageSortOrderPrivate::lessThan(*messagingHelper()->m_MessageSortOrder,
                                              store->message(messageId1),
                                              store->message(messageId2));
}

void MessagingHelper::orderMessages(QMessageIdList& messageIds,  const QMessageSortOrder &sortOrder)
{
    messagingHelper()->m_MessageSortOrder = (QMessageSortOrder*)&sortOrder;
    qSort(messageIds.begin(), messageIds.end(), MessagingHelper::messageLessThan);
}

void MessagingHelper::applyOffsetAndLimitToMessageIdList(QMessageIdList& messageIds, int limit, int offset)
{
    if (offset > 0) {
        if (offset > messageIds.count()) {
            messageIds.clear();
        } else {
            for (int i = 0; i < offset; i++) {
                messageIds.removeFirst();
            }
        }
    }
    if (limit > 0) {
        for (int i = messageIds.count()-1; i >= limit; i--) {
            messageIds.removeAt(i);
        }
    }
}

QTM_END_NAMESPACE
