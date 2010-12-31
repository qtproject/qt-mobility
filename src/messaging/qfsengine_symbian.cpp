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

#include <QThreadStorage>
#include <QCoreApplication>
#include <QDesktopServices>

using namespace EmailInterface;

QTM_BEGIN_NAMESPACE

using namespace SymbianHelpers;

Q_GLOBAL_STATIC(CFSEngine, applicationThreadFsEngine);
Q_GLOBAL_STATIC(QThreadStorage<CFSEngine *>, fsEngineThreadStorage)

CFSEngine::CFSEngine()
 : m_messageQueryActive(false), m_cleanedup(false)
{
    if (QCoreApplication::instance() && QCoreApplication::instance()->thread() == QThread::currentThread()) {
        // Make sure that application/main thread specific FsEngine will be cleaned up
        // when application event loop quits
        connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(cleanupFSBackend()));

        // If application event loop is not running, aboutToQuit() won't be called
        // => Use qAddPostRoutine to make sure that FsEngine will be correctly
        //    cleaned up when QApplication is destroyed
        qAddPostRoutine(CFSEngine::cleanup);
    }

    TRAPD(err, {
        m_factory = CEmailInterfaceFactory::NewL(); 
        m_ifPtr = m_factory->InterfaceL(KEmailClientApiInterface);
    });
    
    Q_UNUSED(err);
    m_clientApi = static_cast<MEmailClientApi*>(m_ifPtr);
#ifdef FREESTYLEMAILBOXOBSERVERUSED
    if (QCoreApplication::instance() && QCoreApplication::instance()->thread() == QThread::currentThread()) {
        TRAP_IGNORE(setPluginObserversL());
    }
#endif
}

CFSEngine::~CFSEngine()
{
    cleanupFSBackend();
}

void CFSEngine::cleanup()
{
    if (QCoreApplication::instance() && QCoreApplication::instance()->thread() == QThread::currentThread()) {
        CFSEngine* pEngine = applicationThreadFsEngine();
        pEngine->cleanupFSBackend();
    }
}

void CFSEngine::cleanupFSBackend()
{
    for (TInt i = 0; i < m_attachments.Count(); i++){
        m_attachments[i]->Release();
    }
    m_attachments.Reset();

    foreach (CFSContentFetchOperation* operation, m_fetchOperations) {
        delete operation;
    }
    m_fetchOperations.clear();

    foreach (MEmailMailbox* value, m_mailboxes) {
        value->Release();
    }
    m_mailboxes.clear();

    if (m_clientApi) {
        m_clientApi->Release();
        m_clientApi = NULL;
    }

    if (m_factory) {
        delete m_factory;
        m_factory = NULL;
    }
}

CFSEngine* CFSEngine::instance()
{   
    if (QCoreApplication::instance() && QCoreApplication::instance()->thread() == QThread::currentThread()) {
        return applicationThreadFsEngine();
    }

    if (!fsEngineThreadStorage()->hasLocalData()) {
        fsEngineThreadStorage()->setLocalData(new CFSEngine);
    }
    
    return fsEngineThreadStorage()->localData();
}

bool CFSEngine::accountLessThan(const QMessageAccountId accountId1, const QMessageAccountId accountId2)
{
    CFSEngine* freestyleEngine = instance();
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
    CFSEngine* freestyleEngine = instance();
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
    CFSEngine* freestyleEngine = instance();
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

    if (!m_accounts.contains(id.toString())) {
        return QMessageAccountPrivate::from(QMessageAccountId(), QString(), 0, 0, QMessage::NoType);
    }

    return m_accounts[id.toString()];
}

QMessageAccountId CFSEngine::defaultAccount(QMessage::Type type) const
{
    TRAPD(err, updateEmailAccountsL());
    Q_UNUSED(err); 
    QMessageAccountIdList accountIds = accountsByType(type);
    if (accountIds.count() > 0)
        return accountIds.at(0);
        
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
        QMessageAccountId messageAccountId = qMessageAccountIdFromFsMailboxId(mailbox->MailboxId());
        if (!m_accounts.contains(messageAccountId.toString())) {
            QMessageAccount account = QMessageAccountPrivate::from(
                                      messageAccountId,
                                      QString::fromUtf16(mailbox->MailboxName().Ptr(), mailbox->MailboxName().Length()),
                                      0,
                                      0,
                                      QMessage::Email);
          
            m_accounts.insert(messageAccountId.toString(), account);
        } else {
            keys.removeOne(messageAccountId.toString());
        }
        mailbox->Release();
    }  
    
    mailboxes.Reset();
    CleanupStack::PopAndDestroy(); // mailboxes
    
    for (int i=0; i < keys.count(); i++) {
        m_accounts.remove(keys[i]);
    }   
}

#ifdef FREESTYLEMAILBOXOBSERVERUSED
void CFSEngine::setPluginObserversL()
{
    RMailboxPtrArray mailboxes;
    m_clientApi->GetMailboxesL(mailboxes);
    for (TInt i = 0; i < mailboxes.Count(); i++) {
        MEmailMailbox *mailbox = mailboxes[i];
        TRAP_IGNORE(mailbox->RegisterObserverL(*this));
        m_mailboxes.insert(mailbox->MailboxId().iId, mailbox);
    }
    mailboxes.Close();
}

void CFSEngine::NewMessageEventL(const TMailboxId& aMailbox, const REmailMessageIdArray aNewMessages, const TFolderId& aParentFolderId)
{
    for (TInt i = 0; i < aNewMessages.Count(); i++) {
        notificationL(aMailbox, aNewMessages[i], aParentFolderId, QMessageStorePrivate::Added);
    }
}

void CFSEngine::MessageChangedEventL(const TMailboxId& aMailbox, const REmailMessageIdArray aChangedMessages, const TFolderId& aParentFolderId)
{
    for (TInt i = 0; i < aChangedMessages.Count(); i++) {
        notificationL(aMailbox, aChangedMessages[i], aParentFolderId, QMessageStorePrivate::Updated);
    }
}

void CFSEngine::MessageDeletedEventL(const TMailboxId& aMailbox, const REmailMessageIdArray aDeletedMessages, const TFolderId& aParentFolderId)
{
    for (TInt i = 0; i < aDeletedMessages.Count(); i++) {
        notificationL(aMailbox, aDeletedMessages[i], aParentFolderId, QMessageStorePrivate::Removed);
    }
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
    QMessageId realMessageId = qMessageIdFromFsMessageId(aMessageId);

    if (aNotificationType == QMessageStorePrivate::Removed ||
        aNotificationType == QMessageStorePrivate::Updated) {
        // Remove updated or removed message from the cache
        MessageCache::instance()->remove(realMessageId.toString());
    }

    bool messageRetrieved = false;
    MEmailMailbox* mailbox = m_clientApi->MailboxL(aMailbox);
    CleanupReleasePushL(*mailbox);
    for ( ; it != end; ++it) {
        const QMessageFilter &filter(it.value());
        if (!messageRetrieved) {
            MEmailMessage* fsMessage = NULL;
            TRAP_IGNORE(fsMessage = mailbox->MessageL(aMessageId));
            if (!fsMessage) {
                CleanupStack::PopAndDestroy(mailbox);
                return;
            }
            CleanupReleasePushL(*fsMessage);
            CreateQMessageL(&message, *fsMessage);
            messageRetrieved = true;
            CleanupStack::PopAndDestroy(fsMessage);
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
    if (matchingFilters.count() > 0) {
        QT_TRYCATCH_LEAVING(ipMessageStorePrivate->messageNotification(aNotificationType, realMessageId, matchingFilters));
    }
    
    CleanupStack::PopAndDestroy(mailbox);
}

#endif

MEmailMessage* CFSEngine::createFSMessageL(const QMessage &message, const MEmailMailbox* mailbox)
{
    MEmailAddress* pTemplateAddress = mailbox->AddressL();
    TPtrC16 stringPtr(KNullDesC);

    MEmailMessage* fsMessage = mailbox->CreateDraftMessageL();
    CleanupReleasePushL(*fsMessage);

    // Priority
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

    // Read status
    if (message.status() & QMessage::Read) {
        fsMessage->SetFlag(EmailInterface::EFlag_Read);
    } else {
        fsMessage->ResetFlag(EmailInterface::EFlag_Read);
    }

    // Sender/Reply to address
    MEmailAddress* pSenderAddress = fsMessage->SenderAddressL();
    stringPtr.Set(reinterpret_cast<const TUint16*>(QMessagePrivate::senderName(message).utf16()));
    if (pSenderAddress) {
        pSenderAddress->SetDisplayNameL(stringPtr);
    }
    pTemplateAddress->SetDisplayNameL(stringPtr);
    stringPtr.Set(reinterpret_cast<const TUint16*>(message.from().addressee().utf16()));
    if (pTemplateAddress->DisplayName().Length() == 0) {
        if (pSenderAddress) {
            pSenderAddress->SetDisplayNameL(stringPtr);
        }
        pTemplateAddress->SetDisplayNameL(stringPtr);
    }
    if (pSenderAddress) {
        pSenderAddress->SetAddressL(stringPtr);
    }
    pTemplateAddress->SetAddressL(stringPtr);
    fsMessage->SetReplyToAddressL(*pTemplateAddress);

    // To addresses
    QList<QMessageAddress> toList(message.to());
    if (toList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < toList.size(); ++i) {
            REmailAddressArray toAddress;
            qreceiver = toList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            pTemplateAddress->SetAddressL(receiver);
            pTemplateAddress->SetDisplayNameL(receiver);
            pTemplateAddress->SetRole(MEmailAddress::ETo);
            toAddress.Append(pTemplateAddress);
            fsMessage->SetRecipientsL(MEmailAddress::ETo, toAddress);
        }
    }
    
    // Cc addresses
    QList<QMessageAddress> ccList(message.cc());
    if (ccList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < ccList.size(); ++i) {
            REmailAddressArray ccAddress;
            qreceiver = ccList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            pTemplateAddress->SetDisplayNameL(receiver);
            pTemplateAddress->SetRole(MEmailAddress::ECc);
            pTemplateAddress->SetAddressL(receiver);
            ccAddress.Append(pTemplateAddress);
            fsMessage->SetRecipientsL(MEmailAddress::ECc, ccAddress);
        }
    }
        
    // Bcc addresses
    QList<QMessageAddress> bccList(message.bcc());
    if (bccList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < bccList.size(); ++i) {
            REmailAddressArray bccAddress;
            qreceiver = bccList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            pTemplateAddress->SetDisplayNameL(receiver);
            pTemplateAddress->SetRole(MEmailAddress::EBcc);
            pTemplateAddress->SetAddressL(receiver);
            bccAddress.Append(pTemplateAddress);
            fsMessage->SetRecipientsL(MEmailAddress::EBcc, bccAddress);
        }
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
                if (filePath.length() > 0) {
                    if (filePath.startsWith('.')) {
                        // Remove "."
                        filePath.remove(0,1);
                        // Insert data directory location to the path
                        filePath.insert(0,QDesktopServices::storageLocation(QDesktopServices::DataLocation).toAscii());
                    }
                    // Replace Qt style path separator "/" with Symbian path separator "\"
                    filePath.replace(QByteArray("/"), QByteArray("\\"));
                    QString temp_path = QString(filePath);
                    TPtrC16 attachmentPath(KNullDesC);
                    attachmentPath.Set(reinterpret_cast<const TUint16*>(temp_path.utf16()));
                    fsMessage->AddAttachmentL(attachmentPath);
                }
            }        
        }
    }
    fsMessage->SetSubjectL(TPtrC(reinterpret_cast<const TUint16*>(message.subject().utf16())));
    
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
    privateMessage->_id = qMessageIdFromFsMessageId(fsMessage->MessageId());
    
    fsMessage->SaveChangesL();
    CleanupStack::Pop(fsMessage);
    return fsMessage;
}

