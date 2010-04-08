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
#include "symbianhelpers_p.h"
#include "qmessageaccount.h"
#include "qmessageaccount_p.h"
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"
#include "qmessagecontentcontainer_symbian_p.h"
#include "qmessagefolder.h"
#include "qmessagefolder_p.h"
#include "qmessagefolderfilter.h"
#include "qmessagefolderfilter_p.h"

#include <emailinterfacefactory.h>
#include <QTextCodec>
#include <emailapidefs.h>
#include <memailmailbox.h>
#include <memailfolder.h>
#include <memailmessage.h>
#include <memailaddress.h>
#include <memailcontent.h>
#include <mmessageiterator.h>

#include <QMessageBox>

using namespace EmailInterface;

QTM_BEGIN_NAMESPACE

using namespace SymbianHelpers;
using namespace EmailInterface;

Q_GLOBAL_STATIC(CFSEngine,fsEngine);

CFSEngine::CFSEngine()
{
    m_factory = CEmailInterfaceFactory::NewL(); 
    m_ifPtr = m_factory->InterfaceL(KEmailClientApiInterface);
    m_clientApi = static_cast<MEmailClientApi*>(m_ifPtr);
}

CFSEngine::~CFSEngine()
{
    delete m_factory;
    m_ifPtr->Release();
}

CFSEngine* CFSEngine::instance()
{   
    return fsEngine();
}

QMessageAccountIdList CFSEngine::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageAccountIdList accountIds;

    TRAPD(err, updateEmailAccountsL());
    
    QMessageAccountFilterPrivate* privateMessageAccountFilter = QMessageAccountFilterPrivate::implementation(filter);
    if (filter.isEmpty()) {
        if (!privateMessageAccountFilter->_notFilter) {
            // All accounts are returned for empty filter
            foreach (QMessageAccount value, m_accounts) {
                QString v = value.id().toString();
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

    //TODO: Sort accounts according to QMessageAccountSortOrder
    
    return accountIds;
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
    return QMessageAccountId();
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
        QString fsIdAsString = addFreestylePrefix(idAsString);
        
        if (!m_accounts.contains(fsIdAsString)) {
            
            QMessageAccount account = QMessageAccountPrivate::from(
                QMessageAccountId(fsIdAsString),
                addFreestylePrefix(QString::fromUtf16(mailbox->MailboxName().Ptr(), mailbox->MailboxName().Length())),
                0, //TODO: ID for IMAP service if needed
                0, //TODO: ID for SMTP service if needed
                QMessage::Email);
            
            m_accounts.insert(fsIdAsString, account);

        } else {
            keys.removeOne(fsIdAsString);
        }

    }
    
    mailboxes.Reset();
    CleanupStack::PopAndDestroy();
    
    for (int i=0; i < keys.count(); i++) {
        m_accounts.remove(keys[i]);
    }
    
}

MEmailMessage* CFSEngine::createFSMessageL(const QMessage &message)
{
    TMailboxId mailboxId(removeFreestylePrefix(message.parentAccountId().toString()).toInt());
    MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);

    MEmailMessage* fsMessage = mailbox->CreateDraftMessageL();
    
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
            MEmailAddress* address = mailbox->AddressL();;
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
            MEmailAddress* address = mailbox->AddressL();;
            address->SetAddressL(receiver);
            bccAddress.Append(address);
        }
        fsMessage->SetRecipientsL(MEmailAddress::EBcc, bccAddress);
    }
    if (message.bodyId() == QMessageContentContainerPrivate::bodyContentId()) {
        // Message contains only body (not attachments)
        QString messageBody = message.textContent();
        if (!messageBody.isEmpty()) {
            MEmailMessageContent* content = fsMessage->ContentL();
            MEmailTextContent* textContent = content->AsTextContentOrNull();
            textContent->SetTextL(MEmailTextContent::EPlainText, TPtrC(reinterpret_cast<const TUint16*>(message.textContent().utf16())));
            // TODO:
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
    fsMessage->SetSubjectL(TPtrC(reinterpret_cast<const TUint16*>(message.subject().utf16())));
    fsMessage->SaveChangesL();
    return fsMessage;
}

bool CFSEngine::addMessage(QMessage* message)
{
    MEmailMessage* fsMessage;
    TRAPD(err, fsMessage = createFSMessageL(*message));
    if (err != KErrNone)
        return false;
    else
        return true;
}

bool CFSEngine::updateMessage(QMessage* message)
{
    TMailboxId mailboxId(removeFreestylePrefix(message->parentAccountId().toString()).toInt());
    MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);
  
    TMessageId messageId(message->id().toString().toInt(),
                            message->parentFolderId().toString().toInt(), 
                            mailboxId);
    MEmailMessage* fsMessage = mailbox->MessageL(messageId);
    
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
    
    // body and attachments
    
    fsMessage->SetSubjectL(TPtrC(reinterpret_cast<const TUint16*>(message->subject().utf16())));
    fsMessage->SaveChangesL();
    return true;
}

