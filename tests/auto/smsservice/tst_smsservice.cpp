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
class tst_SMSService : public QObject
{
    Q_OBJECT

public:
    tst_SMSService();
    virtual ~tst_SMSService();

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

    void testShow();

    void testSearch();
    void testSearch_data();
    
    void testError();

private:
    void testQueryCountData();
    QMessage newSmsMessage(const QString &to, const QString &text);

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

    QMessageAccountId smsAccountId;
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

QTEST_MAIN(tst_SMSService)

#include "tst_smsservice.moc"

tst_SMSService::tst_SMSService()
{
}

tst_SMSService::~tst_SMSService()
{
}

void tst_SMSService::init()
{
    testService = new QMessageService(this);
}

void tst_SMSService::cleanUp()
{
    if(testService) delete testService; testService = 0;
}

void tst_SMSService::initTestCase()
{

    QMessageManager* manager = new QMessageManager(this);

    Support::clearMessageStore();
    manager->removeMessages(QMessageFilter());

    existingAccountIds = manager->queryAccounts().toSet();


    existingAccountsFilter = ~QMessageFilter();
    foreach(QMessageAccountId id, existingAccountIds) {
        existingAccountsFilter |= QMessageFilter::byParentAccountId(id);
    }

    existingMessageIds = manager->queryMessages(~existingAccountsFilter).toSet();

    QList<Support::Parameters> messageParams;
    messageParams << Params()
        ("type", "sms")
        ("to", "+358443144813")
        ("from", "+358409144835")
        ("subject", "Ice to meet you")
        ("text", "Shall I compare thee to a summers day")
        ("date", "2000-01-01T12:00:00Z")
        ("receivedDate", "2000-01-01T12:01:00Z")
        ("priority", "")
        ("status-read", "true")
        ("custom-flagged", "true")
    << Params()
        ("type", "sms")
        ("to", "+358409644123") 
        ("from", "+49405644999")
        ("subject", "Meeting agenda")
        ("text", "Some text")
        ("date", "2000-01-01T12:00:00Z")
        ("receivedDate", "2000-01-01T20:00:00Z")
        ("priority", "High")
        ("custom-flagged", "true")
    << Params()
        ("type", "sms")
        ("to", "+358409644123,+358401234666") 
        ("from", "+4940333666")
        ("subject", "Motivational message")
        ("text", "School's out for summer")
        ("date", "2000-01-01T13:00:00Z")
        ("receivedDate", "2000-01-01T13:05:00Z")
        ("priority", "High")
    << Params()
        ("type", "sms")
        ("to", "+35840312366,+3584123288")
        ("from", "+4940632789")
        ("subject", "Scheduled maintenance")
        ("text", "Even more text")
        ("date", "2000-01-01T13:00:00Z")
        ("receivedDate", "2000-01-01T13:00:01Z")
        ("priority", "")
        ("status-read", "true")
    << Params()
        ("type", "sms")
        ("to", "+35840312366")
        ("from", "+4940333666")
        ("subject", "Free beer")
        ("text", "More and more text")
        ("date", "1999-04-01T10:30:00Z")
        ("receivedDate", "1999-04-01T10:31:00Z")
        ("priority", "Low")
        ("status-read", "true");

    smsAccountId = QMessageAccount::defaultAccount(QMessage::Sms);
    QVERIFY(smsAccountId.isValid());

    foreach (const Support::Parameters &params, messageParams) {
	QString to(params["to"]);
	QString from(params["from"]);
	QString date(params["date"]);
	QString receivedDate(params["receivedDate"]);
	QString subject(params["subject"]);
	QString text(params["text"]);
	QString type(params["type"]);
	QString priority(params["priority"]);
	QString read(params["status-read"]);

	QMessage msg;

	msg.setParentAccountId(smsAccountId);
	msg.setType(QMessage::Sms);

	QList<QMessageAddress> toList;
	foreach (const QString &addr, to.split(",", QString::SkipEmptyParts)) {
	    toList.append(QMessageAddress(QMessageAddress::Phone, addr.trimmed()));
	}

	msg.setTo(toList);

	msg.setFrom(QMessageAddress(QMessageAddress::Phone, from));
	msg.setSubject(subject);

	QDateTime dt(QDateTime::fromString(date, Qt::ISODate));
	dt.setTimeSpec(Qt::UTC);
	msg.setDate(dt);
	msg.setBody(text);

	if (!receivedDate.isEmpty()) {
	    QDateTime dt(QDateTime::fromString(receivedDate, Qt::ISODate));
	    dt.setTimeSpec(Qt::UTC);
	    msg.setReceivedDate(dt);
	}

	QMessage::StatusFlags flags(0);
	if (read.toLower() == "true") {
	    flags |= QMessage::Read;
	}
	msg.setStatus(flags);

	if (!priority.isEmpty()) {
	    if (priority.toLower() == "high") {
		msg.setPriority(QMessage::HighPriority);
	    } else if (priority.toLower() == "low") {
		msg.setPriority(QMessage::LowPriority);
	    }
	}

	QVERIFY(manager->addMessage(&msg));
	QVERIFY(msg.id().isValid());

	messageIds.append(msg.id());

        messageSizes.append(msg.size());
    }

}

void tst_SMSService::cleanupTestCase()
{
}

void tst_SMSService::testQueryMessages()
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
            QCOMPARE(testService->queryMessages(filter),true);
#if (defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
            QTRY_VERIFY(sc.state == QMessageService::FinishedState);
#else            
            while(testService->state() == QMessageService::ActiveState)
                qApp->processEvents();
#endif            
            QCOMPARE(sc.ids.toSet().subtract(existingMessageIds),ids.toSet());

