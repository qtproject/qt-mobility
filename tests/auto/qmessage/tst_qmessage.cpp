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

#if defined(Q_OS_SYMBIAN) || (defined(Q_OS_WIN) && defined(_WIN32_WCE))
# if defined(TESTDATA_DIR)
#  undef TESTDATA_DIR
# endif
# define TESTDATA_DIR "."
#endif

//TESTED_CLASS=
//TESTED_FILES=

#ifdef Q_OS_WIN
const QByteArray defaultCharset("UTF-16");
#else
const QByteArray defaultCharset("UTF-8");
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
    Unit test for QMessage class.
*/
class tst_QMessage : public QObject
{
    Q_OBJECT

public:
    tst_QMessage();
    virtual ~tst_QMessage();

private slots:
    void initTestCase();
    void cleanup();
    void cleanupTestCase();

#if !defined(Q_OS_WIN)
    // Transmission format not handled yet
    void testFromTransmissionFormat_data();
    void testFromTransmissionFormat();
#endif

    void testToTransmissionFormat_simple();
    void testToTransmissionFormat_multipart();

    void testType();
    void testParentAccountId();
    void testFrom();
    void testSubject();
    void testDate();
    void testReceivedDate();
    void testTo();
    void testCc();
    void testBcc();
    void testStatus();
    void testPriority();
    void testPreferredCharsets_data();
    void testPreferredCharsets();

private:
    QMessageAccountId testAccountId;
    QMessageFolderId testFolderId;
};

QTEST_MAIN(tst_QMessage)

#include "tst_qmessage.moc"

tst_QMessage::tst_QMessage()
{
}

tst_QMessage::~tst_QMessage()
{
}

void tst_QMessage::initTestCase()
{
    Support::clearMessageStore();

    {
        Support::Parameters p;
        p.insert("name", "testAccount");

        testAccountId = Support::addAccount(p);
        QVERIFY(testAccountId.isValid());
    }

    {
        Support::Parameters p;
        p.insert("path", "/root");
        p.insert("displayName", "Root");
        p.insert("parentAccountName", "testAccount");

        testFolderId = Support::addFolder(p);
        QVERIFY(testFolderId.isValid());
    }
}

void tst_QMessage::cleanup()
{
}

void tst_QMessage::cleanupTestCase()
{
}

#if !defined(Q_OS_WIN)
Q_DECLARE_METATYPE(QList<QByteArray>)

