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
#include "qmessagecontentcontainer_maemo_p.h"
#include "qmessagemanager.h"
#include "qmessageid_p.h"

QTM_BEGIN_NAMESPACE

QMessage QMessagePrivate::from(const QMessageId &id)
{
    QMessage result;
    result.d_ptr->_id = id;
    return result;
}

QMessagePrivate* QMessagePrivate::implementation(const QMessage &message)
{
    return message.d_ptr;
}

QMessageContentContainerPrivate* QMessagePrivate::containerImplementation(const QMessage &message)
{
    return ((QMessageContentContainer*)&message)->d_ptr;
}

QString QMessagePrivate::senderName(const QMessage &message)
{
    return message.d_ptr->_senderName;
}

void QMessagePrivate::setSenderName(const QMessage &message, const QString &senderName)
{
    message.d_ptr->_senderName = senderName;
}

void QMessagePrivate::setSize(const QMessage &message, int size)
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

QMessage::QMessage()
 : d_ptr(new QMessagePrivate(this))
{
    setDerivedMessage(this);
    d_ptr->_size = 0;
    d_ptr->_modified = false;
}

QMessage::QMessage(const QMessageId& id)
 : d_ptr(new QMessagePrivate(this))
{
    *this = QMessageManager().message(id);
    setDerivedMessage(this);
}

QMessage::QMessage(const QMessage &other)
 : QMessageContentContainer(other),
   d_ptr(new QMessagePrivate(this))
{
    this->operator=(other);
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
}

QMessageId QMessage::id() const
{
    return d_ptr->_id;
}

QMessage::Type QMessage::type() const
{
    return d_ptr->_type;
}

void QMessage::setType(Type t)
{
    d_ptr->_type = t;
    d_ptr->_modified = true;
}

QMessageAccountId QMessage::parentAccountId() const
{
    return d_ptr->_parentAccountId;
}

void QMessage::setParentAccountId(const QMessageAccountId &accountId)
{
    d_ptr->_parentAccountId = accountId;
    d_ptr->_modified = true;
}

QMessageFolderId QMessage::parentFolderId() const
{
    return d_ptr->_parentFolderId;
}

QMessage::StandardFolder QMessage::standardFolder() const
{
    return d_ptr->_standardFolder;
}

QMessageAddress QMessage::from() const
{
    return d_ptr->_from;
}

void QMessage::setFrom(const QMessageAddress &address)
{
    d_ptr->_from = address;
    d_ptr->_modified = true;
}

QString QMessage::subject() const
{
    return d_ptr->_subject;
}

void QMessage::setSubject(const QString &s)
{
    d_ptr->_subject = s;
    d_ptr->_modified = true;
}

QDateTime QMessage::date() const
{
    return d_ptr->_date;
}

void QMessage::setDate(const QDateTime &d)
{
    d_ptr->_date = d;
    d_ptr->_modified = true;
}

QDateTime QMessage::receivedDate() const
{
    return d_ptr->_receivedDate;
}

void QMessage::setReceivedDate(const QDateTime &d)
{
    d_ptr->_receivedDate = d;
    d_ptr->_modified = true;
}

QMessageAddressList QMessage::to() const
{
    return d_ptr->_to;
}

void QMessage::setTo(const QMessageAddressList& toList)
{
    d_ptr->_to = toList;
    d_ptr->_modified = true;
}

void QMessage::setTo(const QMessageAddress& address)
{
    d_ptr->_to << address;
    d_ptr->_modified = true;
}

QMessageAddressList QMessage::cc() const
{
    return d_ptr->_cc;
}

void QMessage::setCc(const QMessageAddressList& ccList)
{
    d_ptr->_cc = ccList;
    d_ptr->_modified = true;
}

QMessageAddressList QMessage::bcc() const
{
    return d_ptr->_bcc;
}

void QMessage::setBcc(const QMessageAddressList& bccList)
{
    d_ptr->_bcc = bccList;
    d_ptr->_modified = true;
}

QMessage::StatusFlags QMessage::status() const
{
    return d_ptr->_status;
}

void QMessage::setStatus(QMessage::StatusFlags newStatus)
{
    d_ptr->_status = newStatus;
    d_ptr->_modified = true;
}

void QMessage::setStatus(QMessage::Status flag, bool set)
{
    if (set) {
        d_ptr->_status |= flag;
    } else {
        d_ptr->_status &= ~flag;
    }
    d_ptr->_modified = true;
}

QMessage::Priority QMessage::priority() const
{
    return d_ptr->_priority;
}

void QMessage::setPriority(Priority newPriority)
{
    d_ptr->_priority = newPriority;
    d_ptr->_modified = true;
}

int QMessage::size() const
{
    int size = 0;
    if (d_ptr->_size != 0) {
        size = d_ptr->_size;
    } else {
        QMessageContentContainerPrivate *container(((QMessageContentContainer *)(this))->d_ptr);
        if (container->_size != 0) {
            size += container->_size;
        }
        foreach (const QMessageContentContainer &attachment, container->_attachments) {
            size += attachment.size();
        }
    }
    return size;
}

QMessageContentContainerId QMessage::bodyId() const
{
    // TODO: Example body finding algorithm.
    // If the content type of the message is text, then that is the body
    // otherwise if the first part of the body is text then that is the body.

    return d_ptr->_bodyId;
}

