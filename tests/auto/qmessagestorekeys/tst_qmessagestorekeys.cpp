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
#include <QObject>
#include <QTest>
#include <QDebug>

#include "qmessagestore.h"
#include "../support/support.h"

#if (defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
# if defined(TESTDATA_DIR)
#  undef TESTDATA_DIR
# endif
# define TESTDATA_DIR "/var/tmp"
#elif (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) && defined(_WIN32_WCE))
# if defined(TESTDATA_DIR)
#  undef TESTDATA_DIR
# endif
# define TESTDATA_DIR "."
#endif

//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE

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

    QMessageManager *manager;
};

Q_DECLARE_METATYPE(QMessageAccountIdList)
Q_DECLARE_METATYPE(QMessageAccountFilter)
Q_DECLARE_METATYPE(QMessageAccountSortOrder)

Q_DECLARE_METATYPE(QMessageFolderIdList)
Q_DECLARE_METATYPE(QMessageFolderFilter)
Q_DECLARE_METATYPE(QMessageFolderSortOrder)

typedef QList<QMessageFolderSortOrder> FolderSortList;
Q_DECLARE_METATYPE(FolderSortList)

Q_DECLARE_METATYPE(QMessageIdList)
Q_DECLARE_METATYPE(QMessageFilter)
Q_DECLARE_METATYPE(QMessageSortOrder)

typedef QList<QMessageSortOrder> MessageSortList;
Q_DECLARE_METATYPE(MessageSortList)

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
    : manager(0)
{
}

tst_QMessageStoreKeys::~tst_QMessageStoreKeys()
{
    delete manager;
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
}

void tst_QMessageStoreKeys::initTestCase()
{
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    if (!Support::mapiAvailable())
        QSKIP("Skipping tests because a MAPI subsystem does not appear to be available", SkipAll);
#endif

    // Clear any existing data before we instantiate the message manager
    Support::clearMessageStore();

    manager = new QMessageManager;

    existingAccountIds = manager->queryAccounts().toSet();
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

    existingFolderIds = manager->queryFolders().toSet();

    QList<Support::Parameters> folderParams;
    folderParams << Params()("parentAccountName", "Alter Ego")
                            ("path", "My messages")
                            ("name", "My messages")
                            ("parentFolderPath", "")
                 << Params()("parentAccountName", "Work")
                            ("path", "Innbox")
                            ("name", "Innbox")
                            ("parentFolderPath", "")
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
                 << Params()("parentAccountName", "Work")
                            ("path", "Innbox/X-Announce")
                            ("name", "X-Announce")
                            ("parentFolderPath", "Innbox")
                 << Params()("parentAccountName", "Work")
                            ("path", "Innbox/X-Announce/X-Archived")
                            ("name", "X-Archived")
                            ("parentFolderPath", "Innbox/X-Announce");
#else    
                 << Params()("parentAccountName", "Work")
                            ("path", "X-Announce")
                            ("name", "X-Announce")
                            ("parentFolderPath", "")
                 << Params()("parentAccountName", "Work")
                            ("path", "X-Archived")
                            ("name", "X-Archived")
                            ("parentFolderPath", "");
#endif    

    foreach (const Support::Parameters &params, folderParams) {
        folderIds.append(Support::addFolder(params));
        QVERIFY(folderIds.last().isValid());
    }

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    // Local folders can be seen through every account
    // => Make sure that existingFolderIds contains all folderIds
    //    but not ids which were returned from addFolder calls
    QMessageFolderFilter folderFilter;
    foreach(QMessageAccountId id, accountIds) {
        folderFilter |= QMessageFolderFilter::byParentAccountId(id);
    }
    QSet<QMessageFolderId> newFolderIds = manager->queryFolders(folderFilter).toSet().subtract(existingFolderIds);
    newFolderIds = newFolderIds.subtract(folderIds.toSet());
    existingFolderIds.unite(newFolderIds);
#endif

    existingMessageIds = manager->queryMessages(~existingAccountsFilter).toSet();

    // For windows at least, we can't have HasAttachments set without a real attachment
#ifndef Q_OS_SYMBIAN
    const char *attachmentPaths = TESTDATA_DIR "/testdata/1.txt";
#else
    const char *attachmentPaths = TESTDATA_DIR "\\testdata\\1.txt";
#endif

    QList<Support::Parameters> messageParams;
    messageParams << Params()("parentAccountName", "Alter Ego")
                             ("parentFolderPath", "My messages")
#if (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN)) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6) // SMS messages must be in SMS store on Windows and on Symbian
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
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#else
                  << Params()("parentAccountName", "Work")
                             ("parentFolderPath", "X-Announce")
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
                             ("parentFolderPath", "X-Archived")
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
#endif

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

    QMessageAccountFilter andEquals(QMessageAccountFilter::byId(accountIds[0], QMessageDataComparator::NotEqual));
    andEquals &= QMessageAccountFilter::byName("Personal", QMessageDataComparator::Equal);
    QTest::newRow("id inequality AND name equality")
        << andEquals
        << ( QMessageAccountIdList() << accountIds[1] )
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] );

    QMessageAccountFilter orEquals(QMessageAccountFilter::byId(accountIds[0], QMessageDataComparator::Equal));
    orEquals |= QMessageAccountFilter::byName("Personal", QMessageDataComparator::NotEqual);
    QTest::newRow("id equality OR name inequality")
        << orEquals
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[2] )
        << ( QMessageAccountIdList() << accountIds[1] );

    // Test matchFlags
    QMessageAccountFilter caseInsensitive1(QMessageAccountFilter::byName("work", QMessageDataComparator::Equal));
    QTest::newRow("matchFlags:caseInsensitive 1")
        << caseInsensitive1
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );

    QMessageAccountFilter caseSensitive1(QMessageAccountFilter::byName("work", QMessageDataComparator::Equal));
    caseSensitive1.setMatchFlags(QMessageDataComparator::MatchCaseSensitive);
    QTest::newRow("matchFlags:caseSensitive 1")
        << caseSensitive1
        << QMessageAccountIdList()
        << accountIds;

    QMessageAccountFilter caseSensitive2(QMessageAccountFilter::byName("Work", QMessageDataComparator::Equal));
    caseSensitive2.setMatchFlags(QMessageDataComparator::MatchCaseSensitive);
    QTest::newRow("matchFlags:caseSensitive 2")
        << caseSensitive2
        << ( QMessageAccountIdList() << accountIds[0] )
        << ( QMessageAccountIdList() << accountIds[1] << accountIds[2] );
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
        QCOMPARE(manager->queryAccounts(filter).toSet().subtract(existingAccountIds), ids.toSet());
        QCOMPARE(manager->queryAccounts(~filter).toSet().subtract(existingAccountIds), negatedIds.toSet());
    } else {
        QSKIP("Unsupported for this configuration", SkipSingle);
    }
}

