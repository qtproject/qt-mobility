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
#include "qfsengine_symbian_p.h"
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

#include <emailinterfacefactory.h>
#include <QTextCodec>
#include <emailapidefs.h>
#include <memailmailbox.h>
#include <memailfolder.h>
#include <memailmessage.h>
#include <memailaddress.h>
#include <memailcontent.h>
#include <mmessageiterator.h>

using namespace EmailInterface;

QTM_BEGIN_NAMESPACE

using namespace SymbianHelpers;

Q_GLOBAL_STATIC(CFSEngine,fsEngine);

CFSEngine::CFSEngine()
{
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(cleanupFSBackend()));

    TRAPD(err, {
        m_factory = CEmailInterfaceFactory::NewL(); 
        m_ifPtr = m_factory->InterfaceL(KEmailClientApiInterface);
    });
    
    Q_UNUSED(err);
    m_clientApi = static_cast<MEmailClientApi*>(m_ifPtr);
#ifdef FREESTYLEMAILBOXOBSERVERUSED
    TRAPD(err2, setPluginObserversL());
    Q_UNUSED(err2);
#endif
}

CFSEngine::~CFSEngine()
{

}

void CFSEngine::cleanupFSBackend()
{
    m_mtmAccountList.clear();
    for (TInt i = 0; i < m_attachments.Count(); i++){
        m_attachments[i]->Release();
    }
    m_attachments.Reset();
    for (TInt i = 0; i < m_mailboxes.Count(); i++){
        m_mailboxes[i]->Release();
    }
    m_mailboxes.Reset();
    m_clientApi->Release();
    delete m_factory;
}

CFSEngine* CFSEngine::instance()
{   
    return fsEngine();
}

bool CFSEngine::accountLessThan(const QMessageAccountId accountId1, const QMessageAccountId accountId2)
{
    CFSEngine* freestyleEngine = fsEngine();
    return QMessageAccountSortOrderPrivate::lessThan(freestyleEngine->m_currentAccountOrdering,
        freestyleEngine->account(accountId1),
        freestyleEngine->account(accountId2));
}

void CFSEngine::orderAccounts(QMessageAccountIdList& accountIds, const QMessageAccountSortOrder &sortOrder) const
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

void CFSEngine::orderFolders(QMessageFolderIdList& folderIds,  const QMessageFolderSortOrder &sortOrder) const
{
    m_currentFolderOrdering = sortOrder;
    qSort(folderIds.begin(), folderIds.end(), CFSEngine::folderLessThan);
}

bool CFSEngine::messageLessThan(const QMessage& message1, const QMessage& message2)
{
    CFSEngine* freestyleEngine = fsEngine();
    return QMessageSortOrderPrivate::lessThan(freestyleEngine->m_currentMessageOrdering, message1, message2);
}

void CFSEngine::orderMessages(QMessageIdList& messageIds, const QMessageSortOrder &sortOrder) const
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
    for (TInt i = 0; i < accountList.count(); i++) {
        m_mtmAccountList.append(stripIdPrefix(accountList[i]));
    }
}

QMessageAccountIdList CFSEngine::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageAccountIdList accountIds;

    TRAPD(err, updateEmailAccountsL());
    Q_UNUSED(err);
    
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

void CFSEngine::applyOffsetAndLimitToAccountIds(QMessageAccountIdList& idList, int offset, int limit) const
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
    TRAPD(err, updateEmailAccountsL());
    Q_UNUSED(err)
    return m_accounts[id.toString()];
}

