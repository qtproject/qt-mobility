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

#include "qmfhelpers_maemo6_p.h"
#include "qmessage_p.h"
#include "qmessageaccount_p.h"
#include "qmessagecontentcontainer_maemo6_p.h"
#include "qmessagemanager.h"
#include "qmessagefolder_p.h"
#include "qmessagefilter_p.h"
#include "qmessageaccountfilter_p.h"
#include "qmessagefolderfilter_p.h"
#include "qmessageaccountsortorder_p.h"
#include "qmessagefoldersortorder_p.h"
#include "qmessagesortorder_p.h"
#include <qmailnamespace.h>
#include <QFileInfo>
#include <QRegExp>

QTM_BEGIN_NAMESPACE

namespace {

quint64 messageStatusMask(const QString &field)
{
    return QMailStore::instance()->messageStatusMask(field);
}

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

class MessageConverter
{
public:
    MessageConverter(const QMessage &message)
	: _message(message)
	, _priv(QMessagePrivate::implementation(message))
    {}

    QMailMessage convert() const
    {
	QMailMessage result;

	result.setMessageType(QmfHelpers::convert(_priv->_type));
	result.setId(QmfHelpers::convert(_priv->_id));
	result.setSubject(_priv->_subject);
	result.setTo(QmfHelpers::convert(_priv->_to));
	result.setBcc(QmfHelpers::convert(_priv->_bcc));
	result.setCc(QmfHelpers::convert(_priv->_cc));
	result.setDate(QMailTimeStamp(_priv->_date));
	result.setReceivedDate(QMailTimeStamp(_priv->_receivedDate));

	if (!_priv->_contentScheme.isEmpty())
	    result.setContentScheme(_priv->_contentScheme);

	if (!_priv->_contentIdentifier.isEmpty())
	    result.setContentIdentifier(_priv->_contentIdentifier);

	if (!_priv->_from.addressee().isEmpty())
	    result.setFrom(QmfHelpers::convert(_priv->_from));

	if (_priv->_parentAccountId.isValid())
	    result.setParentAccountId(QmfHelpers::convert(_priv->_parentAccountId));
	
	if (_priv->_parentFolderId.isValid()) {
	    result.setParentFolderId(QmfHelpers::convert(_priv->_parentFolderId));
	} else {
	    QMessageFolderId folderId(QMessagePrivate::standardFolderId(_priv->_standardFolder));
	    result.setParentFolderId(QmfHelpers::convert(folderId));
	}

	setBodyAndAttachments(result);
	result.setStatus(QmfHelpers::convert(_priv->_status));

	quint64 setMask(0);
	quint64 unsetMask(0);

	if (_priv->_priority == QMessage::HighPriority) {
	    setMask = QmfHelpers::highPriorityMask();
	    unsetMask = QmfHelpers::lowPriorityMask();
	} else if (_priv->_priority == QMessage::LowPriority) {
	    unsetMask = QmfHelpers::highPriorityMask();
	    setMask = QmfHelpers::lowPriorityMask();
	} else {
	    unsetMask = (QmfHelpers::highPriorityMask() | QmfHelpers::lowPriorityMask());
	}

	if (setMask) {
	    result.setStatus(setMask, true);
	}
	if (unsetMask) {
	    result.setStatus(unsetMask, false);
	}
	result.setSize(_message.size());

	return result;
    }

private:

    QString getBody(QMailMessageContentType &contentType) const
    {
	QMessageContentContainerId bodyId;
	QMessageContentContainer body;
	QString data;

	bodyId = _message.bodyId();
	if (bodyId.isValid()) {
	    body = _message.find(bodyId);
	} else {
	    body = _message;
	}

	contentType.setType(body.contentType());
	contentType.setSubType(body.contentSubType());
	contentType.setParameter("charset", body.contentCharset()); 

	if (!contentType.isNull() && body.isContentAvailable()) {
	    data = body.textContent();
	}
	
	return data;
    }

    void setBodyAndAttachments(QMailMessage &result) const
    {
	QMessageContentContainerIdList attachmentIds;
	QMessage::StatusFlags messageStatus;

	messageStatus = _message.status();

	if (messageStatus & QMessage::HasAttachments) {
	    QMailMessageContentType contentType;
	    QString bodyText = getBody(contentType);

	    QMailMessageContentDisposition contentDisposition(QMailMessageContentDisposition::Inline);
	    contentDisposition.setSize(bodyText.length());
	
	    QMailMessagePart textPart(QMailMessagePart::fromData(bodyText, contentDisposition, contentType, QMailMessageBody::Base64));
	    result.setMultipartType(QMailMessage::MultipartMixed);
	    result.prependPart(textPart);

	    attachmentIds = _message.attachmentIds();
	    QMessageContentContainerId bodyId(_message.bodyId());
	    foreach (QMessageContentContainerId identifier, attachmentIds) {

		if (identifier.isValid() == false) continue;

		QMessageContentContainer attachmentCont;
		attachmentCont = _message.find(identifier);

		if (attachmentCont.isContentAvailable () == false) continue;

		QByteArray filename = QMessageContentContainerPrivate::attachmentFilename(attachmentCont);
		QFileInfo info(filename);
		QMailMessageContentDisposition content(QMailMessageContentDispositionFwd::Attachment);
		if (filename.isEmpty() == false) {
		    content.setFilename(info.fileName().toAscii());
		}
		content.setSize(info.size());

		QString contentType = attachmentCont.contentType();
		if (contentType.isEmpty() == false) {
		    contentType.append("/");
		    contentType.append (attachmentCont.contentSubType());
		} else {
		    contentType = QMail::mimeTypeFromFileName(filename);
		}

		QMailMessagePart part = QMailMessagePart::fromFile(filename, content, contentType.toAscii(),
								   QMailMessageBody::Base64, QMailMessageBody::RequiresEncoding);
		foreach (const QByteArray &name, attachmentCont.headerFields()) {
		    QString value = attachmentCont.headerFieldValue(name);
		    part.setHeaderField(name, value);
		}
		result.appendPart(part);
	    }
	} else {
	    QMailMessageContentType contentType;
	    QString data = getBody(contentType);
	    if (!data.isEmpty()) {
		result.setBody(QMailMessageBody::fromData(data, contentType, QMailMessageBody::Base64));
	    }
	}
    }