void tst_QMessageStoreKeys::testAccountOrdering_data()
{
    QTest::addColumn<QMessageAccountSortOrder>("sortOrder");
    QTest::addColumn<QMessageAccountIdList>("ids");

    QTest::newRow("name ascending")
        << QMessageAccountSortOrder::byName(Qt::AscendingOrder)
        << ( QMessageAccountIdList() << accountIds[2] << accountIds[1] << accountIds[0] );

    QTest::newRow("name descending")
        << QMessageAccountSortOrder::byName(Qt::DescendingOrder)
        << ( QMessageAccountIdList() << accountIds[0] << accountIds[1] << accountIds[2] );
}

void tst_QMessageStoreKeys::testAccountOrdering()
{
    QFETCH(QMessageAccountSortOrder, sortOrder);
    QFETCH(QMessageAccountIdList, ids);

    if (sortOrder.isSupported()) {
        QVERIFY(sortOrder == sortOrder);
        QCOMPARE(sortOrder != QMessageAccountSortOrder(), !sortOrder.isEmpty());

        // Filter out the existing accounts
        QMessageAccountIdList sortedIds(manager->queryAccounts(QMessageAccountFilter(), sortOrder));
        for (QMessageAccountIdList::iterator it = sortedIds.begin(); it != sortedIds.end(); ) {
            if (existingAccountIds.contains(*it)) {
                it = sortedIds.erase(it);
            } else {
                ++it;
            }
        }

        QCOMPARE(sortedIds, ids);
    } else {
        QSKIP("Unsupported for this configuration", SkipSingle);
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

#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("id filter inclusion 2")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("Innbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("id filter inclusion 3")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("box/X-Ann", QMessageDataComparator::Includes), QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );
#endif    

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

#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("id filter exclusion 2")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("Innbox/X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("id filter exclusion 3")
        << QMessageFolderFilter::byId(QMessageFolderFilter::byPath("box/X-Ann", QMessageDataComparator::Includes), QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );
#endif    

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

#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("path equality 2")
        << QMessageFolderFilter::byPath("Innbox/X-Announce", QMessageDataComparator::Equal)
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );
#endif    

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

#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("path inequality 2")
        << QMessageFolderFilter::byPath("Innbox/X-Announce", QMessageDataComparator::NotEqual)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );
#endif    

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

#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("path inclusion 2")
        << QMessageFolderFilter::byPath("box/X-Ann", QMessageDataComparator::Includes)
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );
#endif    

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

#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("path exclusion 2")
        << QMessageFolderFilter::byPath("box/X-Ann", QMessageDataComparator::Excludes)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );
#endif    

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

    QTest::newRow("name equality 1")
        << QMessageFolderFilter::byName("My messages", QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("name equality 2")
        << QMessageFolderFilter::byName("X-Announce", QMessageDataComparator::Equal) 
        << ( QMessageFolderIdList() << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] );

    QTest::newRow("name equality non-matching")
        << QMessageFolderFilter::byName("Nonesuch", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("name equality empty")
        << QMessageFolderFilter::byName(QString(), QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("name equality zero-length")
        << QMessageFolderFilter::byName("", QMessageDataComparator::Equal) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("name inequality 1")
        << QMessageFolderFilter::byName("My messages", QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("name inequality 2")
        << QMessageFolderFilter::byName("X-Announce", QMessageDataComparator::NotEqual) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[2] );

    QTest::newRow("name inequality non-matching")
        << QMessageFolderFilter::byName("Nonesuch", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("name inequality empty")
        << QMessageFolderFilter::byName(QString(), QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("name inequality zero-length")
        << QMessageFolderFilter::byName("", QMessageDataComparator::NotEqual) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("name inclusion 1")
        << QMessageFolderFilter::byName("message", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[0] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] );

    QTest::newRow("name inclusion 2")
        << QMessageFolderFilter::byName("X-A", QMessageDataComparator::Includes) 
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("name inclusion non-matching")
        << QMessageFolderFilter::byName("Nonesuch", QMessageDataComparator::Includes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("name inclusion empty")
        << QMessageFolderFilter::byName(QString(), QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("name inclusion zero-length")
        << QMessageFolderFilter::byName("", QMessageDataComparator::Includes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("name exclusion 1")
        << QMessageFolderFilter::byName("messages", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] );

    QTest::newRow("name exclusion 2")
        << QMessageFolderFilter::byName("X-A", QMessageDataComparator::Excludes) 
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] );

    QTest::newRow("name exclusion non-matching")
        << QMessageFolderFilter::byName("Nonesuch", QMessageDataComparator::Excludes) 
        << folderIds
        << QMessageFolderIdList();

    QTest::newRow("name exclusion empty")
        << QMessageFolderFilter::byName(QString(), QMessageDataComparator::Excludes) 
        << QMessageFolderIdList()
        << folderIds;

    QTest::newRow("name exclusion zero-length")
        << QMessageFolderFilter::byName("", QMessageDataComparator::Excludes) 
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

#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#endif    

    // Test some basic combinations
    QTest::newRow("id inequality AND name inclusion")
        << ( QMessageFolderFilter::byId(folderIds[0], QMessageDataComparator::NotEqual) &
             QMessageFolderFilter::byName("X-A", QMessageDataComparator::Includes) )
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QTest::newRow("id equality OR name equality")
        << ( QMessageFolderFilter::byId(folderIds[0], QMessageDataComparator::Equal) |
             QMessageFolderFilter::byName("X-Announce", QMessageDataComparator::Equal) )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[3] );

    QMessageFolderFilter andEquals(QMessageFolderFilter::byId(folderIds[0], QMessageDataComparator::NotEqual));
    andEquals &= QMessageFolderFilter::byName("X-A", QMessageDataComparator::Includes);
    QTest::newRow("QMessageFolderFilter::operator&=")
        << andEquals
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );

    QMessageFolderFilter orEquals(QMessageFolderFilter::byId(folderIds[0], QMessageDataComparator::Equal));
    orEquals |= QMessageFolderFilter::byName("X-Announce", QMessageDataComparator::Equal);
    QTest::newRow("QMessageFolderFilter::operator|=")
        << orEquals
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[2] )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[3] );

    // Test matchFlags
    QMessageFolderFilter caseInsensitive1(QMessageFolderFilter::byName("x-a", QMessageDataComparator::Includes));
    QTest::newRow("matchFlags:caseInsensitive 1")
        << caseInsensitive1
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );
    
    QMessageFolderFilter caseSensitive1(QMessageFolderFilter::byName("x-a", QMessageDataComparator::Includes));
    caseSensitive1.setMatchFlags(QMessageDataComparator::MatchCaseSensitive);
    QTest::newRow("matchFlags:caseSensitive 1")
        << caseSensitive1
        << QMessageFolderIdList()
        << folderIds;

    QMessageFolderFilter caseSensitive2(QMessageFolderFilter::byName("X-A", QMessageDataComparator::Includes));
    caseSensitive2.setMatchFlags(QMessageDataComparator::MatchCaseSensitive);
    QTest::newRow("matchFlags:caseSensitive 2")
        << caseSensitive2
        << ( QMessageFolderIdList() << folderIds[2] << folderIds[3] )
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[1] );
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
        QCOMPARE(manager->queryFolders(filter).toSet().subtract(existingFolderIds), ids.toSet());
        QCOMPARE(manager->queryFolders(~filter).toSet().subtract(existingFolderIds), negatedIds.toSet());
    } else {
        QSKIP("Unsupported for this configuration", SkipSingle);
    }
}

