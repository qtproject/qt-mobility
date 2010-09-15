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

#include "qmessageservice.h"
#include "qmessageservice_symbian_p.h"
#include "qfsnmengine_symbian_p.h"
#include "qmessage_symbian_p.h"
#include "messagingutil_p.h"
#include "qmessageaccount.h"
#include "qmessageaccount_p.h"
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"
#include "qmessagecontentcontainer_symbian_p.h"
#include "qmessagefolder.h"
#include "qmessagefolder_p.h"
#include "qmessagefolderfilter.h"
#include "qmessagefolderfilter_p.h"
#include "qmessageaccountsortorder_p.h"
#include "qmessagestore_symbian_p.h"
#include "qmessagefoldersortorder_p.h"
#include "qmessagesortorder_p.h"

#include <nmapifolderlisting.h>
#include <nmapiemailaddress.h>

#include <nmapimessage.h>

#include <xqaiwdecl.h>
#include <xqaiwdeclplat.h>
#include <xqaiwrequest.h>
#include <xqservicerequest.h>
#include "email_services_api.h"

#include <QTextCodec>
#include <QEventLoop>
#include <QTimer>

using namespace EmailClientApi;

QTM_BEGIN_NAMESPACE

using namespace SymbianHelpers;

Q_GLOBAL_STATIC(CFSEngine,fsEngine);

void createFSMessage(QMessage &message, NmApiMessage &fsMessage)
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
           
    envelope.setSender(message.from().addressee());
        
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
                else
                    envelope.setPlainText(messageBody);
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
                    else
                        envelope.setPlainText(container.textContent());
                    }
                } else {
                    // ContentContainer is attachment
                    QByteArray filePath = QMessageContentContainerPrivate::attachmentFilename(container);
                    NmApiAttachment attachment;
                    QString temp_path = QString(filePath);
                    attachment.setFileName(temp_path);
                    fsMessage.addAttachment(attachment);
                }        
            }
        }
    }
    envelope.setSubject(message.subject());   
    fsMessage.setEnvelope(envelope);
    
    QMessagePrivate *privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_id = QMessageId(addIdPrefix(QString::number(envelope.id()), SymbianHelpers::EngineTypeFreestyle));
}

CFSEngine::CFSEngine()
{
    startEventObserver();
    m_emailService = new NmApiEmailService(this);
    connect(m_emailService, SIGNAL(initialized(bool)), this, SLOT(emailServiceInitialized(bool)));
    m_emailService->initialise();
}

CFSEngine::~CFSEngine()
{
    while (!m_sendList.isEmpty())
        delete m_sendList.takeFirst();
    
    while (!m_addList.isEmpty())
        delete m_addList.takeFirst();
    
    if (m_emailServiceInitialized)
        m_emailService->uninitialise();
    
    cancelEventObserver();
}

CFSEngine* CFSEngine::instance()
{   
    return fsEngine();
}

void CFSEngine::emailServiceInitialized(bool initialized)
{
    m_emailServiceInitialized = initialized;
}

void CFSEngine::startEventObserver()
{   
    m_eventNotifier = new NmApiEventNotifier(this);
    qRegisterMetaType<EmailClientApi::NmApiMessageEvent>("EmailClientApi::NmApiMessageEvent");
    connect(m_eventNotifier, SIGNAL(messageEvent(EmailClientApi::NmApiMessageEvent, quint64, quint64, QList<quint64>)),this,
                     SLOT(messageEvent(EmailClientApi::NmApiMessageEvent, quint64, quint64, QList<quint64>)));
    m_eventNotifier->start();
    bool running;
    if (m_eventNotifier->isRunning())
        running = true;
    else
        running = false;
}

void CFSEngine::messageEvent(EmailClientApi::NmApiMessageEvent event, quint64 mailboxId, quint64 folderId, QList<quint64> envelopeIdList)
{
    switch(event) {
        case EmailClientApi::MessageCreated:
            newMessageEvent(mailboxId, envelopeIdList, folderId);
            break;
        case EmailClientApi::MessageChanged:
            messageChangedEvent(mailboxId, envelopeIdList, folderId);
            break;
        case EmailClientApi::MessageDeleted:
            messageDeletedEvent(mailboxId, envelopeIdList, folderId);
            break;
    }
}

void CFSEngine::newMessageEvent(quint64 mailboxId, QList<quint64> envelopeIdList, quint64 folderId)
{
    QMessageManager::NotificationFilterIdSet matchingFilters;
    QMessageStorePrivate::NotificationType notificationType = QMessageStorePrivate::Added;
  
    for (TInt i = 0; i < envelopeIdList.count(); i++) {
        quint64 messageId(envelopeIdList[i]);
        notification(mailboxId, messageId, folderId, notificationType);
    }
}

void CFSEngine::messageChangedEvent(quint64 mailboxId, QList<quint64> envelopeIdList, quint64 folderId)
{
    QMessageManager::NotificationFilterIdSet matchingFilters;
    QMessageStorePrivate::NotificationType notificationType = QMessageStorePrivate::Updated;
  
    for (TInt i = 0; i < envelopeIdList.count(); i++) {
        quint64 messageId(envelopeIdList[i]);
        notification(mailboxId, messageId, folderId, notificationType);
    }
}

void CFSEngine::messageDeletedEvent(quint64 mailboxId, QList<quint64> envelopeIdList, quint64 folderId)
{
    QMessageManager::NotificationFilterIdSet matchingFilters;
    QMap<int, QMessageFilter> filters(m_filters);
    QMap<int, QMessageFilter>::const_iterator it = filters.begin(), end = filters.end();  
    QMessageStorePrivate::NotificationType notificationType = QMessageStorePrivate::Removed;
    // TODO: convert mailboxId
    QString idAsString = QString::number(mailboxId);
    for (TInt j = 0; j < m_mtmAccountList.count(); j++) {
        if (idAsString == m_mtmAccountList[j].toString())
            return;
    } 
    for (TInt i = 0; i < envelopeIdList.count(); i++) {
        for ( ; it != end; ++it) {
            // Empty filter matches to all messages
            matchingFilters.insert(it.key());
        }
        quint64 messageId(envelopeIdList[i]);
        ipMessageStorePrivate->messageNotification(notificationType, 
                            QMessageId(addIdPrefix(QString::number(messageId), SymbianHelpers::EngineTypeFreestyle)), 
                            matchingFilters);
    }
}

void CFSEngine::notification(quint64 mailboxId, quint64 envelopeId, quint64 folderId, 
                                    QMessageStorePrivate::NotificationType aNotificationType)
{
    Q_UNUSED(folderId);
    QMessageManager::NotificationFilterIdSet matchingFilters;
    // Copy the filter map to protect against modification during traversal
    QMap<int, QMessageFilter> filters(m_filters);
    QMap<int, QMessageFilter>::const_iterator it = filters.begin(), end = filters.end();
    QMessage message;
    QMessageId realMessageId = QMessageId(addIdPrefix(QString::number(mailboxId), SymbianHelpers::EngineTypeFreestyle));
    bool messageRetrieved = false;    
    // TODO: convert mailboxId
    QString idAsString = QString::number(mailboxId);
    for (TInt j = 0; j < m_mtmAccountList.count(); j++) {
        if (idAsString == m_mtmAccountList[j].toString()) {
            return;
        }
    }   
    for ( ; it != end; ++it) {
        const QMessageFilter &filter(it.value());
        if (!messageRetrieved) {
            NmApiMessage fsMessage;
            int err;
            if (m_emailServiceInitialized)
                m_emailService->getMessage(mailboxId, folderId, envelopeId, fsMessage);
               
            message = CreateQMessage(&fsMessage);
            messageRetrieved = true;
        }

        if (filter.isEmpty()) {
            // Empty filter matches to all messages
            matchingFilters.insert(it.key());
        } else {
            if (message.type() == QMessage::NoType) {
                matchingFilters.clear();
                continue;
            }
        }
        QMessageFilterPrivate* privateMessageFilter = QMessageFilterPrivate::implementation(filter);
        if (privateMessageFilter->filter(message)) {
            matchingFilters.insert(it.key());
        }
        
    }
    int c = matchingFilters.count();
    QString id = realMessageId.toString();
    if (matchingFilters.count() > 0)
        ipMessageStorePrivate->messageNotification(aNotificationType, realMessageId, matchingFilters);
}


void CFSEngine::cancelEventObserver()
{
    if (m_eventNotifier) {
        m_eventNotifier->cancel();
        delete m_eventNotifier;
    }
}

bool CFSEngine::accountLessThan(const QMessageAccountId accountId1, const QMessageAccountId accountId2)
{
    CFSEngine* freestyleEngine = fsEngine();
    return QMessageAccountSortOrderPrivate::lessThan(freestyleEngine->m_currentAccountOrdering,
        freestyleEngine->account(accountId1),
        freestyleEngine->account(accountId2));
}

void CFSEngine::orderAccounts(QMessageAccountIdList &accountIds, const QMessageAccountSortOrder &sortOrder) const
{
    Q_UNUSED(accountIds);
    m_currentAccountOrdering = sortOrder;
    qSort(accountIds.begin(), accountIds.end(), CFSEngine::accountLessThan);
}

bool CFSEngine::folderLessThan(const QMessageFolderId folderId1, const QMessageFolderId folderId2)
{
    CFSEngine* freestyleEngine = fsEngine();
    return QMessageFolderSortOrderPrivate::lessThan(freestyleEngine->m_currentFolderOrdering,
            freestyleEngine->folder(folderId1),
            freestyleEngine->folder(folderId2));
}

void CFSEngine::orderFolders(QMessageFolderIdList &folderIds,  const QMessageFolderSortOrder &sortOrder) const
{
    m_currentFolderOrdering = sortOrder;
    qSort(folderIds.begin(), folderIds.end(), CFSEngine::folderLessThan);
}

bool CFSEngine::messageLessThan(const QMessage &message1, const QMessage &message2)
{
    CFSEngine *freestyleEngine = fsEngine();
    return QMessageSortOrderPrivate::lessThan(freestyleEngine->m_currentMessageOrdering, message1, message2);
}

void CFSEngine::orderMessages(QMessageIdList &messageIds, const QMessageSortOrder &sortOrder) const
{
    m_currentMessageOrdering = sortOrder;
    QList<QMessage> messages;
    for (int i=0; i < messageIds.count(); i++) {
        messages.append(message(messageIds[i]));
    }
    qSort(messages.begin(), messages.end(), CFSEngine::messageLessThan);
    messageIds.clear();
    for (int i=0; i < messages.count(); i++) {
        messageIds.append(messages[i].id());
    }
}

void CFSEngine::setMtmAccountIdList(QMessageAccountIdList accountList)
{
    m_mtmAccountList.clear();
    for (TInt i = 0; i < accountList.count(); i++) {
        m_mtmAccountList.append(stripIdPrefix(accountList[i]));
    }
}

QMessageAccountIdList CFSEngine::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageAccountIdList accountIds;

    updateEmailAccounts();
    
    QMessageAccountFilterPrivate* privateMessageAccountFilter = QMessageAccountFilterPrivate::implementation(filter);
    if (filter.isEmpty()) {
        if (!privateMessageAccountFilter->_notFilter) {
            // All accounts are returned for empty filter
            foreach (QMessageAccount value, m_accounts) {                
                accountIds.append(value.id());
            }
        }
    } else {
        if (privateMessageAccountFilter->_valid) {
            foreach (QMessageAccount value, m_accounts) {
                if (privateMessageAccountFilter->filter(value)) {
                    accountIds.append(value.id());
                }
            }
        } else {
            foreach (QMessageAccount value, m_accounts) {
                if (privateMessageAccountFilter->filter(value)) {
                    accountIds.append(value.id());
                }
            }
        }
    }
    
    if (!sortOrder.isEmpty()) {
        orderAccounts(accountIds, sortOrder);
    }

    applyOffsetAndLimitToAccountIds(accountIds, offset, limit);

    return accountIds;
}

