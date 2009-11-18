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
    void init();
    void initTestCase();
    void cleanup();
    void cleanupTestCase();

    void testAccountFilter_data();
    void testAccountFilter();

    void testAccountOrdering_data();
    void testAccountOrdering();

    void testFolderFilter_data();
    void testFolderFilter();

    void testFolderOrdering_data();
    void testFolderOrdering();

    void testMessageFilter_data();
    void testMessageFilter();

    void testMessageOrdering_data();
    void testMessageOrdering();

private:
    QSet<QMessageAccountId> existingAccountIds;
    QMessageAccountIdList accountIds;

    QSet<QMessageFolderId> existingFolderIds;
    QMessageFolderIdList folderIds;

    QSet<QMessageId> existingMessageIds;
    QMessageFilter existingAccountsFilter;
    QMessageIdList messageIds;

    QList<int> messageSizes;

    uint unsupportedCount;
};

Q_DECLARE_METATYPE(QMessageAccountIdList)
Q_DECLARE_METATYPE(QMessageAccountFilter)
Q_DECLARE_METATYPE(QMessageAccountOrdering)

Q_DECLARE_METATYPE(QMessageFolderIdList)
Q_DECLARE_METATYPE(QMessageFolderFilter)
Q_DECLARE_METATYPE(QMessageFolderOrdering)

Q_DECLARE_METATYPE(QMessageIdList)
Q_DECLARE_METATYPE(QMessageFilter)
Q_DECLARE_METATYPE(QMessageOrdering)

typedef QList<QMessageIdList> MessageListList;
Q_DECLARE_METATYPE(MessageListList)

QDebug &operator<<(QDebug &dbg, const QMessageAccountId &id)
{
    return dbg << id.toString();
}

QDebug &operator<<(QDebug &dbg, const QMessageFolderId &id)
{
    return dbg << id.toString();
}

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

void tst_QMessageStoreKeys::init()
{
    unsupportedCount = 0;
}

void tst_QMessageStoreKeys::initTestCase()
{
    Support::clearMessageStore();

    existingAccountIds = QMessageStore::instance()->queryAccounts().toSet();
    existingAccountsFilter = ~QMessageFilter();
    foreach(QMessageAccountId id, existingAccountIds) {
        existingAccountsFilter |= QMessageFilter::byParentAccountId(id);
    }

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

    existingFolderIds = QMessageStore::instance()->queryFolders().toSet();

    QList<Support::Parameters> folderParams;
    folderParams << Params()("parentAccountName", "Alter Ego")
                            ("path", "My messages")
                            ("displayName", "My messages")
                            ("parentFolderPath", "")
                 << Params()("parentAccountName", "Work")
                            ("path", "Innbox")
                            ("displayName", "Innbox")
                            ("parentFolderPath", "")
                 << Params()("parentAccountName", "Work")
                            ("path", "Innbox/X-Announce")
                            ("displayName", "X-Announce")
                            ("parentFolderPath", "Innbox")
                 << Params()("parentAccountName", "Work")
                            ("path", "Innbox/X-Announce/X-Archived")
                            ("displayName", "X-Archived")
                            ("parentFolderPath", "Innbox/X-Announce");

    foreach (const Support::Parameters &params, folderParams) {
        folderIds.append(Support::addFolder(params));
        QVERIFY(folderIds.last().isValid());
    }

    existingMessageIds = QMessageStore::instance()->queryMessages(~existingAccountsFilter).toSet();

    // For windows at least, we can't have HasAttachments set without a real attachment
    const char *attachmentPaths = TESTDATA_DIR "/testdata/1.txt";

    QList<Support::Parameters> messageParams;
    messageParams << Params()("parentAccountName", "Alter Ego")
                             ("parentFolderPath", "My messages")
#ifdef Q_OS_WIN // SMS messages must be in SMS store on Windows
                             ("type", "email")
#else
                             ("type", "sms")
#endif
                             ("to", "SuperMegaLightningBabe")
                             ("from", "Frozone")
                             ("subject", "Ice to meet you")
                             ("text", "Shall I compare thee to a summers day")
                             ("date", "2000-01-01T12:00:00Z")
                             ("receivedDate", "2000-01-01T12:01:00Z")
                             ("priority", "")
                             ("size", "160")
                             ("status-read", "true")
                             ("custom-flagged", "true")
                  << Params()("parentAccountName", "Work")
                             ("parentFolderPath", "Innbox")
                             ("type", "email")
                             ("to", "Important.Person@example.com")
                             ("from", "Esteemed.Colleague@example.com")
                             ("subject", "Meeting agenda")
                             ("date", "2000-01-01T12:00:00Z")
                             ("receivedDate", "2000-01-01T20:00:00Z")
                             ("priority", "High")
                             ("size", "10240")
                             ("status-hasAttachments", "true")
                             ("attachments", attachmentPaths)
                             ("custom-spam", "filter:no")
                             ("custom-flagged", "true")
                  << Params()("parentAccountName", "Work")
                             ("parentFolderPath", "Innbox")
                             ("type", "email")
                             ("to", "Important.Person@example.com,Minion@example.com")
                             ("from", "Big.Boss@example.com")
                             ("subject", "Motivational message")
                             ("text", "School's out for summer")
                             ("date", "2000-01-01T13:00:00Z")
                             ("receivedDate", "2000-01-01T13:05:00Z")
                             ("priority", "High")
                             ("size", "20480")
                             ("status-hasAttachments", "true")
                             ("attachments", attachmentPaths)
                             ("custom-spam", "filter:no")
                  << Params()("parentAccountName", "Work")
                             ("parentFolderPath", "Innbox/X-Announce")
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
                             ("parentFolderPath", "Innbox/X-Announce/X-Archived")
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
                             ("attachments", attachmentPaths)
                             ("custom-spam", "filter:yes");

    foreach (const Support::Parameters &params, messageParams) {
        messageIds.append(Support::addMessage(params));
        QVERIFY(messageIds.last().isValid());

        // Use the resultant message sizes in testing - the configured sizes aren't honoured by MAPI
        QMessage m(messageIds.last());
        messageSizes.append(m.size());
    }
}

void tst_QMessageStoreKeys::cleanup()
{
    if (unsupportedCount != 0) {
        qWarning() << QString("%1 unsupported tests bypassed").arg(unsupportedCount);
    }
}

void tst_QMessageStoreKeys::cleanupTestCase()
{
}