void tst_QMessageStoreKeys::testFolderOrdering_data()
{
    QTest::addColumn<FolderSortList>("sorts");
    QTest::addColumn<QMessageFolderIdList>("ids");

    QTest::newRow("path ascending")
        << ( FolderSortList() << QMessageFolderSortOrder::byPath(Qt::AscendingOrder) )
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] << folderIds[0] );
#else
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[0] << folderIds[2] << folderIds[3] );
#endif

    QTest::newRow("path descending")
        << ( FolderSortList() << QMessageFolderSortOrder::byPath(Qt::DescendingOrder) )
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
        << ( QMessageFolderIdList() << folderIds[0] << folderIds[3] << folderIds[2] << folderIds[1] );
#else
        << ( QMessageFolderIdList() << folderIds[3] << folderIds[2] << folderIds[0] << folderIds[1] );
#endif

    QTest::newRow("name ascending")
        << ( FolderSortList() << QMessageFolderSortOrder::byName(Qt::AscendingOrder) )
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[0] << folderIds[2] << folderIds[3] );

    QTest::newRow("name descending")
        << ( FolderSortList() << QMessageFolderSortOrder::byName(Qt::DescendingOrder) )
        << ( QMessageFolderIdList() << folderIds[3] << folderIds[2] << folderIds[0] << folderIds[1] );
    
    QTest::newRow("path ascending + name ascending")
        << ( FolderSortList() << QMessageFolderSortOrder::byPath(Qt::AscendingOrder) << QMessageFolderSortOrder::byName(Qt::AscendingOrder) )
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] << folderIds[0] );
#else
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[0] << folderIds[2] << folderIds[3] );
#endif

    QMessageFolderSortOrder plusEquals(QMessageFolderSortOrder::byPath(Qt::AscendingOrder));
    plusEquals += QMessageFolderSortOrder::byName(Qt::AscendingOrder);

    QTest::newRow("path ascending += name ascending")
        << ( FolderSortList() << plusEquals )
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[2] << folderIds[3] << folderIds[0] );
#else
        << ( QMessageFolderIdList() << folderIds[1] << folderIds[0] << folderIds[2] << folderIds[3] );
#endif
}