QMessageAccountId CFSEngine::defaultAccount(QMessage::Type type) const
{
    // TODO
    Q_UNUSED(type);
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


void CFSEngine::updateEmailAccountsL() const
{
    QStringList keys = m_accounts.keys();
    RMailboxPtrArray mailboxes;
    CleanupResetAndRelease<MEmailMailbox>::PushL(mailboxes);
    
    m_clientApi->GetMailboxesL(mailboxes);
    
    for (TInt i = 0; i < mailboxes.Count(); i++) {
        MEmailMailbox *mailbox = mailboxes[i];
        QString idAsString = QString::number(mailbox->MailboxId().iId);
        QString fsIdAsString = addIdPrefix(idAsString, SymbianHelpers::EngineTypeFreestyle);
        TBool overlap = false;
        for (TInt j = 0; j < m_mtmAccountList.count(); j++) {
            if (idAsString == m_mtmAccountList[j].toString())
                overlap = true;
        }
        if (!m_accounts.contains(fsIdAsString) && !overlap) {     
            QMessageAccount account = QMessageAccountPrivate::from(
                                      QMessageAccountId(fsIdAsString),
                                      QString::fromUtf16(mailbox->MailboxName().Ptr(), mailbox->MailboxName().Length()),
                                      0,
                                      0,
                                      QMessage::Email);
          
            m_accounts.insert(fsIdAsString, account);
            
        } else {
            keys.removeOne(fsIdAsString);
        }
        mailbox->Release();
    }  
    
    mailboxes.Reset();
    CleanupStack::PopAndDestroy();
    
    for (int i=0; i < keys.count(); i++) {
        m_accounts.remove(keys[i]);
    }   
}

#ifdef FREESTYLEMAILBOXOBSERVERUSED
void CFSEngine::setPluginObserversL()
{
    m_clientApi->GetMailboxesL(m_mailboxes);    
    for (TInt i = 0; i < m_mailboxes.Count(); i++) {
        MEmailMailbox *mailbox = m_mailboxes[i];
        mailbox->RegisterObserverL(*this);
    }  
}

void CFSEngine::NewMessageEventL(const TMailboxId& aMailbox, const REmailMessageIdArray aNewMessages, const TFolderId& aParentFolderId)
{
    QMessageManager::NotificationFilterIdSet matchingFilters;
    QMessageStorePrivate::NotificationType notificationType = QMessageStorePrivate::Added;
  
    for (TInt i = 0; i < aNewMessages.Count(); i++) {
        TMessageId messageId(aNewMessages[i]);
        notificationL(aMailbox, messageId, aParentFolderId, notificationType);
    }
}

void CFSEngine::MessageChangedEventL(const TMailboxId& aMailbox, const REmailMessageIdArray aChangedMessages, const TFolderId& aParentFolderId)
{
    QMessageManager::NotificationFilterIdSet matchingFilters;
    QMessageStorePrivate::NotificationType notificationType = QMessageStorePrivate::Updated;
  
    for (TInt i = 0; i < aChangedMessages.Count(); i++) {
        TMessageId messageId(aChangedMessages[i]);
        notificationL(aMailbox, messageId, aParentFolderId, notificationType);
    }
}

void CFSEngine::MessageDeletedEventL(const TMailboxId& aMailbox, const REmailMessageIdArray aDeletedMessages, const TFolderId& aParentFolderId)
{
    // TODO: add filter handling
    QMessageManager::NotificationFilterIdSet matchingFilters;
    QMap<int, QMessageFilter> filters(m_filters);
    QMap<int, QMessageFilter>::const_iterator it = filters.begin(), end = filters.end();  
    QMessageStorePrivate::NotificationType notificationType = QMessageStorePrivate::Removed;
    MEmailMailbox* mailbox = m_clientApi->MailboxL(aMailbox);
    MEmailFolder* folder = mailbox->FolderL(aParentFolderId); 
    QString idAsString = QString::number(mailbox->MailboxId().iId);
    for (TInt j = 0; j < m_mtmAccountList.count(); j++) {
        if (idAsString == m_mtmAccountList[j].toString())
            return;
    } 
    for (TInt i = 0; i < aDeletedMessages.Count(); i++) {
        for ( ; it != end; ++it) {
            // Empty filter matches to all messages
            matchingFilters.insert(it.key());
        }
        TMessageId messageId(aDeletedMessages[i]);
        ipMessageStorePrivate->messageNotification(notificationType, 
                            QMessageId(addIdPrefix(QString::number(messageId.iId), SymbianHelpers::EngineTypeFreestyle)), 
                            matchingFilters);
    }
    folder->Release();
    mailbox->Release();
}

void CFSEngine::notificationL(const TMailboxId& aMailbox, const TMessageId& aMessageId, 
                                const TFolderId& aParentFolderId, QMessageStorePrivate::NotificationType aNotificationType)
{
    Q_UNUSED(aParentFolderId);
    QMessageManager::NotificationFilterIdSet matchingFilters;
    // Copy the filter map to protect against modification during traversal
    QMap<int, QMessageFilter> filters(m_filters);
    QMap<int, QMessageFilter>::const_iterator it = filters.begin(), end = filters.end();
    QMessage message;
    QMessageId realMessageId = QMessageId(addIdPrefix(QString::number(aMessageId.iId), SymbianHelpers::EngineTypeFreestyle));
    bool messageRetrieved = false;    
    MEmailMailbox* mailbox = m_clientApi->MailboxL(aMailbox);
    CleanupReleasePushL(*mailbox);
    QString idAsString = QString::number(mailbox->MailboxId().iId);
    for (TInt j = 0; j < m_mtmAccountList.count(); j++) {
        if (idAsString == m_mtmAccountList[j].toString()) {
            CleanupStack::Pop(mailbox);
            return;
        }
    }   
    for ( ; it != end; ++it) {
        const QMessageFilter &filter(it.value());
        if (!messageRetrieved) {
            MEmailMessage* fsMessage = mailbox->MessageL(aMessageId);
            CleanupReleasePushL(*fsMessage);
            
            if (!fsMessage) {
                CleanupStack::Pop(fsMessage);
                CleanupStack::Pop(mailbox);
                return;
            }
            message = CreateQMessageL(fsMessage);
            messageRetrieved = true;
            CleanupStack::Pop(fsMessage);
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
    
    CleanupStack::Pop(mailbox);   
}

#endif

MEmailMessage* CFSEngine::createFSMessageL(const QMessage &message, const MEmailMailbox* mailbox)
{
    MEmailMessage* fsMessage = mailbox->CreateDraftMessageL();
    CleanupReleasePushL(*fsMessage);
    
    switch (message.priority()) {
        case QMessage::HighPriority:
            fsMessage->SetFlag(EmailInterface::EFlag_Important);
            fsMessage->ResetFlag(EmailInterface::EFlag_Low);
            break;
        case QMessage::NormalPriority:
            fsMessage->ResetFlag(EmailInterface::EFlag_Important);
            fsMessage->ResetFlag(EmailInterface::EFlag_Low);
            break;
        case QMessage::LowPriority:
            fsMessage->SetFlag(EmailInterface::EFlag_Low);
            fsMessage->ResetFlag(EmailInterface::EFlag_Important);
            break;            
        }
        if (message.status() & QMessage::Read) {
            fsMessage->SetFlag(EmailInterface::EFlag_Read);
        } else {
            fsMessage->ResetFlag(EmailInterface::EFlag_Read);
        }
    
    MEmailAddress* sender = mailbox->AddressL();
    sender->SetRole(MEmailAddress::ESender);
    fsMessage->SetReplyToAddressL(*sender);
        
    QList<QMessageAddress> toList(message.to());
    if (toList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        REmailAddressArray toAddress;
        for (int i = 0; i < toList.size(); ++i) {
            qreceiver = toList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            MEmailAddress* address = mailbox->AddressL();
            address->SetAddressL(receiver);
            address->SetDisplayNameL(receiver);
            address->SetRole(MEmailAddress::ETo);
            toAddress.Append(address);
        }
        fsMessage->SetRecipientsL(MEmailAddress::ETo, toAddress);
    }
    
    QList<QMessageAddress> ccList(message.cc());
    if (ccList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        REmailAddressArray ccAddress;
        for (int i = 0; i < ccList.size(); ++i) {
            qreceiver = ccList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            MEmailAddress* address = mailbox->AddressL();
            address->SetDisplayNameL(receiver);
            address->SetRole(MEmailAddress::ECc);
            address->SetAddressL(receiver);
            ccAddress.Append(address);
        }
        fsMessage->SetRecipientsL(MEmailAddress::ECc, ccAddress);
    }
        
    QList<QMessageAddress> bccList(message.bcc());
    if (bccList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        REmailAddressArray bccAddress;
        for (int i = 0; i < bccList.size(); ++i) {
            qreceiver = bccList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            MEmailAddress* address = mailbox->AddressL();
            address->SetDisplayNameL(receiver);
            address->SetRole(MEmailAddress::EBcc);
            address->SetAddressL(receiver);
            bccAddress.Append(address);
        }
        fsMessage->SetRecipientsL(MEmailAddress::EBcc, bccAddress);
    }
    if (message.bodyId() == QMessageContentContainerPrivate::bodyContentId()) {
        // Message contains only body (not attachments)
        QString messageBody = message.textContent();
        if (!messageBody.isEmpty()) {
            QByteArray type = message.contentType();
            QByteArray subType = message.contentSubType();
            MEmailMessageContent* content = fsMessage->ContentL();
            MEmailTextContent* textContent = content->AsTextContentOrNull();
            if (textContent) {
                if (type == "text" && subType == "plain") {
                    textContent->SetTextL(MEmailTextContent::EPlainText, TPtrC(reinterpret_cast<const TUint16*>(message.textContent().utf16())));
                } 
               else if (type == "text" && subType == "html") {
                    textContent->SetTextL(MEmailTextContent::EHtmlText, TPtrC(reinterpret_cast<const TUint16*>(message.textContent().utf16())));
                }
            }
            else
                fsMessage->SetPlainTextBodyL(TPtrC(reinterpret_cast<const TUint16*>(message.textContent().utf16())));
        }
    } else {
        // Message contains body and attachments
        QMessageContentContainerIdList contentIds = message.contentIds();
        foreach (QMessageContentContainerId id, contentIds){
            QMessageContentContainer container = message.find(id);
            MEmailMessageContent* content = fsMessage->ContentL(); 
            QMessageContentContainerPrivate* pPrivateContainer = QMessageContentContainerPrivate::implementation(container);
            if (pPrivateContainer->_id == message.bodyId()) {
                // ContentContainer is body
                if (!container.textContent().isEmpty()) {               
                    MEmailTextContent* textContent = content->AsTextContentOrNull();
                    if (textContent) {
                        QByteArray type = container.contentType();
                        QByteArray subType = container.contentSubType();
                        if (type == "text" && subType == "plain") {
                            textContent->SetTextL(MEmailTextContent::EPlainText, TPtrC(reinterpret_cast<const TUint16*>(container.textContent().utf16())));
                        }
                        else if (type == "text" && subType == "html") {
                            textContent->SetTextL(MEmailTextContent::EHtmlText, TPtrC(reinterpret_cast<const TUint16*>(container.textContent().utf16())));
                        } 
                    }
                    else
                        fsMessage->SetPlainTextBodyL(TPtrC(reinterpret_cast<const TUint16*>(container.textContent().utf16())));
                }
            } else {
                // ContentContainer is attachment
                QByteArray filePath = QMessageContentContainerPrivate::attachmentFilename(container);
                // Replace Qt style path separator "/" with Symbian path separator "\"
                filePath.replace(QByteArray("/"), QByteArray("\\"));
                QString temp_path = QString(filePath);
                TPtrC16 attachmentPath(KNullDesC);
                attachmentPath.Set(reinterpret_cast<const TUint16*>(temp_path.utf16()));
                fsMessage->AddAttachmentL(attachmentPath);
            }        
        }
    }
    fsMessage->SetSubjectL(TPtrC(reinterpret_cast<const TUint16*>(message.subject().utf16())));
    
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_id = QMessageId(addIdPrefix(QString::number(fsMessage->MessageId().iId),SymbianHelpers::EngineTypeFreestyle));
    
    fsMessage->SaveChangesL();
    CleanupStack::Pop(fsMessage);
    return fsMessage;
}

bool CFSEngine::addMessage(QMessage* message)
{
    TMailboxId mailboxId(stripIdPrefix(message->parentAccountId().toString()).toInt());
    MEmailMailbox* mailbox = NULL;
    TRAPD(mailerr, mailbox = m_clientApi->MailboxL(mailboxId));
    if (mailerr != KErrNone)
        return false;

    MEmailMessage* fsMessage = NULL;
    TRAPD(err, fsMessage = createFSMessageL(*message, mailbox));
    if (fsMessage)
        fsMessage->Release();
    if (mailbox)
        mailbox->Release();
    
    if (err != KErrNone)
        return false;
    else
        return true;
}

bool CFSEngine::updateMessage(QMessage* message)
{
    TRAPD(err, updateMessageL(message));
    if (err != KErrNone)
        return false;
    else
        return true;
}

void CFSEngine::updateMessageL(QMessage* message)
{
    TMailboxId mailboxId(stripIdPrefix(message->parentAccountId().toString()).toInt());
    MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);
    CleanupReleasePushL(*mailbox);
  
    TMessageId messageId(message->id().toString().toInt(),
                            message->parentFolderId().toString().toInt(), 
                            mailboxId);
    MEmailMessage* fsMessage = mailbox->MessageL(messageId);
    CleanupReleasePushL(*fsMessage);
    
    switch (message->priority()) {
        case QMessage::HighPriority:
            fsMessage->SetFlag(EmailInterface::EFlag_Important);
            fsMessage->ResetFlag(EmailInterface::EFlag_Low);
            break;
        case QMessage::NormalPriority:
            fsMessage->ResetFlag(EmailInterface::EFlag_Important);
            fsMessage->ResetFlag(EmailInterface::EFlag_Low);
            break;
        case QMessage::LowPriority:
            fsMessage->SetFlag(EmailInterface::EFlag_Low);
            fsMessage->ResetFlag(EmailInterface::EFlag_Important);
            break;            
        }
        if (message->status() & QMessage::Read) {
            fsMessage->SetFlag(EmailInterface::EFlag_Read);
        } else {
            fsMessage->ResetFlag(EmailInterface::EFlag_Read);
        }
        
    MEmailAddress* sender = mailbox->AddressL();
    sender->SetRole(MEmailAddress::ESender);
    fsMessage->SetReplyToAddressL(*sender);
        
    QList<QMessageAddress> toList(message->to());
    if (toList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        REmailAddressArray toAddress;
        for (int i = 0; i < toList.size(); ++i) {
            qreceiver = toList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            MEmailAddress* address = mailbox->AddressL();
            address->SetAddressL(receiver);
            toAddress.Append(address);
        }
        fsMessage->SetRecipientsL(MEmailAddress::ETo, toAddress);
    }
    
    QList<QMessageAddress> ccList(message->cc());
    if (ccList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        REmailAddressArray ccAddress;
        for (int i = 0; i < ccList.size(); ++i) {
            qreceiver = ccList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            MEmailAddress* address = mailbox->AddressL();;
            address->SetAddressL(receiver);
            ccAddress.Append(address);
        }
        fsMessage->SetRecipientsL(MEmailAddress::ECc, ccAddress);
    }
        
    QList<QMessageAddress> bccList(message->bcc());
    if (bccList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        REmailAddressArray bccAddress;
        for (int i = 0; i < bccList.size(); ++i) {
            qreceiver = bccList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            MEmailAddress* address = mailbox->AddressL();;
            address->SetAddressL(receiver);
            bccAddress.Append(address);
        }
        fsMessage->SetRecipientsL(MEmailAddress::EBcc, bccAddress);
    }
    
    if (message->bodyId() == QMessageContentContainerPrivate::bodyContentId()) {
        // Message contains only body (not attachments)
        QString messageBody = message->textContent();
        if (!messageBody.isEmpty()) {
            MEmailMessageContent* content = fsMessage->ContentL();
            MEmailTextContent* textContent = content->AsTextContentOrNull();
            textContent->SetTextL(MEmailTextContent::EPlainText, TPtrC(reinterpret_cast<const TUint16*>(message->textContent().utf16())));
            // TODO:
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
                    MEmailMessageContent* content = fsMessage->ContentL();
                    MEmailTextContent* textContent = content->AsTextContentOrNull();
                    QByteArray type = container.contentType();
                    QByteArray subType = container.contentSubType();
                    if (type == "text" && subType == "plain") {
                        textContent->SetTextL(MEmailTextContent::EPlainText, TPtrC(reinterpret_cast<const TUint16*>(container.textContent().utf16())));
                    }
                    else if (type == "text" && subType == "html") {
                        textContent->SetTextL(MEmailTextContent::EHtmlText, TPtrC(reinterpret_cast<const TUint16*>(container.textContent().utf16())));
                    }
                }
            } else {
                // ContentContainer is attachment
                QByteArray filePath = QMessageContentContainerPrivate::attachmentFilename(container);
                // Replace Qt style path separator "/" with Symbian path separator "\"
                filePath.replace(QByteArray("/"), QByteArray("\\"));
                QString temp_path = QString(filePath);
                TPtrC16 attachmentPath(KNullDesC);
                attachmentPath.Set(reinterpret_cast<const TUint16*>(temp_path.utf16()));
                fsMessage->AddAttachmentL(attachmentPath);
            }        
        }
    }
    
    fsMessage->SetSubjectL(TPtrC(reinterpret_cast<const TUint16*>(message->subject().utf16())));
    fsMessage->SaveChangesL();
    CleanupStack::PopAndDestroy(fsMessage);
    CleanupStack::PopAndDestroy(mailbox);
}

