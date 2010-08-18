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
#include <QObject>
#include <QTest>
#include <QSharedPointer>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include <QFile>
#include "qtmessaging.h"
#include "../support/support.h"

QTM_USE_NAMESPACE

const QByteArray defaultCharset("UTF-8");
const QByteArray alternateCharset("UTF-16");

template<typename T1, typename T2>
void approximateCompare(const T1 &actual, const T2 &expected, const T2 &variance, int line)
{
    if (!((expected - variance) <= actual) ||
        !(actual <= (expected + variance))) {
        qWarning() << "at line:" << line;
        qWarning() << "\tactual:" << actual;
        qWarning() << "\texpected:" << expected;
        qWarning() << "\tvariance:" << variance;
    }
    QVERIFY((expected - variance) <= actual);
    QVERIFY(actual <= (expected + variance));
}
#define QAPPROXIMATECOMPARE(a,e,v) approximateCompare(a,e,v,(__LINE__))

class SignalCatcher : public QObject
{
    Q_OBJECT
    
public:
    typedef QPair<QMessageId, QMessageManager::NotificationFilterIdSet> Notification;

    QList<Notification> added;
    QList<Notification> updated;
    QList<Notification> removed;

public slots:
    void messageAdded(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &filterIds)
    {
        added.append(qMakePair(id, filterIds));
    }

    void messageUpdated(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &filterIds)
    {
        updated.append(qMakePair(id, filterIds));
    }

    void messageRemoved(const QMessageId &id, const QMessageManager::NotificationFilterIdSet &filterIds)
    {
        removed.append(qMakePair(id, filterIds));
    }
};

class FilterRegistration
{
public:
    QMessageManager::NotificationFilterId id;
    QMessageManager *manager;
        
    FilterRegistration(const QMessageFilter &filter, QMessageManager *mgr)
        : id(0),
          manager(mgr)
    {
        id = manager->registerNotificationFilter(filter);
    }

    ~FilterRegistration()
    {
        manager->unregisterNotificationFilter(id);
    }
};

/*
    Unit test for QMessageManager class.
*/
class tst_QMessageStore : public QObject
{
    Q_OBJECT

public:
    tst_QMessageStore();
    virtual ~tst_QMessageStore();

private slots:
    void initTestCase();
    void cleanup();
    void cleanupTestCase();

    void testAccount_data();
    void testAccount();

    void testFolder_data();
    void testFolder();

    void testMessage_data();
    void testMessage();

    void testError();

private:
    QMessageManager *manager;
};

QTEST_MAIN(tst_QMessageStore)
Q_DECLARE_METATYPE(QMessageAccountId);
Q_DECLARE_METATYPE(QMessageFolderFilter);

#include "tst_smsstore.moc"

tst_QMessageStore::tst_QMessageStore()
    : manager(0)
{
}

tst_QMessageStore::~tst_QMessageStore()
{
    delete manager;
}

void tst_QMessageStore::initTestCase()
{
    manager = new QMessageManager;
    Support::clearMessageStore();
    manager->removeMessages(QMessageFilter());

}

void tst_QMessageStore::cleanup()
{
}

void tst_QMessageStore::cleanupTestCase()
{
}

void tst_QMessageStore::testAccount_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QMessageAccountId>("accountId");
    
    int n(0);
    foreach (const QMessageAccountId &id, manager->queryAccounts()) {
	QMessageAccount account(id);
	if (account.messageTypes() & QMessage::Sms) {
	    QTest::newRow(QString::number(++n).toAscii().constData()) << account.name() << account.id();
	}
    }
}

void tst_QMessageStore::testAccount()
{
    QFETCH(QString, name);
    QFETCH(QMessageAccountId, accountId);

    QMessageAccountFilter filter1 = QMessageAccountFilter::byName(name);
    
    QMessageAccountIdList ids = manager->queryAccounts(filter1);
    
    QVERIFY(ids.count() == 1);
    QCOMPARE(ids.first(), accountId);
    QCOMPARE(1, manager->countAccounts(filter1));
    
    QMessageAccountFilter filter2 = QMessageAccountFilter::byId(accountId);
    ids = manager->queryAccounts(filter2);

    QVERIFY(ids.count() == 1);
    QCOMPARE(1, manager->countAccounts(filter2));

    QMessageAccount account(ids.first());

    QCOMPARE(account.name(), name);
    QCOMPARE(account.messageTypes(), QMessage::Sms);

    QVERIFY(QMessageAccount::defaultAccount(QMessage::Sms).isValid());
}