void tst_QMessageStoreKeys::testFolderOrdering()
{
    QFETCH(FolderSortList, sorts);
    QFETCH(QMessageFolderIdList, ids);

    bool supported(true);
    foreach (const QMessageFolderSortOrder &element, sorts) {
        QVERIFY(element == element);
        QCOMPARE(element != QMessageFolderSortOrder(), !element.isEmpty());

        supported &= element.isSupported();
    }

    if (supported) {
        // Filter out the existing folders
        QMessageFolderIdList sortedIds(manager->queryFolders(QMessageFolderFilter(), sorts));
        for (QMessageFolderIdList::iterator it = sortedIds.begin(); it != sortedIds.end(); ) {
            if (existingFolderIds.contains(*it)) {
                it = sortedIds.erase(it);
            } else {
                ++it;
            }
        }

        QCOMPARE(sortedIds, ids);
    } else {
        QSKIP("Unsupported for this configuration", SkipSingle);
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

    QTest::newRow("id filter inclusion empty")
        << QMessageFilter::byId(QMessageFilter(), QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("id filter inclusion non matching")
        << QMessageFilter::byId(~QMessageFilter(), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("id filter inclusion 1")
        << QMessageFilter::byId(QMessageFilter::bySubject("Free beer", QMessageDataComparator::Equal), 
                                QMessageDataComparator::Includes)
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";
    QTest::newRow("type equality 1")
        << QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::Equal) 
#if (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
        << QMessageIdList()
        << messageIds
#else
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
#endif
        << "";

    QTest::newRow("type equality 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Equal) 
#if (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
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
#if (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
        << messageIds
        << QMessageIdList()
#else
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
#endif
        << "";

    QTest::newRow("type inequality 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::NotEqual) 
#if (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
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
#if (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
        << QMessageIdList()
        << messageIds
#else
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
#endif
        << "";

    QTest::newRow("type mask inclusion 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Includes) 
#if (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
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
#if (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
        << messageIds
        << QMessageIdList()
#else
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
#endif
        << "";

    QTest::newRow("type mask exclusion 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Excludes) 
#if (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
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

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#endif

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

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#endif

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

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("sender inclusion 2")
        << QMessageFilter::bySender("ozone", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";
#endif

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

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("sender exclusion 2")
        << QMessageFilter::bySender("ozone", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";
#endif

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

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#ifndef Q_OS_SYMBIAN
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("1999-04-01T10:31:00Z", Qt::ISODate), QMessageDataComparator::Equal)
#else    
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("1999-04-01T10:30:00Z", Qt::ISODate), QMessageDataComparator::Equal) 
#endif    
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";

    QTest::newRow("receptionTimeStamp equality 2")
#ifndef Q_OS_SYMBIAN
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("2000-01-01T13:05:00Z", Qt::ISODate), QMessageDataComparator::Equal)
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
#else    
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("2000-01-01T13:00:00Z", Qt::ISODate), QMessageDataComparator::Equal)
        << ( QMessageIdList() << messageIds[3] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
#endif    
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
#ifndef Q_OS_SYMBIAN    
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("1999-04-01T10:31:00Z", Qt::ISODate), QMessageDataComparator::NotEqual)
#else    
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("1999-04-01T10:30:00Z", Qt::ISODate), QMessageDataComparator::NotEqual)
#endif    
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] )
        << "";

    QTest::newRow("receptionTimeStamp inequality 2")
#ifndef Q_OS_SYMBIAN    
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("2000-01-01T13:05:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] )
#else    
        << QMessageFilter::byReceptionTimeStamp(QDateTime::fromString("2000-01-01T13:00:00Z", Qt::ISODate), QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << ( QMessageIdList() << messageIds[3] << messageIds[2] )
#endif    
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
#ifndef Q_OS_SYMBIAN    
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
#else        
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] << messageIds[3] )
#endif
        << "";

    QTest::newRow("receptionTimeStamp greater than")
        << QMessageFilter::byReceptionTimeStamp(epoch, QMessageDataComparator::GreaterThan) 
#ifndef Q_OS_SYMBIAN    
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] )
#else        
        << ( QMessageIdList() << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
#endif
        << "";

    QTest::newRow("receptionTimeStamp greater than equal")
        << QMessageFilter::byReceptionTimeStamp(epoch, QMessageDataComparator::GreaterThanEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[4] )
        << "";
#endif

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

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("size less than")
        << QMessageFilter::bySize(discriminator, QMessageDataComparator::LessThan) 
#if defined(Q_OS_WIN) && defined(_WIN32_WCE)
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[4] )
        << ( QMessageIdList() << messageIds[2] << messageIds[3] )
#elif defined(Q_OS_SYMBIAN)
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
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
#elif defined(Q_OS_SYMBIAN)        
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
#else
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
#endif
        << "";
#endif

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

#if defined(Q_OS_WIN) || defined(Q_OS_SYMBIAN)
    QTest::newRow("standardFolder equality 1")
        << QMessageFilter::byStandardFolder(QMessage::DraftsFolder, QMessageDataComparator::Equal)
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
        << messageIds
        << ( QMessageIdList() )
#else // Created folders are not mapped to any Standard Folder in Symbian/Maemo
      // <=> No messages will be returned, if messages are searched using Standard Folder Filter     
        << ( QMessageIdList() )
        << ( QMessageIdList() )
#endif        
        << "";

    QTest::newRow("standardFolder equality 2")
        << QMessageFilter::byStandardFolder(QMessage::TrashFolder, QMessageDataComparator::Equal) 
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
        << ( QMessageIdList() )
        << messageIds
