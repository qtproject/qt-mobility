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
#include "qmessage.h"


QTM_BEGIN_NAMESPACE

QMessage::QMessage()
{
}

QMessage::QMessage(const QMessageId& id)
{
    Q_UNUSED(id)
}

QMessage::QMessage(const QMessage &other)
    :QMessageContentContainer(other)
{
    Q_UNUSED(other)
}

QMessage& QMessage::operator=(const QMessage& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

QMessage::~QMessage()
{
}

QMessageId QMessage::id() const
{
    return QMessageId(); // stub
}

QMessage::Type QMessage::type() const
{
    return NoType; // stub
}

void QMessage::setType(Type t)
{
    Q_UNUSED(t)
}

QMessageAccountId QMessage::parentAccountId() const
{
    return QMessageAccountId(); // stub
}

void QMessage::setParentAccountId(const QMessageAccountId &accountId)
{
    Q_UNUSED(accountId)
}

QMessageFolderId QMessage::parentFolderId() const
{
    return QMessageFolderId(); // stub
}

QMessage::StandardFolder QMessage::standardFolder() const
{
    return QMessage::DraftsFolder; // stub
}

QMessageAddress QMessage::from() const
{
    return QMessageAddress(); // stub
}

void QMessage::setFrom(const QMessageAddress &address)
{
    Q_UNUSED(address)
}

QString QMessage::subject() const
{
    return QString::null; //stub
}

void QMessage::setSubject(const QString &s)
{
    Q_UNUSED(s)
}

QDateTime QMessage::date() const
{
    return date(); // stub
}

void QMessage::setDate(const QDateTime &d)
{
    Q_UNUSED(d)
}

QDateTime QMessage::receivedDate() const
{
    return QDateTime(); // stub
}

void QMessage::setReceivedDate(const QDateTime &d)
{
    Q_UNUSED(d)
}

QMessageAddressList QMessage::to() const
{
    return QMessageAddressList(); // stub
}

void QMessage::setTo(const QMessageAddressList& toList)
{
    Q_UNUSED(toList)
}

void QMessage::setTo(const QMessageAddress& address)
{
    Q_UNUSED(address)
}

QMessageAddressList QMessage::cc() const
{
    return QMessageAddressList(); // stub
}

void QMessage::setCc(const QMessageAddressList& ccList)
{
    Q_UNUSED(ccList)
}

QMessageAddressList QMessage::bcc() const
{
    return QMessageAddressList(); // stub
}

void QMessage::setBcc(const QMessageAddressList& bccList)
{
    Q_UNUSED(bccList)
}

QMessage::StatusFlags QMessage::status() const
{
    return StatusFlags(0); // stub
}

void QMessage::setStatus(QMessage::StatusFlags newStatus)
{
    Q_UNUSED(newStatus)
}

void QMessage::setStatus(QMessage::Status flag, bool set)
{
    Q_UNUSED(flag)
    Q_UNUSED(set)
}

QMessage::Priority QMessage::priority() const
{
    return QMessage::NormalPriority; // stub
}

void QMessage::setPriority(Priority newPriority)
{
    Q_UNUSED(newPriority)
}

int QMessage::size() const
{
    return 0; // stub
}

QMessageContentContainerId QMessage::bodyId() const
{
    // TODO: Example body finding algorithm.
    // If the content type of the message is text, then that is the body
    // otherwise if the first part of the body is text then that is the body.
    
    return QMessageContentContainerId(); // stub
}

void QMessage::setBody(const QString &body, const QByteArray &mimeType)
{
    Q_UNUSED(body)
    Q_UNUSED(mimeType)
}

void QMessage::setBody(QTextStream &in, const QByteArray &mimeType)
{
    Q_UNUSED(in)
    Q_UNUSED(mimeType)
}

QMessageContentContainerIdList QMessage::attachmentIds() const
{
    //    TODO: Example attachment list generation algorithm, message parts are the main issue, maybe 
    //    have to recurse into them, somewhat ambiguous.
    //    Don't recurse, just ignore any body part, see bodyId() for body finding algorithm.
    
    // Implementation note, this should be platform independent.
    return QMessageContentContainerIdList(); // stub
}

void QMessage::appendAttachments(const QStringList &fileNames)
{
    // Implementation note, this should be platform independent.
    Q_UNUSED(fileNames)
}

void QMessage::clearAttachments()
{
    // Implementation note, this should be platform independent.
}

bool QMessage::isModified() const
{
    return false; // stub
}

QMessage QMessage::createResponseMessage(ResponseType type) const
{
    return QMessage(); // stub
}


QTM_END_NAMESPACE
