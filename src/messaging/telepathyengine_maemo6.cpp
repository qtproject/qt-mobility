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

#include "telepathyengine_maemo6_p.h"
#include "telepathyhelpers_maemo6_p.h"
#include "maemo6helpers_p.h"

#include "qmessageaccount.h"
#include "qmessageaccount_p.h"
#include "qmessageaccountfilter.h"
#include "qmessageservice_maemo6_p.h"
#include "qmessage_p.h"

#include <TelepathyQt4/Debug>
#include <TelepathyQt4/Account>
#include <TelepathyQt4/PendingReady>
#include <TelepathyQt4/TextChannel>

#include <QDebug>

Q_GLOBAL_STATIC(TelepathyEngine, telepathyEngine);

TelepathyEngine::TelepathyEngine()
    : m_sync(true)
    , m_error(QMessageManager::NoError)
{
    QDEBUG_FUNCTION_BEGIN
 
    Tp::registerTypes();
    Tp::enableDebug(false);
    Tp::enableWarnings(true);

    m_AM = Tp::AccountManager::create();
    if (!m_AM) {
	qWarning() << __FUNCTION__ << "Cannot create Account Manager (m_am)";
	return;
    }

    connect(m_AM->becomeReady(), SIGNAL(finished(Tp::PendingOperation *)),
	    SLOT(onAMReady(Tp::PendingOperation *)));

    if (m_sync)
        m_loop.exec(); // Loop locally untill accounts are initialized

    QDEBUG_FUNCTION_END
}

TelepathyEngine::~TelepathyEngine()
{

}

TelepathyEngine *TelepathyEngine::instance()
{
    return telepathyEngine();
}

void TelepathyEngine::onAMReady(Tp::PendingOperation *op)
{
    QDEBUG_FUNCTION_BEGIN

    m_error = convertError(op);

    if (op && op->isError()) {
	syncDone();
        qWarning() << "Account manager cannot become ready:" << op->errorName() << "-" << op->errorMessage();
        return;
    }

    QVariantMap filter;
    filter.insert(QLatin1String("cmName"), QLatin1String("ring"));
    filter.insert(QLatin1String("protocolName"), QLatin1String("tel"));
    filter.insert(QLatin1String("enabled"), true);
     
    m_accountSet = m_AM->filterAccounts(filter);

    connect(m_accountSet.data(), SIGNAL(accountAdded(const Tp::AccountPtr &)),
	    this, SLOT(onAccountAdded(const Tp::AccountPtr &)));
    connect(m_accountSet.data(), SIGNAL(accountRemoved(const Tp::AccountPtr &)),
	    this, SLOT(onAccountRemoved(const Tp::AccountPtr &)));
    
    m_initList.clear();
    foreach (const Tp::AccountPtr &acc, m_accountSet->accounts()) {
	connect(acc->becomeReady(), SIGNAL(finished(Tp::PendingOperation *)),
		this, SLOT(onAccountReady(Tp::PendingOperation *)));
	m_initList.append(acc->uniqueIdentifier());
    }

    QDEBUG_FUNCTION_END
}

void TelepathyEngine::onAccountAdded(const Tp::AccountPtr &account)
{
    QDEBUG_FUNCTION_BEGIN;

    connect(account->becomeReady(), SIGNAL(finished(Tp::PendingOperation *)),
	    this, SLOT(onAccountReady(Tp::PendingOperation *)));

    QDEBUG_FUNCTION_END
}

void TelepathyEngine::onAccountRemoved(const Tp::AccountPtr &account)
{
    QDEBUG_FUNCTION_BEGIN

    foreach (const AccountPair &pair, m_accounts) {
	if (pair.first->cmName() == account->cmName() &&
	    pair.first->protocolName() == account->protocolName()) {
	    m_accounts.remove(pair.second.id().toString());
	}
    }

    QDEBUG_FUNCTION_END
}

