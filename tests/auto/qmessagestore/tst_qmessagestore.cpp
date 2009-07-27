/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Messaging Framework.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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

    void testFolder_data();
    void testFolder();
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
    QCOMPARE(account.fromAddress().recipient(), fromAddress);
    QCOMPARE(account.fromAddress().type(), QMessageAddress::Email);

    QMessageAccountIdList accountIds(QMessageStore::instance()->queryAccounts());
    QVERIFY(accountIds.contains(accountId));
}

void tst_QMessageStore::testFolder_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("displayName");
    QTest::addColumn<QString>("parentFolderPath");
    QTest::addColumn<QString>("displayNameResult");

    QTest::newRow("Inbox") << "INBOX" << "Inbox" << "" << "Inbox";
    QTest::newRow("Drafts") << "Drafts" << "" << "" << "Drafts";
    QTest::newRow("Archived") << "INBOX/archived" << "Archived" << "INBOX" << "Archived";
    QTest::newRow("Backup") << "INBOX/archived/backup" << "" << "INBOX/archived" << "INBOX/archived/backup";
}

void tst_QMessageStore::testFolder()
{
    // Ensure we have an account to link these folders to
    static const QString testAccountName("testAccount");
    QMessageAccountId testAccountId;
    QMessageAccountIdList accountIds(QMessageStore::instance()->queryAccounts(QMessageAccountFilterKey::name(testAccountName)));
    if (accountIds.isEmpty()) {
        Support::Parameters p;
        p.insert("name", testAccountName);
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
        QMessageFolderId parentFolderId(QMessageStore::instance()->queryFolders(key).first());
        QCOMPARE(folder.parentFolderId(), parentFolderId);
    }

    QMessageFolderIdList folderIds(QMessageStore::instance()->queryFolders());
    QVERIFY(folderIds.contains(folderId));
}

