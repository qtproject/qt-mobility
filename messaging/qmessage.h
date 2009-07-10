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
#ifndef QMESSAGE_H
#define QMESSAGE_H
#include <QString>
#include <QList>
#include <QDateTime>
#include "qmessageaccountid.h"
#include "qmessagefolderid.h"
#include "qmessagecontentcontainer.h"
#include "qmessageaddress.h"

class QMessagePrivate;

class QMessage : public QMessageContentContainer {
    Q_DECLARE_PRIVATE(QMessage)

public:
    enum Type
    {
        Mms     = 0x1,
        Sms     = 0x2,
        Email   = 0x4,
        Xmpp    = 0x8,
        // Extensible
        None    = 0,
        AnyType = 0xFFFFFFFF
    };
    Q_DECLARE_FLAGS(TypeFlags, Type)

    enum Status
    {
        Read = 0x1,
        HasAttachments = 0x2,
        Incoming = 0x4,
        Removed = 0x8
    };
    Q_DECLARE_FLAGS(StatusFlags, Status)

    enum Priority
    {
        High = 1,
        Normal,
        Low
    };

    enum StandardFolder
    {
        InboxFolder = 1,
        OutboxFolder,
        DraftsFolder,
        SentFolder,
        TrashFolder
    };

    QMessage();
    QMessage(const QMessageId &id);
    virtual ~QMessage();

    static QMessage fromTransmissionFormat(Type t, const QByteArray &ba);
    static QMessage fromTransmissionFormatFile(Type t, const QString &fileName);

    virtual QByteArray toTransmissionFormat() const;
    virtual void toTransmissionFormat(QDataStream &out) const;

    virtual QMessageId id() const;

    virtual Type type() const;
    virtual void setType(Type t);

    virtual QMessageAccountId parentAccountId() const;
#ifdef QMESSAGING_OPTIONAL_FOLDER
    virtual QMessageFolderId parentFolderId() const;
#endif

    virtual StandardFolder standardFolder() const;
    virtual void setStandardFolder(StandardFolder sf);

    virtual QMessageAddress from() const;
    virtual void setFrom(const QMessageAddress &address);

    virtual QString subject() const;
    virtual void setSubject(const QString &s);

    virtual QDateTime date() const;
    virtual void setDate(const QDateTime &d);

    virtual QDateTime receivedDate() const;
    virtual void setReceivedDate(const QDateTime &d);

    virtual QMessageAddressList to() const;
    virtual void setTo(const QMessageAddressList &toList);
    virtual void setTo(const QMessageAddress &address);
    virtual QMessageAddressList cc() const;
    virtual void setCc(const QMessageAddressList &ccList);
    virtual QMessageAddressList bcc() const;
    virtual void setBcc(const QMessageAddressList &bccList);

    virtual StatusFlags status() const;
    virtual void setStatus(StatusFlags newStatus);

    virtual Priority priority() const;
    virtual void setPriority(Priority newPriority);

    virtual uint size() const;

    virtual QMessageContentContainerId body() const;
    virtual void setBody(const QString &body);
    virtual void setBodyFromFile(const QString &fileName);

    virtual QMessageContentContainerIdList attachments() const;
    virtual void appendAttachments(const QStringList &fileNames);
    virtual void clearAttachments();

#ifdef QMESSAGING_OPTIONAL
    virtual void setOriginatorPort(uint port);
    virtual uint originatorPort();
    virtual void setDestinationPort(uint port);
    virtual uint destinationPort();

    virtual QString customField(const QString &name) const;
    virtual void setCustomField(const QString &name, const QString &value);
    virtual QList<QString> customFields() const;
#endif

    virtual bool dataModified() const;

    virtual QMessage replyTo() const;
    virtual QMessage replyToAll() const;
    virtual QMessage forward() const;

private:
    friend class QMessageStore;

    void setId(const QMessageId &id);
    void setDataModified(bool modified);
    void setParentAccountId(const QMessageAccountId &accountId);
#ifdef QMESSAGING_OPTIONAL_FOLDER
    void setParentFolderId(const QMessageFolderId &folderId);
#endif
    void setSize(uint size);

    QMessagePrivate *d_ptr;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QMessage::TypeFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QMessage::StatusFlags)
#endif