void TelepathyEngine::onAccountReady(Tp::PendingOperation *op)
{
    QDEBUG_FUNCTION_BEGIN

    Tp::AccountPtr account = Tp::AccountPtr(Tp::SharedPtr<Tp::Account>::dynamicCast(op->object()));

    if (op->isError()) {
        qWarning() << "Account cannot become ready:" << op->errorName() << "-" << op->errorMessage();
    } else {
	addAccount(account);
    }
    
    if (m_sync) {
	m_initList.removeOne(account->uniqueIdentifier());
	if (m_initList.isEmpty())
	    syncDone();
    }

    QDEBUG_FUNCTION_END
}


void TelepathyEngine::syncDone()
{    
    QDEBUG_FUNCTION_BEGIN

    if (m_loop.isRunning())
	m_loop.quit();
    m_sync = false;

    QDEBUG_FUNCTION_END
}
 
void TelepathyEngine::addAccount(const Tp::AccountPtr &acc)
{
    QDEBUG_FUNCTION_BEGIN

    const QString cm = acc->cmName();

    if (cm == "ring") { // Ring connection manager for cellular telephony
	QString accountId = acc->uniqueIdentifier();
	QString accountName = "SMS";
	QString accountAddress = "";
	QMessageAccount account = QMessageAccountPrivate::from(QMessageAccountId(accountId),
							       accountName,
							       QMessageAddress(QMessageAddress::Phone, accountAddress),
							       QMessage::Sms);
	m_accounts.insert(accountId, AccountPair(acc, account));
	qDebug() << "SMS account Id:" << accountId << "cmname:" << acc->cmName()  << "protocol:" << acc->protocolName() << "servicename" << acc->serviceName(); 
    } else { 
	qWarning() << "Protocol " << acc->protocolName() << "with connectionmanager " << cm << "Is not yet supported";
    }

    QDEBUG_FUNCTION_END
}

bool TelepathyEngine::sendMessage(QMessage &message, QMessageService *service)
{
    QDEBUG_FUNCTION_BEGIN

    bool retVal(false);	

    QString id = message.parentAccountId().toString();
    qDebug() << __FUNCTION__ << "accountId: " << id;

    if (m_accounts.contains(id)) {
	Tp::AccountPtr acc = m_accounts[id].first;
	SendRequest *request = new SendRequest(message, service);
	foreach (const QString &contactIdentifier, request->to()) {
	    Tp::ContactMessengerPtr messenger = Tp::ContactMessenger::create(acc, contactIdentifier);
	    if (messenger) {
		Tp::PendingSendMessage *pendingMessage = messenger->sendMessage(request->text());
		connect(pendingMessage, SIGNAL(finished(Tp::PendingOperation *)),
			request, SLOT(finished(Tp::PendingOperation *)));
		request->addMessenger(messenger);
	    } else {
		request->setFinished(contactIdentifier, false);
	    }
	}
	if (request->requestCount())
	    retVal = true;
	else
	    delete request;
    } else {
        qWarning() << "TelepathyEngine::sendMessage : unsupported message type :" << message.type();
    }
    
    QDEBUG_FUNCTION_END
    
    return retVal;	
}

QMessageAccountIdList TelepathyEngine::queryAccounts(const QMessageAccountFilter &filter, const QMessageAccountSortOrder &sortOrder,
						     uint limit, uint offset, bool &isFiltered, bool &isSorted)
{
    QDEBUG_FUNCTION_BEGIN
    Q_UNUSED(sortOrder);
    Q_UNUSED(limit);
    Q_UNUSED(offset);

    QMessageAccountIdList accountIds;
    m_error = QMessageManager::NoError;

    foreach (const AccountPair &pair, m_accounts) {
        accountIds.append(pair.second.id());
    }

    MessagingHelper::filterAccounts(accountIds, filter);
    isFiltered = true;

    isSorted = false;

    QDEBUG_FUNCTION_END return accountIds;
}

int TelepathyEngine::countAccounts(const QMessageAccountFilter &filter)
{
    QDEBUG_FUNCTION_BEGIN
    bool isFiltered, isSorted;
    m_error = QMessageManager::NoError;
    QDEBUG_FUNCTION_END return queryAccounts(filter, QMessageAccountSortOrder(), 0, 0, isFiltered, isSorted).count();
}

