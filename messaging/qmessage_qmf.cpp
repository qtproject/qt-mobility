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
#include "qmessage.h"
#include "qmfhelpers_p.h"

#include <qmailnamespace.h>

using namespace QmfHelpers;

class QMessagePrivate
{
public:
    QMailMessage _message;
};

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
}

QMessage::QMessage(const QMessageId& id)
    : d_ptr(new QMessagePrivate)
{
    *this = QMessageStore::instance()->message(id);
}

QMessage::QMessage(const QMessage &other)
    : QMessageContentContainer(other),
      d_ptr(new QMessagePrivate)
{
    *this = other;
}

const QMessage& QMessage::operator=(const QMessage& other)
{
    if (&other != this) {
        d_ptr->_message = other.d_ptr->_message;
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
    return d_ptr->_message.toRfc2822(QMailMessage::TransmissionFormat);
}

void QMessage::toTransmissionFormat(QDataStream& out) const
{
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

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderId QMessage::parentFolderId() const
{
    return convert(d_ptr->_message.parentFolderId());
}
#endif

QMessage::StandardFolder QMessage::standardFolder() const
{
    // TODO
    return QMessage::InboxFolder;
}

void QMessage::setStandardFolder(StandardFolder sf)
{
    // TODO
    Q_UNUSED(sf)
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

QMessage::Priority QMessage::priority() const
{
    QString priority = d_ptr->_message.customField("QMessage::priority");

    if (priority == "high") {
        return QMessage::High;
    } else if (priority == "low") {
        return QMessage::Low;
    }

    return QMessage::Low;
}

void QMessage::setPriority(Priority newPriority)
{
    if (newPriority == QMessage::High) {
        d_ptr->_message.setCustomField("QMessage::priority", "high");
    } else if (newPriority == QMessage::Low) {
        d_ptr->_message.setCustomField("QMessage::priority", "low");
    } else {
        d_ptr->_message.removeCustomField("QMessage::priority");
    }
}

uint QMessage::size() const
{
    return 0; // stub
}

QMessageContentContainerId QMessage::body() const
{
    if (d_ptr->_message.hasBody()) {
        // Return a location with no indices
        QMailMessagePart::Location location;
        location.setContainingMessageId(d_ptr->_message.id());
        return convert(location);
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
            QMailMessageContentDisposition cd(QMailMessageContentDisposition::Attachment);
            cd.setFilename(filename.toAscii());

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
    d_ptr->_message.setCustomField(name, value);
}

QList<QString> QMessage::customFields() const
{
    QList<QString> result;

    foreach (const QString &key, d_ptr->_message.customFields().keys()) {
        if (key.startsWith("QMessage::")) {
            result.append(key);
        }
    }

    return result;
}
#endif

bool QMessage::dataModified() const
{
    return d_ptr->_message.dataModified();
}

QMessage QMessage::replyTo() const
{
    // TODO
    return QMessage();
}

QMessage QMessage::replyToAll() const
{
    // TODO
    return QMessage();
}

QMessage QMessage::forward() const
{
    // TODO
    return QMessage();
}