    const QMessage &_message;
    const QMessagePrivate *_priv;
};


class MailMessageConverter
{
public:
    MailMessageConverter(const QMailMessage &message)
	: _message(message)
    {}

    QMessage convert() const
    {
	QMessage result;
	QMessagePrivate *priv = QMessagePrivate::implementation(result);

	priv->_id = QmfHelpers::convert(_message.id());
	priv->_subject = _message.subject();
	priv->_to = QmfHelpers::convert(_message.to());
	priv->_from = QmfHelpers::convert(_message.from());
	priv->_bcc = QmfHelpers::convert(_message.bcc());
	priv->_cc = QmfHelpers::convert(_message.cc());
	priv->_date = _message.date().toLocalTime();
	priv->_receivedDate = _message.receivedDate().toLocalTime();
	priv->_type =  QmfHelpers::convert(_message.messageType());
	priv->_status = QmfHelpers::convert(_message.status());
	priv->_parentAccountId = QmfHelpers::convert(_message.parentAccountId());
	priv->_parentFolderId = QmfHelpers::convert(_message.parentFolderId());
	priv->_contentIdentifier = _message.contentIdentifier();
	priv->_contentScheme = _message.contentScheme();
	priv->_standardFolder = QMessagePrivate::standardFolder(priv->_parentFolderId);

	quint64 status(_message.status());
	if (status & QmfHelpers::highPriorityMask()) {
	    priv->_priority = QMessage::HighPriority;
	} else if (status & QmfHelpers::lowPriorityMask()) {
	    priv->_priority = QMessage::LowPriority;
	} else {
	    priv->_priority = QMessage::NormalPriority;
	}

	if (_message.hasBody()) {
	    QMailMessageBody body = _message.body();
	    setResultBody(result, body);
	} else {
	    TextPartLocator locator;
	    _message.foreachPart<TextPartLocator&>(locator);
	    if (locator._location.isValid()) {
		const QMailMessagePart &bodyPart = _message.partAt(locator._location);
		if (bodyPart.contentAvailable()) {
		    QMailMessageBody body = bodyPart.body();
		    setResultBody(result, body);
		}
	    }
	}

	AttachmentLocator locator;
	_message.foreachPart<AttachmentLocator&>(locator);
	foreach (const QMailMessagePart::Location &location, locator._locations) {
	    const QMailMessagePart &bodyPart = _message.partAt(location);
	    QMailMessageContentType contentType = bodyPart.contentType();
	    QMessageContentContainer attachment;
	    QMessageContentContainerPrivate *container = QMessageContentContainerPrivate::implementation(attachment);
	
	    container->_containingMessageId = QmfHelpers::convert(_message.id()).toString();
	    container->_attachmentId = location.toString(true);
	    container->_name = bodyPart.contentDisposition().filename();
	    container->_type = contentType.type();
	    container->_subType = contentType.subType();
	    container->_available = bodyPart.contentAvailable();
	
	    if (container->_available) {
		if (container->_type.toLower() == "text") {
		    container->_textContent = bodyPart.body().data();
		    container->_size = container->_textContent.size();
		} else {
		    container->_content = bodyPart.body().data(QMailMessageBodyFwd::Decoded);
		    container->_size = container->_content.size();
		}
	    }

	    appendAttachmentToMessage(result, attachment);
	}

	priv->_size = static_cast<int>(_message.size());
	priv->_modified = false;

	return result;
    }

private:

    void appendAttachmentToMessage(QMessage &message, QMessageContentContainer &attachment) const
    {
	QMessagePrivate *privateMessage = QMessagePrivate::implementation(message);
	QMessageContentContainerPrivate *container = QMessagePrivate::containerImplementation(message);

	if (container->_attachments.isEmpty()) {
	    QMessageContentContainerId existingBodyId(message.bodyId());
	    if (existingBodyId == QMessageContentContainerPrivate::bodyContentId()) {
		// The body content is in the message itself - move it to become the first attachment
		QMessageContentContainer newBody(message);
		QMessageContentContainerPrivate::implementation(newBody)->setDerivedMessage(0);

		container->setContentType("multipart", "mixed", "");
		privateMessage->_bodyId = container->prependContent(newBody);
	    } else {
		// This message is now multipart
		container->setContentType("multipart", "mixed", "");
	    }
	    container->_available = true;
	}

	container->appendContent(attachment);

	bool haveAttachments = !container->_attachments.isEmpty();
	message.setStatus(QMessage::HasAttachments,haveAttachments);

	privateMessage->_modified = true;
    }

