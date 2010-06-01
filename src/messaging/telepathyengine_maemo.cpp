/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
  tpSession=TpSession::instance(TRUE); // Create as sync, telephony "ring" as default
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
  bool retVal=false;
  QMessage::Type type=message.type();
  QMessageAccountId account=message.parentAccountId();
  QString cm=type == QMessage::Sms ? "ring" :  type == QMessage::InstantMessage ? account.toString() : "";
  QMessageAddressList toList=message.to();
  TpSessionAccount *tpsa=tpSession->getAccount(account.toString());
   qDebug() << "sendMessage account:" << account.toString() << tpsa;
  if(!tpsa) return false;
  if(!cm.isEmpty()) {
    foreach(QMessageAddress to,toList) {
      connect(tpsa,SIGNAL(messageQueued(TpSessionAccount *,bool)),SLOT(onMessageQueued(TpSessionAccount *,bool)));
      tpsa->sendMessageToAddress(to.addressee(),message.textContent());
      loop.exec(); // Wait untill this message has been queued to sent next one
      // qDebug() << "sendMessage loop exit";
      retVal=true;
    };
  }
  else
    qDebug() << "TelepathyEngine::sendMessage unsupported message type" << type;
  return retVal;
}


 void TelepathyEngine::onMessageSent(const Tp::Message &,TpSessionAccount *)
 {
    // qDebug() << "onMessageSent:" ;
    // loop.quit();
 };

 void TelepathyEngine::onMessageQueued(TpSessionAccount *,bool status)
 {
     // qDebug() << "onMessageQueued:" << status;
     loop.quit(); // If queuinf failed, exit loop
 };



void TelepathyEngine::updateImAccounts() const
{
//    iDefaultImAccountId = QMessageAccountId();
//  qDebug() << "TelepathyEngine::updateImAccounts";
    iAccounts.clear();
    foreach (TpSessionAccount *tpacc, tpSession->accounts) {
      //     qDebug() << "TelepathyEngine::updateImAccounts" << tpacc->acc->cmName() << " " << tpacc->acc->protocol() << " " << tpacc->acc->displayName();
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
               //  qDebug() << "updateImAccounts Sms id:" << accountId << " accountName:" << accountName << "accountAddress" << accountAddress;
                iAccounts.insert(accountId, account);
                defaultSmsAccountId=accountId;
            } else {
                QString accountId = tpacc->acc->uniqueIdentifier();
                QString accountName = tpacc->acc->normalizedName();
                QString accountAddress = tpacc->acc->normalizedName();
                // Some ugly hardcoded hacks
                if(tpacc->acc->protocol()=="skype") accountName+=QString("@skype");
                if(!accountName.contains("@")) accountName+=QString("@")+tpacc->acc->parameters()["server"].toString();
                // qDebug() << "updateImAccounts InstantMessage id:" << accountId << " accountName:" << accountName << "accountAddress" << accountAddress;
                QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(accountId),
                                                                       accountName,
                                                                       QMessageAddress(QMessageAddress::InstantMessage, accountAddress),
                                                                       QMessage::InstantMessage);
                iAccounts.insert(accountId, account);
            }
//            else qDebug() << "Protocol " << tpacc->acc->protocol() << "with connectionmanager " << cm << "Is not yet supported";
//                if (strncmp(account_name_key, default_account, strlen(default_account))) iDefaultEmailAccountId = accountId;
        }
        }
}

QMessageAccountIdList TelepathyEngine::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder,
                                                  uint limit, uint offset, bool &isFiltered, bool &isSorted) const
{
    Q_UNUSED(sortOrder);
    Q_UNUSED(limit);
    Q_UNUSED(offset);
  //  qDebug() << "TelepathyEngine::queryAccounts";
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
  //  qDebug() << "TelepathyEngine::defaultAccount";
    updateImAccounts();
    return defaultSmsAccountId;
}

#include "moc_telepathyengine_maemo_p.cpp"
QTM_END_NAMESPACE
