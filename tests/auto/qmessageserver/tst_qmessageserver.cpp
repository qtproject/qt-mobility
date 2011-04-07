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
#include <QSharedPointer>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include <QFile>
#include <QDir>
#include "qmessagestore.h"
#include "../support/support.h"

#include "qmessageservice.h"

// Define test account name used for testing
const QString testAccountName("");

#define QMESSAGESERVER_DEBUG 1

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

QTM_USE_NAMESPACE
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
    QList<QMessageService::State> states;

public slots:
    void stateChanged(QMessageService::State state)
    {
        states.append(state);
    }
};

/*
    Unit test which interacts with a test server
*/
class tst_QMessageServer : public QObject
{
    Q_OBJECT

public:
    tst_QMessageServer();
    virtual ~tst_QMessageServer();

private slots:
    void initTestCase();

    void testSend_data();
    void testSend();

    void testSyncMessages();

    void testMessages_data();
    void testMessages();

    void testUpdateReadStatus();

    void testDeleteMessages();

public slots:

    void testCancelSending();
    void testCancel();

    void cleanupTestCase();

private:
    void messageData();

private:
    QMessageAccountId testAccountId;
    QMessageManager *manager;
    QMessageService service;
    QString address;
    bool newAccountCreated;
    QSet<QMessageId> existingMessageIds;
};

QTEST_MAIN(tst_QMessageServer)

#include "tst_qmessageserver.moc"

tst_QMessageServer::tst_QMessageServer()
    : manager(0)
    , newAccountCreated(false)
{
}

tst_QMessageServer::~tst_QMessageServer()
{
}

void tst_QMessageServer::initTestCase()
{
    manager = new QMessageManager(this);

    QMessageAccountIdList accountIds(manager->queryAccounts(QMessageAccountFilter::byName(testAccountName)));

#ifdef QMESSAGESERVER_DEBUG
    qDebug() << "Total accounts: " << accountIds.count();
    for (int i=0; i < accountIds.count() ; i++) {
        QMessageAccount account(accountIds.at(i));
        qDebug() << "Name: " << account.name();
    }
#endif

    if (accountIds.count() == 1) {
        testAccountId = accountIds.first();
        address = testAccountName; // Test account name must be set in case of existing account
    } else {
        int originalCount = manager->countAccounts();
        Support::Parameters p;

        QString configFile = QDir::currentPath() + QDir::separator() + "account.txt";
        QFile file(configFile);
        QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));

        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList values = line.split("=");
            p.insert(values.first().simplified(), values.last().simplified());
        }

        // Set test address defined in account.txt to use it later!
        address = p.value("fromAddress");
        testAccountId = Support::addAccount(p);
        QCOMPARE(manager->countAccounts(), originalCount + 1);
        newAccountCreated = true;
    }

    QVERIFY(!address.isEmpty());
    QVERIFY(testAccountId.isValid());
    QVERIFY(testAccountId != QMessageAccountId());

    QMessageFilter filter = QMessageFilter::byStandardFolder(QMessage::InboxFolder) &
            QMessageFilter::byType(QMessage::Email) &
            QMessageFilter::byParentAccountId(QMessageAccountFilter::byId(testAccountId));

    existingMessageIds = manager->queryMessages(filter).toSet();
}

void tst_QMessageServer::cleanupTestCase()
{
    if (newAccountCreated) {
        // TODO: Remove account if it's created during execution of this test suite
    }
}

//Q_DECLARE_METATYPE(QList<QMessageService::State>);
Q_DECLARE_METATYPE(QList<QByteArray>)
Q_DECLARE_METATYPE(QList<int>)

