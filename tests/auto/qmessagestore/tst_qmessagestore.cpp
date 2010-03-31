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

#if (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) && defined(_WIN32_WCE))
# if defined(TESTDATA_DIR)
#  undef TESTDATA_DIR
# endif
# define TESTDATA_DIR "."
#endif

//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE
#if defined(Q_OS_WIN)
const QByteArray defaultCharset("UTF-16");
const QByteArray alternateCharset("UTF-8");
#else
const QByteArray defaultCharset("UTF-8");
const QByteArray alternateCharset("UTF-16");
#endif

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

private:
    QMessageManager *manager;
};

QTEST_MAIN(tst_QMessageStore)

#include "tst_qmessagestore.moc"

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
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    if (!Support::mapiAvailable())
        QSKIP("Skipping tests because a MAPI subsystem does not appear to be available", SkipAll);
#endif

    Support::clearMessageStore();

    manager = new QMessageManager;
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
    QTest::addColumn<QString>("fromAddress");

    QTest::newRow("1") << "Test Account #1" << "tester1@example.com";
    QTest::newRow("2") << "Test Account #2" << "tester2@example.com";
}

void tst_QMessageStore::testAccount()
{
    QFETCH(QString, name);
    QFETCH(QString, fromAddress);

    Support::Parameters p;
    p.insert("name", name);
    p.insert("fromAddress", fromAddress);

    int originalCount = manager->countAccounts();

    QMessageAccountId accountId(Support::addAccount(p));
    QVERIFY(accountId.isValid());
    QVERIFY(accountId != QMessageAccountId());
    QCOMPARE(manager->countAccounts(), originalCount + 1);
    
    QMessageAccount account(accountId);
    QCOMPARE(account.id(), accountId);
    QCOMPARE(account.name(), name);
    QCOMPARE(account.messageTypes(), QMessage::Email);

    QCOMPARE(QMessageAccount(account).id(), accountId);
    QVERIFY(!(accountId < accountId));
    QVERIFY((QMessageAccountId() < accountId) || (accountId < QMessageAccountId()));

    QMessageAccountIdList accountIds(manager->queryAccounts());
    QVERIFY(accountIds.contains(accountId));

    QVERIFY(QMessageAccount::defaultAccount(QMessage::Email).isValid());
}

void tst_QMessageStore::testFolder_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("parentFolderPath");
    QTest::addColumn<QString>("nameResult");

	// Note: on Win CE, we can't use 'Inbox' 'Drafts' etc., becuase they're added automatically by the system
    QTest::newRow("Inbox") << "Unbox" << "Unbox" << "" << "Unbox";
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    // Symbian does not currently support paths
    QTest::newRow("Drafts") << "Crafts" << "" << "" << "Crafts";
    QTest::newRow("Archived") << "Unbox/Archived" << "Archived" << "Unbox" << "Archived";
    QTest::newRow("Backup") << "Unbox/Archived/Backup" << "Backup" << "Unbox/Archived" << "Backup";
#endif    
}

