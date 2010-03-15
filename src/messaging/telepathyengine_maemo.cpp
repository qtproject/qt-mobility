#include "qmessageglobal.h"
#include "qmessagemanager.h"
#include "qmessageaccount.h"
#include "qmessageaccountid.h"
#include "qmessageaccount_p.h"
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"
#include "qmessageservice.h"
#include "qmessage.h"
#include "telepathyengine_maemo_p.h"
#include "maemohelpers_p.h"


QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(TelepathyEngine,telepathyEngine);

TelepathyEngine::TelepathyEngine()
{
    qDebug() << "TelepathyEngine::TelepathyEngine()";
    tpSession=new TpSession("ring",TRUE); // Create as sync, telephony "ring" as default
    qDebug() << "TelepathyEngine::TelepathyEngine() exit";

}

TelepathyEngine::~TelepathyEngine()
{

}

TelepathyEngine* TelepathyEngine::instance()
{
    return telepathyEngine();
}

bool TelepathyEngine::sendMessage(QMessage &message)
{
    QMessage::Type type=message.type();
    QString cm=type == QMessage::Sms ? "ring" :  type == QMessage::Xmpp ? "gabble" : "";
    QMessageAddressList toList=message.to();
    QMessageAccountId account=message.parentAccountId();
    if(!cm.isEmpty()) {
        foreach(QMessageAddress to,toList) {
             tpSession->sendMessageToAddress(cm,to.recipient(),message.textContent());
        };
    }
    else
        qDebug() << "TelepathyEngine::sendMessage unsupported message type" << type;
}



bool TelepathyEngine::queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return true;
}

bool TelepathyEngine::queryMessages(QMessageService& messageService, const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset) const
{
    return true;
}

bool TelepathyEngine::countMessages(QMessageService& messageService, const QMessageFilter &filter)
{
    return true;
};

void TelepathyEngine::updateImAccounts() const
{
//    iDefaultImAccountId = QMessageAccountId();
    iAccounts.clear();
    foreach (TpSessionAccount *tpacc, tpSession->accounts) {
        qDebug() << "TelepathyEngine::updateImAccounts" << tpacc->acc->cmName() << " " << tpacc->acc->protocol() << " " << tpacc->acc->displayName();
        bool account_ok = tpacc->acc->isEnabled() && tpacc->acc->isValidAccount();
        QString cm=tpacc->acc->cmName();
        if (account_ok) {
            if(cm=="ring") { // Ring connection manager for cellular telephony
                QString accountId = tpacc->acc->uniqueIdentifier();
                QString accountName = "SMS";
                QString accountAddress = "";
                QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(accountId),
                                                                       accountName,
                                                                       QMessageAddress(QMessageAddress::Phone, accountAddress),
                                                                       QMessage::Sms);
                iAccounts.insert(accountId, account);
                defaultSmsAccountId=accountId;
            } else
             if(cm=="gabble") { // Gabble for googletalk
                QString accountId = tpacc->acc->uniqueIdentifier();
                QString accountName = tpacc->acc->normalizedName();
                QString accountAddress = tpacc->acc->normalizedName();
                QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(accountId),
                                                                       accountName,
                                                                       QMessageAddress(QMessageAddress::Xmpp, accountAddress),
                                                                       QMessage::Xmpp);
                iAccounts.insert(accountId, account);
            } else qDebug() << "Protocol " << tpacc->acc->protocol() << "with connectionmanager " << cm << "Is not yet supported";
//                if (strncmp(account_name_key, default_account, strlen(default_account))) iDefaultEmailAccountId = accountId;

            }
        }
}

QMessageAccountIdList TelepathyEngine::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder,
                                                  uint limit, uint offset, bool &isFiltered, bool &isSorted) const
{
    QMessageAccountIdList accountIds;

    updateImAccounts();
    foreach (QMessageAccount value, iAccounts) {
        accountIds.append(value.id());
    }

    MessagingHelper::filterAccounts(accountIds, filter);
    isFiltered = true;

    isSorted = false;

    return accountIds;
}

int TelepathyEngine::countAccounts(const QMessageAccountFilter &filter) const
{
    bool isFiltered, isSorted;
    return queryAccounts(filter, QMessageAccountSortOrder(), 0, 0, isFiltered, isSorted).count();
}

QMessageAccount TelepathyEngine::account(const QMessageAccountId &id) const
{
    updateImAccounts();
    return iAccounts[id.toString()];
}

QMessageAccountId TelepathyEngine ::defaultAccount(QMessage::Type type) const
{
    updateImAccounts();
    return defaultSmsAccountId;
}

QTM_END_NAMESPACE
