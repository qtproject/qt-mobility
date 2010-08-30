/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessage.h"
#include "qmessage_p.h"
#include "qmessageid_p.h"
#include "qmessageaccountid_p.h"
#include "qmessagemanager.h"
#include "qmessagecontentcontainer_p.h"
#include "qmessagefolderid_p.h"
#include "winhelpers_p.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>

QTM_BEGIN_NAMESPACE

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

void QMessagePrivate::ensurePropertiesPresent(QMessage *msg) const
{
    if (!_elementsPresent.properties && _id.isValid()) {
        QMessageManager::Error ignoredError(QMessageManager::NoError);
        MapiSessionPtr session(MapiSession::createSession(&ignoredError));
        if (!session.isNull()) {
            session->updateMessageProperties(&ignoredError, msg);
        }
    }
}

void QMessagePrivate::ensureRecipientsPresent(QMessage *msg) const
{
    if (!_elementsPresent.recipients && _id.isValid()) {
        QMessageManager::Error ignoredError(QMessageManager::NoError);
        MapiSessionPtr session(MapiSession::createSession(&ignoredError));
        if (!session.isNull()) {
            session->updateMessageRecipients(&ignoredError, msg);
        }
    }
}

void QMessagePrivate::ensureBodyPresent(QMessage *msg) const
{
    if (!_elementsPresent.body && _id.isValid()) {
        QMessageManager::Error ignoredError(QMessageManager::NoError);
        MapiSessionPtr session(MapiSession::createSession(&ignoredError));
        if (!session.isNull()) {
            session->updateMessageBody(&ignoredError, msg);
        }
    }
}

void QMessagePrivate::ensureAttachmentsPresent(QMessage *msg) const
{
    if (!_elementsPresent.attachments && _id.isValid()) {
        QMessageManager::Error ignoredError(QMessageManager::NoError);
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
    *this = QMessageManager().message(id);
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

int QMessage::size() const
{
    int size = 0;

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
    QMessage response;
    response.setType(this->type());

    if (type == Forward) {
        response.setSubject("Fwd:" + subject());

        if (contentType().toLower() == "text") {
            // Forward the text content inline
            QStringList addresses;
            foreach (const QMessageAddress &address, to()) {
                addresses.append(address.addressee());
            }

            QString existingText(textContent());

            QString prefix(QString("\r\n----- %1 -----\r\n\r\n").arg(qApp->translate("QMessage", "Forwarded Message")));
            prefix.append(QString("%1: %2\r\n").arg(qApp->translate("QMessage", "Subject")).arg(subject()));
            prefix.append(QString("%1: %2\r\n").arg(qApp->translate("QMessage", "Date")).arg(date().toString()));
            prefix.append(QString("%1: %2\r\n").arg(qApp->translate("QMessage", "From")).arg(from().addressee()));
            prefix.append(QString("%1: %2\r\n").arg(qApp->translate("QMessage", "To")).arg(addresses.join(",")));

            QString postfix("\r\n\r\n-----------------------------\r\n");

            response.setBody(prefix + existingText + postfix);
        } else {
            // Add an empty text body to be composed into
            response.setBody(QString(), "text/plain");

            // We can only forward the original content if it is a single part
            if (contentType().toLower() != "multipart") {
                QByteArray fileName(suggestedFileName());
                if (!fileName.isEmpty()) {
                    // Write the content to a file that we can attach to our response
                    QString path(QDir::tempPath() + "/qtmobility/messaging/" + fileName);
                    if (QFile::exists(path)) {
                        if (!QFile::remove(path)) {
                            qWarning() << "Unable to remove temporary file:" << path;
                        }
                    }

                    QFile out(path);
                    if (!out.open(QFile::WriteOnly)) {
                        qWarning() << "Unable to open temporary file:" << path;
                    } else {
                        out.write(content());
                        out.flush();
                        out.close();
                    }

                    response.appendAttachments(QStringList() << path);
                }
            }
        }
    } else {
        // Prefer to reply to the trply-to address, if present
        QString replyTo(headerFieldValue("Reply-To"));
        if (!replyTo.isEmpty()) {
            response.setTo(QMessageAddressList() << QMessageAddress(QMessageAddress::Email, replyTo));
        } else {
            response.setTo(QMessageAddressList() << from());
        }

        if (type == ReplyToAll) {
            response.setCc(to() + cc());
        }

        response.setSubject("Re:" + subject());

        // Put the existing text into the reply body
        QString existingText;
        QMessageContentContainerIdList attachments;
        if (contentType().toLower() == "text") {
            existingText = textContent();
        } else {
            QMessageContentContainerId textId(bodyId());
            if (textId.isValid()) {
                existingText = find(textId).textContent();
            }

            attachments = attachmentIds();
        }

        if (!existingText.isEmpty()) {
            existingText = existingText.replace("\n", "\n> ");

            QString prefix(qApp->translate("QMessage", "On %1 you wrote:\n> "));
            response.setBody(prefix.arg(date().toLocalTime().toString()) + existingText);
        }

        // Add any attachment parts as attachments
        QStringList attachmentPaths;
        foreach (const QMessageContentContainerId &attachmentId, attachments) {
            QMessageContentContainer attachment(find(attachmentId));

            QByteArray fileName(attachment.suggestedFileName());
            if (!fileName.isEmpty()) {
                QString path(QDir::tempPath() + "/qtmobility/messaging/" + fileName);
                if (QFile::exists(path)) {
                    if (!QFile::remove(path)) {
                        qWarning() << "Unable to remove temporary file:" << path;
                    }
                }

                QString partialPath(QDir::tempPath() + "/qtmobility");
                if (!QFile::exists(partialPath)) {
                    if (!QDir::temp().mkdir("qtmobility")) {
                        qWarning() << "Unable to create temporary directory:" << partialPath;
                    }
                }
                partialPath.append("/messaging");
                if (!QFile::exists(partialPath)) {
                    if (!QDir(QDir::tempPath() + "/qtmobility").mkdir("messaging")) {
                        qWarning() << "Unable to create temporary directory:" << partialPath;
                    }
                }

                QFile out(path);
                if (!out.open(QFile::WriteOnly)) {
                    qWarning() << "Unable to open temporary file:" << path;
                } else {
                    out.write(attachment.content());
                    out.flush();
                    out.close();

                    attachmentPaths.append(path);
                }
            }
        }

        response.appendAttachments(attachmentPaths);
    }

    return response;
}

QTM_END_NAMESPACE
