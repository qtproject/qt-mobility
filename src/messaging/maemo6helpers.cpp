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
#include "maemo6helpers_p.h"
#include "qmessagefilter_p.h"
#include "qmessagesortorder_p.h"
#include "qmessageaccountfilter_p.h"
#include "qmessageaccountsortorder_p.h"
#include "qmessagefolderfilter_p.h"
#include "qmessagefoldersortorder_p.h"
#include "qmessagestore.h"

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(MessagingHelper, messagingHelper);

#define DELIMETER_EMAIL   "; "

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
    if (accountIds.count() && !filter.isEmpty()) {
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
    if (!sortOrder.isEmpty()) {
        messagingHelper()->m_AccountSortOrder = (QMessageAccountSortOrder*)&sortOrder;
        qSort(accountIds.begin(), accountIds.end(), MessagingHelper::accountLessThan);
    }
}

void MessagingHelper::applyOffsetAndLimitToAccountIdList(QMessageAccountIdList& accountIds, int limit, int offset)
{
    if (accountIds.count() == 0) {
        return;
    }

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
    if (folderIds.count() && !filter.isEmpty()) {
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
    if (!sortOrder.isEmpty()) {
        messagingHelper()->m_FolderSortOrder = (QMessageFolderSortOrder*)&sortOrder;
        qSort(folderIds.begin(), folderIds.end(), MessagingHelper::folderLessThan);
    }
}

void MessagingHelper::applyOffsetAndLimitToFolderIdList(QMessageFolderIdList& folderIds, int limit, int offset)
{
    if (folderIds.count() == 0) {
        return;
    }

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
    if (messageIds.count() && !filter.isEmpty()) {
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
    if (!sortOrder.isEmpty()) {
        messagingHelper()->m_MessageSortOrder = (QMessageSortOrder*)&sortOrder;
        qSort(messageIds.begin(), messageIds.end(), MessagingHelper::messageLessThan);
    }
}

void MessagingHelper::applyOffsetAndLimitToMessageIdList(QMessageIdList& messageIds, int limit, int offset)
{
    if (messageIds.count() == 0) {
        return;
    }

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

void MessagingHelper::handleNestedFiltersFromFolderFilter(QMessageFolderFilter &filter)
{
    QMessageStore* store = QMessageStore::instance();

    QMessageFolderFilterPrivate* pMFFilter = QMessageFolderFilterPrivate::implementation(filter);
    if (pMFFilter->_filterList.count() > 0) {
        int filterListCount = pMFFilter->_filterList.count();
        for (int i=0; i < filterListCount; i++) {
            for (int j=0; j < pMFFilter->_filterList[i].count(); j++) {
                QMessageFolderFilterPrivate* pMFFilter2 = QMessageFolderFilterPrivate::implementation(pMFFilter->_filterList[i][j]);
                if (pMFFilter2->_field == QMessageFolderFilterPrivate::ParentAccountIdFilter) {
                    QMessageAccountIdList accountIds = store->queryAccounts(*pMFFilter2->_accountFilter, QMessageAccountSortOrder(), 0, 0);
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter2->_comparatorValue));
                    if (accountIds.count() > 0) {
                        pMFFilter->_filterList[i].removeAt(j);
                        if (cmp == QMessageDataComparator::Includes) {
                            for (int x = 0; x < accountIds.count(); x++) {
                                if (x == 0) {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[i]);
                                    }
                                    pMFFilter->_filterList[i].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                    qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
                                } else {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[pMFFilter->_filterList.count()-1]);
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-2].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-2].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-2].end(), QMessageFolderFilterPrivate::lessThan);
                                    } else {
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-1].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-1].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-1].end(), QMessageFolderFilterPrivate::lessThan);
                                    }
                                }
                            }
                        } else { // Excludes
                            for (int x = 0; x < accountIds.count(); x++) {
                                pMFFilter->_filterList[i].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::NotEqual));
                            }
                            qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
                        }
                    } else {
                        delete pMFFilter2->_accountFilter;
                        pMFFilter2->_accountFilter = 0;
                        pMFFilter2->_field = QMessageFolderFilterPrivate::Id;
                        qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
                    }
                } else {
                    break;
                }
            }
        }
    } else {
        if (pMFFilter->_field == QMessageFolderFilterPrivate::ParentAccountIdFilter) {
            QMessageAccountIdList accountIds = store->queryAccounts(*pMFFilter->_accountFilter, QMessageAccountSortOrder(), 0, 0);
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter->_comparatorValue));
            if (accountIds.count() > 0) {
                for (int i=0; i < accountIds.count(); i++) {
                    if (i == 0) {
                        delete pMFFilter->_accountFilter;
                        pMFFilter->_accountFilter = 0;
                        pMFFilter->_field = QMessageFolderFilterPrivate::ParentAccountId;
                        pMFFilter->_value = accountIds[0].toString();
                        pMFFilter->_comparatorType = QMessageFolderFilterPrivate::Equality;
                        if (cmp == QMessageDataComparator::Includes) {
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
                        } else { // Excludes
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
                        }
                    } else {
                        if (cmp == QMessageDataComparator::Includes) {
                            filter |= QMessageFolderFilter::byParentAccountId(accountIds[i],QMessageDataComparator::Equal);
                        } else { // Excludes
                            filter &= QMessageFolderFilter::byParentAccountId(accountIds[i],QMessageDataComparator::NotEqual);
                        }
                    }
                }
            } else {
                delete pMFFilter->_accountFilter;
                pMFFilter->_accountFilter = 0;
                pMFFilter->_field = QMessageFolderFilterPrivate::Id;
            }
        }
    }
}

