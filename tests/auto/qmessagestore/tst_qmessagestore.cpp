/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in Technology Preview License Agreement accompanying
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QObject>
#include <QTest>
#include <QDebug>

#include "qtmessaging.h"
#include "../support/support.h"

//TESTED_CLASS=
//TESTED_FILES=

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

#ifdef QMESSAGING_OPTIONAL_FOLDER

    void testFolder_data();
    void testFolder();
#endif

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

    QMessageAccountId accountId(Support::addAccount(p));
    QVERIFY(accountId.isValid());
    
    QMessageAccount account(accountId);
    QCOMPARE(account.id(), accountId);
    QCOMPARE(account.name(), name);
#if !defined(Q_OS_WIN)
    // Not currently supported on windows...
    QCOMPARE(account.fromAddress().recipient(), fromAddress);
    QCOMPARE(account.fromAddress().type(), QMessageAddress::Email);
#endif

    QMessageAccountIdList accountIds(QMessageStore::instance()->queryAccounts());
    QVERIFY(accountIds.contains(accountId));
}

#ifdef QMESSAGING_OPTIONAL_FOLDER

void tst_QMessageStore::testFolder_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("displayName");
    QTest::addColumn<QString>("parentFolderPath");
    QTest::addColumn<QString>("displayNameResult");

    QTest::newRow("Inbox") << "Inbox" << "Inbox" << "" << "Inbox";
    QTest::newRow("Drafts") << "Drafts" << "" << "" << "Drafts";
    QTest::newRow("Archived") << "Inbox/Archived" << "Archived" << "Inbox" << "Archived";
    QTest::newRow("Backup") << "Inbox/Archived/Backup" << "" << "Inbox/Archived" << "Inbox/Archived/Backup";
}

void tst_QMessageStore::testFolder()
{
    // Ensure we have an account to link these folders to
    static const QString testAccountName("testAccount");
    QMessageAccountId testAccountId;
    QMessageAccountIdList accountIds(QMessageStore::instance()->queryAccounts(QMessageAccountFilterKey::name(testAccountName)));
#if defined(Q_OS_WIN) && !defined(ACCOUNT_FILTERING_IMPLEMENTED)
{
    // Key filtering is not implemented yet
    QMessageAccountIdList::iterator it = accountIds.begin(), end = accountIds.end();
    while (it != end) {
        QMessageAccount acct(*it);
        if (acct.name() == testAccountName) {
            accountIds.clear();
            accountIds.append(acct.id());
            break;
        }
        if (++it == end) {
            accountIds.clear();
        }
    }
}
#endif
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

    QMessageFolderId folderId(Support::addFolder(p));
    QVERIFY(folderId.isValid());
    
    QMessageFolder folder(folderId);
    QCOMPARE(folder.id(), folderId);
    QCOMPARE(folder.path(), path);
    QCOMPARE(folder.displayName(), displayNameResult);
    QCOMPARE(folder.parentAccountId(), testAccountId);

    if (!parentFolderPath.isEmpty()) {
        QMessageFolderFilterKey key(QMessageFolderFilterKey::path(parentFolderPath) & QMessageFolderFilterKey::parentAccountId(testAccountId));
        QMessageFolderIdList list(QMessageStore::instance()->queryFolders(key));
#if defined(Q_OS_WIN) && !defined(FOLDER_FILTERING_IMPLEMENTED)
{
    // Key filtering is not implemented yet
    QMessageFolderIdList::iterator it = list.begin(), end = list.end();
    while (it != end) {
        QMessageFolder fldr(*it);
        if ((fldr.parentAccountId() == testAccountId) && (fldr.path() == parentFolderPath)) {
            list.clear();
            list.append(fldr.id());
            break;
        }
        if (++it == end) {
            list.clear();
        }
    }
}
#endif
        QMessageFolderId parentFolderId(list.first());
        QCOMPARE(folder.parentFolderId(), parentFolderId);
    }

    QMessageFolderIdList folderIds(QMessageStore::instance()->queryFolders());
    QVERIFY(folderIds.contains(folderId));
}

#endif

typedef QMap<QString, QString> CustomFieldMap;
Q_DECLARE_METATYPE(CustomFieldMap)

void tst_QMessageStore::testMessage_data()
{
    QTest::addColumn<QString>("to");
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("date");
    QTest::addColumn<QString>("subject");
    QTest::addColumn<QString>("text");
    QTest::addColumn<CustomFieldMap>("custom");

    CustomFieldMap customData;
    customData.insert("cake", "chocolate");
    customData.insert("muffin", "blueberry");

    QTest::newRow("1")
        << "alice@example.com"
        << "bob@example.com"
        << "1999-12-31T23:59:59Z"
        << "Last message..."
        << "...before Y2K"
        << customData;
}

