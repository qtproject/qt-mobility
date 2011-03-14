/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef _QMFHELPERS_MAEMO6_H_
#define _QMFHELPERS_MAEMO6_H_

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

    QM_AUTOTEST_EXPORT QMessageId convert(const QMailMessageId &id);
    QM_AUTOTEST_EXPORT QMailMessageId convert(const QMessageId &id);

    QM_AUTOTEST_EXPORT QMessageAccountId convert(const QMailAccountId &id);
    QM_AUTOTEST_EXPORT QMailAccountId convert(const QMessageAccountId &id);

    QM_AUTOTEST_EXPORT QMessageFolderId convert(const QMailFolderId &id);
    QM_AUTOTEST_EXPORT QMailFolderId convert(const QMessageFolderId &id);

    QM_AUTOTEST_EXPORT QMessageContentContainerId convert(const QMailMessagePart::Location &location);
    QM_AUTOTEST_EXPORT QMailMessagePart::Location convert(const QMessageContentContainerId &id);

    QM_AUTOTEST_EXPORT QMessageContentContainerId bodyId(const QMailMessageId &id);

    QM_AUTOTEST_EXPORT QMessageIdList convert(const QList<QMailMessageId> &ids);
    QM_AUTOTEST_EXPORT QList<QMailMessageId> convert(const QMessageIdList &ids);

    QM_AUTOTEST_EXPORT QMessageAccountIdList convert(const QList<QMailAccountId> &ids);
    QM_AUTOTEST_EXPORT QList<QMailAccountId> convert(const QMessageAccountIdList &ids);

    QM_AUTOTEST_EXPORT QMessageFolderIdList convert(const QList<QMailFolderId> &ids);
    QM_AUTOTEST_EXPORT QList<QMailFolderId> convert(const QMessageFolderIdList &ids);

    QM_AUTOTEST_EXPORT QMessageContentContainerIdList convert(const QList<QMailMessagePart::Location> &locations);
    //QList<QMailMessagePart::Location> convert(const QMessageContentContainerIdList &ids);

    QM_AUTOTEST_EXPORT QMailMessage::MessageType convert(QMessage::Type t);
    QM_AUTOTEST_EXPORT QMessage::Type convert(QMailMessage::MessageType t);

    QM_AUTOTEST_EXPORT QMailMessage::MessageType convert(QMessage::TypeFlags v);

    QM_AUTOTEST_EXPORT QMessage::StatusFlags convert(quint64 v);
    QM_AUTOTEST_EXPORT quint64 convert(QMessage::Status v);

    QM_AUTOTEST_EXPORT quint64 convert(QMessage::StatusFlags v);

    QM_AUTOTEST_EXPORT QMessageManager::Error convert(QMailStore::ErrorCode v);
    //QMailStore::ErrorCode convert(QMessageManager::Error v);

    //QMessageManager::RemovalOption convert(QMailStore::MessageRemovalOption v);
    QM_AUTOTEST_EXPORT QMailStore::MessageRemovalOption convert(QMessageManager::RemovalOption v);

    QM_AUTOTEST_EXPORT QMessageService::State convert(QMailServiceAction::Activity v);
    QM_AUTOTEST_EXPORT QMailServiceAction::Activity convert(QMessageService::State v);

    QM_AUTOTEST_EXPORT QMessageAddress convert(const QMailAddress &address);
    QM_AUTOTEST_EXPORT QMailAddress convert(const QMessageAddress &address);

    QM_AUTOTEST_EXPORT QMessageAddressList convert(const QList<QMailAddress> &list);
    QM_AUTOTEST_EXPORT QList<QMailAddress> convert(const QMessageAddressList& list);

    //QMessageDataComparator::EqualityComparator convert(QMailDataComparator::EqualityComparator cmp);
    QM_AUTOTEST_EXPORT QMailDataComparator::EqualityComparator convert(QMessageDataComparator::EqualityComparator cmp);

    //QMessageDataComparator::InclusionComparator convert(QMailDataComparator::InclusionComparator cmp);
    QM_AUTOTEST_EXPORT QMailDataComparator::InclusionComparator convert(QMessageDataComparator::InclusionComparator cmp);

    //QMessageDataComparator::RelationComparator convert(QMailDataComparator::RelationComparator cmp);
    QM_AUTOTEST_EXPORT QMailDataComparator::RelationComparator convert(QMessageDataComparator::RelationComparator cmp);

    //QMessageAccountFilter convert(const QMailAccountKey &key);
    QM_AUTOTEST_EXPORT QMailAccountKey convert(const QMessageAccountFilter &filter);

    //QMessageFolderFilter convert(const QMailFolderKey &key);
    QM_AUTOTEST_EXPORT QMailFolderKey convert(const QMessageFolderFilter &filter);

    //QMessageFilter convert(const QMailMessageKey &key);
    QM_AUTOTEST_EXPORT QMailMessageKey convert(const QMessageFilter &filter);

    //QMessageAccountSortOrder convert(const QMailAccountSortKey &key);
    QM_AUTOTEST_EXPORT QMailAccountSortKey convert(const QMessageAccountSortOrder &sortOrder);

    //QMessageFolderSortOrder convert(const QMailFolderSortKey &key);
    QM_AUTOTEST_EXPORT QMailFolderSortKey convert(const QMessageFolderSortOrder &sortOrder);

    //QMessageSortOrder convert(const QMailMessageSortKey &key);
    QM_AUTOTEST_EXPORT QMailMessageSortKey convert(const QMessageSortOrder &sortOrder);

    QM_AUTOTEST_EXPORT QMessageAccount convert(const QMailAccount &account);
    //QMailAccount convert(const QMessageAccount &account);

    QM_AUTOTEST_EXPORT QMessageFolder convert(const QMailFolder &folder);
    //QMailFolder convert(const QMessageFolder &folder);

    QM_AUTOTEST_EXPORT QMessage convert(const QMailMessage &message);
    QM_AUTOTEST_EXPORT QMailMessage convert(const QMessage &message);

    QM_AUTOTEST_EXPORT quint64 highPriorityMask();
    QM_AUTOTEST_EXPORT quint64 lowPriorityMask();

};

QTM_END_NAMESPACE
#endif
