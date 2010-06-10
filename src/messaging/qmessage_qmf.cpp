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
#include "qmessagecontentcontainer_p.h"
#include "qmfhelpers_p.h"
#include "qmessage_p.h"

#include <qmailnamespace.h>

#include <QCoreApplication>
#include <QFileInfo>
#include <QRegExp>

using namespace QTM_PREPEND_NAMESPACE(QmfHelpers);

// This class has friend access to QMailMessage
// and hsould not be within QTM_NAMESPACE
struct QMailStorePrivate
{
    static inline void setUnmodified(QMailMessage *msg)
    {
        msg->setUnmodified();
    }
};



QTM_BEGIN_NAMESPACE

Q_SCOPED_STATIC_DEFINE(QMessagePrivate::StandardFolderMap,QMessagePrivate,standardFolderMap);

QMessageFolderId QMessagePrivate::standardFolderId(QMessage::StandardFolder folder)
{
    StandardFolderMap::const_iterator it = standardFolderMap()->find(folder);
    if (it == standardFolderMap()->end()) {
        const char *path((folder == QMessage::InboxFolder ? "Inbox" : 
                         (folder == QMessage::OutboxFolder ? "Outbox" : 
                         (folder == QMessage::DraftsFolder ? "Drafts" : 
                         (folder == QMessage::SentFolder ? "Sent" : 
                         (folder == QMessage::TrashFolder ? "Trash" : ""))))));

        // Find the ID for this standard folder
        QMessageFolderFilter pathFilter(QMessageFolderFilter::byPath(path));
        QMessageFolderFilter accountFilter(QMessageFolderFilter::byParentAccountId(QMessageAccountId()));

        QMessageFolderId folderId(QMessageManager().queryFolders(pathFilter & accountFilter).first());
        it = standardFolderMap()->insert(folder, folderId);
    }

    return *it;
}

QMessage::StandardFolder QMessagePrivate::standardFolder(QMessageFolderId folderId)
{
    StandardFolderMap::const_iterator it = standardFolderMap()->begin(), end = standardFolderMap()->end();
    for ( ; it != end; ++it) {
        if (it.value() == folderId) {
            return it.key();
        }
    }

    return QMessage::DraftsFolder;
}

QMessage QMessagePrivate::convert(const QMailMessage &message)
{
    QMessage result;
    result.d_ptr->_message = message;
    return result;
}

QMailMessage QMessagePrivate::convert(const QMessage &message)
{
    return message.d_ptr->_message;
}

QMailMessage *QMessagePrivate::convert(QMessage *message)
{
    return &message->d_ptr->_message;
}

/*
const QMailMessage *QMessagePrivate::convert(const QMessage *message)
{
    return &message->d_ptr->_message;
}
*/

void QMessagePrivate::setStandardFolder(QMessage& message, QMessage::StandardFolder sf)
{
    QMessageFolderId folderId(standardFolderId(sf));
    message.d_ptr->_message.setParentFolderId(QmfHelpers::convert(folderId));
}

namespace QmfHelpers {

QMessage convert(const QMailMessage &message)
{
    return QMessagePrivate::convert(message);
}

QMailMessage convert(const QMessage &message)
{
    return QMessagePrivate::convert(message);
}

QMailMessage *convert(QMessage *message)
{
    return QMessagePrivate::convert(message);
}

/*
const QMailMessage *convert(const QMessage *message)
{
    return QMessagePrivate::convert(message);
}
*/

}

namespace {

struct TextPartLocator
{
    QMailMessagePart::Location _location;

    bool operator()(const QMailMessagePart &part)
    {
        if (part.contentType().type().toLower() == "text") {
            _location = part.location();
            return false;
        }

        return true;
    }
};

struct PartLocator
{
    QMailMessagePart::Location _location;
    const QMailMessagePart *_part;

    PartLocator(const QMailMessagePart::Location &location) : _location(location), _part(0) {}

    bool operator()(const QMailMessagePart &part)
    {
        if (part.location() == _location) {
            _part = &part;
            return false;
        }

        return true;
    }
};

struct AttachmentLocator
{
    QList<QMailMessagePart::Location> _locations;
    bool _foundText;

    AttachmentLocator() : _foundText(false) {}

    bool operator()(const QMailMessagePart &part)
    {
        if (!_foundText && part.contentType().type().toLower() == "text") {
            _foundText = true;
        } else if (part.multipartType() == QMailMessagePart::MultipartNone) {
            _locations.append(part.location());
        }

        return true;
    }
};

QByteArray charsetFor(const QString &input)
{
    QByteArray result(QMessage::preferredCharsetFor(input));
    if (result.isEmpty()) {
        result = "UTF-8";
    }

    return result;
}

}