void QMessage::setBody(const QString &body, const QByteArray &mimeType)
{
    QByteArray mainType("text");
    QByteArray subType("plain");
    QByteArray charset;

    QString mime = QString(mimeType);

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
        charset = QMessage::preferredCharsetFor(body);
        if (charset.isEmpty()) {
            charset = "UTF-8";
        }
    }

    QMessageContentContainerPrivate *container(((QMessageContentContainer *)(this))->d_ptr);

    QMessageContentContainerId existingBodyId(bodyId());
    if (existingBodyId.isValid()) {
        if (existingBodyId == QMessageContentContainerPrivate::bodyContentId()) {
            // The body content is in the message itself
            container->setContent(body, mainType, subType, charset);
        } else {
            // The body content is in the first attachment
            QMessageContentContainerPrivate *attachmentContainer(container->attachment(existingBodyId)->d_ptr);
            attachmentContainer->setContent(body, mainType, subType, charset);
        }
    } else {
        if (container->_attachments.isEmpty()) {
            // Put the content directly into the message
            container->setContent(body, mainType, subType, charset);
            d_ptr->_bodyId = QMessageContentContainerPrivate::bodyContentId();
        } else {
            // Add the body as the first attachment
            QMessageContentContainer newBody;
            newBody.d_ptr->setContent(body, mainType, subType, charset);
            d_ptr->_bodyId = container->prependContent(newBody);
        }
    }

    d_ptr->_modified = true;
}

void QMessage::setBody(QTextStream &in, const QByteArray &mimeType)
{
    setBody(in.readAll(), mimeType);
    d_ptr->_modified = true;
}

QMessageContentContainerIdList QMessage::attachmentIds() const
{
    QMessageContentContainerIdList ids;

    QMessageContentContainerId msgBodyId(bodyId());
    foreach (const QMessageContentContainerId &contentId, contentIds()) {
        if (contentId != msgBodyId) {
            ids.append(contentId);
        }
    }

    return ids;
}

void QMessage::appendAttachments(const QStringList &fileNames)
{
    if (!fileNames.isEmpty()) {
        d_ptr->_modified = true;

        QMessageContentContainerPrivate *container(((QMessageContentContainer *)(this))->d_ptr);

        if (container->_attachments.isEmpty()) {
            QMessageContentContainerId existingBodyId(bodyId());
            if (existingBodyId == QMessageContentContainerPrivate::bodyContentId()) {
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
    d_ptr->_modified = true;
}

void QMessage::clearAttachments()
{
    d_ptr->_modified = true;
    QMessageContentContainerPrivate *container(((QMessageContentContainer *)(this))->d_ptr);
    container->_attachments.clear();
}

bool QMessage::isModified() const
{
    return d_ptr->_modified;
}

QMessage QMessage::createResponseMessage(ResponseType type) const
{
    QMessage message;
    message.setType(d_ptr->_type);
    message.setParentAccountId(d_ptr->_parentAccountId);
    message.setDate(QDateTime::currentDateTime());

    QString body;
    QMessageContentContainerPrivate *container(((QMessageContentContainer *)(this))->d_ptr);
    QMessageContentContainerId existingBodyId(bodyId());
    if (existingBodyId.isValid()) {
        if (existingBodyId == QMessageContentContainerPrivate::bodyContentId()) {
            // The body content is in the message itself
            body = textContent();
            message.setBody(body);
        } else {
            // The body content is in the first attachment
            QMessageContentContainerPrivate *attachmentContainer(container->attachment(existingBodyId)->d_ptr);
            body = attachmentContainer->_textContent;
            message.setBody(body);
        }
    }

    if (type == ReplyToSender) {
        message.setTo(d_ptr->_from);

        QString subj = subject();
        if (!subj.isEmpty()) {
            subj.insert(0, "Re:");
            message.setSubject(subj);
        }
    } else if (type == ReplyToAll) {
        QList<QMessageAddress> addressList;
        addressList.append(d_ptr->_from);
        message.setTo(addressList);

        QList<QMessageAddress> ccAddressList;
        QMessageAddressList toList = to();
        foreach(QMessageAddress address, toList) {
            ccAddressList.append(address);
        }
        QMessageAddressList ccList = cc();
        foreach(QMessageAddress ccAddress, ccList) {
            ccAddressList.append(ccAddress);
        }
        message.setCc(ccAddressList);

        QList<QMessageAddress> bccAddressList;
        QMessageAddressList bccList = bcc();
        foreach(QMessageAddress bccAddress, bccList) {
            bccAddressList.append(bccAddress);
        }
        message.setBcc(bccAddressList);

        QString subj = subject();
        if (!subj.isEmpty()) {
            subj.insert(0, "Re:");
            message.setSubject(subj);
        }
    } else if (type == Forward) {
        QString subj = subject();
        if (!subj.isEmpty()) {
            subj.insert(0, "Fwd:");
            message.setSubject(subj);
        }

        QMessageContentContainerIdList ids = attachmentIds();
        QStringList containerList;
        foreach (QMessageContentContainerId id, ids) {
            QMessageContentContainer container = find(id);
            QByteArray filePath = QMessageContentContainerPrivate::attachmentFilename(container);
            containerList.append(QString(filePath));
        }
        message.appendAttachments(containerList);
    }

    return message;
}


QTM_END_NAMESPACE
