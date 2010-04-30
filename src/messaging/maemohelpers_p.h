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

#ifndef QMESSAGINGMAEMOHELPERPRIVATE_H
#define QMESSAGINGMAEMOHELPERPRIVATE_H

#include <qmessageglobal.h>
#include <qmessageid.h>
#include <qmessagesortorder.h>
#include <qmessageaccountid.h>
#include <qmessageaccountsortorder.h>
#include <qmessagefolderid.h>
#include <qmessagefoldersortorder.h>

QTM_BEGIN_NAMESPACE

class QMessageAccountFilter;
class QMessageAccountSortOrder;
class QMessageFolderFilter;
class QMessageFolderSortOrder;
class QMessageFilter;
class QMessageSortOrder;

class MessagingHelper
{
public:
    static MessagingHelper* instance();

    MessagingHelper();
    ~MessagingHelper();

    static void filterAccounts(QMessageAccountIdList& accountIds, const QMessageAccountFilter& filter);
    static void orderAccounts(QMessageAccountIdList& accountIds, const QMessageAccountSortOrder &sortOrder);
    static void applyOffsetAndLimitToAccountIdList(QMessageAccountIdList& accountIds, int limit, int offset);
    
    static void filterFolders(QMessageFolderIdList& folderIds, const QMessageFolderFilter& filter);
    static void orderFolders(QMessageFolderIdList& folderIds,  const QMessageFolderSortOrder &sortOrder);
    static void applyOffsetAndLimitToFolderIdList(QMessageFolderIdList& folderIds, int limit, int offset);

    static void filterMessages(QMessageIdList& messageIds, const QMessageFilter& filter);
    static void orderMessages(QMessageIdList& messageIds,  const QMessageSortOrder &sortOrder);
    static void applyOffsetAndLimitToMessageIdList(QMessageIdList& messageIds, int limit, int offset);

    static void handleNestedFiltersFromFolderFilter(QMessageFolderFilter &filter);
    static void handleNestedFiltersFromMessageFilter(QMessageFilter &filter);

private:
   static bool accountLessThan(const QMessageAccountId accountId1, const QMessageAccountId accountId2);
   static bool folderLessThan(const QMessageFolderId folderId1, const QMessageFolderId folderId2);
   static bool messageLessThan(const QMessageId messageId1, const QMessageId messageId2);

private:
    QMessageAccountSortOrder* m_AccountSortOrder;
    QMessageFolderSortOrder* m_FolderSortOrder;
    QMessageSortOrder* m_MessageSortOrder;
};

QTM_END_NAMESPACE

#endif // QMESSAGINGMAEMOHELPERPRIVATE_H