bool CFSEngine::addMessage(QMessage* message)
{
    TMailboxId mailboxId(fsMailboxIdFromQMessageAccountId(message->parentAccountId()));
    MEmailMailbox* mailbox = NULL;
    TRAPD(mailerr, mailbox = m_clientApi->MailboxL(mailboxId));
    if (mailerr != KErrNone)
        return false;

    MEmailMessage* fsMessage = NULL;
    MEmailAddress* pMailboxAddress = mailbox->AddressL();
    HBufC* pOriginalAddress = pMailboxAddress->Address().AllocLC();
    HBufC* pOriginalDisplayName = pMailboxAddress->DisplayName().AllocLC();
    MEmailAddress::TRole originalRole = pMailboxAddress->Role();
    TRAPD(err, fsMessage = createFSMessageL(*message, mailbox));
    pMailboxAddress->SetRole(originalRole);
    pMailboxAddress->SetDisplayNameL(*pOriginalDisplayName);
    pMailboxAddress->SetAddressL(*pOriginalAddress);
    CleanupStack::PopAndDestroy(pOriginalDisplayName);
    CleanupStack::PopAndDestroy(pOriginalAddress);

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
    // Remove updated message from the cache
    MessageCache::instance()->remove(message->id().toString());

    TMailboxId mailboxId(fsMailboxIdFromQMessageAccountId(message->parentAccountId()));
    MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);
    CleanupReleasePushL(*mailbox);

    MEmailAddress* pTemplateAddress = mailbox->AddressL();
    TPtrC16 stringPtr(KNullDesC);
  
    TMessageId messageId(fsMessageIdFromQMessageId(message->id()));
    MEmailMessage* fsMessage = mailbox->MessageL(messageId);
    CleanupReleasePushL(*fsMessage);
    
    // Priority
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

    // Read status
    if (message->status() & QMessage::Read) {
        fsMessage->SetFlag(EmailInterface::EFlag_Read);
    } else {
        fsMessage->ResetFlag(EmailInterface::EFlag_Read);
    }
        
    // Sender/Reply to address
    MEmailAddress* pSenderAddress = fsMessage->SenderAddressL();
    stringPtr.Set(reinterpret_cast<const TUint16*>(QMessagePrivate::senderName(*message).utf16()));
    if (pSenderAddress) {
        pSenderAddress->SetDisplayNameL(stringPtr);
    }
    pTemplateAddress->SetDisplayNameL(stringPtr);
    stringPtr.Set(reinterpret_cast<const TUint16*>(message->from().addressee().utf16()));
    if (pTemplateAddress->DisplayName().Length() == 0) {
        if (pSenderAddress) {
            pSenderAddress->SetDisplayNameL(stringPtr);
        }
        pTemplateAddress->SetDisplayNameL(stringPtr);
    }
    if (pSenderAddress) {
        pSenderAddress->SetAddressL(stringPtr);
    }
    pTemplateAddress->SetAddressL(stringPtr);
    fsMessage->SetReplyToAddressL(*pTemplateAddress);

    // Remove all addresses from existing email message
    // to make sure that there won't be duplicates when
    // message addresses are updated.
    REmailAddressArray oldAddresses;
    fsMessage->GetRecipientsL(MEmailAddress::EUndefined, oldAddresses);
    CleanupResetAndRelease<MEmailAddress>::PushL(oldAddresses);
    for (int i=0; i<oldAddresses.Count(); i++) {
        fsMessage->RemoveRecipientL(*oldAddresses[i]);
    }
    CleanupStack::PopAndDestroy(&oldAddresses);

    // To addresses
    QList<QMessageAddress> toList(message->to());
    if (toList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < toList.size(); ++i) {
            REmailAddressArray toAddress;
            qreceiver = toList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            MEmailAddress* address = mailbox->AddressL();
            address->SetAddressL(receiver);
            toAddress.Append(address);
            fsMessage->SetRecipientsL(MEmailAddress::ETo, toAddress);
        }
    }

    // Cc addresses
    QList<QMessageAddress> ccList(message->cc());
    if (ccList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < ccList.size(); ++i) {
            REmailAddressArray ccAddress;
            qreceiver = ccList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            pTemplateAddress->SetDisplayNameL(receiver);
            pTemplateAddress->SetRole(MEmailAddress::ECc);
            pTemplateAddress->SetAddressL(receiver);
            ccAddress.Append(pTemplateAddress);
            fsMessage->SetRecipientsL(MEmailAddress::ECc, ccAddress);
        }
    }

    // Bcc addresses
    QList<QMessageAddress> bccList(message->bcc());
    if (bccList.count() > 0) {
        TPtrC16 receiver(KNullDesC);
        QString qreceiver;
        for (int i = 0; i < bccList.size(); ++i) {
            REmailAddressArray bccAddress;
            qreceiver = bccList.at(i).addressee();
            receiver.Set(reinterpret_cast<const TUint16*>(qreceiver.utf16()));
            pTemplateAddress->SetDisplayNameL(receiver);
            pTemplateAddress->SetRole(MEmailAddress::EBcc);
            pTemplateAddress->SetAddressL(receiver);
            bccAddress.Append(pTemplateAddress);
            fsMessage->SetRecipientsL(MEmailAddress::EBcc, bccAddress);
        }
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
                if (filePath.length() > 0) {
                    if (filePath.startsWith('.')) {
                        // Remove "."
                        filePath.remove(0,1);
                        // Insert data directory location to the path
                        filePath.insert(0,QDesktopServices::storageLocation(QDesktopServices::DataLocation).toAscii());
                    }
                    // Replace Qt style path separator "/" with Symbian path separator "\"
                    filePath.replace(QByteArray("/"), QByteArray("\\"));
                    QString temp_path = QString(filePath);
                    TPtrC16 attachmentPath(KNullDesC);
                    attachmentPath.Set(reinterpret_cast<const TUint16*>(temp_path.utf16()));
                    fsMessage->AddAttachmentL(attachmentPath);
                }
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

    TMessageId fsMessageId = fsMessageIdFromQMessageId(id);
    MEmailMailbox* mailbox = NULL;
    TRAPD(err, mailbox = m_clientApi->MailboxL(fsMessageId.iFolderId.iMailboxId));
    if (err == KErrNone) {
        MEmailFolder* folder = NULL;
        TRAP(err, folder = mailbox->FolderL(fsMessageId.iFolderId));
        if (err == KErrNone) {
            REmailMessageIdArray messageIds;
            messageIds.Append(fsMessageId);
            TRAP(err, folder->DeleteMessagesL(messageIds));
            folder->Release();
        }
        mailbox->Release();
    }

    // Remove removed message from the cache
    MessageCache::instance()->remove(id.toString());

    if (err != KErrNone) {
        return false;
    }
    return true;
}

bool CFSEngine::showMessage(const QMessageId &id)
{
    TMessageId fsMessageId = fsMessageIdFromQMessageId(id);
    MEmailMailbox* mailbox = NULL;
    TRAPD(err, mailbox = m_clientApi->MailboxL(fsMessageId.iFolderId.iMailboxId));
    if (err == KErrNone) {
        MEmailMessage* message = NULL;
        TRAP(err, message = mailbox->MessageL(fsMessageId));
        if (err == KErrNone) {
            TRAP(err, message->ShowMessageViewerL());
            message->Release();
        }
        mailbox->Release();
    }

    if (err != KErrNone) {
        return false;
    }
    return true;
}

