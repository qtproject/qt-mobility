/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QDebug>

#include <TelepathyQt4/Debug>

#include "qmessageglobal.h"
#include "qmessagemanager.h"
#include "qmessageaccount.h"
#include "qmessageaccountid.h"
#include "qmessageaccount_p.h"
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"
#include "qmessageservice.h"
#include "qmessage.h"
#include "telepathyengine_maemo6_p.h"
#include "maemo6helpers_p.h"
#include "telepathyhelpers_maemo6_p.h"

Q_GLOBAL_STATIC(TelepathyEngine, telepathyEngine);

TelepathyEngine::TelepathyEngine():
        m_sync(true)
{
    QDEBUG_FUNCTION_BEGIN
    Tp::registerTypes();
    Tp::enableDebug(false);
    Tp::enableWarnings(true);

    if (!(m_AM = Tp::AccountManager::create()))
        qWarning() << __FUNCTION__ << "Cannot create Account Manager (m_am)";
    else {
        connect(m_AM->becomeReady(), SIGNAL(finished(Tp::PendingOperation *)),
                SLOT(onAMReady(Tp::PendingOperation *)));
        connect(m_AM.data(), SIGNAL(accountCreated(const QString &)),
                SLOT(onAccountCreated(const QString &)));
    }


    m_CMName = CM_NAME_DEFAULT;

    if (m_sync)
        m_loop.exec(); // Loop locally untill accounts are initialized

    QDEBUG_FUNCTION_END
}

/*
bool TelepathyEngine::initialize()
{
    QDEBUG_FUNCTION_BEGIN
    bool ret = false;

    if (m_AM)
    {
        QObject::connect(m_AM->becomeReady(),SIGNAL(finished(Tp::PendingOperation *)),
            this, SLOT(onAMReady(Tp::PendingOperation *)));
        QObject::connect(m_AM.data(), SIGNAL(accountCreated(const QString &)),
            this, SLOT(onAccountCreated(const QString &)));
        ret = true;
    }

    QDEBUG_FUNCTION_END return ret;
}
*/
TelepathyEngine::~TelepathyEngine()
{

}

TelepathyEngine* TelepathyEngine::instance()
{
    TelepathyEngine* te = telepathyEngine();

    return te;
}

/******************************************************************/
/*                 PRIVATE SLOTS                                  */
/******************************************************************/
void TelepathyEngine::onAMReady(Tp::PendingOperation *op)
{
    QDEBUG_FUNCTION_BEGIN

    if (op && op->isError()) {
        qWarning() << "Account manager cannot become ready:" << op->errorName() << "-" << op->errorMessage();
        return;
    }

    TpSessionAccount *tpacc;

    foreach(const QString &path, m_AM->allAccountPaths()) {
        qDebug() << __FUNCTION__ << "found account";
        m_accounts += tpacc = new TpSessionAccount(m_AM, path);
        connect(tpacc, SIGNAL(accountReady(TpSessionAccount*)),
                SLOT(onAccountReady(TpSessionAccount *)));
    }


    // AccountManager is now ready
    qDebug() << "Valid accounts:";
    foreach(const QString &path, m_AM->validAccountPaths()) {
        qDebug() << " path:" << path;
    }

    QDEBUG_FUNCTION_END
}

/******************************************************************/

void TelepathyEngine::onAccountCreated(const QString &path)
{
    QDEBUG_FUNCTION_BEGIN
    qWarning() << "TelepathyEngine::onAccountCreated";
    m_accounts += new TpSessionAccount(m_AM, path);
    QDEBUG_FUNCTION_END
}

/******************************************************************/

void TelepathyEngine::onAccountReady(TpSessionAccount *tpacc)
{
    QDEBUG_FUNCTION_BEGIN
    qDebug() << "TpSession::onAccountReady:Account " << tpacc->acc->cmName() << "is Ready";

    connect(tpacc, SIGNAL(messageReceived(const Tp::ReceivedMessage &, TpSessionAccount *)),
            SLOT(onMessageReceived(const Tp::ReceivedMessage &, TpSessionAccount *)));

    if (!m_CMName.isEmpty() && tpacc->acc->cmName() == m_CMName) {
        if (m_sync) {
            m_sync = false;
            m_loop.quit();
        }

        emit accountReady(tpacc);

        if (!m_reqMsg.isEmpty())
            tpacc->sendMessageToAddress(m_reqAddress, m_reqMsg);

    }
    QDEBUG_FUNCTION_END
}

/******************************************************************/

void TelepathyEngine::onReady(Tp::PendingOperation *)
{
    QDEBUG_FUNCTION_BEGIN
    QDEBUG_FUNCTION_END
};

/******************************************************************/

void TelepathyEngine::onMessageReceived(const Tp::ReceivedMessage &msg, TpSessionAccount *acc)
{
    QDEBUG_FUNCTION_BEGIN
    qDebug() << msg.text() << "from " << msg.sender()->id();

    emit messageReceived(msg, acc);
    QDEBUG_FUNCTION_END
}

/******************************************************************/

/**
 * Send message using specified connection manager to address
 *
 * \param connectionMgr  Name of the connection manager
 * \param address Valid address for this connection manager type. Asexample telephone number to Ring, GoogleTalk address for Gabble
 * \param message Message body
 */
bool TelepathyEngine::_sendMessageToAddress(QString connectionMgr, QString address, QString message)
{
    QDEBUG_FUNCTION_BEGIN

    TpSessionAccount *tpsa = _getTpSessionAccount(connectionMgr);

    if (tpsa)
        return tpsa->sendMessageToAddress(address, message);
    QDEBUG_FUNCTION_END
    return false;	
}

