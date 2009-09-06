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
    QSet<QMessageAccountId> existingAccountIds;
    QMessageAccountIdList accountIds;

#ifdef QMESSAGING_OPTIONAL_FOLDER
    QSet<QMessageFolderId> existingFolderIds;
    QMessageFolderIdList folderIds;
#endif

    QSet<QMessageId> existingMessageIds;
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

    existingAccountIds = QMessageStore::instance()->queryAccounts().toSet();

    QList<Support::Parameters> accountParams;

    accountParams << Params()("name", "Work")
                             ("fromAddress", "Important.Person@example.com")
                  << Params()("name", "Personal")
                             ("fromAddress", "superstar2000@example.net")
                  << Params()("name", "Alter Ego")
                             ("fromAddress", "UltraMegaLightningBabe@superhero.test");

    foreach (const Support::Parameters &params, accountParams) {
        accountIds.append(Support::addAccount(params));
        QVERIFY(accountIds.last().isValid());
    }

#ifdef QMESSAGING_OPTIONAL_FOLDER
    existingFolderIds = QMessageStore::instance()->queryFolders().toSet();

    QList<Support::Parameters> folderParams;
    folderParams << Params()("parentAccountName", "Alter Ego")
                            ("path", "My messages")
                            ("displayName", "My messages")
                            ("parentFolderPath", "")
                 << Params()("parentAccountName", "Work")
                            ("path", "Inbox")
                            ("displayName", "Inbox")
                            ("parentFolderPath", "")
                 << Params()("parentAccountName", "Work")
                            ("path", "Inbox/X-Announce")
                            ("displayName", "X-Announce")
                            ("parentFolderPath", "Inbox")
                 << Params()("parentAccountName", "Work")
                            ("path", "Inbox/X-Announce/X-Archived")
                            ("displayName", "X-Archived")
                            ("parentFolderPath", "Inbox/X-Announce");

    foreach (const Support::Parameters &params, folderParams) {
        folderIds.append(Support::addFolder(params));
        QVERIFY(folderIds.last().isValid());
    }