void tst_QMessageStore::testMessage()
{
    // Ensure we have an account to create messages with
    static const QString testAccountName("testAccount");

    QMessageAccountId testAccountId;
    QMessageAccountIdList accountIds(QMessageStore::instance()->queryAccounts(QMessageAccountFilterKey::name(testAccountName)));
#if defined(Q_OS_WIN) && !defined(ACCOUNT_FILTERING_IMPLEMENTED)
{
    // Key filtering is not implemented yet
    QMessageAccountIdList::iterator it = accountIds.begin(), end = accountIds.end();
    while (it != end) {
        QMessageAccount acct(*it);
        if (acct.name() == testAccountName) {
            accountIds.clear();
            accountIds.append(acct.id());
            break;
        }
        if (++it == end) {
            accountIds.clear();
        }
    }
}
#endif
    if (accountIds.isEmpty()) {
        Support::Parameters p;
        p.insert("name", testAccountName);
        testAccountId = Support::addAccount(p);
    } else {
        testAccountId = accountIds.first();
        QMessageAccount acc(testAccountId);
    }
    QVERIFY(testAccountId.isValid());

#ifdef QMESSAGING_OPTIONAL_FOLDER
    QMessageFolderId testFolderId;
    QMessageFolderFilterKey key(QMessageFolderFilterKey::displayName("Inbox") & QMessageFolderFilterKey::parentAccountId(testAccountId));
    QMessageFolderIdList folderIds(QMessageStore::instance()->queryFolders(key));
#if defined(Q_OS_WIN) && !defined(FOLDER_FILTERING_IMPLEMENTED)
{
    // Key filtering is not implemented yet
    QMessageFolderIdList::iterator it = folderIds.begin(), end = folderIds.end();
    while (it != end) {
        QMessageFolder fldr(*it);
        if ((fldr.parentAccountId() == testAccountId) && (fldr.displayName() == "Inbox")) {
            folderIds.clear();
            folderIds.append(fldr.id());
            break;
        }
        if (++it == end) {
            folderIds.clear();
        }
    }
}
#endif
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
#endif

    QFETCH(QString, to);
    QFETCH(QString, from);
    QFETCH(QString, date);
    QFETCH(QString, subject);
    QFETCH(QString, text);
    QFETCH(CustomFieldMap, custom);

    Support::Parameters p;
    p.insert("to", to);
    p.insert("from", from);
    p.insert("date", date);
    p.insert("subject", subject);
    p.insert("text", text);
    p.insert("parentAccountName", testAccountName);
#ifdef QMESSAGING_OPTIONAL_FOLDER
    p.insert("parentFolderPath", testFolder.path());
#endif

#ifdef QMESSAGING_OPTIONAL
    CustomFieldMap::const_iterator it = custom.begin(), end = custom.end();
    for ( ; it != end; ++it) {
        p.insert("custom-" + it.key(), it.value());
    }
#endif

    QMessageId messageId(Support::addMessage(p));
    QVERIFY(messageId.isValid());
    
    QMessage message(messageId);
    QCOMPARE(message.id(), messageId);

#if !defined(Q_OS_WIN)
    // Addresses are currently in the wrong format using MAPI
    QCOMPARE(message.to().first().recipient(), to);
    QCOMPARE(message.from().recipient(), from);
#endif
    QCOMPARE(message.date(), QDateTime::fromString(date, Qt::ISODate));
    QCOMPARE(message.subject(), subject);

    QMessageContentContainerId bodyId(message.body());
    QVERIFY(bodyId.isValid());

    QMessageContentContainer body(message.container(bodyId));
    // Note: this is not true, which is somewhat counter-intuitive:
    //QVERIFY(body.containerId().isValid());

    QCOMPARE(body.contentType().toLower(), QByteArray("text"));
#if !defined(Q_OS_WIN)
    // Not currently working for complicated reasons...
    QCOMPARE(body.contentSubType().toLower(), QByteArray("plain"));
    QCOMPARE(body.contentCharset().toLower(), QByteArray("utf-8"));
    QCOMPARE(body.contentAvailable(), true);
    QCOMPARE(body.decodedTextContent(), text);
#endif

#ifdef QMESSAGING_OPTIONAL
    QCOMPARE(message.customFields().toSet(), custom.keys().toSet());
    foreach (const QString &key, custom.keys()) {
        QCOMPARE(message.customField(key), custom[key]);
    }
#endif

    QMessageIdList messageIds(QMessageStore::instance()->queryMessages());
    QVERIFY(messageIds.contains(messageId));

    QString replacementText("This is replacement text.");

#ifdef QMESSAGING_OPTIONAL
    QMessageContentContainer replacement;
    replacement.setContentType("text");
    replacement.setContentSubType("fancy");
    replacement.setContentCharset("UTF-8");
    replacement.setContent(QByteArray(replacementText.toAscii()));
    message.replaceContent(bodyId, replacement);
#endif
    body = message.container(bodyId);

    QCOMPARE(body.contentType().toLower(), QByteArray("text"));
    QCOMPARE(body.contentSubType().toLower(), QByteArray("fancy"));
    QCOMPARE(body.contentCharset().toLower(), QByteArray("utf-8"));
    QCOMPARE(body.contentAvailable(), true);
    QCOMPARE(body.decodedTextContent(), replacementText);
}