QMessageAccount TelepathyEngine::account(const QMessageAccountId &id)
{
    QDEBUG_FUNCTION_BEGIN

    QMessageAccount result;
   
    const QString accountId = id.toString();
    if (m_accounts.contains(accountId)) {
	result = m_accounts[accountId].second;
    }

    QDEBUG_FUNCTION_END

    return result;
}

QMessageAccountId TelepathyEngine::defaultAccount(QMessage::Type type)
{
    QDEBUG_FUNCTION_BEGIN

    QMessageAccountId result;	
    
    foreach (const AccountPair &pair, m_accounts) {
	if (pair.second.messageTypes() & type) {
	    result = pair.second.id();
	    break;
	}
    }

    m_error = QMessageManager::NoError;

    QDEBUG_FUNCTION_END
	
    return result;
}

QMessageManager::Error TelepathyEngine::convertError(const Tp::PendingOperation *op)
{
    if (!op || op->isError())
        return QMessageManager::FrameworkFault ;

    if (!op->isValid())
        return QMessageManager::RequestIncomplete;

    return QMessageManager::NoError;
}

QMessageManager::Error TelepathyEngine::error() const
{
    return m_error;
}

// SMS is stored by the Harmattan. No need to duplicate it.
//#define TELEPATHY_ENGINE_STORE_MESSAGE

SendRequest::SendRequest(const QMessage &message, QMessageService *service)
    : _service(service)
    , _message(message)
    , _pendingRequestCount(message.to().count())
    , _failCount(0)
{
    if (_service)
        connect(_service, SIGNAL(destroyed(QObject*)), SLOT(onServiceDestroyed(QObject*)));
#ifdef TELEPATHY_ENGINE_STORE_MESSAGE
    QMessagePrivate::setStandardFolder(_message, QMessage::DraftsFolder);
    if (!QMessageManager().addMessage(&_message)) {
	qWarning() << "SendRequest::SendRequest() : cannot add message";
    }
#endif
}

SendRequest::~SendRequest()
{
    qDebug() << "SendRequest::~SendRequest()";
}

QStringList SendRequest::to() const
{
    QStringList result;

    foreach (const QMessageAddress &address, _message.to()) {
	result << address.addressee();
    }
    
    return result;
}

QString SendRequest::text() const
{
    return _message.textContent();
}

void SendRequest::setFinished(const QString &address, bool success)
{
    if (!success) {
	_failCount++;
	qWarning() << "SendRequest::setFinished() : sending message to" << address << "failed";
    }
    QTimer::singleShot(0, this, SLOT(down()));
}

void SendRequest::finished(Tp::PendingOperation *operation, bool processLater)
{
    QDEBUG_FUNCTION_BEGIN
    if (operation->isError()) {
	_failCount++;
	qWarning() << "SendRequest::finished() : " << operation->errorName() << ":" << operation->errorMessage(); 
    }

    if (processLater) {
	QTimer::singleShot(0, this, SLOT(down()));
    } else {
	down();
    }
    QDEBUG_FUNCTION_END
}

void SendRequest::onServiceDestroyed(QObject*)
{
    qDebug() << __PRETTY_FUNCTION__ ;
    _service = 0;
    // To be sure that instance is deleted
    deleteLater();
}

void SendRequest::down()
{
    if (--_pendingRequestCount == 0) {
        bool success = (_failCount == 0);
#ifdef TELEPATHY_ENGINE_STORE_MESSAGE
        if (success) {
            QMessagePrivate::setStandardFolder(_message, QMessage::SentFolder);
            if (!_message.id().isValid() || !QMessageManager().updateMessage(&_message)) {
                qWarning() << "SendRequest::down() : cannot update message";
            }
        }
#endif
        if (_service) {
            QMessageServicePrivate *privateService = QMessageServicePrivate::implementation(*_service);
	    privateService->setFinished(success);
	}

	deleteLater();
    }
}

void SendRequest::addMessenger(const Tp::ContactMessengerPtr &messenger)
{
    _messengerList.append(messenger);
}

int SendRequest::requestCount() const
{
    return _messengerList.count();
}