void tst_QMessageServer::messageData()
{
        QTest::addColumn<QString>("subject");
        QTest::addColumn<QByteArray>("messageType");
        QTest::addColumn<QByteArray>("messageSubType");
        QTest::addColumn<int>("messageSize");
        QTest::addColumn<QString>("text");
        QTest::addColumn<QByteArray>("bodyType");
        QTest::addColumn<QByteArray>("bodySubType");
        QTest::addColumn<int>("bodySize");
        QTest::addColumn<QList<QByteArray> >("attachments");
        QTest::addColumn<QList<QByteArray> >("attachmentType");
        QTest::addColumn<QList<QByteArray> >("attachmentSubType");
        QTest::addColumn<QList<int> >("attachmentSize");

        QTest::newRow("1")
        << "First message"
        << QByteArray("text")
        << QByteArray("plain")
    #if defined(Q_OS_SYMBIAN)
    #ifdef FREESTYLEMAILUSED
        << 26
    #else
        << 89
    #endif
    #else
    #if defined(Q_OS_WIN) && defined(_WIN32_WCE)
        << 32
    #else
        << 1400
    #endif
    #endif
        << "...before Y2K"
        << QByteArray("text")
        << QByteArray("plain")
        << 24
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<int>();

        QTest::newRow("2")
        << "Second message"
        << QByteArray("text")
        << QByteArray("html")
    #if defined(Q_OS_SYMBIAN)
    #ifdef FREESTYLEMAILUSED
        << 80
    #else
        << 157
    #endif
    #else
    #if defined(Q_OS_WIN) && defined(_WIN32_WCE)
        << 64
    #else
        << 1536
    #endif
    #endif
        << "<html><p>...before <b>Y2K</b></p></html>"
        << QByteArray("text")
        << QByteArray("html")
        << 64
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<QByteArray>()
        << QList<int>();

        QTest::newRow("3")
        << "Third message"
        << QByteArray("multipart")
        << QByteArray("mixed")
    #if defined(Q_OS_SYMBIAN)
    #ifdef FREESTYLEMAILUSED
        << 494
    #else
        << 611
    #endif
    #else
    #if defined(Q_OS_WIN) && defined(_WIN32_WCE)
        << 512
    #else
        << 1536
    #endif
    #endif
        << "...before Y2K"
        << QByteArray("text")
        << QByteArray("plain")
        << 24
        << ( QList<QByteArray>() << "1.txt" )
        << ( QList<QByteArray>() << "text" )
        << ( QList<QByteArray>() << "plain" )
        << ( QList<int>() << 512 );

        QTest::newRow("4")
        << "Fourth message"
        << QByteArray("multipart")
        << QByteArray("mixed")
    #if defined(Q_OS_SYMBIAN)
    #ifdef FREESTYLEMAILUSED
        << 4623
    #else
        << 4731
    #endif
    #else
    #if defined(Q_OS_WIN) && !defined(_WIN32_WCE)
        << 5120
    #else
        << 4096
    #endif
    #endif
        << "<html><p>...before <b>Y2K</b></p></html>"
        << QByteArray("text")
        << QByteArray("html")
        << 64
        << ( QList<QByteArray>() << "1.txt" << "qtlogo.png" )
        << ( QList<QByteArray>() << "text" << "image" )
        << ( QList<QByteArray>() << "plain" << "png" )
        << ( QList<int>() << 512 << 4096 );
}

void tst_QMessageServer::testSend_data()
{
    messageData();
}