bool CFSEngine::removeMessage(const QMessageId &id, QMessageManager::RemovalOption option)
{
    Q_UNUSED(option);
    bool retVal = false;
    foreach (QMessageAccount account, m_accounts) {
        MEmailMessage* message = NULL;
        TMailboxId mailboxId(stripIdPrefix(account.id().toString()).toInt());
        MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);
        
        TMessageId messageId(
            stripIdPrefix(id.toString()).toInt(),
            0, 
            mailboxId);
        
        TRAPD(err, message = mailbox->MessageL(messageId));

        if (err == KErrNone) {
            TFolderId folderId(message->ParentFolderId());
            TRAPD(err2,
                MEmailFolder* folder = mailbox->FolderL(folderId);
                REmailMessageIdArray messageIds;
                messageIds.Append(message->MessageId());
                folder->DeleteMessagesL(messageIds);
                folder->Release();
                );
            if (err2 == KErrNone)
                retVal = true;
            mailbox->Release();
            break; // no need to continue
        }
        mailbox->Release();
    }
    return retVal;
}

bool CFSEngine::showMessage(const QMessageId &id)
{
    bool retVal = false;
    foreach (QMessageAccount account, m_accounts) {
        MEmailMessage* message = NULL;
        TMailboxId mailboxId(stripIdPrefix(account.id().toString()).toInt());
        MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);
        
        TMessageId messageId(
            stripIdPrefix(id.toString()).toInt(),
            0, 
            mailboxId);
        
        TRAPD(err, message = mailbox->MessageL(messageId));

        if (err == KErrNone) {
            TRAPD(err2, message->ShowMessageViewerL());
            if (err2 == KErrNone)
                retVal = true;
            message->Release();
            mailbox->Release();
            break; // no need to continue
        }
        mailbox->Release();
    } 
    return retVal;
}