/**
 * Returns pointer to TpSessionAccout object with specified connection manager or protocol, returns NULL if no match found
 *
 * \param cm  Name of the connection manager, if left empty matches every entry
 * \param protocol Name of the protocol manager, if left empty matches every entry
 */
TpSessionAccount* TelepathyEngine::_getTpSessionAccount(const  QString cm, QString protocol)
{
    QDEBUG_FUNCTION_BEGIN
    qWarning() << "TelepathyEngine::getAccount" << cm << " " << protocol;

    foreach(TpSessionAccount *tpacc, m_accounts) {
        if ((!cm.isEmpty()  && tpacc->acc->cmName() == cm)
                || (!protocol.isEmpty() && tpacc->acc->protocol() == protocol)) {
            qWarning() << "TelepathyEngine::getAccount found" << tpacc->acc->cmName() << " " << tpacc->acc->protocol();
            QDEBUG_FUNCTION_END return tpacc;
        }
    }
    QDEBUG_FUNCTION_END return NULL;
}

bool TelepathyEngine::sendMessage(QMessage &message)
{
    QDEBUG_FUNCTION_BEGIN

    bool retVal(false);	
    QMessage::Type type = message.type();
    QString cm = (type == QMessage::Sms) ? "ring" : (type == QMessage::InstantMessage) ? "gabble" : "";
    QMessageAddressList toList = message.to();
    QMessageAccountId accountId = message.parentAccountId();

    qDebug() << __FUNCTION__ << "accountId: " << accountId.toString() ;

    if (!cm.isEmpty()) {
        foreach (const QMessageAddress &to, toList) {
            qDebug() << __FUNCTION__ << " cm: " << cm << " to: " << to.addressee() << " text: " << message.textContent();
            if (!_sendMessageToAddress(cm, to.addressee(), message.textContent()))
		return false;
        }
	retVal = true;
    } else
        qWarning() << "TelepathyEngine::sendMessage unsupported message type" << type;
    QDEBUG_FUNCTION_END
	
   return retVal;	
}

void TelepathyEngine::_updateImAccounts() const
{
    QDEBUG_FUNCTION_BEGIN
//    iDefaultImAccountId = QMessageAccountId();
    m_iAccounts.clear();


    foreach(TpSessionAccount *tpacc, m_accounts) {
        qDebug() << "TelepathyEngine::updateImAccounts" << tpacc->acc->cmName() << " protocol " << tpacc->acc->protocol() << " displayName " << tpacc->acc->displayName();
        bool account_ok = tpacc->acc->isEnabled() && tpacc->acc->isValidAccount();
        QString cm = tpacc->acc->cmName();

        qDebug() << "TelepathyEngine::updateImAccounts account_ok: " << account_ok << " isEnabled: " << tpacc->acc->isEnabled() << " isValidAccount: " << tpacc->acc->isValidAccount();

        if (account_ok) {
            if (cm == "ring") { // Ring connection manager for cellular telephony
                QString accountId = tpacc->acc->uniqueIdentifier();
                QString accountName = "SMS";
                QString accountAddress = "";
                QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(accountId),
                                          accountName,
                                          QMessageAddress(QMessageAddress::Phone, accountAddress),
                                          QMessage::Sms);
                m_iAccounts.insert(accountId, account);
                m_defaultSmsAccountId = accountId;
                qDebug() << __FUNCTION__ << " Setting m_defaultSmsAccountId: " << m_defaultSmsAccountId.toString() << endl;
            } else if (cm == "gabble") { // Gabble for googletalk
                QString accountId = tpacc->acc->uniqueIdentifier();
                QString accountName = tpacc->acc->normalizedName();
                QString accountAddress = tpacc->acc->normalizedName();
                QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(accountId),
                                          accountName,
                                          QMessageAddress(QMessageAddress::InstantMessage, accountAddress),
                                          QMessage::InstantMessage);
                m_iAccounts.insert(accountId, account);
            } else qWarning() << "Protocol " << tpacc->acc->protocol() << "with connectionmanager " << cm << "Is not yet supported";
//                if (strncmp(account_name_key, default_account, strlen(default_account))) iDefaultEmailAccountId = accountId;

        }
    }
    QDEBUG_FUNCTION_END
}

QMessageAccountIdList TelepathyEngine::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder,
        uint limit, uint offset, bool &isFiltered, bool &isSorted) const
{
    QDEBUG_FUNCTION_BEGIN
    Q_UNUSED(sortOrder);
    Q_UNUSED(limit);
    Q_UNUSED(offset);

    QMessageAccountIdList accountIds;

    _updateImAccounts();
    foreach(QMessageAccount value, m_iAccounts) {
        accountIds.append(value.id());
    }

    MessagingHelper::filterAccounts(accountIds, filter);
    isFiltered = true;

    isSorted = false;

    QDEBUG_FUNCTION_END return accountIds;
}

int TelepathyEngine::countAccounts(const QMessageAccountFilter &filter) const
{
    QDEBUG_FUNCTION_BEGIN
    bool isFiltered, isSorted;
    QDEBUG_FUNCTION_END return queryAccounts(filter, QMessageAccountSortOrder(), 0, 0, isFiltered, isSorted).count();
}

QMessageAccount TelepathyEngine::account(const QMessageAccountId &id) const
{
    QDEBUG_FUNCTION_BEGIN
    _updateImAccounts();
    QDEBUG_FUNCTION_END return m_iAccounts[id.toString()];
}

QMessageAccountId TelepathyEngine ::defaultAccount(QMessage::Type type) const
{
    QDEBUG_FUNCTION_BEGIN
    Q_UNUSED(type);

    _updateImAccounts();
    return m_defaultSmsAccountId;
    QDEBUG_FUNCTION_END
}

