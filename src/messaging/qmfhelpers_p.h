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

#ifndef _QMFHELPERS_H_
#define _QMFHELPERS_H_

#include "qmessageglobal.h"
#include "qmessage.h"
#include "qmessageaccount.h"
#include "qmessagedatacomparator.h"
#include "qmessagefolder.h"
#include "qmessageservice.h"
#include "qmessagemanager.h"

#include <qmaildatacomparator.h>
#include <qmailmessage.h>
#include <qmailserviceaction.h>
#include <qmailstore.h>

class QMailFolderKey;
class QMailMessageKey;
class QMailAccountKey;

QTM_BEGIN_NAMESPACE

class QMessageAccountFilter;
class QMessageFolderFilter;
class QMessageFilter;

namespace QmfHelpers {

    Q_AUTOTEST_EXPORT QMessageId convert(const QMailMessageId &id);
    Q_AUTOTEST_EXPORT QMailMessageId convert(const QMessageId &id);

    Q_AUTOTEST_EXPORT QMessageAccountId convert(const QMailAccountId &id);
    Q_AUTOTEST_EXPORT QMailAccountId convert(const QMessageAccountId &id);

    Q_AUTOTEST_EXPORT QMessageFolderId convert(const QMailFolderId &id);
    Q_AUTOTEST_EXPORT QMailFolderId convert(const QMessageFolderId &id);

    Q_AUTOTEST_EXPORT QMessageContentContainerId convert(const QMailMessagePart::Location &location);
    Q_AUTOTEST_EXPORT QMailMessagePart::Location convert(const QMessageContentContainerId &id);

    Q_AUTOTEST_EXPORT QMessageContentContainerId bodyId(const QMailMessageId &id);

    Q_AUTOTEST_EXPORT QMessageIdList convert(const QList<QMailMessageId> &ids);
    Q_AUTOTEST_EXPORT QList<QMailMessageId> convert(const QMessageIdList &ids);

    Q_AUTOTEST_EXPORT QMessageAccountIdList convert(const QList<QMailAccountId> &ids);
    Q_AUTOTEST_EXPORT QList<QMailAccountId> convert(const QMessageAccountIdList &ids);

    Q_AUTOTEST_EXPORT QMessageFolderIdList convert(const QList<QMailFolderId> &ids);
    Q_AUTOTEST_EXPORT QList<QMailFolderId> convert(const QMessageFolderIdList &ids);

    Q_AUTOTEST_EXPORT QMessageContentContainerIdList convert(const QList<QMailMessagePart::Location> &locations);
    //QList<QMailMessagePart::Location> convert(const QMessageContentContainerIdList &ids);

    Q_AUTOTEST_EXPORT QMailMessage::MessageType convert(QMessage::Type t);
    Q_AUTOTEST_EXPORT QMessage::Type convert(QMailMessage::MessageType t);

    Q_AUTOTEST_EXPORT QMailMessage::MessageType convert(QMessage::TypeFlags v);

    Q_AUTOTEST_EXPORT QMessage::StatusFlags convert(quint64 v);
    Q_AUTOTEST_EXPORT quint64 convert(QMessage::Status v);

    Q_AUTOTEST_EXPORT quint64 convert(QMessage::StatusFlags v);

    Q_AUTOTEST_EXPORT QMessageManager::Error convert(QMailStore::ErrorCode v);
    //QMailStore::ErrorCode convert(QMessageManager::Error v);

    //QMessageManager::RemovalOption convert(QMailStore::MessageRemovalOption v);
    Q_AUTOTEST_EXPORT QMailStore::MessageRemovalOption convert(QMessageManager::RemovalOption v);

    Q_AUTOTEST_EXPORT QMessageService::State convert(QMailServiceAction::Activity v);
    Q_AUTOTEST_EXPORT QMailServiceAction::Activity convert(QMessageService::State v);

    Q_AUTOTEST_EXPORT QMessageAddress convert(const QMailAddress &address);
    Q_AUTOTEST_EXPORT QMailAddress convert(const QMessageAddress &address);

    Q_AUTOTEST_EXPORT QMessageAddressList convert(const QList<QMailAddress> &list);
    Q_AUTOTEST_EXPORT QList<QMailAddress> convert(const QMessageAddressList& list);

    //QMessageDataComparator::EqualityComparator convert(QMailDataComparator::EqualityComparator cmp);
    Q_AUTOTEST_EXPORT QMailDataComparator::EqualityComparator convert(QMessageDataComparator::EqualityComparator cmp);

    //QMessageDataComparator::InclusionComparator convert(QMailDataComparator::InclusionComparator cmp);
    Q_AUTOTEST_EXPORT QMailDataComparator::InclusionComparator convert(QMessageDataComparator::InclusionComparator cmp);

    //QMessageDataComparator::RelationComparator convert(QMailDataComparator::RelationComparator cmp);
    Q_AUTOTEST_EXPORT QMailDataComparator::RelationComparator convert(QMessageDataComparator::RelationComparator cmp);

    //QMessageAccountFilter convert(const QMailAccountKey &key);
    Q_AUTOTEST_EXPORT QMailAccountKey convert(const QMessageAccountFilter &filter);

    //QMessageFolderFilter convert(const QMailFolderKey &key);
    Q_AUTOTEST_EXPORT QMailFolderKey convert(const QMessageFolderFilter &filter);

    //QMessageFilter convert(const QMailMessageKey &key);
    Q_AUTOTEST_EXPORT QMailMessageKey convert(const QMessageFilter &filter);

    //QMessageAccountSortOrder convert(const QMailAccountSortKey &key);
    Q_AUTOTEST_EXPORT QMailAccountSortKey convert(const QMessageAccountSortOrder &sortOrder);

    //QMessageFolderSortOrder convert(const QMailFolderSortKey &key);
    Q_AUTOTEST_EXPORT QMailFolderSortKey convert(const QMessageFolderSortOrder &sortOrder);

    //QMessageSortOrder convert(const QMailMessageSortKey &key);
    Q_AUTOTEST_EXPORT QMailMessageSortKey convert(const QMessageSortOrder &sortOrder);

    Q_AUTOTEST_EXPORT QMessageAccount convert(const QMailAccount &account);
    //QMailAccount convert(const QMessageAccount &account);

    Q_AUTOTEST_EXPORT QMessageFolder convert(const QMailFolder &folder);
    //QMailFolder convert(const QMessageFolder &folder);

    Q_AUTOTEST_EXPORT QMessage convert(const QMailMessage &message);
    QMailMessage convert(const QMessage &message);

    Q_AUTOTEST_EXPORT QMailMessage* convert(QMessage *message);
    //const QMailMessage* convert(const QMessage *message);

    Q_AUTOTEST_EXPORT QMailStore *convert(QMessageStore *store);
    Q_AUTOTEST_EXPORT QMailStore *convert(QMessageManager &manager);

    Q_AUTOTEST_EXPORT quint64 highPriorityMask();
    Q_AUTOTEST_EXPORT quint64 lowPriorityMask();

};

bool operator==(const QMailMessagePart::Location &lhs, const QMailMessagePart::Location &rhs);


QTM_END_NAMESPACE
#endif