    void setResultBody(QMessage &result, const QMailMessageBody &body) const
    {
	QString data = body.data();
	QByteArray contentType = body.contentType().content();
	QByteArray charset = body.contentType().charset();
	if (!charset.isEmpty()) {
	    contentType += "; charset=";
	    contentType += charset;
	}
	result.setBody(data, contentType);
    }

    const QMailMessage &_message;
};

}

namespace QmfHelpers {

QMessageId convert(const QMailMessageId &id)
{
    if (id.isValid()) {
	return QMessageId(QString("QMF_%1").arg(id.toULongLong()));
    }

    return QMessageId();
}

QMailMessageId convert(const QMessageId &id)
{
    if (id.isValid()) {
	return QMailMessageId(id.toString().mid(4).toULongLong());
    }
    
    return QMailMessageId();
}

QMessageAccountId convert(const QMailAccountId &id)
{
    if (id.isValid()) {
	return QMessageAccountId(QString("QMF_%1").arg(id.toULongLong()));
    }
    
    return QMessageAccountId();
}

QMailAccountId convert(const QMessageAccountId &id)
{
    if (id.isValid()) {
	return QMailAccountId(id.toString().mid(4).toULongLong());
    }

    return QMailAccountId();
}

QMessageFolderId convert(const QMailFolderId &id)
{
    if (id.isValid()) {
	return QMessageFolderId(QString("QMF_%1").arg(id.toULongLong()));
    }
    
    return QMessageFolderId();
}

QMailFolderId convert(const QMessageFolderId &id)
{
    if (id.isValid()) {
	return QMailFolderId(id.toString().mid(4).toULongLong());
    }

    return QMailFolderId();
}

QMessageContentContainerId convert(const QMailMessagePart::Location &location)
{
    return QMessageContentContainerId(location.toString(true));
}

QMailMessagePart::Location convert(const QMessageContentContainerId &id)
{
    QRegExp pattern("^\\d*_(.+)");

    int pos = pattern.indexIn(id.toString());
    if (pos > -1) {
	return QMailMessagePart::Location(pattern.cap(1));
    }
    return QMailMessagePart::Location();
}

QMessageIdList convert(const QList<QMailMessageId> &ids)
{
    QMessageIdList result;

    foreach (const QMailMessageId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QList<QMailMessageId> convert(const QMessageIdList &ids)
{
    QList<QMailMessageId> result;

    foreach (const QMessageId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QMessageAccountIdList convert(const QList<QMailAccountId> &ids)
{
    QMessageAccountIdList result;

    foreach (const QMailAccountId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QList<QMailAccountId> convert(const QMessageAccountIdList &ids)
{
    QList<QMailAccountId> result;

    foreach (const QMessageAccountId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QMessageFolderIdList convert(const QList<QMailFolderId> &ids)
{
    QMessageFolderIdList result;

    foreach (const QMailFolderId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QList<QMailFolderId> convert(const QMessageFolderIdList &ids)
{
    QList<QMailFolderId> result;

    foreach (const QMessageFolderId &id, ids) {
        result.append(convert(id));
    }

    return result;
}

QMessageContentContainerIdList convert(const QList<QMailMessagePart::Location> &locations)
{
    QMessageContentContainerIdList result;

    foreach (const QMailMessagePart::Location &location, locations) {
        result.append(convert(location));
    }

    return result;
}

/*
QList<QMailMessagePart::Location> convert(const QMessageContentContainerIdList &ids)
{
    QList<QMailMessagePart::Location> result;

    foreach (const QMessageContentContainerId &id, ids) {
        result.append(convert(id));
    }

    return result;
}
*/

QMailMessage::MessageType convert(QMessage::Type t)
{
    QMailMessage::MessageType result(QMailMessage::None);

    // This could be a single value or a mask
    if (t & QMessage::Mms) {
        result = static_cast<QMailMessage::MessageType>(result | QMailMessage::Mms);
    }
    if (t & QMessage::Sms) {
        result = static_cast<QMailMessage::MessageType>(result | QMailMessage::Sms);
    }
    if (t & QMessage::Email) {
        result = static_cast<QMailMessage::MessageType>(result | QMailMessage::Email);
    }
    if (t & QMessage::InstantMessage) {
        result = static_cast<QMailMessage::MessageType>(result | QMailMessage::Instant);
    }

    return result;
}

QMessage::Type convert(QMailMessage::MessageType t)
{
    QMessage::Type result(QMessage::NoType);

    // This could be a single value or a mask
    if (t & QMailMessage::Mms) {
        result = static_cast<QMessage::Type>(static_cast<uint>(result | QMessage::Mms));
    }
    if (t & QMailMessage::Sms) {
        result = static_cast<QMessage::Type>(static_cast<uint>(result | QMessage::Sms));
    }
    if (t & QMailMessage::Email) {
        result = static_cast<QMessage::Type>(static_cast<uint>(result | QMessage::Email));
    }
    if (t & QMailMessage::Instant) {
        result = static_cast<QMessage::Type>(static_cast<uint>(result | QMessage::InstantMessage));
    }

    return result;
}

QMailMessage::MessageType convert(QMessage::TypeFlags v)
{
    return convert(static_cast<QMessage::Type>(static_cast<uint>(v)));
}

/*
QMailStore::ErrorCode convert(QMessageManager::Error v)
{
    switch (v) {
    case QMessageManager::InvalidId: return QMailStore::InvalidId;
    case QMessageManager::ConstraintFailure: return QMailStore::ConstraintFailure;
    case QMessageManager::ContentInaccessible: return QMailStore::ContentInaccessible;
    case QMessageManager::NotYetImplemented: return QMailStore::NotYetImplemented;
    case QMessageManager::FrameworkFault: return QMailStore::FrameworkFault;
    default: break;
    }

    return QMailStore::NoError;
}
*/

QMessageManager::Error convert(QMailStore::ErrorCode v)
{
    switch (v) {
    case QMailStore::InvalidId: return QMessageManager::InvalidId;
    case QMailStore::ConstraintFailure: return QMessageManager::ConstraintFailure;
    case QMailStore::ContentInaccessible: return QMessageManager::ContentInaccessible;
    case QMailStore::NotYetImplemented: return QMessageManager::NotYetImplemented;
    case QMailStore::FrameworkFault: return QMessageManager::FrameworkFault;
    default: break;
    }

    return QMessageManager::NoError;
}

QMailStore::MessageRemovalOption convert(QMessageManager::RemovalOption v)
{
    switch (v) {
    case QMessageManager::RemoveOnOriginatingServer: return QMailStore::CreateRemovalRecord;
    default: break;
    }

    return QMailStore::NoRemovalRecord;
}

/*
QMessageManager::RemovalOption convert(QMailStore::MessageRemovalOption v)
{
    switch (v) {
    case QMailStore::CreateRemovalRecord: return QMessageManager::RemoveOnOriginatingServer;
    default: break;
    }

    return QMessageManager::RemoveLocalCopyOnly;
}
*/

QMailServiceAction::Activity convert(QMessageService::State v)
{
    switch (v) {
    case QMessageService::InactiveState: return QMailServiceAction::Pending;
    case QMessageService::ActiveState: return QMailServiceAction::InProgress;
    case QMessageService::CanceledState: return QMailServiceAction::Failed;
    case QMessageService::FinishedState: return QMailServiceAction::Successful;
    default: break;
    }

    return QMailServiceAction::Pending;
}

QMessageService::State convert(QMailServiceAction::Activity v)
{
    switch (v) {
    case QMailServiceAction::Pending: return QMessageService::InactiveState;
    case QMailServiceAction::InProgress: return QMessageService::ActiveState;
    case QMailServiceAction::Successful: return QMessageService::FinishedState;
    case QMailServiceAction::Failed: return QMessageService::FinishedState;
    default: break;
    }

    return QMessageService::InactiveState;
}

QMessage::StatusFlags convert(quint64 v)
{
    QMessage::StatusFlags result(0);

    if (v & (QMailMessage::Read | QMailMessage::ReadElsewhere)) {
        result |= QMessage::Read;
    }
    if (v & QMailMessage::HasAttachments) {
        result |= QMessage::HasAttachments;
    }
    if (v & QMailMessage::Incoming) {
        result |= QMessage::Incoming;
    }
    if (v & QMailMessage::Removed) {
        result |= QMessage::Removed;
    }

    return result;
}

quint64 convert(QMessage::Status v)
{
    // We cannot rely on the QMailMessage status masks until the store has been initialized
    static QMailStore *store = QMailStore::instance();
    Q_UNUSED(store);

    quint64 result(0);

    if (v & QMessage::Read) {
        result |= QMailMessage::Read;
    }
    if (v & QMessage::HasAttachments) {
        result |= QMailMessage::HasAttachments;
    }
    if (v & QMessage::Incoming) {
        result |= QMailMessage::Incoming;
    }
    if (v & QMessage::Removed) {
        result |= QMailMessage::Removed;
    }

    return result;
}

quint64 convert(QMessage::StatusFlags v)
{
    return convert(static_cast<QMessage::Status>(static_cast<uint>(v)));
}

QMessageAddress convert(const QMailAddress &address)
{
    QString addr(address.toString());
    if (!addr.isEmpty()) {
        QMessageAddress::Type type(QMessageAddress::Email);

        QRegExp suffix("\\s+\\(TYPE=(\\w*)\\)$");
        int index = suffix.indexIn(addr);
        if (index != -1) {
            addr = addr.left(addr.length() - suffix.cap(0).length());

            QString spec(suffix.cap(1));
            if (spec == "System") {
                type = QMessageAddress::System;
            } else if (spec == "Phone") {
                type = QMessageAddress::Phone;
            } else if (spec == "InstantMessage") {
                type = QMessageAddress::InstantMessage;
            }
        }

        return QMessageAddress(type, addr);
    }

    return QMessageAddress();
}

QMailAddress convert(const QMessageAddress &address)
{
    QString suffix;
    if (address.type() == QMessageAddress::System) {
        suffix = " (TYPE=System)";
    } else if (address.type() == QMessageAddress::Phone) {
        suffix = " (TYPE=Phone)";
    } else if (address.type() == QMessageAddress::InstantMessage) {
        suffix = " (TYPE=InstantMessage)";
    }

    return QMailAddress(address.addressee() + suffix);
}

QMessageAddressList convert(const QList<QMailAddress> &list)
{
    QList<QMessageAddress> result;

    foreach (const QMailAddress &address, list) {
        result.append(convert(address));
    }

    return result;
}

QList<QMailAddress> convert(const QMessageAddressList& list)
{
    QList<QMailAddress> result;

    foreach (const QMessageAddress &address, list) {
        result.append(convert(address));
    }

    return result;
}

/*
QMessageDataComparator::EqualityComparator convert(QMailDataComparator::EqualityComparator cmp)
{
    switch (cmp)
    {
    case QMailDataComparator::Equal: return QMessageDataComparator::Equal;
    case QMailDataComparator::NotEqual: return QMessageDataComparator::NotEqual;
    default: break;
    }

    return QMessageDataComparator::Equal;
}
*/

QMailDataComparator::EqualityComparator convert(QMessageDataComparator::EqualityComparator cmp)
{
    switch (cmp)
    {
    case QMessageDataComparator::Equal: return QMailDataComparator::Equal;
    case QMessageDataComparator::NotEqual: return QMailDataComparator::NotEqual;
    default: break;
    }

    return QMailDataComparator::Equal;
}

/*
QMessageDataComparator::InclusionComparator convert(QMailDataComparator::InclusionComparator cmp)
{
    switch (cmp)
    {
    case QMailDataComparator::Includes: return QMessageDataComparator::Includes;
    case QMailDataComparator::Excludes: return QMessageDataComparator::Excludes;
    default: break;
    }
    
    return QMessageDataComparator::Includes;
}
*/

QMailDataComparator::InclusionComparator convert(QMessageDataComparator::InclusionComparator cmp)
{
    switch (cmp)
    {
    case QMessageDataComparator::Includes: return QMailDataComparator::Includes;
    case QMessageDataComparator::Excludes: return QMailDataComparator::Excludes;
    default: break;
    }

    return QMailDataComparator::Includes;
}

/*
QMessageDataComparator::RelationComparator convert(QMailDataComparator::RelationComparator cmp)
{
    switch (cmp)
    {
    case QMailDataComparator::LessThan: return QMessageDataComparator::LessThan;
    case QMailDataComparator::LessThanEqual: return QMessageDataComparator::LessThanEqual;
    case QMailDataComparator::GreaterThan: return QMessageDataComparator::GreaterThan;
    case QMailDataComparator::GreaterThanEqual: return QMessageDataComparator::GreaterThanEqual;
    default: break;
    }
    
    return QMessageDataComparator::LessThan;
}
*/

QMailDataComparator::RelationComparator convert(QMessageDataComparator::RelationComparator cmp)
{
    switch (cmp)
    {
    case QMessageDataComparator::LessThan: return QMailDataComparator::LessThan;
    case QMessageDataComparator::LessThanEqual: return QMailDataComparator::LessThanEqual;
    case QMessageDataComparator::GreaterThan: return QMailDataComparator::GreaterThan;
    case QMessageDataComparator::GreaterThanEqual: return QMailDataComparator::GreaterThanEqual;
    default: break;
    }

    return QMailDataComparator::LessThan;
}

static QMailAccountKey convertOne(const QMessageAccountFilter &filter)
{
    QMessageAccountFilterPrivate *p = QMessageAccountFilterPrivate::implementation(filter);

    if (!p->_valid || p->_notFilter)
	return ~QMailAccountKey();

    switch (p->_field) {
    case QMessageAccountFilterPrivate::None:
	break;
    case QMessageAccountFilterPrivate::Id:
	if (p->_comparatorType == QMessageAccountFilterPrivate::Equality) {
	    QMailAccountId id = convert(QMessageAccountId(p->_value.toString()));
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailAccountKey::id(id, cmp);
	} else if (p->_comparatorType == QMessageAccountFilterPrivate::Inclusion) {
	    QMailAccountIdList ids = convert(p->_ids);
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));
	    return QMailAccountKey::id(ids, cmp);
	}
	break;
    case QMessageAccountFilterPrivate::Name:
	if (p->_comparatorType == QMessageAccountFilterPrivate::Equality) {
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailAccountKey::name(p->_value.toString(), cmp);
	} else if (p->_comparatorType == QMessageAccountFilterPrivate::Inclusion) {
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));
	    return QMailAccountKey::name(p->_value.toString(), cmp);
	}
	break;
    }

    return QMailAccountKey();
}

QMailAccountKey convert(const QMessageAccountFilter &filter)
{
    QMessageAccountFilterPrivate *p =  QMessageAccountFilterPrivate::implementation(filter);

    if (p->_filterList.isEmpty())
	return convertOne(filter);

    QMailAccountKey result = ~QMailAccountKey();

    for (int i = 0; i < p->_filterList.count(); ++i) {
	QMailAccountKey key;
	for (int j = 0; j < p->_filterList[i].count(); ++j) {
	    key &= convertOne(p->_filterList[i][j]);
	}
	result |= key;
    }

    return result;    
}

static QMailFolderKey convertOne(const QMessageFolderFilter &filter)
{
    QMessageFolderFilterPrivate *p =  QMessageFolderFilterPrivate::implementation(filter);
    if (!p->_valid || p->_notFilter)
	return ~QMailFolderKey();

    switch (p->_field) {
    case QMessageFolderFilterPrivate::None:
	break;
    case QMessageFolderFilterPrivate::ParentAccountIdFilter:
	if (p->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
	    QMailAccountKey key = convert(*p->_accountFilter);
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));
	    return QMailFolderKey::parentAccountId(key, cmp);
	}
	break;
    case QMessageFolderFilterPrivate::Id:
	if (p->_comparatorType == QMessageFolderFilterPrivate::Equality) {
	    QMailFolderId id = convert(QMessageFolderId(p->_value.toString()));
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailFolderKey::id(id, cmp);
	} else if (p->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
	    QMailFolderIdList ids = convert(p->_ids);
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));
	    return QMailFolderKey::id(ids, cmp);
	}
	break;
    case QMessageFolderFilterPrivate::ParentFolderId:
	qWarning() << "QMessageFolderFilterPrivate::ParentFolderId" << "not implemented";
	break;
    case QMessageFolderFilterPrivate::AncestorFolderIds:
	qWarning() << "QMessageFolderFilterPrivate::AncestorFolderIds" << "not implemented";
	break;
    case QMessageFolderFilterPrivate::ParentAccountId:
	if (p->_comparatorType == QMessageFolderFilterPrivate::Equality) {
	    QMailAccountId id = convert(QMessageAccountId(p->_value.toString()));
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailFolderKey::parentAccountId(id, cmp);
	}
	break;
    case QMessageFolderFilterPrivate::Name:
	if (p->_comparatorType == QMessageFolderFilterPrivate::Equality) {
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailFolderKey::displayName(p->_value.toString(), cmp);
	} else if (p->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));
	    return QMailFolderKey::displayName(p->_value.toString(), cmp);
	}
	break;
    case QMessageFolderFilterPrivate::Path:
	if (p->_comparatorType == QMessageFolderFilterPrivate::Equality) {
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailFolderKey::path(p->_value.toString(), cmp);
	} else if (p->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));
	    return QMailFolderKey::path(p->_value.toString(), cmp);
	}
	break;
    }

    return QMailFolderKey();
}