void tst_QMessageStore::testFolder()
{
    // Ensure we have an account to link these folders to
    static const QString testAccountName("testAccount");
    QMessageAccountId testAccountId;
    QMessageAccountIdList accountIds(manager->queryAccounts(QMessageAccountFilter::byName(testAccountName)));
    if (accountIds.isEmpty()) {
        Support::Parameters p;
        p.insert("name", testAccountName);
        p.insert("fromAddress", "someone@example.com");
        testAccountId = Support::addAccount(p);
    } else {
        testAccountId = accountIds.first();
    }
    QVERIFY(testAccountId.isValid());

    QFETCH(QString, path);
    QFETCH(QString, name);
    QFETCH(QString, parentFolderPath);
    QFETCH(QString, nameResult);

    Support::Parameters p;
    p.insert("path", path);
    p.insert("name", name);
    p.insert("parentAccountName", testAccountName);
    p.insert("parentFolderPath", parentFolderPath);

#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    int originalCount = manager->countFolders(QMessageFolderFilter::byParentAccountId(testAccountId));
#else
    int originalCount = manager->countFolders();
#endif    

    QMessageFolderId folderId(Support::addFolder(p));
    QVERIFY(folderId.isValid());
    QVERIFY(folderId != QMessageFolderId());
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    QCOMPARE(manager->countFolders(QMessageFolderFilter::byParentAccountId(testAccountId)), originalCount + 1);
#else
    QCOMPARE(manager->countFolders(), originalCount + 1);
#endif    
    
    QMessageFolder folder(folderId);
    QCOMPARE(folder.id(), folderId);
    QCOMPARE(folder.path(), path);
    QCOMPARE(folder.name(), nameResult);
    QCOMPARE(folder.parentAccountId(), testAccountId);

    QCOMPARE(QMessageFolder(folder).id(), folderId);
    QVERIFY(!(folderId < folderId));
    QVERIFY((QMessageFolderId() < folderId) || (folderId < QMessageFolderId()));

    if (!parentFolderPath.isEmpty()) {
        QMessageFolderFilter filter(QMessageFolderFilter::byPath(parentFolderPath) & QMessageFolderFilter::byParentAccountId(testAccountId));
        QMessageFolderIdList list(manager->queryFolders(filter));
        QMessageFolderId parentFolderId(list.first());
        QCOMPARE(folder.parentFolderId(), parentFolderId);
    }

    QMessageFolderIdList folderIds(manager->queryFolders());
    QVERIFY(folderIds.contains(folderId));
}

Q_DECLARE_METATYPE(QList<QByteArray>)
Q_DECLARE_METATYPE(QList<int>)

typedef QMap<QString, QString> CustomFieldMap;
Q_DECLARE_METATYPE(CustomFieldMap)

void tst_QMessageStore::testMessage_data()
{
    QTest::addColumn<QString>("to");
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("cc");
    QTest::addColumn<QString>("date");
    QTest::addColumn<QString>("subject");
    QTest::addColumn<QByteArray>("messageType");
    QTest::addColumn<QByteArray>("messageSubType");
    QTest::addColumn<int>("messageSize");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QByteArray>("bodyType");
    QTest::addColumn<QByteArray>("bodySubType");
    QTest::addColumn<int>("bodySize");
    QTest::addColumn<QList<QByteArray> >("attachments");
    QTest::addColumn<QList<QByteArray> >("attachmentType");
    QTest::addColumn<QList<QByteArray> >("attachmentSubType");
    QTest::addColumn<QList<int> >("attachmentSize");
    QTest::addColumn<CustomFieldMap>("custom");
    QTest::addColumn<QString>("removeMessage");

    CustomFieldMap customData;
    customData.insert("cake", "chocolate");
    customData.insert("muffin", "blueberry");

    QTest::newRow("1")
        << "alice@example.com"
        << "bob@example.com"
        << ""
        << "1999-12-31T23:59:59Z"
        << "Last message..."
        << QByteArray("text")
        << QByteArray("plain")
#if defined(Q_OS_SYMBIAN)
        << 89
#else
#if defined(Q_OS_WIN) && defined(_WIN32_WCE)
        << 32
#else
        << 1400
#endif
#endif        
        << "...before Y2K"
        << QByteArray("text")
        << QByteArray("plain")
        << 24
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<int>()
        << customData
        << "byId";

    QTest::newRow("2")
        << "alice@example.com"
        << "bob@example.com"
        << "charlie@example.com, donald@example.com"
        << "1999-12-31T23:59:59Z"
        << "Last HTML message..."
        << QByteArray("text")
        << QByteArray("html")
#if defined(Q_OS_SYMBIAN)
        << 157
#else
#if defined(Q_OS_WIN) && defined(_WIN32_WCE)
        << 64
#else
        << 1536
#endif
#endif
        << "<html><p>...before <b>Y2K</b></p></html>"
        << QByteArray("text")
        << QByteArray("html")
        << 64
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<int>()
        << customData
        << "byFilter";

    QTest::newRow("3")
        << "alice@example.com"
        << "bob@example.com"
        << ""
        << "1999-12-31T23:59:59Z"
        << "Last message..."
        << QByteArray("multipart")
        << QByteArray("mixed")
#if defined(Q_OS_SYMBIAN)
        << 611
#else
#if defined(Q_OS_WIN) && defined(_WIN32_WCE)
        << 512
#else
        << 1536
#endif
#endif        
        << "...before Y2K"
        << QByteArray("text")
        << QByteArray("plain")
        << 24
        << ( QList<QByteArray>() << "1.txt" )
        << ( QList<QByteArray>() << "text" )
        << ( QList<QByteArray>() << "plain" )
        << ( QList<int>() << 512 )
        << customData
        << "byId";

    QTest::newRow("4")
        << "alice@example.com"
        << "bob@example.com"
        << ""
        << "1999-12-31T23:59:59Z"
        << "Last HTML message..."
        << QByteArray("multipart")
        << QByteArray("mixed")
#if defined(Q_OS_SYMBIAN)
        << 4731
#else
#if defined(Q_OS_WIN) && !defined(_WIN32_WCE)
        << 5120
#else
        << 4096
#endif
#endif
        << "<html><p>...before <b>Y2K</b></p></html>"
        << QByteArray("text")
        << QByteArray("html")
        << 64
        << ( QList<QByteArray>() << "1.txt" << "qtlogo.png" )
        << ( QList<QByteArray>() << "text" << "image" )
        << ( QList<QByteArray>() << "plain" << "png" )
        << ( QList<int>() << 512 << 4096 )
        << customData
        << "byFilter";
}