void CFSEngine::applyOffsetAndLimitToAccountIds(QMessageAccountIdList &idList, int offset, int limit) const
{
    if (offset > 0) {
        if (offset > idList.count()) {
            idList.clear();
        } else {
            for (int i = 0; i < offset; i++) {
                idList.removeFirst();
            }
        }
    }
    if (limit > 0) {
        for (int i = idList.count()-1; i >= limit; i--) {
            idList.removeAt(i);
        }
    }
}

int CFSEngine::countAccounts(const QMessageAccountFilter &filter) const
{
    return queryAccounts(filter, QMessageAccountSortOrder(), 0, 0).count();
}

QMessageAccount CFSEngine::account(const QMessageAccountId &id) const
{
    updateEmailAccounts();

    return m_accounts[id.toString()];
}

QMessageAccountId CFSEngine::defaultAccount(QMessage::Type type) const
{
    if (type == QMessage::Email) {
        updateEmailAccounts();
        QMessageAccountIdList accountIds = accountsByType(type);
        if (accountIds.count() > 0)
            return accountIds.at(0);
    }
    return QMessageAccountId();
}

QMessageAccountIdList CFSEngine::accountsByType(QMessage::Type type) const
{
    QMessageAccountIdList accountIds = QMessageAccountIdList();
    
    foreach (QMessageAccount value, m_accounts) {
        if ((value.messageTypes() & type) == (int)type) {
            accountIds.append(value.id());
        }
    }
    
    return accountIds;
}

void CFSEngine::updateEmailAccounts() const
{
    QStringList keys = m_accounts.keys();

    NmApiMailboxListing* mailboxlisting = new NmApiMailboxListing(0);       
    QEventLoop* eventloop = new QEventLoop();
    connect(mailboxlisting, SIGNAL(mailboxesListed(qint32)), eventloop, SLOT(quit()));
    mailboxlisting->cancel();
    mailboxlisting->start();
    eventloop->exec();

    QList<NmApiMailbox> mailboxes;
    mailboxlisting->getMailboxes(mailboxes);
    int count = mailboxes.count();
    
    for (TInt i = 0; i < mailboxes.count(); i++) {
        NmApiMailbox mailbox = mailboxes[i];
        QString idAsString = QString::number(mailbox.id());
        QString fsIdAsString = addIdPrefix(idAsString, SymbianHelpers::EngineTypeFreestyle);
        QString name = mailbox.name();
        TBool overlap = false;
        for (TInt j = 0; j < m_mtmAccountList.count(); j++) {
            QString mtmId = m_mtmAccountList[j].toString();
            if (idAsString == m_mtmAccountList[j].toString())
                overlap = true;
        }
        if (!m_accounts.contains(fsIdAsString) && !overlap) {     
            QMessageAccount account = QMessageAccountPrivate::from(
                                      QMessageAccountId(fsIdAsString),
                                      mailbox.name(),
                                      0,
                                      0,
                                      QMessage::Email);
          
            m_accounts.insert(fsIdAsString, account);
            
        } else {
            keys.removeOne(fsIdAsString);
        }
    }  
    
    for (int i=0; i < keys.count(); i++) {
        m_accounts.remove(keys[i]);
    }
    delete eventloop;
    //delete mailboxlisting; //TODO: Deleting causes KERN-EXEC 3
}

bool CFSEngine::sendEmail(QMessageServicePrivate &privateService, QMessage &message)
{
    CFSAsynchronousSendOperation *sendOperation = NULL;
    try{
        sendOperation = new CFSAsynchronousSendOperation();
    }
    catch(...){
        return false;
    }
    connect(sendOperation, SIGNAL(messageSend(int, CFSAsynchronousSendOperation*)), this, 
            SLOT(sendCompleted(int, CFSAsynchronousSendOperation*)));
    m_sendList.append(sendOperation);
    sendOperation->sendMessage(privateService, message);

    return true;
}

void CFSEngine::sendCompleted(int success, CFSAsynchronousSendOperation *operation)
{
    m_sendList.removeAt(m_sendList.indexOf(operation));
    delete operation;   
}

bool CFSEngine::addMessage(QMessage *message)
{
    m_addMessageError = false;
    CFSAsynchronousAddOperation *addOperation = NULL;
    try{
        addOperation = new CFSAsynchronousAddOperation();
    }
    catch(...){
        return false;
    }

    connect(addOperation, SIGNAL(messageAdded(int, CFSAsynchronousAddOperation*)), this, 
                SLOT(addMessageCompleted(int, CFSAsynchronousAddOperation*)));
    m_addList.append(addOperation);    
    QEventLoop* eventloop = new QEventLoop();
    connect(addOperation, SIGNAL(messageAdded(int, CFSAsynchronousAddOperation*)), eventloop, SLOT(quit()));
    addOperation->addMessage(*message);
    
    eventloop->exec();  
    delete eventloop;
    if (!m_addMessageError)
        return true;
    else
        return false;
}

void CFSEngine::addMessageCompleted(int success, CFSAsynchronousAddOperation *operation)
{
    m_addList.removeAt(m_addList.indexOf(operation));
    delete operation; 
    if (success == 0)
        m_addMessageError = false;
    else
        m_addMessageError = true;
}

bool CFSEngine::updateMessage(QMessage *message)
{
    NmApiMessage fsMessage = updateFsMessage(message);
    quint64 mailboxId = stripIdPrefix(message->parentAccountId().toString()).toULongLong();
    NmApiMessageManager* manager = new NmApiMessageManager(0, mailboxId);
    QPointer<NmApiOperation> saveOperation = manager->saveMessage(fsMessage);
    QEventLoop* eventloop = new QEventLoop();
    qRegisterMetaType<EmailClientApi::NmApiMessage>("EmailClientApi::NmApiMessage");
    connect(saveOperation, SIGNAL(operationComplete(int, QVariant)), this, SLOT(saveCompleted(QVariant, int)));
    connect(saveOperation, SIGNAL(operationComplete(int, QVariant)), eventloop, SLOT(quit()));
    eventloop->exec();
    
    delete manager;
    delete eventloop;
    
    if (m_updateMessageError)
        return false;
    else 
        return true;
}