#else // Created folders are not mapped to any Standard Folder in Symbian/Maemo
      // <=> No messages will be returned, if messages are searched using Standard Folder Filter     
        << ( QMessageIdList() )
        << ( QMessageIdList() )
#endif        
        << "";

    QTest::newRow("standardFolder inequality 1")
        << QMessageFilter::byStandardFolder(QMessage::DraftsFolder, QMessageDataComparator::NotEqual) 
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
        << ( QMessageIdList() )
        << messageIds
#else // Created folders are not mapped to any Standard Folder in Symbian/Maemo
      // <=> No messages will be returned, if messages are searched using Standard Folder Filter     
        << ( QMessageIdList() )
        << ( QMessageIdList() )
#endif        
        << "";

    QTest::newRow("standardFolder inequality 2")
        << QMessageFilter::byStandardFolder(QMessage::TrashFolder, QMessageDataComparator::NotEqual) 
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
        << messageIds
        << ( QMessageIdList() )
#else // Created folders are not mapped to any Standard Folder in Symbian/Maemo
      // <=> No messages will be returned, if messages are searched using Standard Folder Filter     
        << ( QMessageIdList() )
        << ( QMessageIdList() )
#endif        
        << "";
#endif

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

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)    
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
#ifndef Q_OS_SYMBIAN    
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
#else
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
#endif
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
#ifndef Q_OS_SYMBIAN    
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
#else
        << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] )
#endif
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
#endif

#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byName("Innbox", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << "";

    QTest::newRow("ancestorFolderIds filter inclusion 2")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byName("X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Includes) 
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
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byName("Innbox", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] )
        << ( QMessageIdList() << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("ancestorFolderIds filter exclusion 2")
        << QMessageFilter::byAncestorFolderIds(QMessageFolderFilter::byName("X-Announce", QMessageDataComparator::Equal), QMessageDataComparator::Excludes) 
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
#endif    

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#elif defined(Q_OS_SYMBIAN)        
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[4] )
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
#endif

    QTest::newRow("subject inclusion OR subject exclusion")
        << ( QMessageFilter::bySubject("agenda", QMessageDataComparator::Includes) |
             QMessageFilter::bySubject("ee", QMessageDataComparator::Excludes) )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[4] )
        << "";

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QMessageFilter andEquals(QMessageFilter::bySender("Boss", QMessageDataComparator::Includes));
    andEquals &= QMessageFilter::byTimeStamp(epoch, QMessageDataComparator::GreaterThan);
    QTest::newRow("QMessageFilter::operator&=")
        << andEquals
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[3] << messageIds[4] )
        << "";
#endif

    QMessageFilter orEquals(QMessageFilter::bySubject("agenda", QMessageDataComparator::Includes));
    orEquals |= QMessageFilter::bySubject("ee", QMessageDataComparator::Excludes);
    QTest::newRow("QMessageFilter::operator|=")
        << orEquals
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[4] )
        << "";

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#endif

    // Test matchFlags
    QMessageFilter caseInsensitive1(QMessageFilter::bySubject("free beer", QMessageDataComparator::Equal));
    QTest::newRow("matchFlags:caseInsensitive 1")
        << caseInsensitive1
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";

    QMessageFilter caseSensitive1(QMessageFilter::bySubject("free beer", QMessageDataComparator::Equal));
    caseSensitive1.setMatchFlags(QMessageDataComparator::MatchCaseSensitive);
    QTest::newRow("matchFlags:caseSensitive 1")
        << caseSensitive1
        << QMessageIdList()
        << messageIds
        << "";

    QMessageFilter caseSensitive2(QMessageFilter::bySubject("Free beer", QMessageDataComparator::Equal));
    caseSensitive2.setMatchFlags(QMessageDataComparator::MatchCaseSensitive);
    QTest::newRow("matchFlags:caseSensitive 2")
        << caseSensitive2
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";
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
            QCOMPARE(manager->queryMessages(filter&~existingAccountsFilter).toSet().subtract(existingMessageIds), ids.toSet());
            QCOMPARE(manager->queryMessages(~filter&~existingAccountsFilter).toSet().subtract(existingMessageIds), negatedIds.toSet());
        } else {
            QCOMPARE(manager->queryMessages(filter&~existingAccountsFilter, body).toSet().subtract(existingMessageIds), ids.toSet());
            QCOMPARE(manager->queryMessages(~filter&~existingAccountsFilter, body).toSet().subtract(existingMessageIds), negatedIds.toSet());
        }
    } else {
        QSKIP("Unsupported for this configuration", SkipSingle);
    }
}