bool CFSEngine::composeMessage(const QMessage &message)
{
    bool retVal = false;
    MEmailMailbox* mailbox = NULL;
    TMailboxId mailboxId(fsMailboxIdFromQMessageAccountId(message.parentAccountId()));
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
    bool retVal = false;

    QMessage msg = message(messageId);
    QMessageContentContainer cont = msg.find(id);
    QMessageContentContainerPrivate *contPrivate = QMessageContentContainerPrivate::implementation(cont);
    TMessageContentId contentId = contPrivate->_fsContentId;

    MEmailAttachment* attachment = attachmentById(contentId);
    if (attachment) {
        if (attachment->TotalSize() != attachment->AvailableSize()) {
            CFSContentFetchOperation* op = new CFSContentFetchOperation(*this, privateService, attachment);
            if (op->fetch()) {
                m_fetchOperations.insert(&privateService, op);
                retVal = true;
            } else {
                delete op;
                retVal = false;
            }
        } else {
            attachment->Release();
            retVal = false;
        }
    }

    return retVal;
}

bool CFSEngine::retrieveBody(QMessageServicePrivate& privateService, const QMessageId& id)
{
    bool retVal = false;

    QMessage msg = message(id);
    QMessageContentContainer cont = msg.find(msg.bodyId());
    QMessageContentContainerPrivate *contPrivate = QMessageContentContainerPrivate::implementation(cont);
    TMessageContentId contentId = contPrivate->_fsContentId;
    if (contentId.iId > 0) {
        MEmailMailbox* mailbox = NULL;
        TRAPD(err, mailbox = m_clientApi->MailboxL(contentId.iMessageId.iFolderId.iMailboxId));
        if (err == KErrNone) {
            MEmailMessage* pEmailMessage = NULL;
            TRAP(err, pEmailMessage = mailbox->MessageL(contentId.iMessageId));
            if (err == KErrNone) {
                MEmailMessageContent* emailContent = NULL;
                TRAP(err, emailContent = pEmailMessage->ContentL());
                if ((err == KErrNone) && emailContent) {
                    MEmailTextContent* bodyContent = textContentById(contentId, emailContent);
                    if (bodyContent) {
                        if (bodyContent->TotalSize() != bodyContent->AvailableSize()) {
                            CFSContentFetchOperation* op = new CFSContentFetchOperation(*this, privateService, bodyContent, pEmailMessage);
                            pEmailMessage = NULL; // CFSContentFetchOperation took pEmailMessage ownership
                            bodyContent = NULL; // CFSContentFetchOperation took bodyContent ownership
                            if (op->fetch()) {
                                m_fetchOperations.insert(&privateService, op);
                                retVal = true;
                            } else {
                                delete op;
                            }
                        }
                        if (bodyContent) {
                            bodyContent->Release();
                        }
                    }
                }
                if (pEmailMessage) {
                    pEmailMessage->Release();
                }
            }
            mailbox->Release();
        }
    }

    return retVal;
}

bool CFSEngine::retrieveHeader(QMessageServicePrivate& privateService, const QMessageId& id)
{
    Q_UNUSED(id);
    Q_UNUSED(privateService);
    return false;
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
    TMailboxId mailboxId(fsMailboxIdFromQMessageAccountId(id));
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
    TRAP_IGNORE(updateEmailAccountsL());

    FSMessageQueryInfo queryInfo;
    queryInfo.operationId = ++m_operationIds;
    if (queryInfo.operationId == 100000) {
        queryInfo.operationId = 1;
    }
    queryInfo.isQuery = true;
    queryInfo.body = QString();
    queryInfo.matchFlags = 0;
    queryInfo.filter = filter;
    queryInfo.sortOrder = sortOrder;
    queryInfo.offset = offset;
    queryInfo.limit = limit;
    queryInfo.findOperation = new CFSMessagesFindOperation((CFSEngine&)*this, queryInfo.operationId);
    queryInfo.privateService = &privateService;
    queryInfo.currentFilterListIndex = 0;
    queryInfo.canceled = false;
    m_messageQueries.append(queryInfo);

    handleNestedFiltersFromMessageFilter(m_messageQueries[m_messageQueries.count()-1].filter);
    
    doNextQuery();
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
    TRAP_IGNORE(updateEmailAccountsL());

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
    queryInfo.canceled = false;
    m_messageQueries.append(queryInfo);
    
    handleNestedFiltersFromMessageFilter(m_messageQueries[m_messageQueries.count()-1].filter);
    
    doNextQuery();
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
    TRAP_IGNORE(updateEmailAccountsL());

    FSMessageQueryInfo queryInfo;
    queryInfo.operationId = ++m_operationIds;
    if (queryInfo.operationId == 100000) {
        queryInfo.operationId = 1;
    }
    queryInfo.isQuery = false;
    queryInfo.body = QString();
    queryInfo.matchFlags = 0;
    queryInfo.filter = filter;
    queryInfo.sortOrder = QMessageSortOrder();
    queryInfo.offset = 0;
    queryInfo.limit = 0;
    queryInfo.findOperation = new CFSMessagesFindOperation((CFSEngine&)*this, queryInfo.operationId);
    queryInfo.privateService = &privateService;
    queryInfo.currentFilterListIndex = 0;
    queryInfo.count = 0;
    queryInfo.canceled = false;
    m_messageQueries.append(queryInfo);
    
    handleNestedFiltersFromMessageFilter(m_messageQueries[m_messageQueries.count()-1].filter);
    
    doNextQuery();
}

void CFSEngine::doNextQuery() const
{
    int retVal = KErrNone;
    while (m_messageQueries.count() && !m_messageQueryActive) {
        if (m_messageQueries[0].canceled) {
            delete m_messageQueries[0].findOperation;
            m_messageQueries.removeAt(0);
        } else {
            m_messageQueryActive = true;
            QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(m_messageQueries[0].filter); 
            if (pf->_filterList.count() == 0) {
                retVal = m_messageQueries[0].findOperation->filterAndOrderMessages(m_messageQueries[0].filter,
                                                                                   m_messageQueries[0].sortOrder,
                                                                                   m_messageQueries[0].body,
                                                                                   m_messageQueries[0].matchFlags);
            } else {
                retVal = m_messageQueries[0].findOperation->filterAndOrderMessages(pf->_filterList[0],
                                                                                   m_messageQueries[0].sortOrder,
                                                                                   m_messageQueries[0].body,
                                                                                   m_messageQueries[0].matchFlags);
            }
            if (retVal != KErrNone) {
                // filtering & ordering failed
                if (m_messageQueries[0].isQuery) {
                    // => return empty id list
                    m_messageQueries[0].privateService->messagesFound(QMessageIdList(), false, false);
                } else {
                    // => return 0 as count
                    m_messageQueries[0].privateService->messagesCounted(0);
                }
                
                m_messageQueryActive = false;
                delete m_messageQueries[0].findOperation;
                m_messageQueries.removeAt(0);
            }
        }
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
    
    if (m_messageQueries[index].canceled) {
        m_messageQueryActive = false;
        delete m_messageQueries[index].findOperation;
        m_messageQueries.removeAt(index);
        
        doNextQuery();
        return;
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
                int retVal = m_messageQueries[index].findOperation->filterAndOrderMessages(pf->_filterList[m_messageQueries[index].currentFilterListIndex],
                                                                                           m_messageQueries[index].sortOrder,
                                                                                           m_messageQueries[index].body,
                                                                                           m_messageQueries[index].matchFlags);
                if (retVal != KErrNone) {
                    filterAndOrderMessagesReady(false, operationId, QMessageIdList(), numberOfHandledFilters, false);
                }
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
                    MessagingHelper::orderMessages(ids, m_messageQueries[index].sortOrder);
                }
                // Handle offest & limit
                applyOffsetAndLimitToMsgIds(ids, m_messageQueries[index].offset, m_messageQueries[index].limit);
                m_messageQueries[index].privateService->messagesFound(ids, true, true);
            } else {
                m_messageQueries[index].privateService->messagesCounted(ids.count());
            }
        }
    } else {
        // filtering & ordering failed
        if (m_messageQueries[index].isQuery) {
            // => return empty id list
            m_messageQueries[index].privateService->messagesFound(QMessageIdList(), false, false);
        } else {
            // => return 0 as count
            m_messageQueries[index].privateService->messagesCounted(0);
        }
    }

    m_messageQueryActive = false;
    delete m_messageQueries[index].findOperation;
    m_messageQueries.removeAt(index);
    
    doNextQuery();
}


void CFSEngine::cancel(QMessageServicePrivate& privateService)
{
    for (int i=0; i < m_messageQueries.count(); i++) {
        if (m_messageQueries[i].privateService == &privateService) {
            m_messageQueries[i].canceled = true;
        }
    }
    CFSContentFetchOperation* op = m_fetchOperations.value(&privateService);
    if (op) {
        delete op;
    }
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
    if (QCoreApplication::instance() && QCoreApplication::instance()->thread() != QThread::currentThread()) {
        if (this != applicationThreadFsEngine()) {
            return applicationThreadFsEngine()->registerNotificationFilter(aPrivateStore, filter, aId);
        }
    }

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
    if (QCoreApplication::instance() && QCoreApplication::instance()->thread() != QThread::currentThread()) {
        if (this != applicationThreadFsEngine()) {
            return applicationThreadFsEngine()->unregisterNotificationFilter(notificationFilterId);
        }
    }

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

    TFolderId folderId = fsFolderIdFromQMessageFolderId(id);
    MEmailMailbox* mailbox = NULL;
    TRAPD(err, mailbox = m_clientApi->MailboxL(folderId.iMailboxId));
    if (err == KErrNone) {
        MEmailFolder* fsFolder = NULL;
        TRAP(err, fsFolder = mailbox->FolderL(folderId));
        if (err == KErrNone) {
            QMessageFolderId parentId;
            QMessageAccountId accountId = qMessageAccountIdFromFsMailboxId(mailbox->MailboxId());
            QString name = QString::fromUtf16(fsFolder->Name().Ptr(), fsFolder->Name().Length());
            if (name.length() == 0) {
                switch (fsFolder->FolderType()) {
                case EInbox:
                    name = "Inbox";
                    break;
                case EOutbox:
                    name = "Outbox";
                    break;
                case EDrafts:
                    name = "Drafts";
                    break;
                case EDeleted:
                    name = "Deleted";
                    break;
                case ESent:
                    name = "Sent";
                    break;
                case EOther:
                    name = "Unknown";
                    break;
                default:
                    name = "Unknown";
                    break;
                }
            }
            folder = QMessageFolderPrivate::from(id, accountId, parentId, name, name);
            fsFolder->Release();
        }
        mailbox->Release();
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
                            ids.append(qMessageFolderIdFromFsFolderId(subFolder->FolderId()));
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
    
    TMailboxId mailboxId = fsMailboxIdFromQMessageAccountId(accountId);
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
        
        folderIds.append(qMessageFolderIdFromFsFolderId(mailFolder->FolderId()));

        //TODO: Support for subfolders?
        mailFolder->Release();
    }
    
    CleanupStack::PopAndDestroy(&folders);
    CleanupStack::PopAndDestroy(mailbox);
    
    return folderIds;
}