QMailFolderKey convert(const QMessageFolderFilter &filter)
{
    QMessageFolderFilterPrivate *p =  QMessageFolderFilterPrivate::implementation(filter);

    if (p->_filterList.isEmpty())
	return convertOne(filter);

    QMailFolderKey result = ~QMailFolderKey();
    for (int i = 0; i < p->_filterList.count(); ++i) {
	QMailFolderKey key;
	for (int j = 0; j < p->_filterList[i].count(); ++j) {
	    key &= convertOne(p->_filterList[i][j]);
	}
	result |= key;
    }

    return result;
}

static QMailMessageKey convertOne(const QMessageFilter &filter)
{
    QMessageFilterPrivate *p = QMessageFilterPrivate::implementation(filter);

    if (!p->_valid || p->_notFilter)
	return ~QMailMessageKey();

    switch (p->_field) {
    case QMessageFilterPrivate::None:
	break;
    case QMessageFilterPrivate::AncestorFolderIds:
	break;
    case QMessageFilterPrivate::ParentAccountIdFilter:
	if (p->_comparatorType == QMessageFilterPrivate::Inclusion) {
	    QMailAccountKey key = convert(*p->_accountFilter);
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));
	    return QMailMessageKey::parentAccountId(key, cmp);
	}
    case QMessageFilterPrivate::ParentFolderIdFilter:
	if (p->_comparatorType == QMessageFilterPrivate::Inclusion) {
	    QMailFolderKey key = convert(*p->_folderFilter);
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));
	    return QMailMessageKey::parentFolderId(key, cmp);
	}
	break;
    case QMessageFilterPrivate::Id:
	if (p->_comparatorType == QMessageFilterPrivate::Equality) {
	    QMailMessageId id = convert(QMessageId(p->_value.toString()));
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailMessageKey::id(id, cmp);
	} else if (p->_comparatorType == QMessageFilterPrivate::Inclusion) {
	    QMailMessageIdList ids = convert(p->_ids);
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));
	    return QMailMessageKey::id(ids, cmp);
	}
	break;
    case QMessageFilterPrivate::ParentFolderId:
	if (p->_comparatorType == QMessageFilterPrivate::Equality) {
	    QMailFolderId id = convert(QMessageFolderId(p->_value.toString()));
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailMessageKey::parentFolderId(id, cmp);
	}
	break;
    case QMessageFilterPrivate::ParentAccountId:
	if (p->_comparatorType == QMessageFilterPrivate::Equality) {
	    QMailAccountId id = convert(QMessageAccountId(p->_value.toString()));
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailMessageKey::parentAccountId(id, cmp);
	}
	break;
    case QMessageFilterPrivate::Type:
	if (p->_comparatorType == QMessageFilterPrivate::Equality) {
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    QMailMessageMetaDataFwd::MessageType type = convert(static_cast<QMessage::Type>(p->_value.toUInt()));
	    return QMailMessageKey::messageType(type, cmp);
	} else if (p->_comparatorType == QMessageFilterPrivate::Inclusion) {
	    QMailMessageMetaDataFwd::MessageType type = convert(static_cast<QMessage::TypeFlags>(p->_value.toUInt()));
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));
	    return QMailMessageKey::messageType(type, cmp);
	}
	break;
    case QMessageFilterPrivate::StandardFolder:
	{
	    QMailDataComparator::InclusionComparator cmp;
	    QMessage::StandardFolder standardFolder = static_cast<QMessage::StandardFolder>(p->_value.toInt());

	    if (static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue) == QMessageDataComparator::Equal) {
		cmp = QMailDataComparator::Includes;
	    } else {
		cmp = QMailDataComparator::Excludes;
	    }

	    switch (standardFolder){
	    case QMessage::InboxFolder: 
		{
		    QMailMessageKey key;
		    key = QMailMessageKey::status(QMailMessage::Incoming, cmp);
		    key &= ~QMailMessageKey::status(QMailMessage::Trash, cmp);
		    return key;
		}
	    case QMessage::OutboxFolder:
	        return QMailMessageKey::status(QMailMessage::Outbox, cmp);
	    case QMessage::DraftsFolder:
		return QMailMessageKey::status(QMailMessage::Draft, cmp);
	    case QMessage::SentFolder:
		return QMailMessageKey::status(QMailMessage::Sent, cmp);
	    case QMessage::TrashFolder:
		return QMailMessageKey::status(QMailMessage::Trash, cmp);
	    }
	}
	break;
    case QMessageFilterPrivate::TimeStamp:
	if (p->_comparatorType == QMessageFilterPrivate::Equality) {
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailMessageKey::timeStamp(p->_value.toDateTime(), cmp);
	} else if (p->_comparatorType == QMessageFilterPrivate::Relation) {
	    QMailDataComparator::RelationComparator cmp = convert(static_cast<QMessageDataComparator::RelationComparator>(p->_comparatorValue));
	    return QMailMessageKey::timeStamp(p->_value.toDateTime(), cmp);
	}
	break;
    case QMessageFilterPrivate::ReceptionTimeStamp:
	if (p->_comparatorType == QMessageFilterPrivate::Equality) {
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailMessageKey::receptionTimeStamp(p->_value.toDateTime(), cmp);
	} else if (p->_comparatorType == QMessageFilterPrivate::Relation) {
	    QMailDataComparator::RelationComparator cmp = convert(static_cast<QMessageDataComparator::RelationComparator>(p->_comparatorValue));
	    return QMailMessageKey::receptionTimeStamp(p->_value.toDateTime(), cmp);
	}
	break;
    case QMessageFilterPrivate::Sender:
	if (p->_comparatorType == QMessageFilterPrivate::Equality) {
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailMessageKey::sender(p->_value.toString(), cmp);
	} else if (p->_comparatorType == QMessageFilterPrivate::Inclusion) {
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));
	    return QMailMessageKey::sender(p->_value.toString(), cmp);
	}
	break;
    case QMessageFilterPrivate::Recipients:
	{
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));
	    return QMailMessageKey::recipients(p->_value.toString(), cmp);
	}
	break;
    case QMessageFilterPrivate::Subject:
	if (p->_comparatorType == QMessageFilterPrivate::Equality) {
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailMessageKey::subject(p->_value.toString(), cmp);
	} else if (p->_comparatorType == QMessageFilterPrivate::Inclusion) {
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));
	    return QMailMessageKey::subject(p->_value.toString(), cmp);
	}
	break;
    case QMessageFilterPrivate::Priority:
	{
	    QMessageDataComparator::EqualityComparator cmp = static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue);
	    QMessage::Priority priority = static_cast<QMessage::Priority>(p->_value.toInt());
	    switch (priority)
	    {
	    case QMessage::HighPriority: 
		return QMailMessageKey::status(highPriorityMask(), (cmp == QMessageDataComparator::Equal ? QMailDataComparator::Includes : QMailDataComparator::Excludes)); 
	    case QMessage::LowPriority: 
		return QMailMessageKey::status(lowPriorityMask(), (cmp == QMessageDataComparator::Equal ? QMailDataComparator::Includes : QMailDataComparator::Excludes)); 
	    case QMessage::NormalPriority:
		return QMailMessageKey::status(lowPriorityMask() | highPriorityMask(), (cmp == QMessageDataComparator::Equal ? QMailDataComparator::Excludes : QMailDataComparator::Includes)); 
	    }
	}
    case QMessageFilterPrivate::Status:
	if (p->_comparatorType == QMessageFilterPrivate::Equality) {
	    QMessageDataComparator::EqualityComparator cmp = static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue);
	    quint64 mask = convert(static_cast<QMessage::Status>(p->_value.toInt()));
	    return QMailMessageKey::status(mask, (cmp == QMessageDataComparator::Equal ? QMailDataComparator::Includes : QMailDataComparator::Excludes));
	} else if (p->_comparatorType == QMessageFilterPrivate::Inclusion) {
	    QMailMessageKey key;

	    QMessage::StatusFlags mask = static_cast<QMessage::StatusFlags>(p->_value.toInt());
	    QMailDataComparator::InclusionComparator cmp = convert(static_cast<QMessageDataComparator::InclusionComparator>(p->_comparatorValue));

	    if (mask & QMessage::Read) {
		key &= QMailMessageKey::status(convert(QMessage::Read), cmp);
	    }
	    if (mask & QMessage::HasAttachments) {
		key &= QMailMessageKey::status(convert(QMessage::HasAttachments), cmp);
	    }
	    if (mask & QMessage::Incoming) {
		key &= QMailMessageKey::status(convert(QMessage::Incoming), cmp);
	    }
	    if (mask & QMessage::Removed) {
		key &= QMailMessageKey::status(convert(QMessage::Removed), cmp);
	    }

	    if (key.isEmpty()) {
		key = QMailMessageKey::nonMatchingKey();
	    }

	    return p->_notFilterForComparator ? ~key : key;
	}
	break;
    case QMessageFilterPrivate::Size:
	if (p->_comparatorType == QMessageFilterPrivate::Equality) {
	    QMailDataComparator::EqualityComparator cmp = convert(static_cast<QMessageDataComparator::EqualityComparator>(p->_comparatorValue));
	    return QMailMessageKey::size(p->_value.toInt(), cmp);
	} else if (p->_comparatorType == QMessageFilterPrivate::Relation) {
	    QMailDataComparator::RelationComparator cmp = convert(static_cast<QMessageDataComparator::RelationComparator>(p->_comparatorValue));
	    return QMailMessageKey::size(p->_value.toInt(), cmp);
	}
	break;
    }

    return QMailMessageKey();
}

