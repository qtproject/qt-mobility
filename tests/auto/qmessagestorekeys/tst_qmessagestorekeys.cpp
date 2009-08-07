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
    Unit test for classes implementing query keys for the QMessageStore class.
*/
class tst_QMessageStoreKeys : public QObject
{
    Q_OBJECT

public:
    tst_QMessageStoreKeys();
    virtual ~tst_QMessageStoreKeys();

private slots:
    void initTestCase();
    void cleanup();
    void cleanupTestCase();

    void testAccountFilterKey_data();
    void testAccountFilterKey();

    void testAccountSortKey_data();
    void testAccountSortKey();

#ifdef QMESSAGING_OPTIONAL_FOLDER
    void testFolderFilterKey_data();
    void testFolderFilterKey();

    void testFolderSortKey_data();
    void testFolderSortKey();
#endif
    void testMessageFilterKey_data();
    void testMessageFilterKey();

    void testMessageSortKey_data();
    void testMessageSortKey();

private:
#ifdef QMESSAGING_OPTIONAL_FOLDER
    QMessageFolderIdList standardFolderIds;
    QMessageFolderIdList folderIds;
#endif

    QMessageAccountIdList accountIds;
    QMessageIdList messageIds;
};

Q_DECLARE_METATYPE(QMessageAccountIdList)
Q_DECLARE_METATYPE(QMessageAccountFilterKey)
Q_DECLARE_METATYPE(QMessageAccountSortKey)

#ifdef QMESSAGING_OPTIONAL_FOLDER
Q_DECLARE_METATYPE(QMessageFolderIdList)
Q_DECLARE_METATYPE(QMessageFolderFilterKey)
Q_DECLARE_METATYPE(QMessageFolderSortKey)
#endif

Q_DECLARE_METATYPE(QMessageIdList)
Q_DECLARE_METATYPE(QMessageFilterKey)
Q_DECLARE_METATYPE(QMessageSortKey)

typedef QList<QMessageIdList> MessageListList;
Q_DECLARE_METATYPE(MessageListList)