bool CFSEngine::fsFolderL(const QMessageFolderId& id, MEmailMailbox*& mailbox, MEmailFolder*& folder) const
{
    TFolderId folderId = fsFolderIdFromQMessageFolderId(id);
    TRAPD(err, mailbox = m_clientApi->MailboxL(folderId.iMailboxId));
    if (err == KErrNone) {
        TRAP(err, folder = mailbox->FolderL(folderId));
        if (err == KErrNone) {               
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
    QMessage msg = QMessage();

    MessageCache::instance()->lock();
    QMessage* msgPtr = MessageCache::instance()->messageObject(id.toString());
    if (msgPtr == NULL) {
        MessageCache::instance()->unlock();
        msgPtr = new QMessage();
        if (message(msgPtr, id)) {
            MessageCache::instance()->insertObject(msgPtr);
            msg = *msgPtr;
        } else {
            delete msgPtr;
        }
    } else {
        msg = *msgPtr;
        MessageCache::instance()->unlock();
    }

    return msg;
}

bool CFSEngine::message(QMessage* message, const QMessageId& id) const
{
    bool retVal = false;

    TMessageId messageId = fsMessageIdFromQMessageId(id);
    MEmailMailbox* mailbox = NULL;
    bool mailboxFoundFromCache = true;
    TInt err = KErrNone;
    mailbox = m_mailboxes.value(messageId.iFolderId.iMailboxId.iId);
    if (mailbox == NULL) {
        TRAP(err, mailbox = m_clientApi->MailboxL(messageId.iFolderId.iMailboxId.iId));
        mailboxFoundFromCache = false;
    }
    if (err == KErrNone) {
        MEmailMessage* fsMessage = NULL;
        TRAP(err, fsMessage = mailbox->MessageL(messageId));
        if (err == KErrNone) {
            TRAP(err, CreateQMessageL(message, *fsMessage));
            if (err == KErrNone) {
                retVal = true;
                QMessagePrivate* privateMessage = QMessagePrivate::implementation(*message);
                privateMessage->_id = id;
                privateMessage->_modified = false;
            }
            fsMessage->Release();
        }
        if (!mailboxFoundFromCache) {
            mailbox->Release();
        }
    }

    return retVal;
}

bool CFSEngine::sendEmail(QMessage &message)
{
    TMailboxId mailboxId(fsMailboxIdFromQMessageAccountId(message.parentAccountId()));
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

void CFSEngine::CreateQMessageL(QMessage* aQMessage, const MEmailMessage& aFSMessage) const
{
    QMessagePrivate* privateMessage = QMessagePrivate::implementation(*aQMessage);

    aQMessage->setType(QMessage::Email);

    aQMessage->setDate(symbianTTimetoQDateTime(aFSMessage.Date()));
    aQMessage->setReceivedDate(symbianTTimetoQDateTime(aFSMessage.Date()));

    const TFolderId& folderId = aFSMessage.ParentFolderId();
    aQMessage->setParentAccountId(qMessageAccountIdFromFsMailboxId(folderId.iMailboxId));
    privateMessage->_parentFolderId = qMessageFolderIdFromFsFolderId(folderId);
    privateMessage->_id = qMessageIdFromFsMessageId(aFSMessage.MessageId());

    MEmailMailbox* mailbox = NULL;
    bool mailboxFoundFromCache = true;
    mailbox = m_mailboxes.value(folderId.iMailboxId.iId);
    if (mailbox == NULL) {
        mailbox = m_clientApi->MailboxL(folderId.iMailboxId);
        mailboxFoundFromCache = false;
    }
    if (m_folderTypes.contains(folderId.iId)) {
        QMessagePrivate::setStandardFolder(*aQMessage, m_folderTypes.value(folderId.iId));
    } else {
        MEmailFolder* folder = mailbox->FolderL(folderId);
        QMessagePrivate::setStandardFolder(*aQMessage, QMessage::InboxFolder);
        if (folder->FolderType() == EDrafts) {
            QMessagePrivate::setStandardFolder(*aQMessage, QMessage::DraftsFolder);
        } else if (folder->FolderType() == EDeleted) {
            QMessagePrivate::setStandardFolder(*aQMessage, QMessage::TrashFolder);
        } else if (folder->FolderType() == ESent) {
            QMessagePrivate::setStandardFolder(*aQMessage, QMessage::SentFolder);
        }
        m_folderTypes.insert(folderId.iId, aQMessage->standardFolder());
        folder->Release();
    }
    if (!mailboxFoundFromCache) {
        mailbox->Release();
    }

    if (aFSMessage.Flags() & EFlag_Read) {
        privateMessage->_status = privateMessage->_status | QMessage::Read; 
    }

    if (aFSMessage.Flags() & EFlag_Important) {
        aQMessage->setPriority(QMessage::HighPriority);
    } else if (aFSMessage.Flags() & EFlag_Low) {
        aQMessage->setPriority(QMessage::LowPriority);
    } else {
        aQMessage->setPriority(QMessage::NormalPriority);
    }

    if (aFSMessage.Flags() & EFlag_Attachments) {
        privateMessage->_status = privateMessage->_status | QMessage::HasAttachments;
    }

    // Body & Attachments
    QMessageContentContainerPrivate* pContainer = QMessagePrivate::containerImplementation(*aQMessage);
    pContainer->_contentRetrieved = false;
    
    //from
    MEmailAddress* pSenderAddress = aFSMessage.SenderAddressL();
    if (pSenderAddress) {
        TPtrC from = pSenderAddress->Address();
        if (from.Length() > 0) {
            aQMessage->setFrom(QMessageAddress(QMessageAddress::Email, QString::fromUtf16(from.Ptr(), from.Length())));
            QMessagePrivate::setSenderName(*aQMessage, QString::fromUtf16(from.Ptr(), from.Length()));
        }
    }
    
    //to
    REmailAddressArray toRecipients;
    CleanupResetAndRelease<MEmailAddress>::PushL(toRecipients);

    aFSMessage.GetRecipientsL(MEmailAddress::ETo, toRecipients);
    QList<QMessageAddress> toList;
    for(TInt i = 0; i < toRecipients.Count(); i++) {
        TPtrC to = toRecipients[i]->Address();
        toList.append(QMessageAddress(QMessageAddress::Email, QString::fromUtf16(to.Ptr(), to.Length())));
    }
    aQMessage->setTo(toList);
    CleanupStack::PopAndDestroy(&toRecipients);
    toRecipients.Close();
    
    //cc
    REmailAddressArray ccRecipients;
    CleanupResetAndRelease<MEmailAddress>::PushL(ccRecipients);
    aFSMessage.GetRecipientsL(MEmailAddress::ECc, ccRecipients);
    QList<QMessageAddress> ccList;
    for(TInt i = 0; i < ccRecipients.Count(); i++) {
        TPtrC cc = ccRecipients[i]->Address();
        ccList.append(QMessageAddress(QMessageAddress::Email, QString::fromUtf16(cc.Ptr(), cc.Length())));
    }
    aQMessage->setCc(ccList);
    CleanupStack::PopAndDestroy(&ccRecipients);
    ccRecipients.Close();
    
    //bcc
    REmailAddressArray bccRecipients;
    CleanupResetAndRelease<MEmailAddress>::PushL(bccRecipients);
    aFSMessage.GetRecipientsL(MEmailAddress::EBcc, bccRecipients);
    QList<QMessageAddress> bccList;
    for(TInt i = 0; i < bccRecipients.Count(); i++) {
        TPtrC bcc = bccRecipients[i]->Address();
        bccList.append(QMessageAddress(QMessageAddress::Email, QString::fromUtf16(bcc.Ptr(), bcc.Length())));
    }
    aQMessage->setBcc(bccList);
    CleanupStack::PopAndDestroy(&bccRecipients);
    bccRecipients.Close();
    
    // Read message subject   
    TPtrC subject = aFSMessage.Subject();
    if (subject.Length() > 0) {
        aQMessage->setSubject(QString::fromUtf16(subject.Ptr(), subject.Length()));
    }
}

void CFSEngine::retrieveMessageContentHeaders(QMessage& message) const
{
    MessageCache::instance()->lock();
    QMessage* msgPtr = MessageCache::instance()->messageObject(message.id().toString());

    QMessageContentContainerPrivate* pContainer = QMessagePrivate::containerImplementation(*msgPtr);
    pContainer->_contentRetrieved = true;

    bool contentHeadersRetrieved = false;
    TMessageId messageId = fsMessageIdFromQMessageId(message.id());
    MEmailMailbox* mailbox = NULL;
    TRAPD(err, mailbox = m_clientApi->MailboxL(messageId.iFolderId.iMailboxId.iId));
    if (err == KErrNone) {
        MEmailMessage* pEmailMessage = NULL;
        TRAP(err, pEmailMessage = mailbox->MessageL(messageId));
        if (err == KErrNone) {
            addMessagePartsToQMessage(*msgPtr, *pEmailMessage);
            contentHeadersRetrieved = true;
            pEmailMessage->Release();
        }
        mailbox->Release();
    }

    if (contentHeadersRetrieved) {
        message = *msgPtr;
    }

    MessageCache::instance()->unlock();
}

void CFSEngine::addMessagePartsToQMessage(QMessage& message, MEmailMessage& mEmailMessage) const
{
    int size = 0;
    QByteArray mimeType;
    QByteArray mimeSubType;
    QByteArray charset;

    TMessageId msgId = mEmailMessage.MessageId();

    MEmailMessageContent* pContent = NULL;
    TRAPD(err, pContent = mEmailMessage.ContentL());
    if ((err == KErrNone) && pContent) {
        // Message MIME type
        QByteArray mimeHeader = QString::fromUtf16(pContent->ContentType().Ptr(),
                                                   pContent->ContentType().Length()).toAscii();
        MessagingHelper::extractMIMEHeaderParts(mimeHeader, mimeType, mimeSubType, charset);
        QMessageContentContainerPrivate* container = QMessagePrivate::containerImplementation(message);
        container->_type = mimeType;
        container->_subType = mimeSubType;
        addContentToQMessage(message, *pContent, msgId);
        size = pContent->TotalSize();
        pContent->Release();
    }

    // Attachments
    REmailAttachmentArray attachments;
    TInt count = mEmailMessage.GetAttachmentsL(attachments);
    for (int i=0; i < attachments.Count(); i++) {
        QByteArray fileName;
        TPtrC fName(KNullDesC);
        TRAPD(err, fName.Set(attachments[i]->FileNameL()));
        if (err == KErrNone) {
            fileName = QString::fromUtf16(fName.Ptr(), fName.Length()).toLocal8Bit();
        }
        QByteArray mimeHeader = QString::fromUtf16(attachments[i]->ContentType().Ptr(),
                                                   attachments[i]->ContentType().Length()).toAscii();
        MessagingHelper::extractMIMEHeaderParts(mimeHeader, mimeType, mimeSubType, charset);
        int attachmentSize = attachments[i]->TotalSize();
        size += attachmentSize;
        QMessageContentContainer attachment = QMessageContentContainerPrivate::from(msgId.iId,
                                                                                    1,
                                                                                    fileName, mimeType,
                                                                                    mimeSubType, attachmentSize,
                                                                                    attachments[i]->Id());
        QMessageContentContainerPrivate *attachmentContainer = QMessageContentContainerPrivate::implementation(attachment);
        attachmentContainer->_freestyleAttachment = true;
        if (attachments[i]->TotalSize() == attachments[i]->AvailableSize()) {
            attachmentContainer->_available = true;
        } else {
            attachmentContainer->_available = false;
        }
        addAttachmentToQMessage(message, attachment);
        attachments[i]->Release();
    }
    attachments.Reset();

    QMessagePrivate* pPrivateMessage = QMessagePrivate::implementation(message);
    pPrivateMessage->_size = size;
}

void CFSEngine::addContentToQMessage(QMessage& message, const MEmailMessageContent &content, TMessageId messageId) const
{
    MEmailMultipart* pMultipart = content.AsMultipartOrNull();
    if (pMultipart) {
        TInt partCount = 0;
        TRAPD(err, partCount = pMultipart->PartCountL());
        if (err == KErrNone) {
            for (TInt i = 0; i < partCount; i++) {
                MEmailMessageContent* pContent = NULL;
                TRAP(err, pContent = pMultipart->PartByIndexL(i));
                if (err == KErrNone) {
                    addContentToQMessage(message, *pContent, messageId);
                    pContent->Release();
                }
            }
        }
        return;
    }

    QByteArray mimeType;
    QByteArray mimeSubType;
    QByteArray charset;

    MEmailTextContent* pTextContent = content.AsTextContentOrNull();
    if (pTextContent) {
        QMessagePrivate* pPrivateMessage = QMessagePrivate::implementation(message);
        QMessageContentContainerPrivate* pContainer = QMessagePrivate::containerImplementation(message);
        QMessageContentContainerId existingBodyId(message.bodyId());

        QByteArray mimeHeader = QString::fromUtf16(pTextContent->ContentType().Ptr(),
                                                   pTextContent->ContentType().Length()).toAscii();
        MessagingHelper::extractMIMEHeaderParts(mimeHeader, mimeType, mimeSubType, charset);
        if (charset.isEmpty()) {
            charset = "UTF-8";
        }
        if (existingBodyId.isValid()) {
            if (existingBodyId == QMessageContentContainerPrivate::bodyContentId()) {
                // The body content is in the message itself
                if (!pContainer->_attachmentId || (pTextContent->TextType() == MEmailTextContent::EPlainText)) {
                    pContainer->_fsContentId = pTextContent->Id();
                    pContainer->_attachmentId = 1;
                    pContainer->_containingMessageId = messageId.iId;
                    pContainer->_name = QByteArray();
                    pContainer->_type = mimeType;
                    pContainer->_subType = mimeSubType;
                    pContainer->_charset = charset;
                    pContainer->_size = pTextContent->TotalSize();
                    pContainer->_freestyleAttachment = true;
                    if (pTextContent->TotalSize() == pTextContent->AvailableSize()) {
                        pContainer->_available = true;
                    } else {
                        pContainer->_available = false;
                    }
                }
            } else {
                // The body content is in the first attachment
                QMessageContentContainerPrivate *pAttachmentContainer(QMessageContentContainerPrivate::implementation(*pContainer->attachment(existingBodyId)));
                if (!pAttachmentContainer->_attachmentId || (pTextContent->TextType() == MEmailTextContent::EPlainText)) {
                    pAttachmentContainer->_fsContentId = pTextContent->Id();
                    pAttachmentContainer->_attachmentId = 1;
                    pAttachmentContainer->_containingMessageId = messageId.iId;
                    pAttachmentContainer->_name = QByteArray();
                    pAttachmentContainer->_type = mimeType;
                    pAttachmentContainer->_subType = mimeSubType;
                    pAttachmentContainer->_charset = charset;
                    pAttachmentContainer->_size = pTextContent->TotalSize();
                    pAttachmentContainer->_freestyleAttachment = true;
                    if (pTextContent->TotalSize() == pTextContent->AvailableSize()) {
                        pAttachmentContainer->_available = true;
                    } else {
                        pAttachmentContainer->_available = false;
                    }
                }
            }
        } else {
            if (pContainer->_attachments.isEmpty()) {
                // Put the content directly into the message
                pContainer->_fsContentId = pTextContent->Id();
                pContainer->_attachmentId = 1;
                pContainer->_containingMessageId = messageId.iId;
                pContainer->_name = QByteArray();
                pContainer->_type = mimeType;
                pContainer->_subType = mimeSubType;
                pContainer->_charset = charset;
                pContainer->_size = pTextContent->TotalSize();
                pContainer->_freestyleAttachment = true;
                if (pTextContent->TotalSize() == pTextContent->AvailableSize()) {
                    pContainer->_available = true;
                } else {
                    pContainer->_available = false;
                }
                pPrivateMessage->_bodyId = QMessageContentContainerPrivate::bodyContentId();
            } else {
                // Add the body as the first attachment
                QMessageContentContainer newBody;
                QMessageContentContainerPrivate *pAttachmentContainer = QMessageContentContainerPrivate::implementation(newBody);
                pAttachmentContainer->_fsContentId = pTextContent->Id();
                pAttachmentContainer->_attachmentId = 1;
                pAttachmentContainer->_containingMessageId = messageId.iId;
                pAttachmentContainer->_name = QByteArray();
                pAttachmentContainer->_type = mimeType;
                pAttachmentContainer->_subType = mimeSubType;
                pAttachmentContainer->_charset = charset;
                pAttachmentContainer->_size = pTextContent->TotalSize();
                pAttachmentContainer->_freestyleAttachment = true;
                if (pTextContent->TotalSize() == pTextContent->AvailableSize()) {
                    pAttachmentContainer->_available = true;
                } else {
                    pAttachmentContainer->_available = false;
                }
                pPrivateMessage->_bodyId = pContainer->prependContent(newBody);
            }
        }
    }

    MEmailAttachment* pAttachment = content.AsAttachmentOrNull();
    if (pAttachment) {
        QByteArray fileName;
        TPtrC fName(KNullDesC);
        TRAPD(err, fName.Set(pAttachment->FileNameL()));
        if (err == KErrNone) {
            fileName = QString::fromUtf16(fName.Ptr(), fName.Length()).toLocal8Bit();
        }
        QByteArray mimeHeader = QString::fromUtf16(pAttachment->ContentType().Ptr(),
                                                   pAttachment->ContentType().Length()).toAscii();
        MessagingHelper::extractMIMEHeaderParts(mimeHeader, mimeType, mimeSubType, charset);
        int size = pAttachment->TotalSize();
        QMessageContentContainer attachment = QMessageContentContainerPrivate::from(messageId.iId,
                                                                                    1,
                                                                                    fileName, mimeType,
                                                                                    mimeSubType,
                                                                                    size,
                                                                                    pAttachment->Id());
        QMessageContentContainerPrivate *attachmentContainer = QMessageContentContainerPrivate::implementation(attachment);
        attachmentContainer->_freestyleAttachment = true;
        if (pAttachment->TotalSize() == pAttachment->AvailableSize()) {
            attachmentContainer->_available = true;
        } else {
            attachmentContainer->_available = false;
        }
        addAttachmentToQMessage(message, attachment);
    }

    QMessageContentContainerPrivate* pContainer = QMessagePrivate::containerImplementation(message);
}

void CFSEngine::addAttachmentToQMessage(QMessage& message, QMessageContentContainer& attachment) const
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

QString CFSEngine::attachmentTextContent(long int messageId, TMessageContentId attachmentContentId,
                                         const QByteArray &charset) const
{
    QString result;

    QByteArray data = attachmentContent(messageId, attachmentContentId);
    if (!data.isEmpty()) {
        // Convert attachment data to string form
        QTextCodec *codec;
        if (!charset.isEmpty()) {
            codec = QTextCodec::codecForName(charset);
        } else {
            codec = QTextCodec::codecForLocale();
        }

        if (codec) {
            result = codec->toUnicode(data);
        }
    } else {
        result = bodyContent(messageId, attachmentContentId);
    }

    return result;
}

QByteArray CFSEngine::attachmentContent(long int messageId, TMessageContentId attachmentContentId) const
{
    QByteArray content;

    TMailboxId mailBoxId(attachmentContentId.iMessageId.iFolderId.iMailboxId);
    MEmailAttachment* attachment = attachmentById(attachmentContentId);
    if (attachment) {
        TRAP_IGNORE(
            RFile file = attachment->FileL();
            CleanupClosePushL(file);
            TInt fileSize = 0;
            file.Size(fileSize);
            if (fileSize != 0) {
                TInt pos;
                file.Seek(ESeekStart, pos);
                HBufC8* pContentBuf = HBufC8::NewLC(fileSize);
                TPtr8 contentBuf = pContentBuf->Des();
                if (file.Read(contentBuf, fileSize) == KErrNone) {
                    content = QByteArray((char*)pContentBuf->Ptr(), pContentBuf->Length());
                }
                CleanupStack::PopAndDestroy(pContentBuf);
            }
            CleanupStack::PopAndDestroy(&file);
        );
        attachment->Release();
    }

    return content;
}

MEmailAttachment* CFSEngine::attachmentById(TMessageContentId attachmentId) const
{
    MEmailAttachment* attachment = NULL;

    MEmailMailbox* mailbox = NULL;
    TRAPD(err, mailbox = m_clientApi->MailboxL(attachmentId.iMessageId.iFolderId.iMailboxId));
    if (err == KErrNone) {
        MEmailMessage* pEmailMessage = NULL;
        TRAP(err, pEmailMessage = mailbox->MessageL(attachmentId.iMessageId));
        if (err == KErrNone) {
            REmailAttachmentArray attachments;
            TRAP(err, pEmailMessage->GetAttachmentsL(attachments));
            if (err == KErrNone) {
                for (int i=0; i < attachments.Count(); i++) {
                    if (attachments[i]->Id() == attachmentId) {
                        attachment = attachments[i];
                    } else {
                        attachments[i]->Release();
                    }
                }
                attachments.Close();
            }
            pEmailMessage->Release();
        }
        mailbox->Release();
    }

    return attachment;
}

MEmailTextContent* CFSEngine::textContentById(TMessageContentId contentId, MEmailMessageContent* content) const
{
    MEmailTextContent* retContent = NULL;

    if (content) {
        MEmailMultipart* pMultipart = content->AsMultipartOrNull();
        if (pMultipart) {
            TInt partCount = 0;
            TRAP_IGNORE(partCount = pMultipart->PartCountL());
            for (TInt i = 0; i < partCount; i++) {
                MEmailMessageContent* pContent = NULL;
                TRAPD(err, pContent = pMultipart->PartByIndexL(i));
                if (err == KErrNone) {
                    retContent = textContentById(contentId, pContent);
                    if (retContent) {
                        break;
                    } else {
                        pContent->Release();
                    }
                }
            }
        } else {
            MEmailTextContent* pTextContent = content->AsTextContentOrNull();
            if (pTextContent) {
                if (pTextContent->Id() == contentId) {
                    retContent = pTextContent;
                }
            }
        }
    }

    return retContent;
}

QString CFSEngine::bodyContent(long int messageId, TMessageContentId bodyContentId) const
{
    QString content;

    MEmailMailbox* mailbox = NULL;
    TRAPD(err, mailbox = m_clientApi->MailboxL(bodyContentId.iMessageId.iFolderId.iMailboxId));
    if (err == KErrNone) {
        MEmailMessage* pEmailMessage = NULL;
        TRAP(err, pEmailMessage = mailbox->MessageL(bodyContentId.iMessageId));
        if (err == KErrNone) {
            MEmailMessageContent* emailContent = NULL;
            TRAP(err, emailContent = pEmailMessage->ContentL());
            if (err == KErrNone) {
                MEmailTextContent* bodyContent = textContentById(bodyContentId, emailContent);
                if (bodyContent) {
                    TPtrC contentPtr(KNullDesC);
                    TRAPD(err, contentPtr.Set(bodyContent->ContentL()));
                    if (err == KErrNone) {
                        content = QString::fromUtf16(contentPtr.Ptr(), contentPtr.Length());
                    }
                    bodyContent->Release();
                }
            }
            pEmailMessage->Release();
        }
        mailbox->Release();
    }

    return content;
}

QString CFSEngine::bodyContentFromMessageContent(const MEmailMessageContent& messageContent) const
{
    QString bodyContent;

    MEmailMultipart* pMultipart = messageContent.AsMultipartOrNull();
    if (pMultipart) {
        TInt partCount = 0;
        TRAP_IGNORE(partCount = pMultipart->PartCountL());
        for (TInt i = 0; i < partCount; i++) {
            MEmailMessageContent* pContent = NULL;
            TRAP_IGNORE(pContent = pMultipart->PartByIndexL(i));
            if (pContent) {
                bodyContent = bodyContentFromMessageContent(*pContent);
                pContent->Release();
            }
        }
        return bodyContent;
    }

    MEmailTextContent* pTextContent = messageContent.AsTextContentOrNull();
    if (pTextContent) {
        TPtrC content(KNullDesC);
        TRAPD(err, content.Set(pTextContent->ContentL()));
        if (err == KErrNone) {
            bodyContent = QString::fromUtf16(content.Ptr(), content.Length());
        }
    }

    return bodyContent;
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

TMessageId CFSEngine::fsMessageIdFromQMessageId(QMessageId messageId)
{
    TUint mailboxId = 0;
    TUint folderId = 0;
    TUint msgId = 0;

    QString messageIdString = stripIdPrefix(messageId.toString());
    int index = messageIdString.indexOf('_');
    if (index != -1) {
        mailboxId = messageIdString.left(index).toUInt();
        messageIdString = messageIdString.mid(index + 1);
        int index = messageIdString.indexOf('_');
        if (index != -1) {
            folderId = messageIdString.left(index).toUInt();
            msgId = messageIdString.mid(index + 1).toUInt();
        }
    }

    return TMessageId(msgId, folderId, mailboxId);
}

QMessageId CFSEngine::qMessageIdFromFsMessageId(TMessageId messageId)
{
    QString messageIdString;
    messageIdString = QString::number(messageId.iFolderId.iMailboxId.iId);
    messageIdString += "_" + QString::number(messageId.iFolderId.iId);
    messageIdString += "_" + QString::number(messageId.iId);
    return QMessageId(addIdPrefix(messageIdString, SymbianHelpers::EngineTypeFreestyle));
}

TFolderId CFSEngine::fsFolderIdFromQMessageFolderId(QMessageFolderId folderId)
{
    TUint mailboxId = 0;
    TUint fldrId = 0;

    QString folderIdString = stripIdPrefix(folderId.toString());
    int index = folderIdString.indexOf('_');
    if (index != -1) {
        mailboxId = folderIdString.left(index).toUInt();
        fldrId = folderIdString.mid(index + 1).toUInt();
    }

    return TFolderId(fldrId, mailboxId);
}

QMessageFolderId CFSEngine::qMessageFolderIdFromFsFolderId(TFolderId folderId)
{
    QString folderIdString;
    folderIdString = QString::number(folderId.iMailboxId.iId);
    folderIdString += "_" + QString::number(folderId.iId);
    return QMessageFolderId(addIdPrefix(folderIdString, SymbianHelpers::EngineTypeFreestyle));
}

TMailboxId CFSEngine::fsMailboxIdFromQMessageAccountId(QMessageAccountId accountId)
{
    return TMailboxId(stripIdPrefix(accountId.toString()).toUInt());
}

QMessageAccountId CFSEngine::qMessageAccountIdFromFsMailboxId(TMailboxId mailboxId)
{
    return QMessageAccountId(addIdPrefix(QString::number(mailboxId.iId), SymbianHelpers::EngineTypeFreestyle));
}

void CFSEngine::contentFetched(void* service, bool success)
{
    QMessageServicePrivate* pService = reinterpret_cast<QMessageServicePrivate*>(service);
    CFSContentFetchOperation* op = m_fetchOperations.take(pService);
    if (op) {
        if (success) {
            QMessageId messageId = qMessageIdFromFsMessageId(op->m_content->Id().iMessageId);
            // Make sure that new message contents will be updated to cache
            MessageCache::instance()->remove(messageId.toString());
            pService->setFinished(true);
        } else {
            pService->setFinished(false);
        }
        delete op;
    }
}

CFSContentFetchOperation::CFSContentFetchOperation(CFSEngine& parentEngine, QMessageServicePrivate& service,
                                                   MEmailMessageContent* content, MEmailMessage* message)
    : m_parentEngine(parentEngine),
      m_service(service),
      m_content(content),
      m_message(message)
{
}

CFSContentFetchOperation::~CFSContentFetchOperation()
{
    m_content->Release(); // Note: Cancels fetch if fetch is ongoing
    if (m_message) {
        m_message->Release();
    }
}

bool CFSContentFetchOperation::fetch()
{
    TRAPD(err, m_content->FetchL(*this));
    if (err != KErrNone) {
        return false;
    }
    return true;
}

void CFSContentFetchOperation::DataFetchedL(const TInt aResult)
{
    bool result = false;
    if (aResult == KErrNone) {
        result = true;
    }
    QMetaObject::invokeMethod(&m_parentEngine, "contentFetched", Qt::QueuedConnection,
                              Q_ARG(void*, reinterpret_cast<void*>(&m_service)),
                              Q_ARG(bool, result));
}

CFSMessagesFindOperation::CFSMessagesFindOperation(CFSEngine& aOwner, int aOperationId)
    : m_owner(aOwner), 
      m_operationId(aOperationId),
      m_resultCorrectlyOrdered(false)
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
        if (operation.m_mailbox) {
            operation.m_mailbox->Release();
        }
    }
    
    m_clientApi->Release();
    delete m_factory;

}

int CFSMessagesFindOperation::filterAndOrderMessages(const QMessageFilter &filter, const QMessageSortOrder& sortOrder,
                                                     QString body, QMessageDataComparator::MatchFlags matchFlags)
{
    m_filterList.clear();
    m_filterList.append(filter);
    return filterAndOrderMessages(m_filterList, sortOrder, body, matchFlags);
}

int CFSMessagesFindOperation::filterAndOrderMessages(const QMessageFilterPrivate::SortedMessageFilterList& filters,
                                                     const QMessageSortOrder& sortOrder,
                                                     QString body,
                                                     QMessageDataComparator::MatchFlags matchFlags)
{
    TRAPD(err, filterAndOrderMessagesL(filters, sortOrder, body, matchFlags));
    return err;
}

void CFSMessagesFindOperation::filterAndOrderMessagesL(const QMessageFilterPrivate::SortedMessageFilterList& filters,
                                                       const QMessageSortOrder& sortOrder,
                                                       QString body,
                                                       QMessageDataComparator::MatchFlags matchFlags)
{
    m_numberOfHandledFilters = 0;
    m_resultCorrectlyOrdered = false;
    m_asynchronousSearchStarted = false;
    m_body = body;
    m_matchFlags = matchFlags;
    m_idList = QMessageIdList();

    TEmailSortCriteria sortCriteria = TEmailSortCriteria();
    // This is a workaroud for MEmailFolder::MessagesL(...) crashing when default TEmailSortCriteria (EDontCare) is set
    // => TEmailSortCriteria::EByDate is always used by default
    sortCriteria.iField = TEmailSortCriteria::EByDate;
    m_excludeIdList = QMessageIdList();

    if (filters.count() == 0) {
        m_idList = QMessageIdList();
        QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
        return;
    }

    // Handle ordering
    QMessageSortOrderPrivate* privateMessageOrdering = QMessageSortOrderPrivate::implementation(sortOrder);
    if (privateMessageOrdering->_fieldOrderList.count() == 1) {
        // Set sortOrder
        if (!sortOrder.isEmpty() ) {
            QPair<QMessageSortOrderPrivate::Field, Qt::SortOrder> fieldOrder = privateMessageOrdering->_fieldOrderList.at(0);
            switch (fieldOrder.first) {
            case QMessageSortOrderPrivate::Type:
                break;
            case QMessageSortOrderPrivate::Sender:
                sortCriteria.iField = TEmailSortCriteria::EBySender;
                m_resultCorrectlyOrdered = true;
                break;
            case QMessageSortOrderPrivate::Recipients:
                sortCriteria.iField = TEmailSortCriteria::EByRecipient;
                m_resultCorrectlyOrdered = true;
                break;
            case QMessageSortOrderPrivate::Subject:
                sortCriteria.iField = TEmailSortCriteria::EBySubject;
                m_resultCorrectlyOrdered = true;
                break;
            case QMessageSortOrderPrivate::TimeStamp:
                sortCriteria.iField = TEmailSortCriteria::EByDate;
                m_resultCorrectlyOrdered = true;
                break;
            case QMessageSortOrderPrivate::ReceptionTimeStamp:
                sortCriteria.iField = TEmailSortCriteria::EByDate;
                m_resultCorrectlyOrdered = true;
                break;
            case QMessageSortOrderPrivate::Read:
                sortCriteria.iField = TEmailSortCriteria::EByUnread;
                m_resultCorrectlyOrdered = true;
                break;
            case QMessageSortOrderPrivate::HasAttachments:
                sortCriteria.iField = TEmailSortCriteria::EByAttachment;
                m_resultCorrectlyOrdered = true;
                break;
            case QMessageSortOrderPrivate::Incoming:
                //TODO:
                break;
            case QMessageSortOrderPrivate::Removed:
                //TODO:
                break;
            case QMessageSortOrderPrivate::Priority:
                sortCriteria.iField = TEmailSortCriteria::EByPriority;
                m_resultCorrectlyOrdered = true;
                break;
            case QMessageSortOrderPrivate::Size:
                sortCriteria.iField = TEmailSortCriteria::EBySize;
                m_resultCorrectlyOrdered = true;
                break;
            }
            sortCriteria.iAscending = fieldOrder.second == Qt::AscendingOrder?true:false;
        }
    }

    // Handle empty filter
    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(filters[m_numberOfHandledFilters]);
    if ((filters.count() == 1) &&
        (pf->_field == QMessageFilterPrivate::None) &&
        (pf->_filterList.count() == 0)) {
        if (pf->_notFilter) {
            // There is only one filter: empty ~QMessageFilter()
            // => return empty QMessageIdList 
            m_numberOfHandledFilters++;
            m_idList = QMessageIdList();
            QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
        } else {
            // There is only one filter: empty QMessageFilter()
            // => return all messages
            m_numberOfHandledFilters++;
            getAllMessagesL(sortCriteria);
        }
        return;
    }

    // Handle filtering
    switch (pf->_field) {
    case QMessageFilterPrivate::ParentFolderId: {
        if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageFolderId
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                m_numberOfHandledFilters++;
                TFolderId folderId = CFSEngine::fsFolderIdFromQMessageFolderId(QMessageFolderId(pf->_value.toString()));
                MEmailMailbox* mailbox = m_clientApi->MailboxL(folderId.iMailboxId);
                if (mailbox) {
                    CleanupReleasePushL(*mailbox);
                    MEmailFolder* folder = mailbox->FolderL(folderId);
                    CleanupReleasePushL(*folder);
                    getFolderSpecificMessagesL(*folder, sortCriteria);
                    CleanupStack::PopAndDestroy(folder);
                    CleanupStack::PopAndDestroy(mailbox);
                }
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
        m_numberOfHandledFilters++;
        if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageId
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            if (!pf->_value.isNull() && pf->_value.toString().length() > QString(SymbianHelpers::freestylePrefix).length()) {
                if (cmp == QMessageDataComparator::Equal) {
                    QMessage message = m_owner.message(QMessageId(pf->_value.toString()));
                    m_idList.clear();
                    m_idList.append(message.id());
                    m_resultCorrectlyOrdered = true;
                } else { // NotEqual
                    m_excludeIdList.clear();
                    m_excludeIdList.append(QMessageId(pf->_value.toString()));
                    getAllMessagesL(sortCriteria);
                }
            } else { // Invalid QMessageId
                if (cmp == QMessageDataComparator::Equal) {
                    m_idList.clear();
                    m_resultCorrectlyOrdered = true;
                } else { // NotEqual
                    getAllMessagesL(sortCriteria);
                }
            }
        } else if (pf->_comparatorType == QMessageFilterPrivate::Inclusion) {
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pf->_comparatorValue));
            if (pf->_ids.count() > 0) { // QMessageIdList
                if (cmp == QMessageDataComparator::Includes) {
                    for (int i=0; i < pf->_ids.count(); i++) {
                        QMessage message = m_owner.message(QMessageId(pf->_ids[i].toString()));
                        if (message.type() != QMessage::NoType) {
                            m_idList.append(message.id());
                        }
                    }
                } else { // Excludes
                    m_excludeIdList.clear();
                    for (int i=0; i < pf->_ids.count(); i++) {
                        m_excludeIdList.append(QMessageId(pf->_ids[i].toString()));
                    }
                    getAllMessagesL(sortCriteria);
                }
            } else { // Empty QMessageAccountIdList
                if (cmp == QMessageDataComparator::Includes) {
                    m_idList.clear();
                    m_resultCorrectlyOrdered = true;
                } else { // Excludes
                    getAllMessagesL(sortCriteria);
                }
            }
        }
        break;
        }
    case QMessageFilterPrivate::ParentAccountId: {
        if (pf->_comparatorType == QMessageFilterPrivate::Equality) { // QMessageAccountId
            m_numberOfHandledFilters++;
            QMessageDataComparator::EqualityComparator cmp(static_cast<QMessageDataComparator::EqualityComparator>(pf->_comparatorValue));
            if (cmp == QMessageDataComparator::Equal) {
                QMessageAccount messageAccount = m_owner.account(pf->_value.toString());
                getAccountSpecificMessagesL(messageAccount, sortCriteria);
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
                TMailboxId mailboxId(CFSEngine::fsMailboxIdFromQMessageAccountId(messageAccount.id()));
                MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);
                if (mailbox) {
                    CleanupReleasePushL(*mailbox);
                    MEmailFolder* folder = mailbox->FolderByTypeL(stdFolder);
                    if (folder) {
                        CleanupReleasePushL(*folder);
                        getFolderSpecificMessagesL(*folder, sortCriteria);
                        CleanupStack::PopAndDestroy(folder);
                    }
                    CleanupStack::PopAndDestroy(mailbox);
                }
            }
        } else { // NotEqual
            foreach (QMessageAccount messageAccount, m_owner.m_accounts) {
                TMailboxId mailboxId(CFSEngine::fsMailboxIdFromQMessageAccountId(messageAccount.id()));
                MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);
                if (mailbox) {
                    CleanupReleasePushL(*mailbox);
                    QMessage::StandardFolder i = QMessage::InboxFolder;
                    while (i <= QMessage::TrashFolder) {
                        if (i != standardFolder) {
                            MEmailFolder* folder = mailbox->FolderByTypeL(m_owner.standardFolderId(i));
                            if (folder) {
                                CleanupReleasePushL(*folder);
                                getFolderSpecificMessagesL(*folder, sortCriteria);
                                CleanupStack::PopAndDestroy(folder);
                            }
                        }
                        i = static_cast<QMessage::StandardFolder>(static_cast<int>(i) + 1);
                    }
                    CleanupStack::PopAndDestroy(mailbox);
                }
            }
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
    
    if (m_numberOfHandledFilters == 0) {
        // None of the filters were handled
        // => Get all messages and let the engine do brute force filtering
        getAllMessagesL(sortCriteria);
    }

    if (!m_asynchronousSearchStarted) {
        // Messages were searched & filtered synchronously
        // => invoke SearchCompleted method asynchronously
        // (Note: Asynchronous searches invoke automatically SearchCompleted
        //        method as soon as search is finished)
        QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
    }
}