bool CFSEngine::removeMessage(const QMessageId &id, QMessageManager::RemovalOption option)
{
    bool retVal = false;
    return retVal;
}

bool CFSEngine::showMessage(const QMessageId &id)
{
    // we need to search from every mailbox, because we have only messageid to use..
    return false;
}

bool CFSEngine::composeMessage(const QMessage &message)
{
    bool retVal = false;
    MEmailMessage* fsMessage;
    TRAPD(err, 
        fsMessage = createFSMessageL(message);
        fsMessage->SaveChangesL();
        fsMessage->ShowMessageViewerL();
    );
    if (err != KErrNone)
        retVal = false;
    else
        retVal = true;   
    return retVal;
}

bool CFSEngine::retrieve(const QMessageId &messageId, const QMessageContentContainerId& id)
{
    // not supported in Email Client Api
    return false;
}

bool CFSEngine::retrieveBody(const QMessageId& id)
{
    // not supported in Email Client Api
     return false;
}

bool CFSEngine::retrieveHeader(const QMessageId& id)
{
    // not supported in Email Client Api
    return false;
}

bool CFSEngine::exportUpdates(const QMessageAccountId &id)
{
    return false;
}

bool CFSEngine::removeMessages(const QMessageFilter& /*filter*/, QMessageManager::RemovalOption /*option*/)
{
    return false;
}

bool CFSEngine::queryMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
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

    QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(m_messageQueries[m_messageQueries.count()-1].filter);
    if (pf->_filterList.count() == 0) {
        queryInfo.findOperation->filterAndOrderMessages(m_messageQueries[m_messageQueries.count()-1].filter,
            m_messageQueries[m_messageQueries.count()-1].sortOrder);
        return true;
    } else {
        queryInfo.findOperation->filterAndOrderMessages(pf->_filterList[0], m_messageQueries[m_messageQueries.count()-1].sortOrder);
        return true;
    }
    return false;
}

bool CFSEngine::queryMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return false;
}

bool CFSEngine::countMessages(QMessageServicePrivate& privateService, const QMessageFilter &filter)
{
    return false;
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
    m_messageQueries[index].privateService->messagesFound(ids, true, resultSetOrdered);
}

QMessageFolderIdList CFSEngine::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageFolderIdList ids;
    
    QMessageFolderFilter copyOfFilter = filter;
    //handleNestedFiltersFromFolderFilter(copyOfFilter);
    
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
    
    //if (!sortOrder.isEmpty()) {
    //    orderFolders(ids, sortOrder);
    //}
    
    //applyOffsetAndLimitToMsgFolderIds(ids, offset, limit);
    
    return ids;
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
            TMailboxId mailboxId(removeFreestylePrefix(accountId.toString()).toInt());
            mailbox = m_clientApi->MailboxL(mailboxId);
            break;
        }
    }

    TFolderId folderId(removeFreestylePrefix(id.toString()).toInt(), mailbox->MailboxId());
    MEmailFolder* emailFolder = mailbox->FolderL(folderId);
    CleanupReleasePushL(*emailFolder);
    QString name = QString::fromUtf16(emailFolder->Name().Ptr(), emailFolder->Name().Length());
    folder = QMessageFolderPrivate::from(id, accountId, parentId, name, name);
    CleanupStack::PopAndDestroy();
    
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
            break;
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
            break;
        case QMessageFolderFilterPrivate::AncestorFolderIds:
            break;
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
    QMessageAccount messageAccount = account(accountId);
    
    TMailboxId mailboxId(removeFreestylePrefix(accountId.toString()).toInt());
    MEmailMailbox* mailbox = NULL;
    mailbox = m_clientApi->MailboxL(mailboxId);

    if (mailbox == NULL)
        return QMessageFolderIdList();
    
    RFolderArray folders;
    
    mailbox->GetFoldersL(folders);
    CleanupClosePushL(folders);

    for(TInt i=0; i < folders.Count(); i++) {
        MEmailFolder *mailFolder = folders[i];
        
        QString fsIdAsString = addFreestylePrefix(QString::number(mailFolder->FolderId().iId));
        folderIds.append(QMessageFolderId(fsIdAsString));

        //TODO: Support for subfolders?
    }
    
    CleanupStack::PopAndDestroy(&folders);
    
    return folderIds;
}

QMessage CFSEngine::message(const QMessageId& id) const
{
    QMessage message;
    return message;
}