void tst_QMessage::testFromTransmissionFormat_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("to");
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("date");
    QTest::addColumn<QString>("subject");
    QTest::addColumn<int>("parts");
    QTest::addColumn<QByteArray>("type");
    QTest::addColumn<QByteArray>("subtype");
    QTest::addColumn<QByteArray>("charset");
    QTest::addColumn<QString>("text");
    QTest::addColumn<QList<QByteArray> >("headerFields");
    QTest::addColumn<QStringList>("headerFieldValues");

    // A simple message
    QTest::newRow("1") 
        << "1.txt"
        << "bbb@zzz.test"
        << "bbb@ddd.example (John X. Doe)"
        << "2001-05-04T18:05:44Z" // Convert the time in the message to UTC
        << "This is a test message"
        << 0
        << QByteArray("text")
        << QByteArray("plain")
        << QByteArray("us-ascii")
        << "\
\n\
Hi,\n\
\n\
Do you like this message?\n\
\n\
-Me\n\
"
        << ( QList<QByteArray>() << "Return-Path"
                                 << "Delivered-To"
                                 << "Received" 
                                 << "MIME-Version"
                                 << "Content-Type"
                                 << "Content-Transfer-Encoding"
                                 << "Message-ID"
                                 << "From"
                                 << "To"
                                 << "Subject"
                                 << "Date" )
        << ( QStringList() << "<bbb@zzz.test>" 
                           << "bbb@zzz.test"
                           << "by mail.zzz.test (Postfix, from userid 889)\tid 27CEAD38CC; Fri,  4 May 2001 14:05:44 -0400 (EDT)"
                           << "1.0"
                           << "text/plain; charset=us-ascii"
                           << "7bit"
                           << "<15090.61304.110929.45684@aaa.zzz.test>"
                           << "bbb@ddd.example (John X. Doe)"
                           << "bbb@zzz.test"
                           << "This is a test message"
                           << "Fri, 4 May 2001 14:05:44 -0400" );

    // A multipart message
    QTest::newRow("2") 
        << "2.txt"
        << "barry@python.test"
        << "barry@python.test (Barry A. Krakow)"
        << "2001-09-11T04:05:05Z"
        << "a simple multipart"
        << 2
        << QByteArray("multipart")
        << QByteArray("mixed")
        << QByteArray()
        << QString()
        << ( QList<QByteArray>() << "Return-Path"
                                 << "Delivered-To"
                                 << "Received" 
                                 << "MIME-Version"
                                 << "Content-Type"
                                 << "Content-Transfer-Encoding"
                                 << "Message-ID"
                                 << "From"
                                 << "To"
                                 << "Subject"
                                 << "Date"
                                 << "X-Mailer"
                                 << "X-Attribution"
                                 << "X-Oblique-Strategy" )
        << ( QStringList() << "<barry@python.test>"
                           << "barry@python.test"
                           << "by mail.python.test (Postfix, from userid 889)\tid C2BF0D37C6; Tue, 11 Sep 2001 00:05:05 -0400 (EDT)"
                           << "1.0"
                           << "multipart/mixed; boundary=\"h90VIIIKmx\""
                           << "7bit"
                           << "<15261.36209.358846.118674@anthem.python.test>"
                           << "barry@python.test (Barry A. Krakow)"
                           << "barry@python.test"
                           << "a simple multipart"
                           << "Tue, 11 Sep 2001 00:05:05 -0400"
                           << "VM 6.95 under 21.4 (patch 4) \"Artificial Intelligence\" XEmacs Lucid"
                           << "BAW"
                           << "Make a door into a window" );

    // A nested multipart message
    QTest::newRow("3") 
        << "3.txt"
        << "Dingus Lovers <cravindogs@cravindogs.test>"
        << "Barry <barry@example.com>"
        << "2001-04-20T23:35:02Z"
        << "Here is your dingus fish"
        << 2
        << QByteArray("multipart")
        << QByteArray("mixed")
        << QByteArray()
        << QString()
        << ( QList<QByteArray>() << "MIME-Version"
                                 << "From"
                                 << "To"
                                 << "Subject"
                                 << "Date"
                                 << "Content-Type" )
        << ( QStringList() << "1.0"
                           << "Barry <barry@example.com>"
                           << "Dingus Lovers <cravindogs@cravindogs.test>"
                           << "Here is your dingus fish"
                           << "Fri, 20 Apr 2001 19:35:02 -0400"
                           << "multipart/mixed; boundary=\"OUTER\"" );
}

void tst_QMessage::testFromTransmissionFormat()
{
    QFETCH(QString, fileName);
    QFETCH(QString, to);
    QFETCH(QString, from);
    QFETCH(QString, date);
    QFETCH(QString, subject);
    QFETCH(int, parts);
    QFETCH(QByteArray, type);
    QFETCH(QByteArray, subtype);
    QFETCH(QByteArray, charset);
    QFETCH(QString, text);
    QFETCH(QList<QByteArray>, headerFields);
    QFETCH(QStringList, headerFieldValues);

    QString path(TESTDATA_DIR "/testdata/" + fileName);
    QMessage message(QMessage::fromTransmissionFormatFile(QMessage::Email, path));

    QCOMPARE(message.to().first().recipient(), to);
    QCOMPARE(message.from().recipient(), from);
    QCOMPARE(message.date(), QDateTime::fromString(date, Qt::ISODate).toLocalTime());
    QCOMPARE(message.subject(), subject);

    QCOMPARE(message.contentIds().count(), parts);

    QCOMPARE(message.contentType().toLower(), type);
    QCOMPARE(message.contentSubType().toLower(), subtype);
    QCOMPARE(message.contentCharset().toLower(), charset);

    if (!text.isEmpty()) {
        QMessageContentContainerId bodyId(message.bodyId());
        QVERIFY(bodyId.isValid());

        QMessageContentContainer body(message.find(bodyId));
        QCOMPARE(body.contentType().toLower(), type);
        QCOMPARE(body.contentSubType().toLower(), subtype);
        QCOMPARE(body.contentCharset().toLower(), charset);
        QCOMPARE(body.isContentAvailable(), true);
        QCOMPARE(body.textContent(), text);
    }

    QCOMPARE(message.headerFields(), headerFields);

    QStringList::const_iterator it = headerFieldValues.begin(), end = headerFieldValues.end();
    foreach (const QByteArray &field, headerFields) {
        QVERIFY(it != end);
        QCOMPARE(message.headerFieldValue(field), *it);

        foreach (const QString &value, message.headerFieldValues(field)) {
            QVERIFY(it != end);
            QCOMPARE(value, *it);
            ++it;
        }
    }
}
#endif