NmApiMessage CFSEngine::updateFsMessage(QMessage *message)
{
    m_updateMessageError = false;
    quint64 messageId = stripIdPrefix(message->id().toString()).toULongLong();
    quint64 mailboxId = stripIdPrefix(message->parentAccountId().toString()).toLongLong();
    quint64 folderId = stripIdPrefix(message->parentFolderId().toString()).toLongLong();
    NmApiMessage fsMessage = this->message(mailboxId, folderId, messageId);
    NmApiMessageEnvelope envelope = fsMessage.envelope();
    
    switch (message->priority()) {
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
    if (message->status() & QMessage::Read) {
        envelope.setIsRead(true);
    } else {
        envelope.setIsRead(false);
    }
        
    envelope.setSender(message->from().addressee());
    
    QList<QMessageAddress> toList(message->to());
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
    
    QList<QMessageAddress> ccList(message->cc());
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
        
    QList<QMessageAddress> bccList(message->bcc());
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
    
    QMessageContentContainerIdList contentIds = message->contentIds();
    foreach (QMessageContentContainerId id, contentIds){
        QMessageContentContainer container = message->find(id);
        QMessageContentContainerPrivate* pPrivateContainer = QMessageContentContainerPrivate::implementation(container);
        if (message->bodyId() == QMessageContentContainerPrivate::bodyContentId()) {
            // Message contains only body (not attachments)
            QString messageBody = message->textContent();
            if (!messageBody.isEmpty()) {
                QByteArray type = message->contentType();
                QByteArray subType = message->contentSubType();
                NmApiTextContent content;
                content.setContent(message->textContent());
                if (type == "text" && subType == "plain")
                    fsMessage.setPlainTextContent(content);
                else if (type == "text" && subType == "html")
                    fsMessage.setHtmlContent(content);
                else
                    envelope.setPlainText(messageBody);
            }
        } else {
            // Message contains body and attachments
            QMessageContentContainerIdList contentIds = message->contentIds();
            foreach (QMessageContentContainerId id, contentIds){
                QMessageContentContainer container = message->find(id);
                QMessageContentContainerPrivate* pPrivateContainer = QMessageContentContainerPrivate::implementation(container);
                if (pPrivateContainer->_id == message->bodyId()) {
                    // ContentContainer is body
                    if (!container.textContent().isEmpty()) {               
                    QByteArray type = message->contentType();
                    QByteArray subType = message->contentSubType();
                    NmApiTextContent content;
                    content.setContent(container.textContent());
                    if (type == "text" && subType == "plain")
                        fsMessage.setPlainTextContent(content);
                    else if (type == "text" && subType == "html")
                        fsMessage.setHtmlContent(content);
                    else
                        envelope.setPlainText(container.textContent());
                    }
                } else {
                    // ContentContainer is attachment
                    QByteArray filePath = QMessageContentContainerPrivate::attachmentFilename(container);
                    // Replace Qt style path separator "/" with Symbian path separator "\"
                    //filePath.replace(QByteArray("/"), QByteArray("\\"));
                    NmApiAttachment attachment;
                    QString temp_path = QString(filePath);
                    attachment.setFileName(temp_path);
                    fsMessage.addAttachment(attachment);
                }        
            }
        }
    }
    envelope.setSubject(message->subject());
    
    //fsMessage.setEnvelope(envelope); is this needed?
    
    return fsMessage;
}

void CFSEngine::saveCompleted(QVariant variant, int success)
{
    if (success == 0)
        m_updateMessageError = false;
    else
        m_updateMessageError = true;
}

NmApiMessage CFSEngine::message(const quint64 mailboxId, const quint64 folderId, const quint64 messageId) const
{
    NmApiMessage fsMessage;
    if (m_emailServiceInitialized)
        m_emailService->getMessage(mailboxId, folderId, messageId, fsMessage);
    return fsMessage;
}

QMessage CFSEngine::message(const QMessageId &id) const
{
    // TODO:
    /* Use NmApiEmailService::getMessage() to find NmApiMessage by messageId.
    fsMessage = ...
    conver fsMessage to qmessage, using CreateQMessage()
*/
    return QMessage();
}

bool CFSEngine::removeMessage(const QMessageId &id, QMessageManager::RemovalOption /*option*/)
{
    m_deleteMessageError = false;
    QList<quint64> messageIds;
    quint64 messageId = stripIdPrefix(id.toString()).toULongLong();
    messageIds.append(messageId);

    QMessage qmessage = message(id);
    quint64 mailboxId = stripIdPrefix(qmessage.parentAccountId().toString()).toULongLong();
    NmApiMessageManager* manager = new NmApiMessageManager(this, mailboxId);
    QPointer<NmApiOperation> deleteOperation = manager->deleteMessages(messageIds);
    QEventLoop* eventloop = new QEventLoop();

    connect(deleteOperation, SIGNAL(operationComplete(int, QVariant)), this, SLOT(deleteCompleted(QVariant, int)));
    connect(deleteOperation, SIGNAL(operationComplete(int, QVariant)), eventloop, SLOT(quit()));
    eventloop->exec();
    
    delete manager;
    delete eventloop;
    
    if (m_deleteMessageError)
        return false;
    else 
        return true;
}

void CFSEngine::deleteCompleted(QVariant variant, int success)
{
    if (success == 0)
        m_deleteMessageError = false;
    else
        m_deleteMessageError = true;
}

bool CFSEngine::showMessage(const QMessageId &id)
{
    bool syncronous;
        
    XQAiwRequest* request = m_applicationManager.create(emailInterfaceNameMessage,
            emailOperationViewMessage,
            syncronous);
  
    QMessage qmessage = message(id);
   
    QList<QVariant> list;
    list.append(stripIdPrefix(qmessage.parentAccountId().toString()).toLongLong());
    list.append(stripIdPrefix(qmessage.parentFolderId().toString()).toLongLong());
    list.append(stripIdPrefix(qmessage.id().toString()).toLongLong());
    request->setArguments(list);

    QVariant returnValue;
    bool rval = request->send(returnValue);
    return rval;
}

bool CFSEngine::composeMessage(const QMessage &message)
{
    bool embedded = false;
    
    XQAiwRequest* request = m_applicationManager.create(XQI_EMAIL_MESSAGE_SEND,
            "send(QVariant)",//XQOP_EMAIL_MESSAGE_SEND,
            embedded);
    
    QMap<QString,QVariant> map;

    // Add receivers
    QStringList toRecipients;
    QStringList ccRecipients;
    QStringList bccRecipients;

    foreach(QMessageAddress address, message.to())
        toRecipients.append(address.addressee());

    foreach(QMessageAddress address, message.cc())
        ccRecipients.append(address.addressee());

    foreach(QMessageAddress address, message.bcc())
        bccRecipients.append(address.addressee());

    map.insert(emailSendToKey, toRecipients);
    map.insert(emailSendCcKey, ccRecipients);
    map.insert(emailSendBccKey, bccRecipients);
    map.insert(emailSendSubjectKey, message.subject());
    
    // body and attachments not yet supported
    map.insert(emailSendBodyTextKey, message.textContent());
    
    QList<QVariant> data;
    data.append(map);
    request->setArguments(data);
        
    bool res = request->send();
   
    return res;
}

bool CFSEngine::retrieve(QMessageServicePrivate &privateService, const QMessageId &messageId, const QMessageContentContainerId &id)
{
    Q_UNUSED(id);
    m_privateService = &privateService;
    bool retVal = false;
    return retVal;
}

bool CFSEngine::retrieveBody(QMessageServicePrivate &privateService, const QMessageId &id)
{
    bool retVal = false;
    m_privateService = &privateService;
    return retVal;
}

bool CFSEngine::retrieveHeader(QMessageServicePrivate &privateService, const QMessageId &id)
{
    Q_UNUSED(id);
    Q_UNUSED(privateService);
    return false;
}

bool CFSEngine::exportUpdates(const QMessageAccountId &id)
{
 /* TODO: ?  
    quint64 mailboxId = stripIdPrefix(id.toString()).toULongLong();
    if (m_emailServiceInitialized){
        m_emailService->synchroniseMailbox(mailboxId);
        return true;
    } else
        return false;*/
}

bool CFSEngine::removeMessages(const QMessageFilter& /*filter*/, QMessageManager::RemovalOption /*option*/)
{
    return false;
}

void CFSEngine::handleNestedFiltersFromMessageFilter(QMessageFilter &filter) const
{
    QMessageFilterPrivate* pMFFilter = QMessageFilterPrivate::implementation(filter);
    if (pMFFilter->_filterList.count() > 0) {
        int filterListCount = pMFFilter->_filterList.count();
        for (int i=0; i < filterListCount; i++) {
            for (int j=0; j < pMFFilter->_filterList[i].count(); j++) {
                QMessageFilterPrivate* pMFFilter2 = QMessageFilterPrivate::implementation(pMFFilter->_filterList[i][j]);
                if (pMFFilter2->_field == QMessageFilterPrivate::ParentAccountIdFilter) {
                    QMessageAccountIdList accountIds = queryAccounts(*pMFFilter2->_accountFilter, QMessageAccountSortOrder(), 0, 0);
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter2->_comparatorValue));
                    if (accountIds.count() > 0) {
                        pMFFilter->_filterList[i].removeAt(j);
                        if (cmp == QMessageDataComparator::Includes) {
                            for (int x = 0; x < accountIds.count(); x++) {
                                if (x == 0) {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[i]);
                                    }
                                    pMFFilter->_filterList[i].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                    qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                                } else {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[pMFFilter->_filterList.count()-1]);
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-2].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-2].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-2].end(), QMessageFilterPrivate::lessThan);
                                    } else {
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-1].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-1].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-1].end(), QMessageFilterPrivate::lessThan);
                                    }
                                }
                            }
                        } else { // Excludes
                            for (int x = 0; x < accountIds.count(); x++) {
                                pMFFilter->_filterList[i].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::NotEqual));
                            }
                            qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                        }
                    } else {
                        delete pMFFilter2->_accountFilter;
                        pMFFilter2->_accountFilter = 0;
                        pMFFilter2->_field = QMessageFilterPrivate::Id;
                        qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                    }
                } else if (pMFFilter2->_field == QMessageFilterPrivate::ParentFolderIdFilter) { 
                    QMessageFolderIdList folderIds = queryFolders(*pMFFilter2->_folderFilter, QMessageFolderSortOrder(), 0, 0);
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter2->_comparatorValue));
                    if (folderIds.count() > 0) {
                        pMFFilter->_filterList[i].removeAt(j);
                        if (cmp == QMessageDataComparator::Includes) {
                            for (int x = 0; x < folderIds.count(); x++) {
                                if (x == 0) {
                                    if (x+1 < folderIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[i]);
                                    }
                                    pMFFilter->_filterList[i].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::Equal));
                                    qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                                } else {
                                    if (x+1 < folderIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[pMFFilter->_filterList.count()-1]);
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-2].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-2].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-2].end(), QMessageFilterPrivate::lessThan);
                                    } else {
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-1].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-1].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-1].end(), QMessageFilterPrivate::lessThan);
                                    }
                                }
                            }
                        } else { // Excludes
                            for (int x = 0; x < folderIds.count(); x++) {
                                pMFFilter->_filterList[i].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::NotEqual));
                            }
                            qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                        }
                    } else {
                        delete pMFFilter2->_folderFilter;
                        pMFFilter2->_folderFilter = 0;
                        pMFFilter2->_field = QMessageFilterPrivate::Id;
                        qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                    }
                } else {
                    break;
                }
            }
        }
    } else {
        if (pMFFilter->_field == QMessageFilterPrivate::ParentAccountIdFilter) {
            QMessageAccountIdList accountIds = queryAccounts(*pMFFilter->_accountFilter, QMessageAccountSortOrder(), 0, 0);
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter->_comparatorValue));
            if (accountIds.count() > 0) {
                for (int i=0; i < accountIds.count(); i++) {
                    if (i == 0) {
                        delete pMFFilter->_accountFilter;
                        pMFFilter->_accountFilter = 0;
                        pMFFilter->_field = QMessageFilterPrivate::ParentAccountId;
                        pMFFilter->_value = accountIds[0].toString();
                        pMFFilter->_comparatorType = QMessageFilterPrivate::Equality;
                        if (cmp == QMessageDataComparator::Includes) {
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
                        } else { // Excludes
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
                        }
                    } else {
                        if (cmp == QMessageDataComparator::Includes) {
                            filter |= QMessageFilter::byParentAccountId(accountIds[i],QMessageDataComparator::Equal);
                        } else { // Excludes
                            filter &= QMessageFilter::byParentAccountId(accountIds[i],QMessageDataComparator::NotEqual);
                        }
                    }
                }
            } else {
                delete pMFFilter->_accountFilter;
                pMFFilter->_accountFilter = 0;
                pMFFilter->_field = QMessageFilterPrivate::Id;
            }
        } else if (pMFFilter->_field == QMessageFilterPrivate::ParentFolderIdFilter) {
            QMessageFolderIdList folderIds = queryFolders(*pMFFilter->_folderFilter, QMessageFolderSortOrder(), 0, 0);
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter->_comparatorValue));
            if (folderIds.count() > 0) {
                for (int i=0; i < folderIds.count(); i++) {
                    if (i == 0) {
                        delete pMFFilter->_folderFilter;
                        pMFFilter->_folderFilter = 0;
                        pMFFilter->_field = QMessageFilterPrivate::ParentFolderId;
                        pMFFilter->_value = folderIds[0].toString();
                        pMFFilter->_comparatorType = QMessageFilterPrivate::Equality;
                        if (cmp == QMessageDataComparator::Includes) {
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
                        } else { // Excludes
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
                        }
                    } else {
                        if (cmp == QMessageDataComparator::Includes) {
                            filter |= QMessageFilter::byParentFolderId(folderIds[i],QMessageDataComparator::Equal);
                        } else { // Excludes
                            filter &= QMessageFilter::byParentFolderId(folderIds[i],QMessageDataComparator::NotEqual);
                        }
                    }
                }
            } else {
                delete pMFFilter->_folderFilter;
                pMFFilter->_folderFilter = 0;
                pMFFilter->_field = QMessageFilterPrivate::Id;
            }
        }
    }
}

bool CFSEngine::queryMessages(QMessageServicePrivate &privateService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    TRAPD(err, queryMessagesL(privateService, filter, sortOrder, limit, offset));
    if (err != KErrNone) {
        return false;
    }
    return true;
}


void CFSEngine::queryMessagesL(QMessageServicePrivate &privateService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    
    FSMessageQueryInfo queryInfo;
    queryInfo.operationId = ++m_operationIds;
    if (queryInfo.operationId == 100000) {
        queryInfo.operationId = 1;
    }
    queryInfo.isQuery = true;
    queryInfo.filter = filter;
    queryInfo.sortOrder = sortOrder;
    queryInfo.offset = offset;
    queryInfo.limit = limit;
    queryInfo.findOperation = new CFSMessagesFindOperation((CFSEngine&)*this, queryInfo.operationId);
    queryInfo.privateService = &privateService;
    queryInfo.currentFilterListIndex = 0;
    m_messageQueries.append(queryInfo);

    handleNestedFiltersFromMessageFilter(m_messageQueries[m_messageQueries.count()-1].filter);
    
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(m_messageQueries[m_messageQueries.count()-1].filter);
    if (pf->_filterList.count() == 0) {
        queryInfo.findOperation->filterAndOrderMessages(m_messageQueries[m_messageQueries.count()-1].filter,
            m_messageQueries[m_messageQueries.count()-1].sortOrder);
    } else {
        queryInfo.findOperation->filterAndOrderMessages(pf->_filterList[0], m_messageQueries[m_messageQueries.count()-1].sortOrder);
    }
}

bool CFSEngine::queryMessages(QMessageServicePrivate &privateService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    TRAPD(err, queryMessagesL(privateService, filter, body, matchFlags, sortOrder, limit, offset));
    if (err != KErrNone) {
        return false;
    }
    return true;
}

