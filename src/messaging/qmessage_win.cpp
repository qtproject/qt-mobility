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
#include "qmessage.h"
#include "qmessage_p.h"
#include "qmessageid_p.h"
#include "qmessageaccountid_p.h"
#include "qmessagestore.h"
#include "qmessagecontentcontainer_p.h"
#include "qmessagefolderid_p.h"
#include "winhelpers_p.h"
#include <QDebug>

namespace {

QByteArray charsetFor(const QString &input)
{
    QByteArray result(QMessage::preferredCharsetFor(input));
    if (result.isEmpty()) {
        result = "UTF-16";
    }

    return result;
}

}

QMessage QMessagePrivate::from(const QMessageId &id)
{
    QMessage result;
    result.d_ptr->_id = id;
    result.d_ptr->_parentAccountId = QMessageAccountIdPrivate::from(QMessageIdPrivate::storeRecordKey(id));
    return result;
}

QString QMessagePrivate::senderName(const QMessage &message)
{
    return message.d_ptr->_senderName;
}

void QMessagePrivate::setSenderName(const QMessage &message, const QString &senderName)
{
    message.d_ptr->_senderName = senderName;
}

void QMessagePrivate::setSize(const QMessage &message, uint size)
{
    message.d_ptr->_size = size;
}

void QMessagePrivate::setParentFolderId(QMessage& message, const QMessageFolderId& id)
{
    message.d_ptr->_parentFolderId = id;
    message.d_ptr->_modified = true;
}

void QMessagePrivate::setStandardFolder(QMessage& message, QMessage::StandardFolder sf)
{
    message.d_ptr->_standardFolder = sf;
    message.d_ptr->_modified = true;
}

void QMessagePrivate::ensurePropertiesPresent(QMessage *msg) const
{
    if (!_elementsPresent.properties && _id.isValid()) {
        QMessageStore::ErrorCode ignoredError(QMessageStore::NoError);
        MapiSessionPtr session(MapiSession::createSession(&ignoredError));
        if (!session.isNull()) {
            session->updateMessageProperties(&ignoredError, msg);
        }
    }
}

void QMessagePrivate::ensureRecipientsPresent(QMessage *msg) const
{
    if (!_elementsPresent.recipients && _id.isValid()) {
        QMessageStore::ErrorCode ignoredError(QMessageStore::NoError);
        MapiSessionPtr session(MapiSession::createSession(&ignoredError));
        if (!session.isNull()) {
            session->updateMessageRecipients(&ignoredError, msg);
        }
    }
}

void QMessagePrivate::ensureBodyPresent(QMessage *msg) const
{
    if (!_elementsPresent.body && _id.isValid()) {
        QMessageStore::ErrorCode ignoredError(QMessageStore::NoError);
        MapiSessionPtr session(MapiSession::createSession(&ignoredError));
        if (!session.isNull()) {
            session->updateMessageBody(&ignoredError, msg);
        }
    }
}

void QMessagePrivate::ensureAttachmentsPresent(QMessage *msg) const
{
    if (!_elementsPresent.attachments && _id.isValid()) {
        QMessageStore::ErrorCode ignoredError(QMessageStore::NoError);
        MapiSessionPtr session(MapiSession::createSession(&ignoredError));
        if (!session.isNull()) {
            session->updateMessageAttachments(&ignoredError, msg);
        }
    }
}


QMessage::QMessage()
    :
    QMessageContentContainer(),
    d_ptr(new QMessagePrivate(this))
{
    d_ptr->_modified = false;
    d_ptr->_size = 0;
    setDerivedMessage(this);
}

QMessage::QMessage(const QMessageId& id)
    :
    QMessageContentContainer(),
    d_ptr(new QMessagePrivate(this))
{
    *this = QMessageStore::instance()->message(id);
    setDerivedMessage(this);
}

QMessage::QMessage(const QMessage &other)
    :
    QMessageContentContainer(other),
    d_ptr(new QMessagePrivate(this))
{
    this->operator=(other);
    setDerivedMessage(this);
}

QMessage& QMessage::operator=(const QMessage& other)
{
    if (&other != this) {
        QMessageContentContainer::operator=(other);
        *d_ptr = *other.d_ptr;
        setDerivedMessage(this);
    }

    return *this;
}

QMessage::~QMessage()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessage QMessage::fromTransmissionFormat(Type t, const QByteArray &ba)
{
    Q_UNUSED(t)
    Q_UNUSED(ba)
    return QMessage(); // stub
}

QMessage QMessage::fromTransmissionFormatFile(Type t, const QString& fileName)
{
    Q_UNUSED(t)
    Q_UNUSED(fileName)
    return QMessage(); // stub
}

