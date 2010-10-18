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

#include "qfsnmasyncoperations_symbian.h"
#include "qmessagecontentcontainer_symbian_p.h"
#include "qmessage_symbian_p.h"
#include "messagingutil_p.h"


#include <nmapiemailaddress.h>


QTM_BEGIN_NAMESPACE

using namespace SymbianHelpers;

CFSAsynchronousOperation::CFSAsynchronousOperation(AsynchronousOperationType opType):
    m_privateService(0),
    m_operationType(opType),
    m_manager(0)
{

}

CFSAsynchronousOperation::CFSAsynchronousOperation(AsynchronousOperationType opType, QMessageServicePrivate &privateService):
    m_privateService(&privateService),
    m_operationType(opType),
    m_manager(0)
{

}

CFSAsynchronousOperation::~CFSAsynchronousOperation()
{

}

void CFSAsynchronousOperation::createFSMessage(QMessage &message, NmApiMessage &fsMessage)
{
    handleFSMessage(message, fsMessage);

    NmApiMessageEnvelope envelope = fsMessage.envelope();

    QMessagePrivate *privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_id = buildQMessageId(
        envelope.mailboxId(),
        envelope.parentFolder(),
        envelope.id(),
        SymbianHelpers::EngineTypeFreestyle);
    
}

void CFSAsynchronousOperation::handleFSMessage(QMessage &message, NmApiMessage &fsMessage)
{      
    NmApiMessageEnvelope envelope = fsMessage.envelope();

    switch (message.priority()) {
        case QMessage::HighPriority:
            envelope.setPriority(EmailClientApi::NmApiMessagePriorityHigh);
            break;
        case QMessage::NormalPriority:
            envelope.setPriority(EmailClientApi::NmApiMessagePriorityNormal);
            break;
        case QMessage::LowPriority:
            envelope.setPriority(EmailClientApi::NmApiMessagePriorityLow);
            break;            
        }
    
    if (message.status() & QMessage::Read)
        envelope.setIsRead(true);
    else
        envelope.setIsRead(false);
           
    NmApiEmailAddress sender;
    sender.setAddress(message.from().addressee());
    
    envelope.setSender(sender);
        
    QList<QMessageAddress> toList(message.to());
    if (toList.count() > 0) {
        QList<EmailClientApi::NmApiEmailAddress> toRecipients;
        for (int i = 0; i < toList.size(); ++i) {
            NmApiEmailAddress address;
            address.setAddress(toList.at(i).addressee());
            address.setDisplayName(toList.at(i).addressee());
            toRecipients.append(address);
        }
        envelope.setToRecipients(toRecipients);
    }
    
    QList<QMessageAddress> ccList(message.cc());
    if (ccList.count() > 0) {
        QList<EmailClientApi::NmApiEmailAddress> ccRecipients;
        for (int i = 0; i < ccList.size(); ++i) {
            NmApiEmailAddress address;
            address.setAddress(ccList.at(i).addressee());
            address.setDisplayName(ccList.at(i).addressee());
            ccRecipients.append(address);
        }
        envelope.setCcRecipients(ccRecipients);
    }
        
    QList<QMessageAddress> bccList(message.bcc());
    if (bccList.count() > 0) {
        QList<EmailClientApi::NmApiEmailAddress> bccRecipients;
        for (int i = 0; i < bccList.size(); ++i) {
            NmApiEmailAddress address;
            address.setAddress(bccList.at(i).addressee());
            address.setDisplayName(bccList.at(i).addressee());
            bccRecipients.append(address);
        }
        envelope.setBccRecipients(bccRecipients);
    }
    
    if (message.bodyId() == QMessageContentContainerPrivate::bodyContentId()) {
        // Message contains only body (not attachments)
        QString messageBody = message.textContent();
        if (!messageBody.isEmpty()) {
            QByteArray type = message.contentType();
            QByteArray subType = message.contentSubType();
            NmApiTextContent content;
            content.setContent(message.textContent());
            if (type == "text" && subType == "plain")
                fsMessage.setPlainTextContent(content);
            else if (type == "text" && subType == "html")
                fsMessage.setHtmlContent(content);
        }
    } else {
        QMessageContentContainerIdList contentIds = message.contentIds();
        foreach (QMessageContentContainerId id, contentIds){
            QMessageContentContainer container = message.find(id);
            QMessageContentContainerPrivate* pPrivateContainer = QMessageContentContainerPrivate::implementation(container);
            if (message.bodyId() == QMessageContentContainerPrivate::bodyContentId()) {
                // Message contains only body (not attachments)
                QString messageBody = message.textContent();
                if (!messageBody.isEmpty()) {
                    QByteArray type = message.contentType();
                    QByteArray subType = message.contentSubType();
                    NmApiTextContent content;
                    content.setContent(message.textContent());
                    if (type == "text" && subType == "plain")
                        fsMessage.setPlainTextContent(content);
                    else if (type == "text" && subType == "html")
                        fsMessage.setHtmlContent(content);
                }
            } else {
                // Message contains body and attachments
                QMessageContentContainerIdList contentIds = message.contentIds();
                foreach (QMessageContentContainerId id, contentIds){
                    QMessageContentContainer container = message.find(id);
                    QMessageContentContainerPrivate* pPrivateContainer = QMessageContentContainerPrivate::implementation(container);
                    if (pPrivateContainer->_id == message.bodyId()) {
                        // ContentContainer is body
                        if (!container.textContent().isEmpty()) {               
                        QByteArray type = message.contentType();
                        QByteArray subType = message.contentSubType();
                        NmApiTextContent content;
                        content.setContent(container.textContent());
                        if (type == "text" && subType == "plain")
                            fsMessage.setPlainTextContent(content);
                        else if (type == "text" && subType == "html")
                            fsMessage.setHtmlContent(content);
                        }
                    } else {
                        // ContentContainer is attachment
                        if (m_manager) {
                            QByteArray filePath = QMessageContentContainerPrivate::attachmentFilename(container);
                            QVariant attachmentSpec = QVariant(QString(filePath));
                            QEventLoop* eventloop = new QEventLoop();
                            QPointer<NmApiOperation> operation = m_manager->createAttachment(fsMessage, attachmentSpec);
                            connect(operation, SIGNAL(operationCompleted(int)), eventloop, SLOT(quit()));

                            delete eventloop;
                        }
                    }        
                }
            }
        }
    }
    
    envelope.setSubject(message.subject());   
    fsMessage.setEnvelope(envelope);
}