void tst_QMessage::testToTransmissionFormat_simple()
{
    const QString from("Alice <alice@example.com>");
    const QString to("Bob <bob@example.com>");
    const QString cc("Charlie <charlie@example.com>");
    const QString bcc("Dave <dave@example.com>");
    const QDateTime date(QDateTime::fromString(QDateTime::currentDateTime().toString(Qt::ISODate), Qt::ISODate)); // Strip unrepresentable elements
    const QString subject("This is a simple message");
    const QByteArray contentType("text");
    const QByteArray contentSubType("plain");
    const QByteArray contentCharset(defaultCharset);
    const QString contentText(QString("This is the happy face:").append(QChar(0x263a)));

    QMessage m1;

    m1.setType(QMessage::Email);
    m1.setFrom(QMessageAddress(from, QMessageAddress::Email));
    m1.setTo(QMessageAddressList() << QMessageAddress(to, QMessageAddress::Email));
    m1.setCc(QMessageAddressList() << QMessageAddress(cc, QMessageAddress::Email));
    m1.setBcc(QMessageAddressList() << QMessageAddress(bcc, QMessageAddress::Email));
    m1.setSubject(subject);
    m1.setDate(date);

    {
        // Set body text using stream (for coverage testing only)
        QTextStream is(const_cast<QString*>(&contentText), QIODevice::ReadOnly);

        QByteArray mimeType(contentType + "/" + contentSubType + "; charset=" + contentCharset);
        m1.setBody(is, mimeType);
    }

#if !defined(Q_OS_WIN)
    QByteArray serialized;
    {
        // Serialize using stream (for coverage testing only)
        QDataStream os(&serialized, QIODevice::WriteOnly);
        m1.toTransmissionFormat(os);
    }
#endif

    QCOMPARE(m1.from().recipient(), from);
    QCOMPARE(m1.to().first().recipient(), to);
    QCOMPARE(m1.cc().first().recipient(), cc);
    QCOMPARE(m1.bcc().first().recipient(), bcc);
    QCOMPARE(m1.subject(), subject);
    QCOMPARE(m1.date(), date);
    QAPPROXIMATECOMPARE(m1.size(), 2048u, 1024u);

    QCOMPARE(m1.contentType().toLower(), contentType.toLower());
    QCOMPARE(m1.contentSubType().toLower(), contentSubType.toLower());
    QCOMPARE(m1.contentCharset().toLower(), contentCharset.toLower());
    QCOMPARE(m1.textContent(), contentText);

#if !defined(Q_OS_WIN)
    QMessage m2(QMessage::fromTransmissionFormat(QMessage::Email, serialized));

    QCOMPARE(m2.from().recipient(), from);
    QCOMPARE(m2.to().first().recipient(), to);
    QCOMPARE(m2.cc().first().recipient(), cc);
    QCOMPARE(m2.bcc().count(), 0);
    QCOMPARE(m2.date(), date);
    QAPPROXIMATECOMPARE(m2.size(), 2048u, 1024u);

    QCOMPARE(m2.contentType().toLower(), contentType.toLower());
    QCOMPARE(m2.contentSubType().toLower(), contentSubType.toLower());
    QCOMPARE(m2.contentCharset().toLower(), contentCharset.toLower());
    QCOMPARE(m2.textContent(), contentText);
#endif
}