QMailMessageKey convert(const QMessageFilter &filter)
{
    QMessageFilterPrivate *p =  QMessageFilterPrivate::implementation(filter);

    if (p->_filterList.isEmpty())
	return convertOne(filter);

    QMailMessageKey result = ~QMailMessageKey();

    for (int i = 0; i < p->_filterList.count(); ++i) {
	QMailMessageKey key;
	for (int j = 0; j < p->_filterList[i].count(); ++j) {
	    key &= convertOne(p->_filterList[i][j]);
	}
	result |= key;
    }

    return result;
}

QMailAccountSortKey convert(const QMessageAccountSortOrder &sortOrder)
{
    if (sortOrder.isEmpty())
	return QMailAccountSortKey::name();
    Qt::SortOrder order = QMessageAccountSortOrderPrivate::order(sortOrder);
    return QMailAccountSortKey::name(order);
}

static QMailFolderSortKey convert(const QPair<QMessageFolderSortOrderPrivate::Field, Qt::SortOrder> &pair)
{
    switch (pair.first) {
    case QMessageFolderSortOrderPrivate::Name:
	return QMailFolderSortKey::displayName(pair.second);
    case QMessageFolderSortOrderPrivate::Path:
	return QMailFolderSortKey::path(pair.second);
    }
    return QMailFolderSortKey();
}