void CFSAsynchronousOperation::operationCompleted(int success)
{
    if (m_privateService)
        m_privateService->setFinished(success);
    emit operationCompleted(success, this);
}

AsynchronousOperationType CFSAsynchronousOperation::operationType()
{
    return m_operationType;
}

/*
 * 
 */

CFSAsynchronousSendOperation::CFSAsynchronousSendOperation(QMessageServicePrivate& privateService):
    CFSAsynchronousOperation(sendOperation, privateService)
{
    
}

CFSAsynchronousSendOperation::~CFSAsynchronousSendOperation()
{
    
}

void CFSAsynchronousSendOperation::sendMessage(QMessage &message)
{
    m_qMessage = message;
    quint64 mailboxId = stripIdPrefix(m_qMessage.parentAccountId().toString()).toULongLong();
    m_manager = new NmApiMessageManager(this, mailboxId);
    createDraftMessage();
}
    
void CFSAsynchronousSendOperation::createDraftMessageCompleted(int success, QVariant message)
{
    if (success == 0) {
        if (message.canConvert<NmApiMessage>()) {
            m_fsMessage = qvariant_cast<NmApiMessage>(message);
            createFSMessage(m_qMessage, m_fsMessage);
            saveMessage();
        }
    }
    else
        operationCompleted(success);
}

void CFSAsynchronousSendOperation::saveCompleted(int success)
{
    if (success == 0)
        sendMessage();
    else
        operationCompleted(success);
}
    
void CFSAsynchronousSendOperation::saveMessage()
{
    QPointer<NmApiOperation> operation = m_manager->saveMessage(m_fsMessage);
    qRegisterMetaType<EmailClientApi::NmApiMessage>("EmailClientApi::NmApiMessage");
    connect(operation, SIGNAL(operationComplete(int)), this, SLOT(saveCompleted(int)));
}
    