bool CFSEngine::composeMessage(const QMessage &message)
{
    bool retVal = false;
    MEmailMailbox* mailbox = NULL;
    TMailboxId mailboxId(stripIdPrefix(message.parentAccountId().toString()).toInt());
    TRAPD(err, mailbox = m_clientApi->MailboxL(mailboxId));
    if (err == KErrNone) {
        TRAPD(err2, mailbox->EditNewMessageL());
        if (err2 == KErrNone)
            retVal = true;
        mailbox->Release();
    }
    return retVal;
}

bool CFSEngine::retrieve(QMessageServicePrivate& privateService, const QMessageId &messageId, const QMessageContentContainerId& id)
{
    Q_UNUSED(id);
    m_privateService = &privateService;
    bool retVal = false;
    foreach (QMessageAccount account, m_accounts) {
        MEmailMessage* message = NULL;
        TMailboxId mailboxId(stripIdPrefix(account.id().toString()).toInt());
        MEmailMailbox* mailbox = NULL;
        TRAPD(mailboxError, mailbox = m_clientApi->MailboxL(mailboxId));
        if (mailboxError == KErrNone) {       
            TMessageId mId(
                stripIdPrefix(messageId.toString()).toInt(),
                0, 
                mailboxId);
            
            TRAPD(err, message = mailbox->MessageL(mId));
            if (err == KErrNone) {
                MEmailMessageContent* content = NULL;
                TRAPD(contentError, content = message->ContentL());
                if (contentError == KErrNone) {
                    TRAPD(err, retrieveAttachmentsL(message));
                        if (err == KErrNone)
                            retVal = true;
                        }
                }
                message->Release();
                mailbox->Release();
                break; // no need to continue
        }
        mailbox->Release();
    } 
    return retVal;
}

bool CFSEngine::retrieveBody(QMessageServicePrivate& privateService, const QMessageId& id)
{
    bool retVal = false;
    m_privateService = &privateService;
    foreach (QMessageAccount account, m_accounts) {
        MEmailMessage* message = NULL;
        TMailboxId mailboxId(stripIdPrefix(account.id().toString()).toInt());
        MEmailMailbox* mailbox = NULL;
        TRAPD(mailBoxError, mailbox = m_clientApi->MailboxL(mailboxId));
        if (mailBoxError == KErrNone) {
            TMessageId messageId(
                stripIdPrefix(id.toString()).toInt(),
                0, 
                mailboxId);
            
            TRAPD(err, message = mailbox->MessageL(messageId));
            if (err == KErrNone) {
                MEmailMessageContent* content = NULL;
                TRAPD(contentError, content = message->ContentL());
                if (contentError == KErrNone) { 
                    TRAPD(err, retrieveTotalBodyL(content));
                    if (err == KErrNone)
                        retVal = true;
                    }
                }
                message->Release();
                mailbox->Release();
                break; // no need to continue
            }
            mailbox->Release();
        } 
    return retVal;
}

void CFSEngine::retrieveTotalBodyL(MEmailMessageContent* aContent)
{
    MEmailMultipart* mPart = aContent->AsMultipartOrNull();
    if (mPart) {
        TInt partCount = 0;
        TRAPD(err, partCount = mPart->PartCountL());
            if (err == KErrNone) {
                for (TInt i = 0; i < partCount; i++) {
                    MEmailMessageContent* content = NULL;
                    TRAPD(err2, content = mPart->PartByIndexL(i));
                    if (err2 == KErrNone) {
                        retrieveTotalBodyL(content);
                        content->Release();
                    }
                }
            }
            return;
        }
 
    MEmailTextContent* textContent = aContent->AsTextContentOrNull();
    if (textContent) { 
        TInt availableSize = textContent->AvailableSize();
        TInt totalSize = textContent->TotalSize();
        if (totalSize > availableSize) {
            TRAPD(textErr, textContent->FetchL(*this));
            Q_UNUSED(textErr);
        }      
    }   
    return;
}

void CFSEngine::retrieveAttachmentsL(MEmailMessage* aMessage)
{
    for (TInt i = 0; i < m_attachments.Count(); i++) {
        m_attachments[i]->Release();
    }
    m_attachments.Reset();
    TInt count = aMessage->GetAttachmentsL(m_attachments);
    for(TInt i = 0; i < count; i++) {
        MEmailAttachment* att = m_attachments[i];
        int totalSize = att->TotalSize();
        int availableSize = att->AvailableSize();
        if (totalSize > availableSize) {
            TRAPD(err, att->FetchL(*this));
            Q_UNUSED(err);
        }
    }
}

