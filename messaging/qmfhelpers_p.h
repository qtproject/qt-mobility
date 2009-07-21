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

#ifndef _QMFHELPERS_H_
#define _QMFHELPERS_H_

#include "qmessage.h"
#include "qmessageaccountid.h"
#include "qmessagefolderid.h"
#include "qmessagestore.h"
#include "qmessagedatacomparator.h"

#include <qmaildatacomparator.h>
#include <qmailmessage.h>
#include <qmailstore.h>

class QMessageAccountFilterKey;
class QMessageFolderFilterKey;
class QMessageFilterKey;

class QMailAccountKey;
class QMailFolderKey;
class QMailMessageKey;

namespace QmfHelpers {

QMessageId convert(const QMailMessageId &id);
QMailMessageId convert(const QMessageId &id);

QMessageAccountId convert(const QMailAccountId &id);
QMailAccountId convert(const QMessageAccountId &id);

QMessageFolderId convert(const QMailFolderId &id);
QMailFolderId convert(const QMessageFolderId &id);

QMessageContentContainerId convert(const QMailMessagePart::Location &location);
QMailMessagePart::Location convert(const QMessageContentContainerId &id);

QMessageIdList convert(const QList<QMailMessageId> &ids);
QList<QMailMessageId> convert(const QMessageIdList &ids);

QMessageAccountIdList convert(const QList<QMailAccountId> &ids);
QList<QMailAccountId> convert(const QMessageAccountIdList &ids);

QMessageFolderIdList convert(const QList<QMailFolderId> &ids);
QList<QMailFolderId> convert(const QMessageFolderIdList &ids);

QMessageContentContainerIdList convert(const QList<QMailMessagePart::Location> &locations);
QList<QMailMessagePart::Location> convert(const QMessageContentContainerIdList &ids);

QMailMessage::MessageType convert(QMessage::Type t);
QMessage::Type convert(QMailMessage::MessageType t);

QMessage::StatusFlags convert(quint64 v);
quint64 convert(QMessage::StatusFlags v);

QMailStore::ErrorCode convert(QMessageStore::ErrorCode v);
QMessageStore::ErrorCode convert(QMailStore::ErrorCode v);

QMailStore::MessageRemovalOption convert(QMessageStore::RemovalOption v);
QMessageStore::RemovalOption convert(QMailStore::MessageRemovalOption v);

QMessageAddress convert(const QMailAddress &address);
QMailAddress convert(const QMessageAddress &address);

QMessageAddressList convert(const QList<QMailAddress> &list);
QList<QMailAddress> convert(const QMessageAddressList& list);

QMessageDataComparator::EqualityComparator convert(QMailDataComparator::EqualityComparator cmp);
QMailDataComparator::EqualityComparator convert(QMessageDataComparator::EqualityComparator cmp);

QMessageDataComparator::InclusionComparator convert(QMailDataComparator::InclusionComparator cmp);
QMailDataComparator::InclusionComparator convert(QMessageDataComparator::InclusionComparator cmp);

QMessageDataComparator::RelationComparator convert(QMailDataComparator::RelationComparator cmp);
QMailDataComparator::RelationComparator convert(QMessageDataComparator::RelationComparator cmp);

QMessageAccountFilterKey convert(const QMailAccountKey &key);
QMailAccountKey convert(const QMessageAccountFilterKey &key);

QMessageFolderFilterKey convert(const QMailFolderKey &key);
QMailFolderKey convert(const QMessageFolderFilterKey &key);

QMessageFilterKey convert(const QMailMessageKey &key);
QMailMessageKey convert(const QMessageFilterKey &key);

QMessageAccountSortKey convert(const QMailAccountSortKey &key);
QMailAccountSortKey convert(const QMessageAccountSortKey &key);

QMessageFolderSortKey convert(const QMailFolderSortKey &key);
QMailFolderSortKey convert(const QMessageFolderSortKey &key);

QMessageSortKey convert(const QMailMessageSortKey &key);
QMailMessageSortKey convert(const QMessageSortKey &key);

};

bool operator==(const QMailMessagePart::Location &lhs, const QMailMessagePart::Location &rhs);

#endif