void tst_QMessage::testToTransmissionFormat_multipart()
{
    const QString from("Alice <alice@example.com>");
    const QString to("Bob <bob@example.com>");
    const QString subject("This is a multipart message");
    const QByteArray contentType("multipart");
    const QByteArray contentSubType("mixed");
    const unsigned messageSize(6244u);
    const unsigned messageVariance(3072u);

    const QByteArray p1ContentType("text");
    const QByteArray p1ContentSubType("plain");
    const QByteArray p1ContentCharset(defaultCharset);
    const QString p1ContentText(QString("This is the happy face:").append(QChar(0x263a)));
    const unsigned p1Size(48u);
    const unsigned p1Variance(24u);

    const QByteArray p2FileName("1.txt");
    const QByteArray p2ContentType("text");
    const QByteArray p2ContentSubType("plain");
    const QByteArray p2ContentCharset("");
    const QString p2ContentText("\
Return-Path: <bbb@zzz.test>\r\n\
Delivered-To: bbb@zzz.test\r\n\
Received: by mail.zzz.test (Postfix, from userid 889)\r\n\
\tid 27CEAD38CC; Fri,  4 May 2001 14:05:44 -0400 (EDT)\r\n\
MIME-Version: 1.0\r\n\
Content-Type: text/plain; charset=us-ascii\r\n\
Content-Transfer-Encoding: 7bit\r\n\
Message-ID: <15090.61304.110929.45684@aaa.zzz.test>\r\n\
From: bbb@ddd.example (John X. Doe)\r\n\
To: bbb@zzz.test\r\n\
Subject: This is a test message\r\n\
Date: Fri, 4 May 2001 14:05:44 -0400\r\n\
\r\n\
\r\n\
Hi,\r\n\
\r\n\
Do you like this message?\r\n\
\r\n\
-Me\r\n\
");
    const unsigned p2Size(512u);
    const unsigned p2Variance(256u);

    const QByteArray p3FileName("qtlogo.png");
    const QByteArray p3ContentType("image");
    const QByteArray p3ContentSubType("png");
    const QByteArray p3ContentCharset("");
    const unsigned p3Size(4096u);
    const unsigned p3Variance(2048u);

    const QByteArray p4FileName("pointless.sh");
    const QByteArray p4ContentType("application");
#ifdef Q_OS_WIN
    const QByteArray p4ContentSubType("octet-stream");
#else
    const QByteArray p4ContentSubType("x-sh");
#endif
    const QByteArray p4ContentCharset("");
    const QByteArray p4ContentData("\
#!/bin/sh\n\
# This script does nothing\n\
exit 0\n");
    const unsigned p4Size(64u);
    const unsigned p4Variance(32u);

    const QByteArray p5FileName("datafile");
    const QByteArray p5ContentType("application");
    const QByteArray p5ContentSubType("octet-stream");
    const QByteArray p5ContentCharset("");
    const QByteArray p5ContentData("abcdefghijklmnopqrstuvwxyz");
    const unsigned p5Size(48u);
    const unsigned p5Variance(24u);

    QMessage m1;

    m1.setType(QMessage::Email);
    m1.setFrom(QMessageAddress(from, QMessageAddress::Email));
    m1.setTo(QMessageAddressList() << QMessageAddress(to, QMessageAddress::Email));
    m1.setSubject(subject);

    QByteArray mimeType(p1ContentType + "/" + p1ContentSubType);
    m1.setBody(p1ContentText, mimeType);

    QStringList attachments;
    foreach (const QString &file, ( QStringList() << p2FileName << p3FileName << p4FileName << p5FileName)) {
        attachments.append(TESTDATA_DIR "/testdata/" + file);
    }
    m1.appendAttachments(attachments);

    QByteArray serialized(m1.toTransmissionFormat());

    {
        QCOMPARE(m1.from().recipient(), from);
        QCOMPARE(m1.to().first().recipient(), to);
        QCOMPARE(m1.subject(), subject);
        QAPPROXIMATECOMPARE(m1.size(), messageSize, messageVariance);

        QCOMPARE(m1.contentType().toLower(), contentType.toLower());
        QCOMPARE(m1.contentSubType().toLower(), contentSubType.toLower());
        QCOMPARE(m1.isContentAvailable(), true);
        QCOMPARE(m1.contentIds().count(), 5);
        QCOMPARE(m1.attachmentIds().count(), 4);

        QMessageContentContainerId bodyId(m1.bodyId());
        QMessageContentContainerIdList ids(m1.contentIds());

        QCOMPARE(bodyId, ids.first());
        QCOMPARE(bodyId.isValid(), true);
        QCOMPARE(bodyId != QMessageContentContainerId(), true);
        QCOMPARE(QMessageContentContainerId(bodyId.toString()), bodyId);

        QMessageContentContainerId attachmentId(ids.at(0));
        QCOMPARE(attachmentId.isValid(), true);
        QCOMPARE(attachmentId != QMessageContentContainerId(), true);
        QCOMPARE(QMessageContentContainerId(attachmentId.toString()), bodyId);

        QMessageContentContainer p1(m1.find(attachmentId));

        QCOMPARE(p1.contentType().toLower(), p1ContentType.toLower());
        QCOMPARE(p1.contentSubType().toLower(), p1ContentSubType.toLower());
        QCOMPARE(p1.contentCharset().toLower(), p1ContentCharset.toLower());
        QCOMPARE(p1.isContentAvailable(), true);
        QCOMPARE(p1.contentIds().count(), 0);
        QCOMPARE(p1.textContent(), p1ContentText);
        QAPPROXIMATECOMPARE(p1.size(), p1Size, p1Variance);

        attachmentId = ids.at(1);
        QCOMPARE(attachmentId.isValid(), true);
        QCOMPARE(attachmentId != QMessageContentContainerId(), true);
        QCOMPARE(QMessageContentContainerId(attachmentId.toString()), attachmentId);

        QMessageContentContainer p2(m1.find(attachmentId));

        QCOMPARE(p2.contentType().toLower(), p2ContentType.toLower());
        QCOMPARE(p2.contentSubType().toLower(), p2ContentSubType.toLower());
        QCOMPARE(p2.contentCharset().toLower(), p2ContentCharset.toLower());
        QCOMPARE(p2.isContentAvailable(), true);
        QCOMPARE(p2.contentIds().count(), 0);
        QCOMPARE(p2.textContent(), p2ContentText);
        QAPPROXIMATECOMPARE(p2.size(), p2Size, p2Variance);

        attachmentId = ids.at(2);
        QCOMPARE(attachmentId.isValid(), true);
        QCOMPARE(attachmentId != QMessageContentContainerId(), true);
        QCOMPARE(QMessageContentContainerId(attachmentId.toString()), attachmentId);

        QMessageContentContainer p3(m1.find(attachmentId));

        QCOMPARE(p3.contentType().toLower(), p3ContentType.toLower());
        QCOMPARE(p3.contentSubType().toLower(), p3ContentSubType.toLower());
        QCOMPARE(p3.contentCharset().toLower(), p3ContentCharset.toLower());
        QCOMPARE(p3.isContentAvailable(), true);
        QCOMPARE(p3.contentIds().count(), 0);
        QCOMPARE(p3.content().length(), 4075);
        QAPPROXIMATECOMPARE(p3.size(), p3Size, p3Variance);

        attachmentId = ids.at(3);
        QCOMPARE(attachmentId.isValid(), true);
        QCOMPARE(attachmentId != QMessageContentContainerId(), true);
        QCOMPARE(QMessageContentContainerId(attachmentId.toString()), attachmentId);

        QMessageContentContainer p4(m1.find(attachmentId));

        QCOMPARE(p4.contentType().toLower(), p4ContentType.toLower());
        QCOMPARE(p4.contentSubType().toLower(), p4ContentSubType.toLower());
        QCOMPARE(p4.contentCharset().toLower(), p4ContentCharset.toLower());
        QCOMPARE(p4.isContentAvailable(), true);
        QCOMPARE(p4.contentIds().count(), 0);
        QCOMPARE(p4.content(), p4ContentData);
        QAPPROXIMATECOMPARE(p4.size(), p4Size, p4Variance);

        attachmentId = ids.at(4);
        QCOMPARE(attachmentId.isValid(), true);
        QCOMPARE(attachmentId != QMessageContentContainerId(), true);
        QCOMPARE(QMessageContentContainerId(attachmentId.toString()), attachmentId);

        QMessageContentContainer p5(m1.find(attachmentId));

        QCOMPARE(p5.contentType().toLower(), p5ContentType.toLower());
        QCOMPARE(p5.contentSubType().toLower(), p5ContentSubType.toLower());
        QCOMPARE(p5.contentCharset().toLower(), p5ContentCharset.toLower());
        QCOMPARE(p5.isContentAvailable(), true);
        QCOMPARE(p5.contentIds().count(), 0);
        QCOMPARE(p5.content(), p5ContentData);
        QAPPROXIMATECOMPARE(p5.size(), p5Size, p5Variance);
    }

    // Verify that attachments can be removed
    m1.clearAttachments();
    QCOMPARE(m1.attachmentIds(), QMessageContentContainerIdList());

#if !defined(Q_OS_WIN)
    QMessage m2(QMessage::fromTransmissionFormat(QMessage::Email, serialized));

    {
        QCOMPARE(m2.from().recipient(), from);
        QCOMPARE(m2.to().first().recipient(), to);
        QCOMPARE(m2.subject(), subject);
        QAPPROXIMATECOMPARE(m2.size(), messageSize, messageVariance);

        QCOMPARE(m2.contentType().toLower(), contentType.toLower());
        QCOMPARE(m2.contentSubType().toLower(), contentSubType.toLower());
        QCOMPARE(m2.isContentAvailable(), true);
        QCOMPARE(m2.contentIds().count(), 5);
        QCOMPARE(m2.attachmentIds().count(), 4);

        QMessageContentContainerId bodyId(m2.bodyId());
        QMessageContentContainerIdList ids(m2.contentIds());

        QCOMPARE(bodyId, ids.first());

        QMessageContentContainer p1(m2.find(ids.at(0)));

        QCOMPARE(p1.contentType().toLower(), p1ContentType.toLower());
        QCOMPARE(p1.contentSubType().toLower(), p1ContentSubType.toLower());
        QCOMPARE(p1.contentCharset().toLower(), p1ContentCharset.toLower());
        QCOMPARE(p1.isContentAvailable(), true);
        QCOMPARE(p1.contentIds().count(), 0);
        QCOMPARE(p1.textContent(), p1ContentText);
        QAPPROXIMATECOMPARE(p1.size(), p1Size, p1Variance);

        QMessageContentContainer p2(m2.find(ids.at(1)));

        QCOMPARE(p2.contentType().toLower(), p2ContentType.toLower());
        QCOMPARE(p2.contentSubType().toLower(), p2ContentSubType.toLower());
        QCOMPARE(p2.contentCharset().toLower(), p2ContentCharset.toLower());
        QCOMPARE(p2.isContentAvailable(), true);
        QCOMPARE(p2.contentIds().count(), 0);
        // This text has had the '\r' stripped out...
        QCOMPARE(p2.textContent(), QString(p2ContentText).replace("\r\n", "\n"));
        QAPPROXIMATECOMPARE(p2.size(), p2Size, p2Variance);

        QMessageContentContainer p3(m2.find(ids.at(2)));

        QCOMPARE(p3.contentType().toLower(), p3ContentType.toLower());
        QCOMPARE(p3.contentSubType().toLower(), p3ContentSubType.toLower());
        QCOMPARE(p3.contentCharset().toLower(), p3ContentCharset.toLower());
        QCOMPARE(p3.isContentAvailable(), true);
        QCOMPARE(p3.contentIds().count(), 0);
        QCOMPARE(p3.content().length(), 4075);
        QAPPROXIMATECOMPARE(p3.size(), p3Size, p3Variance);

        QMessageContentContainer p4(m2.find(ids.at(3)));

        QCOMPARE(p4.contentType().toLower(), p4ContentType.toLower());
        QCOMPARE(p4.contentSubType().toLower(), p4ContentSubType.toLower());
        QCOMPARE(p4.contentCharset().toLower(), p4ContentCharset.toLower());
        QCOMPARE(p4.isContentAvailable(), true);
        QCOMPARE(p4.contentIds().count(), 0);
        QCOMPARE(p4.content(), p4ContentData);
        QAPPROXIMATECOMPARE(p4.size(), p4Size, p4Variance);

        QMessageContentContainer p5(m2.find(ids.at(4)));

        QCOMPARE(p5.contentType().toLower(), p5ContentType.toLower());
        QCOMPARE(p5.contentSubType().toLower(), p5ContentSubType.toLower());
        QCOMPARE(p5.contentCharset().toLower(), p5ContentCharset.toLower());
        QCOMPARE(p5.isContentAvailable(), true);
        QCOMPARE(p5.contentIds().count(), 0);
        QCOMPARE(p5.content(), p5ContentData);
        QAPPROXIMATECOMPARE(p5.size(), p5Size, p5Variance);
    }
#endif
}