bool CFSEngine::retrieveHeader(QMessageServicePrivate& privateService, const QMessageId& id)
{
    Q_UNUSED(id);
    Q_UNUSED(privateService);
    return false;
}

void CFSEngine::DataFetchedL(const TInt aResult)
{
    if (aResult == KErrNone)
        m_privateService->setFinished(true);
    else
        m_privateService->setFinished(false);      
}

bool CFSEngine::exportUpdates(const QMessageAccountId &id)
{
    TRAPD(err, exportUpdatesL(id));
    if (err != KErrNone) {
        return false;
    } else {
        return true;
    }
}

void CFSEngine::exportUpdatesL(const QMessageAccountId &id)
{
    TMailboxId mailboxId(stripIdPrefix(id.toString()).toInt());
    MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);
    mailbox->SynchroniseL(*this);
    mailbox->Release();
}

void CFSEngine::MailboxSynchronisedL(TInt aResult)
{
    Q_UNUSED(aResult);
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

bool CFSEngine::queryMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    TRAPD(err, queryMessagesL(privateService, filter, sortOrder, limit, offset));
    if (err != KErrNone) {
        return false;
    }
    return true;
}


void CFSEngine::queryMessagesL(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
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

bool CFSEngine::queryMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    TRAPD(err, queryMessagesL(privateService, filter, body, matchFlags, sortOrder, limit, offset));
    if (err != KErrNone) {
        return false;
    }
    return true;
}

void CFSEngine::queryMessagesL(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
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

bool CFSEngine::countMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter)
{
    TRAPD(err, countMessagesL(privateService, filter));
    if (err != KErrNone) {
        return false;
    }
    return true;
}

void CFSEngine::countMessagesL(QMessageServicePrivate& privateService, const QMessageFilter &filter)
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
    int index=0;
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
    m_messageQueries.removeAt(index);
}

void CFSEngine::applyOffsetAndLimitToMsgIds(QMessageIdList& idList, int offset, int limit) const
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

QMessageManager::NotificationFilterId CFSEngine::registerNotificationFilter(QMessageStorePrivate& aPrivateStore,
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

void CFSEngine::applyOffsetAndLimitToMsgFolderIds(QMessageFolderIdList& idList, int offset, int limit) const
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
    //return QMessageFolder();
    
    QMessageFolder folder;
    TRAPD(err, folder = folderL(id));
    Q_UNUSED(err)
       
    return folder;
}

QMessageFolder CFSEngine::folderL(const QMessageFolderId &id) const
{
    QMessageFolder folder;
    MEmailMailbox* mailbox = NULL;
    QMessageFolderId parentId;
    QMessageAccountId accountId;

    // get account containing folder
    TRAPD(err, updateEmailAccountsL());
    Q_UNUSED(err)
    foreach (QMessageAccount value, m_accounts) {
        accountId = value.id();
        QMessageFolderIdList ids = folderIdsByAccountIdL(accountId);
        if (ids.contains(id)) {
            TMailboxId mailboxId(stripIdPrefix(accountId.toString()).toInt());
            mailbox = m_clientApi->MailboxL(mailboxId);
            CleanupReleasePushL(*mailbox);
            TFolderId folderId(stripIdPrefix(id.toString()).toInt(), mailbox->MailboxId());
            MEmailFolder* emailFolder = mailbox->FolderL(folderId);
            CleanupReleasePushL(*emailFolder);
            QString name = QString::fromUtf16(emailFolder->Name().Ptr(), emailFolder->Name().Length());
            folder = QMessageFolderPrivate::from(id, accountId, parentId, name, name);
            CleanupStack::PopAndDestroy(emailFolder);
            CleanupStack::PopAndDestroy(mailbox);
            break;
        }
    }
    return folder;
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
                    MEmailMailbox* mailbox = NULL;
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
                    }
                } else {
                    if (cmp == QMessageDataComparator::NotEqual) {
                        ids = allFolders();
                    }
                }
                filterHandled = true;
            } else if (pf->_comparatorType == QMessageFolderFilterPrivate::Inclusion) {
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (pf->_ids.count() > 0) { // QMessageIdList
                    QMessageFolderIdList ids2;
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
                    }
                    filterHandled = true;
                } else {
                    // Empty QMessageIdList as a list
                    if (cmp == QMessageDataComparator::Excludes) {
                        ids = allFolders();
                    }
                    filterHandled = true;
                
                    // QMessageFilter 
                    /*if (cmp == QMessageDataComparator::Includes) {
                        // TODO:
                    } else { // Excludes
                        // TODO:
                    }*/
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
                        ids = folderIdsByAccountIdL(QMessageAccountId(pf->_value.toString()));
                    }
                } else { // NotEqual
                    ids = allFolders();
                    if (pf->_value.toString().length() > 0) {
                        QMessageFolderIdList ids2 = folderIdsByAccountIdL(QMessageAccountId(pf->_value.toString()));
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
                    MEmailMailbox* mailbox = NULL;
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
                    }
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
    TRAPD(err, updateEmailAccountsL());
    Q_UNUSED(err)
    foreach (QMessageAccount value, m_accounts) {
        QMessageFolderIdList ids2 = folderIdsByAccountId(value.id());
        ids << ids2;
    }
    return ids;
}

QMessageFolderIdList CFSEngine::folderIdsByAccountId(const QMessageAccountId& accountId) const
{
    QMessageFolderIdList idList;
    TRAPD(err, idList << folderIdsByAccountIdL(accountId))
    Q_UNUSED(err);
    return idList;
}

QMessageFolderIdList CFSEngine::folderIdsByAccountIdL(const QMessageAccountId& accountId) const
{
    QMessageFolderIdList folderIds;
    
    if (idType(accountId) != EngineTypeFreestyle)
        return QMessageFolderIdList();
    
    QMessageAccount messageAccount = account(accountId);
    
    TMailboxId mailboxId(stripIdPrefix(accountId.toString()).toInt());
    MEmailMailbox* mailbox = NULL;
    mailbox = m_clientApi->MailboxL(mailboxId);

    if (mailbox == NULL)
        return QMessageFolderIdList();

    CleanupReleasePushL(*mailbox);

    RFolderArray folders;
    
    mailbox->GetFoldersL(folders);
    CleanupClosePushL(folders);

    for(TInt i=0; i < folders.Count(); i++) {
        MEmailFolder *mailFolder = folders[i];
        
        QString fsIdAsString = addIdPrefix(QString::number(mailFolder->FolderId().iId), SymbianHelpers::EngineTypeFreestyle);
        folderIds.append(QMessageFolderId(fsIdAsString));

        //TODO: Support for subfolders?
        mailFolder->Release();
    }
    
    CleanupStack::PopAndDestroy(&folders);
    CleanupStack::PopAndDestroy(mailbox);
    
    return folderIds;
}