void tst_QMessageStore::testMessage()
{
    // Ensure we have an account to create messages with
    static const QString testAccountName("testAccount");

    QMessageAccountId testAccountId;
    QMessageAccountIdList accountIds(manager->queryAccounts(QMessageAccountFilter::byName(testAccountName)));
    if (accountIds.isEmpty()) {
        Support::Parameters p;
        p.insert("name", testAccountName);
        testAccountId = Support::addAccount(p);
    } else {
        testAccountId = accountIds.first();
        QMessageAccount acc(testAccountId);
    }
    QVERIFY(testAccountId.isValid());

    QMessageFolderId testFolderId;
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QMessageFolderFilter filter(QMessageFolderFilter::byName("Inbox") & QMessageFolderFilter::byParentAccountId(testAccountId));
#else
    // Created Messages can not be stored into "Inbox" folder in Symbian & Meamo
    QMessageFolderFilter filter(QMessageFolderFilter::byName("Unbox") & QMessageFolderFilter::byParentAccountId(testAccountId));
#endif
    QMessageFolderIdList folderIds(manager->queryFolders(filter));
    if (folderIds.isEmpty()) {
        Support::Parameters p;
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
        p.insert("path", "Inbox");
        p.insert("name", "Inbox");
#else
        // Created Messages can not be stored into "Inbox" folder in Symbian & Maemo
        p.insert("path", "Unbox");
        p.insert("name", "Unbox");
#endif
        p.insert("parentAccountName", testAccountName);
        testFolderId = Support::addFolder(p);
    } else {
        testFolderId = folderIds.first();
    }
    QVERIFY(testFolderId.isValid());

    QMessageFolder testFolder(testFolderId);

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    // Wait 1/100 second to make sure that there is
    // enough time to start monitoring new folder
    {
        QEventLoop eventLoop;
        QTimer::singleShot(100, &eventLoop, SLOT(quit()));
        eventLoop.exec();
    }
    // Note: QTest::qSleep(100); can not be used
    //       because QTest::qSleep(100); blocks
    //       signaling from other threads
#endif

    SignalCatcher catcher;
    connect(manager, SIGNAL(messageAdded(QMessageId, QMessageManager::NotificationFilterIdSet)), &catcher, SLOT(messageAdded(QMessageId, QMessageManager::NotificationFilterIdSet)));
    connect(manager, SIGNAL(messageUpdated(QMessageId, QMessageManager::NotificationFilterIdSet)), &catcher, SLOT(messageUpdated(QMessageId, QMessageManager::NotificationFilterIdSet)));

    SignalCatcher removeCatcher;
    connect(manager, SIGNAL(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)), &removeCatcher, SLOT(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)));

    QSharedPointer<FilterRegistration> filter1(new FilterRegistration(QMessageFilter::byParentAccountId(QMessageAccountId()), manager));
    QSharedPointer<FilterRegistration> filter2(new FilterRegistration(QMessageFilter::byParentAccountId(testAccountId), manager));
    QSharedPointer<FilterRegistration> filter3(new FilterRegistration(QMessageFilter(), manager));

    QFETCH(QString, to);
    QFETCH(QString, from);
    QFETCH(QString, cc);
    QFETCH(QString, date);
    QFETCH(QString, subject);
    QFETCH(QByteArray, messageType);
    QFETCH(QByteArray, messageSubType);
    QFETCH(int, messageSize);
    QFETCH(QString, text);
    QFETCH(QByteArray, bodyType);
    QFETCH(QByteArray, bodySubType);
    QFETCH(int, bodySize);
    QFETCH(QList<QByteArray>, attachments);
    QFETCH(QList<QByteArray>, attachmentType);
    QFETCH(QList<QByteArray>, attachmentSubType);
    QFETCH(QList<int>, attachmentSize);
    QFETCH(CustomFieldMap, custom);
    QFETCH(QString, removeMessage);

    Support::Parameters p;
    p.insert("to", to);
    p.insert("from", from);
    p.insert("cc", cc);
    p.insert("date", date);
    p.insert("subject", subject);
    p.insert("mimeType", bodyType + '/' + bodySubType);
    p.insert("text", text);
    p.insert("parentAccountName", testAccountName);
    p.insert("parentFolderPath", testFolder.path());

    if (!attachments.isEmpty()) {
        QStringList attachmentPaths;
        foreach (const QString &fileName, attachments) {
            attachmentPaths.append(TESTDATA_DIR "/testdata/" + fileName);
        }
        p.insert("attachments", attachmentPaths.join("\n"));
        p.insert("status-hasAttachments", "true");
    }

    int originalCount = manager->countMessages();

    // Test message addition
    QMessageId messageId(Support::addMessage(p));
    QVERIFY(messageId.isValid());
    QVERIFY(messageId != QMessageId());
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