#endif

    existingMessageIds = QMessageStore::instance()->queryMessages().toSet();

    QList<Support::Parameters> messageParams;
    messageParams << Params()("parentAccountName", "Alter Ego")
                             ("parentFolderPath", "My messages")
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
                             ("parentFolderPath", "Inbox")
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
                             ("parentFolderPath", "Inbox")
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
                             ("parentFolderPath", "Inbox/X-Announce")
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
                             ("parentFolderPath", "Inbox/X-Announce/X-Archived")
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
        << QMessageAccountFilterKey::byId(accountIds[0], QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("id equality 2")
        << QMessageAccountFilterKey::byId(accountIds[1], QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("id equality invalid")
        << QMessageAccountFilterKey::byId(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id inequality 1")
        << QMessageAccountFilterKey::byId(accountIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id inequality 2")
        << QMessageAccountFilterKey::byId(accountIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id inequality invalid")
        << QMessageAccountFilterKey::byId(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id list inclusion 1")
        << QMessageAccountFilterKey::byId(QMessageAccountIdList() << accountIds[0], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("id list inclusion 2")
        << QMessageAccountFilterKey::byId(QMessageAccountIdList() << accountIds[1], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("id list inclusion 3")
        << QMessageAccountFilterKey::byId(QMessageAccountIdList() << accountIds[0] << accountIds[1], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[2] );

    QTest::newRow("id list inclusion empty")
        << QMessageAccountFilterKey::byId(QMessageAccountIdList(), QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id list exclusion 1")
        << QMessageAccountFilterKey::byId(QMessageAccountIdList() << accountIds[0], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id list exclusion 2")
        << QMessageAccountFilterKey::byId(QMessageAccountIdList() << accountIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id list exclusion 3")
        << QMessageAccountFilterKey::byId(QMessageAccountIdList() << accountIds[0] << accountIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] );

    QTest::newRow("id list exclusion empty")
        << QMessageAccountFilterKey::byId(QMessageAccountIdList(), QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion 1")
        << QMessageAccountFilterKey::byId(QMessageAccountFilterKey::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("id filter inclusion 2")
        << QMessageAccountFilterKey::byId(QMessageAccountFilterKey::byName("Personal", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("id filter inclusion 3")
        << QMessageAccountFilterKey::byId(QMessageAccountFilterKey::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion empty")
        << QMessageAccountFilterKey::byId(QMessageAccountFilterKey(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion non-matching")
        << QMessageAccountFilterKey::byId(QMessageAccountFilterKey::byName("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion 1")
        << QMessageAccountFilterKey::byId(QMessageAccountFilterKey::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id filter exclusion 2")
        << QMessageAccountFilterKey::byId(QMessageAccountFilterKey::byName("Personal", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id filter exclusion 3")
        << QMessageAccountFilterKey::byId(QMessageAccountFilterKey::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion empty")
        << QMessageAccountFilterKey::byId(QMessageAccountFilterKey(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion non-matching")
        << QMessageAccountFilterKey::byId(QMessageAccountFilterKey::byName("NoneSuch"), QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name equality 1")
        << QMessageAccountFilterKey::byName("Work", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("name equality 2")
        << QMessageAccountFilterKey::byName("Personal", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("name equality non-matching")
        << QMessageAccountFilterKey::byName("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name equality empty")
        << QMessageAccountFilterKey::byName(QString(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name equality zero-length")
        << QMessageAccountFilterKey::byName("", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name inequality 1")
        << QMessageAccountFilterKey::byName("Work", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("name inequality 2")
        << QMessageAccountFilterKey::byName("Personal", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("name inequality non-matching")
        << QMessageAccountFilterKey::byName("Nonesuch", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inequality empty")
        << QMessageAccountFilterKey::byName(QString(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inequality zero-length")
        << QMessageAccountFilterKey::byName("", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion 1")
        << QMessageAccountFilterKey::byName("ork", QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("name inclusion 2")
        << QMessageAccountFilterKey::byName("o", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion non-matching")
        << QMessageAccountFilterKey::byName("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name inclusion empty")
        << QMessageAccountFilterKey::byName(QString(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion zero-length")
        << QMessageAccountFilterKey::byName("", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name exclusion 1")
        << QMessageAccountFilterKey::byName("ork", QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("name exclusion 2")
        << QMessageAccountFilterKey::byName("o", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name exclusion non-matching")
        << QMessageAccountFilterKey::byName("Nonesuch", QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name exclusion empty")
        << QMessageAccountFilterKey::byName(QString(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name exclusion zero-length")
        << QMessageAccountFilterKey::byName("", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress equality 1")
        << QMessageAccountFilterKey::byFromAddress("Important.Person@example.com", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("fromAddress equality 2")
        << QMessageAccountFilterKey::byFromAddress("superstar2000@example.net", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("fromAddress equality non-matching")
        << QMessageAccountFilterKey::byFromAddress("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress equality empty")
        << QMessageAccountFilterKey::byFromAddress(QString(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress equality zero-length")
        << QMessageAccountFilterKey::byFromAddress("", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress inequality 1")
        << QMessageAccountFilterKey::byFromAddress("Important.Person@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("fromAddress inequality 2")
        << QMessageAccountFilterKey::byFromAddress("superstar2000@example.net", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("fromAddress inequality non-matching")
        << QMessageAccountFilterKey::byFromAddress("Nonesuch", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inequality empty")
        << QMessageAccountFilterKey::byFromAddress(QString(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inequality zero-length")
        << QMessageAccountFilterKey::byFromAddress("", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inclusion 1")
        << QMessageAccountFilterKey::byFromAddress("Import", QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("fromAddress inclusion 2")
        << QMessageAccountFilterKey::byFromAddress("@example.", QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[2] );

    QTest::newRow("fromAddress inclusion non-matching")
        << QMessageAccountFilterKey::byFromAddress("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress inclusion empty")
        << QMessageAccountFilterKey::byFromAddress(QString(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress inclusion zero-length")
        << QMessageAccountFilterKey::byFromAddress("", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress exclusion 1")
        << QMessageAccountFilterKey::byFromAddress("Import", QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("fromAddress exclusion 2")
        << QMessageAccountFilterKey::byFromAddress("@example.", QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] );

    QTest::newRow("fromAddress exclusion non-matching")
        << QMessageAccountFilterKey::byFromAddress("Nonesuch", QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("fromAddress exclusion empty")
        << QMessageAccountFilterKey::byFromAddress(QString(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("fromAddress exclusion zero-length")
        << QMessageAccountFilterKey::byFromAddress("", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;
}

void tst_QMessageStoreKeys::testAccountFilterKey()
{
    QFETCH(QMessageAccountFilterKey, key);
    QFETCH(QMessageAccountIdList, ids);
    QFETCH(QMessageAccountIdList, negatedIds);

    if (key.isSupported()) {
        // Order is irrelevant for filtering
        QCOMPARE(QMessageStore::instance()->queryAccounts(key).toSet().subtract(existingAccountIds), ids.toSet());
        QCOMPARE(QMessageStore::instance()->queryAccounts(~key).toSet().subtract(existingAccountIds), negatedIds.toSet());
    }
}

void tst_QMessageStoreKeys::testAccountSortKey_data()
{
    QTest::addColumn<QMessageAccountSortKey>("sortKey");
    QTest::addColumn<QMessageAccountIdList>("ids");

    QTest::newRow("name ascending")
        << QMessageAccountSortKey::byName(Qt::AscendingOrder)
        << ( QMessageAccountIdList() << accountIds[2] << accountIds[1] << accountIds[0] );

    QTest::newRow("name descending")
        << QMessageAccountSortKey::byName(Qt::DescendingOrder)
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] << accountIds[2] );
}

void tst_QMessageStoreKeys::testAccountSortKey()
{
    QFETCH(QMessageAccountSortKey, sortKey);
    QFETCH(QMessageAccountIdList, ids);

    // Filter out the existing accounts
    QMessageAccountIdList sortedIds(QMessageStore::instance()->queryAccounts(QMessageAccountFilterKey(), sortKey));
    for (QMessageAccountIdList::iterator it = sortedIds.begin(); it != sortedIds.end(); ) {
        if (existingAccountIds.contains(*it)) {
            it = sortedIds.erase(it);
        } else {
            ++it;
        }
    }

    QCOMPARE(sortedIds, ids);
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
void tst_QMessageStoreKeys::testFolderFilterKey_data()
{
    QTest::addColumn<QMessageFolderFilterKey>("key");
    QTest::addColumn<QMessageFolderIdList>("ids");
    QTest::addColumn<QMessageFolderIdList>("negatedIds");

    QTest::newRow("empty key")
        << QMessageFolderFilterKey() 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("id equality 1")
        << QMessageFolderFilterKey::byId(folderIds[0], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("id equality 2")
        << QMessageFolderFilterKey::byId(folderIds[1], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] );

    QTest::newRow("id equality invalid")
        << QMessageFolderFilterKey::byId(QMessageFolderId(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("id inequality 1")
        << QMessageFolderFilterKey::byId(folderIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("id inequality 2")
        << QMessageFolderFilterKey::byId(folderIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[1] );

    QTest::newRow("id inequality invalid")
        << QMessageFolderFilterKey::byId(QMessageFolderId(), QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("id list inclusion 1")
        << QMessageFolderFilterKey::byId(QMessageFolderIdList() << folderIds[0], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("id list inclusion 2")
        << QMessageFolderFilterKey::byId(QMessageFolderIdList() << folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] );

    QTest::newRow("id list inclusion 3")
        << QMessageFolderFilterKey::byId(QMessageFolderIdList() << folderIds[0] << folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("id list inclusion empty")
        << QMessageFolderFilterKey::byId(QMessageFolderIdList(), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("id list exclusion 1")
        << QMessageFolderFilterKey::byId(QMessageFolderIdList() << folderIds[0], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("id list exclusion 2")
        << QMessageFolderFilterKey::byId(QMessageFolderIdList() << folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[1] );

    QTest::newRow("id list exclusion 3")
        << QMessageFolderFilterKey::byId(QMessageFolderIdList() << folderIds[0] << folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("id list exclusion empty")
        << QMessageFolderFilterKey::byId(QMessageFolderIdList(), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("id filter inclusion 1")
        << QMessageFolderFilterKey::byId(QMessageFolderFilterKey::byPath("My messages", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("id filter inclusion 2")
        << QMessageFolderFilterKey::byId(QMessageFolderFilterKey::byPath("Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("id filter inclusion 3")
        << QMessageFolderFilterKey::byId(QMessageFolderFilterKey::byPath("box/X-Ann", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("id filter inclusion empty")
        << QMessageFolderFilterKey::byId(QMessageFolderFilterKey(), QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("id filter inclusion non-matching")
        << QMessageFolderFilterKey::byId(QMessageFolderFilterKey::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("id filter exclusion 1")
        << QMessageFolderFilterKey::byId(QMessageFolderFilterKey::byPath("My messages", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("id filter exclusion 2")
        << QMessageFolderFilterKey::byId(QMessageFolderFilterKey::byPath("Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("id filter exclusion 3")
        << QMessageFolderFilterKey::byId(QMessageFolderFilterKey::byPath("box/X-Ann", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("id filter exclusion empty")
        << QMessageFolderFilterKey::byId(QMessageFolderFilterKey(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("id filter exclusion non-matching")
        << QMessageFolderFilterKey::byId(QMessageFolderFilterKey::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path equality 1")
        << QMessageFolderFilterKey::byPath("My messages", QMessageDataComparator::Equal)
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("path equality 2")
        << QMessageFolderFilterKey::byPath("Inbox/X-Announce", QMessageDataComparator::Equal)
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("path equality non-matching")
        << QMessageFolderFilterKey::byPath("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path equality empty")
        << QMessageFolderFilterKey::byPath(QString(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path equality zero-length")
        << QMessageFolderFilterKey::byPath("", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path inequality 1")
        << QMessageFolderFilterKey::byPath("My messages", QMessageDataComparator::NotEqual)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("path inequality 2")
        << QMessageFolderFilterKey::byPath("Inbox/X-Announce", QMessageDataComparator::NotEqual)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("path inequality non-matching")
        << QMessageFolderFilterKey::byPath("Nonesuch", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path inequality empty")
        << QMessageFolderFilterKey::byPath(QString(), QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path inequality zero-length")
        << QMessageFolderFilterKey::byPath("", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path inclusion 1")
        << QMessageFolderFilterKey::byPath("ess", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("path inclusion 2")
        << QMessageFolderFilterKey::byPath("box/X-Ann", QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("path inclusion non-matching")
        << QMessageFolderFilterKey::byPath("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path inclusion empty")
        << QMessageFolderFilterKey::byPath(QString(), QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path inclusion zero-length")
        << QMessageFolderFilterKey::byPath("", QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path exclusion 1")
        << QMessageFolderFilterKey::byPath("ess", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("path exclusion 2")
        << QMessageFolderFilterKey::byPath("box/X-Ann", QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("path exclusion non-matching")
        << QMessageFolderFilterKey::byPath("Nonesuch", QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path exclusion empty")
        << QMessageFolderFilterKey::byPath(QString(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path exclusion zero-length")
        << QMessageFolderFilterKey::byPath("", QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName equality 1")
        << QMessageFolderFilterKey::byDisplayName("My messages", QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName equality 2")
        << QMessageFolderFilterKey::byDisplayName("X-Announce", QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("displayName equality non-matching")
        << QMessageFolderFilterKey::byDisplayName("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName equality empty")
        << QMessageFolderFilterKey::byDisplayName(QString(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName equality zero-length")
        << QMessageFolderFilterKey::byDisplayName("", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName inequality 1")
        << QMessageFolderFilterKey::byDisplayName("My messages", QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("displayName inequality 2")
        << QMessageFolderFilterKey::byDisplayName("X-Announce", QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("displayName inequality non-matching")
        << QMessageFolderFilterKey::byDisplayName("Nonesuch", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inequality empty")
        << QMessageFolderFilterKey::byDisplayName(QString(), QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inequality zero-length")
        << QMessageFolderFilterKey::byDisplayName("", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inclusion 1")
        << QMessageFolderFilterKey::byDisplayName("message", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName inclusion 2")
        << QMessageFolderFilterKey::byDisplayName("X-A", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("displayName inclusion non-matching")
        << QMessageFolderFilterKey::byDisplayName("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName inclusion empty")
        << QMessageFolderFilterKey::byDisplayName(QString(), QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inclusion zero-length")
        << QMessageFolderFilterKey::byDisplayName("", QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName exclusion 1")
        << QMessageFolderFilterKey::byDisplayName("messages", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("displayName exclusion 2")
        << QMessageFolderFilterKey::byDisplayName("X-A", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName exclusion non-matching")
        << QMessageFolderFilterKey::byDisplayName("Nonesuch", QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName exclusion empty")
        << QMessageFolderFilterKey::byDisplayName(QString(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName exclusion zero-length")
        << QMessageFolderFilterKey::byDisplayName("", QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId equality 1")
        << QMessageFolderFilterKey::byParentAccountId(accountIds[0], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId equality 2")
        << QMessageFolderFilterKey::byParentAccountId(accountIds[2], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId equality invalid")
        << QMessageFolderFilterKey::byParentAccountId(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId inequality 1")
        << QMessageFolderFilterKey::byParentAccountId(accountIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId inequality 2")
        << QMessageFolderFilterKey::byParentAccountId(accountIds[2], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId inequality invalid")
        << QMessageFolderFilterKey::byParentAccountId(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("parentAccountId filter inclusion 1")
        << QMessageFolderFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId filter inclusion 2")
        << QMessageFolderFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId filter inclusion 3")
        << QMessageFolderFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("parentAccountId filter inclusion empty")
        << QMessageFolderFilterKey::byParentAccountId(QMessageAccountFilterKey(), QMessageDataComparator::Includes) 
        << folderIds 
        << QMessageFolderIdList();

    QTest::newRow("parentAccountId filter inclusion non-matching")
        << QMessageFolderFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId filter exclusion 1")
        << QMessageFolderFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId filter exclusion 2")
        << QMessageFolderFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId filter exclusion 3")
        << QMessageFolderFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId filter exclusion empty")
        << QMessageFolderFilterKey::byParentAccountId(QMessageAccountFilterKey(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId filter exclusion non-matching")
        << QMessageFolderFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("parentFolderId equality 1")
        << QMessageFolderFilterKey::byParentFolderId(folderIds[1], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("parentFolderId equality 2")
        << QMessageFolderFilterKey::byParentFolderId(folderIds[2], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] );

    QTest::newRow("parentFolderId equality invalid")
        << QMessageFolderFilterKey::byParentFolderId(QMessageFolderId(), QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("parentFolderId inequality 1")
        << QMessageFolderFilterKey::byParentFolderId(folderIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("parentFolderId inequality 2")
        << QMessageFolderFilterKey::byParentFolderId(folderIds[2], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("parentFolderId inequality invalid")
        << QMessageFolderFilterKey::byParentFolderId(QMessageFolderId(), QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("parentFolderId filter inclusion 1")
        << QMessageFolderFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] );

    QTest::newRow("parentFolderId filter inclusion 2")
        << QMessageFolderFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("parentFolderId filter inclusion 3")
        << QMessageFolderFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("parentFolderId filter inclusion empty")
        << QMessageFolderFilterKey::byParentFolderId(QMessageFolderFilterKey(), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("parentFolderId filter inclusion non-matching")
        << QMessageFolderFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentFolderId filter exclusion 1")
        << QMessageFolderFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("parentFolderId filter exclusion 2")
        << QMessageFolderFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("parentFolderId filter exclusion 3")
        << QMessageFolderFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("parentFolderId filter exclusion empty")
        << QMessageFolderFilterKey::byParentFolderId(QMessageFolderFilterKey(), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("parentFolderId filter exclusion non-matching")
        << QMessageFolderFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("ancestorFolderIds list inclusion 1")
        << QMessageFolderFilterKey::byAncestorFolderIds(folderIds[2], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] );

    QTest::newRow("ancestorFolderIds list inclusion 2")
        << QMessageFolderFilterKey::byAncestorFolderIds(folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("ancestorFolderIds list inclusion invalid")
        << QMessageFolderFilterKey::byAncestorFolderIds(QMessageFolderId(), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("ancestorFolderIds list exclusion 1")
        << QMessageFolderFilterKey::byAncestorFolderIds(folderIds[2], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("ancestorFolderIds list exclusion 2")
        << QMessageFolderFilterKey::byAncestorFolderIds(folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds list exclusion invalid")
        << QMessageFolderFilterKey::byAncestorFolderIds(QMessageFolderId(), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("ancestorFolderIds filter inclusion 1")
        << QMessageFolderFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byPath("Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] );

    QTest::newRow("ancestorFolderIds filter inclusion 2")
        << QMessageFolderFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("ancestorFolderIds filter inclusion 3")
        << QMessageFolderFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("ancestorFolderIds filter inclusion empty")
        << QMessageFolderFilterKey::byAncestorFolderIds(QMessageFolderFilterKey(), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("ancestorFolderIds filter inclusion non-matching")
        << QMessageFolderFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("ancestorFolderIds filter exclusion 1")
        << QMessageFolderFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byPath("Inbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion 2")
        << QMessageFolderFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion 3")
        << QMessageFolderFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion empty")
        << QMessageFolderFilterKey::byAncestorFolderIds(QMessageFolderFilterKey(), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion non-matching")
        << QMessageFolderFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();
}

void tst_QMessageStoreKeys::testFolderFilterKey()
{
    QFETCH(QMessageFolderFilterKey, key);
    QFETCH(QMessageFolderIdList, ids);
    QFETCH(QMessageFolderIdList, negatedIds);

    if (key.isSupported()) {
        // Order is irrelevant for filtering
        QCOMPARE(QMessageStore::instance()->queryFolders(key).toSet().subtract(existingFolderIds), ids.toSet());
        QCOMPARE(QMessageStore::instance()->queryFolders(~key).toSet().subtract(existingFolderIds), negatedIds.toSet());
    }
}

void tst_QMessageStoreKeys::testFolderSortKey_data()
{
    QTest::addColumn<QMessageFolderSortKey>("sortKey");
    QTest::addColumn<QMessageFolderIdList>("ids");

    QTest::newRow("path ascending")
        << QMessageFolderSortKey::byPath(Qt::AscendingOrder)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] << folderIds[0] );

    QTest::newRow("path descending")
        << QMessageFolderSortKey::byPath(Qt::DescendingOrder)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[3] << folderIds[2] << folderIds[1] );

    QTest::newRow("displayName ascending")
        << QMessageFolderSortKey::byDisplayName(Qt::AscendingOrder)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[0] << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName descending")
        << QMessageFolderSortKey::byDisplayName(Qt::DescendingOrder)
        << ( QMessageFolderIdList() << folderIds[3] << folderIds[2] << folderIds[0] << folderIds[1] );
}

void tst_QMessageStoreKeys::testFolderSortKey()
{
    QFETCH(QMessageFolderSortKey, sortKey);
    QFETCH(QMessageFolderIdList, ids);

    // Filter out the existing folders
    QMessageFolderIdList sortedIds(QMessageStore::instance()->queryFolders(QMessageFolderFilterKey(), sortKey));
    for (QMessageFolderIdList::iterator it = sortedIds.begin(); it != sortedIds.end(); ) {
        if (existingFolderIds.contains(*it)) {
            it = sortedIds.erase(it);
        } else {
            ++it;
        }
    }

    QCOMPARE(sortedIds, ids);
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
        << QMessageFilterKey::byId(messageIds[0], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id equality 2")
        << QMessageFilterKey::byId(messageIds[1], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] )
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id equality invalid")
        << QMessageFilterKey::byId(QMessageId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("id inequality 1")
        << QMessageFilterKey::byId(messageIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("id inequality 2")
        << QMessageFilterKey::byId(messageIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] );

    QTest::newRow("id inequality invalid")
        << QMessageFilterKey::byId(QMessageId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("id list inclusion 1")
        << QMessageFilterKey::byId(QMessageIdList() << messageIds[0], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id list inclusion 2")
        << QMessageFilterKey::byId(QMessageIdList() << messageIds[1], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] )
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id list inclusion 3")
        << QMessageFilterKey::byId(QMessageIdList() << messageIds[0] << messageIds[1], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] )
        << ( QMessageIdList() << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("id list inclusion empty")
        << QMessageFilterKey::byId(QMessageIdList(), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("id list exclusion 1")
        << QMessageFilterKey::byId(QMessageIdList() << messageIds[0], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("id list exclusion 2")
        << QMessageFilterKey::byId(QMessageIdList() << messageIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] );

    QTest::newRow("id list exclusion 3")
        << QMessageFilterKey::byId(QMessageIdList() << messageIds[0] << messageIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] );

    QTest::newRow("id list exclusion empty")
        << QMessageFilterKey::byId(QMessageIdList(), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("type equality 1")
        << QMessageFilterKey::byType(QMessage::Sms, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("type equality 2")
        << QMessageFilterKey::byType(QMessage::Email, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("type equality invalid")
        << QMessageFilterKey::byType(QMessage::None, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("type inequality 1")
        << QMessageFilterKey::byType(QMessage::Sms, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("type inequality 2")
        << QMessageFilterKey::byType(QMessage::Email, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("type inequality invalid")
        << QMessageFilterKey::byType(QMessage::None, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("type mask inclusion 1")
        << QMessageFilterKey::byType(QMessage::Sms, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("type mask inclusion 2")
        << QMessageFilterKey::byType(QMessage::Email, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("type mask inclusion 3")
        << QMessageFilterKey::byType(QMessage::Sms | QMessage::Email, QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("type mask inclusion empty")
        << QMessageFilterKey::byType(QMessage::None, QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("type mask exclusion 1")
        << QMessageFilterKey::byType(QMessage::Sms, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("type mask exclusion 2")
        << QMessageFilterKey::byType(QMessage::Email, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("type mask exclusion 3")
        << QMessageFilterKey::byType(QMessage::Sms | QMessage::Email, QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("type mask exclusion empty")
        << QMessageFilterKey::byType(QMessage::None, QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender equality 1")
        << QMessageFilterKey::bySender("Esteemed.Colleague@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] )
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("sender equality 2")
        << QMessageFilterKey::bySender("sysadmin@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("sender equality non-matching")
        << QMessageFilterKey::bySender("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender equality empty")
        << QMessageFilterKey::bySender(QString(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender equality zero-length")
        << QMessageFilterKey::bySender("", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender inequality 1")
        << QMessageFilterKey::bySender("Esteemed.Colleague@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] );

    QTest::newRow("sender inequality 2")
        << QMessageFilterKey::bySender("sysadmin@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] );

    QTest::newRow("sender inequality non-matching")
        << QMessageFilterKey::bySender("Nonesuch", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender inequality empty")
        << QMessageFilterKey::bySender(QString(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender inequality zero-length")
        << QMessageFilterKey::bySender("", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender inclusion 1")
        << QMessageFilterKey::bySender("example", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("sender inclusion 2")
        << QMessageFilterKey::bySender("ozone", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("sender inclusion non-matching")
        << QMessageFilterKey::bySender("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender inclusion empty")
        << QMessageFilterKey::bySender(QString(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender inclusion zero-length")
        << QMessageFilterKey::bySender("", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender exclusion 1")
        << QMessageFilterKey::bySender("example", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("sender exclusion 2")
        << QMessageFilterKey::bySender("ozone", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("sender exclusion non-matching")
        << QMessageFilterKey::bySender("Nonesuch", QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("sender exclusion empty")
        << QMessageFilterKey::bySender(QString(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("sender exclusion zero-length")
        << QMessageFilterKey::bySender("", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients equality 1")
        << QMessageFilterKey::byRecipients("Minion@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("recipients equality 2")
        << QMessageFilterKey::byRecipients("Important.Person@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("recipients equality non-matching")
        << QMessageFilterKey::byRecipients("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients equality empty")
        << QMessageFilterKey::byRecipients(QString(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients equality zero-length")
        << QMessageFilterKey::byRecipients("", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients inequality 1")
        << QMessageFilterKey::byRecipients("Minion@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("recipients inequality 2")
        << QMessageFilterKey::byRecipients("Important.Person@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("recipients inequality non-matching")
        << QMessageFilterKey::byRecipients("Nonesuch", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients inequality empty")
        << QMessageFilterKey::byRecipients(QString(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients inequality zero-length")
        << QMessageFilterKey::byRecipients("", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients inclusion 1")
        << QMessageFilterKey::byRecipients("example", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("recipients inclusion 2")
        << QMessageFilterKey::byRecipients("Mega", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("recipients inclusion non-matching")
        << QMessageFilterKey::byRecipients("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients inclusion empty")
        << QMessageFilterKey::byRecipients(QString(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients inclusion zero-length")
        << QMessageFilterKey::byRecipients("", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients exclusion 1")
        << QMessageFilterKey::byRecipients("example", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("recipients exclusion 2")
        << QMessageFilterKey::byRecipients("Mega", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("recipients exclusion non-matching")
        << QMessageFilterKey::byRecipients("Nonesuch", QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("recipients exclusion empty")
        << QMessageFilterKey::byRecipients(QString(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("recipients exclusion zero-length")
        << QMessageFilterKey::byRecipients("", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject equality 1")
        << QMessageFilterKey::bySubject("Free beer", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("subject equality 2")
        << QMessageFilterKey::bySubject("Scheduled maintenance", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("subject equality non-matching")
        << QMessageFilterKey::bySubject("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject equality empty")
        << QMessageFilterKey::bySubject(QString(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject equality zero-length")
        << QMessageFilterKey::bySubject("", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject inequality 1")
        << QMessageFilterKey::bySubject("Free beer", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("subject inequality 2")
        << QMessageFilterKey::bySubject("Scheduled maintenance", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] );

    QTest::newRow("subject inequality non-matching")
        << QMessageFilterKey::bySubject("Nonesuch", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inequality empty")
        << QMessageFilterKey::bySubject(QString(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inequality zero-length")
        << QMessageFilterKey::bySubject("", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inclusion 1")
        << QMessageFilterKey::bySubject("vation", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("subject inclusion 2")
        << QMessageFilterKey::bySubject(" ", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inclusion non-matching")
        << QMessageFilterKey::bySubject("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject inclusion empty")
        << QMessageFilterKey::bySubject(QString(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject inclusion zero-length")
        << QMessageFilterKey::bySubject("", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject exclusion 1")
        << QMessageFilterKey::bySubject("vation", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("subject exclusion 2")
        << QMessageFilterKey::bySubject(" ", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject exclusion non-matching")
        << QMessageFilterKey::bySubject("Nonesuch", QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("subject exclusion empty")
        << QMessageFilterKey::bySubject(QString(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("subject exclusion zero-length")
        << QMessageFilterKey::bySubject("", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("timeStamp equality 1")
        << QMessageFilterKey::byTimeStamp(QDateTime::fromString("1999-04-01T10:30:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("timeStamp equality 2")
        << QMessageFilterKey::byTimeStamp(QDateTime::fromString("2000-01-01T13:00:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] );

    QTest::newRow("timeStamp equality non-matching")
        << QMessageFilterKey::byTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("timeStamp equality empty")
        << QMessageFilterKey::byTimeStamp(QDateTime(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("timeStamp inequality 1")
        << QMessageFilterKey::byTimeStamp(QDateTime::fromString("1999-04-01T10:30:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("timeStamp inequality 2")
        << QMessageFilterKey::byTimeStamp(QDateTime::fromString("2000-01-01T13:00:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] << messageIds[2] );

    QTest::newRow("timeStamp inequality non-matching")
        << QMessageFilterKey::byTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("timeStamp inequality empty")
        << QMessageFilterKey::byTimeStamp(QDateTime(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QDateTime epoch(QDateTime::fromString("2000-01-01T12:00:00Z", Qt::ISODate));

    QTest::newRow("timeStamp less than")
        << QMessageFilterKey::byTimeStamp(epoch, QMessageDataComparator::LessThan) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("timeStamp less than equal")
        << QMessageFilterKey::byTimeStamp(epoch, QMessageDataComparator::LessThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] << messageIds[3] );

    QTest::newRow("timeStamp greater than")
        << QMessageFilterKey::byTimeStamp(epoch, QMessageDataComparator::GreaterThan) 
        << ( QMessageIdList() << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] );

    QTest::newRow("timeStamp greater than equal")
        << QMessageFilterKey::byTimeStamp(epoch, QMessageDataComparator::GreaterThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("receptionTimeStamp equality 1")
        << QMessageFilterKey::byReceptionTimeStamp(QDateTime::fromString("1999-04-01T10:31:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("receptionTimeStamp equality 2")
        << QMessageFilterKey::byReceptionTimeStamp(QDateTime::fromString("2000-01-01T13:05:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("receptionTimeStamp equality non-matching")
        << QMessageFilterKey::byReceptionTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("receptionTimeStamp equality empty")
        << QMessageFilterKey::byReceptionTimeStamp(QDateTime(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("receptionTimeStamp inequality 1")
        << QMessageFilterKey::byReceptionTimeStamp(QDateTime::fromString("1999-04-01T10:31:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("receptionTimeStamp inequality 2")
        << QMessageFilterKey::byReceptionTimeStamp(QDateTime::fromString("2000-01-01T13:05:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("receptionTimeStamp inequality non-matching")
        << QMessageFilterKey::byReceptionTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("receptionTimeStamp inequality empty")
        << QMessageFilterKey::byReceptionTimeStamp(QDateTime(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("receptionTimeStamp less than")
        << QMessageFilterKey::byReceptionTimeStamp(epoch, QMessageDataComparator::LessThan) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("receptionTimeStamp less than equal")
        << QMessageFilterKey::byReceptionTimeStamp(epoch, QMessageDataComparator::LessThanEqual) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("receptionTimeStamp greater than")
        << QMessageFilterKey::byReceptionTimeStamp(epoch, QMessageDataComparator::GreaterThan) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("receptionTimeStamp greater than equal")
        << QMessageFilterKey::byReceptionTimeStamp(epoch, QMessageDataComparator::GreaterThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("status equality 1")
        << QMessageFilterKey::byStatus(QMessage::Read, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("status equality 2")
        << QMessageFilterKey::byStatus(QMessage::HasAttachments, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] );

    QTest::newRow("status equality 3")
        << QMessageFilterKey::byStatus(QMessage::Removed, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("status inequality 1")
        << QMessageFilterKey::byStatus(QMessage::Read, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("status inequality 2")
        << QMessageFilterKey::byStatus(QMessage::HasAttachments, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("status inequality 3")
        << QMessageFilterKey::byStatus(QMessage::Removed, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("status mask inclusion 1")
        << QMessageFilterKey::byStatus(QMessage::Read, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("status mask inclusion 2")
        << QMessageFilterKey::byStatus(QMessage::Read | QMessage::HasAttachments, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("status mask inclusion 3")
        << QMessageFilterKey::byStatus(QMessage::Read | QMessage::Removed, QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("status mask inclusion empty")
        << QMessageFilterKey::byStatus(static_cast<QMessage::StatusFlags>(0), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("status mask exclusion 1")
        << QMessageFilterKey::byStatus(QMessage::Read, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("status mask exclusion 2")
        << QMessageFilterKey::byStatus(QMessage::Read | QMessage::HasAttachments, QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("status mask exclusion 3")
        << QMessageFilterKey::byStatus(QMessage::Read | QMessage::Removed, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("status mask exclusion empty")
        << QMessageFilterKey::byStatus(static_cast<QMessage::StatusFlags>(0), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("priority equality 1")
        << QMessageFilterKey::byPriority(QMessage::High, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("priority equality 2")
        << QMessageFilterKey::byPriority(QMessage::Low, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("priority equality 3")
        << QMessageFilterKey::byPriority(QMessage::Normal, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("priority inequality 1")
        << QMessageFilterKey::byPriority(QMessage::High, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("priority inequality 2")
        << QMessageFilterKey::byPriority(QMessage::Low, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("priority inequality 3")
        << QMessageFilterKey::byPriority(QMessage::Normal, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] );

    QTest::newRow("size equality 1")
        << QMessageFilterKey::bySize(1056, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] );

    QTest::newRow("size equality 2")
        << QMessageFilterKey::bySize(20480, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("size equality non-matching")
        << QMessageFilterKey::bySize(13, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("size equality zero")
        << QMessageFilterKey::bySize(0, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("size inequality 1")
        << QMessageFilterKey::bySize(1056, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] );

    QTest::newRow("size inequality 2")
        << QMessageFilterKey::bySize(20480, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("size inequality non-matching")
        << QMessageFilterKey::bySize(13, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("size inequality empty")
        << QMessageFilterKey::bySize(0, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    uint discriminator(10240);

    QTest::newRow("size less than")
        << QMessageFilterKey::bySize(discriminator, QMessageDataComparator::LessThan) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("size less than equal")
        << QMessageFilterKey::bySize(discriminator, QMessageDataComparator::LessThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] );

    QTest::newRow("size greater than")
        << QMessageFilterKey::bySize(discriminator, QMessageDataComparator::GreaterThan) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] );

    QTest::newRow("size greater than equal")
        << QMessageFilterKey::bySize(discriminator, QMessageDataComparator::GreaterThanEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    // Note: custom field negations do not work as expected!
    QTest::newRow("customField equality 1")
        << QMessageFilterKey::byCustomField("flagged", "true", QMessageDataComparator::Equal)
        << ( QMessageIdList() << messageIds[0] << messageIds[1] )
        //<< ( QMessageIdList() << messageIds[2] << messageIds[3] << messageIds[4] );
        << QMessageIdList();

    QTest::newRow("customField equality 2")
        << QMessageFilterKey::byCustomField("spam", "filter:yes", QMessageDataComparator::Equal)
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("customField equality empty")
        << QMessageFilterKey::byCustomField("spam", "", QMessageDataComparator::Equal)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField equality non-matching key")
        << QMessageFilterKey::byCustomField("hello", "world", QMessageDataComparator::Equal)
        << QMessageIdList()
        << QMessageIdList();

    QTest::newRow("customField equality non-matching value")
        << QMessageFilterKey::byCustomField("spam", "spam spam spam", QMessageDataComparator::Equal)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField inequality 1")
        << QMessageFilterKey::byCustomField("flagged", "true", QMessageDataComparator::NotEqual)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[0] << messageIds[1] );

    QTest::newRow("customField inequality 2")
        << QMessageFilterKey::byCustomField("spam", "filter:yes", QMessageDataComparator::NotEqual)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("customField inequality empty")
        << QMessageFilterKey::byCustomField("spam", "", QMessageDataComparator::NotEqual)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("customField inequality non-matching key")
        << QMessageFilterKey::byCustomField("hello", "world", QMessageDataComparator::NotEqual)
        << QMessageIdList()
        << QMessageIdList();

    QTest::newRow("customField inequality non-matching value")
        << QMessageFilterKey::byCustomField("spam", "spam spam spam", QMessageDataComparator::NotEqual)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("customField inclusion 1")
        << QMessageFilterKey::byCustomField("flagged", "ru", QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[0] << messageIds[1] )
        << QMessageIdList();

    QTest::newRow("customField inclusion 2")
        << QMessageFilterKey::byCustomField("spam", "filter", QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("customField inclusion empty")
        << QMessageFilterKey::byCustomField("spam", "", QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("customField inclusion non-matching key")
        << QMessageFilterKey::byCustomField("hello", "world", QMessageDataComparator::Includes)
        << QMessageIdList()
        << QMessageIdList();

    QTest::newRow("customField inclusion non-matching value")
        << QMessageFilterKey::byCustomField("spam", "spam spam spam", QMessageDataComparator::Includes)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField exclusion 1")
        << QMessageFilterKey::byCustomField("flagged", "ru", QMessageDataComparator::Excludes)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[0] << messageIds[1] );

    QTest::newRow("customField exclusion 2")
        << QMessageFilterKey::byCustomField("spam", "filter", QMessageDataComparator::Excludes)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField exclusion empty")
        << QMessageFilterKey::byCustomField("spam", "", QMessageDataComparator::Excludes)
        << QMessageIdList()
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("customField exclusion non-matching key")
        << QMessageFilterKey::byCustomField("hello", "world", QMessageDataComparator::Excludes)
        << QMessageIdList()
        << QMessageIdList();

    QTest::newRow("customField exclusion non-matching value")
        << QMessageFilterKey::byCustomField("spam", "spam spam spam", QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << QMessageIdList();

    QTest::newRow("parentAccountId equality 1")
        << QMessageFilterKey::byParentAccountId(accountIds[0], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentAccountId equality 2")
        << QMessageFilterKey::byParentAccountId(accountIds[2], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentAccountId equality invalid")
        << QMessageFilterKey::byParentAccountId(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentAccountId inequality 1")
        << QMessageFilterKey::byParentAccountId(accountIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentAccountId inequality 2")
        << QMessageFilterKey::byParentAccountId(accountIds[2], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentAccountId inequality invalid")
        << QMessageFilterKey::byParentAccountId(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentAccountId filter inclusion 1")
        << QMessageFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentAccountId filter inclusion 2")
        << QMessageFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentAccountId filter inclusion 3")
        << QMessageFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentAccountId filter inclusion empty")
        << QMessageFilterKey::byParentAccountId(QMessageAccountFilterKey(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentAccountId filter inclusion non-matching")
        << QMessageFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentAccountId filter exclusion 1")
        << QMessageFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentAccountId filter exclusion 2")
        << QMessageFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentAccountId filter exclusion 3")
        << QMessageFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentAccountId filter exclusion empty")
        << QMessageFilterKey::byParentAccountId(QMessageAccountFilterKey(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentAccountId filter exclusion non-matching")
        << QMessageFilterKey::byParentAccountId(QMessageAccountFilterKey::byName("NoneSuch"), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();


#ifdef QMESSAGING_OPTIONAL_FOLDER
    QTest::newRow("parentFolderId equality 1")
        << QMessageFilterKey::byParentFolderId(folderIds[0], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId equality 2")
        << QMessageFilterKey::byParentFolderId(folderIds[1], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId equality invalid")
        << QMessageFilterKey::byParentFolderId(QMessageFolderId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentFolderId inequality 1")
        << QMessageFilterKey::byParentFolderId(folderIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentFolderId inequality 2")
        << QMessageFilterKey::byParentFolderId(folderIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("parentFolderId inequality invalid")
        << QMessageFilterKey::byParentFolderId(QMessageFolderId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentFolderId filter inclusion 1")
        << QMessageFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("My messages", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId filter inclusion 2")
        << QMessageFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId filter inclusion 3")
        << QMessageFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentFolderId filter inclusion empty")
        << QMessageFilterKey::byParentFolderId(QMessageFolderFilterKey(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("parentFolderId filter inclusion non-matching")
        << QMessageFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentFolderId filter exclusion 1")
        << QMessageFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("My messages", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] );

    QTest::newRow("parentFolderId filter exclusion 2")
        << QMessageFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] );

    QTest::newRow("parentFolderId filter exclusion 3")
        << QMessageFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] );

    QTest::newRow("parentFolderId filter exclusion empty")
        << QMessageFilterKey::byParentFolderId(QMessageFolderFilterKey(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("parentFolderId filter exclusion non-matching")
        << QMessageFilterKey::byParentFolderId(QMessageFolderFilterKey::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();
    QTest::newRow("ancestorFolderIds inclusion 1")
        << QMessageFilterKey::byAncestorFolderIds(folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] );

    QTest::newRow("ancestorFolderIds inclusion 2")
        << QMessageFilterKey::byAncestorFolderIds(folderIds[2], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("ancestorFolderIds inclusion invalid")
        << QMessageFilterKey::byAncestorFolderIds(QMessageFolderId(), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("ancestorFolderIds exclusion 1")
        << QMessageFilterKey::byAncestorFolderIds(folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] );

    QTest::newRow("ancestorFolderIds exclusion 2")
        << QMessageFilterKey::byAncestorFolderIds(folderIds[2], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("ancestorFolderIds exclusion invalid")
        << QMessageFilterKey::byAncestorFolderIds(QMessageFolderId(), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();

    QTest::newRow("ancestorFolderIds filter inclusion 1")
        << QMessageFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byDisplayName("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] );

    QTest::newRow("ancestorFolderIds filter inclusion 2")
        << QMessageFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byDisplayName("X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] );

    QTest::newRow("ancestorFolderIds filter inclusion 3")
        << QMessageFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] );

    QTest::newRow("ancestorFolderIds filter inclusion empty")
        << QMessageFilterKey::byAncestorFolderIds(QMessageFolderFilterKey(), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] );

    QTest::newRow("ancestorFolderIds filter inclusion non-matching")
        << QMessageFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds;

    QTest::newRow("ancestorFolderIds filter exclusion 1")
        << QMessageFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byDisplayName("Inbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] );

    QTest::newRow("ancestorFolderIds filter exclusion 2")
        << QMessageFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byDisplayName("X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] );

    QTest::newRow("ancestorFolderIds filter exclusion 3")
        << QMessageFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byPath("Inbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] );

    QTest::newRow("ancestorFolderIds filter exclusion empty")
        << QMessageFilterKey::byAncestorFolderIds(QMessageFolderFilterKey(), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] );

    QTest::newRow("ancestorFolderIds filter exclusion non-matching")
        << QMessageFilterKey::byAncestorFolderIds(QMessageFolderFilterKey::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList();
#endif
}

void tst_QMessageStoreKeys::testMessageFilterKey()
{
    QFETCH(QMessageFilterKey, key);
    QFETCH(QMessageIdList, ids);
    QFETCH(QMessageIdList, negatedIds);

    if (key.isSupported()) {
        // Order is irrelevant for filtering
        QCOMPARE(QMessageStore::instance()->queryMessages(key).toSet().subtract(existingMessageIds), ids.toSet());
        QCOMPARE(QMessageStore::instance()->queryMessages(~key).toSet().subtract(existingMessageIds), negatedIds.toSet());
    }
}

void tst_QMessageStoreKeys::testMessageSortKey_data()
{
    QTest::addColumn<QMessageSortKey>("sortKey");
    QTest::addColumn<MessageListList>("ids");

    QTest::newRow("type ascending")
        << QMessageSortKey::byType(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] ) );

    QTest::newRow("type descending")
        << QMessageSortKey::byType(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("sender ascending")
        << QMessageSortKey::bySender(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] ) );

    QTest::newRow("sender descending")
        << QMessageSortKey::bySender(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] << messageIds[4] ) );

    QTest::newRow("recipients ascending")
        << QMessageSortKey::byRecipients(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] << messageIds[4] ) );

    QTest::newRow("recipients descending")
        << QMessageSortKey::byRecipients(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] ) );

    QTest::newRow("subject ascending")
        << QMessageSortKey::bySubject(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] ) );

    QTest::newRow("subject descending")
        << QMessageSortKey::bySubject(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("timeStamp ascending")
        << QMessageSortKey::byTimeStamp(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] << messageIds[3] ) );

    QTest::newRow("timeStamp descending")
        << QMessageSortKey::byTimeStamp(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[1] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("receptionTimeStamp ascending")
        << QMessageSortKey::byReceptionTimeStamp(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] ) );

    QTest::newRow("receptionTimeStamp descending")
        << QMessageSortKey::byReceptionTimeStamp(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("priority ascending")
        << QMessageSortKey::byPriority(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] ) );

    QTest::newRow("priority descending")
        << QMessageSortKey::byPriority(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("size ascending")
        << QMessageSortKey::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] ) );

    QTest::newRow("size descending")
        << QMessageSortKey::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type ascending, priority ascending, size ascending")
        << QMessageSortKey::byType(Qt::AscendingOrder) + QMessageSortKey::byPriority(Qt::AscendingOrder) + QMessageSortKey::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] ) );

    QTest::newRow("type ascending, priority ascending, size descending")
        << QMessageSortKey::byType(Qt::AscendingOrder) + QMessageSortKey::byPriority(Qt::AscendingOrder) + QMessageSortKey::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] ) );

    QTest::newRow("type ascending, priority descending, size ascending")
        << QMessageSortKey::byType(Qt::AscendingOrder) + QMessageSortKey::byPriority(Qt::DescendingOrder) + QMessageSortKey::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("type ascending, priority descending, size descending")
        << QMessageSortKey::byType(Qt::AscendingOrder) + QMessageSortKey::byPriority(Qt::DescendingOrder) + QMessageSortKey::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("type descending, priority ascending, size ascending")
        << QMessageSortKey::byType(Qt::DescendingOrder) + QMessageSortKey::byPriority(Qt::AscendingOrder) + QMessageSortKey::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority ascending, size descending")
        << QMessageSortKey::byType(Qt::DescendingOrder) + QMessageSortKey::byPriority(Qt::AscendingOrder) + QMessageSortKey::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority descending, size ascending")
        << QMessageSortKey::byType(Qt::DescendingOrder) + QMessageSortKey::byPriority(Qt::DescendingOrder) + QMessageSortKey::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority descending, size descending")
        << QMessageSortKey::byType(Qt::DescendingOrder) + QMessageSortKey::byPriority(Qt::DescendingOrder) + QMessageSortKey::bySize(Qt::DescendingOrder)
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

    // Filter out the existing messages
    QMessageIdList sortedIds(QMessageStore::instance()->queryMessages(QMessageFilterKey(), sortKey));
    for (QMessageIdList::iterator it = sortedIds.begin(); it != sortedIds.end(); ) {
        if (existingMessageIds.contains(*it)) {
            it = sortedIds.erase(it);
        } else {
            ++it;
        }
    }

    QMessageIdList::const_iterator sit = sortedIds.begin();
    MessageListList::const_iterator iit = ids.begin();

    bool match(true);
    for (int groupCount = 0; sit != sortedIds.end(); ++sit) {
        if (iit == ids.end()) {
            match = false;
            break;
        } else {
            if ((*iit).contains(*sit)) {
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