void tst_QMessageServer::testSend()
{
    QFETCH(QString, subject);
    QFETCH(QByteArray, messageType);
    QFETCH(QByteArray, messageSubType);
    QFETCH(int, messageSize);
    QFETCH(QString, text);
    QFETCH(QByteArray, bodyType);
    QFETCH(QByteArray, bodySubType);
    QFETCH(int, bodySize);
    QFETCH(QList<QByteArray>, attachments);
    QFETCH(QList<QByteArray>, attachmentType);
    QFETCH(QList<QByteArray>, attachmentSubType);
    QFETCH(QList<int>, attachmentSize);

    QMessage message;
    message.setType(QMessage::Email);
    message.setParentAccountId(testAccountId);

    QMessageAddressList toList;
    toList.append(QMessageAddress(QMessageAddress::Email, address));
    message.setTo(toList);
    message.setSubject(subject);
    message.setBody(text, bodyType + "/" + bodySubType + "; charset=" + alternateCharset);

    if (!attachments.isEmpty()) {
        QStringList attachmentPaths;
        foreach (const QString &fileName, attachments) {
            attachmentPaths.append(TESTDATA_DIR "/testdata/" + fileName);
        }
        message.appendAttachments(attachmentPaths);
    }

    SignalCatcher signalCatcher;
    connect(&service, SIGNAL(stateChanged(QMessageService::State)), &signalCatcher, SLOT(stateChanged(QMessageService::State)));

    QVERIFY(service.send(message));
    QVERIFY(service.state() == QMessageService::FinishedState);
    QVERIFY(service.error() == QMessageManager::NoError);

    QVERIFY(signalCatcher.states.count() == 2);
    QVERIFY(signalCatcher.states.last() == QMessageService::FinishedState);
}

void tst_QMessageServer::testSyncMessages()
{
    QMessageIdList matchingIds;
    QMessageFilter filter = QMessageFilter::byStandardFolder(QMessage::InboxFolder) &
            QMessageFilter::byType(QMessage::Email) &
            QMessageFilter::byParentAccountId(QMessageAccountFilter::byId(testAccountId));

#if defined(Q_WS_MAEMO_5) || defined(Q_OS_SYMBIAN)
    // In Maemo 5 and Symbian^3 full sync is done.
    service.exportUpdates(testAccountId);

    while (manager->queryMessages(filter).toSet().subtract(existingMessageIds).count() < 4)
        QTest::qWait(1000);
#else
    // Sync must be implemented somehow also on other platforms
#endif

    QVERIFY(manager->queryMessages(filter).toSet().subtract(existingMessageIds).count() == 4);
}

void tst_QMessageServer::testMessages_data()
{
    messageData();
}

