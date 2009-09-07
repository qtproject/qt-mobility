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
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessage.h"
#include "qmfhelpers_p.h"

#include <qmailnamespace.h>

#include <QCoreApplication>
#include <QFileInfo>

using namespace QmfHelpers;

class QMessagePrivate
{
public:
    QMailMessage _message;

    typedef QMap<QMessage::StandardFolder, QMessageFolderId> StandardFolderMap;
    Q_SCOPED_STATIC_DECLARE(StandardFolderMap,standardFolderMap);

    static QMessageFolderId standardFolderId(QMessage::StandardFolder folder);
    static QMessage::StandardFolder standardFolder(QMessageFolderId folderId);

    static QMessage convert(const QMailMessage &message);
    static QMailMessage convert(const QMessage &message);
    static QMailMessage *convert(QMessage *message);
    static const QMailMessage *convert(const QMessage *message);
};

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

        QMessageFolderId folderId(QMessageStore::instance()->queryFolders(pathFilter & accountFilter).first());
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

    return QMessage::InboxFolder;
}

QMessage QMessagePrivate::convert(const QMailMessage &message)
{
    QMessage result;
    result.d_ptr->_message = message;
    return result;
}

QMailMessage QMessagePrivate::convert(const QMessage &message)
{
    message.applyPendingChanges();
    return message.d_ptr->_message;
}

QMailMessage *QMessagePrivate::convert(QMessage *message)
{
    message->applyPendingChanges();
    return &message->d_ptr->_message;
}

const QMailMessage *QMessagePrivate::convert(const QMessage *message)
{
    message->applyPendingChanges();
    return &message->d_ptr->_message;
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

const QMailMessage *convert(const QMessage *message)
{
    return QMessagePrivate::convert(message);
}

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
        } else if (part.multipartType() != QMailMessagePart::MultipartNone) {
            _locations.append(part.location());
        }

        return true;
    }
};

}

QMessage::QMessage()
    : d_ptr(new QMessagePrivate)
{
    setDerivedMessage(this);
}

QMessage::QMessage(const QMessageId& id)
    : d_ptr(new QMessagePrivate)
{
    *this = QMessageStore::instance()->message(id);
    setDerivedMessage(this);
}

QMessage::QMessage(const QMessage &other)
    : QMessageContentContainer(),
      d_ptr(new QMessagePrivate)
{
    this->operator=(other);
    setDerivedMessage(this);
}

const QMessage& QMessage::operator=(const QMessage& other)
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

QMessage QMessage::fromTransmissionFormat(Type t, const QByteArray &ba)
{
    QMessage result;

    QMailMessage msg = QMailMessage::fromRfc2822(ba);
    msg.setMessageType(convert(t));

    result.d_ptr->_message = msg;
    return result;
}

QMessage QMessage::fromTransmissionFormatFile(Type t, const QString& fileName)
{
    QMessage result;

    QMailMessage msg = QMailMessage::fromRfc2822File(fileName);
    msg.setMessageType(convert(t));

    result.d_ptr->_message = msg;
    return result;
}

QByteArray QMessage::toTransmissionFormat() const
{
    applyPendingChanges();
    return d_ptr->_message.toRfc2822(QMailMessage::TransmissionFormat);
}