void CFSEngine::queryMessagesL(QMessageServicePrivate &privateService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    FSMessageQueryInfo queryInfo;
    queryInfo.operationId = ++m_operationIds;
    if (queryInfo.operationId == 100000) {
        queryInfo.operationId = 1;
    }
    queryInfo.isQuery = true;
    queryInfo.body = body;
    queryInfo.matchFlags = matchFlags;
    queryInfo.filter = filter;
    queryInfo.sortOrder = sortOrder;
    queryInfo.offset = offset;
    queryInfo.limit = limit;
    queryInfo.findOperation = new CFSMessagesFindOperation((CFSEngine&)*this, queryInfo.operationId);
    queryInfo.privateService = &privateService;
    queryInfo.currentFilterListIndex = 0;
    m_messageQueries.append(queryInfo);
    
    handleNestedFiltersFromMessageFilter(m_messageQueries[m_messageQueries.count()-1].filter);
    
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(m_messageQueries[m_messageQueries.count()-1].filter);
    if (pf->_filterList.count() == 0) {
        queryInfo.findOperation->filterAndOrderMessages(m_messageQueries[m_messageQueries.count()-1].filter,
                                                        m_messageQueries[m_messageQueries.count()-1].sortOrder,
                                                        body,
                                                        matchFlags);
    } else {
        queryInfo.findOperation->filterAndOrderMessages(pf->_filterList[0],
                                                        m_messageQueries[m_messageQueries.count()-1].sortOrder,
                                                        body,
                                                        matchFlags);
    }
}

bool CFSEngine::countMessages(QMessageServicePrivate &privateService, const QMessageFilter &filter)
{
    TRAPD(err, countMessagesL(privateService, filter));
    if (err != KErrNone) {
        return false;
    }
    return true;
}

void CFSEngine::countMessagesL(QMessageServicePrivate &privateService, const QMessageFilter &filter)
{
    FSMessageQueryInfo queryInfo;
    queryInfo.operationId = ++m_operationIds;
    if (queryInfo.operationId == 100000) {
        queryInfo.operationId = 1;
    }
    queryInfo.isQuery = false;
    queryInfo.matchFlags = 0;
    queryInfo.filter = filter;
    queryInfo.sortOrder = QMessageSortOrder();
    queryInfo.offset = 0;
    queryInfo.limit = 0;
    queryInfo.findOperation = new CFSMessagesFindOperation((CFSEngine&)*this, queryInfo.operationId);
    queryInfo.privateService = &privateService;
    queryInfo.currentFilterListIndex = 0;
    queryInfo.count = 0;
    m_messageQueries.append(queryInfo);
    
    handleNestedFiltersFromMessageFilter(m_messageQueries[m_messageQueries.count()-1].filter);
    
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(m_messageQueries[m_messageQueries.count()-1].filter);
    if (pf->_filterList.count() == 0) {
        queryInfo.findOperation->filterAndOrderMessages(m_messageQueries[m_messageQueries.count()-1].filter,
            m_messageQueries[m_messageQueries.count()-1].sortOrder);
    } else {
        queryInfo.findOperation->filterAndOrderMessages(pf->_filterList[0], m_messageQueries[m_messageQueries.count()-1].sortOrder);
    }
}

void CFSEngine::filterAndOrderMessagesReady(bool success, int operationId, QMessageIdList ids, int numberOfHandledFilters,
                                             bool resultSetOrdered)
{
 /*   int index=0;
    for (; index < m_messageQueries.count(); index++) {
        if (m_messageQueries[index].operationId == operationId) {
            break;
        }
    }
    
    if (success) {
        // If there are unhandled filters, loop through all filters and do filtering for ids using unhandled filters.
        QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(m_messageQueries[index].filter);
        if (pf->_filterList.count() > 0) {
            if (pf->_filterList[m_messageQueries[index].currentFilterListIndex].count() > numberOfHandledFilters) {
                for (int i=0; i < ids.count(); i++) {
                    QMessage msg = message(ids[i]);
                    for (int j=numberOfHandledFilters; j < pf->_filterList[m_messageQueries[index].currentFilterListIndex].count(); j++) {
                        QMessageFilterPrivate* pf2 = QMessageFilterPrivate::implementation(pf->_filterList[m_messageQueries[index].currentFilterListIndex][j]);
                        if (!pf2->filter(msg)) {
                            ids.removeAt(i);
                            i--;
                            break;
                        }
                    }
                }
            }
        }
        if (pf->_filterList.count() > 0) {
            // Filter contains filterlist (or filterlists), not just one single filter 
            if (m_messageQueries[index].currentFilterListIndex == 0) {
                m_messageQueries[index].ids << ids;
                m_messageQueries[index].count = ids.count(); 
            } else {
                // Append new ids to resultset
                for (int i=0; i < ids.count(); i++) {
                    if (!m_messageQueries[index].ids.contains(ids[i])) {
                        m_messageQueries[index].ids.append(ids[i]);
                        m_messageQueries[index].count++;; 
                    }
                }
            }
            
            m_messageQueries[index].currentFilterListIndex++;
            if (m_messageQueries[index].currentFilterListIndex < pf->_filterList.count()) {
                // There are still unhandled filter lists left
                m_messageQueries[index].findOperation->filterAndOrderMessages(pf->_filterList[m_messageQueries[index].currentFilterListIndex],
                                                                             m_messageQueries[index].sortOrder,
                                                                             m_messageQueries[index].body,
                                                                             m_messageQueries[index].matchFlags);
                return;
            } else {
                // All filters successfully handled
                if (m_messageQueries[index].isQuery) {
                    if (!m_messageQueries[index].sortOrder.isEmpty()) {
                        // Make sure that messages are correctly ordered
                        orderMessages(m_messageQueries[index].ids, m_messageQueries[index].sortOrder);
                    }
                    applyOffsetAndLimitToMsgIds(m_messageQueries[index].ids,
                                                m_messageQueries[index].offset,
                                                m_messageQueries[index].limit);
                    m_messageQueries[index].privateService->messagesFound(m_messageQueries[index].ids, true, true);

                    //emit m_messageQueries[index].privateService->messagesFound(m_messageQueries[index].ids);
                } else {
                    m_messageQueries[index].privateService->messagesCounted(m_messageQueries[index].count);
                }
            }
        } else {
            // There was only one single filter to handle
            if (numberOfHandledFilters == 0) {
                // The one and only filter was not handled
                // => Do filtering for all returned messages
                for (int i=ids.count()-1; i >= 0; i--) {
                    QMessage msg = message(ids[i]);
                    if (!pf->filter(msg)) {
                        ids.removeAt(i);
                    }
                }
            }
            // => All filters successfully handled
            if (m_messageQueries[index].isQuery) {
                // Make sure that messages are correctly ordered
                if (!m_messageQueries[index].sortOrder.isEmpty() && !resultSetOrdered) {
                    orderMessages(ids, m_messageQueries[index].sortOrder);
                }
                // Handle offest & limit
                applyOffsetAndLimitToMsgIds(ids, m_messageQueries[index].offset, m_messageQueries[index].limit);
                //emit m_messageQueries[index].privateService->messagesFound(ids);
                m_messageQueries[index].privateService->messagesFound(ids, true, true);
            } else {
                m_messageQueries[index].privateService->messagesCounted(ids.count());
            }
        }
    } else {
        m_messageQueries[index].privateService->_active = false;
        if (m_messageQueries[index].privateService->_error == QMessageManager::NoError) {
            m_messageQueries[index].privateService->_error = QMessageManager::RequestIncomplete;
        }
    }

    delete m_messageQueries[index].findOperation;
    m_messageQueries.removeAt(index);*/
}

void CFSEngine::applyOffsetAndLimitToMsgIds(QMessageIdList &idList, int offset, int limit) const
{
    if (offset > 0) {
        if (offset > idList.count()) {
            idList.clear();
        } else {
            for (int i = 0; i < offset; i++) {
                idList.removeFirst();
            }
        }
    }
    if (limit > 0) {
        for (int i = idList.count()-1; i >= limit; i--) {
            idList.removeAt(i);
        }
    }
}

QMessageManager::NotificationFilterId CFSEngine::registerNotificationFilter(QMessageStorePrivate &aPrivateStore,
                                                                           const QMessageFilter &filter, QMessageManager::NotificationFilterId aId)
{
    ipMessageStorePrivate = &aPrivateStore;
    iListenForNotifications = true;    

    int filterId = aId;
    if (filterId == 0)
        filterId = ++m_filterId;
    m_filters.insert(filterId, filter);
    return filterId;
}

void CFSEngine::unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId)
{
    m_filters.remove(notificationFilterId);
    if (m_filters.count() == 0) {
        iListenForNotifications = false;
    }
}
void CFSEngine::handleNestedFiltersFromFolderFilter(QMessageFolderFilter &filter) const
{
    QMessageFolderFilterPrivate* pMFFilter = QMessageFolderFilterPrivate::implementation(filter);
    if (pMFFilter->_filterList.count() > 0) {
        int filterListCount = pMFFilter->_filterList.count();
        for (int i=0; i < filterListCount; i++) {
            for (int j=0; j < pMFFilter->_filterList[i].count(); j++) {
                QMessageFolderFilterPrivate* pMFFilter2 = QMessageFolderFilterPrivate::implementation(pMFFilter->_filterList[i][j]);
                if (pMFFilter2->_field == QMessageFolderFilterPrivate::ParentAccountIdFilter) {
                    QMessageAccountIdList accountIds = queryAccounts(*pMFFilter2->_accountFilter, QMessageAccountSortOrder(), 0, 0);
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter2->_comparatorValue));
                    if (accountIds.count() > 0) {
                        pMFFilter->_filterList[i].removeAt(j);
                        if (cmp == QMessageDataComparator::Includes) {
                            for (int x = 0; x < accountIds.count(); x++) {
                                if (x == 0) {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[i]);
                                    }
                                    pMFFilter->_filterList[i].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                    qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
                                } else {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[pMFFilter->_filterList.count()-1]);
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-2].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-2].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-2].end(), QMessageFolderFilterPrivate::lessThan);
                                    } else {
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-1].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-1].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-1].end(), QMessageFolderFilterPrivate::lessThan);
                                    }
                                }
                            }
                        } else { // Excludes
                            for (int x = 0; x < accountIds.count(); x++) {
                                pMFFilter->_filterList[i].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::NotEqual));
                            }
                            qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
                        }
                    } else {
                        delete pMFFilter2->_accountFilter;
                        pMFFilter2->_accountFilter = 0;
                        pMFFilter2->_field = QMessageFolderFilterPrivate::Id;
                        qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
                    }
                } else {
                    break;
                }
            }
        }
    } else {
        if (pMFFilter->_field == QMessageFolderFilterPrivate::ParentAccountIdFilter) {
            QMessageAccountIdList accountIds = queryAccounts(*pMFFilter->_accountFilter, QMessageAccountSortOrder(), 0, 0);
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter->_comparatorValue));
            if (accountIds.count() > 0) {
                for (int i=0; i < accountIds.count(); i++) {
                    if (i == 0) {
                        delete pMFFilter->_accountFilter;
                        pMFFilter->_accountFilter = 0;
                        pMFFilter->_field = QMessageFolderFilterPrivate::ParentAccountId;
                        pMFFilter->_value = accountIds[0].toString();
                        pMFFilter->_comparatorType = QMessageFolderFilterPrivate::Equality;
                        if (cmp == QMessageDataComparator::Includes) {
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
                        } else { // Excludes
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
                        }
                    } else {
                        if (cmp == QMessageDataComparator::Includes) {
                            filter |= QMessageFolderFilter::byParentAccountId(accountIds[i],QMessageDataComparator::Equal);
                        } else { // Excludes
                            filter &= QMessageFolderFilter::byParentAccountId(accountIds[i],QMessageDataComparator::NotEqual);
                        }
                    }
                }
            } else {
                delete pMFFilter->_accountFilter;
                pMFFilter->_accountFilter = 0;
                pMFFilter->_field = QMessageFolderFilterPrivate::Id;
            }
        }
    }
}