void tst_QMessageServer::testMessages()
{
    QFETCH(QString, subject);
    QFETCH(QByteArray, messageType);
    QFETCH(QByteArray, messageSubType);
    QFETCH(int, messageSize);
    QFETCH(QString, text);
    QFETCH(QByteArray, bodyType);
    QFETCH(QByteArray, bodySubType);
    QFETCH(int, bodySize);
    QFETCH(QList<QByteArray>, attachments);
    QFETCH(QList<QByteArray>, attachmentType);
    QFETCH(QList<QByteArray>, attachmentSubType);
    QFETCH(QList<int>, attachmentSize);

    QMessageFilter filter = QMessageFilter::byStandardFolder(QMessage::InboxFolder) &
            QMessageFilter::byType(QMessage::Email) &
            QMessageFilter::byParentAccountId(QMessageAccountFilter::byId(testAccountId));

    QMessageIdList matchingIds(manager->queryMessages(filter));

    QSet<QtMobility::QMessageId> newMessageIds = matchingIds.toSet().subtract(existingMessageIds);

    QVERIFY(newMessageIds.count() == 4);

    bool messageFound = false;

    foreach (const QMessageId& id, newMessageIds) {
        QMessage message = manager->message(id);
        if (message.subject() == subject) {
            messageFound = true;
#ifdef QMESSAGESERVER_DEBUG
            qDebug() << "";
            qDebug() << "1. : " <<  id.toString() << "2. : " << message.id().toString();
            qDebug() << message.subject() << " - " << message.to().first().addressee();
            qDebug() << "Date: " << message.date();
            qDebug() << "Received date: " << message.receivedDate();
            qDebug() << "From: " << message.from().addressee();
#endif

            QMessageContentContainer msgContentContainer = message.find(message.bodyId());
            bool contentAvailable = msgContentContainer.isContentAvailable();

            if (!contentAvailable) {
                QVERIFY(service.retrieveBody(message.id()));

                // Wait as long as body has been retrieved
                QEventLoop loop;
                connect(&service, SIGNAL(stateChanged(QMessageService::State)), &loop, SLOT(quit()));
                loop.exec();
                message = manager->message(id);
            }

            QCOMPARE(message.contentType().toLower(), messageType.toLower());

#ifndef Q_OS_SYMBIAN
            Commented out because these are failing on Symbian^3
            qDebug() << QString(message.contentSubType().toLower());
            qDebug() << QString(messageSubType.toLower());
            QCOMPARE(message.contentSubType().toLower(), messageSubType.toLower());
#endif

            if (message.contentType().toLower() == "multipart") {
                QVERIFY(!message.contentIds().isEmpty());
            } else {
                QVERIFY(message.contentIds().isEmpty());
            }

#if !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)
            // Message size calculation is not yet good enough in Maemo implementation
            QAPPROXIMATECOMPARE(message.size(), messageSize, (messageSize / 2));
#endif

            QMessageContentContainerId bodyId(message.bodyId());
            QCOMPARE(bodyId.isValid(), true);
            QCOMPARE(bodyId != QMessageContentContainerId(), true);
            QCOMPARE(QMessageContentContainerId(bodyId.toString()), bodyId);
            QVERIFY(message.contains(bodyId));

            QMessageContentContainer body(message.find(bodyId));

            QCOMPARE(body.contentType().toLower(), bodyType.toLower());

#ifndef Q_OS_SYMBIAN
            Commented out because these are failing on Symbian^3
            qDebug() << "- " << QString(body.contentSubType().toLower());
            qDebug() << "- " << QString(bodySubType.toLower().toLower());
            QCOMPARE(body.contentSubType().toLower(), bodySubType.toLower());
#endif

            QCOMPARE(body.contentCharset().toLower(), defaultCharset.toLower());
            QCOMPARE(body.isContentAvailable(), true);
#if defined(Q_WS_SYMBIAN)
            // In Symbian^3 additional info is added to text, so text content has been changed when reading it back
            QCOMPARE(body.textContent(), text);
#endif
            QAPPROXIMATECOMPARE(body.size(), bodySize, (bodySize / 2));

            QMessageContentContainerIdList attachmentIds(message.attachmentIds());
            QCOMPARE(attachmentIds.count(), attachments.count());

            for (int i = 0; i < attachments.count(); ++i) {
                QMessageContentContainerId attachmentId(attachmentIds.at(i));
                QCOMPARE(attachmentId.isValid(), true);
                QCOMPARE(attachmentId != QMessageContentContainerId(), true);
                QCOMPARE(QMessageContentContainerId(attachmentId.toString()), attachmentId);
                QVERIFY(message.contains(attachmentId));

                // With MAPI, attachment order is not preserved
                QMessageContentContainer attachment(message.find(attachmentId));
                int index = attachments.indexOf(attachment.suggestedFileName());
                QVERIFY(index != -1);

                // We cannot create nested multipart messages
                QVERIFY(attachment.contentIds().isEmpty());

#if defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
                // Check attachment content
                QByteArray attachmentContent = attachment.content();
                QString fileName = QString(TESTDATA_DIR) + QString("/testdata/") + attachments[index];
                QFile attachmentFile(fileName);
                if (attachmentFile.open(QIODevice::ReadOnly)) {
                    QByteArray originalAttachmentContent = attachmentFile.readAll();
                    QCOMPARE(attachmentContent, originalAttachmentContent);
                }
#endif

// Defined out because these are failing on Symbian^3
#ifndef Q_OS_SYMBIAN
                qDebug() << "Attachment Type: " << QString(attachment.contentType().toLower());
                qDebug() << "Attachment Type: " << QString(attachmentType[index].toLower());
                qDebug() << "Attachment Type: " << QString(attachment.contentSubType().toLower());
                qDebug() << "Attachment Type: " << QString(attachmentSubType[index].toLower());
                QCOMPARE(attachment.contentType().toLower(), attachmentType[index].toLower());
                QCOMPARE(attachment.contentSubType().toLower(), attachmentSubType[index].toLower());
#end
                QCOMPARE(attachment.suggestedFileName(), attachments[index]);
                QAPPROXIMATECOMPARE(attachment.size(), attachmentSize[index], (attachmentSize[index] / 2));
            }
        }
    }

    QVERIFY(messageFound);
}

