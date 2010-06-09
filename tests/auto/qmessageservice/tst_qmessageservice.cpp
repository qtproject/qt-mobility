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

#include "qtmessaging.h"
#include "../support/support.h"

#if defined(Q_OS_SYMBIAN) || (defined(Q_OS_WIN) && defined(_WIN32_WCE))
# if defined(TESTDATA_DIR)
#  undef TESTDATA_DIR
# endif
# define TESTDATA_DIR "."
#endif

// Will try to wait for the condition while allowing event processing
#define QTRY_VERIFY(__expr) \
    do { \
        const int __step = 50; \
        const int __timeout = 90000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)

//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE

class SignalCatcher : public QObject
{
    Q_OBJECT

public:
    SignalCatcher(QObject* parent = 0);

    void reset();

public slots:
    void messagesCounted(int count);
    void messagesFound(const QMessageIdList& messageList);
    void stateChanged(QMessageService::State s);
    void progressChanged(int,int);

public:
    QMessageIdList ids;
    int count;
    QMessageService::State state;
    int progressMin, progressMax;
};

SignalCatcher::SignalCatcher(QObject* parent)
:
QObject(parent)
{
    reset();
}

void SignalCatcher::reset()
{
    ids.clear();
    count = 0;
    progressMin = 0;
    progressMax = 0;
    state = QMessageService::InactiveState;
}

void SignalCatcher::messagesCounted(int c)
{
    count = c;
}

void SignalCatcher::messagesFound(const QMessageIdList& results)
{
    ids.append(results);
}

void SignalCatcher::stateChanged(QMessageService::State s)
{
    state = s;
}