void tst_QMessage::testType()
{
    QMessage msg;
    QCOMPARE(msg.type(), QMessage::NoType);
    QCOMPARE(msg.isModified(), false);

    msg.setType(QMessage::Email);
    QCOMPARE(msg.type(), QMessage::Email);
    QCOMPARE(msg.isModified(), true);

    msg.setType(QMessage::Mms);
    QCOMPARE(msg.type(), QMessage::Mms);
}

void tst_QMessage::testParentAccountId()
{
    QMessage msg;
    QCOMPARE(msg.parentAccountId(), QMessageAccountId());
    QCOMPARE(msg.isModified(), false);

    msg.setParentAccountId(testAccountId);
    QCOMPARE(msg.parentAccountId(), testAccountId);
    QCOMPARE(msg.isModified(), true);
}

void tst_QMessage::testFrom()
{
    QMessage msg;
    QCOMPARE(msg.from(), QMessageAddress());
    QCOMPARE(msg.isModified(), false);

    QMessageAddress addr("alice@example.org", QMessageAddress::Email);
    msg.setFrom(addr);
    QCOMPARE(msg.from(), addr);
    QCOMPARE(msg.from() != QMessageAddress(), true);
    QCOMPARE(msg.isModified(), true);

    addr = QMessageAddress("bob@example.org", QMessageAddress::Xmpp);
    msg.setFrom(addr);
    QCOMPARE(msg.from(), addr);
    QCOMPARE(msg.from() != QMessageAddress(), true);
}