void CFSMessagesFindOperation::getAllMessagesL(TEmailSortCriteria& sortCriteria)
{
    // Get all messages from every known account
    QList<QMessageAccount> accounts = m_owner.m_accounts.values();
    for (int i=accounts.count()-1; i >= 0; i--) {
        getAccountSpecificMessagesL(accounts[i], sortCriteria);
    }

    if (m_searchOperations.count() == 0) {
        QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
    }
}

void CFSMessagesFindOperation::getAccountSpecificMessagesL(QMessageAccount& messageAccount, TEmailSortCriteria& sortCriteria)
{
    TMailboxId mailboxId(CFSEngine::fsMailboxIdFromQMessageAccountId(messageAccount.id()));
    FSSearchOperation operation;
    operation.m_mailbox = m_clientApi->MailboxL(mailboxId);
    operation.m_emailSortCriteria = sortCriteria;
    if (m_searchOperations.isEmpty()) {
        m_searchOperations.append(operation);
        QMetaObject::invokeMethod(this, "searchAccountFolders", Qt::QueuedConnection);
        m_asynchronousSearchStarted = true;
    } else {
        m_searchOperations.append(operation);
    }
}

void CFSMessagesFindOperation::getAccountSpecificMessagesFromAccountFoldersL(FSSearchOperation& searchOperation)
{
    MEmailMailbox* pEmailMailbox = searchOperation.m_mailbox;
    if (pEmailMailbox == NULL) {
        return;
    }

    RSortCriteriaArray sortCriteriaArray;
    CleanupClosePushL(sortCriteriaArray);
    sortCriteriaArray.Append(searchOperation.m_emailSortCriteria);

    RFolderArray folders;
    pEmailMailbox->GetFoldersL(folders);
    CleanupClosePushL(folders);
    for (TInt i=0; i < folders.Count(); i++) {
        MEmailFolder* pEmailFolder = folders[i];
        CleanupReleasePushL(*pEmailFolder);
        MMessageIterator* msgIterator = NULL;
        // TODO: Bug in CMessageIterator implementation
        //       CMessageIterator constructor does not call iPluginData.ClaimInstance()
        //       BUT CMessageIterator destructor calls iPluginData.ReleaseInstance()
        //       => BUG results crash sooner or later
        //
        // => Take reference count and make sure that reference count
        //    will be restored to its original value after iteration
        unsigned int refCount1 = pluginReferenceCount(pEmailFolder);
        TRAP_IGNORE(msgIterator = pEmailFolder->MessagesL(sortCriteriaArray));
        if (msgIterator) {
            MEmailMessage* msg = NULL;
            while ( NULL != (msg = msgIterator->NextL())) {
                QMessageId messageId = CFSEngine::qMessageIdFromFsMessageId(msg->MessageId());
                if (!MessageCache::instance()->contains(messageId)) {
                    QMessage message;
                    TRAPD(err, m_owner.CreateQMessageL(&message, *msg));
                    if (err == KErrNone) {
                        QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
                        privateMessage->_id = messageId;
                        privateMessage->_modified = false;
                    }
                    MessageCache::instance()->insert(message);
                }
                if (!m_excludeIdList.contains(messageId)) {
                    // Make sure that same ids won't be added twice
                    if (!m_idList.contains(messageId)) {
                        m_idList.append(messageId);
                    }
                }
            }
            msgIterator->Release();
        }
        // TODO: Bug in CMessageIterator implementation
        //       CMessageIterator constructor does not call iPluginData.ClaimInstance()
        //       BUT CMessageIterator destructor calls iPluginData.ReleaseInstance()
        //       => BUG results crash sooner or later
        //
        // Check if reference count was changed during iteration
        unsigned int refCount2 = pluginReferenceCount(pEmailFolder);
        if (refCount1 != refCount2) {
            // Reference count was changed
            // => Set original value to reference count
            setPluginReferenceCount(pEmailFolder, refCount1);
        }
        CleanupStack::PopAndDestroy(pEmailFolder);
    }
    CleanupStack::PopAndDestroy(&folders);
    CleanupStack::PopAndDestroy(&sortCriteriaArray);
}