bool CFSEngine::fsFolderL(const QMessageFolderId& id, MEmailMailbox* mailbox, MEmailFolder* folder) const
{
    Q_UNUSED(folder);
    MEmailFolder* fsFolder = NULL;
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
    folder = NULL;
    return false;
}


QMessage CFSEngine::message(const QMessageId& id) const
{
    QMessage message = QMessage();
    TRAPD(err, message = messageL(id));
    Q_UNUSED(err);
    return message;
}

QMessage CFSEngine::messageL(const QMessageId& id) const
{
    QMessage message = QMessage();
    foreach (QMessageAccount account, m_accounts) {
        TMailboxId mailboxId(stripIdPrefix(account.id().toString()).toInt());
        MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);
        CleanupReleasePushL(*mailbox);
        
        TMessageId messageId(
            stripIdPrefix(id.toString()).toInt(),
            0, //stripIdPrefix(folderId.toString()).toInt(), 
            mailboxId);
            
        MEmailMessage* fsMessage = NULL;
        
        TRAPD(err, fsMessage = mailbox->MessageL(messageId));
        if (err == KErrNone) {
            CleanupReleasePushL(*fsMessage);
            message = CreateQMessageL(fsMessage);
            
            QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
            privateMessage->_id = id; 
            privateMessage->_modified = false;
                
            CleanupStack::PopAndDestroy(fsMessage);
            CleanupStack::PopAndDestroy(mailbox);
            return message;
        }
        CleanupStack::PopAndDestroy(mailbox);
    }
    return message;
}

bool CFSEngine::sendEmail(QMessage &message)
{
    TMailboxId mailboxId(stripIdPrefix(message.parentAccountId().toString()).toInt());
    MEmailMailbox* mailbox = NULL;
    TRAPD(mailerr, mailbox = m_clientApi->MailboxL(mailboxId));
    Q_UNUSED(mailerr);
    
    MEmailMessage* fsMessage = NULL;
    TRAPD(err,
        fsMessage = createFSMessageL(message, mailbox);
        fsMessage->SaveChangesL();
        fsMessage->SendL(); 
    );

    if (fsMessage)
        fsMessage->Release();
    if (mailbox)
        mailbox->Release();

    if (err != KErrNone)
        return false;
    else
        return true;
}

QMessage CFSEngine::CreateQMessageL(MEmailMessage* aMessage) const
{
    QMessage message;
    int size = 0;
    message.setType(QMessage::Email);

    message.setDate(symbianTTimetoQDateTime(aMessage->Date()));
    message.setReceivedDate(symbianTTimetoQDateTime(aMessage->Date()));
    
    const TFolderId& folderId = aMessage->ParentFolderId();
    TMailboxId mailboxId = folderId.iMailboxId;
    const QMessageAccountId accountId = QMessageAccountId(QString::number(mailboxId.iId));
    message.setParentAccountId(accountId);
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_parentFolderId = QMessageFolderId(QString::number(folderId.iId));
    
    MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);
    MEmailFolder* folder = mailbox->FolderL(folderId);
    QMessagePrivate::setStandardFolder(message, QMessage::InboxFolder);
    if (folder->FolderType() == EDrafts) {
        QMessagePrivate::setStandardFolder(message, QMessage::DraftsFolder);
    } else if (folder->FolderType() == EDeleted) {
        QMessagePrivate::setStandardFolder(message, QMessage::TrashFolder);
    } else if (folder->FolderType() == ESent) {
        QMessagePrivate::setStandardFolder(message, QMessage::SentFolder);
    }
    folder->Release();
    mailbox->Release();

    if (aMessage->Flags() & EFlag_Read) {
        privateMessage->_status = privateMessage->_status | QMessage::Read; 
    }

    if (aMessage->Flags() & EFlag_Important) {
        message.setPriority(QMessage::HighPriority); 
    } else if (aMessage->Flags() & EFlag_Low) {
        message.setPriority(QMessage::LowPriority);
    } else {
        message.setPriority(QMessage::NormalPriority);
    }

    // bodytext and attachment(s)
    MEmailMessageContent* content = aMessage->ContentL();
    if (content) {
       AddContentToMessage(content, &message);
    }
   
    REmailAttachmentArray attachments;                  
    CleanupResetAndRelease<MEmailAttachment>::PushL(attachments);
    TInt count = aMessage->GetAttachmentsL(attachments);
    if (count > 0)
        privateMessage->_status = privateMessage->_status | QMessage::HasAttachments;  
    
    for(TInt i = 0; i < count; i++) {
        TInt availableSize = attachments[i]->AvailableSize();       
        QByteArray name = QString::fromUtf16(attachments[i]->FileNameL().Ptr(), attachments[i]->FileNameL().Length()).toLocal8Bit();
        QByteArray mimeType; // TODO: email client api doesn't offer information about attachment mimetype
        QByteArray mimeSubType; // TODO;
        int size = attachments[i]->TotalSize();
        QMessageContentContainer attachment = QMessageContentContainerPrivate::from(
                                                aMessage->MessageId().iId, 
                                                attachments[i]->Id().iId, 
                                                name, mimeType, 
                                                mimeSubType, size);
        addAttachmentToMessage(message, attachment);       
    }
    CleanupStack::PopAndDestroy();
    attachments.Close();
    
    //from
    TPtrC from = aMessage->SenderAddressL()->Address();
    if (from.Length() > 0) {
        message.setFrom(QMessageAddress(QMessageAddress::Email, QString::fromUtf16(from.Ptr(), from.Length())));
        QMessagePrivate::setSenderName(message, QString::fromUtf16(from.Ptr(), from.Length()));
    }
    
    //to
    REmailAddressArray toRecipients;
    CleanupResetAndRelease<MEmailAddress>::PushL(toRecipients);

    aMessage->GetRecipientsL(MEmailAddress::ETo, toRecipients);
    QList<QMessageAddress> toList;
    for(TInt i = 0; i < toRecipients.Count(); i++) {
        TPtrC to = toRecipients[i]->Address();
        toList.append(QMessageAddress(QMessageAddress::Email, QString::fromUtf16(to.Ptr(), to.Length())));
    }
    message.setTo(toList);
    CleanupStack::PopAndDestroy(&toRecipients);
    toRecipients.Close();
    
    //cc
    REmailAddressArray ccRecipients;
    CleanupResetAndRelease<MEmailAddress>::PushL(ccRecipients);
    aMessage->GetRecipientsL(MEmailAddress::ECc, ccRecipients);
    QList<QMessageAddress> ccList;
    for(TInt i = 0; i < ccRecipients.Count(); i++) {
        TPtrC cc = ccRecipients[i]->Address();
        ccList.append(QMessageAddress(QMessageAddress::Email, QString::fromUtf16(cc.Ptr(), cc.Length())));
    }
    message.setCc(ccList); 
    CleanupStack::PopAndDestroy(&ccRecipients);
    ccRecipients.Close();
    
    //bcc
    REmailAddressArray bccRecipients;
    CleanupResetAndRelease<MEmailAddress>::PushL(bccRecipients);
    aMessage->GetRecipientsL(MEmailAddress::EBcc, bccRecipients);
    QList<QMessageAddress> bccList;
    for(TInt i = 0; i < bccRecipients.Count(); i++) {
        TPtrC bcc = bccRecipients[i]->Address();
        bccList.append(QMessageAddress(QMessageAddress::Email, QString::fromUtf16(bcc.Ptr(), bcc.Length())));
    }
    message.setBcc(bccList);
    CleanupStack::PopAndDestroy(&bccRecipients);
    bccRecipients.Close();
    
    // Read message subject   
    TPtrC subject = aMessage->Subject();
    if (subject.Length() > 0) {
        message.setSubject(QString::fromUtf16(subject.Ptr(), subject.Length()));
    }
    // TODO: size
    privateMessage->_size = size;

    return message;    
}