QMessageFolderIdList CFSEngine::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageFolderIdList ids;
    
    QMessageFolderFilter copyOfFilter = filter;

    handleNestedFiltersFromFolderFilter(copyOfFilter);
    
    QMessageFolderFilterPrivate* pMFFilter = QMessageFolderFilterPrivate::implementation(copyOfFilter);

    if (pMFFilter->_filterList.count() > 0) {
        for (int i=0; i < pMFFilter->_filterList.count(); i++) {
            bool filterHandled;
            QMessageFolderIdList ids2 = filterMessageFolders(pMFFilter->_filterList[i][0], filterHandled);
            for (int x=ids2.count()-1; x >= 0; x--) {
                QMessageFolder mf = folder(ids2[x]);
                int j = filterHandled ? 1 : 0;
                for (; j < pMFFilter->_filterList[i].count(); j++) {
                    if (!QMessageFolderFilterPrivate::implementation(pMFFilter->_filterList[i][j])->filter(mf)) {
                        ids2.removeAt(x);
                        break;
                    }
                }
            }
            for (int j=0; j < ids2.count(); j++) {
                if (!ids.contains(ids2[j])) {
                   ids.append(ids2[j]);
                }
            }
        }
    } else {
        bool filterHandled;
        ids = filterMessageFolders(copyOfFilter, filterHandled);
        if (!filterHandled) {
            for (int i=ids.count()-1; i >= 0; i--) {
                if (!QMessageFolderFilterPrivate::implementation(copyOfFilter)->filter(ids[i])) {
                    ids.removeAt(i);
                }
            }
        }
    }
    
    if (!sortOrder.isEmpty()) {
        orderFolders(ids, sortOrder);
    }
    
    applyOffsetAndLimitToMsgFolderIds(ids, offset, limit);
    
    return ids;
}

void CFSEngine::applyOffsetAndLimitToMsgFolderIds(QMessageFolderIdList &idList, int offset, int limit) const
{
    if (offset > 0) {
        if (offset > idList.count()) {
            idList.clear();
        } else {
            for (int i = 0; i < offset; i++) {
                idList.removeFirst();
            }
        }
    }
    if (limit > 0) {
        for (int i = idList.count()-1; i >= limit; i--) {
            idList.removeAt(i);
        }
    }
}

int CFSEngine::countFolders(const QMessageFolderFilter &filter) const
{
    return queryFolders(filter, QMessageFolderSortOrder(), 0, 0).count();
}

QMessageFolder CFSEngine::folder(const QMessageFolderId &id) const
{
    QMessageFolder messageFolder;
    QMessageFolderId parentId;
    QMessageAccountId accountId;

    // get account containing folder
    updateEmailAccounts();

    foreach (QMessageAccount account, m_accounts) {
        accountId = account.id();
        quint64 mailboxId(stripIdPrefix(accountId.toString()).toULongLong());

        QList<NmApiFolder> folders = getFolderListByAccountId(mailboxId);
        
        foreach(NmApiFolder folder, folders) {
            QString fsIdAsString = QString::number(folder.id());
            if (stripIdPrefix(id.toString()).toULongLong() == folder.id()) {
                messageFolder = QMessageFolderPrivate::from(
                    id,
                    accountId,
                    parentId,
                    folder.name(),
                    folder.name());
                break;
            }
        }
    }
        
    return messageFolder;
}

QMessageFolderIdList CFSEngine::filterMessageFolders(const QMessageFolderFilter& filter, bool& filterHandled) const
{
    QMessageFolderIdList ids;
    TRAPD(err, ids = filterMessageFoldersL(filter, filterHandled));
    Q_UNUSED(err)
    return ids;
}

QMessageFolderIdList CFSEngine::filterMessageFoldersL(const QMessageFolderFilter& filter, bool& filterHandled) const
{
    filterHandled = false;
    QMessageFolderIdList ids;
    
    if (filter.isEmpty()) {
        QMessageFolderFilterPrivate* pf = QMessageFolderFilterPrivate::implementation(filter);
        if (!pf->_notFilter) {
            ids = allFolders();
        }
        filterHandled = true;
    } else {
        QMessageFolderFilterPrivate* pf = QMessageFolderFilterPrivate::implementation(filter);
        if (!pf->_valid) {
            return QMessageFolderIdList();
        }
    
        switch (pf->_field) {
        case QMessageFolderFilterPrivate::Id:
            {
            if (pf->_comparatorType == QMessageFolderFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (pf->_value.toString().length() > QString(SymbianHelpers::mtmPrefix).length()) {
                    bool folderOk = false;
                    /*MEmailMailbox* mailbox = NULL;
                    MEmailFolder* folder = NULL;;
                    if (fsFolderL(QMessageFolderId(pf->_value.toString()), mailbox, folder)) {
                        folderOk = true;
                        // cleanup
                        folder->Release();
                        mailbox->Release();
                    }
                    if (cmp == QMessageDataComparator::Equal) {
                        if (folderOk) {
                            ids.append(QMessageFolderId(pf->_value.toString()));
                        }
                    } else { // NotEqual
                        ids = allFolders();
                        if (folderOk) {
                            ids.removeOne(QMessageFolderId(pf->_value.toString()));
                        }
                    }*/
                } else {
                    if (cmp == QMessageDataComparator::NotEqual) {
                        ids = allFolders();
                    }
                }
                filterHandled = true;
            } else if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (pf->_ids.count() > 0) { // QMessageIdList
                    /*QMessageFolderIdList ids2;
                    for (int i=0; i < pf->_ids.count(); i++) {
                        MEmailMailbox* mailbox = NULL;
                        MEmailFolder* folder = NULL;
                        if (fsFolderL(QMessageFolderId(pf->_ids[i]), mailbox, folder)) {
                            ids2.append(pf->_ids[i]);
                            // cleanup
                            folder->Release();
                            mailbox->Release();
                        }
                    }
                    if (cmp == QMessageDataComparator::Includes) {
                        ids << ids2;
                    } else { // Excludes
                        ids = allFolders();
                        for (int i=0; i < ids2.count(); i++) {
                            ids.removeOne(ids2[i]);
                        }
                    }*/
                    filterHandled = true;
                } else {
                    // Empty QMessageIdList as a list
                    if (cmp == QMessageDataComparator::Excludes) {
                        ids = allFolders();
                    }
                    filterHandled = true;
                
                    // QMessageFilter 
                    if (cmp == QMessageDataComparator::Includes) {
                        // TODO:
                    } else { // Excludes
                        // TODO:
                    }
                }
            }
            break;
            }
        case QMessageFolderFilterPrivate::Name:
            {
            if (pf->_comparatorType == QMessageFolderFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    // TODO:
                } else { // NotEqual
                    // TODO:
                }
            } else if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    if (pf->_value.toString().isEmpty() || pf->_value.toString().length() == 0) {
                        filterHandled = true;
                    }
                }
            }
            break;
            }
        case QMessageFolderFilterPrivate::Path:
            {
            if (pf->_comparatorType == QMessageFolderFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    // TODO:
                } else { // NotEqual
                    // TODO:
                }
            } else if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    if (pf->_value.toString().isEmpty() || pf->_value.toString().length() == 0) {
                        filterHandled = true;
                    }
                }
            }
            break;
            }
        case QMessageFolderFilterPrivate::ParentAccountId:
            {
            if (pf->_comparatorType == QMessageFolderFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    if (pf->_value.toString().length() > 0) {
                        ids = folderIdsByAccountId(QMessageAccountId(pf->_value.toString()));
                    }
                } else { // NotEqual
                    ids = allFolders();
                    if (pf->_value.toString().length() > 0) {
                        QMessageFolderIdList ids2 = folderIdsByAccountId(QMessageAccountId(pf->_value.toString()));
                        for (int i = 0; i < ids2.count(); i++) {
                            ids.removeOne(ids2[i]);
                        }
                    }
                }
                filterHandled = true;
            } else if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    // TODO:
                }
            }
            break;
            }
        case QMessageFolderFilterPrivate::ParentFolderId:
            {
            if (pf->_comparatorType == QMessageFolderFilterPrivate::Equality) {
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    /*MEmailMailbox* mailbox = NULL;
                    MEmailFolder* parentFolder = NULL;
                    if (fsFolderL(QMessageFolderId(pf->_value.toString()), mailbox, parentFolder)) {
                        CleanupReleasePushL(*mailbox);
                        CleanupReleasePushL(*parentFolder);

                        RFolderArray subfolders;
                        
                        parentFolder->GetSubfoldersL(subfolders);
                        CleanupClosePushL(subfolders);

                        for(TInt i=0; i < subfolders.Count(); i++) {
                            MEmailFolder *subFolder = subfolders[i];
                            
                            ids.append(QMessageFolderId(addIdPrefix(
                                QString::number(subFolder->FolderId().iId), 
                                SymbianHelpers::EngineTypeFreestyle)));
                            
                            subFolder->Release();
                        }
                        
                        CleanupStack::PopAndDestroy(&subfolders);
                        CleanupStack::PopAndDestroy(parentFolder);
                        CleanupStack::PopAndDestroy(mailbox);
                    }*/
                } else { // NotEqual
                    // TODO:
                }
            } else if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    // TODO:
                }
            }
            break;
            }
        case QMessageFolderFilterPrivate::AncestorFolderIds:
            {
                if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                    if (!pf->_value.isNull()) { // QMessageFolderId
                        if (cmp == QMessageDataComparator::Includes) {
                            // TODO:
                        } else { // Excludes
                            // TODO:
                        }
                    } else { // QMessageFolderFilter
                        if (cmp == QMessageDataComparator::Includes) {
                            // TODO:
                        } else { // Excludes
                            // TODO:
                        }
                    }
                }
                break;
            }
        case QMessageFolderFilterPrivate::ParentAccountIdFilter:
        case QMessageFolderFilterPrivate::None:
            break;        
        }
    }
    
    if (!filterHandled) {
        ids = allFolders();
    }

    return ids;
}


QMessageFolderIdList CFSEngine::allFolders() const
{
    QMessageFolderIdList ids;
    updateEmailAccounts();

    foreach (QMessageAccount value, m_accounts) {
        QMessageFolderIdList ids2 = folderIdsByAccountId(value.id());
        ids << ids2;
    }
    return ids;
}

QMessageFolderIdList CFSEngine::folderIdsByAccountId(const QMessageAccountId& accountId) const
{
    QMessageFolderIdList folderIds;
    
    if (idType(accountId) != EngineTypeFreestyle)
        return QMessageFolderIdList();
    
    QMessageAccount messageAccount = account(accountId);
    
    quint64 mailboxId = stripIdPrefix(accountId.toString()).toULongLong();

    QList<NmApiFolder> folders = getFolderListByAccountId(mailboxId);
    
    foreach(NmApiFolder folder, folders) {
        QString fsIdAsString = addIdPrefix(QString::number(folder.id()), SymbianHelpers::EngineTypeFreestyle);
        folderIds.append(QMessageFolderId(fsIdAsString));
    }
    
    return folderIds;
}

QList<NmApiFolder> CFSEngine::getFolderListByAccountId(const quint64 mailboxId) const
{
    NmApiFolderListing* folderlisting = new NmApiFolderListing(0, mailboxId);       
    QEventLoop* eventloop = new QEventLoop();
    QTimer* timeOut = new QTimer();
    connect(folderlisting, SIGNAL(foldersListed(qint32)), eventloop, SLOT(quit()));
    connect(timeOut, SIGNAL(timeout()), eventloop, SLOT(quit()));
    timeOut->start(5000);
    folderlisting->cancel();
    folderlisting->start();
    eventloop->exec();
    
    if (timeOut->isActive())
        timeOut->stop();
    
    QList<NmApiFolder> folders;
    folderlisting->getFolders(folders);

    delete eventloop;
    delete folderlisting;
    
    return folders;
}