void tst_QMessage::testSubject()
{
    QMessage msg;
    QCOMPARE(msg.subject(), QString());
    QCOMPARE(msg.isModified(), false);

    QString subject("Short message");
    msg.setSubject(subject);
    QCOMPARE(msg.subject(), subject);
    QCOMPARE(msg.isModified(), true);

    subject = QString("A slightly longer message than the predecessor message");
    msg.setSubject(subject);
    QCOMPARE(msg.subject(), subject);
}

void tst_QMessage::testDate()
{
    QMessage msg;
    QCOMPARE(msg.date(), QDateTime());
    QCOMPARE(msg.isModified(), false);

    QDateTime now(QDateTime::fromString(QDateTime::currentDateTime().toString(Qt::ISODate), Qt::ISODate));
    msg.setDate(now);
    QCOMPARE(msg.date(), now);
    QCOMPARE(msg.isModified(), true);

    now = QDateTime::fromString("2000-01-01T00:00:01Z", Qt::ISODate);
    msg.setDate(now);
    QCOMPARE(msg.date(), now);
}

void tst_QMessage::testReceivedDate()
{
    QMessage msg;
    QCOMPARE(msg.receivedDate(), QDateTime());
    QCOMPARE(msg.isModified(), false);

    QDateTime now(QDateTime::fromString(QDateTime::currentDateTime().toString(Qt::ISODate), Qt::ISODate));
    msg.setReceivedDate(now);
    QCOMPARE(msg.receivedDate(), now);
    QCOMPARE(msg.isModified(), true);

    now = QDateTime::fromString("2000-01-01T00:00:01Z", Qt::ISODate);
    msg.setReceivedDate(now);
    QCOMPARE(msg.receivedDate(), now);
}