void tst_QMessageStoreKeys::testAccountFilter_data()
{
    QTest::addColumn<QMessageAccountFilter>("filter");
    QTest::addColumn<QMessageAccountIdList>("ids");
    QTest::addColumn<QMessageAccountIdList>("negatedIds");

    QTest::newRow("empty filter")
        << QMessageAccountFilter() 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id equality 1")
        << QMessageAccountFilter::byId(accountIds[0], QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("id equality 2")
        << QMessageAccountFilter::byId(accountIds[1], QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("id equality invalid")
        << QMessageAccountFilter::byId(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id inequality 1")
        << QMessageAccountFilter::byId(accountIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id inequality 2")
        << QMessageAccountFilter::byId(accountIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id inequality invalid")
        << QMessageAccountFilter::byId(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id list inclusion 1")
        << QMessageAccountFilter::byId(QMessageAccountIdList() << accountIds[0], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("id list inclusion 2")
        << QMessageAccountFilter::byId(QMessageAccountIdList() << accountIds[1], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("id list inclusion 3")
        << QMessageAccountFilter::byId(QMessageAccountIdList() << accountIds[0] << accountIds[1], QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[2] );

    QTest::newRow("id list inclusion empty")
        << QMessageAccountFilter::byId(QMessageAccountIdList(), QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id list exclusion 1")
        << QMessageAccountFilter::byId(QMessageAccountIdList() << accountIds[0], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id list exclusion 2")
        << QMessageAccountFilter::byId(QMessageAccountIdList() << accountIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id list exclusion 3")
        << QMessageAccountFilter::byId(QMessageAccountIdList() << accountIds[0] << accountIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] );

    QTest::newRow("id list exclusion empty")
        << QMessageAccountFilter::byId(QMessageAccountIdList(), QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion 1")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("id filter inclusion 2")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("Personal", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("id filter inclusion 3")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion empty")
        << QMessageAccountFilter::byId(QMessageAccountFilter(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("id filter inclusion non-matching")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion 1")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("id filter exclusion 2")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("Personal", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("id filter exclusion 3")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion empty")
        << QMessageAccountFilter::byId(QMessageAccountFilter(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("id filter exclusion non-matching")
        << QMessageAccountFilter::byId(QMessageAccountFilter::byName("NoneSuch"), QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name equality 1")
        << QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("name equality 2")
        << QMessageAccountFilter::byName("Personal", QMessageDataComparator::Equal) 
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("name equality non-matching")
        << QMessageAccountFilter::byName("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name equality empty")
        << QMessageAccountFilter::byName(QString(), QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name equality zero-length")
        << QMessageAccountFilter::byName("", QMessageDataComparator::Equal) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name inequality 1")
        << QMessageAccountFilter::byName("Work", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("name inequality 2")
        << QMessageAccountFilter::byName("Personal", QMessageDataComparator::NotEqual) 
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    QTest::newRow("name inequality non-matching")
        << QMessageAccountFilter::byName("Nonesuch", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inequality empty")
        << QMessageAccountFilter::byName(QString(), QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inequality zero-length")
        << QMessageAccountFilter::byName("", QMessageDataComparator::NotEqual) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion 1")
        << QMessageAccountFilter::byName("ork", QMessageDataComparator::Includes) 
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QTest::newRow("name inclusion 2")
        << QMessageAccountFilter::byName("o", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion non-matching")
        << QMessageAccountFilter::byName("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name inclusion empty")
        << QMessageAccountFilter::byName(QString(), QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name inclusion zero-length")
        << QMessageAccountFilter::byName("", QMessageDataComparator::Includes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name exclusion 1")
        << QMessageAccountFilter::byName("ork", QMessageDataComparator::Excludes) 
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[0] );

    QTest::newRow("name exclusion 2")
        << QMessageAccountFilter::byName("o", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name exclusion non-matching")
        << QMessageAccountFilter::byName("Nonesuch", QMessageDataComparator::Excludes) 
        << accountIds
        << QMessageAccountIdList();

    QTest::newRow("name exclusion empty")
        << QMessageAccountFilter::byName(QString(), QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    QTest::newRow("name exclusion zero-length")
        << QMessageAccountFilter::byName("", QMessageDataComparator::Excludes) 
        << QMessageAccountIdList()
        << accountIds;

    // Test some basic combinations
    QTest::newRow("id inequality AND name equality")
        << ( QMessageAccountFilter::byId(accountIds[0], QMessageDataComparator::NotEqual) &
             QMessageAccountFilter::byName("Personal", QMessageDataComparator::Equal) )
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QTest::newRow("id equality OR name inequality")
        << ( QMessageAccountFilter::byId(accountIds[0], QMessageDataComparator::Equal) |
             QMessageAccountFilter::byName("Personal", QMessageDataComparator::NotEqual) )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );
}

void tst_QMessageStoreKeys::testAccountFilter()
{
    QFETCH(QMessageAccountFilter, filter);
    QFETCH(QMessageAccountIdList, ids);
    QFETCH(QMessageAccountIdList, negatedIds);

    if (filter.isSupported()) {
        QVERIFY(filter == filter);
        QCOMPARE(filter != QMessageAccountFilter(), !filter.isEmpty());

        // Order is irrelevant for filtering
        QCOMPARE(QMessageStore::instance()->queryAccounts(filter).toSet().subtract(existingAccountIds), ids.toSet());
        QCOMPARE(QMessageStore::instance()->queryAccounts(~filter).toSet().subtract(existingAccountIds), negatedIds.toSet());
    } else {
        ++unsupportedCount;
    }
}

void tst_QMessageStoreKeys::testAccountOrdering_data()
{
    QTest::addColumn<QMessageAccountOrdering>("ordering");
    QTest::addColumn<QMessageAccountIdList>("ids");

    QTest::newRow("name ascending")
        << QMessageAccountOrdering::byName(Qt::AscendingOrder)
        << ( QMessageAccountIdList() << accountIds[2] << accountIds[1] << accountIds[0] );

    QTest::newRow("name descending")
        << QMessageAccountOrdering::byName(Qt::DescendingOrder)
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] << accountIds[2] );
}

void tst_QMessageStoreKeys::testAccountOrdering()
{
    QFETCH(QMessageAccountOrdering, ordering);
    QFETCH(QMessageAccountIdList, ids);

    if (ordering.isSupported()) {
        QVERIFY(ordering == ordering);
        QCOMPARE(ordering != QMessageAccountOrdering(), !ordering.isEmpty());

        // Filter out the existing accounts
        QMessageAccountIdList sortedIds(QMessageStore::instance()->queryAccounts(QMessageAccountFilter(), ordering));
        for (QMessageAccountIdList::iterator it = sortedIds.begin(); it != sortedIds.end(); ) {
            if (existingAccountIds.contains(*it)) {
                it = sortedIds.erase(it);
            } else {
                ++it;
            }
        }

        QCOMPARE(sortedIds, ids);
    } else {
        ++unsupportedCount;
    }
}

void tst_QMessageStoreKeys::testFolderFilter_data()
{
    QTest::addColumn<QMessageFolderFilter>("filter");
    QTest::addColumn<QMessageFolderIdList>("ids");
    QTest::addColumn<QMessageFolderIdList>("negatedIds");

    QTest::newRow("empty filter")
        << QMessageFolderFilter() 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("id equality 1")
        << QMessageFolderFilter::byId(folderIds[0], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("id equality 2")
        << QMessageFolderFilter::byId(folderIds[1], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] );

    QTest::newRow("id equality invalid")
        << QMessageFolderFilter::byId(QMessageFolderId(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("id inequality 1")
        << QMessageFolderFilter::byId(folderIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("id inequality 2")
        << QMessageFolderFilter::byId(folderIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[1] );

    QTest::newRow("id inequality invalid")
        << QMessageFolderFilter::byId(QMessageFolderId(), QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("id list inclusion 1")
        << QMessageFolderFilter::byId(QMessageFolderIdList() << folderIds[0], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("id list inclusion 2")
        << QMessageFolderFilter::byId(QMessageFolderIdList() << folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] );

    QTest::newRow("id list inclusion 3")
        << QMessageFolderFilter::byId(QMessageFolderIdList() << folderIds[0] << folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("id list inclusion empty")
        << QMessageFolderFilter::byId(QMessageFolderIdList(), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("id list exclusion 1")
        << QMessageFolderFilter::byId(QMessageFolderIdList() << folderIds[0], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("id list exclusion 2")
        << QMessageFolderFilter::byId(QMessageFolderIdList() << folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[1] );

    QTest::newRow("id list exclusion 3")
        << QMessageFolderFilter::byId(QMessageFolderIdList() << folderIds[0] << folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("id list exclusion empty")
        << QMessageFolderFilter::byId(QMessageFolderIdList(), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("id filter inclusion 1")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("My messages", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("id filter inclusion 2")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("Innbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("id filter inclusion 3")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("box/X-Ann", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("id filter inclusion empty")
        << QMessageFolderFilter::byId(QMessageFolderFilter(), QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("id filter inclusion non-matching")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("id filter exclusion 1")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("My messages", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("id filter exclusion 2")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("Innbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("id filter exclusion 3")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("box/X-Ann", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("id filter exclusion empty")
        << QMessageFolderFilter::byId(QMessageFolderFilter(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("id filter exclusion non-matching")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path equality 1")
        << QMessageFolderFilter::byPath("My messages", QMessageDataComparator::Equal)
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("path equality 2")
        << QMessageFolderFilter::byPath("Innbox/X-Announce", QMessageDataComparator::Equal)
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("path equality non-matching")
        << QMessageFolderFilter::byPath("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path equality empty")
        << QMessageFolderFilter::byPath(QString(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path equality zero-length")
        << QMessageFolderFilter::byPath("", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path inequality 1")
        << QMessageFolderFilter::byPath("My messages", QMessageDataComparator::NotEqual)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("path inequality 2")
        << QMessageFolderFilter::byPath("Innbox/X-Announce", QMessageDataComparator::NotEqual)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("path inequality non-matching")
        << QMessageFolderFilter::byPath("Nonesuch", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path inequality empty")
        << QMessageFolderFilter::byPath(QString(), QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path inequality zero-length")
        << QMessageFolderFilter::byPath("", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path inclusion 1")
        << QMessageFolderFilter::byPath("ess", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("path inclusion 2")
        << QMessageFolderFilter::byPath("box/X-Ann", QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("path inclusion non-matching")
        << QMessageFolderFilter::byPath("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path inclusion empty")
        << QMessageFolderFilter::byPath(QString(), QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path inclusion zero-length")
        << QMessageFolderFilter::byPath("", QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path exclusion 1")
        << QMessageFolderFilter::byPath("ess", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("path exclusion 2")
        << QMessageFolderFilter::byPath("box/X-Ann", QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("path exclusion non-matching")
        << QMessageFolderFilter::byPath("Nonesuch", QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("path exclusion empty")
        << QMessageFolderFilter::byPath(QString(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("path exclusion zero-length")
        << QMessageFolderFilter::byPath("", QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName equality 1")
        << QMessageFolderFilter::byDisplayName("My messages", QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName equality 2")
        << QMessageFolderFilter::byDisplayName("X-Announce", QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("displayName equality non-matching")
        << QMessageFolderFilter::byDisplayName("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName equality empty")
        << QMessageFolderFilter::byDisplayName(QString(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName equality zero-length")
        << QMessageFolderFilter::byDisplayName("", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName inequality 1")
        << QMessageFolderFilter::byDisplayName("My messages", QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("displayName inequality 2")
        << QMessageFolderFilter::byDisplayName("X-Announce", QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("displayName inequality non-matching")
        << QMessageFolderFilter::byDisplayName("Nonesuch", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inequality empty")
        << QMessageFolderFilter::byDisplayName(QString(), QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inequality zero-length")
        << QMessageFolderFilter::byDisplayName("", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inclusion 1")
        << QMessageFolderFilter::byDisplayName("message", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName inclusion 2")
        << QMessageFolderFilter::byDisplayName("X-A", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("displayName inclusion non-matching")
        << QMessageFolderFilter::byDisplayName("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName inclusion empty")
        << QMessageFolderFilter::byDisplayName(QString(), QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName inclusion zero-length")
        << QMessageFolderFilter::byDisplayName("", QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName exclusion 1")
        << QMessageFolderFilter::byDisplayName("messages", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("displayName exclusion 2")
        << QMessageFolderFilter::byDisplayName("X-A", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName exclusion non-matching")
        << QMessageFolderFilter::byDisplayName("Nonesuch", QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("displayName exclusion empty")
        << QMessageFolderFilter::byDisplayName(QString(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("displayName exclusion zero-length")
        << QMessageFolderFilter::byDisplayName("", QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId equality 1")
        << QMessageFolderFilter::byParentAccountId(accountIds[0], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId equality 2")
        << QMessageFolderFilter::byParentAccountId(accountIds[2], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId equality invalid")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId inequality 1")
        << QMessageFolderFilter::byParentAccountId(accountIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId inequality 2")
        << QMessageFolderFilter::byParentAccountId(accountIds[2], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId inequality invalid")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("parentAccountId filter inclusion 1")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId filter inclusion 2")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId filter inclusion 3")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("parentAccountId filter inclusion empty")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter(), QMessageDataComparator::Includes) 
        << folderIds 
        << QMessageFolderIdList();

    QTest::newRow("parentAccountId filter inclusion non-matching")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId filter exclusion 1")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("parentAccountId filter exclusion 2")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("parentAccountId filter exclusion 3")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId filter exclusion empty")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentAccountId filter exclusion non-matching")
        << QMessageFolderFilter::byParentAccountId(QMessageAccountFilter::byName("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("parentFolderId equality 1")
        << QMessageFolderFilter::byParentFolderId(folderIds[1], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("parentFolderId equality 2")
        << QMessageFolderFilter::byParentFolderId(folderIds[2], QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] );

    QTest::newRow("parentFolderId equality invalid")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderId(), QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("parentFolderId inequality 1")
        << QMessageFolderFilter::byParentFolderId(folderIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("parentFolderId inequality 2")
        << QMessageFolderFilter::byParentFolderId(folderIds[2], QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("parentFolderId inequality invalid")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderId(), QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("parentFolderId filter inclusion 1")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("Innbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] );

    QTest::newRow("parentFolderId filter inclusion 2")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("parentFolderId filter inclusion 3")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("parentFolderId filter inclusion empty")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter(), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("parentFolderId filter inclusion non-matching")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("parentFolderId filter exclusion 1")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("Innbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("parentFolderId filter exclusion 2")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("parentFolderId filter exclusion 3")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("parentFolderId filter exclusion empty")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter(), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("parentFolderId filter exclusion non-matching")
        << QMessageFolderFilter::byParentFolderId(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("ancestorFolderIds list inclusion 1")
        << QMessageFolderFilter::byAncestorFolderIds(folderIds[2], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] );

    QTest::newRow("ancestorFolderIds list inclusion 2")
        << QMessageFolderFilter::byAncestorFolderIds(folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("ancestorFolderIds list inclusion invalid")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderId(), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("ancestorFolderIds list exclusion 1")
        << QMessageFolderFilter::byAncestorFolderIds(folderIds[2], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("ancestorFolderIds list exclusion 2")
        << QMessageFolderFilter::byAncestorFolderIds(folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds list exclusion invalid")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderId(), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("ancestorFolderIds filter inclusion 1")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Innbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] );

    QTest::newRow("ancestorFolderIds filter inclusion 2")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("ancestorFolderIds filter inclusion 3")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("ancestorFolderIds filter inclusion empty")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter(), QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("ancestorFolderIds filter inclusion non-matching")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("ancestorFolderIds filter exclusion 1")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Innbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion 2")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion 3")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion empty")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter(), QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("ancestorFolderIds filter exclusion non-matching")
        << QMessageFolderFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    // Test some basic combinations
    QTest::newRow("id inequality AND displayName inclusion")
        << ( QMessageFolderFilter::byId(folderIds[0], QMessageDataComparator::NotEqual) &
             QMessageFolderFilter::byDisplayName("X-A", QMessageDataComparator::Includes) )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("id equality OR displayName equality")
        << ( QMessageFolderFilter::byId(folderIds[0], QMessageDataComparator::Equal) |
             QMessageFolderFilter::byDisplayName("X-Announce", QMessageDataComparator::Equal) )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[3] );
}

void tst_QMessageStoreKeys::testFolderFilter()
{
    QFETCH(QMessageFolderFilter, filter);
    QFETCH(QMessageFolderIdList, ids);
    QFETCH(QMessageFolderIdList, negatedIds);

    if (filter.isSupported()) {
        QVERIFY(filter == filter);
        QCOMPARE(filter != QMessageFolderFilter(), !filter.isEmpty());

        // Order is irrelevant for filtering
        QCOMPARE(QMessageStore::instance()->queryFolders(filter).toSet().subtract(existingFolderIds), ids.toSet());
        QCOMPARE(QMessageStore::instance()->queryFolders(~filter).toSet().subtract(existingFolderIds), negatedIds.toSet());
    } else {
        ++unsupportedCount;
    }
}

void tst_QMessageStoreKeys::testFolderOrdering_data()
{
    QTest::addColumn<QMessageFolderOrdering>("ordering");
    QTest::addColumn<QMessageFolderIdList>("ids");

    QTest::newRow("path ascending")
        << QMessageFolderOrdering::byPath(Qt::AscendingOrder)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] << folderIds[0] );

    QTest::newRow("path descending")
        << QMessageFolderOrdering::byPath(Qt::DescendingOrder)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[3] << folderIds[2] << folderIds[1] );

    QTest::newRow("displayName ascending")
        << QMessageFolderOrdering::byDisplayName(Qt::AscendingOrder)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[0] << folderIds[2] << folderIds[3] );

    QTest::newRow("displayName descending")
        << QMessageFolderOrdering::byDisplayName(Qt::DescendingOrder)
        << ( QMessageFolderIdList() << folderIds[3] << folderIds[2] << folderIds[0] << folderIds[1] );
}

void tst_QMessageStoreKeys::testFolderOrdering()
{
    QFETCH(QMessageFolderOrdering, ordering);
    QFETCH(QMessageFolderIdList, ids);

    if (ordering.isSupported()) {
        QVERIFY(ordering == ordering);
        QCOMPARE(ordering != QMessageFolderOrdering(), !ordering.isEmpty());

        // Filter out the existing folders
        QMessageFolderIdList sortedIds(QMessageStore::instance()->queryFolders(QMessageFolderFilter(), ordering));
        for (QMessageFolderIdList::iterator it = sortedIds.begin(); it != sortedIds.end(); ) {
            if (existingFolderIds.contains(*it)) {
                it = sortedIds.erase(it);
            } else {
                ++it;
            }
        }

        QCOMPARE(sortedIds, ids);
    } else {
        ++unsupportedCount;
    }
}

void tst_QMessageStoreKeys::testMessageFilter_data()
{
    QTest::addColumn<QMessageFilter>("filter");
    QTest::addColumn<QMessageIdList>("ids");
    QTest::addColumn<QMessageIdList>("negatedIds");
    QTest::addColumn<QString>("body");

    QTest::newRow("empty filter")
        << QMessageFilter() 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("id equality 1")
        << QMessageFilter::byId(messageIds[0], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("id equality 2")
        << QMessageFilter::byId(messageIds[1], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] )
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("id equality invalid")
        << QMessageFilter::byId(QMessageId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("id inequality 1")
        << QMessageFilter::byId(messageIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("id inequality 2")
        << QMessageFilter::byId(messageIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] )
        << "";

    QTest::newRow("id inequality invalid")
        << QMessageFilter::byId(QMessageId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("id list inclusion 1")
        << QMessageFilter::byId(QMessageIdList() << messageIds[0], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("id list inclusion 2")
        << QMessageFilter::byId(QMessageIdList() << messageIds[1], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] )
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("id list inclusion 3")
        << QMessageFilter::byId(QMessageIdList() << messageIds[0] << messageIds[1], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] )
        << ( QMessageIdList() << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("id list inclusion empty")
        << QMessageFilter::byId(QMessageIdList(), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("id list exclusion 1")
        << QMessageFilter::byId(QMessageIdList() << messageIds[0], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("id list exclusion 2")
        << QMessageFilter::byId(QMessageIdList() << messageIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] )
        << "";

    QTest::newRow("id list exclusion 3")
        << QMessageFilter::byId(QMessageIdList() << messageIds[0] << messageIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] )
        << "";

    QTest::newRow("id list exclusion empty")
        << QMessageFilter::byId(QMessageIdList(), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("type equality 1")
        << QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::Equal) 
#ifdef Q_OS_WIN
        << QMessageIdList()
        << messageIds
#else
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
#endif
        << "";

    QTest::newRow("type equality 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Equal) 
#ifdef Q_OS_WIN
        << messageIds
        << QMessageIdList()
#else
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
#endif
        << "";

    QTest::newRow("type equality invalid")
        << QMessageFilter::byType(QMessage::NoType, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("type inequality 1")
        << QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::NotEqual) 
#ifdef Q_OS_WIN
        << messageIds
        << QMessageIdList()
#else
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
#endif
        << "";

    QTest::newRow("type inequality 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::NotEqual) 
#ifdef Q_OS_WIN
        << QMessageIdList()
        << messageIds
#else
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
#endif
        << "";

    QTest::newRow("type inequality invalid")
        << QMessageFilter::byType(QMessage::NoType, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("type mask inclusion 1")
        << QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::Includes) 
#ifdef Q_OS_WIN
        << QMessageIdList()
        << messageIds
#else
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
#endif
        << "";

    QTest::newRow("type mask inclusion 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Includes) 
#ifdef Q_OS_WIN
        << messageIds
        << QMessageIdList()
#else
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
#endif
        << "";

    QTest::newRow("type mask inclusion 3")
        << QMessageFilter::byType(QMessage::Sms | QMessage::Email, QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("type mask inclusion empty")
        << QMessageFilter::byType(QMessage::NoType, QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("type mask exclusion 1")
        << QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::Excludes) 
#ifdef Q_OS_WIN
        << messageIds
        << QMessageIdList()
#else
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
#endif
        << "";

    QTest::newRow("type mask exclusion 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Excludes) 
#ifdef Q_OS_WIN
        << QMessageIdList()
        << messageIds
#else
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
#endif
        << "";

    QTest::newRow("type mask exclusion 3")
        << QMessageFilter::byType(QMessage::Sms | QMessage::Email, QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("type mask exclusion empty")
        << QMessageFilter::byType(QMessage::NoType, QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("sender equality 1")
        << QMessageFilter::bySender("Esteemed.Colleague@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] )
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("sender equality 2")
        << QMessageFilter::bySender("sysadmin@example.com", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << "";

    QTest::newRow("sender equality non-matching")
        << QMessageFilter::bySender("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("sender equality empty")
        << QMessageFilter::bySender(QString(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("sender equality zero-length")
        << QMessageFilter::bySender("", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("sender inequality 1")
        << QMessageFilter::bySender("Esteemed.Colleague@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] )
        << "";

    QTest::newRow("sender inequality 2")
        << QMessageFilter::bySender("sysadmin@example.com", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] )
        << "";

    QTest::newRow("sender inequality non-matching")
        << QMessageFilter::bySender("Nonesuch", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("sender inequality empty")
        << QMessageFilter::bySender(QString(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("sender inequality zero-length")
        << QMessageFilter::bySender("", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("sender inclusion 1")
        << QMessageFilter::bySender("example", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("sender inclusion 2")
        << QMessageFilter::bySender("ozone", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("sender inclusion non-matching")
        << QMessageFilter::bySender("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("sender inclusion empty")
        << QMessageFilter::bySender(QString(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("sender inclusion zero-length")
        << QMessageFilter::bySender("", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("sender exclusion 1")
        << QMessageFilter::bySender("example", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("sender exclusion 2")
        << QMessageFilter::bySender("ozone", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("sender exclusion non-matching")
        << QMessageFilter::bySender("Nonesuch", QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("sender exclusion empty")
        << QMessageFilter::bySender(QString(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("sender exclusion zero-length")
        << QMessageFilter::bySender("", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("recipients inclusion 1")
        << QMessageFilter::byRecipients("example", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("recipients inclusion 2")
        << QMessageFilter::byRecipients("Mega", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("recipients inclusion non-matching")
        << QMessageFilter::byRecipients("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("recipients inclusion empty")
        << QMessageFilter::byRecipients(QString(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("recipients inclusion zero-length")
        << QMessageFilter::byRecipients("", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("recipients exclusion 1")
        << QMessageFilter::byRecipients("example", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("recipients exclusion 2")
        << QMessageFilter::byRecipients("Mega", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("recipients exclusion non-matching")
        << QMessageFilter::byRecipients("Nonesuch", QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("recipients exclusion empty")
        << QMessageFilter::byRecipients(QString(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("recipients exclusion zero-length")
        << QMessageFilter::byRecipients("", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("subject equality 1")
        << QMessageFilter::bySubject("Free beer", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";

    QTest::newRow("subject equality 2")
        << QMessageFilter::bySubject("Scheduled maintenance", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << "";

    QTest::newRow("subject equality non-matching")
        << QMessageFilter::bySubject("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("subject equality empty")
        << QMessageFilter::bySubject(QString(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("subject equality zero-length")
        << QMessageFilter::bySubject("", QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("subject inequality 1")
        << QMessageFilter::bySubject("Free beer", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] )
        << "";

    QTest::newRow("subject inequality 2")
        << QMessageFilter::bySubject("Scheduled maintenance", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] )
        << "";

    QTest::newRow("subject inequality non-matching")
        << QMessageFilter::bySubject("Nonesuch", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("subject inequality empty")
        << QMessageFilter::bySubject(QString(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("subject inequality zero-length")
        << QMessageFilter::bySubject("", QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("subject inclusion 1")
        << QMessageFilter::bySubject("vation", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("subject inclusion 2")
        << QMessageFilter::bySubject(" ", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("subject inclusion non-matching")
        << QMessageFilter::bySubject("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("subject inclusion empty")
        << QMessageFilter::bySubject(QString(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("subject inclusion zero-length")
        << QMessageFilter::bySubject("", QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("subject exclusion 1")
        << QMessageFilter::bySubject("vation", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] )
        << "";

    QTest::newRow("subject exclusion 2")
        << QMessageFilter::bySubject(" ", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("subject exclusion non-matching")
        << QMessageFilter::bySubject("Nonesuch", QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("subject exclusion empty")
        << QMessageFilter::bySubject(QString(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("subject exclusion zero-length")
        << QMessageFilter::bySubject("", QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("timeStamp equality 1")
        << QMessageFilter::byTimeStamp(QDateTime::fromString("1999-04-01T10:30:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";

    QTest::newRow("timeStamp equality 2")
        << QMessageFilter::byTimeStamp(QDateTime::fromString("2000-01-01T13:00:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << "";

    QTest::newRow("timeStamp equality non-matching")
        << QMessageFilter::byTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("timeStamp equality empty")
        << QMessageFilter::byTimeStamp(QDateTime(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("timeStamp inequality 1")
        << QMessageFilter::byTimeStamp(QDateTime::fromString("1999-04-01T10:30:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] )
        << "";

    QTest::newRow("timeStamp inequality 2")
        << QMessageFilter::byTimeStamp(QDateTime::fromString("2000-01-01T13:00:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] << messageIds[2] )
        << "";

    QTest::newRow("timeStamp inequality non-matching")
        << QMessageFilter::byTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("timeStamp inequality empty")
        << QMessageFilter::byTimeStamp(QDateTime(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QDateTime epoch(QDateTime::fromString("2000-01-01T12:00:00Z", Qt::ISODate));

    QTest::newRow("timeStamp less than")
        << QMessageFilter::byTimeStamp(epoch, QMessageDataComparator::LessThan) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";

    QTest::newRow("timeStamp less than equal")
        << QMessageFilter::byTimeStamp(epoch, QMessageDataComparator::LessThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] << messageIds[3] )
        << "";

    QTest::newRow("timeStamp greater than")
        << QMessageFilter::byTimeStamp(epoch, QMessageDataComparator::GreaterThan) 
        << ( QMessageIdList() << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << "";

    QTest::newRow("timeStamp greater than equal")
        << QMessageFilter::byTimeStamp(epoch, QMessageDataComparator::GreaterThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] )
        << "";

    QTest::newRow("receptionTimeStamp equality 1")
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("1999-04-01T10:31:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";

    QTest::newRow("receptionTimeStamp equality 2")
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("2000-01-01T13:05:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("receptionTimeStamp equality non-matching")
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("receptionTimeStamp equality empty")
        << QMessageFilter::byReceptionTimeStamp(QDateTime(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("receptionTimeStamp inequality 1")
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("1999-04-01T10:31:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] )
        << "";

    QTest::newRow("receptionTimeStamp inequality 2")
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("2000-01-01T13:05:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] )
        << "";

    QTest::newRow("receptionTimeStamp inequality non-matching")
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("1990-01-01T01:00:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("receptionTimeStamp inequality empty")
        << QMessageFilter::byReceptionTimeStamp(QDateTime(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("receptionTimeStamp less than")
        << QMessageFilter::byReceptionTimeStamp(epoch, QMessageDataComparator::LessThan) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";

    QTest::newRow("receptionTimeStamp less than equal")
        << QMessageFilter::byReceptionTimeStamp(epoch, QMessageDataComparator::LessThanEqual) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";

    QTest::newRow("receptionTimeStamp greater than")
        << QMessageFilter::byReceptionTimeStamp(epoch, QMessageDataComparator::GreaterThan) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] )
        << "";

    QTest::newRow("receptionTimeStamp greater than equal")
        << QMessageFilter::byReceptionTimeStamp(epoch, QMessageDataComparator::GreaterThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] )
        << "";

    QTest::newRow("status equality 1")
        << QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << "";

    QTest::newRow("status equality 2")
        << QMessageFilter::byStatus(QMessage::HasAttachments, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] )
        << "";

    QTest::newRow("status equality 3")
        << QMessageFilter::byStatus(QMessage::Removed, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("status inequality 1")
        << QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("status inequality 2")
        << QMessageFilter::byStatus(QMessage::HasAttachments, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] )
        << "";

    QTest::newRow("status inequality 3")
        << QMessageFilter::byStatus(QMessage::Removed, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("status mask inclusion 1")
        << QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << "";

    QTest::newRow("status mask inclusion 2")
        << QMessageFilter::byStatus(QMessage::Read | QMessage::HasAttachments, QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";

    QTest::newRow("status mask inclusion 3")
        << QMessageFilter::byStatus(QMessage::Read | QMessage::Removed, QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("status mask inclusion empty")
        << QMessageFilter::byStatus(static_cast<QMessage::StatusFlags>(0), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("status mask exclusion 1")
        << QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("status mask exclusion 2")
        << QMessageFilter::byStatus(QMessage::Read | QMessage::HasAttachments, QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("status mask exclusion 3")
        << QMessageFilter::byStatus(QMessage::Read | QMessage::Removed, QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("status mask exclusion empty")
        << QMessageFilter::byStatus(static_cast<QMessage::StatusFlags>(0), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("priority equality 1")
        << QMessageFilter::byPriority(QMessage::HighPriority, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("priority equality 2")
        << QMessageFilter::byPriority(QMessage::LowPriority, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";

    QTest::newRow("priority equality 3")
        << QMessageFilter::byPriority(QMessage::NormalPriority, QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] )
        << "";

    QTest::newRow("priority inequality 1")
        << QMessageFilter::byPriority(QMessage::HighPriority, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << "";

    QTest::newRow("priority inequality 2")
        << QMessageFilter::byPriority(QMessage::LowPriority, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] )
        << "";

    QTest::newRow("priority inequality 3")
        << QMessageFilter::byPriority(QMessage::NormalPriority, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] )
        << "";

    QTest::newRow("size equality 1")
        << QMessageFilter::bySize(messageSizes[3], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << "";

    QTest::newRow("size equality 2")
        << QMessageFilter::bySize(messageSizes[2], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("size equality non-matching")
        << QMessageFilter::bySize(13, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("size equality zero")
        << QMessageFilter::bySize(0, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("size inequality 1")
        << QMessageFilter::bySize(messageSizes[3], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] )
        << "";

    QTest::newRow("size inequality 2")
        << QMessageFilter::bySize(messageSizes[2], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] )
        << "";

    QTest::newRow("size inequality non-matching")
        << QMessageFilter::bySize(13, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("size inequality empty")
        << QMessageFilter::bySize(0, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

#if defined(Q_OS_WIN) && defined(_WIN32_WCE)
	// Sizes are ordered differently on CE
    uint discriminator(messageSizes[2]);
#else
    uint discriminator(messageSizes[1]);
#endif

    QTest::newRow("size less than")
        << QMessageFilter::bySize(discriminator, QMessageDataComparator::LessThan) 
#if defined(Q_OS_WIN) && defined(_WIN32_WCE)
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] << messageIds[3] )
#else
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
#endif
        << "";

    QTest::newRow("size less than equal")
        << QMessageFilter::bySize(discriminator, QMessageDataComparator::LessThanEqual) 
#if defined(Q_OS_WIN) && defined(_WIN32_WCE)
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] )
#else
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] )
#endif
        << "";

    QTest::newRow("size greater than")
        << QMessageFilter::bySize(discriminator, QMessageDataComparator::GreaterThan) 
#if defined(Q_OS_WIN) && defined(_WIN32_WCE)
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
#else
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
#endif
        << "";

    QTest::newRow("size greater than equal")
        << QMessageFilter::bySize(discriminator, QMessageDataComparator::GreaterThanEqual) 
#if defined(Q_OS_WIN) && defined(_WIN32_WCE)
        << ( QMessageIdList() << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
#else
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
#endif
        << "";

    QTest::newRow("parentAccountId equality 1")
        << QMessageFilter::byParentAccountId(accountIds[0], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("parentAccountId equality 2")
        << QMessageFilter::byParentAccountId(accountIds[2], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("parentAccountId equality invalid")
        << QMessageFilter::byParentAccountId(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("parentAccountId inequality 1")
        << QMessageFilter::byParentAccountId(accountIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("parentAccountId inequality 2")
        << QMessageFilter::byParentAccountId(accountIds[2], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("parentAccountId inequality invalid")
        << QMessageFilter::byParentAccountId(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("parentAccountId filter inclusion 1")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("parentAccountId filter inclusion 2")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("parentAccountId filter inclusion 3")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("parentAccountId filter inclusion empty")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("parentAccountId filter inclusion non-matching")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("parentAccountId filter exclusion 1")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("Alter Ego", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("parentAccountId filter exclusion 2")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("parentAccountId filter exclusion 3")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("o", QMessageDataComparator::Includes), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("parentAccountId filter exclusion empty")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("parentAccountId filter exclusion non-matching")
        << QMessageFilter::byParentAccountId(QMessageAccountFilter::byName("NoneSuch"), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("standardFolder equality 1")
        << QMessageFilter::byStandardFolder(QMessage::InboxFolder, QMessageDataComparator::Equal) 
        << messageIds
        << ( QMessageIdList() )
        << "";

    QTest::newRow("standardFolder equality 2")
        << QMessageFilter::byStandardFolder(QMessage::TrashFolder, QMessageDataComparator::Equal) 
        << ( QMessageIdList() )
        << messageIds
        << "";

    QTest::newRow("standardFolder inequality 1")
        << QMessageFilter::byStandardFolder(QMessage::InboxFolder, QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() )
        << messageIds
        << "";

    QTest::newRow("standardFolder inequality 2")
        << QMessageFilter::byStandardFolder(QMessage::TrashFolder, QMessageDataComparator::NotEqual) 
        << messageIds
        << ( QMessageIdList() )
        << "";

    QTest::newRow("parentFolderId equality 1")
        << QMessageFilter::byParentFolderId(folderIds[0], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("parentFolderId equality 2")
        << QMessageFilter::byParentFolderId(folderIds[1], QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("parentFolderId equality invalid")
        << QMessageFilter::byParentFolderId(QMessageFolderId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("parentFolderId inequality 1")
        << QMessageFilter::byParentFolderId(folderIds[0], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("parentFolderId inequality 2")
        << QMessageFilter::byParentFolderId(folderIds[1], QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << "";

    QTest::newRow("parentFolderId inequality invalid")
        << QMessageFilter::byParentFolderId(QMessageFolderId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("parentFolderId filter inclusion 1")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("My messages", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("parentFolderId filter inclusion 2")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("parentFolderId filter inclusion 3")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("parentFolderId filter inclusion empty")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("parentFolderId filter inclusion non-matching")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("parentFolderId filter exclusion 1")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("My messages", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("parentFolderId filter exclusion 2")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << "";

    QTest::newRow("parentFolderId filter exclusion 3")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("parentFolderId filter exclusion empty")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter(), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("parentFolderId filter exclusion non-matching")
        << QMessageFilter::byParentFolderId(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("ancestorFolderIds inclusion 1")
        << QMessageFilter::byAncestorFolderIds(folderIds[1], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << "";

    QTest::newRow("ancestorFolderIds inclusion 2")
        << QMessageFilter::byAncestorFolderIds(folderIds[2], QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";

    QTest::newRow("ancestorFolderIds inclusion invalid")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderId(), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("ancestorFolderIds exclusion 1")
        << QMessageFilter::byAncestorFolderIds(folderIds[1], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("ancestorFolderIds exclusion 2")
        << QMessageFilter::byAncestorFolderIds(folderIds[2], QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] )
        << "";

    QTest::newRow("ancestorFolderIds exclusion invalid")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderId(), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("ancestorFolderIds filter inclusion 1")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byDisplayName("Innbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << "";

    QTest::newRow("ancestorFolderIds filter inclusion 2")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byDisplayName("X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";

    QTest::newRow("ancestorFolderIds filter inclusion 3")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << "";

    QTest::newRow("ancestorFolderIds filter inclusion empty")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter(), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << "";

    QTest::newRow("ancestorFolderIds filter inclusion non-matching")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("ancestorFolderIds filter exclusion 1")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byDisplayName("Innbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("ancestorFolderIds filter exclusion 2")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byDisplayName("X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] )
        << "";

    QTest::newRow("ancestorFolderIds filter exclusion 3")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("Innbox", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("ancestorFolderIds filter exclusion empty")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter(), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("ancestorFolderIds filter exclusion non-matching")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byPath("NoneSuch"), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList()
        << "";

    // Test some basic combinations
    QTest::newRow("status mask inclusion AND timeStamp greater than")
        << ( QMessageFilter::byStatus(QMessage::Read, QMessageDataComparator::Includes) &
             QMessageFilter::byTimeStamp(epoch, QMessageDataComparator::GreaterThan) )
        << ( QMessageIdList() << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[4] )
        << "";

    QTest::newRow("size greater than equal OR timeStamp greater than")
        << ( QMessageFilter::bySize(discriminator, QMessageDataComparator::GreaterThanEqual) |
             QMessageFilter::byTimeStamp(epoch, QMessageDataComparator::GreaterThan) )
#if defined(Q_OS_WIN) && defined(_WIN32_WCE)
        << ( QMessageIdList() << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
#else
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[4] )
#endif
        << "";

    QTest::newRow("sender inclusion AND timeStamp greater than")
        << ( QMessageFilter::bySender("Boss", QMessageDataComparator::Includes) &
             QMessageFilter::byTimeStamp(epoch, QMessageDataComparator::GreaterThan) )
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("subject inclusion OR subject exclusion")
        << ( QMessageFilter::bySubject("agenda", QMessageDataComparator::Includes) |
             QMessageFilter::bySubject("ee", QMessageDataComparator::Excludes) )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[4] )
        << "";

    QTest::newRow("subject inclusion OR subject exclusion")
        << ( QMessageFilter::bySubject("agenda", QMessageDataComparator::Includes) |
             QMessageFilter::bySubject("ee", QMessageDataComparator::Excludes) )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[4] )
        << "";

    QTest::newRow("body")
        << QMessageFilter()
        << ( QMessageIdList() << messageIds[0] << messageIds[2] )
        << QMessageIdList() // contains body but does not match filter
        << "summer";

    QTest::newRow("body and subject")
        << QMessageFilter::bySubject("message", QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] ) // contains body but does not match filter
        << "summer";
}

void tst_QMessageStoreKeys::testMessageFilter()
{
    QFETCH(QMessageFilter, filter);
    QFETCH(QMessageIdList, ids);
    QFETCH(QMessageIdList, negatedIds);
    QFETCH(QString, body);

    if (filter.isSupported()) {
        QVERIFY(filter == filter);
        QCOMPARE(filter != QMessageFilter(), !filter.isEmpty());
        
        // Order is irrelevant for filtering
        if (body.isEmpty()) {
            QCOMPARE(QMessageStore::instance()->queryMessages(filter&~existingAccountsFilter).toSet().subtract(existingMessageIds), ids.toSet());
            QCOMPARE(QMessageStore::instance()->queryMessages(~filter&~existingAccountsFilter).toSet().subtract(existingMessageIds), negatedIds.toSet());
        } else {
            QCOMPARE(QMessageStore::instance()->queryMessages(filter&~existingAccountsFilter, body).toSet().subtract(existingMessageIds), ids.toSet());
            QCOMPARE(QMessageStore::instance()->queryMessages(~filter&~existingAccountsFilter, body).toSet().subtract(existingMessageIds), negatedIds.toSet());
        }
    } else {
        ++unsupportedCount;
    }
}

void tst_QMessageStoreKeys::testMessageOrdering_data()
{
    QTest::addColumn<QMessageOrdering>("ordering");
    QTest::addColumn<MessageListList>("ids");

    QTest::newRow("type ascending")
        << QMessageOrdering::byType(Qt::AscendingOrder)
#ifdef Q_OS_WIN
        << ( MessageListList() << messageIds ); // All messages are Email type
#else
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] ) );
#endif

    QTest::newRow("type descending")
        << QMessageOrdering::byType(Qt::DescendingOrder)
#ifdef Q_OS_WIN
        << ( MessageListList() << messageIds ); // All messages are Email type
#else
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );
#endif

    QTest::newRow("sender ascending")
        << QMessageOrdering::bySender(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] ) );

    QTest::newRow("sender descending")
        << QMessageOrdering::bySender(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] << messageIds[4] ) );

    QTest::newRow("recipients ascending")
        << QMessageOrdering::byRecipients(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] << messageIds[4] ) );

    QTest::newRow("recipients descending")
        << QMessageOrdering::byRecipients(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] ) );

    QTest::newRow("subject ascending")
        << QMessageOrdering::bySubject(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] ) );

    QTest::newRow("subject descending")
        << QMessageOrdering::bySubject(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("timeStamp ascending")
        << QMessageOrdering::byTimeStamp(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] << messageIds[3] ) );

    QTest::newRow("timeStamp descending")
        << QMessageOrdering::byTimeStamp(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[1] )
                               << ( QMessageIdList() << messageIds[4] ) );

	QTest::newRow("receptionTimeStamp ascending")
        << QMessageOrdering::byReceptionTimeStamp(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] ) );

    QTest::newRow("receptionTimeStamp descending")
        << QMessageOrdering::byReceptionTimeStamp(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("priority ascending")
        << QMessageOrdering::byPriority(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] ) );

    QTest::newRow("priority descending")
        << QMessageOrdering::byPriority(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("size ascending")
        << QMessageOrdering::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
#if defined(Q_OS_WIN)
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
#else
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
#endif
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] ) );

    QTest::newRow("size descending")
        << QMessageOrdering::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
#if defined(Q_OS_WIN)
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
#else
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
#endif
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("status:HasAttachments ascending")
        << QMessageOrdering::byStatus(QMessage::HasAttachments, Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] ) );

    QTest::newRow("status:HasAttachments descending")
        << QMessageOrdering::byStatus(QMessage::HasAttachments, Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] ) );

    QTest::newRow("status:Read ascending")
        << QMessageOrdering::byStatus(QMessage::Read, Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] ) );

    QTest::newRow("status:Read descending")
        << QMessageOrdering::byStatus(QMessage::Read, Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] ) );

    QTest::newRow("type ascending, priority ascending, size ascending")
        << QMessageOrdering::byType(Qt::AscendingOrder) + QMessageOrdering::byPriority(Qt::AscendingOrder) + QMessageOrdering::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] ) );

    QTest::newRow("type ascending, priority ascending, size descending")
        << QMessageOrdering::byType(Qt::AscendingOrder) + QMessageOrdering::byPriority(Qt::AscendingOrder) + QMessageOrdering::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] ) );

    QTest::newRow("type ascending, priority descending, size ascending")
        << QMessageOrdering::byType(Qt::AscendingOrder) + QMessageOrdering::byPriority(Qt::DescendingOrder) + QMessageOrdering::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("type ascending, priority descending, size descending")
        << QMessageOrdering::byType(Qt::AscendingOrder) + QMessageOrdering::byPriority(Qt::DescendingOrder) + QMessageOrdering::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );

    QTest::newRow("type descending, priority ascending, size ascending")
        << QMessageOrdering::byType(Qt::DescendingOrder) + QMessageOrdering::byPriority(Qt::AscendingOrder) + QMessageOrdering::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority ascending, size descending")
        << QMessageOrdering::byType(Qt::DescendingOrder) + QMessageOrdering::byPriority(Qt::AscendingOrder) + QMessageOrdering::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority descending, size ascending")
        << QMessageOrdering::byType(Qt::DescendingOrder) + QMessageOrdering::byPriority(Qt::DescendingOrder) + QMessageOrdering::bySize(Qt::AscendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority descending, size descending")
        << QMessageOrdering::byType(Qt::DescendingOrder) + QMessageOrdering::byPriority(Qt::DescendingOrder) + QMessageOrdering::bySize(Qt::DescendingOrder)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );
}

void tst_QMessageStoreKeys::testMessageOrdering()
{
    QFETCH(QMessageOrdering, ordering);
    QFETCH(MessageListList, ids);

    if (ordering.isSupported()) {
        QVERIFY(ordering == ordering);
        QCOMPARE(ordering != QMessageOrdering(), !ordering.isEmpty());

        // Filter out the existing messages
        QMessageIdList sortedIds(QMessageStore::instance()->queryMessages(~existingAccountsFilter, ordering));
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
    } else {
        ++unsupportedCount;
    }
}