void tst_QMessageStore::testFolder_data()
{
    QMessageAccountId accountId = QMessageAccount::defaultAccount(QMessage::Sms);

    QTest::addColumn<QMessageFolderFilter>("filter");
    QTest::addColumn<QStringList>("nameResults");

    QTest::newRow("Folder filter empty")
	<< (QMessageFolderFilter() & QMessageFolderFilter::byParentAccountId(accountId))
	<< (QStringList() << "InboxFolder" << "OutboxFolder" << "DraftsFolder" << "SentFolder" << "TrashFolder");

    QTest::newRow("Folder non-matching filter")
	<< (~QMessageFolderFilter() & QMessageFolderFilter::byParentAccountId(accountId))
	<< QStringList();

    QTest::newRow("Folder name equality")
	<< (QMessageFolderFilter::byName("SentFolder", QMessageDataComparator::Equal) & QMessageFolderFilter::byParentAccountId(accountId))
	<< (QStringList() << "SentFolder");

    QTest::newRow("Folder name inclusion")
	<< (QMessageFolderFilter::byName("box", QMessageDataComparator::Includes) & QMessageFolderFilter::byParentAccountId(accountId))
	<< (QStringList() << "InboxFolder" << "OutboxFolder");

    QTest::newRow("Folder name exclusion")
	<< (QMessageFolderFilter::byName("box", QMessageDataComparator::Excludes) & QMessageFolderFilter::byParentAccountId(accountId))
	<< (QStringList() << "DraftsFolder" << "SentFolder" << "TrashFolder");

    QTest::newRow("Folder id equality")
	<< (QMessageFolderFilter::byId(QMessageFolderId("SMS_DraftsFolder"), QMessageDataComparator::Equal) & QMessageFolderFilter::byParentAccountId(accountId))
	<< (QStringList() << "DraftsFolder");

}

void tst_QMessageStore::testFolder()
{
    QFETCH(QMessageFolderFilter, filter);
    QFETCH(QStringList, nameResults);
    
    QCOMPARE(nameResults.count(), manager->countFolders(filter));
    
    QStringList names;
    foreach (const QMessageFolderId &id, manager->queryFolders(filter)) {
	names << manager->folder(id).name();
    }
    
    QCOMPARE(nameResults.toSet(), names.toSet());
}


void tst_QMessageStore::testMessage_data()
{
    QTest::addColumn<QString>("to");
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("date");
    QTest::addColumn<int>("messageSize");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QString>("removeMessage");

    
    QTest::newRow("1") 
	<< "+358123456789"
	<< "+358987654321"
	<< "1999-12-31T23:59:59Z"
	<< 28
	<< "This is a test SMS message 1"
	<< "byId";

    QTest::newRow("2") 
	<< "+35844333666"
	<< "+35844312123"
        << "1999-11-21T23:59:59Z"
	<< 28
	<< "This is a test SMS message 2"
	<< "byFilter";
}

void tst_QMessageStore::testMessage()
{

    QMessageAccountId accountId = QMessageAccount::defaultAccount(QMessage::Sms);

    QVERIFY(accountId.isValid() == true);

    QFETCH(QString, to);
    QFETCH(QString, from);
    QFETCH(QString, date);
    QFETCH(int, messageSize);
    QFETCH(QString, text);
    QFETCH(QString, removeMessage);

    SignalCatcher catcher;
    connect(manager, SIGNAL(messageAdded(QMessageId, QMessageManager::NotificationFilterIdSet)), &catcher, SLOT(messageAdded(QMessageId, QMessageManager::NotificationFilterIdSet)));
    connect(manager, SIGNAL(messageUpdated(QMessageId, QMessageManager::NotificationFilterIdSet)), &catcher, SLOT(messageUpdated(QMessageId, QMessageManager::NotificationFilterIdSet)));

    SignalCatcher removeCatcher;
    connect(manager, SIGNAL(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)), &removeCatcher, SLOT(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)));

    QSharedPointer<FilterRegistration> filter1(new FilterRegistration(QMessageFilter::byParentAccountId(QMessageAccountId()), manager));
    QSharedPointer<FilterRegistration> filter2(new FilterRegistration(QMessageFilter::byParentAccountId(accountId), manager));
    QSharedPointer<FilterRegistration> filter3(new FilterRegistration(QMessageFilter(), manager));

    QMessage newMessage;

    newMessage.setTo(QMessageAddress(QMessageAddress::Phone, to));
    newMessage.setFrom(QMessageAddress(QMessageAddress::Phone, from));
    newMessage.setBody(text);
    newMessage.setType(QMessage::Sms);

    QDateTime dateTime(QDateTime::fromString(date, Qt::ISODate));
    dateTime.setTimeSpec(Qt::UTC);
    newMessage.setDate(dateTime);

    int originalCount = manager->countMessages();

    QVERIFY(manager->addMessage(&newMessage) == true);
    
    QMessageId messageId(newMessage.id());

    QCOMPARE(messageId.isValid(), true);
    QCOMPARE(manager->countMessages(), originalCount + 1);

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    // Wait 1 second to make sure that there is
    // enough time to get add signal
    {
        QEventLoop eventLoop;
        QTimer::singleShot(1000, &eventLoop, SLOT(quit()));
        eventLoop.exec();
    }