void QMessage::toTransmissionFormat(QDataStream& out) const
{
    applyPendingChanges();
    d_ptr->_message.toRfc2822(out, QMailMessage::TransmissionFormat);
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

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderId QMessage::parentFolderId() const
{
    return convert(d_ptr->_message.parentFolderId());
}
#endif

QMessage::StandardFolder QMessage::standardFolder() const
{
    return QMessagePrivate::standardFolder(convert(d_ptr->_message.parentFolderId()));
}

void QMessage::setStandardFolder(StandardFolder sf)
{
    QMessageFolderId folderId(QMessagePrivate::standardFolderId(sf));
    d_ptr->_message.setParentFolderId(convert(folderId));
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

uint QMessage::size() const
{
    return 0; // stub
}

QMessageContentContainerId QMessage::body() const
{
    if (d_ptr->_message.hasBody()) {
        return QmfHelpers::bodyId(d_ptr->_message.id());
    }

    // Return the first text part
    TextPartLocator locator;
    d_ptr->_message.foreachPart<TextPartLocator&>(locator);

    return convert(locator._location);
}


void QMessage::setBody(const QString &body)
{
    if (d_ptr->_message.multipartType() != QMailMessage::MultipartNone) {
        d_ptr->_message.clearParts();
    }

    QMailMessageContentType ct("text/plain; charset=UTF-8");
    d_ptr->_message.setBody(QMailMessageBody::fromData(body, ct, QMailMessageBody::Base64));
}

void QMessage::setBodyFromFile(const QString &fileName)
{
    if (d_ptr->_message.multipartType() != QMailMessage::MultipartNone) {
        d_ptr->_message.clearParts();
    }

    QMailMessageContentType ct("text/plain; charset=UTF-8");
    d_ptr->_message.setBody(QMailMessageBody::fromFile(fileName, ct, QMailMessageBody::Base64, QMailMessageBody::RequiresEncoding));
}

QMessageContentContainerIdList QMessage::attachments() const
{
    // Return any non-multipart parts excluding the first text part
    AttachmentLocator locator;
    d_ptr->_message.foreachPart<AttachmentLocator&>(locator);

    return convert(locator._locations);
}

void QMessage::appendAttachments(const QStringList &fileNames)
{
    foreach (const QString &filename, fileNames) {
        QString mimeType(QMail::mimeTypeFromFileName(filename));
        if (!mimeType.isEmpty()) {
            QFileInfo fi(filename);

            QMailMessageContentDisposition cd(QMailMessageContentDisposition::Attachment);
            cd.setFilename(fi.fileName().toAscii());

            QMailMessageContentType ct(mimeType.toAscii());

            QMailMessagePart part(QMailMessagePart::fromFile(filename, cd, ct, QMailMessageBody::Base64, QMailMessageBody::RequiresEncoding));
            d_ptr->_message.appendPart(part);
        }
    }
}

void QMessage::clearAttachments()
{
    QMessageContentContainerId textId(body());

    for (uint i = d_ptr->_message.partCount(); i > 0; --i) {
        QMailMessagePart &part = d_ptr->_message.partAt(i - 1);
        if (!(convert(part.location()) == textId)) {
            // See if this part contains the text part
            PartLocator locator(convert(textId));
            part.foreachPart<PartLocator&>(locator);
            if (locator._part == 0) {
                d_ptr->_message.removePartAt(i - 1);
            }
        }
    }
}

#ifdef QMESSAGING_OPTIONAL
void QMessage::setOriginatorPort(uint port)
{
    d_ptr->_message.setCustomField("QMessage::originatorPort", QString::number(port));
}

uint QMessage::originatorPort()
{
    return d_ptr->_message.customField("QMessage::originatorPort").toUInt();
}

void QMessage::setDestinationPort(uint port)
{
    d_ptr->_message.setCustomField("QMessage::destinationPort", QString::number(port));
}

uint QMessage::destinationPort()
{
    return d_ptr->_message.customField("QMessage::destinationPort").toUInt();
}

QString QMessage::customField(const QString &name) const
{
    return d_ptr->_message.customField(name);
}

void QMessage::setCustomField(const QString &name, const QString &value)
{
    if (value.isNull()) {
        d_ptr->_message.removeCustomField(name);
    } else {
        d_ptr->_message.setCustomField(name, value);
    }
}

QStringList QMessage::customFields() const
{
    QStringList result;

    foreach (const QString &key, d_ptr->_message.customFields().keys()) {
        if (!key.startsWith("QMessage::")) {
            result.append(key);
        }
    }

    return result;
}
#endif

bool QMessage::isModified() const
{
    return d_ptr->_message.dataModified();
}

QMessage QMessage::createResponseMessage(ResponseType type) const
{
    QMessage response;

    if (type == Forward) {
        QString subject(d_ptr->_message.subject());
        response.setSubject("Fwd:" + subject);

        QMessageContentContainerIdList attachmentIds;

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
            response.setContentType("multipart");
            response.setContentSubType("mixed");

            // Add an empty text body to be composed into
            QMessageContentContainer textPart;
            textPart.setContentType("text");
            textPart.setContentSubType("plain");
            textPart.setContent(QString());
            response.appendContent(textPart);

            // Include the entirety of this message as a nested part
            QMessageContentContainer messagePart;
            messagePart.setContentType("message");
            messagePart.setContentSubType("rfc822");
            messagePart.setContent(toTransmissionFormat());
            response.appendContent(messagePart);
        }
    } else {
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

            response.setHeaderField("In-Reply-To", messageId);
        }
        if (!references.isEmpty()) {
            response.setHeaderField("References", references);
        }

        QString existingText;
        QMessageContentContainerIdList attachmentIds;

        if (d_ptr->_message.contentType().type().toLower() == "text") {
            existingText = d_ptr->_message.body().data();
        } else {
            // Is there any text in this message?
            QMessageContentContainerId textId(body());
            if (textId.isValid()) {
                QMessageContentContainer textPart(container(textId));
                existingText = textPart.decodedTextContent();
            }

            // Find the remaining parts of the message
            attachmentIds = attachments();
        }

        if (!existingText.isEmpty()) {
            existingText = existingText.replace("\n", "\n> ");

            QString prefix(qApp->translate("QMessage", "On %1 you wrote:\n"));
            prefix = prefix.arg(d_ptr->_message.date().toLocalTime().toString());
            response.setBody(prefix + existingText);
        }
        foreach (const QMessageContentContainerId &attachmentId, attachmentIds) {
            QMessageContentContainer attachment = container(attachmentId);
            response.appendContent(attachment);
        }
    }

    return response;
}