#if defined(Q_OS_WIN)
	// Give MAPI enough time to emit the message added notification
	QTest::qSleep(1000);
#endif
    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();

    QCOMPARE(catcher.added.count(), 1);
    QCOMPARE(catcher.added.first().first, messageId);
    QCOMPARE(catcher.added.first().second.count(), 2);
    QCOMPARE(catcher.added.first().second, QSet<QMessageManager::NotificationFilterId>() << filter2->id << filter3->id);

    // Test message retrieval
    QMessage message(messageId);
    QCOMPARE(message.id(), messageId);
    QCOMPARE(message.isModified(), false);

    QMessageAddress toAddress;
    toAddress.setType(QMessageAddress::Email);
    toAddress.setAddressee(to);
    QVERIFY(!message.to().isEmpty());
    QCOMPARE(message.to().first(), toAddress);
    QCOMPARE(message.to().first().addressee(), to);

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    // From address is currently taken automatically from account in Maemo implementation
    QMessageAddress fromAddress;
    fromAddress.setType(QMessageAddress::Email);
    fromAddress.setAddressee(from);
    QCOMPARE(message.from(), fromAddress);
    QCOMPARE(message.from().addressee(), from);
#endif
    QList<QMessageAddress> ccAddresses;
    foreach (const QString &element, cc.split(",", QString::SkipEmptyParts)) {
        QMessageAddress addr;
        addr.setType(QMessageAddress::Email);
        addr.setAddressee(element.trimmed());
        ccAddresses.append(addr);
    }

    QCOMPARE(message.cc(), ccAddresses);

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    // Dates can not be stored with addMessage in Maemo implementation
    QCOMPARE(message.date(), QDateTime::fromString(date, Qt::ISODate));