#endif

    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();

    QCOMPARE(catcher.added.count(), 1);
    QCOMPARE(catcher.added.first().first, messageId);
    QCOMPARE(catcher.added.first().second.count(), 2);
    QCOMPARE(catcher.added.first().second, QSet<QMessageManager::NotificationFilterId>() << filter2->id << filter3->id);

    QMessage message(messageId);
    QCOMPARE(message.id(), messageId);
    QCOMPARE(message.isModified(), false);

    QMessageAddress toAddress;
    toAddress.setType(QMessageAddress::Phone);
    toAddress.setAddressee(to);
    QVERIFY(!message.to().isEmpty());
    QCOMPARE(message.to().first(), toAddress);
    QCOMPARE(message.to().first().addressee(), to);

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    // From address is currently taken automatically from account in Maemo implementation
    QMessageAddress fromAddress;
    fromAddress.setType(QMessageAddress::Phone);
    fromAddress.setAddressee(from);
    QCOMPARE(message.from(), fromAddress);
    QCOMPARE(message.from().addressee(), from);
#endif

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    // Dates can not be stored with addMessage in Maemo implementation
    QCOMPARE(message.date(), QDateTime::fromString(date, Qt::ISODate));
#endif
    QCOMPARE(message.parentAccountId(), accountId);
    
    QMessageFolderId folderId = message.parentFolderId();
    QVERIFY(folderId.isValid() == true);
    
    QMessageFolder folder(folderId);
    QVERIFY(folderId == folder.id());
    QVERIFY(folder.parentAccountId() == accountId);

    QMessageFolderIdList fids = manager->queryFolders(QMessageFolderFilter::byName(folder.name()) & 
						      QMessageFolderFilter::byParentAccountId(folder.parentAccountId()));
    QVERIFY(!fids.isEmpty());
    QCOMPARE(fids.first(), folder.id());

#ifndef Q_OS_SYMBIAN // Created Messages are not stored in Standard Folders in Symbian & Maemo
    QCOMPARE(message.standardFolder(), QMessage::DraftsFolder);
#endif    
  
#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    // Message size calculation is not yet good enough in Maemo implementation
    QAPPROXIMATECOMPARE(message.size(), messageSize, (messageSize / 2));
#endif

    QMessageContentContainerId bodyId(message.bodyId());
    QCOMPARE(bodyId.isValid(), true);
    QCOMPARE(bodyId != QMessageContentContainerId(), true);
    QCOMPARE(QMessageContentContainerId(bodyId.toString()), bodyId);
    QVERIFY(message.contains(bodyId));

    QMessageContentContainer body(message.find(bodyId));

    //QCOMPARE(body.contentType().toLower(), bodyType.toLower());
    //QCOMPARE(body.contentSubType().toLower(), bodySubType.toLower());
    //QCOMPARE(body.contentCharset().toLower(), defaultCharset.toLower());
    QCOMPARE(body.isContentAvailable(), true);
    QCOMPARE(body.textContent(), text);
    
    int bodySize(text.size());
    QAPPROXIMATECOMPARE(body.size(), bodySize, (bodySize / 2));

    QMessageIdList messageIds(manager->queryMessages());
    QVERIFY(messageIds.contains(messageId));

    // Update the message to contain new text
    QString replacementText("This is replacement text");

    message.setBody(replacementText);
    body = message.find(bodyId);
    
    //QCOMPARE(body.contentType().toLower(), QByteArray("text"));
    //QCOMPARE(body.contentSubType().toLower(), QByteArray("html"));
    //QCOMPARE(body.contentCharset().toLower(), alternateCharset.toLower());
    QCOMPARE(body.isContentAvailable(), true);
    QCOMPARE(body.textContent(), replacementText);
    
    bodySize = replacementText.size();
    QAPPROXIMATECOMPARE(body.size(), bodySize, bodySize/2);