QMessage::QMessage()
    : d_ptr(new QMessagePrivate)
{
    setDerivedMessage(this);
    QMailStorePrivate::setUnmodified(&d_ptr->_message);
}

QMessage::QMessage(const QMessageId& id)
    : d_ptr(new QMessagePrivate)
{
    *this = QMessageManager().message(id);
    setDerivedMessage(this);
    QMailStorePrivate::setUnmodified(&d_ptr->_message);
}

QMessage::QMessage(const QMessage &other)
    : QMessageContentContainer(),
      d_ptr(new QMessagePrivate)
{
    this->operator=(other);
    setDerivedMessage(this);
    QMailStorePrivate::setUnmodified(&d_ptr->_message);
}

QMessage& QMessage::operator=(const QMessage& other)
{
    if (&other != this) {
        QMessageContentContainer::operator=(other);
        d_ptr->_message = other.d_ptr->_message;
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
    return convert(d_ptr->_message.id());
}

QMessage::Type QMessage::type() const
{
    return convert(d_ptr->_message.messageType());
}

void QMessage::setType(Type t)
{
    d_ptr->_message.setMessageType(convert(t));
}

QMessageAccountId QMessage::parentAccountId() const
{
    return convert(d_ptr->_message.parentAccountId());
}

void QMessage::setParentAccountId(const QMessageAccountId &accountId)
{
    d_ptr->_message.setParentAccountId(convert(accountId));    
}

QMessageFolderId QMessage::parentFolderId() const
{
    return convert(d_ptr->_message.parentFolderId());
}

QMessage::StandardFolder QMessage::standardFolder() const
{
    return QMessagePrivate::standardFolder(convert(d_ptr->_message.parentFolderId()));
}

QMessageAddress QMessage::from() const
{
    return convert(d_ptr->_message.from());
}

void QMessage::setFrom(const QMessageAddress &address)
{
    d_ptr->_message.setFrom(convert(address));
}

QString QMessage::subject() const
{
    return d_ptr->_message.subject();
}

void QMessage::setSubject(const QString &s)
{
    d_ptr->_message.setSubject(s);
}

QDateTime QMessage::date() const
{
    return d_ptr->_message.date().toLocalTime();
}

void QMessage::setDate(const QDateTime &d)
{
    d_ptr->_message.setDate(QMailTimeStamp(d));
}

QDateTime QMessage::receivedDate() const
{
    return d_ptr->_message.receivedDate().toLocalTime();
}

void QMessage::setReceivedDate(const QDateTime &d)
{
    d_ptr->_message.setReceivedDate(QMailTimeStamp(d));
}

QMessageAddressList QMessage::to() const
{
    return convert(d_ptr->_message.to());
}

void QMessage::setTo(const QMessageAddressList& toList)
{
    d_ptr->_message.setTo(convert(toList));
}

void QMessage::setTo(const QMessageAddress& address)
{
    d_ptr->_message.setTo(convert(address));
}

QMessageAddressList QMessage::cc() const
{
    return convert(d_ptr->_message.cc());
}

void QMessage::setCc(const QMessageAddressList& ccList)
{
    d_ptr->_message.setCc(convert(ccList));
}

QMessageAddressList QMessage::bcc() const
{
    return convert(d_ptr->_message.bcc());
}

void QMessage::setBcc(const QMessageAddressList& bccList)
{
    d_ptr->_message.setBcc(convert(bccList));
}

QMessage::StatusFlags QMessage::status() const
{
    return convert(d_ptr->_message.status());
}

void QMessage::setStatus(QMessage::StatusFlags newStatus)
{
    d_ptr->_message.setStatus(convert(newStatus));
}

void QMessage::setStatus(QMessage::Status flag, bool set)
{
    d_ptr->_message.setStatus(convert(flag), set);
}

QMessage::Priority QMessage::priority() const
{
    quint64 status(d_ptr->_message.status());

    if (status & highPriorityMask()) {
        return QMessage::HighPriority;
    } else if (status & lowPriorityMask()) {
        return QMessage::LowPriority;
    }

    return QMessage::NormalPriority;
}

void QMessage::setPriority(Priority newPriority)
{
    quint64 setMask(0);
    quint64 unsetMask(0);

    if (newPriority == QMessage::HighPriority) {
        setMask = highPriorityMask();
        unsetMask = lowPriorityMask();
    } else if (newPriority == QMessage::LowPriority) {
        unsetMask = highPriorityMask();
        setMask = lowPriorityMask();
    } else {
        unsetMask = (highPriorityMask() | lowPriorityMask());
    }

    if (setMask) {
        d_ptr->_message.setStatus(setMask, true);
    }
    if (unsetMask) {
        d_ptr->_message.setStatus(unsetMask, false);
    }
}

int QMessage::size() const
{
    if ((d_ptr->_message.status() & QMailMessage::LocalOnly) && 
        (d_ptr->_message.dataModified() ||
         d_ptr->_message.contentModified())) {
        // We need to update the size estimate for this message
        d_ptr->_message.setSize(static_cast<int>(d_ptr->_message.indicativeSize()) * 1024);
    }

    return static_cast<int>(d_ptr->_message.size());
}

QMessageContentContainerId QMessage::bodyId() const
{
    if (d_ptr->_message.hasBody()) {
        return QmfHelpers::bodyId(d_ptr->_message.id());
    }

    // Return the first text part
    TextPartLocator locator;
    d_ptr->_message.foreachPart<TextPartLocator&>(locator);

    return convert(locator._location);
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

    QMailMessageContentType ct;
    ct.setType(mainType);
    ct.setSubType(subType);
    ct.setCharset(charset);

    QMailMessageBody textBody(QMailMessageBody::fromData(bodyText, ct, QMailMessageBody::Base64));

    if (d_ptr->_message.multipartType() == QMailMessage::MultipartNone) {
        // Replace the body with this data
        d_ptr->_message.setBody(textBody);
    } else {
        // Replace any existing text with this part
        TextPartLocator locator;
        d_ptr->_message.foreachPart<TextPartLocator&>(locator);
        if (locator._location.isValid()) {
            // Update the existing body text part to contain the new text
            QMailMessagePart &bodyPart = d_ptr->_message.partAt(locator._location);
            bodyPart.setBody(textBody);
        } else {
            // Insert the text as the new first part
            QMailMessageContentDisposition cd(QMailMessageContentDisposition::Inline);
            QMailMessagePart part(QMailMessagePart::fromData(bodyText, ct, cd, QMailMessageBody::Base64));
            d_ptr->_message.prependPart(part);
        }
    }
}

void QMessage::setBody(QTextStream &in, const QByteArray &mimeType)
{
    // Note we have to read the data from the stream, in order to determine the relevant charset
    setBody(in.readAll(), mimeType);
}

QMessageContentContainerIdList QMessage::attachmentIds() const
{
    // Return any non-multipart parts excluding the first text part
    AttachmentLocator locator;
    d_ptr->_message.foreachPart<AttachmentLocator&>(locator);

    return convert(locator._locations);
}

void QMessage::appendAttachments(const QStringList &fileNames)
{
    if (d_ptr->_message.multipartType() == QMailMessage::MultipartNone) {
        if (d_ptr->_message.hasBody()) {
            // Move the existing body to become the first part
            QMailMessageContentType ct(d_ptr->_message.contentType());
            QString bodyText(d_ptr->_message.body().data());

            QMailMessageContentDisposition cd(QMailMessageContentDisposition::Inline);
            cd.setSize(bodyText.length());

            QMailMessagePart textPart(QMailMessagePart::fromData(bodyText, cd, ct, QMailMessageBody::Base64));

            d_ptr->_message.setMultipartType(QMailMessage::MultipartMixed);
            d_ptr->_message.appendPart(textPart);
        } else {
            // Just set this message to be multipart
            d_ptr->_message.setMultipartType(QMailMessage::MultipartMixed);
        }
    }

    foreach (const QString &filename, fileNames) {
        QString mimeType(QMail::mimeTypeFromFileName(filename));
        if (!mimeType.isEmpty()) {
            QFileInfo fi(filename);

            QMailMessageContentDisposition cd(QMailMessageContentDisposition::Attachment);
            cd.setFilename(fi.fileName().toAscii());
            cd.setSize(fi.size());

            QMailMessageContentType ct(mimeType.toAscii());

            QMailMessagePart part(QMailMessagePart::fromFile(filename, cd, ct, QMailMessageBody::Base64, QMailMessageBody::RequiresEncoding));
            d_ptr->_message.appendPart(part);
        }
    }
}

void QMessage::clearAttachments()
{
    QMessageContentContainerId textId(bodyId());
    QMailMessagePart::Location textLocation(convert(textId));

    for (uint i = d_ptr->_message.partCount(); i > 0; --i) {
        QMailMessagePart &part = d_ptr->_message.partAt(i - 1);
        if (!(part.location() == textLocation)) {
            // Ensure that this part does not contain the text part
            PartLocator locator(textLocation);
            part.foreachPart<PartLocator&>(locator);
            if (locator._part == 0) {
                d_ptr->_message.removePartAt(i - 1);
            }
        }
    }
}

bool QMessage::isModified() const
{
    return (d_ptr->_message.dataModified() || d_ptr->_message.contentModified());
}

QMessage QMessage::createResponseMessage(ResponseType type) const
{
    QMessage response;

    if (type == Forward) {
        QString subject(d_ptr->_message.subject());
        response.setSubject("Fwd:" + subject);

        if (d_ptr->_message.contentType().type().toLower() == "text") {
            // Forward the text content inline
            QString existingText = d_ptr->_message.body().data();

            QString prefix("\r\n----- Forwarded Message -----\r\n\r\n");
            prefix.append(QString("%1: %2\r\n").arg(qApp->translate("QMessage", "Subject")).arg(subject));
            prefix.append(QString("%1: %2\r\n").arg(qApp->translate("QMessage", "Date")).arg(date().toString()));
            prefix.append(QString("%1: %2\r\n").arg(qApp->translate("QMessage", "From")).arg(d_ptr->_message.from().toString()));

            QStringList addresses;
            foreach (const QMailAddress &address, d_ptr->_message.to()) {
                addresses.append(address.toString());
            }

            prefix.append(QString("%1: %2\r\n").arg(qApp->translate("QMessage", "To")).arg(addresses.join(",")));

            QString postfix("\r\n\r\n-----------------------------\r\n");

            response.setBody(prefix + existingText + postfix);
        } else {
            if (QMailMessage *msg = convert(&response)) {
                msg->setMultipartType(QMailMessage::MultipartMixed);

                QMailMessageContentDisposition cd(QMailMessageContentDisposition::Inline);

                {
                    // Add an empty text body to be composed into
                    QMailMessageContentType ct("text/plain");
                    QMailMessagePart part(QMailMessagePart::fromData(QString(), cd, ct, QMailMessageBody::Base64));
                    msg->appendPart(part);
                }

                {
                    // Include the entirety of this message as a nested part
                    QMailMessageContentType ct("message/rfc822");
                    QByteArray responseData(d_ptr->_message.toRfc2822(QMailMessage::TransmissionFormat));
                    QMailMessagePart part(QMailMessagePart::fromData(responseData, cd, ct, QMailMessageBody::Base64));
                    msg->appendPart(part);
                }
            }
        }
    } else {
        if (QMailMessage *msg = convert(&response)) {
            QMailAddress to(d_ptr->_message.replyTo());
            if (to.address().isEmpty()) {
                to = d_ptr->_message.from();
            }
            response.setTo(convert(to));

            if (type == ReplyToAll) {
                QList<QMessageAddress> cc;
                foreach (const QMailAddress &address, d_ptr->_message.to() + d_ptr->_message.cc()) {
                    cc.append(convert(address));
                }

                response.setCc(cc);
            }

            QString subject(d_ptr->_message.subject());
            response.setSubject("Re:" + subject);

            QString messageId(d_ptr->_message.headerFieldText("Message-Id"));
            QString references(d_ptr->_message.headerFieldText("References"));

            if (!messageId.isEmpty()) {
                if (!references.isEmpty()) {
                    references.append(' ');
                }
                references.append(messageId);

                msg->setHeaderField("In-Reply-To", messageId);
            }
            if (!references.isEmpty()) {
                msg->setHeaderField("References", references);
            }

            QString existingText;
            QMessageContentContainerIdList attachments;

            if (d_ptr->_message.contentType().type().toLower() == "text") {
                existingText = d_ptr->_message.body().data();
            } else {
                // Is there any text in this message?
                QMessageContentContainerId textId(bodyId());
                if (textId.isValid()) {
                    QMessageContentContainer textPart(find(textId));
                    existingText = textPart.textContent();
                }

                // Find the remaining parts of the message
                attachments = attachmentIds();
            }

            if (!existingText.isEmpty()) {
                existingText = existingText.replace("\n", "\n> ");

                QString prefix(qApp->translate("QMessage", "On %1 you wrote:\n> "));
                prefix = prefix.arg(d_ptr->_message.date().toLocalTime().toString());
                response.setBody(prefix + existingText);
            }

            foreach (const QMessageContentContainerId &attachmentId, attachments) {
                QMessageContentContainer attachment(find(attachmentId));
                msg->appendPart(attachment.d_ptr->_part);
            }
        }
    }

    return response;
}


QTM_END_NAMESPACE