void tst_QMessageStoreKeys::testMessageOrdering_data()
{
    QTest::addColumn<MessageSortList>("sorts");
    QTest::addColumn<MessageListList>("ids");

    QTest::newRow("type ascending")
        << ( MessageSortList() << QMessageSortOrder::byType(Qt::AscendingOrder) )
#if (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
        << ( MessageListList() << messageIds ); // All messages are Email type
#else
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] ) );
#endif

    QTest::newRow("type descending")
        << ( MessageSortList() << QMessageSortOrder::byType(Qt::DescendingOrder) )
#if (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
        << ( MessageListList() << messageIds ); // All messages are Email type
#else
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );
#endif

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("sender ascending")
        << ( MessageSortList() << QMessageSortOrder::bySender(Qt::AscendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] ) );

    QTest::newRow("sender descending")
        << ( MessageSortList() << QMessageSortOrder::bySender(Qt::DescendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] << messageIds[4] ) );
#endif

    QTest::newRow("recipients ascending")
        << ( MessageSortList() << QMessageSortOrder::byRecipients(Qt::AscendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] << messageIds[4] ) );

    QTest::newRow("recipients descending")
        << ( MessageSortList() << QMessageSortOrder::byRecipients(Qt::DescendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] ) );

    QTest::newRow("subject ascending")
        << ( MessageSortList() << QMessageSortOrder::bySubject(Qt::AscendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] ) );

    QTest::newRow("subject descending")
        << ( MessageSortList() << QMessageSortOrder::bySubject(Qt::DescendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] ) );

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("timeStamp ascending")
        << ( MessageSortList() << QMessageSortOrder::byTimeStamp(Qt::AscendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] << messageIds[3] ) );

    QTest::newRow("timeStamp descending")
        << ( MessageSortList() << QMessageSortOrder::byTimeStamp(Qt::DescendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[1] )
                               << ( QMessageIdList() << messageIds[4] ) );

#ifndef Q_OS_SYMBIAN
    QTest::newRow("receptionTimeStamp ascending")
        << ( MessageSortList() << QMessageSortOrder::byReceptionTimeStamp(Qt::AscendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] ) );

    QTest::newRow("receptionTimeStamp descending")
        << ( MessageSortList() << QMessageSortOrder::byReceptionTimeStamp(Qt::DescendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] ) );
#endif

    QTest::newRow("priority ascending")
        << ( MessageSortList() << QMessageSortOrder::byPriority(Qt::AscendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] ) );

    QTest::newRow("priority descending")
        << ( MessageSortList() << QMessageSortOrder::byPriority(Qt::DescendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );
#endif

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("size ascending")
        << ( MessageSortList() << QMessageSortOrder::bySize(Qt::AscendingOrder) )
#if (defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
        << ( MessageListList() << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[1] )
#else    
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
#if defined(Q_OS_WIN)
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
#else
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
#endif
                               << ( QMessageIdList() << messageIds[1] )
#endif    
                               << ( QMessageIdList() << messageIds[2] ) );

    QTest::newRow("size descending")
        << ( MessageSortList() << QMessageSortOrder::bySize(Qt::DescendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] )
#if (defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] ) );
#else    
                               << ( QMessageIdList() << messageIds[1] )
#if defined(Q_OS_WIN)
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
#else
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
#endif
                               << ( QMessageIdList() << messageIds[0] ) );
#endif
#endif

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("status:HasAttachments ascending")
        << ( MessageSortList() << QMessageSortOrder::byStatus(QMessage::HasAttachments, Qt::AscendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] ) );

    QTest::newRow("status:HasAttachments descending")
        << ( MessageSortList() << QMessageSortOrder::byStatus(QMessage::HasAttachments, Qt::DescendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] ) );

    QTest::newRow("status:Read ascending")
        << ( MessageSortList() << QMessageSortOrder::byStatus(QMessage::Read, Qt::AscendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] ) );

    QTest::newRow("status:Read descending")
        << ( MessageSortList() << QMessageSortOrder::byStatus(QMessage::Read, Qt::DescendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] << messageIds[3] << messageIds[4] )
                               << ( QMessageIdList() << messageIds[1] << messageIds[2] ) );
#endif

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    // On Windows, the following tests do not vary by type (which is always Email)
    QTest::newRow("type ascending, priority ascending, size ascending")
        << ( MessageSortList() << QMessageSortOrder::byType(Qt::AscendingOrder)
                               << QMessageSortOrder::byPriority(Qt::AscendingOrder)
                               << QMessageSortOrder::bySize(Qt::AscendingOrder) )
