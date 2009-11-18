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
    typedef QPair<QMessageId, QMessageStore::NotificationFilterIdSet> Notification;

    QList<Notification> added;
    QList<Notification> updated;
    QList<Notification> removed;

public slots:
    void messageAdded(const QMessageId &id, const QMessageStore::NotificationFilterIdSet &filterIds)
    {
        added.append(qMakePair(id, filterIds));
    }

    void messageUpdated(const QMessageId &id, const QMessageStore::NotificationFilterIdSet &filterIds)
    {
        updated.append(qMakePair(id, filterIds));
    }

    void messageRemoved(const QMessageId &id, const QMessageStore::NotificationFilterIdSet &filterIds)
    {
        removed.append(qMakePair(id, filterIds));
    }
};

class FilterRegistration
{
public:
    QMessageStore::NotificationFilterId id;
        
    FilterRegistration(const QMessageFilter &filter)
        : id(0)
    {
        id = QMessageStore::instance()->registerNotificationFilter(filter);
    }

    ~FilterRegistration()
    {
        QMessageStore::instance()->unregisterNotificationFilter(id);
    }
};

/*
    Unit test for QMessageStore class.
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
};

QTEST_MAIN(tst_QMessageStore)

#include "tst_qmessagestore.moc"

tst_QMessageStore::tst_QMessageStore()
{
}

tst_QMessageStore::~tst_QMessageStore()
{
}

void tst_QMessageStore::initTestCase()
{
    Support::clearMessageStore();
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

    int originalCount = QMessageStore::instance()->countAccounts();

    QMessageAccountId accountId(Support::addAccount(p));
    QVERIFY(accountId.isValid());
    QVERIFY(accountId != QMessageAccountId());
    QCOMPARE(QMessageStore::instance()->countAccounts(), originalCount + 1);
    
    QMessageAccount account(accountId);
    QCOMPARE(account.id(), accountId);
    QCOMPARE(account.name(), name);
    QCOMPARE(account.messageTypes(), QMessage::Email);

    QMessageAccountIdList accountIds(QMessageStore::instance()->queryAccounts());
    QVERIFY(accountIds.contains(accountId));

    QVERIFY(QMessageAccount::defaultAccount(QMessage::Email).isValid());
}

void tst_QMessageStore::testFolder_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("displayName");
    QTest::addColumn<QString>("parentFolderPath");
    QTest::addColumn<QString>("displayNameResult");

	// Note: on Win CE, we can't use 'Inbox' 'Drafts' etc., becuase they're added automatically by the system
    QTest::newRow("Inbox") << "Unbox" << "Unbox" << "" << "Unbox";
    QTest::newRow("Drafts") << "Crafts" << "" << "" << "Crafts";
    QTest::newRow("Archived") << "Unbox/Archived" << "Archived" << "Unbox" << "Archived";
    QTest::newRow("Backup") << "Unbox/Archived/Backup" << "Backup" << "Unbox/Archived" << "Backup";
}

void tst_QMessageStore::testFolder()
{
    // Ensure we have an account to link these folders to
    static const QString testAccountName("testAccount");
    QMessageAccountId testAccountId;
    QMessageAccountIdList accountIds(QMessageStore::instance()->queryAccounts(QMessageAccountFilter::byName(testAccountName)));
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
    QFETCH(QString, displayName);
    QFETCH(QString, parentFolderPath);
    QFETCH(QString, displayNameResult);

    Support::Parameters p;
    p.insert("path", path);
    p.insert("displayName", displayName);
    p.insert("parentAccountName", testAccountName);
    p.insert("parentFolderPath", parentFolderPath);

    int originalCount = QMessageStore::instance()->countFolders();

    QMessageFolderId folderId(Support::addFolder(p));
    QVERIFY(folderId.isValid());
    QVERIFY(folderId != QMessageFolderId());
    QCOMPARE(QMessageStore::instance()->countFolders(), originalCount + 1);
    
    QMessageFolder folder(folderId);
    QCOMPARE(folder.id(), folderId);
    QCOMPARE(folder.path(), path);
    QCOMPARE(folder.displayName(), displayNameResult);
    QCOMPARE(folder.parentAccountId(), testAccountId);

    if (!parentFolderPath.isEmpty()) {
        QMessageFolderFilter filter(QMessageFolderFilter::byPath(parentFolderPath) & QMessageFolderFilter::byParentAccountId(testAccountId));
        QMessageFolderIdList list(QMessageStore::instance()->queryFolders(filter));
        QMessageFolderId parentFolderId(list.first());
        QCOMPARE(folder.parentFolderId(), parentFolderId);
    }

    QMessageFolderIdList folderIds(QMessageStore::instance()->queryFolders());
    QVERIFY(folderIds.contains(folderId));
}

Q_DECLARE_METATYPE(QList<QByteArray>)
Q_DECLARE_METATYPE(QList<unsigned>)

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
    QTest::addColumn<unsigned>("messageSize");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QByteArray>("bodyType");
    QTest::addColumn<QByteArray>("bodySubType");
    QTest::addColumn<unsigned>("bodySize");
    QTest::addColumn<QList<QByteArray> >("attachments");
    QTest::addColumn<QList<QByteArray> >("attachmentType");
    QTest::addColumn<QList<QByteArray> >("attachmentSubType");
    QTest::addColumn<QList<unsigned> >("attachmentSize");
    QTest::addColumn<CustomFieldMap>("custom");

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
#if defined(Q_OS_WIN)
        << 32u
#else
        << 1536u
#endif
        << "...before Y2K"
        << QByteArray("text")
        << QByteArray("plain")
        << 24u
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<unsigned>()
        << customData;

    QTest::newRow("2")
        << "alice@example.com"
        << "bob@example.com"
        << "charlie@example.com, donald@example.com"
        << "1999-12-31T23:59:59Z"
        << "Last HTML message..."
        << QByteArray("text")
        << QByteArray("html")
#if defined(Q_OS_WIN)
        << 64u
#else
        << 1536u
#endif
        << "<html><p>...before <b>Y2K</b></p></html>"
        << QByteArray("text")
        << QByteArray("html")
        << 64u
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<unsigned>()
        << customData;

    QTest::newRow("3")
        << "alice@example.com"
        << "bob@example.com"
        << ""
        << "1999-12-31T23:59:59Z"
        << "Last message..."
        << QByteArray("multipart")
        << QByteArray("mixed")
#if defined(Q_OS_WIN)
        << 512u
#else
        << 1536u
#endif
        << "...before Y2K"
        << QByteArray("text")
        << QByteArray("plain")
        << 24u
        << ( QList<QByteArray>() << "1.txt" )
        << ( QList<QByteArray>() << "text" )
        << ( QList<QByteArray>() << "plain" )
        << ( QList<unsigned>() << 512u )
        << customData;

    QTest::newRow("4")
        << "alice@example.com"
        << "bob@example.com"
        << ""
        << "1999-12-31T23:59:59Z"
        << "Last HTML message..."
        << QByteArray("multipart")
        << QByteArray("mixed")
#if defined(Q_OS_WIN) && !defined(_WIN32_WCE)
        << 5120u
#else
        << 4096u
#endif
        << "<html><p>...before <b>Y2K</b></p></html>"
        << QByteArray("text")
        << QByteArray("html")
        << 64u
        << ( QList<QByteArray>() << "1.txt" << "qtlogo.png" )
        << ( QList<QByteArray>() << "text" << "image" )
        << ( QList<QByteArray>() << "plain" << "png" )
        << ( QList<unsigned>() << 512u << 4096u )
        << customData;
}

void tst_QMessageStore::testMessage()
{
    // Ensure we have an account to create messages with
    static const QString testAccountName("testAccount");

    QMessageAccountId testAccountId;
    QMessageAccountIdList accountIds(QMessageStore::instance()->queryAccounts(QMessageAccountFilter::byName(testAccountName)));
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
    QMessageFolderFilter filter(QMessageFolderFilter::byDisplayName("Inbox") & QMessageFolderFilter::byParentAccountId(testAccountId));
    QMessageFolderIdList folderIds(QMessageStore::instance()->queryFolders(filter));
    if (folderIds.isEmpty()) {
        Support::Parameters p;
        p.insert("path", "Inbox");
        p.insert("displayName", "Inbox");
        p.insert("parentAccountName", testAccountName);
        testFolderId = Support::addFolder(p);
    } else {
        testFolderId = folderIds.first();
    }
    QVERIFY(testFolderId.isValid());

    QMessageFolder testFolder(testFolderId);

    SignalCatcher catcher;
    connect(QMessageStore::instance(), SIGNAL(messageAdded(QMessageId, QMessageStore::NotificationFilterIdSet)), &catcher, SLOT(messageAdded(QMessageId, QMessageStore::NotificationFilterIdSet)));
    connect(QMessageStore::instance(), SIGNAL(messageUpdated(QMessageId, QMessageStore::NotificationFilterIdSet)), &catcher, SLOT(messageUpdated(QMessageId, QMessageStore::NotificationFilterIdSet)));

    SignalCatcher removeCatcher;
    connect(QMessageStore::instance(), SIGNAL(messageRemoved(QMessageId, QMessageStore::NotificationFilterIdSet)), &removeCatcher, SLOT(messageRemoved(QMessageId, QMessageStore::NotificationFilterIdSet)));

    QSharedPointer<FilterRegistration> filter1(new FilterRegistration(QMessageFilter::byParentAccountId(QMessageAccountId())));
    QSharedPointer<FilterRegistration> filter2(new FilterRegistration(QMessageFilter::byParentAccountId(testAccountId)));
    QSharedPointer<FilterRegistration> filter3(new FilterRegistration(QMessageFilter()));

    QFETCH(QString, to);
    QFETCH(QString, from);
    QFETCH(QString, cc);
    QFETCH(QString, date);
    QFETCH(QString, subject);
    QFETCH(QByteArray, messageType);
    QFETCH(QByteArray, messageSubType);
    QFETCH(unsigned, messageSize);
    QFETCH(QString, text);
    QFETCH(QByteArray, bodyType);
    QFETCH(QByteArray, bodySubType);
    QFETCH(unsigned, bodySize);
    QFETCH(QList<QByteArray>, attachments);
    QFETCH(QList<QByteArray>, attachmentType);
    QFETCH(QList<QByteArray>, attachmentSubType);
    QFETCH(QList<unsigned>, attachmentSize);
    QFETCH(CustomFieldMap, custom);

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

    int originalCount = QMessageStore::instance()->countMessages();

    // Test message addition
    QMessageId messageId(Support::addMessage(p));
    QVERIFY(messageId.isValid());
    QVERIFY(messageId != QMessageId());
    QCOMPARE(QMessageStore::instance()->countMessages(), originalCount + 1);

#if defined(Q_OS_WIN)
	// Give MAPI enough time to emit the message added notification
	QTest::qSleep(1000);
#endif
    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();

    QCOMPARE(catcher.added.count(), 1);
    QCOMPARE(catcher.added.first().first, messageId);
    QCOMPARE(catcher.added.first().second.count(), 2);
    QCOMPARE(catcher.added.first().second, QSet<QMessageStore::NotificationFilterId>() << filter2->id << filter3->id);

    // Test message retrieval
    QMessage message(messageId);
    QCOMPARE(message.id(), messageId);
    QCOMPARE(message.isModified(), false);

    QMessageAddress toAddress;
    toAddress.setRecipient(to);
    toAddress.setType(QMessageAddress::Email);
    QVERIFY(!message.to().isEmpty());
    QCOMPARE(message.to().first(), toAddress);
    QCOMPARE(message.to().first().recipient(), to);

    QMessageAddress fromAddress;
    fromAddress.setRecipient(from);
    fromAddress.setType(QMessageAddress::Email);
    QCOMPARE(message.from(), fromAddress);
    QCOMPARE(message.from().recipient(), from);

    QList<QMessageAddress> ccAddresses;
    foreach (const QString &element, cc.split(",", QString::SkipEmptyParts)) {
        QMessageAddress addr;
        addr.setRecipient(element.trimmed());
        addr.setType(QMessageAddress::Email);
        ccAddresses.append(addr);
    }
    QCOMPARE(message.cc(), ccAddresses);

    QCOMPARE(message.date(), QDateTime::fromString(date, Qt::ISODate));
    QCOMPARE(message.subject(), subject);

    QCOMPARE(message.contentType().toLower(), messageType.toLower());
    QCOMPARE(message.contentSubType().toLower(), messageSubType.toLower());

    QCOMPARE(message.parentFolderId(), testFolderId);
    QCOMPARE(message.standardFolder(), QMessage::InboxFolder);

    QAPPROXIMATECOMPARE(message.size(), messageSize, (messageSize / 2));

    QMessageContentContainerId bodyId(message.bodyId());
    QCOMPARE(bodyId.isValid(), true);
    QCOMPARE(bodyId != QMessageContentContainerId(), true);
    QCOMPARE(QMessageContentContainerId(bodyId.toString()), bodyId);

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

        // With MAPI, attachment order is not preserved
        QMessageContentContainer attachment(message.find(attachmentId));
        int index = attachments.indexOf(attachment.suggestedFileName());
        QVERIFY(index != -1);

        QCOMPARE(attachment.contentType().toLower(), attachmentType[index].toLower());
        QCOMPARE(attachment.contentSubType().toLower(), attachmentSubType[index].toLower());
        QCOMPARE(attachment.suggestedFileName(), attachments[index]);
        QAPPROXIMATECOMPARE(attachment.size(), attachmentSize[index], (attachmentSize[index] / 2));
    }

    QMessageIdList messageIds(QMessageStore::instance()->queryMessages());
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
    QAPPROXIMATECOMPARE(body.size(), 72u, 36u);

    QMessageStore::instance()->updateMessage(&message);
    QCOMPARE(QMessageStore::instance()->lastError(), QMessageStore::NoError);

#if defined(Q_OS_WIN)
	QTest::qSleep(1000);
#endif
    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();

    // MAPI generates multiple update notifications per message updated
    QVERIFY(catcher.updated.count() > 0);
    QCOMPARE(catcher.updated.first().first, messageId);
    QCOMPARE(catcher.updated.first().second.count(), 2);
    QCOMPARE(catcher.updated.first().second, QSet<QMessageStore::NotificationFilterId>() << filter2->id << filter3->id);

    QMessage updated(message.id());

    QCOMPARE(updated.id(), message.id());
    QCOMPARE(updated.isModified(), false);

    bodyId = updated.bodyId();
    QCOMPARE(bodyId.isValid(), true);
    QCOMPARE(bodyId != QMessageContentContainerId(), true);
    QCOMPARE(QMessageContentContainerId(bodyId.toString()), bodyId);

    body = updated.find(bodyId);
    QCOMPARE(body.contentType().toLower(), QByteArray("text"));
    QCOMPARE(body.contentSubType().toLower(), QByteArray("html"));
#if !defined(Q_OS_WIN)
    // Original charset is not preserved on windows
    QCOMPARE(body.contentCharset().toLower(), alternateCharset.toLower());
#endif
    QCOMPARE(body.isContentAvailable(), true);
    QCOMPARE(body.textContent(), replacementText);
    QAPPROXIMATECOMPARE(body.size(), 72u, 36u);

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

    // Test message removal
    QMessageStore::instance()->removeMessage(message.id());
    QCOMPARE(QMessageStore::instance()->lastError(), QMessageStore::NoError);
    QCOMPARE(QMessageStore::instance()->countMessages(), originalCount);

#if defined(Q_OS_WIN)
	QTest::qSleep(1000);
#endif
    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();

    QCOMPARE(removeCatcher.removed.count(), 1);
    QCOMPARE(removeCatcher.removed.first().first, messageId);
    QCOMPARE(removeCatcher.removed.first().second.count(), 1);
    QCOMPARE(removeCatcher.removed.first().second, QSet<QMessageStore::NotificationFilterId>() << filter3->id);
}