void tst_QMessage::testTo()
{
    QMessage msg;
    QCOMPARE(msg.to(), QMessageAddressList());
    QCOMPARE(msg.isModified(), false);

    QMessageAddressList addresses;
    addresses.append(QMessageAddress("alice@example.org", QMessageAddress::Email));
    addresses.append(QMessageAddress("bob@example.org", QMessageAddress::Email));

    msg.setTo(addresses);
    QCOMPARE(msg.to(), addresses);
    QCOMPARE(msg.isModified(), true);

    addresses = QMessageAddressList();
    addresses.append(QMessageAddress("charlie@example.org", QMessageAddress::System));
    msg.setTo(addresses);
    QCOMPARE(msg.to(), addresses);
}

void tst_QMessage::testCc()
{
    QMessage msg;
    QCOMPARE(msg.isModified(), false);
    QCOMPARE(msg.cc(), QMessageAddressList());
    QCOMPARE(msg.isModified(), false);

    QMessageAddressList addresses;
    addresses.append(QMessageAddress("alice@example.org", QMessageAddress::Email));
    addresses.append(QMessageAddress("bob@example.org", QMessageAddress::Email));

    msg.setCc(addresses);
    QCOMPARE(msg.cc(), addresses);
    QCOMPARE(msg.isModified(), true);

    addresses = QMessageAddressList();
    addresses.append(QMessageAddress("charlie@example.org", QMessageAddress::Phone));
    msg.setCc(addresses);
    QCOMPARE(msg.cc(), addresses);
}