void SignalCatcher::progressChanged(int min, int max)
{
    progressMin = min; progressMax = max;
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

/*
    Unit test for QMessageService class.
*/
class tst_QMessageService : public QObject
{
    Q_OBJECT

public:
    tst_QMessageService();
    virtual ~tst_QMessageService();

private slots:

    void init();
    void cleanUp();

    void initTestCase();
    void cleanupTestCase();

    void testQueryMessages();
    void testQueryMessages_data();

    void testCountMessages();
    void testCountMessages_data();

    void testSend();

    void testCompose();

    void testRetrieveHeader();

    void testRetrieveBody();

    void testRetrieve();

    void testShow();

    void testExportUpdates();

private:
    void testQueryCountData();

private:
    QMessageService* testService;

    QSet<QMessageFolderId> existingFolderIds;
    QMessageFolderIdList folderIds;

    QSet<QMessageAccountId> existingAccountIds;
    QMessageAccountIdList accountIds;

    QSet<QMessageId> existingMessageIds;
    QMessageIdList messageIds;

    QMessageFilter existingAccountsFilter;

    QList<int> messageSizes;
};

Q_DECLARE_METATYPE(QMessageAccountIdList)
Q_DECLARE_METATYPE(QMessageAccountFilter)
Q_DECLARE_METATYPE(QMessageAccountSortOrder)

Q_DECLARE_METATYPE(QMessageFolderIdList)
Q_DECLARE_METATYPE(QMessageFolderFilter)
Q_DECLARE_METATYPE(QMessageFolderSortOrder)

Q_DECLARE_METATYPE(QMessageIdList)
Q_DECLARE_METATYPE(QMessageFilter)
Q_DECLARE_METATYPE(QMessageSortOrder)

QTEST_MAIN(tst_QMessageService)

#include "tst_qmessageservice.moc"

tst_QMessageService::tst_QMessageService()
{
}

tst_QMessageService::~tst_QMessageService()
{
}

void tst_QMessageService::init()
{
    testService = new QMessageService(this);
}

void tst_QMessageService::cleanUp()
{
    if(testService) delete testService; testService = 0;
}

void tst_QMessageService::initTestCase()
{
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    if (!Support::mapiAvailable())
        QSKIP("Skipping tests because a MAPI subsystem does not appear to be available", SkipAll);
#endif

    Support::clearMessageStore();

    QMessageManager* manager = new QMessageManager(this);

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

    existingMessageIds = manager->queryMessages(~existingAccountsFilter).toSet();

    // For windows at least, we can't have HasAttachments set without a real attachment
    const char *attachmentPaths = TESTDATA_DIR "/testdata/1.txt";

    QList<Support::Parameters> messageParams;
    messageParams << Params()("parentAccountName", "Alter Ego")
        ("parentFolderPath", "My messages")
#if (defined(Q_OS_SYMBIAN) || defined(Q_OS_WIN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)) // SMS messages must be in SMS store on Windows and on Symbian
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

void tst_QMessageService::cleanupTestCase()
{
}

void tst_QMessageService::testQueryMessages()
{
    QFETCH(QMessageFilter, filter);
    QFETCH(QMessageIdList, ids);
    QFETCH(QMessageIdList, negatedIds);
    QFETCH(QString, body);

    if (filter.isSupported()) {
        QVERIFY(filter == filter);
        QCOMPARE(filter != QMessageFilter(), !filter.isEmpty());

        SignalCatcher sc(this);
        connect(testService,SIGNAL(messagesFound(const QMessageIdList&)),&sc,SLOT(messagesFound(const QMessageIdList&)));
#if (defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
        connect(testService,SIGNAL(stateChanged(QMessageService::State)),&sc,SLOT(stateChanged(QMessageService::State)));
#endif        

        // Order is irrelevant for filtering

        if (body.isEmpty()) {
            sc.reset();
            QCOMPARE(testService->queryMessages(filter&~existingAccountsFilter),true);
#if (defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
            QTRY_VERIFY(sc.state == QMessageService::FinishedState);
#else            
            while(testService->state() == QMessageService::ActiveState)
                qApp->processEvents();
#endif            
            QCOMPARE(sc.ids.toSet().subtract(existingMessageIds),ids.toSet());

            sc.reset();

            QCOMPARE(testService->queryMessages(~filter&~existingAccountsFilter),true);
#if (defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
            QTRY_VERIFY(sc.state == QMessageService::FinishedState);
#else            
            while(testService->state() == QMessageService::ActiveState)
                qApp->processEvents();
#endif            
            QCOMPARE(sc.ids.toSet().subtract(existingMessageIds),negatedIds.toSet());

        } else {

            sc.reset();

            QCOMPARE(testService->queryMessages(filter&~existingAccountsFilter,body),true);
#if (defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
            QTRY_VERIFY(sc.state == QMessageService::FinishedState);
#else            
            while(testService->state() == QMessageService::ActiveState)
                qApp->processEvents();
#endif            
            QCOMPARE(sc.ids.toSet().subtract(existingMessageIds),ids.toSet());

            sc.reset();

            QCOMPARE(testService->queryMessages(~filter&~existingAccountsFilter,body),true);
#if (defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
            QTRY_VERIFY(sc.state == QMessageService::FinishedState);
#else            
            while(testService->state() == QMessageService::ActiveState)
                qApp->processEvents();
#endif            
            QCOMPARE(sc.ids.toSet().subtract(existingMessageIds),negatedIds.toSet());
        }
    } else {
        QSKIP("Unsupported for this configuration", SkipSingle);
    }
}

void tst_QMessageService::testQueryCountData()
{
    QTest::addColumn<QMessageFilter>("filter");
    QTest::addColumn<QMessageIdList>("ids");
    QTest::addColumn<QMessageIdList>("negatedIds");
    QTest::addColumn<QString>("body");

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("empty filter")
        << QMessageFilter() 
        << messageIds
        << QMessageIdList()
        << "";
#endif

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

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("id list inclusion empty")
        << QMessageFilter::byId(QMessageIdList(), QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
        << "";
#endif

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

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("id list exclusion empty")
        << QMessageFilter::byId(QMessageIdList(), QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList()
        << "";
#endif

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#endif

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
#endif

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

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#endif

    QTest::newRow("status equality 3")
        << QMessageFilter::byStatus(QMessage::Removed, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#endif

    QTest::newRow("status inequality 3")
        << QMessageFilter::byStatus(QMessage::Removed, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#endif

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

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#endif

    QTest::newRow("status mask exclusion empty")
        << QMessageFilter::byStatus(static_cast<QMessage::StatusFlags>(0), QMessageDataComparator::Excludes) 
        << QMessageIdList()
        << messageIds
        << "";

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#endif

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

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#endif

    QTest::newRow("parentAccountId equality invalid")
        << QMessageFilter::byParentAccountId(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#endif

    QTest::newRow("parentAccountId inequality invalid")
        << QMessageFilter::byParentAccountId(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
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
#endif

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    QTest::newRow("standardFolder equality 1")
        << QMessageFilter::byStandardFolder(QMessage::InboxFolder, QMessageDataComparator::Equal)
#ifndef Q_OS_SYMBIAN    
        << messageIds
        << ( QMessageIdList() )
#else // Created folders are not mapped to any Standard Folder in Symbian 
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
#else // Created folders are not mapped to any Standard Folder in Symbian & Maemo
        // <=> No messages will be returned, if messages are searched using Standard Folder Filter     
        << ( QMessageIdList() )
        << ( QMessageIdList() )
#endif        
        << "";

    QTest::newRow("standardFolder inequality 1")
        << QMessageFilter::byStandardFolder(QMessage::InboxFolder, QMessageDataComparator::NotEqual) 
#if !defined(Q_OS_SYMBIAN) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
        << ( QMessageIdList() )
        << messageIds
#else // Created folders are not mapped to any Standard Folder in Symbian & Maemo
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
#else // Created folders are not mapped to any Standard Folder in Symbian & Maemo
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

    QTest::newRow("subject inclusion OR subject exclusion")
        << ( QMessageFilter::bySubject("agenda", QMessageDataComparator::Includes) |
                QMessageFilter::bySubject("ee", QMessageDataComparator::Excludes) )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] )
        << ( QMessageIdList() << messageIds[0] << messageIds[4] )
        << "";

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

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
    // Test options
    QMessageFilter caseInsensitive1(QMessageFilter::bySubject("free beer", QMessageDataComparator::Equal));
    QTest::newRow("options:caseInsensitive 1")
        << caseInsensitive1
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";
#endif

    QMessageFilter caseSensitive1(QMessageFilter::bySubject("free beer", QMessageDataComparator::Equal));
    caseSensitive1.setMatchFlags(QMessageDataComparator::MatchCaseSensitive);
    QTest::newRow("options:caseSensitive 1")
        << caseSensitive1
        << QMessageIdList()
        << messageIds
        << "";

    QMessageFilter caseSensitive2(QMessageFilter::bySubject("Free beer", QMessageDataComparator::Equal));
    caseSensitive2.setMatchFlags(QMessageDataComparator::MatchCaseSensitive);
    QTest::newRow("options:caseSensitive 2")
        << caseSensitive2
        << ( QMessageIdList() << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] << messageIds[1] << messageIds[2] << messageIds[3] )
        << "";
}

void tst_QMessageService::testQueryMessages_data()
{
    testQueryCountData();
}

void tst_QMessageService::testCountMessages()
{
    QFETCH(QMessageFilter, filter);
    QFETCH(QMessageIdList, ids);
    QFETCH(QMessageIdList, negatedIds);
    QFETCH(QString, body);

    if (filter.isSupported()) {
        QVERIFY(filter == filter);
        QCOMPARE(filter != QMessageFilter(), !filter.isEmpty());

        SignalCatcher sc(this);
        connect(testService,SIGNAL(messagesCounted(int)),&sc,SLOT(messagesCounted(int)));
#ifdef Q_OS_SYMBIAN
        connect(testService,SIGNAL(stateChanged(QMessageService::State)),&sc,SLOT(stateChanged(QMessageService::State)));
#endif        

        // Order is irrelevant for filtering

        if(body.isEmpty()) {
            QCOMPARE(testService->countMessages(filter&~existingAccountsFilter),true);
#ifdef Q_OS_SYMBIAN            
            QTRY_VERIFY(sc.state == QMessageService::FinishedState);
#else            
            while(testService->state() == QMessageService::ActiveState)
                qApp->processEvents();
#endif            
            QCOMPARE(sc.count-existingMessageIds.count(), ids.count());

            QCOMPARE(testService->countMessages(~filter&~existingAccountsFilter),true);
#ifdef Q_OS_SYMBIAN            
            QTRY_VERIFY(sc.state == QMessageService::FinishedState);
#else            
            while(testService->state() == QMessageService::ActiveState)
                qApp->processEvents();
#endif            
            QCOMPARE(sc.count-existingMessageIds.count(), negatedIds.count());
        }

    } else {
        QSKIP("Unsupported for this configuration", SkipSingle);
    }
}

void tst_QMessageService::testCountMessages_data()
{
    testQueryCountData();
}

void tst_QMessageService::testSend()
{
    QMessage testMessage;
    testMessage.setType(QMessage::Email);
    bool result = testService->send(testMessage);
    QVERIFY(result == true || result == false);
}

void tst_QMessageService::testCompose()
{
    QMessage testMessage;
    testMessage.setType(QMessage::Email);
    bool result = testService->compose(testMessage);
    QVERIFY(result == true || result == false);
}


void tst_QMessageService::testRetrieveHeader()
{
    bool result = testService->retrieveHeader(QMessageId());
    QCOMPARE(result,false);
}

void tst_QMessageService::testRetrieveBody()
{
    bool result = testService->retrieveBody(QMessageId());
    QCOMPARE(result,false);
}

void tst_QMessageService::testRetrieve()
{
    bool result = testService->retrieve(QMessageId(),QMessageContentContainerId());
    QCOMPARE(result,false);
}

void tst_QMessageService::testShow()
{
    QMessageId testId;
    bool result = testService->show(testId);
    QCOMPARE(result,false);
}

void tst_QMessageService::testExportUpdates()
{
    bool result = testService->exportUpdates(QMessageAccountId());
    QCOMPARE(result,false);
}