void CFSEngine::AddContentToMessage(MEmailMessageContent* aContent, QMessage* aMessage) const
{
    MEmailMultipart* mPart = aContent->AsMultipartOrNull();
    if (mPart) {
    TInt partCount = 0;
    TRAPD(err, partCount = mPart->PartCountL());
        if (err == KErrNone) {
            for (TInt i = 0; i < partCount-1; i++) {
                MEmailMessageContent* content = NULL;
                TRAPD(err2, content = mPart->PartByIndexL(i));
                if (err2 == KErrNone) {
                    AddContentToMessage(content, aMessage);
                    content->Release();
                }
            }
        }
        return;
    }
 
    MEmailTextContent* textContent = aContent->AsTextContentOrNull();
    if (textContent) { 
        TInt availableSize = textContent->AvailableSize();
        TRAPD(err, 
            TPtrC body = textContent->ContentL(); 
            QString text = QString::fromUtf16(body.Ptr(), body.Length());
            if (textContent->TextType() == MEmailTextContent::EPlainText) {
                aMessage->setBody(text, "text/plain");
            }
            else if (textContent->TextType() == MEmailTextContent::EHtmlText) {
                aMessage->setBody(text, "text/html");
            }
            );
        Q_UNUSED(err);
        return;
    }
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

QDateTime CFSEngine::symbianTTimetoQDateTime(const TTime& time) const
{
    TDateTime dateTime = time.DateTime();
    QDate qdate = QDate(dateTime.Year(), static_cast<int>(dateTime.Month())+1, dateTime.Day()+1);
    QTime qtime = QTime(dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond()/1000 );
    return QDateTime(qdate, qtime, Qt::UTC);
}

TTime CFSEngine::qDateTimeToSymbianTTime(const QDateTime& date) const
{
    TDateTime dateTime;
    dateTime.SetYear(date.date().year());
    dateTime.SetMonth(static_cast<TMonth>(date.date().month()-1));
    dateTime.SetDay(date.date().day()-1);
    dateTime.SetHour(date.time().hour());
    dateTime.SetMinute(date.time().minute());
    dateTime.SetSecond(date.time().second());
    dateTime.SetMicroSecond(date.time().msec()*1000);
    return TTime(dateTime);
}

TFolderType CFSEngine::standardFolderId(QMessage::StandardFolder standardFolder)
{
    switch(standardFolder) {
        case QMessage::InboxFolder: return EInbox;
        case QMessage::OutboxFolder: return EOutbox;
        case QMessage::DraftsFolder: return EDrafts;
        case QMessage::SentFolder: return ESent;
        case QMessage::TrashFolder: return EDeleted;
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
    TRAPD(err,
            m_factory = CEmailInterfaceFactory::NewL(); 
            m_interfacePtr = m_factory->InterfaceL(KEmailClientApiInterface); 
            m_clientApi = static_cast<MEmailClientApi*>(m_interfacePtr); 
        );
    Q_UNUSED(err);
}

CFSMessagesFindOperation::~CFSMessagesFindOperation()
{
    foreach(FSSearchOperation operation, m_searchOperations) {
        operation.m_search->Release();
        operation.m_mailbox->Release();
    }
    
    m_receiveNewMessages = false;
    m_clientApi->Release();
    delete m_factory;

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
    TRAPD(err, filterAndOrderMessagesL(filters, sortOrder, body, matchFlags));
    if (err != KErrNone) {
        //something has failed -> return empty list
        m_idList = QMessageIdList();
        QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
    }
}

void CFSMessagesFindOperation::filterAndOrderMessagesL(const QMessageFilterPrivate::SortedMessageFilterList& filters,
                                                    const QMessageSortOrder& sortOrder,
                                                    QString body,
                                                    QMessageDataComparator::MatchFlags matchFlags)
{
    m_numberOfHandledFilters = 0;
    
    TEmailSortCriteria sortCriteria = TEmailSortCriteria();
    m_excludeIdList = QMessageIdList();

    m_matchFlags = matchFlags;
    
    if (filters.count() == 0) {
        m_idList = QMessageIdList();
        QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
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
                sortCriteria.iField = TEmailSortCriteria::EBySender;
                break;
            case QMessageSortOrderPrivate::Recipients:
                sortCriteria.iField = TEmailSortCriteria::EByRecipient;
                break;
            case QMessageSortOrderPrivate::Subject:
                sortCriteria.iField = TEmailSortCriteria::EBySubject;
                break;
            case QMessageSortOrderPrivate::TimeStamp:
                sortCriteria.iField = TEmailSortCriteria::EByDate;
                break;
            case QMessageSortOrderPrivate::ReceptionTimeStamp:
                sortCriteria.iField = TEmailSortCriteria::EBySender;
                break;
            case QMessageSortOrderPrivate::Read:
                sortCriteria.iField = TEmailSortCriteria::EByUnread;
                break;
            case QMessageSortOrderPrivate::HasAttachments:
                sortCriteria.iField = TEmailSortCriteria::EByAttachment;
                break;
            case QMessageSortOrderPrivate::Incoming:
                //TODO:
                break;
            case QMessageSortOrderPrivate::Removed:
                //TODO:
                break;
            case QMessageSortOrderPrivate::Priority:
                sortCriteria.iField = TEmailSortCriteria::EByPriority;
                break;
            case QMessageSortOrderPrivate::Size:
                sortCriteria.iField = TEmailSortCriteria::EBySize;
                break;
        }
        sortCriteria.iAscending = fieldOrder.second == Qt::AscendingOrder?true:false;
    } else {
        // This is a workaroud for getFolderSpecificMessagesL crashing when default TEmailSortCriteria (EDontCare) is set
        sortCriteria.iField = TEmailSortCriteria::EByDate;
    }

    if ((filters.count() == 1) &&
        (pf->_field == QMessageFilterPrivate::None) &&
        (pf->_filterList.count() == 0)) {
        if (pf->_notFilter) {
            // There is only one filter: empty ~QMessageFilter()
            // => return empty QMessageIdList 
            m_idList = QMessageIdList();
            QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
        } else {
            // There is only one filter: empty QMessageFilter()
            // => return all messages
            getAllMessagesL(sortCriteria);
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
                QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
                return;
            }
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageFolderId
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    m_numberOfHandledFilters++;
                    QMessageFolder messageFolder = m_owner.folder(QMessageFolderId(pf->_value.toString()));
                    getFolderSpecificMessagesL(messageFolder, sortCriteria);
                    m_resultCorrectlyOrdered = true;
                    QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
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
                QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
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
                        QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);

                    } else { // NotEqual
                        m_excludeIdList.clear();
                        m_excludeIdList.append(QMessageId(pf->_value.toString()));
                        getAllMessagesL(sortCriteria);
                    }
                } else {
                    if (cmp == QMessageDataComparator::NotEqual) {
                        getAllMessagesL(sortCriteria);
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
                        QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
                    } else { // Excludes
                        for (int i=0; i < pf->_ids.count(); i++) {
                            m_excludeIdList.clear();
                            m_excludeIdList.append(QMessageId(pf->_ids[i].toString()));
                            getAllMessagesL(sortCriteria);
                        }
                        getAllMessagesL(sortCriteria);
                    }
                } else {
                    //ipEntrySelection = new(ELeave)CMsvEntrySelection;
                    if (cmp == QMessageDataComparator::Excludes) {
                        getAllMessagesL(sortCriteria);
                    }
                    /*// QMessageFilter
                    if (cmp == QMessageDataComparator::Includes) {
                        // TODO:
                    } else { // Excludes
                        // TODO:
                    }*/
                }
            }
            break;
            }
        case QMessageFilterPrivate::ParentAccountId: {
            if (idType(pf->_value.toString()) != EngineTypeFreestyle) {
                QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
                return;
            }
            if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageAccountId
                m_numberOfHandledFilters++;
                QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Equal) {
                    QMessageAccount messageAccount = m_owner.account(pf->_value.toString());
                    getAccountSpecificMessagesL(messageAccount, sortCriteria);
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
                            getAccountSpecificMessagesL(value, sortCriteria);
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
                        getAccountSpecificMessagesL(messageAccount, sortCriteria);
                    }
                } else { // NotEqual
                    foreach (QMessageAccount value, m_owner.m_accounts) {
                        if (!(value.messageTypes() & type)) {
                            getAccountSpecificMessagesL(value, sortCriteria);
                        }
                    }
                }
            } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) { // QMessage::TypeFlags
                QMessage::TypeFlags typeFlags = static_cast<QMessage::TypeFlags>(pf->_value.toInt());
                QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
                if (cmp == QMessageDataComparator::Includes) {
                    foreach (QMessageAccount value, m_owner.m_accounts) {
                        if (value.messageTypes() | typeFlags) {
                            getAccountSpecificMessagesL(value, sortCriteria);
                        }
                    }

                } else { // Excludes
                    foreach (QMessageAccount value, m_owner.m_accounts) {
                        if (!(value.messageTypes() & typeFlags)) {
                            getAccountSpecificMessagesL(value, sortCriteria);
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
                QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
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
                QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
            }
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
            if (m_activeSearchCount == 0)
                getAllMessagesL(sortCriteria);
        }
    }
    if (m_activeSearchCount == 0)
        QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
}