void CFSMessagesFindOperation::getFolderSpecificMessagesL(MEmailFolder& folder, TEmailSortCriteria sortCriteria)
{
    RSortCriteriaArray sortCriteriaArray;
    CleanupClosePushL(sortCriteriaArray);
    sortCriteriaArray.Append(sortCriteria);

    // TODO: Bug in CMessageIterator implementation
    //       CMessageIterator constructor does not call iPluginData.ClaimInstance()
    //       BUT CMessageIterator destructor calls iPluginData.ReleaseInstance()
    //       => BUG results crash sooner or later
    //
    // => Take reference count and make sure that reference count
    //    will be restored to its original value after iteration
    unsigned int refCount1 = pluginReferenceCount(&folder);
    MMessageIterator* msgIterator = folder.MessagesL(sortCriteriaArray);
    if (msgIterator) {
        MEmailMessage* msg = NULL;
        while ( NULL != (msg = msgIterator->NextL())) {
            QMessageId messageId = CFSEngine::qMessageIdFromFsMessageId(msg->MessageId());;
            if (!MessageCache::instance()->contains(messageId)) {
                QMessage message;
                TRAPD(err, m_owner.CreateQMessageL(&message, *msg));
                if (err == KErrNone) {
                    QMessagePrivate* privateMessage = QMessagePrivate::implementation(message);
                    privateMessage->_id = messageId;
                    privateMessage->_modified = false;
                }
                MessageCache::instance()->insert(message);
            }
            if (!m_excludeIdList.contains(messageId)) {
                m_idList.append(messageId);
            }
        }
        msgIterator->Release();
        // TODO: Bug in CMessageIterator implementation
        //       CMessageIterator constructor does not call iPluginData.ClaimInstance()
        //       BUT CMessageIterator destructor calls iPluginData.ReleaseInstance()
        //       => BUG results crash sooner or later
        //
        // Check if reference count was changed during iteration
        unsigned int refCount2 = pluginReferenceCount(&folder);
        if (refCount1 != refCount2) {
            // Reference count was changed
            // => Set original value to reference count
            setPluginReferenceCount(&folder, refCount1);
        }
    }

    CleanupStack::PopAndDestroy(&sortCriteriaArray);
}