bool CFSEngine::fsFolderL(const QMessageFolderId& id, NmApiMailbox* mailbox, NmApiFolder* folder) const
{
    Q_UNUSED(folder);
   /* MEmailFolder* fsFolder = NULL;
    foreach (QMessageAccount account, m_accounts) {
        TMailboxId mailboxId(stripIdPrefix(account.id().toString()).toInt());
        mailbox = m_clientApi->MailboxL(mailboxId);
        
        TFolderId folderId(
            stripIdPrefix(id.toString()).toInt(),
            mailboxId);
            
        TRAPD(err, folder = mailbox->FolderL(folderId));
        if (err == KErrNone) {               
            CleanupReleasePushL(*fsFolder);
            return true;
        }
        mailbox->Release();
    }
    mailbox = NULL;
    folder = NULL;*/
    return false;
}

QMessage CFSEngine::CreateQMessage(NmApiMessage* aMessage) const
{
    QMessage message;
    int size = 0;
    message.setType(QMessage::Email);
    
    NmApiMessageEnvelope envelope = aMessage->envelope();

    message.setDate(envelope.sentTime());
    message.setReceivedDate(envelope.sentTime());
    
    quint64 mailboxId = envelope.mailboxId();
    const QMessageAccountId accountId = QMessageAccountId(QString::number(mailboxId));
    message.setParentAccountId(accountId);
    
    quint64 parentFolderId = envelope.parentFolder();
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_parentFolderId = QMessageFolderId(QString::number(parentFolderId));
    
  /*  use emailservice getFolder
    QMessagePrivate::setStandardFolder(message, QMessage::InboxFolder);
    if (folder->FolderType() == EDrafts) {
        QMessagePrivate::setStandardFolder(message, QMessage::DraftsFolder);
    } else if (folder->FolderType() == EDeleted) {
        QMessagePrivate::setStandardFolder(message, QMessage::TrashFolder);
    } else if (folder->FolderType() == ESent) {
        QMessagePrivate::setStandardFolder(message, QMessage::SentFolder);
    }
*/
    if (envelope.isRead()) {
        privateMessage->_status = privateMessage->_status | QMessage::Read; 
    }
    
    if (envelope.priority() == EmailClientApi::NmApiMessagePriorityHigh) {
        message.setPriority(QMessage::HighPriority); 
    } else if (envelope.priority() == EmailClientApi::NmApiMessagePriorityLow) {
        message.setPriority(QMessage::LowPriority);
    } else {
        message.setPriority(QMessage::NormalPriority);
    }
               
    QString htmlContent = aMessage->htmlContent().content();
    if (!htmlContent.isEmpty())
        message.setBody(htmlContent, "text/html");
    QString plainContent = aMessage->plainTextContent().content();
    if (!plainContent.isEmpty())
        message.setBody(plainContent, "text/plain");
  
    if (envelope.hasAttachments())
        privateMessage->_status = privateMessage->_status | QMessage::HasAttachments;
    
    // attachment implementation changes.. use messagemanager or what ??
    QList<NmApiAttachment> attachments = aMessage->attachments();
    foreach (NmApiAttachment attachment, attachments) {
        QByteArray name = attachment.fileName().toUtf8();
        QString contentType = attachment.contentType(); // ??
        QByteArray mimeType; // TODO: email client api doesn't offer information about attachment mimetype
        QByteArray mimeSubType; // TODO:
        QMessageContentContainer att = QMessageContentContainerPrivate::from(
                                                        envelope.id(), 
                                                        attachment.id(), 
                                                        name, mimeType, 
                                                        mimeSubType, attachment.size());
        addAttachmentToMessage(message, att);
    }

    //from
    QString from = envelope.sender();
    if (!from.isEmpty()) {
        message.setFrom(QMessageAddress(QMessageAddress::Email, from));
        QMessagePrivate::setSenderName(message, from);
    }
    
    //to
    QList<EmailClientApi::NmApiEmailAddress> toRecipients;
    envelope.getToRecipients(toRecipients);
    QList<QMessageAddress> toList;
    for(TInt i = 0; i < toRecipients.count(); i++) {
        QString to = toRecipients[i].address();
        toList.append(QMessageAddress(QMessageAddress::Email, to));
    }
    message.setTo(toList);
    toRecipients.clear();
    
    //cc
    QList<EmailClientApi::NmApiEmailAddress> ccRecipients;
    envelope.getCcRecipients(ccRecipients);
    QList<QMessageAddress> ccList;
    for(TInt i = 0; i < ccRecipients.count(); i++) {
        QString cc = ccRecipients[i].address();
        ccList.append(QMessageAddress(QMessageAddress::Email, cc));
    }
    message.setCc(ccList); 
    ccRecipients.clear();
    
    //bcc
    QList<EmailClientApi::NmApiEmailAddress> bccRecipients;
    envelope.getBccRecipients(bccRecipients);
    QList<QMessageAddress> bccList;
    for(TInt i = 0; i < bccRecipients.count(); i++) {
        QString bcc = bccRecipients[i].address();
        bccList.append(QMessageAddress(QMessageAddress::Email, bcc));
    }
    message.setBcc(bccList); 
    bccRecipients.clear();
    
    // Read message subject   
    QString subject = envelope.subject();
    if (!subject.isEmpty()) {
        message.setSubject(subject);
    }

    privateMessage->_size = envelope.totalSize();

    return message;    
}