            sc.reset();

            QCOMPARE(testService->queryMessages(~filter),true);
#if (defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
            QTRY_VERIFY(sc.state == QMessageService::FinishedState);
#else            
            while(testService->state() == QMessageService::ActiveState)
                qApp->processEvents();
#endif            
            QCOMPARE(sc.ids.toSet().subtract(existingMessageIds),negatedIds.toSet());

        } else {

            sc.reset();

            QCOMPARE(testService->queryMessages(filter,body),true);
#if (defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
            QTRY_VERIFY(sc.state == QMessageService::FinishedState);
#else            
            while(testService->state() == QMessageService::ActiveState)
                qApp->processEvents();
#endif            
            QCOMPARE(sc.ids.toSet().subtract(existingMessageIds),ids.toSet());

            sc.reset();

            QCOMPARE(testService->queryMessages(~filter,body),true);
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

void tst_SMSService::testQueryCountData()
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
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("type equality 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("type equality invalid")
        << QMessageFilter::byType(QMessage::NoType, QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("type inequality 1")
        << QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::NotEqual) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("type inequality 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("type inequality invalid")
        << QMessageFilter::byType(QMessage::NoType, QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("type mask inclusion 1")
        << QMessageFilter::byType(QMessage::Sms, QMessageDataComparator::Includes) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("type mask inclusion 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Includes) 
        << QMessageIdList()
        << messageIds
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
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("type mask exclusion 2")
        << QMessageFilter::byType(QMessage::Email, QMessageDataComparator::Excludes) 
        << messageIds
        << QMessageIdList()
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

#if !defined(Q_WS_MAEMO_5)
    QTest::newRow("sender equality 1")
        << QMessageFilter::bySender("+49405644999", QMessageDataComparator::Equal) 
        << ( QMessageIdList() << messageIds[1] )
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("sender equality 2")
        << QMessageFilter::bySender("+4940632789", QMessageDataComparator::Equal) 
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

#if !defined(Q_WS_MAEMO_5)
    QTest::newRow("sender inequality 1")
        << QMessageFilter::bySender("+49405644999", QMessageDataComparator::NotEqual) 
        << ( QMessageIdList() << messageIds[0] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[1] )
        << "";

    QTest::newRow("sender inequality 2")
        << QMessageFilter::bySender("+4940632789", QMessageDataComparator::NotEqual) 
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
        << QMessageFilter::bySender("940", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("sender inclusion 2")
        << QMessageFilter::bySender("584", QMessageDataComparator::Includes) 
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
        << QMessageFilter::bySender("940", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("sender exclusion 2")
        << QMessageFilter::bySender("0914", QMessageDataComparator::Excludes) 
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
        << QMessageFilter::byRecipients("123", QMessageDataComparator::Includes) 
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << ( QMessageIdList() << messageIds[0] )
        << "";

    QTest::newRow("recipients inclusion 2")
        << QMessageFilter::byRecipients("813", QMessageDataComparator::Includes) 
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
        << QMessageFilter::byRecipients("123", QMessageDataComparator::Excludes) 
        << ( QMessageIdList() << messageIds[0] )
        << ( QMessageIdList() << messageIds[1] << messageIds[2] << messageIds[3] << messageIds[4] )
        << "";

    QTest::newRow("recipients exclusion 2")
        << QMessageFilter::byRecipients("813", QMessageDataComparator::Excludes) 
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

    QTest::newRow("parentAccountId equality")
        << QMessageFilter::byParentAccountId(smsAccountId, QMessageDataComparator::Equal) 
        << messageIds
        << QMessageIdList()
        << "";

    QTest::newRow("parentAccountId equality invalid")
        << QMessageFilter::byParentAccountId(QMessageAccountId(), QMessageDataComparator::Equal) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("parentAccountId inequality")
        << QMessageFilter::byParentAccountId(smsAccountId, QMessageDataComparator::NotEqual) 
        << QMessageIdList()
        << messageIds
        << "";

    QTest::newRow("parentAccountId inequality invalid")
        << QMessageFilter::byParentAccountId(QMessageAccountId(), QMessageDataComparator::NotEqual) 
        << messageIds
        << QMessageIdList()
        << "";

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
    /*
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
    */
}

void tst_SMSService::testQueryMessages_data()
{
    testQueryCountData();
}

void tst_SMSService::testCountMessages()
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
            QCOMPARE(testService->countMessages(filter),true);
#ifdef Q_OS_SYMBIAN            
            QTRY_VERIFY(sc.state == QMessageService::FinishedState);
#else            
            while(testService->state() == QMessageService::ActiveState)
                qApp->processEvents();
#endif            
            QCOMPARE(sc.count-existingMessageIds.count(), ids.count());

            QCOMPARE(testService->countMessages(~filter),true);
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

void tst_SMSService::testCountMessages_data()
{
    testQueryCountData();
}

QMessage tst_SMSService::newSmsMessage(const QString &to, const QString &text)
{
    QMessage msg;
    
    msg.setParentAccountId(smsAccountId);
    msg.setType(QMessage::Sms);

    QList<QMessageAddress> toList;
    foreach (const QString &addr, to.split(",", QString::SkipEmptyParts)) {
	toList.append(QMessageAddress(QMessageAddress::Phone, addr.trimmed()));
    }

    msg.setTo(toList);
    //msg.setFrom(QMessageAddress(QMessageAddress::Phone, from));
    msg.setDate(QDateTime::currentDateTime());
    msg.setBody(text);

    return msg;
}

void tst_SMSService::testSend()
{
    QString to("+358XXXXXXX");
    QString to2("+358XXXXXXX,+358_INVALID");

    SignalCatcher sc(this);
    connect(testService, SIGNAL(stateChanged(QMessageService::State)), &sc, SLOT(stateChanged(QMessageService::State)));

    QMessage m1 = newSmsMessage(to, "1st message");
    QMessage m2 = newSmsMessage(to, "2nd message");

    QVERIFY(testService->send(m1) == true);
    QCOMPARE(testService->send(m2), false);

    QTRY_VERIFY(sc.state == QMessageService::FinishedState);
    
    QCOMPARE(testService->error(), QMessageManager::NoError);

    QMessageService otherService(this);
    SignalCatcher sc2(this);
    connect(&otherService, SIGNAL(stateChanged(QMessageService::State)), &sc2, SLOT(stateChanged(QMessageService::State)));

    QMessage m3 = newSmsMessage(to, "3rd message");
    QMessage m4 = newSmsMessage(to2, "4th message");

    QVERIFY(testService->send(m3) == true);
    QVERIFY(otherService.send(m4) == true);

    QTRY_VERIFY(sc.state == QMessageService::FinishedState);
    QTRY_VERIFY(sc2.state == QMessageService::FinishedState);

    QCOMPARE(testService->error(), QMessageManager::NoError);
    QCOMPARE(otherService.error() != QMessageManager::NoError, true);

    QMessageIdList ids = QMessageManager().queryMessages(QMessageFilter::byType(QMessage::Sms), m1.textContent());
    QCOMPARE(ids.count() > 0, true);
}

void tst_SMSService::testCompose()
{
    QMessage testMessage;
    testMessage.setType(QMessage::Email);
    bool result = testService->compose(testMessage);
    QVERIFY(result == true || result == false);
}

void tst_SMSService::testShow()
{
    QMessageId testId;
    bool result = testService->show(testId);
    QCOMPARE(result,false);
}

void tst_SMSService::testSearch()
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
        connect(testService,SIGNAL(stateChanged(QMessageService::State)),&sc,SLOT(stateChanged(QMessageService::State)));
	sc.reset();

	QCOMPARE(testService->queryMessages(filter,body),true);
#if (defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
	QTRY_VERIFY(sc.state == QMessageService::FinishedState);
#else            
	while(testService->state() == QMessageService::ActiveState)
	    qApp->processEvents();
#endif            
	QCOMPARE(sc.ids.toSet().subtract(existingMessageIds),ids.toSet());

	sc.reset();

	QCOMPARE(testService->queryMessages(~filter,body),true);
#if (defined(Q_OS_SYMBIAN) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6))
	QTRY_VERIFY(sc.state == QMessageService::FinishedState);
#else            
	while(testService->state() == QMessageService::ActiveState)
	    qApp->processEvents();
#endif            
	QCOMPARE(sc.ids.toSet().subtract(existingMessageIds),negatedIds.toSet());
        
    } else {
        QSKIP("Unsupported for this configuration", SkipSingle);
    }

    QCOMPARE(QMessageManager().queryMessages(filter, body).toSet().subtract(existingMessageIds), ids.toSet());
    QCOMPARE(QMessageManager().queryMessages(~filter, body).toSet().subtract(existingMessageIds), negatedIds.toSet());

}

void tst_SMSService::testSearch_data()
{
    QTest::addColumn<QMessageFilter>("filter");
    QTest::addColumn<QMessageIdList>("ids");
    QTest::addColumn<QMessageIdList>("negatedIds");
    QTest::addColumn<QString>("body");

    QTest::newRow("body")
        << QMessageFilter()
        << ( QMessageIdList() << messageIds[0] << messageIds[2] )
        << QMessageIdList() // contains body but does not match filter
        << "summer";

    QTest::newRow("body and subject")
        << QMessageFilter::bySender("+4940333666", QMessageDataComparator::Equal)
        << ( QMessageIdList() << messageIds[2] )
        << ( QMessageIdList() << messageIds[0] ) // contains body but does not match filter
        << "summer";
}

void tst_SMSService::testError()
{
    QMessageId invalidId;
    
    QVERIFY(invalidId.isValid() == false);

    QCOMPARE(testService->retrieve(invalidId, QMessageContentContainerId()), false);
    QCOMPARE(testService->error(), QMessageManager::InvalidId);

    QCOMPARE(testService->retrieveHeader(invalidId), false);
    QCOMPARE(testService->error(), QMessageManager::InvalidId);
    
    QCOMPARE(testService->retrieveBody(invalidId), false);
    QCOMPARE(testService->error(), QMessageManager::InvalidId);

    QCOMPARE(testService->exportUpdates(QMessageAccountId()), false);
    QCOMPARE(testService->error(), QMessageManager::InvalidId);

    QCOMPARE(testService->show(invalidId), false);
    QCOMPARE(testService->error(), QMessageManager::InvalidId);

}
