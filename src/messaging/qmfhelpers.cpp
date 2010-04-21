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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmfhelpers_p.h"
#include "qmessagemanager.h"

#include <QRegExp>


QTM_BEGIN_NAMESPACE

namespace {

quint64 messageStatusMask(const QString &field)
{
    QMessageManager mgr;
    return QmfHelpers::convert(mgr)->messageStatusMask(field);
}

}

namespace QmfHelpers {

QString stripIdentifierPrefix(const QString &s)
{
    return s.mid(4);
}
    
QString prefixIdentifier(const QString &s)
{
    return "QMF_" + s;
}
    
QMessageId convert(const QMailMessageId &id)
{
    return QMessageId(prefixIdentifier(QString::number(id.toULongLong())));
}

QMailMessageId convert(const QMessageId &id)
{
    return QMailMessageId(stripIdentifierPrefix(id.toString()).toULongLong());
}

QMessageAccountId convert(const QMailAccountId &id)
{
    return QMessageAccountId(prefixIdentifier(QString::number(id.toULongLong())));
}

QMailAccountId convert(const QMessageAccountId &id)
{
    return QMailAccountId(stripIdentifierPrefix(id.toString()).toULongLong());
}

QMessageFolderId convert(const QMailFolderId &id)
{
    return QMessageFolderId(prefixIdentifier(QString::number(id.toULongLong())));
}

QMailFolderId convert(const QMessageFolderId &id)
{
    return QMailFolderId(stripIdentifierPrefix(id.toString()).toULongLong());
}

/* in qmessagecontentcontainerid_qmf.cpp
QMessageContentContainerId convert(const QMailMessagePart::Location &location);

QMailMessagePart::Location convert(const QMessageContentContainerId &id);

QMessageContentContainerId bodyId(const QMailMessageId &id);
*/

QMessageIdList convert(const QList<QMailMessageId> &ids)
{
    QMessageIdList result;

    foreach (const QMailMessageId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QList<QMailMessageId> convert(const QMessageIdList &ids)
{
    QList<QMailMessageId> result;

    foreach (const QMessageId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QMessageAccountIdList convert(const QList<QMailAccountId> &ids)
{
    QMessageAccountIdList result;

    foreach (const QMailAccountId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QList<QMailAccountId> convert(const QMessageAccountIdList &ids)
{
    QList<QMailAccountId> result;

    foreach (const QMessageAccountId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QMessageFolderIdList convert(const QList<QMailFolderId> &ids)
{
    QMessageFolderIdList result;

    foreach (const QMailFolderId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QList<QMailFolderId> convert(const QMessageFolderIdList &ids)
{
    QList<QMailFolderId> result;

    foreach (const QMessageFolderId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QMessageContentContainerIdList convert(const QList<QMailMessagePart::Location> &locations)
{
    QMessageContentContainerIdList result;

    foreach (const QMailMessagePart::Location &location, locations) {
        result.append(convert(location));
    }

    return result;
}

/*
QList<QMailMessagePart::Location> convert(const QMessageContentContainerIdList &ids)
{
    QList<QMailMessagePart::Location> result;

    foreach (const QMessageContentContainerId &id, ids) {
        result.append(convert(id));
    }

    return result;
}
*/

QMailMessage::MessageType convert(QMessage::Type t)
{
    QMailMessage::MessageType result(QMailMessage::None);

    // This could be a single value or a mask
    if (t & QMessage::Mms) {
        result = static_cast<QMailMessage::MessageType>(result | QMailMessage::Mms);
    }
    if (t & QMessage::Sms) {
        result = static_cast<QMailMessage::MessageType>(result | QMailMessage::Sms);
    }
    if (t & QMessage::Email) {
        result = static_cast<QMailMessage::MessageType>(result | QMailMessage::Email);
    }
    if (t & QMessage::InstantMessage) {
        result = static_cast<QMailMessage::MessageType>(result | QMailMessage::Instant);
    }

    return result;
}

QMessage::Type convert(QMailMessage::MessageType t)
{
    QMessage::Type result(QMessage::NoType);

    // This could be a single value or a mask
    if (t & QMailMessage::Mms) {
        result = static_cast<QMessage::Type>(static_cast<uint>(result | QMessage::Mms));
    }
    if (t & QMailMessage::Sms) {
        result = static_cast<QMessage::Type>(static_cast<uint>(result | QMessage::Sms));
    }
    if (t & QMailMessage::Email) {
        result = static_cast<QMessage::Type>(static_cast<uint>(result | QMessage::Email));
    }
    if (t & QMailMessage::Instant) {
        result = static_cast<QMessage::Type>(static_cast<uint>(result | QMessage::InstantMessage));
    }

    return result;
}

QMailMessage::MessageType convert(QMessage::TypeFlags v)
{
    return convert(static_cast<QMessage::Type>(static_cast<uint>(v)));
}

/*
QMailStore::ErrorCode convert(QMessageManager::Error v)
{
    switch (v) {
    case QMessageManager::InvalidId: return QMailStore::InvalidId;
    case QMessageManager::ConstraintFailure: return QMailStore::ConstraintFailure;
    case QMessageManager::ContentInaccessible: return QMailStore::ContentInaccessible;
    case QMessageManager::NotYetImplemented: return QMailStore::NotYetImplemented;
    case QMessageManager::FrameworkFault: return QMailStore::FrameworkFault;
    default: break;
    }

    return QMailStore::NoError;
}
*/

QMessageManager::Error convert(QMailStore::ErrorCode v)
{
    switch (v) {
    case QMailStore::InvalidId: return QMessageManager::InvalidId;
    case QMailStore::ConstraintFailure: return QMessageManager::ConstraintFailure;
    case QMailStore::ContentInaccessible: return QMessageManager::ContentInaccessible;
    case QMailStore::NotYetImplemented: return QMessageManager::NotYetImplemented;
    case QMailStore::FrameworkFault: return QMessageManager::FrameworkFault;
    default: break;
    }

    return QMessageManager::NoError;
}

QMailStore::MessageRemovalOption convert(QMessageManager::RemovalOption v)
{
    switch (v) {
    case QMessageManager::RemoveOnOriginatingServer: return QMailStore::CreateRemovalRecord;
    default: break;
    }

    return QMailStore::NoRemovalRecord;
}

/*
QMessageManager::RemovalOption convert(QMailStore::MessageRemovalOption v)
{
    switch (v) {
    case QMailStore::CreateRemovalRecord: return QMessageManager::RemoveOnOriginatingServer;
    default: break;
    }

    return QMessageManager::RemoveLocalCopyOnly;
}
*/

QMailServiceAction::Activity convert(QMessageService::State v)
{
    switch (v) {
    case QMessageService::InactiveState: return QMailServiceAction::Pending;
    case QMessageService::ActiveState: return QMailServiceAction::InProgress;
    case QMessageService::CanceledState: return QMailServiceAction::Failed;
    case QMessageService::FinishedState: return QMailServiceAction::Successful;
    default: break;
    }

    return QMailServiceAction::Pending;
}

QMessageService::State convert(QMailServiceAction::Activity v)
{
    switch (v) {
    case QMailServiceAction::Pending: return QMessageService::InactiveState;
    case QMailServiceAction::InProgress: return QMessageService::ActiveState;
    case QMailServiceAction::Successful: return QMessageService::FinishedState;
    case QMailServiceAction::Failed: return QMessageService::FinishedState;
    default: break;
    }

    return QMessageService::InactiveState;
}

QMessage::StatusFlags convert(quint64 v)
{
    QMessage::StatusFlags result(0);

    if (v & (QMailMessage::Read | QMailMessage::ReadElsewhere)) {
        result |= QMessage::Read;
    }
    if (v & QMailMessage::HasAttachments) {
        result |= QMessage::HasAttachments;
    }
    if (v & QMailMessage::Incoming) {
        result |= QMessage::Incoming;
    }
    if (v & QMailMessage::Removed) {
        result |= QMessage::Removed;
    }

    return result;
}

quint64 convert(QMessage::Status v)
{
    // We cannot rely on the QMailMessage status masks until the store has been initialized
    static QMailStore *store = QMailStore::instance();
    Q_UNUSED(store);

    quint64 result(0);

    if (v & QMessage::Read) {
        result |= QMailMessage::Read;
    }
    if (v & QMessage::HasAttachments) {
        result |= QMailMessage::HasAttachments;
    }
    if (v & QMessage::Incoming) {
        result |= QMailMessage::Incoming;
    }
    if (v & QMessage::Removed) {
        result |= QMailMessage::Removed;
    }

    return result;
}

quint64 convert(QMessage::StatusFlags v)
{
    return convert(static_cast<QMessage::Status>(static_cast<uint>(v)));
}

QMessageAddress convert(const QMailAddress &address)
{
    QString addr(address.toString());
    if (!addr.isEmpty()) {
        QMessageAddress::Type type(QMessageAddress::Email);

        QRegExp suffix("\\s+\\(TYPE=(\\w*)\\)$");
        int index = suffix.indexIn(addr);
        if (index != -1) {
            addr = addr.left(addr.length() - suffix.cap(0).length());

            QString spec(suffix.cap(1));
            if (spec == "System") {
                type = QMessageAddress::System;
            } else if (spec == "Phone") {
                type = QMessageAddress::Phone;
            } else if (spec == "InstantMessage") {
                type = QMessageAddress::InstantMessage;
            }
        }

        return QMessageAddress(type, addr);
    }

    return QMessageAddress();
}

QMailAddress convert(const QMessageAddress &address)
{
    QString suffix;
    if (address.type() == QMessageAddress::System) {
        suffix = " (TYPE=System)";
    } else if (address.type() == QMessageAddress::Phone) {
        suffix = " (TYPE=Phone)";
    } else if (address.type() == QMessageAddress::InstantMessage) {
        suffix = " (TYPE=InstantMessage)";
    }

    return QMailAddress(address.addressee() + suffix);
}

QMessageAddressList convert(const QList<QMailAddress> &list)
{
    QList<QMessageAddress> result;

    foreach (const QMailAddress &address, list) {
        result.append(convert(address));
    }

    return result;
}

QList<QMailAddress> convert(const QMessageAddressList& list)
{
    QList<QMailAddress> result;

    foreach (const QMessageAddress &address, list) {
        result.append(convert(address));
    }

    return result;
}

/*
QMessageDataComparator::EqualityComparator convert(QMailDataComparator::EqualityComparator cmp)
{
    switch (cmp)
    {
    case QMailDataComparator::Equal: return QMessageDataComparator::Equal;
    case QMailDataComparator::NotEqual: return QMessageDataComparator::NotEqual;
    default: break;
    }

    return QMessageDataComparator::Equal;
}
*/

QMailDataComparator::EqualityComparator convert(QMessageDataComparator::EqualityComparator cmp)
{
    switch (cmp)
    {
    case QMessageDataComparator::Equal: return QMailDataComparator::Equal;
    case QMessageDataComparator::NotEqual: return QMailDataComparator::NotEqual;
    default: break;
    }

    return QMailDataComparator::Equal;
}

/*
QMessageDataComparator::InclusionComparator convert(QMailDataComparator::InclusionComparator cmp)
{
    switch (cmp)
    {
    case QMailDataComparator::Includes: return QMessageDataComparator::Includes;
    case QMailDataComparator::Excludes: return QMessageDataComparator::Excludes;
    default: break;
    }
    
    return QMessageDataComparator::Includes;
}
*/

QMailDataComparator::InclusionComparator convert(QMessageDataComparator::InclusionComparator cmp)
{
    switch (cmp)
    {
    case QMessageDataComparator::Includes: return QMailDataComparator::Includes;
    case QMessageDataComparator::Excludes: return QMailDataComparator::Excludes;
    default: break;
    }

    return QMailDataComparator::Includes;
}

/*
QMessageDataComparator::RelationComparator convert(QMailDataComparator::RelationComparator cmp)
{
    switch (cmp)
    {
    case QMailDataComparator::LessThan: return QMessageDataComparator::LessThan;
    case QMailDataComparator::LessThanEqual: return QMessageDataComparator::LessThanEqual;
    case QMailDataComparator::GreaterThan: return QMessageDataComparator::GreaterThan;
    case QMailDataComparator::GreaterThanEqual: return QMessageDataComparator::GreaterThanEqual;
    default: break;
    }
    
    return QMessageDataComparator::LessThan;
}
*/

QMailDataComparator::RelationComparator convert(QMessageDataComparator::RelationComparator cmp)
{
    switch (cmp)
    {
    case QMessageDataComparator::LessThan: return QMailDataComparator::LessThan;
    case QMessageDataComparator::LessThanEqual: return QMailDataComparator::LessThanEqual;
    case QMessageDataComparator::GreaterThan: return QMailDataComparator::GreaterThan;
    case QMessageDataComparator::GreaterThanEqual: return QMailDataComparator::GreaterThanEqual;
    default: break;
    }

    return QMailDataComparator::LessThan;
}

/* in qmessageaccountfilterkey_qmf.cpp
QMessageAccountFilter convert(const QMailAccountKey &key);
QMailAccountKey convert(const QMessageAccountFilter &filter);
*/

/* in qmessagefolderfilterkey_qmf.cpp
QMessageFolderFilter convert(const QMailFolderKey &key);
QMailFolderKey convert(const QMessageFolderFilter &filter);
*/

/* in qmessagefilterkey_qmf.cpp
QMessageFilter convert(const QMailMessageKey &key);
QMailMessageKey convert(const QMessageFilter &filter);
*/

/* in qmessageaccountsortkey_qmf.cpp
QMessageAccountSortOrder convert(const QMailAccountSortKey &key);
QMailAccountSortKey convert(const QMessageAccountSortOrder &sortOrder);
*/

/* in qmessagefoldersortkey_qmf.cpp
QMessageFolderSortOrder convert(const QMailFolderSortKey &key);
QMailFolderSortKey convert(const QMessageFolderSortOrder &sortOrder);
*/

/* in qmessagesortkey_qmf.cpp
QMessageSortOrder convert(const QMailMessageSortKey &key);
QMailMessageSortKey convert(const QMessageSortOrder &sortOrder);
*/

/* in qmessageaccount_qmf.cpp
QMessageAccount convert(const QMailAccount &account);
QMailAccount convert(const QMessageAccount &account);
*/

/* in qmessagefolder_qmf.cpp
QMessageFolder convert(const QMailFolder &folder);
QMailFolder convert(const QMessageFolder &folder);
*/

/* in qmessage_qmf.cpp
QMessage convert(const QMailMessage &message);
QMailMessage convert(const QMessage &message);
QMailMessage* convert(QMessage *message);
*/

/* in qmessagestore_qmf.cpp
QMailStore *convert(QMessageStore *store);
QMailStore *convert(QMessageManager &manager);
*/

quint64 highPriorityMask()
{
    static quint64 mask(messageStatusMask("QMessage::HighPriority"));
    return mask;
}

quint64 lowPriorityMask()
{
    static quint64 mask(messageStatusMask("QMessage::LowPriority"));
    return mask;
}

}

bool operator==(const QMailMessagePart::Location &lhs, const QMailMessagePart::Location &rhs)
{
    return (lhs.toString(true) == rhs.toString(true));
}


QTM_END_NAMESPACE