void MessagingHelper::handleNestedFiltersFromMessageFilter(QMessageFilter &filter)
{
    QMessageStore* store = QMessageStore::instance();

    QMessageFilterPrivate* pMFFilter = QMessageFilterPrivate::implementation(filter);
    if (pMFFilter->_filterList.count() > 0) {
        int filterListCount = pMFFilter->_filterList.count();
        for (int i=0; i < filterListCount; i++) {
            for (int j=0; j < pMFFilter->_filterList[i].count(); j++) {
                QMessageFilterPrivate* pMFFilter2 = QMessageFilterPrivate::implementation(pMFFilter->_filterList[i][j]);
                if (pMFFilter2->_field == QMessageFilterPrivate::ParentAccountIdFilter) {
                    QMessageAccountIdList accountIds = store->queryAccounts(*pMFFilter2->_accountFilter, QMessageAccountSortOrder(), 0, 0);
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter2->_comparatorValue));
                    if (accountIds.count() > 0) {
                        pMFFilter->_filterList[i].removeAt(j);
                        if (cmp == QMessageDataComparator::Includes) {
                            for (int x = 0; x < accountIds.count(); x++) {
                                if (x == 0) {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[i]);
                                    }
                                    pMFFilter->_filterList[i].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                    qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                                } else {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[pMFFilter->_filterList.count()-1]);
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-2].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-2].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-2].end(), QMessageFilterPrivate::lessThan);
                                    } else {
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-1].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-1].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-1].end(), QMessageFilterPrivate::lessThan);
                                    }
                                }
                            }
                        } else { // Excludes
                            for (int x = 0; x < accountIds.count(); x++) {
                                pMFFilter->_filterList[i].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::NotEqual));
                            }
                            qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                        }
                    } else {
                        delete pMFFilter2->_accountFilter;
                        pMFFilter2->_accountFilter = 0;
                        pMFFilter2->_field = QMessageFilterPrivate::Id;
                        qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                    }
                } else if (pMFFilter2->_field == QMessageFilterPrivate::ParentFolderIdFilter) {
                    QMessageFolderIdList folderIds = store->queryFolders(*pMFFilter2->_folderFilter, QMessageFolderSortOrder(), 0, 0);
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter2->_comparatorValue));
                    if (folderIds.count() > 0) {
                        pMFFilter->_filterList[i].removeAt(j);
                        if (cmp == QMessageDataComparator::Includes) {
                            for (int x = 0; x < folderIds.count(); x++) {
                                if (x == 0) {
                                    if (x+1 < folderIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[i]);
                                    }
                                    pMFFilter->_filterList[i].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::Equal));
                                    qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                                } else {
                                    if (x+1 < folderIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[pMFFilter->_filterList.count()-1]);
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-2].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-2].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-2].end(), QMessageFilterPrivate::lessThan);
                                    } else {
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-1].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-1].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-1].end(), QMessageFilterPrivate::lessThan);
                                    }
                                }
                            }
                        } else { // Excludes
                            for (int x = 0; x < folderIds.count(); x++) {
                                pMFFilter->_filterList[i].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::NotEqual));
                            }
                            qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                        }
                    } else {
                        delete pMFFilter2->_folderFilter;
                        pMFFilter2->_folderFilter = 0;
                        pMFFilter2->_field = QMessageFilterPrivate::Id;
                        qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                    }
                } else {
                    break;
                }
            }
        }
    } else {
        if (pMFFilter->_field == QMessageFilterPrivate::ParentAccountIdFilter) {
            QMessageAccountIdList accountIds = store->queryAccounts(*pMFFilter->_accountFilter, QMessageAccountSortOrder(), 0, 0);
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter->_comparatorValue));
            if (accountIds.count() > 0) {
                for (int i=0; i < accountIds.count(); i++) {
                    if (i == 0) {
                        delete pMFFilter->_accountFilter;
                        pMFFilter->_accountFilter = 0;
                        pMFFilter->_field = QMessageFilterPrivate::ParentAccountId;
                        pMFFilter->_value = accountIds[0].toString();
                        pMFFilter->_comparatorType = QMessageFilterPrivate::Equality;
                        if (cmp == QMessageDataComparator::Includes) {
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
                        } else { // Excludes
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
                        }
                    } else {
                        if (cmp == QMessageDataComparator::Includes) {
                            filter |= QMessageFilter::byParentAccountId(accountIds[i],QMessageDataComparator::Equal);
                        } else { // Excludes
                            filter &= QMessageFilter::byParentAccountId(accountIds[i],QMessageDataComparator::NotEqual);
                        }
                    }
                }
            } else {
                delete pMFFilter->_accountFilter;
                pMFFilter->_accountFilter = 0;
                pMFFilter->_field = QMessageFilterPrivate::Id;
            }
        } else if (pMFFilter->_field == QMessageFilterPrivate::ParentFolderIdFilter) {
            QMessageFolderIdList folderIds = store->queryFolders(*pMFFilter->_folderFilter, QMessageFolderSortOrder(), 0, 0);
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter->_comparatorValue));
            if (folderIds.count() > 0) {
                for (int i=0; i < folderIds.count(); i++) {
                    if (i == 0) {
                        delete pMFFilter->_folderFilter;
                        pMFFilter->_folderFilter = 0;
                        pMFFilter->_field = QMessageFilterPrivate::ParentFolderId;
                        pMFFilter->_value = folderIds[0].toString();
                        pMFFilter->_comparatorType = QMessageFilterPrivate::Equality;
                        if (cmp == QMessageDataComparator::Includes) {
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
                        } else { // Excludes
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
                        }
                    } else {
                        if (cmp == QMessageDataComparator::Includes) {
                            filter |= QMessageFilter::byParentFolderId(folderIds[i],QMessageDataComparator::Equal);
                        } else { // Excludes
                            filter &= QMessageFilter::byParentFolderId(folderIds[i],QMessageDataComparator::NotEqual);
                        }
                    }
                }
            } else {
                delete pMFFilter->_folderFilter;
                pMFFilter->_folderFilter = 0;
                pMFFilter->_field = QMessageFilterPrivate::Id;
            }
        }
    }
}

QString MessagingHelper::addressListToString(const QMessageAddressList &alist)
{
    QString ret;

    for (int i = 0; i < alist.size(); i++) {
        if (i != 0)
            ret += DELIMETER_EMAIL;

            ret += alist.at(i).addressee();
        }

    return ret;
}

QMessageAddressList MessagingHelper::stringToAddressList(const QString &astring)
{
    QMessageAddressList ret;
    int i = 0;

    while(1) {
        QString str = astring.section(DELIMETER_EMAIL, i, i);
        i++;
        if (str.length()) {
            QMessageAddress address(QMessageAddress::Phone, str);
            ret.append(address);
        } else
            break;
    }

    return ret;
}

QStringList MessagingHelper::stringListFromAddressList(const QMessageAddressList &alist)
{
    QStringList ret;

    foreach(QMessageAddress address, alist)
    {
        ret.append(address.addressee());
    }

    return ret;
}

QTM_END_NAMESPACE