void CFSAsynchronousSendOperation::createDraftMessage()
{
    QPointer<NmApiOperation> operation = m_manager->createDraftMessage(NULL);
    qRegisterMetaType<EmailClientApi::NmApiMessage>("EmailClientApi::NmApiMessage");
    connect(operation, SIGNAL(operationComplete(int, QVariant)), this, SLOT(createDraftMessageCompleted(int, QVariant)));
}
    
void CFSAsynchronousSendOperation::sendMessage()
{
    QPointer<NmApiOperation> operation = m_manager->sendMessage(m_fsMessage);
    qRegisterMetaType<EmailClientApi::NmApiMessage>("EmailClientApi::NmApiMessage");
    connect(operation, SIGNAL(operationComplete(int)), this, SLOT(operationComplete(int)));
}

/*
 * 
 */

CFSAsynchronousAddOperation::CFSAsynchronousAddOperation():
    CFSAsynchronousOperation(addOperation)
{
    
}

CFSAsynchronousAddOperation::~CFSAsynchronousAddOperation()
{
    m_manager = NULL;
}

void CFSAsynchronousAddOperation::addMessage(QMessage *message)
{
    m_qMessage = message;
    quint64 mailboxId = stripIdPrefix(m_qMessage->parentAccountId().toString()).toULongLong();
    m_manager = new NmApiMessageManager(this, mailboxId);
    createDraftMessage();
}
    
void CFSAsynchronousAddOperation::createDraftMessageCompleted(int success, QVariant message)
{
    if (success == 0) {
        if (message.canConvert<NmApiMessage>()) {
            m_fsMessage = qvariant_cast<NmApiMessage>(message);
            createFSMessage(*m_qMessage, m_fsMessage);
            saveMessage();
        }
    }
    else
        operationCompleted(success);
}
    
void CFSAsynchronousAddOperation::saveMessage()
{
    QPointer<NmApiOperation> operation = m_manager->saveMessage(m_fsMessage);
    qRegisterMetaType<EmailClientApi::NmApiMessage>("EmailClientApi::NmApiMessage");
    connect(operation, SIGNAL(operationComplete(int)), this, SLOT(operationCompleted(int)));
}
    
void CFSAsynchronousAddOperation::createDraftMessage()
{
    QPointer<NmApiOperation> operation = m_manager->createDraftMessage(NULL);
    qRegisterMetaType<EmailClientApi::NmApiMessage>("EmailClientApi::NmApiMessage");
    connect(operation, SIGNAL(operationComplete(int, QVariant)), this, SLOT(createDraftMessageCompleted(int, QVariant)));    
}

CFSAsynchronousRetrieveBodyOperation::CFSAsynchronousRetrieveBodyOperation(QMessageServicePrivate &privateService):
    CFSAsynchronousOperation(retrieveBodyOperation, privateService)
{
    
}

CFSAsynchronousRetrieveBodyOperation::~CFSAsynchronousRetrieveBodyOperation()
{

}

/*
 * 
 */

void CFSAsynchronousRetrieveBodyOperation::retrieveBody(const QMessageId &messageId)
{
    quint64 mailboxId = 0;
    quint64 folderId = 0;
    quint64 msgId = 0;
    splitQMessageId(messageId, mailboxId, folderId, msgId);
    m_manager = new NmApiMessageManager(this, mailboxId);
    QPointer<NmApiOperation> operation = m_manager->fetchMessage(msgId);
    connect(operation, SIGNAL(operationComplete(int)), this, SLOT(operationCompleted(int)));
}

CFSAsynchronousRetrieveAttachmentOperation::CFSAsynchronousRetrieveAttachmentOperation(NmApiEmailService *emailService, QMessageServicePrivate &privateService):
    CFSAsynchronousOperation(retrieveAttachmentOperation, privateService),
    m_emailService(emailService)
{
    
}

CFSAsynchronousRetrieveAttachmentOperation::~CFSAsynchronousRetrieveAttachmentOperation()
{

}