#endif
    QCOMPARE(message.subject(), subject);

    QCOMPARE(message.contentType().toLower(), messageType.toLower());
    QCOMPARE(message.contentSubType().toLower(), messageSubType.toLower());

    if (message.contentType().toLower() == "multipart") {
        QVERIFY(!message.contentIds().isEmpty());
    } else {
        QVERIFY(message.contentIds().isEmpty());
    }

    QCOMPARE(message.parentAccountId(), testAccountId);
    QCOMPARE(message.parentFolderId(), testFolderId);
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

    QCOMPARE(body.contentType().toLower(), bodyType.toLower());
    QCOMPARE(body.contentSubType().toLower(), bodySubType.toLower());
    QCOMPARE(body.contentCharset().toLower(), defaultCharset.toLower());
    QCOMPARE(body.isContentAvailable(), true);
    QCOMPARE(body.textContent(), text);
    QAPPROXIMATECOMPARE(body.size(), bodySize, (bodySize / 2));

    QMessageContentContainerIdList attachmentIds(message.attachmentIds());
    QCOMPARE(attachmentIds.count(), attachments.count());

    for (int i = 0; i < attachments.count(); ++i) {
        QMessageContentContainerId attachmentId(attachmentIds.at(i));
        QCOMPARE(attachmentId.isValid(), true);
        QCOMPARE(attachmentId != QMessageContentContainerId(), true);
        QCOMPARE(QMessageContentContainerId(attachmentId.toString()), attachmentId);
        QVERIFY(message.contains(attachmentId));

        // With MAPI, attachment order is not preserved
        QMessageContentContainer attachment(message.find(attachmentId));
        int index = attachments.indexOf(attachment.suggestedFileName());
        QVERIFY(index != -1);

        // We cannot create nested multipart messages
        QVERIFY(attachment.contentIds().isEmpty());

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
        // Check attachment content
        QByteArray attachmentContent = attachment.content();
        QString fileName = QString(TESTDATA_DIR) + QString("/testdata/") + attachments[index];
        QFile attachmentFile(fileName);
        if (attachmentFile.open(QIODevice::ReadOnly)) {
            QByteArray originalAttachmentContent = attachmentFile.readAll();
            QCOMPARE(attachmentContent, originalAttachmentContent);
        }
#endif

        QCOMPARE(attachment.contentType().toLower(), attachmentType[index].toLower());
        QCOMPARE(attachment.contentSubType().toLower(), attachmentSubType[index].toLower());
        QCOMPARE(attachment.suggestedFileName(), attachments[index]);
        QAPPROXIMATECOMPARE(attachment.size(), attachmentSize[index], (attachmentSize[index] / 2));
    }

    QMessageIdList messageIds(manager->queryMessages());
    QVERIFY(messageIds.contains(messageId));

    // Update the message to contain new text
    QString replacementText("<html>This is replacement text.</html>");

    message.setBody(replacementText, "text/html; charset=" + alternateCharset);
    body = message.find(bodyId);
    
    QCOMPARE(body.contentType().toLower(), QByteArray("text"));
    QCOMPARE(body.contentSubType().toLower(), QByteArray("html"));
    QCOMPARE(body.contentCharset().toLower(), alternateCharset.toLower());
    QCOMPARE(body.isContentAvailable(), true);
    QCOMPARE(body.textContent(), replacementText);
    QAPPROXIMATECOMPARE(body.size(), 72, 36);
    
