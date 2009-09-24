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
    if (!((expected - variance) < actual) ||
        !(actual < (expected + variance))) {
        qWarning() << "at line:" << line;
        qWarning() << "\tactual:" << actual;
        qWarning() << "\texpected:" << expected;
        qWarning() << "\tvariance:" << variance;
    }
    QVERIFY((expected - variance) < actual);
    QVERIFY(actual < (expected + variance));
}
#define QAPPROXIMATECOMPARE(a,e,v) approximateCompare(a,e,v,(__LINE__))

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
    QVERIFY(accountId != QMessageAccountId());
    
    QMessageAccount account(accountId);
    QCOMPARE(account.id(), accountId);
    QCOMPARE(account.name(), name);
    QCOMPARE(account.messageTypes(), QMessage::Email);

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
    QTest::newRow("Backup") << "Inbox/Archived/Backup" << "Backup" << "Inbox/Archived" << "Backup";
}

void tst_QMessageStore::testFolder()
{
    // Ensure we have an account to link these folders to
    static const QString testAccountName("testAccount");
    QMessageAccountId testAccountId;
    QMessageAccountIdList accountIds(QMessageStore::instance()->queryAccounts(QMessageAccountFilter::byName(testAccountName)));
#if defined(Q_OS_WIN) && !defined(ACCOUNT_FILTERING_IMPLEMENTED)
{
    // Filtering is not implemented yet
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
    QVERIFY(folderId != QMessageFolderId());
    
    QMessageFolder folder(folderId);
    QCOMPARE(folder.id(), folderId);
    QCOMPARE(folder.path(), path);
    QCOMPARE(folder.displayName(), displayNameResult);
    QCOMPARE(folder.parentAccountId(), testAccountId);

    if (!parentFolderPath.isEmpty()) {
        QMessageFolderFilter filter(QMessageFolderFilter::byPath(parentFolderPath) & QMessageFolderFilter::byParentAccountId(testAccountId));
        QMessageFolderIdList list(QMessageStore::instance()->queryFolders(filter));
#if defined(Q_OS_WIN) && !defined(FOLDER_FILTERING_IMPLEMENTED)
{
    // Filtering is not implemented yet
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

Q_DECLARE_METATYPE(QList<QByteArray>)
Q_DECLARE_METATYPE(QList<unsigned>)

typedef QMap<QString, QString> CustomFieldMap;
Q_DECLARE_METATYPE(CustomFieldMap)

void tst_QMessageStore::testMessage_data()
{
    QTest::addColumn<QString>("to");
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("date");
    QTest::addColumn<QString>("subject");
    QTest::addColumn<QByteArray>("messageType");
    QTest::addColumn<QByteArray>("messageSubType");
    QTest::addColumn<unsigned>("messageSize");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QByteArray>("bodyType");
    QTest::addColumn<QByteArray>("bodySubType");
    QTest::addColumn<unsigned>("bodySize");
    QTest::addColumn<QStringList>("attachments");
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
        << "1999-12-31T23:59:59Z"
        << "Last message..."
        << QByteArray("text")
        << QByteArray("plain")
        << 2048u
        << "...before Y2K"
        << QByteArray("text")
        << QByteArray("plain")
        << 24u
        << QStringList()
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<unsigned>()
        << customData;

    QTest::newRow("2")
        << "alice@example.com"
        << "bob@example.com"
        << "1999-12-31T23:59:59Z"
        << "Last HTML message..."
        << QByteArray("text")
        << QByteArray("html")
        << 2048u
        << "<html><p>...before <b>Y2K</b></p></html>"
        << QByteArray("text")
        << QByteArray("html")
        << 48u
        << QStringList()
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<unsigned>()
        << customData;

    QTest::newRow("3")
        << "alice@example.com"
        << "bob@example.com"
        << "1999-12-31T23:59:59Z"
        << "Last message..."
        << QByteArray("multipart")
        << QByteArray("mixed")
        << 2048u
        << "...before Y2K"
        << QByteArray("text")
        << QByteArray("plain")
        << 24u
        << ( QStringList() << "1.txt" )
        << ( QList<QByteArray>() << "text" )
        << ( QList<QByteArray>() << "plain" )
        << ( QList<unsigned>() << 512u )
        << customData;

    QTest::newRow("4")
        << "alice@example.com"
        << "bob@example.com"
        << "1999-12-31T23:59:59Z"
        << "Last HTML message..."
        << QByteArray("multipart")
        << QByteArray("mixed")
        << 5120u
        << "<html><p>...before <b>Y2K</b></p></html>"
        << QByteArray("text")
        << QByteArray("html")
        << 48u
        << ( QStringList() << "1.txt" << "qtlogo.png" )
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
#if defined(Q_OS_WIN) && !defined(ACCOUNT_FILTERING_IMPLEMENTED)
{
    // Filtering is not implemented yet
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
    QMessageFolderFilter filter(QMessageFolderFilter::byDisplayName("Inbox") & QMessageFolderFilter::byParentAccountId(testAccountId));
    QMessageFolderIdList folderIds(QMessageStore::instance()->queryFolders(filter));
#if defined(Q_OS_WIN) && !defined(FOLDER_FILTERING_IMPLEMENTED)
{
    // Filtering is not implemented yet
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
    QFETCH(QByteArray, messageType);
    QFETCH(QByteArray, messageSubType);
    QFETCH(unsigned, messageSize);
    QFETCH(QString, text);
    QFETCH(QByteArray, bodyType);
    QFETCH(QByteArray, bodySubType);
    QFETCH(unsigned, bodySize);
    QFETCH(QStringList, attachments);
    QFETCH(QList<QByteArray>, attachmentType);
    QFETCH(QList<QByteArray>, attachmentSubType);
    QFETCH(QList<unsigned>, attachmentSize);
    QFETCH(CustomFieldMap, custom);

    Support::Parameters p;
    p.insert("to", to);
    p.insert("from", from);
    p.insert("date", date);
    p.insert("subject", subject);
    p.insert("mimeType", bodyType + '/' + bodySubType);
    p.insert("text", text);
    p.insert("parentAccountName", testAccountName);
#ifdef QMESSAGING_OPTIONAL_FOLDER
    p.insert("parentFolderPath", testFolder.path());
#endif
    if (!attachments.isEmpty()) {
        QStringList attachmentPaths;
        foreach (const QString &fileName, attachments) {
            attachmentPaths.append(SRCDIR "/testdata/" + fileName);
        }
        p.insert("attachments", attachmentPaths.join("\n"));
        p.insert("status-hasAttachments", "true");
    }

    QMessageId messageId(Support::addMessage(p));
    QVERIFY(messageId.isValid());
    QVERIFY(messageId != QMessageId());
    
    QMessage message(messageId);
    QCOMPARE(message.id(), messageId);
    QCOMPARE(message.isModified(), false);

    QMessageAddress toAddress;
    toAddress.setRecipient(to);
    toAddress.setType(QMessageAddress::Email);
    QCOMPARE(message.to().first(), toAddress);
    QCOMPARE(message.to().first().recipient(), to);

    QMessageAddress fromAddress;
    fromAddress.setRecipient(from);
    fromAddress.setType(QMessageAddress::Email);
    QCOMPARE(message.from(), fromAddress);
    QCOMPARE(message.from().recipient(), from);

    QCOMPARE(message.date(), QDateTime::fromString(date, Qt::ISODate));
    QCOMPARE(message.subject(), subject);

    QCOMPARE(message.contentType().toLower(), messageType.toLower());
    QCOMPARE(message.contentSubType().toLower(), messageSubType.toLower());

    QAPPROXIMATECOMPARE(message.size(), messageSize, (messageSize / 2));

    QMessageContentContainerId bodyId(message.bodyId());
    QVERIFY(bodyId.isValid());

    QMessageContentContainer body(message.find(bodyId));
    // Note: this is not true, which is somewhat counter-intuitive:
    //QVERIFY(body.containerId().isValid());

    QCOMPARE(body.contentType().toLower(), bodyType.toLower());
    QCOMPARE(body.contentSubType().toLower(), bodySubType.toLower());
    QCOMPARE(body.contentCharset().toLower(), defaultCharset.toLower());
    QCOMPARE(body.isContentAvailable(), true);
    QCOMPARE(body.textContent(), text);
    QAPPROXIMATECOMPARE(body.size(), bodySize, (bodySize / 2));

    QMessageContentContainerIdList attachmentIds(message.attachmentIds());
    QCOMPARE(attachmentIds.count(), attachments.count());

    for (int i = 0; i < attachments.count(); ++i) {
        QMessageContentContainer attachment(message.find(attachmentIds[i]));
        QCOMPARE(attachment.contentType().toLower(), attachmentType[i].toLower());
        QCOMPARE(attachment.contentSubType().toLower(), attachmentSubType[i].toLower());
        QCOMPARE(attachment.suggestedFileName(), attachments[i].toAscii());
        QAPPROXIMATECOMPARE(attachment.size(), attachmentSize[i], (attachmentSize[i] / 2));
    }

    QMessageIdList messageIds(QMessageStore::instance()->queryMessages());
    QVERIFY(messageIds.contains(messageId));

    // Update the message to contain new text
    QString replacementText("This is replacement text.");

    message.setBody(replacementText, "text/fancy; charset=" + alternateCharset);
    body = message.find(bodyId);

    QCOMPARE(body.contentType().toLower(), QByteArray("text"));
    QCOMPARE(body.contentSubType().toLower(), QByteArray("fancy"));
    QCOMPARE(body.contentCharset().toLower(), alternateCharset.toLower());
    QCOMPARE(body.isContentAvailable(), true);
    QCOMPARE(body.textContent(), replacementText);
    QAPPROXIMATECOMPARE(body.size(), 72u, 36u);
}