#if defined(Q_OS_WIN)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] ) );
#elif defined(Q_OS_SYMBIAN)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] ) );
#else
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] ) );
#endif

    QTest::newRow("type ascending, priority ascending, size descending")
        << ( MessageSortList() << QMessageSortOrder::byType(Qt::AscendingOrder)
                               << QMessageSortOrder::byPriority(Qt::AscendingOrder)
                               << QMessageSortOrder::bySize(Qt::DescendingOrder) )
#if defined(Q_OS_WIN)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] ) );
#elif defined(Q_OS_SYMBIAN)
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] ) );
#else
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] ) );
#endif

    QTest::newRow("type ascending, priority descending, size ascending")
        << ( MessageSortList() << QMessageSortOrder::byType(Qt::AscendingOrder) 
                               << QMessageSortOrder::byPriority(Qt::DescendingOrder)
                               << QMessageSortOrder::bySize(Qt::AscendingOrder) )
#if defined(Q_OS_WIN)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );
#elif defined(Q_OS_SYMBIAN)
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] ) );
#else
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );
#endif

    QMessageSortOrder plus(QMessageSortOrder::byType(Qt::AscendingOrder) + 
                           QMessageSortOrder::byPriority(Qt::DescendingOrder) + 
                           QMessageSortOrder::bySize(Qt::DescendingOrder));

    QTest::newRow("type ascending, priority descending, size descending")
        << ( MessageSortList() << plus )
#if defined(Q_OS_WIN)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] ) );
#elif defined(Q_OS_SYMBIAN)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );
#else
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );
#endif

    QMessageSortOrder plusEquals(QMessageSortOrder::byType(Qt::AscendingOrder));
    plusEquals += QMessageSortOrder::byPriority(Qt::DescendingOrder);
    plusEquals += QMessageSortOrder::bySize(Qt::DescendingOrder);

    QTest::newRow("type ascending += priority descending += size descending")
        << ( MessageSortList() << plusEquals )
#if defined(Q_OS_WIN)
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[4] ) );
#elif (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );
#else
        << ( MessageListList() << ( QMessageIdList() << messageIds[0] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] ) );
#endif

#if !defined(Q_OS_WIN) && !defined(Q_OS_SYMBIAN)
    QTest::newRow("type descending, priority ascending, size ascending")
        << ( MessageSortList() << QMessageSortOrder::byType(Qt::DescendingOrder)
                               << QMessageSortOrder::byPriority(Qt::AscendingOrder)
                               << QMessageSortOrder::bySize(Qt::AscendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority ascending, size descending")
        << ( MessageSortList() << QMessageSortOrder::byType(Qt::DescendingOrder)
                               << QMessageSortOrder::byPriority(Qt::AscendingOrder)
                               << QMessageSortOrder::bySize(Qt::DescendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority descending, size ascending")
        << ( MessageSortList() << QMessageSortOrder::byType(Qt::DescendingOrder)
                               << QMessageSortOrder::byPriority(Qt::DescendingOrder)
                               << QMessageSortOrder::bySize(Qt::AscendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );

    QTest::newRow("type descending, priority descending, size descending")
        << ( MessageSortList() << QMessageSortOrder::byType(Qt::DescendingOrder)
                               << QMessageSortOrder::byPriority(Qt::DescendingOrder)
                               << QMessageSortOrder::bySize(Qt::DescendingOrder) )
        << ( MessageListList() << ( QMessageIdList() << messageIds[2] )
                               << ( QMessageIdList() << messageIds[1] )
                               << ( QMessageIdList() << messageIds[3] )
                               << ( QMessageIdList() << messageIds[4] )
                               << ( QMessageIdList() << messageIds[0] ) );
#endif
#endif
}

void tst_QMessageStoreKeys::testMessageOrdering()
{
    QFETCH(MessageSortList, sorts);
    QFETCH(MessageListList, ids);

    bool supported(true);
    foreach (const QMessageSortOrder &element, sorts) {
        QVERIFY(element == element);
        QCOMPARE(element != QMessageSortOrder(), !element.isEmpty());

        supported &= element.isSupported();
    }

    if (supported) {
        // Filter out the existing messages
        QMessageIdList sortedIds(manager->queryMessages(~existingAccountsFilter, sorts));
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
        QSKIP("Unsupported for this configuration", SkipSingle);
    }
}