QDebug &operator<<(QDebug &dbg, const QMessageAccountId &id)
{
    return dbg << id.toString();
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QDebug &operator<<(QDebug &dbg, const QMessageFolderId &id)
{
    return dbg << id.toString();
}
#endif
QDebug &operator<<(QDebug &dbg, const QMessageId &id)
{
    return dbg << id.toString();
}

QTEST_MAIN(tst_QMessageStoreKeys)

#include "tst_qmessagestorekeys.moc"

tst_QMessageStoreKeys::tst_QMessageStoreKeys()
{
}

tst_QMessageStoreKeys::~tst_QMessageStoreKeys()
{
}

class Params
{
    QMap<const char*, const char*> _map;

public:
    Params() {}

    Params& operator()(const char* name, const char* value)
    {
        _map.insert(name, value); return *this; 
    }

    operator Support::Parameters() const
    {
        Support::Parameters result;
        QMap<const char*, const char*>::const_iterator it = _map.begin(), end = _map.end();
        for ( ; it != end; ++it) {
            result.insert(it.key(), it.value());
        }
        return result;
    }
};

void tst_QMessageStoreKeys::initTestCase()
{
    Support::clearMessageStore();

    QList<Support::Parameters> accountParams;

    accountParams << Params()("name", "Work")
                             ("fromAddress", "Important.Person@example.com")
                  << Params()("name", "Personal")
                             ("fromAddress", "superstar2000@example.net")
                  << Params()("name", "Alter Ego")
                             ("fromAddress", "UltraMegaLightningBabe@superhero.test");

#ifdef QMESSAGING_OPTIONAL_FOLDER
    standardFolderIds = QMessageStore::instance()->queryFolders();

    QList<Support::Parameters> folderParams;
    folderParams << Params()("parentAccountName", "Alter Ego")
                            ("path", "[Root]")
                            ("displayName", "My messages")
                            ("parentFolderPath", "")
                 << Params()("parentAccountName", "Work")
                            ("path", "/Inbox")
                            ("displayName", "Inbox")
                            ("parentFolderPath", "")
                 << Params()("parentAccountName", "Work")
                            ("path", "/Inbox/X-Announce")
                            ("displayName", "X-Announce")
                            ("parentFolderPath", "/Inbox")
                 << Params()("parentAccountName", "Work")
                            ("path", "/Inbox/X-Announce/X-Archived")
                            ("displayName", "X-Archived")
                            ("parentFolderPath", "/Inbox/X-Announce");

foreach (const Support::Parameters &params, folderParams) {
        folderIds.append(Support::addFolder(params));
        QVERIFY(folderIds.last().isValid());
    }
#endif

    QList<Support::Parameters> messageParams;
    messageParams << Params()("parentAccountName", "Alter Ego")
                             ("parentFolderPath", "[Root]")
                             ("type", "sms")
                             ("to", "SuperMegaLightningBabe")
                             ("from", "Frozone")
                             ("subject", "Ice to meet you")
                             ("date", "2000-01-01T12:00:00Z")
                             ("receivedDate", "2000-01-01T12:01:00Z")
                             ("priority", "")
                             ("size", "160")
                             ("status-read", "true")
                             ("custom-flagged", "true")
                  << Params()("parentAccountName", "Work")
                             ("parentFolderPath", "/Inbox")
                             ("type", "email")
                             ("to", "Important.Person@example.com")
                             ("from", "Esteemed.Colleague@example.com")
                             ("subject", "Meeting agenda")
                             ("date", "2000-01-01T12:00:00Z")
                             ("receivedDate", "2000-01-01T20:00:00Z")
                             ("priority", "High")
                             ("size", "10240")
                             ("status-hasAttachments", "true")
                             ("custom-spam", "filter:no")
                             ("custom-flagged", "true")
                  << Params()("parentAccountName", "Work")
                             ("parentFolderPath", "/Inbox")
                             ("type", "email")
                             ("to", "Important.Person@example.com,Minion@example.com")
                             ("from", "Big.Boss@example.com")
                             ("subject", "Motivational message")
                             ("date", "2000-01-01T13:00:00Z")
                             ("receivedDate", "2000-01-01T13:05:00Z")
                             ("priority", "High")
                             ("size", "20480")
                             ("status-hasAttachments", "true")
                             ("custom-spam", "filter:no")
                  << Params()("parentAccountName", "Work")
                             ("parentFolderPath", "/Inbox/X-Announce")
                             ("type", "email")
                             ("to", "announce@example.com,maintenance-log@example.com")
                             ("from", "sysadmin@example.com")
                             ("subject", "Scheduled maintenance")
                             ("date", "2000-01-01T13:00:00Z")
                             ("receivedDate", "2000-01-01T13:00:01Z")
                             ("priority", "")
                             ("size", "1056")
                             ("status-read", "true")
                             ("custom-spam", "filter:maybe")
                  << Params()("parentAccountName", "Work")
                             ("parentFolderPath", "/Inbox/X-Announce/X-Archived")
                             ("type", "email")
                             ("to", "announce@example.com")
                             ("from", "Big.Boss@example.com")
                             ("subject", "Free beer")
                             ("date", "1999-04-01T10:30:00Z")
                             ("receivedDate", "1999-04-01T10:31:00Z")
                             ("priority", "Low")
                             ("size", "4096")
                             ("status-read", "true")
                             ("status-hasAttachments", "true")
                             ("custom-spam", "filter:yes");

    foreach (const Support::Parameters &params, accountParams) {
        accountIds.append(Support::addAccount(params));
        QVERIFY(accountIds.last().isValid());
    }
        foreach (const Support::Parameters &params, messageParams) {
        messageIds.append(Support::addMessage(params));
        QVERIFY(messageIds.last().isValid());
    }
}

void tst_QMessageStoreKeys::cleanup()
{
}

void tst_QMessageStoreKeys::cleanupTestCase()
{
}

void tst_QMessageStoreKeys::testAccountFilterKey_data()
{
    QTest::addColumn<QMessageAccountFilterKey>("key");
    QTest::addColumn<QMessageAccountIdList>("ids");
    QTest::addColumn<QMessageAccountIdList>("negatedIds");

    QTest::newRow("empty key")
        << QMessageAccountFilterKey() 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id equality 1")
        << QMessageAccountFilterKey::id(accountIds[0], QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("id equality 2")
        << QMessageAccountFilterKey::id(accountIds[1], QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("id equality invalid")
        << QMessageAccountFilterKey::id(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id inequality 1")
        << QMessageAccountFilterKey::id(accountIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id inequality 2")
        << QMessageAccountFilterKey::id(accountIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id inequality invalid")
        << QMessageAccountFilterKey::id(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id list inclusion 1")
        << QMessageAccountFilterKey::id(QMessageAccountIdList() << accountIds[0], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("id list inclusion 2")
        << QMessageAccountFilterKey::id(QMessageAccountIdList() << accountIds[1], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("id list inclusion 3")
        << QMessageAccountFilterKey::id(QMessageAccountIdList() << accountIds[0] << accountIds[1], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[2] );

    QTest::newRow("id list inclusion empty")
        << QMessageAccountFilterKey::id(QMessageAccountIdList(), QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id list exclusion 1")
        << QMessageAccountFilterKey::id(QMessageAccountIdList() << accountIds[0], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id list exclusion 2")
        << QMessageAccountFilterKey::id(QMessageAccountIdList() << accountIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id list exclusion 3")
        << QMessageAccountFilterKey::id(QMessageAccountIdList() << accountIds[0] << accountIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] );

    QTest::newRow("id list exclusion empty")
        << QMessageAccountFilterKey::id(QMessageAccountIdList(), QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion 1")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("Work", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("id filter inclusion 2")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("Personal", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("id filter inclusion 3")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("o", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion empty")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion non-matching")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion 1")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("Work", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id filter exclusion 2")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("Personal", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id filter exclusion 3")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("o", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion empty")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion non-matching")
        << QMessageAccountFilterKey::id(QMessageAccountFilterKey::name("NoneSuch"), QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name equality 1")
        << QMessageAccountFilterKey::name("Work", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("name equality 2")
        << QMessageAccountFilterKey::name("Personal", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("name equality non-matching")
        << QMessageAccountFilterKey::name("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name equality empty")
        << QMessageAccountFilterKey::name(QString(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name equality zero-length")
        << QMessageAccountFilterKey::name("", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name inequality 1")
        << QMessageAccountFilterKey::name("Work", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("name inequality 2")
        << QMessageAccountFilterKey::name("Personal", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("name inequality non-matching")
        << QMessageAccountFilterKey::name("Nonesuch", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inequality empty")
        << QMessageAccountFilterKey::name(QString(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inequality zero-length")
        << QMessageAccountFilterKey::name("", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion 1")
        << QMessageAccountFilterKey::name("ork", QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("name inclusion 2")
        << QMessageAccountFilterKey::name("o", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion non-matching")
        << QMessageAccountFilterKey::name("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name inclusion empty")
        << QMessageAccountFilterKey::name(QString(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion zero-length")
        << QMessageAccountFilterKey::name("", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name exclusion 1")
        << QMessageAccountFilterKey::name("ork", QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("name exclusion 2")
        << QMessageAccountFilterKey::name("o", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name exclusion non-matching")
        << QMessageAccountFilterKey::name("Nonesuch", QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name exclusion empty")
        << QMessageAccountFilterKey::name(QString(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name exclusion zero-length")
        << QMessageAccountFilterKey::name("", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress equality 1")
        << QMessageAccountFilterKey::fromAddress("Important.Person@example.com", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("fromAddress equality 2")
        << QMessageAccountFilterKey::fromAddress("superstar2000@example.net", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("fromAddress equality non-matching")
        << QMessageAccountFilterKey::fromAddress("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress equality empty")
        << QMessageAccountFilterKey::fromAddress(QString(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress equality zero-length")
        << QMessageAccountFilterKey::fromAddress("", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress inequality 1")
        << QMessageAccountFilterKey::fromAddress("Important.Person@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("fromAddress inequality 2")
        << QMessageAccountFilterKey::fromAddress("superstar2000@example.net", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("fromAddress inequality non-matching")
        << QMessageAccountFilterKey::fromAddress("Nonesuch", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inequality empty")
        << QMessageAccountFilterKey::fromAddress(QString(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inequality zero-length")
        << QMessageAccountFilterKey::fromAddress("", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inclusion 1")
        << QMessageAccountFilterKey::fromAddress("Import", QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("fromAddress inclusion 2")
        << QMessageAccountFilterKey::fromAddress("@example.", QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[2] );

    QTest::newRow("fromAddress inclusion non-matching")
        << QMessageAccountFilterKey::fromAddress("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress inclusion empty")
        << QMessageAccountFilterKey::fromAddress(QString(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inclusion zero-length")
        << QMessageAccountFilterKey::fromAddress("", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress exclusion 1")
        << QMessageAccountFilterKey::fromAddress("Import", QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("fromAddress exclusion 2")
        << QMessageAccountFilterKey::fromAddress("@example.", QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] );

    QTest::newRow("fromAddress exclusion non-matching")
        << QMessageAccountFilterKey::fromAddress("Nonesuch", QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress exclusion empty")
        << QMessageAccountFilterKey::fromAddress(QString(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress exclusion zero-length")
        << QMessageAccountFilterKey::fromAddress("", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;
}

void tst_QMessageStoreKeys::testAccountFilterKey()
{
    QFETCH(QMessageAccountFilterKey, key);
    QFETCH(QMessageAccountIdList, ids);
    QFETCH(QMessageAccountIdList, negatedIds);

    // Order is irrelevant for filtering
    QCOMPARE(QMessageStore::instance()->queryAccounts(key).toSet(), ids.toSet());
    QCOMPARE(QMessageStore::instance()->queryAccounts(~key).toSet(), negatedIds.toSet());
}

void tst_QMessageStoreKeys::testAccountSortKey_data()
{
    QTest::addColumn<QMessageAccountSortKey>("sortKey");
    QTest::addColumn<QMessageAccountIdList>("ids");

    QTest::newRow("name ascending")
        << QMessageAccountSortKey::name(Qt::AscendingOrder)
        << ( QMessageAccountIdList() << accountIds[2] << accountIds[1] << accountIds[0] );

    QTest::newRow("name descending")
        << QMessageAccountSortKey::name(Qt::DescendingOrder)
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] << accountIds[2] );
}

void tst_QMessageStoreKeys::testAccountSortKey()
{
    QFETCH(QMessageAccountSortKey, sortKey);
    QFETCH(QMessageAccountIdList, ids);

    QCOMPARE(QMessageStore::instance()->queryAccounts(QMessageAccountFilterKey(), sortKey), ids);
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
void tst_QMessageStoreKeys::testFolderFilterKey_data()
{
    QTest::addColumn<QMessageFolderFilterKey>("key");
    QTest::addColumn<QMessageFolderIdList>("ids");
    QTest::addColumn<QMessageFolderIdList>("negatedIds");

    QTest::newRow("empty key")
        << QMessageFolderFilterKey() 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("id equality 1")
        << QMessageFolderFilterKey::id(folderIds[0], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("id equality 2")
        << QMessageFolderFilterKey::id(folderIds[1], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("id equality invalid")
        << QMessageFolderFilterKey::id(QMessageFolderId(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("id inequality 1")
        << QMessageFolderFilterKey::id(folderIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("id inequality 2")
        << QMessageFolderFilterKey::id(folderIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[1] );

    QTest::newRow("id inequality invalid")
        << QMessageFolderFilterKey::id(QMessageFolderId(), QMessageDataComparator::NotEqual) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("id list inclusion 1")
        << QMessageFolderFilterKey::id(QMessageFolderIdList() << folderIds[0], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("id list inclusion 2")
        << QMessageFolderFilterKey::id(QMessageFolderIdList() << folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("id list inclusion 3")
        << QMessageFolderFilterKey::id(QMessageFolderIdList() << folderIds[0] << folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("id list inclusion empty")
        << QMessageFolderFilterKey::id(QMessageFolderIdList(), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("id list exclusion 1")
        << QMessageFolderFilterKey::id(QMessageFolderIdList() << folderIds[0], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("id list exclusion 2")
        << QMessageFolderFilterKey::id(QMessageFolderIdList() << folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[1] );

    QTest::newRow("id list exclusion 3")
        << QMessageFolderFilterKey::id(QMessageFolderIdList() << folderIds[0] << folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("id list exclusion empty")
        << QMessageFolderFilterKey::id(QMessageFolderIdList(), QMessageDataComparator::Excludes) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("id filter inclusion 1")
        << QMessageFolderFilterKey::id(QMessageFolderFilterKey::path("[Root]", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("id filter inclusion 2")
        << QMessageFolderFilterKey::id(QMessageFolderFilterKey::path("/Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("id filter inclusion 3")
        << QMessageFolderFilterKey::id(QMessageFolderFilterKey::path("Inbox/X-Ann", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds;

    QTest::newRow("id filter inclusion empty")
        << QMessageFolderFilterKey::id(QMessageFolderFilterKey(), QMessageDataComparator::Includes) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("id filter inclusion non-matching")
        << QMessageFolderFilterKey::id(QMessageFolderFilterKey::path("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("id filter exclusion 1")
        << QMessageFolderFilterKey::id(QMessageFolderFilterKey::path("[Root]", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("id filter exclusion 2")
        << QMessageFolderFilterKey::id(QMessageFolderFilterKey::path("/Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("id filter exclusion 3")
        << QMessageFolderFilterKey::id(QMessageFolderFilterKey::path("Inbox/X-Ann", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("id filter exclusion empty")
        << QMessageFolderFilterKey::id(QMessageFolderFilterKey(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("id filter exclusion non-matching")
        << QMessageFolderFilterKey::id(QMessageFolderFilterKey::path("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("path equality 1")
        << QMessageFolderFilterKey::path("[Root]", QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("path equality 2")
        << QMessageFolderFilterKey::path("/Inbox/X-Announce", QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("path equality non-matching")
        << QMessageFolderFilterKey::path("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("path equality empty")
        << QMessageFolderFilterKey::path(QString(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("path equality zero-length")
        << QMessageFolderFilterKey::path("", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("path inequality 1")
        << QMessageFolderFilterKey::path("[Root]", QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("path inequality 2")
        << QMessageFolderFilterKey::path("/Inbox/X-Announce", QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("path inequality non-matching")
        << QMessageFolderFilterKey::path("Nonesuch", QMessageDataComparator::NotEqual) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("path inequality empty")
        << QMessageFolderFilterKey::path(QString(), QMessageDataComparator::NotEqual) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("path inequality zero-length")
        << QMessageFolderFilterKey::path("", QMessageDataComparator::NotEqual) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("path inclusion 1")
        << QMessageFolderFilterKey::path("oot", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("path inclusion 2")
        << QMessageFolderFilterKey::path("Inbox/X-Ann", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds;

    QTest::newRow("path inclusion non-matching")
        << QMessageFolderFilterKey::path("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("path inclusion empty")
        << QMessageFolderFilterKey::path(QString(), QMessageDataComparator::Includes) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("path inclusion zero-length")
        << QMessageFolderFilterKey::path("", QMessageDataComparator::Includes) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("path exclusion 1")
        << QMessageFolderFilterKey::path("oot", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("path exclusion 2")
        << QMessageFolderFilterKey::path("Inbox/X-Ann", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("path exclusion non-matching")
        << QMessageFolderFilterKey::path("Nonesuch", QMessageDataComparator::Excludes) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("path exclusion empty")
        << QMessageFolderFilterKey::path(QString(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("path exclusion zero-length")
        << QMessageFolderFilterKey::path("", QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("displayName equality 1")
        << QMessageFolderFilterKey::displayName("My messages", QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("displayName equality 2")
        << QMessageFolderFilterKey::displayName("X-Announce", QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("displayName equality non-matching")
        << QMessageFolderFilterKey::displayName("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("displayName equality empty")
        << QMessageFolderFilterKey::displayName(QString(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("displayName equality zero-length")
        << QMessageFolderFilterKey::displayName("", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("displayName inequality 1")
        << QMessageFolderFilterKey::displayName("My messages", QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("displayName inequality 2")
        << QMessageFolderFilterKey::displayName("X-Announce", QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("displayName inequality non-matching")
        << QMessageFolderFilterKey::displayName("Nonesuch", QMessageDataComparator::NotEqual) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inequality empty")
        << QMessageFolderFilterKey::displayName(QString(), QMessageDataComparator::NotEqual) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inequality zero-length")
        << QMessageFolderFilterKey::displayName("", QMessageDataComparator::NotEqual) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inclusion 1")
        << QMessageFolderFilterKey::displayName("message", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("displayName inclusion 2")
        << QMessageFolderFilterKey::displayName("X-A", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds;

    QTest::newRow("displayName inclusion non-matching")
        << QMessageFolderFilterKey::displayName("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("displayName inclusion empty")
        << QMessageFolderFilterKey::displayName(QString(), QMessageDataComparator::Includes) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inclusion zero-length")
        << QMessageFolderFilterKey::displayName("", QMessageDataComparator::Includes) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName exclusion 1")
        << QMessageFolderFilterKey::displayName("messages", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("displayName exclusion 2")
        << QMessageFolderFilterKey::displayName("X-A", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName exclusion non-matching")
        << QMessageFolderFilterKey::displayName("Nonesuch", QMessageDataComparator::Excludes) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName exclusion empty")
        << QMessageFolderFilterKey::displayName(QString(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("displayName exclusion zero-length")
        << QMessageFolderFilterKey::displayName("", QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("parentAccountId equality 1")
        << QMessageFolderFilterKey::parentAccountId(accountIds[0], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] ) + standardFolderIds;

    QTest::newRow("parentAccountId equality 2")
        << QMessageFolderFilterKey::parentAccountId(accountIds[2], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("parentAccountId equality invalid")
        << QMessageFolderFilterKey::parentAccountId(QMessageAccountId(), QMessageDataComparator::Equal) 
        << standardFolderIds
        << folderIds;

    QTest::newRow("parentAccountId inequality 1")
        << QMessageFolderFilterKey::parentAccountId(accountIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId inequality 2")
        << QMessageFolderFilterKey::parentAccountId(accountIds[2], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId inequality invalid")
        << QMessageFolderFilterKey::parentAccountId(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << folderIds
        << standardFolderIds;

    QTest::newRow("parentAccountId filter inclusion 1")
        << QMessageFolderFilterKey::parentAccountId(QMessageAccountFilterKey::name("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("parentAccountId filter inclusion 2")
        << QMessageFolderFilterKey::parentAccountId(QMessageAccountFilterKey::name("Work", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] ) + standardFolderIds;

    QTest::newRow("parentAccountId filter inclusion 3")
        << QMessageFolderFilterKey::parentAccountId(QMessageAccountFilterKey::name("o", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << folderIds
        << standardFolderIds;

    QTest::newRow("parentAccountId filter inclusion empty")
        << QMessageFolderFilterKey::parentAccountId(QMessageAccountFilterKey(), QMessageDataComparator::Includes) 
        << folderIds 
        << standardFolderIds;

    QTest::newRow("parentAccountId filter inclusion non-matching")
        << QMessageFolderFilterKey::parentAccountId(QMessageAccountFilterKey::name("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("parentAccountId filter exclusion 1")
        << QMessageFolderFilterKey::parentAccountId(QMessageAccountFilterKey::name("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId filter exclusion 2")
        << QMessageFolderFilterKey::parentAccountId(QMessageAccountFilterKey::name("Work", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId filter exclusion 3")
        << QMessageFolderFilterKey::parentAccountId(QMessageAccountFilterKey::name("o", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << standardFolderIds
        << folderIds;

    QTest::newRow("parentAccountId filter exclusion empty")
        << QMessageFolderFilterKey::parentAccountId(QMessageAccountFilterKey(), QMessageDataComparator::Excludes) 
        << standardFolderIds
        << folderIds;

    QTest::newRow("parentAccountId filter exclusion non-matching")
        << QMessageFolderFilterKey::parentAccountId(QMessageAccountFilterKey::name("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("parentFolderId equality 1")
        << QMessageFolderFilterKey::parentFolderId(folderIds[1], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("parentFolderId equality 2")
        << QMessageFolderFilterKey::parentFolderId(folderIds[2], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] ) + standardFolderIds;

    QTest::newRow("parentFolderId equality invalid")
        << QMessageFolderFilterKey::parentFolderId(QMessageFolderId(), QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("parentFolderId inequality 1")
        << QMessageFolderFilterKey::parentFolderId(folderIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("parentFolderId inequality 2")
        << QMessageFolderFilterKey::parentFolderId(folderIds[2], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("parentFolderId inequality invalid")
        << QMessageFolderFilterKey::parentFolderId(QMessageFolderId(), QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds;

    QTest::newRow("parentFolderId filter inclusion 1")
        << QMessageFolderFilterKey::parentFolderId(QMessageFolderFilterKey::path("/Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] ) + standardFolderIds;

    QTest::newRow("parentFolderId filter inclusion 2")
        << QMessageFolderFilterKey::parentFolderId(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] ) + standardFolderIds;

    QTest::newRow("parentFolderId filter inclusion 3")
        << QMessageFolderFilterKey::parentFolderId(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds;

    QTest::newRow("parentFolderId filter inclusion empty")
        << QMessageFolderFilterKey::parentFolderId(QMessageFolderFilterKey(), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds;

    QTest::newRow("parentFolderId filter inclusion non-matching")
        << QMessageFolderFilterKey::parentFolderId(QMessageFolderFilterKey::path("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("parentFolderId filter exclusion 1")
        << QMessageFolderFilterKey::parentFolderId(QMessageFolderFilterKey::path("/Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("parentFolderId filter exclusion 2")
        << QMessageFolderFilterKey::parentFolderId(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("parentFolderId filter exclusion 3")
        << QMessageFolderFilterKey::parentFolderId(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("parentFolderId filter exclusion empty")
        << QMessageFolderFilterKey::parentFolderId(QMessageFolderFilterKey(), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("parentFolderId filter exclusion non-matching")
        << QMessageFolderFilterKey::parentFolderId(QMessageFolderFilterKey::path("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("ancestorFolderIds list inclusion 1")
        << QMessageFolderFilterKey::ancestorFolderIds(folderIds[2], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] ) + standardFolderIds;

    QTest::newRow("ancestorFolderIds list inclusion 2")
        << QMessageFolderFilterKey::ancestorFolderIds(folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds;

    QTest::newRow("ancestorFolderIds list inclusion invalid")
        << QMessageFolderFilterKey::ancestorFolderIds(QMessageFolderId(), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("ancestorFolderIds list exclusion 1")
        << QMessageFolderFilterKey::ancestorFolderIds(folderIds[2], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("ancestorFolderIds list exclusion 2")
        << QMessageFolderFilterKey::ancestorFolderIds(folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds list exclusion invalid")
        << QMessageFolderFilterKey::ancestorFolderIds(QMessageFolderId(), QMessageDataComparator::Excludes) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();

    QTest::newRow("ancestorFolderIds filter inclusion 1")
        << QMessageFolderFilterKey::ancestorFolderIds(QMessageFolderFilterKey::path("/Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] ) + standardFolderIds;

    QTest::newRow("ancestorFolderIds filter inclusion 2")
        << QMessageFolderFilterKey::ancestorFolderIds(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds;

    QTest::newRow("ancestorFolderIds filter inclusion 3")
        << QMessageFolderFilterKey::ancestorFolderIds(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds;

    QTest::newRow("ancestorFolderIds filter inclusion empty")
        << QMessageFolderFilterKey::ancestorFolderIds(QMessageFolderFilterKey(), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds;

    QTest::newRow("ancestorFolderIds filter inclusion non-matching")
        << QMessageFolderFilterKey::ancestorFolderIds(QMessageFolderFilterKey::path("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds + standardFolderIds;

    QTest::newRow("ancestorFolderIds filter exclusion 1")
        << QMessageFolderFilterKey::ancestorFolderIds(QMessageFolderFilterKey::path("/Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion 2")
        << QMessageFolderFilterKey::ancestorFolderIds(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion 3")
        << QMessageFolderFilterKey::ancestorFolderIds(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion empty")
        << QMessageFolderFilterKey::ancestorFolderIds(QMessageFolderFilterKey(), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] ) + standardFolderIds
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion non-matching")
        << QMessageFolderFilterKey::ancestorFolderIds(QMessageFolderFilterKey::path("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds + standardFolderIds
        << QMessageFolderIdList();
}

void tst_QMessageStoreKeys::testFolderFilterKey()
{
    QFETCH(QMessageFolderFilterKey, key);
    QFETCH(QMessageFolderIdList, ids);
    QFETCH(QMessageFolderIdList, negatedIds);

    // Order is irrelevant for filtering
    QCOMPARE(QMessageStore::instance()->queryFolders(key).toSet(), ids.toSet());
    QCOMPARE(QMessageStore::instance()->queryFolders(~key).toSet(), negatedIds.toSet());
}

void tst_QMessageStoreKeys::testFolderSortKey_data()
{
    QTest::addColumn<QMessageFolderSortKey>("sortKey");
    QTest::addColumn<QMessageFolderIdList>("ids");

    QTest::newRow("path ascending")
        << QMessageFolderSortKey::path(Qt::AscendingOrder)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] << folderIds[0] );

    QTest::newRow("path descending")
        << QMessageFolderSortKey::path(Qt::DescendingOrder)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[3] << folderIds[2] << folderIds[1] );

    QTest::newRow("displayName ascending")
        << QMessageFolderSortKey::displayName(Qt::AscendingOrder)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[0] << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName descending")
        << QMessageFolderSortKey::displayName(Qt::DescendingOrder)
        << ( QMessageFolderIdList() << folderIds[3] << folderIds[2] << folderIds[0] << folderIds[1] );
}

void tst_QMessageStoreKeys::testFolderSortKey()
{
    QFETCH(QMessageFolderSortKey, sortKey);
    QFETCH(QMessageFolderIdList, ids);

    // Filter out the standard folders
    QMessageFolderFilterKey key(QMessageFolderFilterKey::parentAccountId(QMessageAccountId(), QMessageDataComparator::NotEqual));
    QCOMPARE(QMessageStore::instance()->queryFolders(key, sortKey), ids);
}

#endif //QMESSAGING_OPTIONAL_FOLDER

void tst_QMessageStoreKeys::testMessageFilterKey_data()
{
    QTest::addColumn<QMessageFilterKey>("key");
    QTest::addColumn<QMessageIdList>("ids");
    QTest::addColumn<QMessageIdList>("negatedIds");

    QTest::newRow("empty key")
        << QMessageFilterKey() 
        << messageIds
        << QMessageIdList();

    QTest::newRow("id equality 1")
        << QMessageFilterKey::id(messageIds[0], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id equality 2")
        << QMessageFilterKey::id(messageIds[1], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] )
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id equality invalid")
        << QMessageFilterKey::id(QMessageId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("id inequality 1")
        << QMessageFilterKey::id(messageIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("id inequality 2")
        << QMessageFilterKey::id(messageIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] );

    QTest::newRow("id inequality invalid")
        << QMessageFilterKey::id(QMessageId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("id list inclusion 1")
        << QMessageFilterKey::id(QMessageIdList() << messageIds[0], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id list inclusion 2")
        << QMessageFilterKey::id(QMessageIdList() << messageIds[1], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] )
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id list inclusion 3")
        << QMessageFilterKey::id(QMessageIdList() << messageIds[0] << messageIds[1], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] )
        << ( QMessageIdList() << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id list inclusion empty")
        << QMessageFilterKey::id(QMessageIdList(), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("id list exclusion 1")
        << QMessageFilterKey::id(QMessageIdList() << messageIds[0], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("id list exclusion 2")
        << QMessageFilterKey::id(QMessageIdList() << messageIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] );

    QTest::newRow("id list exclusion 3")
        << QMessageFilterKey::id(QMessageIdList() << messageIds[0] << messageIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] );

    QTest::newRow("id list exclusion empty")
        << QMessageFilterKey::id(QMessageIdList(), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("type equality 1")
        << QMessageFilterKey::type(QMessage::Sms, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("type equality 2")
        << QMessageFilterKey::type(QMessage::Email, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("type equality invalid")
        << QMessageFilterKey::type(QMessage::None, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("type inequality 1")
        << QMessageFilterKey::type(QMessage::Sms, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("type inequality 2")
        << QMessageFilterKey::type(QMessage::Email, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("type inequality invalid")
        << QMessageFilterKey::type(QMessage::None, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("type mask inclusion 1")
        << QMessageFilterKey::type(QMessage::Sms, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("type mask inclusion 2")
        << QMessageFilterKey::type(QMessage::Email, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("type mask inclusion 3")
        << QMessageFilterKey::type(QMessage::Sms | QMessage::Email, QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("type mask inclusion empty")
        << QMessageFilterKey::type(QMessage::None, QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("type mask exclusion 1")
        << QMessageFilterKey::type(QMessage::Sms, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("type mask exclusion 2")
        << QMessageFilterKey::type(QMessage::Email, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("type mask exclusion 3")
        << QMessageFilterKey::type(QMessage::Sms | QMessage::Email, QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("type mask exclusion empty")
        << QMessageFilterKey::type(QMessage::None, QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender equality 1")
        << QMessageFilterKey::sender("Esteemed.Colleague@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] )
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("sender equality 2")
        << QMessageFilterKey::sender("sysadmin@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("sender equality non-matching")
        << QMessageFilterKey::sender("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender equality empty")
        << QMessageFilterKey::sender(QString(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender equality zero-length")
        << QMessageFilterKey::sender("", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender inequality 1")
        << QMessageFilterKey::sender("Esteemed.Colleague@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] );

    QTest::newRow("sender inequality 2")
        << QMessageFilterKey::sender("sysadmin@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] );

    QTest::newRow("sender inequality non-matching")
        << QMessageFilterKey::sender("Nonesuch", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender inequality empty")
        << QMessageFilterKey::sender(QString(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender inequality zero-length")
        << QMessageFilterKey::sender("", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender inclusion 1")
        << QMessageFilterKey::sender("example", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("sender inclusion 2")
        << QMessageFilterKey::sender("ozone", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("sender inclusion non-matching")
        << QMessageFilterKey::sender("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender inclusion empty")
        << QMessageFilterKey::sender(QString(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender inclusion zero-length")
        << QMessageFilterKey::sender("", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender exclusion 1")
        << QMessageFilterKey::sender("example", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("sender exclusion 2")
        << QMessageFilterKey::sender("ozone", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("sender exclusion non-matching")
        << QMessageFilterKey::sender("Nonesuch", QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender exclusion empty")
        << QMessageFilterKey::sender(QString(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender exclusion zero-length")
        << QMessageFilterKey::sender("", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients equality 1")
        << QMessageFilterKey::recipients("Minion@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("recipients equality 2")
        << QMessageFilterKey::recipients("Important.Person@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("recipients equality non-matching")
        << QMessageFilterKey::recipients("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients equality empty")
        << QMessageFilterKey::recipients(QString(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients equality zero-length")
        << QMessageFilterKey::recipients("", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients inequality 1")
        << QMessageFilterKey::recipients("Minion@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("recipients inequality 2")
        << QMessageFilterKey::recipients("Important.Person@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("recipients inequality non-matching")
        << QMessageFilterKey::recipients("Nonesuch", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients inequality empty")
        << QMessageFilterKey::recipients(QString(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients inequality zero-length")
        << QMessageFilterKey::recipients("", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients inclusion 1")
        << QMessageFilterKey::recipients("example", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("recipients inclusion 2")
        << QMessageFilterKey::recipients("Mega", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("recipients inclusion non-matching")
        << QMessageFilterKey::recipients("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients inclusion empty")
        << QMessageFilterKey::recipients(QString(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients inclusion zero-length")
        << QMessageFilterKey::recipients("", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients exclusion 1")
        << QMessageFilterKey::recipients("example", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("recipients exclusion 2")
        << QMessageFilterKey::recipients("Mega", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("recipients exclusion non-matching")
        << QMessageFilterKey::recipients("Nonesuch", QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients exclusion empty")
        << QMessageFilterKey::recipients(QString(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients exclusion zero-length")
        << QMessageFilterKey::recipients("", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject equality 1")
        << QMessageFilterKey::subject("Free beer", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("subject equality 2")
        << QMessageFilterKey::subject("Scheduled maintenance", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("subject equality non-matching")
        << QMessageFilterKey::subject("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject equality empty")
        << QMessageFilterKey::subject(QString(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject equality zero-length")
        << QMessageFilterKey::subject("", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject inequality 1")
        << QMessageFilterKey::subject("Free beer", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("subject inequality 2")
        << QMessageFilterKey::subject("Scheduled maintenance", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] );

    QTest::newRow("subject inequality non-matching")
        << QMessageFilterKey::subject("Nonesuch", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inequality empty")
        << QMessageFilterKey::subject(QString(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inequality zero-length")
        << QMessageFilterKey::subject("", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inclusion 1")
        << QMessageFilterKey::subject("vation", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("subject inclusion 2")
        << QMessageFilterKey::subject(" ", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inclusion non-matching")
        << QMessageFilterKey::subject("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject inclusion empty")
        << QMessageFilterKey::subject(QString(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inclusion zero-length")
        << QMessageFilterKey::subject("", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject exclusion 1")
        << QMessageFilterKey::subject("vation", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("subject exclusion 2")
        << QMessageFilterKey::subject(" ", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject exclusion non-matching")
        << QMessageFilterKey::subject("Nonesuch", QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject exclusion empty")
        << QMessageFilterKey::subject(QString(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject exclusion zero-length")
        << QMessageFilterKey::subject("", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("timeStamp equality 1")
        << QMessageFilterKey::timeStamp(QDateTime::fromString("1999-04-01T10:30:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("timeStamp equality 2")
        << QMessageFilterKey::timeStamp(QDateTime::fromString("2000-01-01T13:00:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] );

    QTest::newRow("timeStamp equality non-matching")
        << QMessageFilterKey::timeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("timeStamp equality empty")
        << QMessageFilterKey::timeStamp(QDateTime(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("timeStamp inequality 1")
        << QMessageFilterKey::timeStamp(QDateTime::fromString("1999-04-01T10:30:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("timeStamp inequality 2")
        << QMessageFilterKey::timeStamp(QDateTime::fromString("2000-01-01T13:00:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] << messageIds[2] );

    QTest::newRow("timeStamp inequality non-matching")
        << QMessageFilterKey::timeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("timeStamp inequality empty")
        << QMessageFilterKey::timeStamp(QDateTime(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QDateTime epoch(QDateTime::fromString("2000-01-01T12:00:00Z", Qt::ISODate));

    QTest::newRow("timeStamp less than")
        << QMessageFilterKey::timeStamp(epoch, QMessageDataComparator::LessThan) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("timeStamp less than equal")
        << QMessageFilterKey::timeStamp(epoch, QMessageDataComparator::LessThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] << messageIds[3] );

    QTest::newRow("timeStamp greater than")
        << QMessageFilterKey::timeStamp(epoch, QMessageDataComparator::GreaterThan) 
        << ( QMessageIdList() << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] );

    QTest::newRow("timeStamp greater than equal")
        << QMessageFilterKey::timeStamp(epoch, QMessageDataComparator::GreaterThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("receptionTimeStamp equality 1")
        << QMessageFilterKey::receptionTimeStamp(QDateTime::fromString("1999-04-01T10:31:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("receptionTimeStamp equality 2")
        << QMessageFilterKey::receptionTimeStamp(QDateTime::fromString("2000-01-01T13:05:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("receptionTimeStamp equality non-matching")
        << QMessageFilterKey::receptionTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("receptionTimeStamp equality empty")
        << QMessageFilterKey::receptionTimeStamp(QDateTime(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("receptionTimeStamp inequality 1")
        << QMessageFilterKey::receptionTimeStamp(QDateTime::fromString("1999-04-01T10:31:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("receptionTimeStamp inequality 2")
        << QMessageFilterKey::receptionTimeStamp(QDateTime::fromString("2000-01-01T13:05:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("receptionTimeStamp inequality non-matching")
        << QMessageFilterKey::receptionTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("receptionTimeStamp inequality empty")
        << QMessageFilterKey::receptionTimeStamp(QDateTime(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("receptionTimeStamp less than")
        << QMessageFilterKey::receptionTimeStamp(epoch, QMessageDataComparator::LessThan) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("receptionTimeStamp less than equal")
        << QMessageFilterKey::receptionTimeStamp(epoch, QMessageDataComparator::LessThanEqual) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("receptionTimeStamp greater than")
        << QMessageFilterKey::receptionTimeStamp(epoch, QMessageDataComparator::GreaterThan) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("receptionTimeStamp greater than equal")
        << QMessageFilterKey::receptionTimeStamp(epoch, QMessageDataComparator::GreaterThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("status equality 1")
        << QMessageFilterKey::status(QMessage::Read, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("status equality 2")
        << QMessageFilterKey::status(QMessage::HasAttachments, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] );

    QTest::newRow("status equality 3")
        << QMessageFilterKey::status(QMessage::Removed, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("status inequality 1")
        << QMessageFilterKey::status(QMessage::Read, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("status inequality 2")
        << QMessageFilterKey::status(QMessage::HasAttachments, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("status inequality 3")
        << QMessageFilterKey::status(QMessage::Removed, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("status mask inclusion 1")
        << QMessageFilterKey::status(QMessage::Read, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("status mask inclusion 2")
        << QMessageFilterKey::status(QMessage::Read | QMessage::HasAttachments, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("status mask inclusion 3")
        << QMessageFilterKey::status(QMessage::Read | QMessage::Removed, QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("status mask inclusion empty")
        << QMessageFilterKey::status(static_cast<QMessage::StatusFlags>(0), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("status mask exclusion 1")
        << QMessageFilterKey::status(QMessage::Read, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("status mask exclusion 2")
        << QMessageFilterKey::status(QMessage::Read | QMessage::HasAttachments, QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("status mask exclusion 3")
        << QMessageFilterKey::status(QMessage::Read | QMessage::Removed, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("status mask exclusion empty")
        << QMessageFilterKey::status(static_cast<QMessage::StatusFlags>(0), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("priority equality 1")
        << QMessageFilterKey::priority(QMessage::High, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("priority equality 2")
        << QMessageFilterKey::priority(QMessage::Low, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("priority equality 3")
        << QMessageFilterKey::priority(QMessage::Normal, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("priority inequality 1")
        << QMessageFilterKey::priority(QMessage::High, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("priority inequality 2")
        << QMessageFilterKey::priority(QMessage::Low, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("priority inequality 3")
        << QMessageFilterKey::priority(QMessage::Normal, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] );

    QTest::newRow("size equality 1")
        << QMessageFilterKey::size(1056, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("size equality 2")
        << QMessageFilterKey::size(20480, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("size equality non-matching")
        << QMessageFilterKey::size(13, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("size equality zero")
        << QMessageFilterKey::size(0, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("size inequality 1")
        << QMessageFilterKey::size(1056, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] );

    QTest::newRow("size inequality 2")
        << QMessageFilterKey::size(20480, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("size inequality non-matching")
        << QMessageFilterKey::size(13, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("size inequality empty")
        << QMessageFilterKey::size(0, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    uint discriminator(10240);

    QTest::newRow("size less than")
        << QMessageFilterKey::size(discriminator, QMessageDataComparator::LessThan) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("size less than equal")
        << QMessageFilterKey::size(discriminator, QMessageDataComparator::LessThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("size greater than")
        << QMessageFilterKey::size(discriminator, QMessageDataComparator::GreaterThan) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("size greater than equal")
        << QMessageFilterKey::size(discriminator, QMessageDataComparator::GreaterThanEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    // Note: custom field negations do not work as expected!
    QTest::newRow("customField equality 1")
        << QMessageFilterKey::customField("flagged", "true", QMessageDataComparator::Equal)
        << ( QMessageIdList() << messageIds[0] << messageIds[1] )
        //<< ( QMessageIdList() << messageIds[2] << messageIds[3] << messageIds[4] );
        << QMessageIdList();

    QTest::newRow("customField equality 2")
        << QMessageFilterKey::customField("spam", "filter:yes", QMessageDataComparator::Equal)
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("customField equality empty")
        << QMessageFilterKey::customField("spam", "", QMessageDataComparator::Equal)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField equality non-matching key")
        << QMessageFilterKey::customField("hello", "world", QMessageDataComparator::Equal)
        << QMessageIdList()
        << QMessageIdList();

    QTest::newRow("customField equality non-matching value")
        << QMessageFilterKey::customField("spam", "spam spam spam", QMessageDataComparator::Equal)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField inequality 1")
        << QMessageFilterKey::customField("flagged", "true", QMessageDataComparator::NotEqual)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[0] << messageIds[1] );

    QTest::newRow("customField inequality 2")
        << QMessageFilterKey::customField("spam", "filter:yes", QMessageDataComparator::NotEqual)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("customField inequality empty")
        << QMessageFilterKey::customField("spam", "", QMessageDataComparator::NotEqual)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("customField inequality non-matching key")
        << QMessageFilterKey::customField("hello", "world", QMessageDataComparator::NotEqual)
        << QMessageIdList()
        << QMessageIdList();

    QTest::newRow("customField inequality non-matching value")
        << QMessageFilterKey::customField("spam", "spam spam spam", QMessageDataComparator::NotEqual)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("customField inclusion 1")
        << QMessageFilterKey::customField("flagged", "ru", QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[0] << messageIds[1] )
        << QMessageIdList();

    QTest::newRow("customField inclusion 2")
        << QMessageFilterKey::customField("spam", "filter", QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("customField inclusion empty")
        << QMessageFilterKey::customField("spam", "", QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("customField inclusion non-matching key")
        << QMessageFilterKey::customField("hello", "world", QMessageDataComparator::Includes)
        << QMessageIdList()
        << QMessageIdList();

    QTest::newRow("customField inclusion non-matching value")
        << QMessageFilterKey::customField("spam", "spam spam spam", QMessageDataComparator::Includes)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField exclusion 1")
        << QMessageFilterKey::customField("flagged", "ru", QMessageDataComparator::Excludes)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[0] << messageIds[1] );

    QTest::newRow("customField exclusion 2")
        << QMessageFilterKey::customField("spam", "filter", QMessageDataComparator::Excludes)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField exclusion empty")
        << QMessageFilterKey::customField("spam", "", QMessageDataComparator::Excludes)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField exclusion non-matching key")
        << QMessageFilterKey::customField("hello", "world", QMessageDataComparator::Excludes)
        << QMessageIdList()
        << QMessageIdList();

    QTest::newRow("customField exclusion non-matching value")
        << QMessageFilterKey::customField("spam", "spam spam spam", QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("parentAccountId equality 1")
        << QMessageFilterKey::parentAccountId(accountIds[0], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentAccountId equality 2")
        << QMessageFilterKey::parentAccountId(accountIds[2], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentAccountId equality invalid")
        << QMessageFilterKey::parentAccountId(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentAccountId inequality 1")
        << QMessageFilterKey::parentAccountId(accountIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentAccountId inequality 2")
        << QMessageFilterKey::parentAccountId(accountIds[2], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentAccountId inequality invalid")
        << QMessageFilterKey::parentAccountId(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentAccountId filter inclusion 1")
        << QMessageFilterKey::parentAccountId(QMessageAccountFilterKey::name("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentAccountId filter inclusion 2")
        << QMessageFilterKey::parentAccountId(QMessageAccountFilterKey::name("Work", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentAccountId filter inclusion 3")
        << QMessageFilterKey::parentAccountId(QMessageAccountFilterKey::name("o", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentAccountId filter inclusion empty")
        << QMessageFilterKey::parentAccountId(QMessageAccountFilterKey(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentAccountId filter inclusion non-matching")
        << QMessageFilterKey::parentAccountId(QMessageAccountFilterKey::name("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentAccountId filter exclusion 1")
        << QMessageFilterKey::parentAccountId(QMessageAccountFilterKey::name("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentAccountId filter exclusion 2")
        << QMessageFilterKey::parentAccountId(QMessageAccountFilterKey::name("Work", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentAccountId filter exclusion 3")
        << QMessageFilterKey::parentAccountId(QMessageAccountFilterKey::name("o", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentAccountId filter exclusion empty")
        << QMessageFilterKey::parentAccountId(QMessageAccountFilterKey(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentAccountId filter exclusion non-matching")
        << QMessageFilterKey::parentAccountId(QMessageAccountFilterKey::name("NoneSuch"), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();


#ifdef QMESSAGING_OPTIONAL_FOLDER
    QTest::newRow("parentFolderId equality 1")
        << QMessageFilterKey::parentFolderId(folderIds[0], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId equality 2")
        << QMessageFilterKey::parentFolderId(folderIds[1], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId equality invalid")
        << QMessageFilterKey::parentFolderId(QMessageFolderId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentFolderId inequality 1")
        << QMessageFilterKey::parentFolderId(folderIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentFolderId inequality 2")
        << QMessageFilterKey::parentFolderId(folderIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("parentFolderId inequality invalid")
        << QMessageFilterKey::parentFolderId(QMessageFolderId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentFolderId filter inclusion 1")
        << QMessageFilterKey::parentFolderId(QMessageFolderFilterKey::path("[Root]", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId filter inclusion 2")
        << QMessageFilterKey::parentFolderId(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId filter inclusion 3")
        << QMessageFilterKey::parentFolderId(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentFolderId filter inclusion empty")
        << QMessageFilterKey::parentFolderId(QMessageFolderFilterKey(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentFolderId filter inclusion non-matching")
        << QMessageFilterKey::parentFolderId(QMessageFolderFilterKey::path("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentFolderId filter exclusion 1")
        << QMessageFilterKey::parentFolderId(QMessageFolderFilterKey::path("[Root]", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentFolderId filter exclusion 2")
        << QMessageFilterKey::parentFolderId(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("parentFolderId filter exclusion 3")
        << QMessageFilterKey::parentFolderId(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId filter exclusion empty")
        << QMessageFilterKey::parentFolderId(QMessageFolderFilterKey(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentFolderId filter exclusion non-matching")
        << QMessageFilterKey::parentFolderId(QMessageFolderFilterKey::path("NoneSuch"), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();
    QTest::newRow("ancestorFolderIds inclusion 1")
        << QMessageFilterKey::ancestorFolderIds(folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] );

    QTest::newRow("ancestorFolderIds inclusion 2")
        << QMessageFilterKey::ancestorFolderIds(folderIds[2], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("ancestorFolderIds inclusion invalid")
        << QMessageFilterKey::ancestorFolderIds(QMessageFolderId(), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("ancestorFolderIds exclusion 1")
        << QMessageFilterKey::ancestorFolderIds(folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] );

    QTest::newRow("ancestorFolderIds exclusion 2")
        << QMessageFilterKey::ancestorFolderIds(folderIds[2], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("ancestorFolderIds exclusion invalid")
        << QMessageFilterKey::ancestorFolderIds(QMessageFolderId(), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("ancestorFolderIds filter inclusion 1")
        << QMessageFilterKey::ancestorFolderIds(QMessageFolderFilterKey::displayName("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] );

    QTest::newRow("ancestorFolderIds filter inclusion 2")
        << QMessageFilterKey::ancestorFolderIds(QMessageFolderFilterKey::displayName("X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("ancestorFolderIds filter inclusion 3")
        << QMessageFilterKey::ancestorFolderIds(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] );

    QTest::newRow("ancestorFolderIds filter inclusion empty")
        << QMessageFilterKey::ancestorFolderIds(QMessageFolderFilterKey(), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] );

    QTest::newRow("ancestorFolderIds filter inclusion non-matching")
        << QMessageFilterKey::ancestorFolderIds(QMessageFolderFilterKey::path("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("ancestorFolderIds filter exclusion 1")
        << QMessageFilterKey::ancestorFolderIds(QMessageFolderFilterKey::displayName("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] );

    QTest::newRow("ancestorFolderIds filter exclusion 2")
        << QMessageFilterKey::ancestorFolderIds(QMessageFolderFilterKey::displayName("X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("ancestorFolderIds filter exclusion 3")
        << QMessageFilterKey::ancestorFolderIds(QMessageFolderFilterKey::path("/Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] );

    QTest::newRow("ancestorFolderIds filter exclusion empty")
        << QMessageFilterKey::ancestorFolderIds(QMessageFolderFilterKey(), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] );

    QTest::newRow("ancestorFolderIds filter exclusion non-matching")
        << QMessageFilterKey::ancestorFolderIds(QMessageFolderFilterKey::path("NoneSuch"), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();
#endif
}

void tst_QMessageStoreKeys::testMessageFilterKey()
{
    QFETCH(QMessageFilterKey, key);
    QFETCH(QMessageIdList, ids);
    QFETCH(QMessageIdList, negatedIds);

    // Order is irrelevant for filtering
    QCOMPARE(QMessageStore::instance()->queryMessages(key).toSet(), ids.toSet());
    QCOMPARE(QMessageStore::instance()->queryMessages(~key).toSet(), negatedIds.toSet());
}

void tst_QMessageStoreKeys::testMessageSortKey_data()
{
    QTest::addColumn<QMessageSortKey>("sortKey");
    QTest::addColumn<MessageListList>("ids");

    QTest::newRow("type ascending")
        << QMessageSortKey::type(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] ) );

    QTest::newRow("type descending")
        << QMessageSortKey::type(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("sender ascending")
        << QMessageSortKey::sender(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] ) );

    QTest::newRow("sender descending")
        << QMessageSortKey::sender(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] << messageIds[4] ) );

    QTest::newRow("recipients ascending")
        << QMessageSortKey::recipients(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] << messageIds[4] ) );

    QTest::newRow("recipients descending")
        << QMessageSortKey::recipients(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] ) );

    QTest::newRow("subject ascending")
        << QMessageSortKey::subject(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] ) );

    QTest::newRow("subject descending")
        << QMessageSortKey::subject(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("timeStamp ascending")
        << QMessageSortKey::timeStamp(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] << messageIds[3] ) );

    QTest::newRow("timeStamp descending")
        << QMessageSortKey::timeStamp(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[1] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("receptionTimeStamp ascending")
        << QMessageSortKey::receptionTimeStamp(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] ) );

    QTest::newRow("receptionTimeStamp descending")
        << QMessageSortKey::receptionTimeStamp(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("priority ascending")
        << QMessageSortKey::priority(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] ) );

    QTest::newRow("priority descending")
        << QMessageSortKey::priority(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("size ascending")
        << QMessageSortKey::size(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] ) );

    QTest::newRow("size descending")
        << QMessageSortKey::size(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type ascending, priority ascending, size ascending")
        << QMessageSortKey::type(Qt::AscendingOrder) + QMessageSortKey::priority(Qt::AscendingOrder) + QMessageSortKey::size(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] ) );

    QTest::newRow("type ascending, priority ascending, size descending")
        << QMessageSortKey::type(Qt::AscendingOrder) + QMessageSortKey::priority(Qt::AscendingOrder) + QMessageSortKey::size(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] ) );

    QTest::newRow("type ascending, priority descending, size ascending")
        << QMessageSortKey::type(Qt::AscendingOrder) + QMessageSortKey::priority(Qt::DescendingOrder) + QMessageSortKey::size(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("type ascending, priority descending, size descending")
        << QMessageSortKey::type(Qt::AscendingOrder) + QMessageSortKey::priority(Qt::DescendingOrder) + QMessageSortKey::size(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("type descending, priority ascending, size ascending")
        << QMessageSortKey::type(Qt::DescendingOrder) + QMessageSortKey::priority(Qt::AscendingOrder) + QMessageSortKey::size(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority ascending, size descending")
        << QMessageSortKey::type(Qt::DescendingOrder) + QMessageSortKey::priority(Qt::AscendingOrder) + QMessageSortKey::size(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority descending, size ascending")
        << QMessageSortKey::type(Qt::DescendingOrder) + QMessageSortKey::priority(Qt::DescendingOrder) + QMessageSortKey::size(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority descending, size descending")
        << QMessageSortKey::type(Qt::DescendingOrder) + QMessageSortKey::priority(Qt::DescendingOrder) + QMessageSortKey::size(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );
}


void tst_QMessageStoreKeys::testMessageSortKey()
{
    QFETCH(QMessageSortKey, sortKey);
    QFETCH(MessageListList, ids);

    QMessageIdList x(QMessageStore::instance()->queryMessages(QMessageFilterKey(), sortKey));
    QMessageIdList::const_iterator xit = x.begin();

    MessageListList::const_iterator iit = ids.begin();

    bool match(true);
    for (int groupCount = 0; xit != x.end(); ++xit) {
        if (iit == ids.end()) {
            match = false;
            break;
        } else {
            if ((*iit).contains(*xit)) {
                ++groupCount;
                if (groupCount == (*iit).count()) {
                    // We have matched all members of this group
                    ++iit;
                    groupCount = 0;
                }
            } else {
                match = false;
                break;
            }
        }
    }
    QVERIFY(match);
    QVERIFY(iit == ids.end());
}

