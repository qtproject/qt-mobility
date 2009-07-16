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

#include "qmfhelpers_p.h"

namespace QmfHelpers {

QMessageId convert(const QMailMessageId &id)
{
    return QMessageId(QString::number(id.toULongLong()));
}

QMailMessageId convert(const QMessageId &id)
{
    return QMailMessageId(id.toString().toULongLong());
}

QMessageAccountId convert(const QMailAccountId &id)
{
    return QMessageAccountId(QString::number(id.toULongLong()));
}

QMailAccountId convert(const QMessageAccountId &id)
{
    return QMailAccountId(id.toString().toULongLong());
}

QMessageFolderId convert(const QMailFolderId &id)
{
    return QMessageFolderId(QString::number(id.toULongLong()));
}

QMailFolderId convert(const QMessageFolderId &id)
{
    return QMailFolderId(id.toString().toULongLong());
}

QMessageContentContainerId convert(const QMailMessagePart::Location &location)
{
    return QMessageContentContainerId(location.toString(true));
}

QMailMessagePart::Location convert(const QMessageContentContainerId &id)
{
    return QMailMessagePart::Location(id.toString());
}

QMessageContentContainerIdList convert(const QList<QMailMessagePart::Location> &locations)
{
    QMessageContentContainerIdList result;

    foreach (const QMailMessagePart::Location &location, locations) {
        result.append(convert(location));
    }

    return result;
}

QList<QMailMessagePart::Location> convert(const QMessageContentContainerIdList &ids)
{
    QList<QMailMessagePart::Location> result;

    foreach (const QMessageContentContainerId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QMailMessage::MessageType convert(QMessage::Type t)
{
    switch (t) {
    case QMessage::Mms: return QMailMessage::Mms;
    case QMessage::Sms: return QMailMessage::Sms;
    case QMessage::Email: return QMailMessage::Email;
    case QMessage::Xmpp: return QMailMessage::Instant;
    default: break;
    }

    return QMailMessage::None;
}

QMessage::Type convert(QMailMessage::MessageType t)
{
    switch (t) {
    case QMailMessage::Mms: return QMessage::Mms;
    case QMailMessage::Sms: return QMessage::Sms;
    case QMailMessage::Email: return QMessage::Email;
    case QMailMessage::Instant: return QMessage::Xmpp;
    default: break;
    }

    return QMessage::None;
}

QMessage::StatusFlags convert(quint64 v)
{
    QMessage::StatusFlags result;

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

quint64 convert(QMessage::StatusFlags v)
{
    quint64 result;

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

QMessageAddress convert(const QMailAddress &address)
{
    return QMessageAddress(address.toString(), QMessageAddress::Email);
}

QMailAddress convert(const QMessageAddress &address)
{
    return QMailAddress(address.recipient());
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

};