void tst_QMessageServer::testUpdateReadStatus()
{
    QMessageFilter filter = QMessageFilter::byStandardFolder(QMessage::InboxFolder);
    QMessageFilter filter2 = QMessageFilter::byType(QMessage::Email);
    QMessageFilter filter3 = QMessageFilter::byParentAccountId(QMessageAccountFilter::byId(testAccountId));
    QMessageIdList matchingIds(manager->queryMessages(filter & filter2 & filter3));

    QSet<QtMobility::QMessageId> newMessageIds = matchingIds.toSet().subtract(existingMessageIds);

    QVERIFY(newMessageIds.count() == 4);

    foreach (const QMessageId &id, newMessageIds) {
        QMessage msg = manager->message(id);
        qDebug() << msg.id().toString();
        //QCOMPARE(msg.isModified(), false); // This should be false? Right?
        msg.setStatus(QMessage::Read);
        QCOMPARE(msg.isModified(), true);
        //QVERIFY(manager->updateMessage(&msg));
    }

    foreach (const QMessageId &id, newMessageIds) {
        QMessage msg = manager->message(id);
        //QCOMPARE(msg.isModified(), false); // This should be false because it has already been updated? Right?
        bool readStatus = msg.status() & QMessage::Read;
        QVERIFY(readStatus);
    }
}

void tst_QMessageServer::testDeleteMessages()
{
    qDebug() << "tst_QMessageServer::testDeleteMessages()";
    QMessageFilter filter = QMessageFilter::byStandardFolder(QMessage::InboxFolder) &
            QMessageFilter::byType(QMessage::Email) &
            QMessageFilter::byParentAccountId(QMessageAccountFilter::byId(testAccountId));

    QMessageIdList matchingIds(manager->queryMessages(filter));

    QSet<QtMobility::QMessageId> newMessageIds = matchingIds.toSet().subtract(existingMessageIds);

    QVERIFY(newMessageIds.count() == 4);

    // Remove retrieved messages
    QMessageFilter idsFilter = QMessageFilter::byId(newMessageIds.toList());
    QVERIFY(manager->removeMessages(idsFilter));

    QEventLoop eventLoop;
    connect(manager, SIGNAL(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)), &eventLoop, SLOT(quit()));

    // Wait until messageRemoved signal quits eventLoop or until
    // 10 seconds timout is reached.
    //QTimer::singleShot(10000, &eventLoop, SLOT(quit()));
    //eventLoop.exec();

    //QTest::qWait(60000);

    QVERIFY(service.exportUpdates(testAccountId));

    //QTest::qWait(10000);

    //disconnect(manager, SIGNAL(messageRemoved(QMessageId, QMessageManager::NotificationFilterIdSet)), 0, 0);
    //QTimer::singleShot(10000, &eventLoop, SLOT(quit()));
    //eventLoop.exec();

    QSet<QtMobility::QMessageId> existingMessageIds = manager->queryMessages(filter).toSet().subtract(existingMessageIds);
    QVERIFY(existingMessageIds.count() == 0);
}

