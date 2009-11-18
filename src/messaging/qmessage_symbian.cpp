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
#include "qmessage_symbian_p.h"
#include "qmessagecontentcontainer_symbian_p.h"

QMessagePrivate::QMessagePrivate(QMessage *message)
 : q_ptr(message),
   _size(0),
   _standardFolder(QMessage::DraftsFolder),
   _type(QMessage::NoType),
   _status(0),
   _priority(QMessage::NormalPriority),
   _modified(false)
{
}

QMessagePrivate::~QMessagePrivate()
{
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
	d_ptr->_modified = false;
}

QMessage::QMessage(const QMessageId& id)
 : d_ptr(new QMessagePrivate(this))
{
	d_ptr->_id = id;
}

QMessage::QMessage(const QMessage &other)
 :QMessageContentContainer(other),
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
}

QMessageAccountId QMessage::parentAccountId() const
{
    return d_ptr->_parentAccountId;
}

void QMessage::setParentAccountId(const QMessageAccountId &accountId)
{
    d_ptr->_parentAccountId = accountId;
}

QMessageFolderId QMessage::parentFolderId() const
{
    return QMessageFolderId(); // stub
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
    return d_ptr->_toList;
}

void QMessage::setTo(const QMessageAddressList& toList)
{
	d_ptr->_modified = true;
    d_ptr->_toList = toList;
}

void QMessage::setTo(const QMessageAddress& address)
{
	d_ptr->_modified = true;
    d_ptr->_toList << address;
}

QMessageAddressList QMessage::cc() const
{
	return d_ptr->_ccList;
}

void QMessage::setCc(const QMessageAddressList& ccList)
{
	d_ptr->_modified = true;
	d_ptr->_ccList = ccList;
}

QMessageAddressList QMessage::bcc() const
{
    return d_ptr->_bccList;
}

void QMessage::setBcc(const QMessageAddressList& bccList)
{
	d_ptr->_modified = true;
	d_ptr->_bccList = bccList;
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

void QMessage::setStatus(QMessage::Status flag, bool set)
{
    Q_UNUSED(flag)
    Q_UNUSED(set)
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
    return 0; // stub
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
    QByteArray charset("UTF-8");
    
    int index = mimeType.indexOf("/");
    if (index != -1) {
        mainType = mimeType.left(index).trimmed();
        subType = mimeType.mid(index + 1).trimmed();
    }
    
    QMessageContentContainerPrivate *container(((QMessageContentContainer *)(this))->d_ptr);
    container->setContent(body, mainType, subType, charset);
    d_ptr->_bodyId = QMessageContentContainerPrivate::bodyContentId();
    d_ptr->_modified = true;
}

void QMessage::setBody(QTextStream &in, const QByteArray &mimeType)
{
    setBody(in.readAll(), mimeType);
}

QMessageContentContainerIdList QMessage::attachmentIds() const
{
	return contentIds();
}

void QMessage::appendAttachments(const QStringList &fileNames)
{
	if (!fileNames.isEmpty()) {
		d_ptr->_modified = true;
        QMessageContentContainerPrivate *container(((QMessageContentContainer *)(this))->d_ptr);
        foreach (const QString &filename, fileNames) {
            QMessageContentContainer attachment;
            if (attachment.d_ptr->createAttachment(filename)) {
                container->appendContent(attachment);
            }
        }
    }
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
    return QMessage(); // stub
}