void CFSEngine::addAttachmentToMessage(QMessage& message, QMessageContentContainer& attachment) const
{
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    QMessageContentContainerPrivate* container = QMessagePrivate::containerImplementation(message);
    
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

NmApiEmailFolderType CFSEngine::standardFolderId(QMessage::StandardFolder standardFolder)
{
    switch(standardFolder) {
        case QMessage::InboxFolder: return Inbox;
        case QMessage::OutboxFolder: return Outbox;
        case QMessage::DraftsFolder: return Drafts;
        case QMessage::SentFolder: return Sent;
        case QMessage::TrashFolder: return Deleted;
        default: return EOther;
    }
}

CFSMessagesFindOperation::CFSMessagesFindOperation(CFSEngine& aOwner, int aOperationId)
    : m_owner(aOwner), 
      m_operationId(aOperationId),
      m_resultCorrectlyOrdered(false),
      m_receiveNewMessages(false),
      m_activeSearchCount(0),
      m_searchField(None)
{

}

CFSMessagesFindOperation::~CFSMessagesFindOperation()
{
   /* foreach(FSSearchOperation operation, m_searchOperations) {
        operation.m_search->Release();
        operation.m_mailbox->Release();
    }*/
    
    m_receiveNewMessages = false;
}

void CFSMessagesFindOperation::filterAndOrderMessages(const QMessageFilter &filter, const QMessageSortOrder& sortOrder,
                                                    QString body, QMessageDataComparator::MatchFlags matchFlags)
{
    m_filterList.clear();
    m_filterList.append(filter);
    filterAndOrderMessages(m_filterList, sortOrder, body, matchFlags);
}

void CFSMessagesFindOperation::filterAndOrderMessages(const QMessageFilterPrivate::SortedMessageFilterList& filters,
                                                    const QMessageSortOrder& sortOrder,
                                                    QString body,
                                                    QMessageDataComparator::MatchFlags matchFlags)
{
    m_numberOfHandledFilters = 0;
    
    NmApiMailSortCriteria sortCriteria = NmApiMailSortCriteria();
    m_excludeIdList = QMessageIdList();

    m_matchFlags = matchFlags;
    
    if (filters.count() == 0) {
        m_idList = QMessageIdList();
        QMetaObject::invokeMethod(this, "searchCompleted", Qt::QueuedConnection);
        return;
    }
    
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(filters[m_numberOfHandledFilters]);

    // Set sortOrder
    
    if (!sortOrder.isEmpty() ) {
        QMessageSortOrderPrivate* privateMessageOrdering = QMessageSortOrderPrivate::implementation(sortOrder);
        QPair<QMessageSortOrderPrivate::Field, Qt::SortOrder> fieldOrder = privateMessageOrdering->_fieldOrderList.at(0);
        switch (fieldOrder.first) {
            case QMessageSortOrderPrivate::Type:
                break;
            case QMessageSortOrderPrivate::Sender:
                sortCriteria.iField = MailSortBySender;
                break;
            case QMessageSortOrderPrivate::Recipients:
                sortCriteria.iField = MailSortByRecipient;
                break;
            case QMessageSortOrderPrivate::Subject:
                sortCriteria.iField = MailSortBySubject;
                break;
            case QMessageSortOrderPrivate::TimeStamp:
                sortCriteria.iField = MailSortByDate;
                break;
            case QMessageSortOrderPrivate::ReceptionTimeStamp:
                sortCriteria.iField = MailSortBySender;
                break;
            case QMessageSortOrderPrivate::Read:
                sortCriteria.iField = MailSortByUnread;
                break;
            case QMessageSortOrderPrivate::HasAttachments:
                sortCriteria.iField = MailSortByAttachment;
                break;
            case QMessageSortOrderPrivate::Incoming:
                //TODO:
                break;
            case QMessageSortOrderPrivate::Removed:
                //TODO:
                break;
            case QMessageSortOrderPrivate::Priority:
                sortCriteria.iField = MailSortByPriority;
                break;
            case QMessageSortOrderPrivate::Size:
                sortCriteria.iField = MailSortBySize;
                break;
        }
        sortCriteria.iOrder = fieldOrder.second == Qt::AscendingOrder?MailAscending:MailDescending;
    } else {
        // This is a workaroud for getFolderSpecificMessagesL crashing when default TEmailSortCriteria (EDontCare) is set
        sortCriteria.iField = MailSortByDate;
    }
    

    if ((filters.count() == 1) &&
        (pf->_field == QMessageFilterPrivate::None) &&
        (pf->_filterList.count() == 0)) {
        if (pf->_notFilter) {
            // There is only one filter: empty ~QMessageFilter()
            // => return empty QMessageIdList 
            m_idList = QMessageIdList();
            QMetaObject::invokeMethod(this, "searchCompleted", Qt::QueuedConnection);
        } else {
            // There is only one filter: empty QMessageFilter()
            // => return all messages
            getAllMessages(sortCriteria);
        }
        m_numberOfHandledFilters++;
        return;
    }

    if (!body.isEmpty()) {
        m_searchField = Body;
        m_searchKey = body;
    }
    
    switch (pf->_field) {
    
        case QMessageFilterPrivate::ParentFolderId: {
            if (idType(pf->_value.toString()) != EngineTypeFreestyle) {
                QMetaObject::invokeMethod(this, "searchCompleted", Qt::QueuedConnection);
                return;
            }
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageFolderId
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    m_numberOfHandledFilters++;
                    QMessageFolder messageFolder = m_owner.folder(QMessageFolderId(pf->_value.toString()));
                    getFolderSpecificMessages(messageFolder);
                    m_resultCorrectlyOrdered = false;
                } else { // NotEqual
                    // TODO:
                }
            } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) { // QMessageFolderFilter
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Includes) {
                    // TODO:
                } else { // Excludes
                    // TODO:
                }
            }
            break;
        }
        case QMessageFilterPrivate::Id: {
            if (idType(pf->_value.toString()) != EngineTypeFreestyle) {
                QMetaObject::invokeMethod(this, "searchCompleted", Qt::QueuedConnection);
                return;
            }
            m_numberOfHandledFilters++;
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageId
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (!pf->_value.isNull() && pf->_value.toString().length() > QString(SymbianHelpers::freestylePrefix).length()) {
                    if (cmp == QMessageDataComparator::Equal) {
                        QMessage message = m_owner.message(QMessageId(pf->_value.toString()));
                        m_idList.clear();
                        m_idList.append(message.id());
                        m_resultCorrectlyOrdered = true;
                        QMetaObject::invokeMethod(this, "searchCompleted", Qt::QueuedConnection);

                    } else { // NotEqual
                        m_excludeIdList.clear();
                        m_excludeIdList.append(QMessageId(pf->_value.toString()));
                        getAllMessages(sortCriteria);
                    }
                } else {
                    if (cmp == QMessageDataComparator::NotEqual) {
                        getAllMessages(sortCriteria);
                    }
                }
            } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (pf->_ids.count() > 0) { // QMessageIdList
                    if (cmp == QMessageDataComparator::Includes) {
                        for (int i=0; i < pf->_ids.count(); i++) {
                            QMessage message = m_owner.message(QMessageId(pf->_ids[i].toString()));
                            m_idList.append(message.id());
                        }
                        QMetaObject::invokeMethod(this, "searchCompleted", Qt::QueuedConnection);
                    } else { // Excludes
                        for (int i=0; i < pf->_ids.count(); i++) {
                            m_excludeIdList.clear();
                            m_excludeIdList.append(QMessageId(pf->_ids[i].toString()));
                            getAllMessages(sortCriteria);
                        }
                        getAllMessages(sortCriteria);
                    }
                } else {
                    if (cmp == QMessageDataComparator::Excludes) {
                        getAllMessages(sortCriteria);
                    }
                    // QMessageFilter
                    if (cmp == QMessageDataComparator::Includes) {
                        // TODO:
                    } else { // Excludes
                        // TODO:
                    }
                }
            }
            break;
            }
        case QMessageFilterPrivate::ParentAccountId: {
            if (idType(pf->_value.toString()) != EngineTypeFreestyle) {
                QMetaObject::invokeMethod(this, "searchCompleted", Qt::QueuedConnection);
                return;
            }
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageAccountId
                m_numberOfHandledFilters++;
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    QMessageAccount messageAccount = m_owner.account(pf->_value.toString());
                    //getAccountSpecificMessagesL(messageAccount);
                    m_resultCorrectlyOrdered = true;
                } else { // NotEqual
                    QStringList exludedAccounts;
                    exludedAccounts << pf->_value.toString();
                    
                    QMessageFilterPrivate* privateFilter = NULL;
                    for (int i=m_numberOfHandledFilters; i < filters.count(); i++) {
                        privateFilter = QMessageFilterPrivate::implementation(filters[i]);
                        if (privateFilter->_field == QMessageFilterPrivate::ParentAccountId &&
                            privateFilter->_comparatorType == QMessageFilterPrivate::Equality) {
                            cmp = static_cast<QMessageDataComparator::EqualityComparator>(privateFilter->_comparatorValue);
                            if (cmp == QMessageDataComparator::NotEqual) {
                                exludedAccounts << privateFilter->_value.toString();
                                m_numberOfHandledFilters++;
                            } else {
                                break;
                            }
                        } else {
                            break;
                        }
                    }

                    privateFilter = NULL;
                    if (filters.count() > m_numberOfHandledFilters) {
                        privateFilter = QMessageFilterPrivate::implementation(filters[m_numberOfHandledFilters]);
                        if (privateFilter->_field == QMessageFilterPrivate::StandardFolder &&
                            privateFilter->_comparatorType == QMessageFilterPrivate::Equality) {
                            cmp = static_cast<QMessageDataComparator::EqualityComparator>(privateFilter->_comparatorValue);
                            if (cmp == QMessageDataComparator::Equal) {
                                m_numberOfHandledFilters++;
                            }
                        } else {
                            privateFilter = NULL;
                        }
                    }
                    
                    foreach (QMessageAccount value, m_owner.m_accounts) {
                        if (!exludedAccounts.contains(value.id().toString())) {
                            getAccountSpecificMessages(value, sortCriteria);
                        }
                    }
                }
            }
            break;
        }
            
        case QMessageFilterPrivate::AncestorFolderIds: {
            m_numberOfHandledFilters++;
            if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (!pf->_value.isNull()) { // QMessageFolderId
                    if (cmp == QMessageDataComparator::Includes) {
                        // TODO:
                    } else { // Excludes
                        // TODO:
                    }
                } else { // QMessageFolderFilter
                    if (cmp == QMessageDataComparator::Includes) {
                        // TODO:
                    } else { // Excludes
                        // TODO:
                    }
                }
            }
            break;
            }
        case QMessageFilterPrivate::Type: {
            m_numberOfHandledFilters++;
            QMessageFilterPrivate* privateFilter = NULL;
            // Check if next filter is StandardFolder filter 
            if (filters.count() > m_numberOfHandledFilters) {
                privateFilter = QMessageFilterPrivate::implementation(filters[m_numberOfHandledFilters]);
                if (privateFilter->_field != QMessageFilterPrivate::StandardFolder) {
                    privateFilter = NULL;
                } else {
                    m_numberOfHandledFilters++;
                }
            }
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessage::Type
                QMessage::Type type = static_cast<QMessage::Type>(pf->_value.toInt()); 
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    QMessageAccountIdList accountIds = m_owner.accountsByType(type);
                    for (int i = 0; i < accountIds.count(); i++) {
                        QMessageAccount messageAccount = m_owner.account(accountIds[i]);
                        getAccountSpecificMessages(messageAccount, sortCriteria);
                    }
                } else { // NotEqual
                    foreach (QMessageAccount value, m_owner.m_accounts) {
                        if (!(value.messageTypes() & type)) {
                            getAccountSpecificMessages(value, sortCriteria);
                        }
                    }
                }
            } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) { // QMessage::TypeFlags
                QMessage::TypeFlags typeFlags = static_cast<QMessage::TypeFlags>(pf->_value.toInt());
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Includes) {
                    foreach (QMessageAccount value, m_owner.m_accounts) {
                        if (value.messageTypes() | typeFlags) {
                            getAccountSpecificMessages(value, sortCriteria);
                        }
                    }

                } else { // Excludes
                    foreach (QMessageAccount value, m_owner.m_accounts) {
                        if (!(value.messageTypes() & typeFlags)) {
                            getAccountSpecificMessages(value, sortCriteria);
                        }
                    }
                }
            }
            break;
            }
        case QMessageFilterPrivate::StandardFolder: {
            m_numberOfHandledFilters++;
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            QMessage::StandardFolder standardFolder = static_cast<QMessage::StandardFolder>(pf->_value.toInt());
            /*
            TFolderType stdFolder = m_owner.standardFolderId(standardFolder);

            if (cmp == QMessageDataComparator::Equal) {
                foreach (QMessageAccount messageAccount, m_owner.m_accounts) {
                    TMailboxId mailboxId(stripIdPrefix(messageAccount.id().toString()).toInt());
                    MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);
                    CleanupReleasePushL(*mailbox);
                    MEmailFolder* folder = mailbox->FolderByTypeL(stdFolder);
                    CleanupReleasePushL(*folder);
                    QMessageFolder standardFolder = m_owner.folder(
                        QMessageFolderId(QString::number(folder->FolderId().iId)));
                    getFolderSpecificMessagesL(standardFolder, sortCriteria);
                    m_activeSearchCount++;
                    CleanupStack::PopAndDestroy(folder);
                    CleanupStack::PopAndDestroy(mailbox);
                }
                m_resultCorrectlyOrdered = true;
                QMetaObject::invokeMethod(this, "searchCompleted", Qt::QueuedConnection);
            } else { // NotEqual
                foreach (QMessageAccount messageAccount, m_owner.m_accounts) {
                    TMailboxId mailboxId(stripIdPrefix(messageAccount.id().toString()).toInt());
                    MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);
                    CleanupReleasePushL(*mailbox);
                    QMessage::StandardFolder i = QMessage::InboxFolder;
                    while (i <= QMessage::TrashFolder) {
                        if (i != standardFolder) {
                            MEmailFolder* folder = mailbox->FolderByTypeL(m_owner.standardFolderId(i));
                            CleanupReleasePushL(*folder);
                            QMessageFolder standardFolder = m_owner.folder(
                                QMessageFolderId(QString::number(folder->FolderId().iId)));
                            getFolderSpecificMessagesL(standardFolder, sortCriteria);
                            CleanupStack::PopAndDestroy(folder);
                        }
                        i = static_cast<QMessage::StandardFolder>(static_cast<int>(i) + 1);
                    }
                    CleanupStack::PopAndDestroy(mailbox);    
                }
                QMetaObject::invokeMethod(this, "searchCompleted", Qt::QueuedConnection);
            }*/
            break;
            }
             
        case QMessageFilterPrivate::Sender: 
        case QMessageFilterPrivate::Recipients: 
        case QMessageFilterPrivate::Subject: 
        case QMessageFilterPrivate::Status:
        case QMessageFilterPrivate::Priority:
        case QMessageFilterPrivate::Size:
        case QMessageFilterPrivate::ParentAccountIdFilter:
        case QMessageFilterPrivate::ParentFolderIdFilter: 
        case QMessageFilterPrivate::TimeStamp:
        case QMessageFilterPrivate::ReceptionTimeStamp:
        case QMessageFilterPrivate::None:
        default:
            break;
    
    }

    if (body.isEmpty()) {
        if (m_numberOfHandledFilters < filters.count()) {
            pf = QMessageFilterPrivate::implementation(filters[m_numberOfHandledFilters]);
            switch (pf->_field) {
                case QMessageFilterPrivate::Sender: {
                    m_searchField = Sender;
                    if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                        QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                        if (cmp == QMessageDataComparator::Equal) {
                            if (pf->_value.toString().length() > 0) {
                                m_searchKey = pf->_value.toString();
                                m_numberOfHandledFilters++;
                            }
                        } else { // NotEqual
                            // TODO:
                        }
                    } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) {
                        QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                        if (cmp == QMessageDataComparator::Includes) {
                            // TODO:
                        } else { // Excludes
                            // TODO:
                        }
                    }
                    break;
                }
    
                case QMessageFilterPrivate::Recipients: {
                    m_searchField = Recipients;
                    if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) {
                        QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                        if (cmp == QMessageDataComparator::Includes) {
                            if (pf->_value.toString().length() > 0) {
                                m_searchKey = pf->_value.toString();
                                m_numberOfHandledFilters++;
                            }
                        } else { // Excludes
                            //TODO:
                        }
                    }
                    break;
                }
    
                case QMessageFilterPrivate::Subject: {
                    m_searchField = Subject;
                    if (pf->_comparatorType == QMessageFilterPrivate::Equality) {
                        QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                        if (cmp == QMessageDataComparator::Equal) {
                            if (pf->_value.toString().length() > 0) {
                                m_searchKey = pf->_value.toString();
                                m_numberOfHandledFilters++;
                            }
                        } else { // NotEqual
                            // TODO:
                        }
                    } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) {
                        QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                        if (cmp == QMessageDataComparator::Includes) {
                            // TODO:
                        } else { // Excludes
                            // TODO:
                        }
                    }
                    break;
                }
                case QMessageFilterPrivate::TimeStamp:
                case QMessageFilterPrivate::ReceptionTimeStamp:
                case QMessageFilterPrivate::Status:
                case QMessageFilterPrivate::Priority:
                case QMessageFilterPrivate::Size:
                case QMessageFilterPrivate::ParentAccountIdFilter:
                case QMessageFilterPrivate::ParentFolderIdFilter: 
                case QMessageFilterPrivate::Id:
                case QMessageFilterPrivate::ParentFolderId:
                case QMessageFilterPrivate::AncestorFolderIds:
                case QMessageFilterPrivate::ParentAccountId:
                case QMessageFilterPrivate::Type:
                case QMessageFilterPrivate::StandardFolder:
                case QMessageFilterPrivate::None:
                default:
                    break;
            }
            if (m_searchOperations.count() == 0)
                getAllMessages(sortCriteria);
        }
    }
    if (m_searchOperations.count() == 0)
        QMetaObject::invokeMethod(this, "searchCompleted", Qt::QueuedConnection);
}