void CFSAsynchronousRetrieveAttachmentOperation::retrieveAttachment(const QMessageId &messageId, const QMessageContentContainerId &containerId)
{
    quint64 mailboxId = 0;
    quint64 folderId = 0;
    quint64 msgId = 0;
    quint64 attachmentId = containerId.toString().toULongLong();
    splitQMessageId(messageId, mailboxId, folderId, msgId);
    NmApiMessage apiMessage;
    m_emailService->getMessage(mailboxId, folderId, msgId, apiMessage);
    m_manager = new NmApiMessageManager(this, mailboxId);
    QPointer<NmApiOperation> operation = m_manager->fetchAttachment(apiMessage, attachmentId);
    connect(operation, SIGNAL(operationComplete(int)), this, SLOT(operationCompleted(int)));
}


CFSAsynchronousSynchronizeOperation::CFSAsynchronousSynchronizeOperation(NmApiEmailService *emailService, QMessageServicePrivate &privateService):
    CFSAsynchronousOperation(synchronizeOperation, privateService),
    m_emailService(emailService)
{

}

CFSAsynchronousSynchronizeOperation::~CFSAsynchronousSynchronizeOperation()
{

}

void CFSAsynchronousSynchronizeOperation::syncronizeMailbox(const QMessageAccountId &id)
{
    quint64 mailboxId = stripIdPrefix(id.toString()).toULongLong();
    QPointer<NmApiOperation> operation = m_emailService->synchronizeMailbox(mailboxId);
    connect(operation, SIGNAL(operationComplete(int)), this, SLOT(operationCompleted(int)));
}

/*
 * 
 */

CFSAsynchronousUpdateOperation::CFSAsynchronousUpdateOperation(NmApiEmailService *emailService):
    CFSAsynchronousOperation(updateOperation),
    m_emailService(emailService)
{
    
}

CFSAsynchronousUpdateOperation::~CFSAsynchronousUpdateOperation()
{
    m_manager = NULL;
}

void CFSAsynchronousUpdateOperation::updateMessage(QMessage &message)
{
    quint64 mailboxId = stripIdPrefix(message.parentAccountId().toString()).toULongLong();
    m_manager = new NmApiMessageManager(this, mailboxId);
    NmApiMessage apiMessage = updateFsMessage(message);
    QPointer<NmApiOperation> saveOperation = m_manager->saveMessage(apiMessage);
    qRegisterMetaType<EmailClientApi::NmApiMessage>("EmailClientApi::NmApiMessage");
    connect(saveOperation, SIGNAL(operationComplete(int, QVariant)), this, SLOT(operationCompleted(int)));
}

NmApiMessage CFSAsynchronousUpdateOperation::updateFsMessage(QMessage &message)
{
    quint64 messageId = stripIdPrefix(message.id().toString()).toULongLong();
    quint64 mailboxId = stripIdPrefix(message.parentAccountId().toString()).toLongLong();
    quint64 folderId = stripIdPrefix(message.parentFolderId().toString()).toLongLong();
    NmApiMessage fsMessage;
    m_emailService->getMessage(mailboxId, folderId, messageId, fsMessage);

    handleFSMessage(message, fsMessage);
    return fsMessage;
}

/*
 * 
 */

CFSAsynchronousRemoveOperation::CFSAsynchronousRemoveOperation(NmApiEmailService *emailService):
    CFSAsynchronousOperation(removeOperation),
    m_emailService(emailService)
{
    
}

CFSAsynchronousRemoveOperation::~CFSAsynchronousRemoveOperation()
{
    m_manager = NULL;
}

void CFSAsynchronousRemoveOperation::removeMessage(const QMessageId &messageId)
{
    QList<quint64> messageIds;
    quint64 mailboxId = 0;
    quint64 folderId = 0;
    quint64 msgId = 0;
    splitQMessageId(messageId, mailboxId, folderId, msgId);
    messageIds.append(msgId);

    NmApiMessageManager* manager = new NmApiMessageManager(this, mailboxId);
    QPointer<NmApiOperation> deleteOperation = manager->deleteMessages(messageIds);
    connect(deleteOperation, SIGNAL(operationComplete(int)), this, SLOT(operationCompleted(int)));
}

#include "moc_qfsnmasyncoperations_symbian.cpp";

QTM_END_NAMESPACE