QByteArray QMessage::toTransmissionFormat() const
{
    return QByteArray(); // stub
}

void QMessage::toTransmissionFormat(QDataStream& out) const
{
    Q_UNUSED(out)
}

QMessageId QMessage::id() const
{
    return d_ptr->_id;
}

QMessage::Type QMessage::type() const
{
    d_ptr->ensurePropertiesPresent(const_cast<QMessage*>(this));
    return d_ptr->_type;
}

void QMessage::setType(Type t)
{
    d_ptr->_modified = true;
    d_ptr->_type = t;
}

QMessageAccountId QMessage::parentAccountId() const
{
    return d_ptr->_parentAccountId;
}

void QMessage::setParentAccountId(const QMessageAccountId &accountId)
{
    d_ptr->_modified = true;
    d_ptr->_parentAccountId = accountId;
}

QMessageFolderId QMessage::parentFolderId() const
{
    d_ptr->ensurePropertiesPresent(const_cast<QMessage*>(this));
    return d_ptr->_parentFolderId;
}

QMessage::StandardFolder QMessage::standardFolder() const
{
    d_ptr->ensurePropertiesPresent(const_cast<QMessage*>(this));
    return d_ptr->_standardFolder;
}

QMessageAddress QMessage::from() const
{
    d_ptr->ensurePropertiesPresent(const_cast<QMessage*>(this));
    return d_ptr->_from;
}

void QMessage::setFrom(const QMessageAddress &address)
{
    d_ptr->_modified = true;
    d_ptr->_from = address;
}

QString QMessage::subject() const
{
    d_ptr->ensurePropertiesPresent(const_cast<QMessage*>(this));
    return d_ptr->_subject;
}

void QMessage::setSubject(const QString &s)
{
    d_ptr->_modified = true;
    d_ptr->_subject = s;
}

QDateTime QMessage::date() const
{
    d_ptr->ensurePropertiesPresent(const_cast<QMessage*>(this));
    return d_ptr->_date;
}

void QMessage::setDate(const QDateTime &d)
{
    d_ptr->_modified = true;
    d_ptr->_date = d;
}

QDateTime QMessage::receivedDate() const
{
    d_ptr->ensurePropertiesPresent(const_cast<QMessage*>(this));
    return d_ptr->_receivedDate;
}

void QMessage::setReceivedDate(const QDateTime &d)
{
    d_ptr->_modified = true;
    d_ptr->_receivedDate = d;
}

QMessageAddressList QMessage::to() const
{
    d_ptr->ensureRecipientsPresent(const_cast<QMessage*>(this));
    return d_ptr->_to;
}

void QMessage::setTo(const QMessageAddressList& toList)
{
    d_ptr->_modified = true;
    d_ptr->_to = toList;
}

void QMessage::setTo(const QMessageAddress& address)
{
    d_ptr->_modified = true;
    d_ptr->_to = QMessageAddressList() << address;
}

QMessageAddressList QMessage::cc() const
{
    d_ptr->ensureRecipientsPresent(const_cast<QMessage*>(this));
    return d_ptr->_cc;
}

void QMessage::setCc(const QMessageAddressList& ccList)
{
    d_ptr->_modified = true;
    d_ptr->_cc = ccList;
}

QMessageAddressList QMessage::bcc() const
{
    d_ptr->ensureRecipientsPresent(const_cast<QMessage*>(this));
    return d_ptr->_bcc;
}

void QMessage::setBcc(const QMessageAddressList& bccList)
{
    d_ptr->_modified = true;
    d_ptr->_bcc = bccList;
}

QMessage::StatusFlags QMessage::status() const
{
    d_ptr->ensurePropertiesPresent(const_cast<QMessage*>(this));
    return d_ptr->_status;
}

void QMessage::setStatus(QMessage::StatusFlags newStatus)
{
    d_ptr->_modified = true;
    d_ptr->_status = newStatus;
}

void QMessage::setStatus(QMessage::Status flag, bool set)
{
    d_ptr->_modified = true;
    if (set) {
        d_ptr->_status |= flag;
    } else {
        d_ptr->_status &= ~flag;
    }
}

QMessage::Priority QMessage::priority() const
{
    d_ptr->ensurePropertiesPresent(const_cast<QMessage*>(this));
    return d_ptr->_priority;
}

void QMessage::setPriority(Priority newPriority)
{
    d_ptr->_modified = true;
    d_ptr->_priority = newPriority;
}