#if !defined(Q_WS_MAEMO_5)
    // Update does not yet work in Maemo
    QDateTime dt(QDateTime::fromString("1980-12-31T23:59:59Z", Qt::ISODate));
    dt.setTimeSpec(Qt::UTC);
    message.setDate(dt);    

    manager->updateMessage(&message);
    QCOMPARE(manager->error(), QMessageManager::NoError);

    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();


    QVERIFY(catcher.updated.count() > 0);
    QCOMPARE(catcher.updated.first().first, messageId);
    QCOMPARE(catcher.updated.first().second.count(), 2);
    QCOMPARE(catcher.updated.first().second, QSet<QMessageManager::NotificationFilterId>() << filter2->id << filter3->id);

    QMessage updated(message.id());

    QCOMPARE(updated.id(), message.id());
    QCOMPARE(updated.isModified(), false);

    QCOMPARE(QMessage(message).id(), message.id());
    QVERIFY(!(message.id() < message.id()));
    QVERIFY((QMessageId() < message.id()) || (message.id() < QMessageId()));
    //    QCOMPARE(updated.date(), dt);

    bodyId = updated.bodyId();
    QCOMPARE(bodyId.isValid(), true);
    QCOMPARE(bodyId != QMessageContentContainerId(), true);
    QCOMPARE(QMessageContentContainerId(bodyId.toString()), bodyId);
    QVERIFY(updated.contains(bodyId));

    body = updated.find(bodyId);

    QCOMPARE(body.isContentAvailable(), true);

    QCOMPARE(body.textContent(), replacementText);
    //    QAPPROXIMATECOMPARE(body.size(), 72, 36);
#endif

    // Test message removal
    if (removeMessage == "byId") {
        manager->removeMessage(message.id());
    } else { // byFilter
        manager->removeMessages(QMessageFilter::byId(message.id()));
    }
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    // Wait 1 second to make sure that there is
    // enough time to get removed signal
    {
        QEventLoop eventLoop;
        QTimer::singleShot(1000, &eventLoop, SLOT(quit()));
        eventLoop.exec();
    }
#endif
    QCOMPARE(manager->error(), QMessageManager::NoError);
    QCOMPARE(manager->countMessages(), originalCount);

    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();

#if !defined(Q_OS_SYMBIAN)
    QCOMPARE(removeCatcher.removed.count(), 1);
    QCOMPARE(removeCatcher.removed.first().first, messageId);
#if defined(Q_WS_MAEMO_5)
    QCOMPARE(removeCatcher.removed.first().second.count(), 2);
    QCOMPARE(removeCatcher.removed.first().second, QSet<QMessageManager::NotificationFilterId>() << filter2->id << filter3->id);
#else
    QCOMPARE(removeCatcher.removed.first().second.count(), 1);
    QCOMPARE(removeCatcher.removed.first().second, QSet<QMessageManager::NotificationFilterId>() << filter3->id);
#endif
#endif

}

void tst_QMessageStore::testError()
{

    QMessageFilter filter;
    QMessageAccountFilter filter2;
    QMessageFolderFilter filter3;
	
    (void)manager->queryMessages(filter);
    QVERIFY(manager->error() == QMessageManager::NoError);
    QMessageAccount account = manager->account(QMessageAccountId());
    QCOMPARE(account.id().isValid(), false);
    QCOMPARE(QMessageManager::InvalidId, manager->error());
    
    (void)manager->queryMessages(filter, "NotFound");
    QVERIFY(manager->error() == QMessageManager::NoError);
    QMessageFolder folder = manager->folder(QMessageFolderId());
    QCOMPARE(folder.id().isValid(), false);
    QCOMPARE(QMessageManager::InvalidId, manager->error());

    (void)manager->countMessages(filter);
    QVERIFY(manager->error() == QMessageManager::NoError);
    QMessage message = manager->message(QMessageId());
    QCOMPARE(message.id().isValid(), false);
    QCOMPARE(QMessageManager::InvalidId, manager->error());

    (void)manager->countAccounts(filter2);
    QVERIFY(manager->error() == QMessageManager::NoError);    
    QCOMPARE(manager->removeMessage(QMessageId()), false);
    QCOMPARE(QMessageManager::InvalidId, manager->error());

    (void)manager->queryAccounts(filter2);
    QVERIFY(manager->error() == QMessageManager::NoError);    
    QMessage msg;
    QCOMPARE(manager->addMessage(&msg), false);
    QCOMPARE(manager->error() != QMessageManager::NoError, true);    

    (void)manager->countFolders(filter3);
    QVERIFY(manager->error() == QMessageManager::NoError);    
    QMessage msg2;
    QCOMPARE(manager->updateMessage(&msg2), false);
    QCOMPARE(manager->error() != QMessageManager::NoError, true);    
    
    (void)manager->queryFolders(filter3);
    QVERIFY(manager->error() == QMessageManager::NoError);    
}