#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    // Update does not yet work in Maemo
    QDateTime dt(QDateTime::fromString("1980-12-31T23:59:59Z", Qt::ISODate));
    dt.setTimeSpec(Qt::UTC);
    message.setDate(dt);    

    manager->updateMessage(&message);
    QCOMPARE(manager->error(), QMessageManager::NoError);

#if defined(Q_OS_WIN)
	QTest::qSleep(1000);
#endif
    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();

    // MAPI generates multiple update notifications per message updated
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
    QCOMPARE(updated.date(), dt);

    bodyId = updated.bodyId();
    QCOMPARE(bodyId.isValid(), true);
    QCOMPARE(bodyId != QMessageContentContainerId(), true);
    QCOMPARE(QMessageContentContainerId(bodyId.toString()), bodyId);
    QVERIFY(updated.contains(bodyId));

    body = updated.find(bodyId);
  
    QCOMPARE(body.contentType().toLower(), QByteArray("text"));
    QCOMPARE(body.contentSubType().toLower(), QByteArray("html"));
#if !defined(Q_OS_WIN)
    // Original charset is not preserved on windows
    QCOMPARE(body.contentCharset().toLower(), alternateCharset.toLower());
#endif
    QCOMPARE(body.isContentAvailable(), true);
    QCOMPARE(body.textContent(), replacementText);
    QAPPROXIMATECOMPARE(body.size(), 72, 36);

    // Test response message properties
    QMessage reply(updated.createResponseMessage(QMessage::ReplyToSender));
    QCOMPARE(reply.subject(), updated.subject().prepend("Re:"));
    QCOMPARE(reply.to(), QList<QMessageAddress>() << updated.from());
    QCOMPARE(reply.cc(), QList<QMessageAddress>());
    QVERIFY(reply.bodyId().isValid());

    QMessage replyToAll(updated.createResponseMessage(QMessage::ReplyToAll));
    QCOMPARE(replyToAll.subject(), updated.subject().prepend("Re:"));  
    QCOMPARE(replyToAll.to(), QList<QMessageAddress>() << updated.from());
    QCOMPARE(replyToAll.cc(), QList<QMessageAddress>() << updated.to() << updated.cc());
    QVERIFY(replyToAll.bodyId().isValid());   

    QMessage forward(updated.createResponseMessage(QMessage::Forward));
    QCOMPARE(forward.subject(), updated.subject().prepend("Fwd:"));
    QVERIFY(forward.bodyId().isValid());

    // Verify that the attachments can be removed
    updated.clearAttachments();
    QVERIFY(updated.attachmentIds().isEmpty());
#endif

    // Test message removal
    if (removeMessage == "byId") {
        manager->removeMessage(message.id());
    } else { // byFilter
        manager->removeMessages(QMessageFilter::byId(message.id()));
    }
    QCOMPARE(manager->error(), QMessageManager::NoError);
    QCOMPARE(manager->countMessages(), originalCount);

#if defined(Q_OS_WIN)
	QTest::qSleep(1000);
#endif
    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();

#if !defined(Q_OS_SYMBIAN)
    QCOMPARE(removeCatcher.removed.count(), 1);
    QCOMPARE(removeCatcher.removed.first().first, messageId);
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    QCOMPARE(removeCatcher.removed.first().second.count(), 2);
    QCOMPARE(removeCatcher.removed.first().second, QSet<QMessageManager::NotificationFilterId>() << filter2->id << filter3->id);
#else
    QCOMPARE(removeCatcher.removed.first().second.count(), 1);
    QCOMPARE(removeCatcher.removed.first().second, QSet<QMessageManager::NotificationFilterId>() << filter3->id);
#endif
#endif    
}