void tst_QMessage::testBcc()
{
    QMessage msg;
    QCOMPARE(msg.bcc(), QMessageAddressList());
    QCOMPARE(msg.isModified(), false);

    QMessageAddressList addresses;
    addresses.append(QMessageAddress("alice@example.org", QMessageAddress::Email));
    addresses.append(QMessageAddress("bob@example.org", QMessageAddress::Email));

    msg.setBcc(addresses);
    QCOMPARE(msg.bcc(), addresses);
    QCOMPARE(msg.isModified(), true);

    addresses = QMessageAddressList();
    addresses.append(QMessageAddress("charlie@example.org", QMessageAddress::Xmpp));
    msg.setBcc(addresses);
    QCOMPARE(msg.bcc(), addresses);
}

void tst_QMessage::testStatus()
{
    QMessage msg;
    QCOMPARE(msg.status(), static_cast<QMessage::StatusFlags>(0));
    QCOMPARE(msg.isModified(), false);

    msg.setStatus(QMessage::Removed);
    QCOMPARE(msg.status(), QMessage::Removed);
    QCOMPARE(msg.isModified(), true);

    msg.setStatus(QMessage::Read | QMessage::HasAttachments);
    QCOMPARE(msg.status(), QMessage::Read | QMessage::HasAttachments);
}

void tst_QMessage::testPriority()
{
    QMessage msg;
    QCOMPARE(msg.priority(), QMessage::NormalPriority);
    QCOMPARE(msg.isModified(), false);

    msg.setPriority(QMessage::HighPriority);
    QCOMPARE(msg.priority(), QMessage::HighPriority);
    QCOMPARE(msg.isModified(), true);

    msg.setPriority(QMessage::LowPriority);
    QCOMPARE(msg.priority(), QMessage::LowPriority);
}

void tst_QMessage::testPreferredCharsets_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<QByteArray>("preferred");
    QTest::addColumn<QByteArray>("encoded");

    const QString asciiText("ASCII text");

    QTest::newRow("ascii")
        << asciiText
        << QByteArray("ISO-8859-1")
        << QByteArray("ISO-8859-1");

    const QChar koreanChars[] = { 0xd55c };
    const QString koreanText(koreanChars, 1);

#ifndef _WIN32_WCE
    // Qt for Win CE does not appear to include EUC-KR support...
    QTest::newRow("euc-kr")
        << koreanText
        << QByteArray("EUC-KR")
        << QByteArray("EUC-KR");
#endif

    const QChar arabicChars[] = { 0x0636 };
    const QString mixedText(koreanText + QString(arabicChars, 1));

    QTest::newRow("mixed")
        << mixedText
        << QByteArray("")
        << defaultCharset;
}

void tst_QMessage::testPreferredCharsets()
{
    QFETCH(QString, text);
    QFETCH(QByteArray, preferred);
    QFETCH(QByteArray, encoded);

    QCOMPARE(QMessage::preferredCharsets(), QList<QByteArray>());

    QList<QByteArray> preferredCharsets;
    preferredCharsets << "ISO-8859-1" << "EUC-KR";

    QMessage::setPreferredCharsets(preferredCharsets);
    QCOMPARE(QMessage::preferredCharsets(), preferredCharsets);

    QCOMPARE(QMessage::preferredCharsetFor(text).toLower(), preferred.toLower());

    QMessage msg;
    msg.setBody(text, "text/plain");

    QMessageContentContainer body(msg.find(msg.bodyId()));
    QCOMPARE(body.contentCharset().toLower(), encoded.toLower());

    QMessage::setPreferredCharsets(QList<QByteArray>());
}