QMailFolderSortKey convert(const QMessageFolderSortOrder &sortOrder)
{
    QMessageFolderSortOrderPrivate *p = QMessageFolderSortOrderPrivate::implementation(sortOrder);
    QMailFolderSortKey result;

    if (p->_fieldOrderList.count()) {
	for (int i = 0; i < p->_fieldOrderList.count(); ++i) {
	    result &= convert(p->_fieldOrderList[i]);
	}
    }

    return result;
}

static QMailMessageSortKey convert(const QPair<QMessageSortOrderPrivate::Field, Qt::SortOrder> &pair)
{
    switch (pair.first) {
    case QMessageSortOrderPrivate::Type:
	return QMailMessageSortKey::messageType(pair.second);
    case QMessageSortOrderPrivate::Sender:
	return QMailMessageSortKey::sender(pair.second);
    case QMessageSortOrderPrivate::Recipients:
	return QMailMessageSortKey::recipients(pair.second);
    case QMessageSortOrderPrivate::Subject:
	return QMailMessageSortKey::subject(pair.second);
    case QMessageSortOrderPrivate::TimeStamp:
	return QMailMessageSortKey::timeStamp(pair.second);
    case QMessageSortOrderPrivate::ReceptionTimeStamp:
	return QMailMessageSortKey::receptionTimeStamp(pair.second);
    case QMessageSortOrderPrivate::Read:
	return QMailMessageSortKey::status(QMailMessageMetaData::Read, pair.second);
    case QMessageSortOrderPrivate::HasAttachments:
	return QMailMessageSortKey::status(QMailMessageMetaData::HasAttachments, pair.second);
    case QMessageSortOrderPrivate::Incoming:
	return QMailMessageSortKey::status(QMailMessageMetaData::Incoming, pair.second);
    case QMessageSortOrderPrivate::Removed:
	return QMailMessageSortKey::status(QMailMessageMetaData::Removed, pair.second);
    case QMessageSortOrderPrivate::Priority:
	break;
    case QMessageSortOrderPrivate::Size:
	return QMailMessageSortKey::size(pair.second);
    }
    return QMailMessageSortKey();
}

