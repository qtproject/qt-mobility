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
#include "qmessage_p.h"
#include "qmessageid_p.h"
#include "qmessageaccountid_p.h"
#include "qmessagestore.h"

//TODO: For the set* functions rather than updating shadowed MAPI structures, 
//TODO: consider using an EntryId to update a MAPI object directly.
//TODO: Note, this will require keeping the MAPI message open which may not
//TODO: be appropriate for many messages (as it may exhaust system resources
//TODO: and crash the OS).

QMessage QMessagePrivate::from(const QMessageId &id, const QMessage::StatusFlags &status, const QMessageAddress &from, const QString &subject, const QDateTime &dt)
{
    QMessage result;
    result.d_ptr->_id = id;
    result.d_ptr->_status = status;
    result.d_ptr->_from = from;
    result.d_ptr->_subject = subject;
    result.d_ptr->_receivedDate = dt;
    result.d_ptr->_parentAccountId = QMessageAccountIdPrivate::from(QMessageIdPrivate::storeRecordKey(id));
    return result;
}

QMessage::QMessage()
    :d_ptr(new QMessagePrivate(this))
{
    d_ptr->_modified = false;
    d_ptr->_size = 0;
    setDerivedMessage(this);
}

QMessage::QMessage(const QMessageId& id)
    :d_ptr(new QMessagePrivate(this))
{
    *this = QMessageStore::instance()->message(id);
    setDerivedMessage(this);
}

QMessage::QMessage(const QMessage &other)
    :QMessageContentContainer(other),
     d_ptr(new QMessagePrivate(this))
{
    this->operator=(other);
    setDerivedMessage(this);
}

const QMessage& QMessage::operator=(const QMessage& other)
{
    if (&other != this) {
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

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFolderId QMessage::parentFolderId() const
{
    return d_ptr->_parentFolderId;
}
#endif

QMessage::StandardFolder QMessage::standardFolder() const
{
    return QMessage::InboxFolder; // stub
}

void QMessage::setStandardFolder(StandardFolder sf)
{
    Q_UNUSED(sf) // stub
}

QMessageAddress QMessage::from() const
{
    return d_ptr->_from;
}

void QMessage::setFrom(const QMessageAddress &address)
{
    d_ptr->_modified = true;
    d_ptr->_from = address;
}

QString QMessage::subject() const
{
    return d_ptr->_subject;
}

void QMessage::setSubject(const QString &s)
{
    d_ptr->_modified = true;
    d_ptr->_subject = s;
}

QDateTime QMessage::date() const
{
    return d_ptr->_date;
}

void QMessage::setDate(const QDateTime &d)
{
    d_ptr->_modified = true;
    d_ptr->_date = d;
}

QDateTime QMessage::receivedDate() const
{
    return d_ptr->_receivedDate;
}

void QMessage::setReceivedDate(const QDateTime &d)
{
    d_ptr->_modified = true;
    d_ptr->_receivedDate = d;
}

QMessageAddressList QMessage::to() const
{
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
    return d_ptr->_cc;
}

void QMessage::setCc(const QMessageAddressList& ccList)
{
    d_ptr->_modified = true;
    d_ptr->_cc = ccList;
}

QMessageAddressList QMessage::bcc() const
{
    return d_ptr->_bcc;
}

void QMessage::setBcc(const QMessageAddressList& bccList)
{
    d_ptr->_modified = true;
    d_ptr->_bcc = bccList;
}

QMessage::StatusFlags QMessage::status() const
{
    return d_ptr->_status;
}

void QMessage::setStatus(QMessage::StatusFlags newStatus)
{
    d_ptr->_modified = true;
    d_ptr->_status = newStatus;
}

QMessage::Priority QMessage::priority() const
{
    return d_ptr->_priority;
}

void QMessage::setPriority(Priority newPriority)
{
    d_ptr->_modified = true;
    d_ptr->_priority = newPriority;
}

uint QMessage::size() const
{
    return d_ptr->_size;
}

QMessageContentContainerId QMessage::body() const
{
    return QMessageContentContainerId(QString::number(0));
}

void QMessage::setBody(const QString &body)
{
    setContent(body);
}

void QMessage::setBodyFromFile(const QString &fileName)
{
    setContentFileName(fileName.toAscii());
}

QMessageContentContainerIdList QMessage::attachments() const
{
    return contentIds();
}

void QMessage::appendAttachments(const QStringList &fileNames)
{
    d_ptr->_modified = true;

    foreach (const QString &filename, fileNames) {
        QMessageContentContainer content;
        content.setContentFileName(filename.toAscii());
        appendContent(content);
    }
}

void QMessage::clearAttachments()
{
    d_ptr->_modified = true;

    foreach (const QMessageContentContainerId &id, contentIds()) {
        removeContent(id);
    }
}

#ifdef QMESSAGING_OPTIONAL
void QMessage::setOriginatorPort(uint port)
{
    d_ptr->_modified = true;
    d_ptr->_originatorPort = port;
}

uint QMessage::originatorPort()
{
    return d_ptr->_originatorPort;
}

void QMessage::setDestinationPort(uint port)
{
    d_ptr->_modified = true;
    d_ptr->_destinationPort = port;
}

uint QMessage::destinationPort()
{
    return d_ptr->_destinationPort;
}

QString QMessage::customField(const QString &name) const
{
    return d_ptr->_customFields[name];
}

void QMessage::setCustomField(const QString &name, const QString &value)
{
    d_ptr->_modified = true;
    d_ptr->_customFields[name] = value;
}

QList<QString> QMessage::customFields() const
{
    return d_ptr->_customFields.keys();
}
#endif

bool QMessage::dataModified() const
{
    return d_ptr->_modified;
}

QMessage QMessage::replyTo() const
{
    return QMessage(); // stub
}

QMessage QMessage::replyToAll() const
{
    return QMessage(); // stub
}

QMessage QMessage::forward() const
{
    return QMessage(); // stub
}