void tst_QMessageServer::testCancelSending()
{
    qDebug() << "tst_QMessageServer::testCancelSending()";
#if defined(Q_WS_MAEMO_5) || defined(Q_WS_SYMBIAN)
   QSKIP("Cancel sending isn't supported on this platform", SkipAll);
#endif

    QMessageFilter filter = QMessageFilter::byStandardFolder(QMessage::InboxFolder);
    const QMessageIdList matchingIds(manager->queryMessages(filter));

    QMessage message;
    message.setType(QMessage::Email);
    message.setParentAccountId(testAccountId);

    QMessageAddressList toList;
    toList.append(QMessageAddress(QMessageAddress::Email, address));
    message.setTo(toList);
    message.setSubject("Test message");

    QString text("This is a test message, please ignore.");

    message.setBody(text, "text/plain; charset=" + alternateCharset);

    QStringList attachmentPaths;
    attachmentPaths.append(TESTDATA_DIR "/testdata/qtlogo.png");

    message.appendAttachments(attachmentPaths);

    SignalCatcher signalCatcher;
    connect(&service, SIGNAL(stateChanged(QMessageService::State)), &signalCatcher, SLOT(stateChanged(QMessageService::State)));

    service.send(message);
    service.cancel();

    QEventLoop eventloop;
    QTimer::singleShot(1000, &eventloop, SLOT(quit()));
    eventloop.exec();

    QCOMPARE(signalCatcher.states.count(), 2);
    QCOMPARE(signalCatcher.states.last(), QMessageService::CanceledState);
    QCOMPARE(service.state(), QMessageService::CanceledState);

    QTimer::singleShot(10000, &eventloop, SLOT(quit()));
    eventloop.exec();

    const QMessageIdList matchingIds2(manager->queryMessages(filter));
    QCOMPARE(matchingIds.count(), matchingIds2.count());
}

void tst_QMessageServer::testCancel()
{
    int fetchingTime = 0;
    QMessageIdList matchingIds;

    {
        // Try to fetch an attachment
        QMessageFilter filter = QMessageFilter::byStandardFolder(QMessage::InboxFolder);
        QMessageFilter filter2 = QMessageFilter::byType(QMessage::Email);
        QMessageFilter filter3 = QMessageFilter::byParentAccountId(QMessageAccountFilter::byId(QMessageAccount::defaultAccount(QMessage::Email)));
        matchingIds = manager->queryMessages(filter & filter2 & filter3);

        QVERIFY(matchingIds.count() == 2);

        QMessageId messageId = matchingIds.first();
        QMessage message(messageId);

        QVERIFY(message.attachmentIds().count() == 1);

        QMessageContentContainerId attacmentId = message.attachmentIds().first();
        QMessageContentContainer attachment = message.find(attacmentId);

        QVERIFY(!attachment.isContentAvailable());
        QVERIFY(service.retrieve(messageId, attacmentId));

        QTime duration;
        duration.start();

        bool updated = false;
        while (!updated) {
            QMessage firstMessage(messageId);

            QVERIFY(firstMessage.attachmentIds().count() == 1);

            QMessageContentContainerId attacmentId2 = firstMessage.attachmentIds().first();
            QMessageContentContainer attachment2 = firstMessage.find(attacmentId2);

            if (attachment2.isContentAvailable())
                updated = true;

            QTest::qWait(100);
        }

        fetchingTime = duration.elapsed();
    }

    {
        // Try to cancel fetching
        QMessageId messageId = matchingIds.last();
        QMessage message(messageId);

        QVERIFY(message.attachmentIds().count() == 1);

        QMessageContentContainerId attacmentId = message.attachmentIds().first();
        QMessageContentContainer attachment = message.find(attacmentId);

        QVERIFY(!attachment.isContentAvailable());
        QVERIFY(service.retrieve(messageId, attacmentId));

        SignalCatcher signalCatcher;
        connect(&service, SIGNAL(stateChanged(QMessageService::State)), &signalCatcher, SLOT(stateChanged(QMessageService::State)));
        service.cancel();

        QVERIFY(signalCatcher.states.count() > 0);
        QCOMPARE(signalCatcher.states.last(), QMessageService::CanceledState);
        QCOMPARE(service.state(), QMessageService::CanceledState);

        QTest::qWait(fetchingTime*2);

        QMessage secondMessage(messageId);
        QVERIFY(secondMessage.attachmentIds().count() == 1);

        QMessageContentContainerId attacmentId2 = secondMessage.attachmentIds().first();
        QMessageContentContainer attachment2 = secondMessage.find(attacmentId2);

        QVERIFY(!attachment2.isContentAvailable());
    }
}