void CFSMessagesFindOperation::searchAccountFolders()
{
    TRAP_IGNORE(getAccountSpecificMessagesFromAccountFoldersL(m_searchOperations.first()));
    // Remove previous search
    if (m_searchOperations.first().m_mailbox) {
        m_searchOperations.first().m_mailbox->Release();
    }
    m_searchOperations.removeFirst();
    if (m_searchOperations.count() > 0) {
        // => Search continues
        QMetaObject::invokeMethod(this, "searchAccountFolders", Qt::QueuedConnection);
    } else {
        // searchOperation list is empty
        // => Search completed
        QMetaObject::invokeMethod(this, "SearchCompleted", Qt::QueuedConnection);
    }
}

void CFSMessagesFindOperation::SearchCompleted()
{
    if (!m_body.isEmpty()) {
        QMessageIdList idList;
        foreach (QMessageId messageId, m_idList) {
            if (filterBody(messageId))
                idList.append(messageId);   
        }
        m_idList = idList;
    }
    m_owner.filterAndOrderMessagesReady(true, m_operationId, m_idList, m_numberOfHandledFilters, m_resultCorrectlyOrdered);
}

bool CFSMessagesFindOperation::filterBody(QMessageId& messageId)
{
    Qt::CaseSensitivity caseSensitivity = (m_matchFlags & QMessageDataComparator::MatchCaseSensitive) ?
        Qt::CaseSensitive:Qt::CaseInsensitive;
    
    QMessage message = m_owner.message(messageId);
    QMessageContentContainer container = message.find(message.bodyId());
    return container.textContent().contains(m_body, caseSensitivity);
}

// TODO: Remove this function as soon as CMessageIterator bug is fixed
unsigned int CFSMessagesFindOperation::pluginReferenceCount(MEmailFolder* folder)
{
    CEmailFolder* pEmailFolder = static_cast<CEmailFolder*>(folder);
    return pEmailFolder->iPluginData.iRefCount;
}

// TODO: Remove this function as soon as CMessageIterator bug is fixed
void CFSMessagesFindOperation::setPluginReferenceCount(MEmailFolder* folder, unsigned int referenceCount)
{
    CEmailFolder* pEmailFolder = static_cast<CEmailFolder*>(folder);
    pEmailFolder->iPluginData.iRefCount = referenceCount;
}

#include "moc_qfsengine_symbian_p.cpp"

QTM_END_NAMESPACE