bool CFSEngine::sendEmail(QMessage &message)
{
    MEmailMessage* fsMessage;
    TRAPD(err,
        fsMessage = createFSMessageL(message);
        fsMessage->SaveChangesL();
        fsMessage->SendL();
        TMailboxId mailboxId(removeFreestylePrefix(message.parentAccountId().toString()).toInt());
        MEmailMailbox* mailbox = m_clientApi->MailboxL(mailboxId);       
        mailbox->SynchroniseL(*this);       
    );
    if (err != KErrNone)
        return false;
    else
        return true;
}

QString CFSEngine::attachmentTextContent(long int messageId, unsigned int attachmentId, const QByteArray &charset)
{
    QString result;
    
    QByteArray data = attachmentContent(messageId, attachmentId);
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
    }
    
    return result;
}

QByteArray CFSEngine::attachmentContent(long int messageId, unsigned int attachmentId)
{
    // TODO:
    QByteArray result;
    return result;
}

QMessageManager::NotificationFilterId CFSEngine::registerNotificationFilter(QMessageStorePrivate& aPrivateStore,
                                                                           const QMessageFilter &filter)
{
    int filterId;
    return filterId;
}

void CFSEngine::unregisterNotificationFilter(QMessageManager::NotificationFilterId notificationFilterId)
{

}

void CFSEngine::MailboxSynchronisedL(TInt aResult)
{
   
}

CFSMessagesFindOperation::CFSMessagesFindOperation(CFSEngine& aOwner, int aOperationId)
    : m_owner(aOwner), m_operationId(aOperationId),
        iResultCorrectlyOrdered(false)
{
}

CFSMessagesFindOperation::~CFSMessagesFindOperation()
{
}

void CFSMessagesFindOperation::filterAndOrderMessages(const QMessageFilter &filter, const QMessageSortOrder& sortOrder,
                                                    QString body, QMessageDataComparator::MatchFlags matchFlags)
{
    iFilterList.clear();
    iFilterList.append(filter);
    filterAndOrderMessages(iFilterList, sortOrder, body, matchFlags);
}

void CFSMessagesFindOperation::filterAndOrderMessages(const QMessageFilterPrivate::SortedMessageFilterList& filters,
                                                    const QMessageSortOrder& sortOrder,
                                                    QString body,
                                                    QMessageDataComparator::MatchFlags matchFlags)
{
    TRAPD(err, filterAndOrderMessagesL(filters, sortOrder, body, matchFlags));
    Q_UNUSED(err);
}

void CFSMessagesFindOperation::filterAndOrderMessagesL(const QMessageFilterPrivate::SortedMessageFilterList& filters,
                                                    const QMessageSortOrder& sortOrder,
                                                    QString body,
                                                    QMessageDataComparator::MatchFlags matchFlags)
{

    m_factory = CEmailInterfaceFactory::NewL(); 
    m_interfacePtr = m_factory->InterfaceL(KEmailClientApiInterface); 
    m_clientApi = static_cast<MEmailClientApi*>(m_interfacePtr); 
    m_clientApi->GetMailboxesL(m_mailboxes);

    SearchMessagesInMailboxL();
}

void CFSMessagesFindOperation::SearchMessagesInMailboxL()
{
    // TODO: currently lists all messages in mailbox. Implement filtering
    m_iteration = 0;
    for(m_iteration = 0; m_iteration < m_mailboxes.Count(); m_iteration++) {
        m_mailbox = m_mailboxes[m_iteration];
        
        RFolderArray folders;
        RSortCriteriaArray sortCriteria;
        m_mailbox->GetFoldersL(folders);
        CleanupClosePushL(folders);
        
        sortCriteria.Append(TEmailSortCriteria());
    
        for(TInt i=0; i < folders.Count(); i++) {
            MEmailFolder *mailFolder = folders[i];
            EmailInterface::MMessageIterator* msgIterator = mailFolder->MessagesL(sortCriteria);
            
            MEmailMessage* msg = msgIterator->NextL();
            while (msg != NULL) {
                iIdList.append(QMessageId(QString::number(msg->MessageId().iId)));
                msg = msgIterator->NextL();
            }
            
            
        }
        CleanupStack::PopAndDestroy(&folders);
    }
    m_owner.filterAndOrderMessagesReady(true, m_operationId, iIdList, 1, true);
    
}

void CFSMessagesFindOperation::HandleResultL(MEmailMessage* aMessage)
{
    qDebug() << aMessage->MessageId().iId;
}

void CFSMessagesFindOperation::SearchCompletedL()
{
    qDebug() << "CFSMessagesFindOperation::SearchCompletedL()";
}
QTM_END_NAMESPACE