QMailMessageSortKey convert(const QMessageSortOrder &sortOrder)
{
    QMessageSortOrderPrivate *p = QMessageSortOrderPrivate::implementation(sortOrder);
    QMailMessageSortKey result;

    if (p->_valid && p->_fieldOrderList.count()) {
	for (int i = 0; i < p->_fieldOrderList.count(); ++i) {
	    result &= convert(p->_fieldOrderList[i]);
	}
    }

    return result;
}

QMessageAccount convert(const QMailAccount &account)
{
    QMessageAccount result;
    QMessageAccountPrivate *p = QMessageAccountPrivate::implementation(result); 
    QString address = account.fromAddress().address();

    p->_id = convert(account.id());
    p->_name = account.name();
    p->_address = QMessageAddress(QMessageAddress::Email, address);
    p->_types = QMessage::Email;

    return result;
}

QMessageFolder convert(const QMailFolder &folder)
{
    return QMessageFolderPrivate::from(convert(folder.id()),
				       convert(folder.parentAccountId()),
				       convert(folder.parentFolderId()),
				       folder.displayName(),
				       folder.path());
}

QMessage convert(const QMailMessage &message)
{
    return MailMessageConverter(message).convert();
}

QMailMessage convert(const QMessage &message)
{
    return MessageConverter(message).convert();
}

quint64 highPriorityMask()
{
    static quint64 mask(messageStatusMask("QMessage::HighPriority"));
    return mask;
}

quint64 lowPriorityMask()
{
    static quint64 mask(messageStatusMask("QMessage::LowPriority"));
    return mask;
}

}

QTM_END_NAMESPACE