uint QMessage::size() const
{
    uint size = 0;

    d_ptr->ensurePropertiesPresent(const_cast<QMessage*>(this));
    if (d_ptr->_size != 0) {
        size = d_ptr->_size;
    } else {
        QMessageContentContainerPrivate *container(((QMessageContentContainer *)(this))->d_ptr);
        if (container->_size != 0) {
            size += ((container->_size / 1024) + 1) * 1024;
        }
        foreach (const QMessageContentContainer &attachment, container->_attachments) {
            // Round size to a multiple of 1KB
            size += ((attachment.size() / 1024) + 1) * 1024;
        }

        // Allow for header
        size += 1024;
    }

    return size;
}

QMessageContentContainerId QMessage::bodyId() const
{
    d_ptr->ensureBodyPresent(const_cast<QMessage*>(this));
    return d_ptr->_bodyId;
}

void QMessage::setBody(const QString &bodyText, const QByteArray &mimeType)
{
    QByteArray mainType("text");
    QByteArray subType("plain");
    QByteArray charset;

    int index = mimeType.indexOf("/");
    if (index != -1) {
        mainType = mimeType.left(index).trimmed();

        subType = mimeType.mid(index + 1).trimmed();
        index = subType.indexOf(";");
        if (index != -1) {
             QString remainder = subType.mid(index + 1);
            subType = subType.left(index).trimmed();

            QRegExp charsetPattern("charset=(\\S+)");
            index = charsetPattern.indexIn(remainder);
            if (index != -1) {
                charset = charsetPattern.cap(1).toLatin1();
            }
       }
    }

    if (charset.isEmpty()) {
        charset = charsetFor(bodyText);
    }

    QMessageContentContainerPrivate *container(((QMessageContentContainer *)(this))->d_ptr);

    QMessageContentContainerId existingBodyId(bodyId());
    if (existingBodyId.isValid()) {
        if (existingBodyId == container->bodyContentId()) {
            // The body content is in the message itself
            container->setContent(bodyText, mainType, subType, charset);
        } else {
            // The body content is in the first attachment
            QMessageContentContainerPrivate *attachmentContainer(container->attachment(existingBodyId)->d_ptr);
            attachmentContainer->setContent(bodyText, mainType, subType, charset);
        }
    } else {
        if (container->_attachments.isEmpty()) {
            // Put the content directly into the message
            container->setContent(bodyText, mainType, subType, charset);
            d_ptr->_bodyId = container->bodyContentId();
        } else {
            // Add the body as the first attachment
            QMessageContentContainer newBody;
            newBody.d_ptr->setContent(bodyText, mainType, subType, charset);
            d_ptr->_bodyId = container->prependContent(newBody);
        }
    }
}

void QMessage::setBody(QTextStream &in, const QByteArray &mimeType)
{
    setBody(in.readAll(), mimeType);
}

QMessageContentContainerIdList QMessage::attachmentIds() const
{
    d_ptr->ensureAttachmentsPresent(const_cast<QMessage*>(this));

    QMessageContentContainerIdList result;

    QMessageContentContainerId bodyId(bodyId());
    foreach (const QMessageContentContainerId &contentId, contentIds()) {
        if (contentId != bodyId) {
            result.append(contentId);
        }
    }

    return result;
}

void QMessage::appendAttachments(const QStringList &fileNames)
{
    if (!fileNames.isEmpty()) {
        d_ptr->_modified = true;

        QMessageContentContainerPrivate *container(((QMessageContentContainer *)(this))->d_ptr);

        if (container->_attachments.isEmpty()) {
            QMessageContentContainerId existingBodyId(bodyId());
            if (existingBodyId == container->bodyContentId()) {
                // The body content is in the message itself - move it to become the first attachment
                QMessageContentContainer newBody(*this);
                newBody.setDerivedMessage(0);

                container->setContentType("multipart", "mixed", "");
                d_ptr->_bodyId = container->prependContent(newBody);
            } else {
                // This message is now multipart
                container->setContentType("multipart", "mixed", "");
            }

            container->_available = true;
        }

        foreach (const QString &filename, fileNames) {
            QMessageContentContainer attachment;
            if (attachment.d_ptr->createAttachment(filename)) {
                container->appendContent(attachment);
            }
        }

        bool haveAttachments = !container->_attachments.isEmpty();
        setStatus(QMessage::HasAttachments,haveAttachments);
    }
}

void QMessage::clearAttachments()
{
    d_ptr->_modified = true;

    QMessageContentContainerPrivate *container(((QMessageContentContainer *)(this))->d_ptr);
    container->_attachments.clear();

    bool haveAttachments = !container->_attachments.isEmpty();
    setStatus(QMessage::HasAttachments,haveAttachments);
}

bool QMessage::isModified() const
{
    return d_ptr->_modified;
}

QMessage QMessage::createResponseMessage(ResponseType type) const
{
    Q_UNUSED(type)
    return QMessage(); // stub
}