void CFSMessagesFindOperation::getFolderSpecificMessages(QMessageFolder& messageFolder)
{
    FSSearchOperation operation;
    quint64 folderId = stripIdPrefix(messageFolder.id().toString()).toULongLong();
    quint64 mailboxId = stripIdPrefix(messageFolder.parentAccountId().toString()).toULongLong();    
    operation.m_FolderSearch = new NmApiEnvelopeListing(0, folderId, mailboxId);
    operation.m_Type = FSSearchOperation::SearchFolder;
    m_searchOperations.append(operation);
    connect(operation.m_FolderSearch, SIGNAL(envelopesListed(qint32)), this, SLOT(searchOperationCompleted()));
    if (m_searchOperations.count() == 1)
        operation.m_FolderSearch->start();
}

void CFSMessagesFindOperation::getAccountSpecificMessages(QMessageAccount& messageAccount, NmApiMailSortCriteria& sortCriteria)
{
    quint64 mailboxId(stripIdPrefix(messageAccount.id().toString()).toULongLong());
    FSSearchOperation operation;
    operation.m_AccountSearch = new NmApiMessageSearch(0, mailboxId);
    operation.m_Type = FSSearchOperation::SearchAccount;
    QList<QString> searchKeys;
    if (m_searchKey.isEmpty())
        searchKeys.append(QString("*"));
    else
        searchKeys.append(m_searchKey);
    operation.m_AccountSearch->initialise(searchKeys, sortCriteria);
    m_searchOperations.append(operation);
    //connect(operation.m_FolderSearch, SIGNAL(messageFound(NmApiMessage)), this, SLOT(messageFound(NmApiMessage)));
    connect(operation.m_AccountSearch, SIGNAL(searchComplete(int)), this, SLOT(searchOperationCompleted()));
    if (m_searchOperations.count() == 1)
        operation.m_AccountSearch->start();
}

void CFSMessagesFindOperation::getAllMessages(NmApiMailSortCriteria& sortCriteria)
{
    // Get all messages from every known account
    foreach (QMessageAccount value, m_owner.m_accounts) {
        getAccountSpecificMessages(value, sortCriteria);
    }
    if (m_searchOperations.count() == 0)
        QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
}

void CFSMessagesFindOperation::searchOperationCompleted()
{
    int i = 0;
    foreach (FSSearchOperation operation, m_searchOperations) {
        if (operation.m_Status == FSSearchOperation::SearchActive) {
            if (operation.m_Type == FSSearchOperation::SearchFolder) {
                QList<EmailClientApi::NmApiMessageEnvelope> envelopes;
                operation.m_FolderSearch->getEnvelopes(envelopes);
                foreach (NmApiMessageEnvelope envelope, envelopes) {
                    QMessageId messageId(addIdPrefix(QString::number(envelope.id()), SymbianHelpers::EngineTypeFreestyle));
                    if (!m_excludeIdList.contains(messageId)) {
                        m_idList.append(messageId);   
                    }
                }
                delete operation.m_FolderSearch;
            } else {
                delete operation.m_AccountSearch;
            }
            m_searchOperations.removeAt(i);
            break;
        }
        i++;
    }

    if (m_searchOperations.count() > 0) {
        FSSearchOperation &operation = m_searchOperations[0]; 
        operation.m_Status = FSSearchOperation::SearchActive;
        if (operation.m_Type == FSSearchOperation::SearchFolder)
            operation.m_FolderSearch->start();
        else // operation.m_Type == FSSearchOperation::SearchAccount
            operation.m_AccountSearch->start();
    } else {
        QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
    }
}


void CFSMessagesFindOperation::searchCompleted()
{
    if (m_searchField != None) { 
        QMessageIdList idList;
        foreach (QMessageId messageId, m_idList) {
            if (fillsSearchKeyCriteria(messageId))
                idList.append(messageId);   
        }
        m_idList = idList;
    }
    m_owner.filterAndOrderMessagesReady(true, m_operationId, m_idList, 1, m_resultCorrectlyOrdered);
}

bool CFSMessagesFindOperation::fillsSearchKeyCriteria(QMessageId& messageId)
{
    QMessage message = m_owner.message(messageId);
    
    Qt::CaseSensitivity caseSensitivity = (m_matchFlags & QMessageDataComparator::MatchCaseSensitive) ?
        Qt::CaseSensitive:Qt::CaseInsensitive;
    
    switch (m_searchField) {
    case Sender: {
        return message.from().addressee().contains(m_searchKey, caseSensitivity);
    }
    case Recipients: {
        foreach (QMessageAddress toRecipient, message.to()) {
            if (toRecipient.addressee().contains(m_searchKey, caseSensitivity))
                return true;
        }
        foreach (QMessageAddress ccRecipient, message.cc()) {
            if (ccRecipient.addressee().contains(m_searchKey, caseSensitivity))
                return true;
        }
        foreach (QMessageAddress bccRecipient, message.bcc()) {
            if (bccRecipient.addressee().contains(m_searchKey, caseSensitivity))
                return true;
        }
        return false;
    }
    case Subject: {
        return message.subject().contains(m_searchKey, caseSensitivity);
    }
    case Body: {
        if (message.bodyId() == QMessageContentContainerPrivate::bodyContentId()) {
            // Message contains only body (not attachments)
            QString messageBody = message.textContent();
            return messageBody.contains(m_searchKey, caseSensitivity);
        } else {
            // Message contains body and attachments
            QMessageContentContainerIdList contentIds = message.contentIds();
            foreach (QMessageContentContainerId id, contentIds){
                QMessageContentContainer container = message.find(id);
                QMessageContentContainerPrivate* pPrivateContainer = QMessageContentContainerPrivate::implementation(container);
                if (pPrivateContainer->_id == message.bodyId()) {
                    // ContentContainer is body
                    return container.textContent().contains(m_searchKey, caseSensitivity);
                }
            }
        }
        break;
    }
    default:
        break;
    }
    return false;
}

CFSAsynchronousSendOperation::CFSAsynchronousSendOperation()
{
    
}

CFSAsynchronousSendOperation::~CFSAsynchronousSendOperation()
{
    
}

void CFSAsynchronousSendOperation::sendMessage(QMessageServicePrivate& privateService, QMessage &message)
{
    m_privateService = &privateService;
    m_qMessage = message;
    quint64 mailboxId = stripIdPrefix(m_qMessage.parentAccountId().toString()).toULongLong();
    m_manager = new NmApiMessageManager(this, mailboxId);
    createDraftMessage();
}
    
void CFSAsynchronousSendOperation::createDraftMessageCompleted(QVariant message, int success)
{
    if (success == 0) {
        if (message.canConvert<NmApiMessage>()) {
            m_fsMessage = qvariant_cast<NmApiMessage>(message);
        }
        createFSMessage(m_qMessage, m_fsMessage);
        saveMessage();
    }
    else
        emit messageSend(success, this);
}

void CFSAsynchronousSendOperation::saveCompleted(QVariant message, int success)
{
    if (success == 0)
        sendMessage();
    else
        emit messageSend(success, this);
}
    
void CFSAsynchronousSendOperation::sendCompleted(QVariant message, int success)
{
    m_privateService->setFinished(success);
    emit messageSend(success, this);
}
    
void CFSAsynchronousSendOperation::saveMessage()
{
    QPointer<NmApiOperation> operation = m_manager->saveMessage(m_fsMessage);
    qRegisterMetaType<EmailClientApi::NmApiMessage>("EmailClientApi::NmApiMessage");
    connect(operation, SIGNAL(operationComplete(int, QVariant)), this, SLOT(saveCompleted(QVariant, int)));
}
    
void CFSAsynchronousSendOperation::createDraftMessage()
{
    QPointer<NmApiOperation> operation = m_manager->createDraftMessage(NULL);
    qRegisterMetaType<EmailClientApi::NmApiMessage>("EmailClientApi::NmApiMessage");
    connect(operation, SIGNAL(operationComplete(int, QVariant)), this, SLOT(createDraftMessageCompleted(QVariant, int)));
}
    
void CFSAsynchronousSendOperation::sendMessage()
{
    QPointer<NmApiOperation> operation = m_manager->sendMessage(m_fsMessage);
    qRegisterMetaType<EmailClientApi::NmApiMessage>("EmailClientApi::NmApiMessage");
    connect(operation, SIGNAL(operationComplete(int, QVariant)), this, SLOT(sendCompleted(QVariant, int)));
}

CFSAsynchronousAddOperation::CFSAsynchronousAddOperation()
{
    
}

CFSAsynchronousAddOperation::~CFSAsynchronousAddOperation()
{

}

void CFSAsynchronousAddOperation::addMessage(QMessage &message)
{
    m_qMessage = message;
    quint64 mailboxId = stripIdPrefix(m_qMessage.parentAccountId().toString()).toULongLong();
    m_manager = new NmApiMessageManager(this, mailboxId);
    createDraftMessage();
}
    
void CFSAsynchronousAddOperation::createDraftMessageCompleted(int success, QVariant message)
{
    if (success == 0) {
        if (message.canConvert<NmApiMessage>()) {
            m_fsMessage = qvariant_cast<NmApiMessage>(message);
        }
        createFSMessage(m_qMessage, m_fsMessage);
        saveMessage();
    }
    else
        emit messageAdded(success, this);
}

void CFSAsynchronousAddOperation::saveCompleted(QVariant message, int success)
{
    emit messageAdded(success, this);
}
    
void CFSAsynchronousAddOperation::saveMessage()
{
    QPointer<NmApiOperation> operation = m_manager->saveMessage(m_fsMessage);
    qRegisterMetaType<EmailClientApi::NmApiMessage>("EmailClientApi::NmApiMessage");
    connect(operation, SIGNAL(operationComplete(int, QVariant)), this, SLOT(saveCompleted(QVariant, int)));
}
    
void CFSAsynchronousAddOperation::createDraftMessage()
{
    QPointer<NmApiOperation> operation = m_manager->createDraftMessage(NULL);
    qRegisterMetaType<EmailClientApi::NmApiMessage>("EmailClientApi::NmApiMessage");
    connect(operation, SIGNAL(operationComplete(int, QVariant)), this, SLOT(createDraftMessageCompleted(int, QVariant)));
}

CFSAsynchronousRetrieveBodyOperation::CFSAsynchronousRetrieveBodyOperation()
{
    
}

CFSAsynchronousRetrieveBodyOperation::~CFSAsynchronousRetrieveBodyOperation()
{

}

void CFSAsynchronousRetrieveBodyOperation::retrieveBody(QMessageId &messageId, QMessageServicePrivate &privateService)
{
    m_privateService = &privateService;
}

CFSAsynchronousRetrieveAttachmentOperation::CFSAsynchronousRetrieveAttachmentOperation()
{
    
}

CFSAsynchronousRetrieveAttachmentOperation::~CFSAsynchronousRetrieveAttachmentOperation()
{

}

void CFSAsynchronousRetrieveAttachmentOperation::retrieveAttachment(QMessageId &messageId, QMessageServicePrivate &privateService)
{
    m_privateService = &privateService;
}

#include "..\..\build\Release\QtMessaging\moc\moc_qfsnmengine_symbian_p.cpp";

QTM_END_NAMESPACE
