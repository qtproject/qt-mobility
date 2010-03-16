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

#include <emailinterfacefactory.h>
#include <QTextCodec>
#include <emailapidefs.h>
#include <memailmailbox.h>


using namespace EmailInterface;

QTM_BEGIN_NAMESPACE


CFSEngine::CFSEngine()
{
    CEmailInterfaceFactory* factory = CEmailInterfaceFactory::NewL(); 
    CleanupStack::PushL(factory); 
    MEmailInterface* ifPtr = factory->InterfaceL(KEmailClientApiInterface); 
    m_clientApi = static_cast<MEmailClientApi*>(ifPtr); 
    /*CleanupReleasePushL(clientApi); 
    
    RMailboxPtrArray mailboxes;
    clientApi->GetMailboxesL(mailboxes);
    CleanupStack::PopAndDestroy(2); // clientApi and factory*/
    CleanupStack::PopAndDestroy(factory); // factory
}

CFSEngine::~CFSEngine()
{
    m_clientApi->Release();
}

CFSEngine* CFSEngine::instance()
{

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
    m_clientApi->GetMailboxesL(mailboxes);
    CleanupClosePushL(mailboxes);

    for (TInt i = 0; i < mailboxes.Count(); i++) {
        MEmailMailbox *mailbox = mailboxes[i];
        QString idAsString = QString::number(mailbox->MailboxId().iId);
        QString fsIdAsString = SymbianHelpers::addFreestylePrefix(idAsString);

        if (!m_accounts.contains(fsIdAsString)) {
            
            QMessageAccount account = QMessageAccountPrivate::from(
                QMessageAccountId(SymbianHelpers::addFreestylePrefix(fsIdAsString)),
                QString::fromUtf16(mailbox->MailboxName().Ptr(), mailbox->MailboxName().Length()),
                0, //TODO: ID for IMAP service if needed
                0, //TODO: ID for SMTP service if needed
                QMessage::Email);
            
            m_accounts.insert(fsIdAsString, account);
        } else {
            keys.removeOne(fsIdAsString);
        }
    }
    
    CleanupStack::PopAndDestroy(&mailboxes);  // mailboxes
    
    for (int i=0; i < keys.count(); i++) {
        m_accounts.remove(keys[i]);
    }
    
}

bool CFSEngine::addMessage(QMessage* message)
{
    bool retVal = false;   
    return retVal;
}

bool CFSEngine::updateMessage(QMessage* message)
{
    bool retVal = false;   
    return retVal;
}

bool CFSEngine::removeMessage(const QMessageId &id, QMessageManager::RemovalOption option)
{
    bool retVal = false;
    return retVal;
}

bool CFSEngine::showMessage(const QMessageId &id)
{
    return false;
}


bool CFSEngine::composeMessage(const QMessage &message)
{
    bool retVal = true;
    return retVal;
}

bool CFSEngine::retrieve(const QMessageId &messageId, const QMessageContentContainerId& id)
{
    return false;
}

bool CFSEngine::retrieveBody(const QMessageId& id)
{
     return false;
}

bool CFSEngine::retrieveHeader(const QMessageId& id)
{
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
    
}

QMessageFolderIdList CFSEngine::queryFolders(const QMessageFolderFilter &filter, const QMessageFolderSortOrder &sortOrder, uint limit, uint offset) const
{
    QMessageFolderIdList ids;
    return ids;
}

int CFSEngine::countFolders(const QMessageFolderFilter &filter) const
{
    return queryFolders(filter, QMessageFolderSortOrder(), 0, 0).count();
}

QMessageFolder CFSEngine::folder(const QMessageFolderId &id) const
{   
    QMessageFolder folder;
    return folder;
}

QMessage CFSEngine::message(const QMessageId& id) const
{
    QMessage message;
    return message;
}

bool CFSEngine::storeEmail(QMessage &message)
{
    return false;
}

bool CFSEngine::sendEmail(QMessage &message)
{
    return false;
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

QTM_END_NAMESPACE