void CFSMessagesFindOperation::getAllMessagesL(TEmailSortCriteria& sortCriteria)
{
    // Get all messages from every known account
    foreach (QMessageAccount value, m_owner.m_accounts) {
        getAccountSpecificMessagesL(value, sortCriteria);
    }
    if (m_activeSearchCount == 0)
        QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
}

void CFSMessagesFindOperation::getAccountSpecificMessagesL(QMessageAccount& messageAccount, TEmailSortCriteria& sortCriteria)
{
    TMailboxId mailboxId(stripIdPrefix(messageAccount.id().toString()).toInt());
    FSSearchOperation operation;
    operation.m_mailbox = m_clientApi->MailboxL(mailboxId);
    operation.m_search = operation.m_mailbox->MessageSearchL();
    operation.m_search->AddSearchKeyL(_L("*"));
    operation.m_search->SetSortCriteriaL( sortCriteria );
    operation.m_search->StartSearchL( *this ); // this implements MEmailSearchObserver
    m_activeSearchCount++;
    m_searchOperations.append(operation);
}


void CFSMessagesFindOperation::getFolderSpecificMessagesL(QMessageFolder& messageFolder, TEmailSortCriteria sortCriteria)
{
    m_activeSearchCount++;
    RSortCriteriaArray sortCriteriaArray;
    CleanupClosePushL(sortCriteriaArray);
    TFolderId folderId(stripIdPrefix(messageFolder.id().toString()).toInt(), 
        stripIdPrefix(messageFolder.parentAccountId().toString()).toInt());
    MEmailMailbox* mailbox = m_clientApi->MailboxL(stripIdPrefix(messageFolder.parentAccountId().toString()).toInt());
    CleanupReleasePushL(*mailbox);
    MEmailFolder *mailFolder = mailbox->FolderL(folderId);
    CleanupReleasePushL(*mailFolder);
        
    sortCriteriaArray.Append(sortCriteria);
    
    MMessageIterator* msgIterator = mailFolder->MessagesL(sortCriteriaArray);
    CleanupReleasePushL(*msgIterator);
        
    MEmailMessage* msg = NULL;
    while ( NULL != (msg = msgIterator->NextL())) {
        QMessageId messageId(addIdPrefix(QString::number(msg->MessageId().iId), SymbianHelpers::EngineTypeFreestyle));
        if (!m_excludeIdList.contains(messageId)) {
            m_idList.append(messageId);   
        }
        msg->Release();
    }

    CleanupStack::PopAndDestroy(msgIterator);
    CleanupStack::PopAndDestroy(mailFolder);
    CleanupStack::PopAndDestroy(mailbox);
    CleanupStack::PopAndDestroy(&sortCriteriaArray);
}

void CFSMessagesFindOperation::HandleResultL(MEmailMessage* aMessage)
{
    QMessageId messageId(addIdPrefix(QString::number(aMessage->MessageId().iId), SymbianHelpers::EngineTypeFreestyle));
    if (!m_excludeIdList.contains(messageId)) {
        m_idList.append(messageId);   
    }
    aMessage->Release();
}

void CFSMessagesFindOperation::SearchCompletedL()
{
    if (m_receiveNewMessages) {
        m_receiveNewMessages = false;
    } else {
        m_activeSearchCount--;
        if (m_activeSearchCount <= 0) {
            QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
        }
    }
}
    
void CFSMessagesFindOperation::SearchCompleted()
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

#include "..\..\build\Release\QtMessaging\moc\moc_qfsengine_symbian_p.cpp";

QTM_END_NAMESPACE
