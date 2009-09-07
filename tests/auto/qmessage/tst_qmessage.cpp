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
#if !defined(Q_OS_WIN)
    // No support for nested multipart on windows
    void testToTransmissionFormat_multipart();
#endif

    void testType();
    void testParentAccountId();
#ifdef QMESSAGING_OPTIONAL_FOLDER
    void testParentFolderId();
    void testStandardFolder();
#endif
    void testFrom();
    void testSubject();
    void testDate();
    void testReceivedDate();
    void testTo();
    void testCc();
    void testBcc();
    void testStatus();
    void testPriority();

#ifdef QMESSAGING_OPTIONAL
    void testOriginatorPort();
    void testDestinationPort();
    void testCustomField();
#endif

private:
    QMessageAccountId testAccountId;
#ifdef QMESSAGING_OPTIONAL_FOLDER
    QMessageFolderId testFolderId;
#endif
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

#ifdef QMESSAGING_OPTIONAL_FOLDER
        testFolderId = Support::addFolder(p);
        QVERIFY(testFolderId.isValid());
#endif
    }
}

void tst_QMessage::cleanup()
{
}

void tst_QMessage::cleanupTestCase()
{
}

#if !defined(Q_OS_WIN)
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
";

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
        << QString();

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
        << QString();
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

    QString path(SRCDIR "/testdata/" + fileName);
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
        QMessageContentContainerId bodyId(message.body());
        QVERIFY(bodyId.isValid());

        QMessageContentContainer body(message.container(bodyId));
        QCOMPARE(body.contentType().toLower(), type);
        QCOMPARE(body.contentSubType().toLower(), subtype);
        QCOMPARE(body.contentCharset().toLower(), charset);
        QCOMPARE(body.isContentAvailable(), true);
        QCOMPARE(body.textContent(), text);
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
    const QByteArray contentCharset("UTF-8");
    const QString contentText(QString("This is the happy face:").append(QChar(0x263a)));

    QMessage m1;

    m1.setType(QMessage::Email);
    m1.setFrom(QMessageAddress(from, QMessageAddress::Email));
    m1.setTo(QMessageAddressList() << QMessageAddress(to, QMessageAddress::Email));
    m1.setCc(QMessageAddressList() << QMessageAddress(cc, QMessageAddress::Email));
    m1.setBcc(QMessageAddressList() << QMessageAddress(bcc, QMessageAddress::Email));
    m1.setSubject(subject);
    m1.setDate(date);

#ifdef QMESSAGING_OPTIONAL
    m1.setContentType(contentType);
    m1.setContentSubType(contentSubType);
    m1.setContentCharset(contentCharset);
    m1.setContent(contentText);
#endif

#if !defined(Q_OS_WIN)
    QByteArray serialized(m1.toTransmissionFormat());
#endif

    QCOMPARE(m1.from().recipient(), from);
    QCOMPARE(m1.to().first().recipient(), to);
    QCOMPARE(m1.cc().first().recipient(), cc);
    QCOMPARE(m1.bcc().first().recipient(), bcc);
    QCOMPARE(m1.subject(), subject);
    QCOMPARE(m1.date(), date);

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

    QCOMPARE(m2.contentType().toLower(), contentType.toLower());
    QCOMPARE(m2.contentSubType().toLower(), contentSubType.toLower());
    QCOMPARE(m2.contentCharset().toLower(), contentCharset.toLower());
    QCOMPARE(m2.textContent(), contentText);
#endif
}

#if !defined(Q_OS_WIN)
void tst_QMessage::testToTransmissionFormat_multipart()
{
    const QString from("Alice <alice@example.com>");
    const QString to("Bob <bob@example.com>");
    const QString subject("This is a multipart message");
    const QByteArray contentType("multipart");
    const QByteArray contentSubType("mixed");

    const QByteArray p1ContentType("text");
    const QByteArray p1ContentSubType("plain");
    const QByteArray p1ContentCharset("UTF-8");
    const QString p1ContentText(QString("This is the happy face:").append(QChar(0x263a)));

    const QByteArray p2ContentType("multipart");
    const QByteArray p2ContentSubType("related");

    const QByteArray p3ContentType("text");
    const QByteArray p3ContentSubType("plain");
    const QByteArray p3ContentCharset("us-ascii");
    const QByteArray p3ContentData("This is a\ntext message\n");

    const QByteArray p4ContentType("application");
    const QByteArray p4ContentSubType("octet-stream");
    const QByteArray p4ContentData(10, static_cast<char>(0xffu));

    QMessage m1;

    m1.setType(QMessage::Email);
    m1.setFrom(QMessageAddress(from, QMessageAddress::Email));
    m1.setTo(QMessageAddressList() << QMessageAddress(to, QMessageAddress::Email));
    m1.setSubject(subject);

#ifdef QMESSAGING_OPTIONAL
    m1.setContentType(contentType);
    m1.setContentSubType(contentSubType);

    {
        QMessageContentContainer p1;

        p1.setContentType(p1ContentType);
        p1.setContentSubType(p1ContentSubType);
        p1.setContentCharset(p1ContentCharset);
        p1.setContent(p1ContentText);

        m1.appendContent(p1);

        QMessageContentContainer p2;

        p2.setContentType(p2ContentType);
        p2.setContentSubType(p2ContentSubType);

        QMessageContentContainer p3;

        p3.setContentType(p3ContentType);
        p3.setContentSubType(p3ContentSubType);
        p3.setContentCharset(p3ContentCharset);
        p3.setContent(p3ContentData);

        p2.appendContent(p3);

        QMessageContentContainer p4;

        p4.setContentType(p4ContentType);
        p4.setContentSubType(p4ContentSubType);
        p4.setContent(p4ContentData);

        p2.appendContent(p4);

        m1.appendContent(p2);
    }
#endif

    QByteArray serialized(m1.toTransmissionFormat());

    QCOMPARE(m1.from().recipient(), from);
    QCOMPARE(m1.to().first().recipient(), to);
    QCOMPARE(m1.subject(), subject);

    QCOMPARE(m1.contentType().toLower(), contentType.toLower());
    QCOMPARE(m1.contentSubType().toLower(), contentSubType.toLower());
    QCOMPARE(m1.isContentAvailable(), true);
    QCOMPARE(m1.contentIds().count(), 2);

    QMessageContentContainerIdList ids(m1.contentIds());

    QMessageContentContainer p1(m1.container(ids.at(0)));

    QCOMPARE(p1.contentType().toLower(), p1ContentType.toLower());
    QCOMPARE(p1.contentSubType().toLower(), p1ContentSubType.toLower());
    QCOMPARE(p1.contentCharset().toLower(), p1ContentCharset.toLower());
    QCOMPARE(p1.isContentAvailable(), true);
    QCOMPARE(p1.contentIds().count(), 0);
    QCOMPARE(p1.textContent(), p1ContentText);

    QMessageContentContainer p2(m1.container(ids.at(1)));

    QCOMPARE(p2.contentType().toLower(), p2ContentType.toLower());
    QCOMPARE(p2.contentSubType().toLower(), p2ContentSubType.toLower());
    QCOMPARE(p2.isContentAvailable(), true);
    QCOMPARE(p2.contentIds().count(), 2);

    ids = p2.contentIds();

    QMessageContentContainer p3(p2.container(ids.at(0)));

    QCOMPARE(p3.contentType().toLower(), p3ContentType.toLower());
    QCOMPARE(p3.contentSubType().toLower(), p3ContentSubType.toLower());
    QCOMPARE(p3.isContentAvailable(), true);
    QCOMPARE(p3.contentIds().count(), 0);
    QCOMPARE(p3.content(), p3ContentData);

    QMessageContentContainer p4(p2.container(ids.at(1)));

    QCOMPARE(p4.contentType().toLower(), p4ContentType.toLower());
    QCOMPARE(p4.contentSubType().toLower(), p4ContentSubType.toLower());
    QCOMPARE(p4.isContentAvailable(), true);
    QCOMPARE(p4.contentIds().count(), 0);
    QCOMPARE(p4.content(), p4ContentData);

    QMessage m2(QMessage::fromTransmissionFormat(QMessage::Email, serialized));

    QCOMPARE(m2.from().recipient(), from);
    QCOMPARE(m2.to().first().recipient(), to);
    QCOMPARE(m2.subject(), subject);

    QCOMPARE(m2.contentType().toLower(), contentType.toLower());
    QCOMPARE(m2.contentSubType().toLower(), contentSubType.toLower());
    QCOMPARE(m2.isContentAvailable(), true);
    QCOMPARE(m2.contentIds().count(), 2);

    ids = m2.contentIds();

    QMessageContentContainer p5(m2.container(ids.at(0)));

    QCOMPARE(p5.contentType().toLower(), p1ContentType.toLower());
    QCOMPARE(p5.contentSubType().toLower(), p1ContentSubType.toLower());
    QCOMPARE(p5.contentCharset().toLower(), p1ContentCharset.toLower());
    QCOMPARE(p5.isContentAvailable(), true);
    QCOMPARE(p5.contentIds().count(), 0);
    QCOMPARE(p5.textContent(), p1ContentText);

    QMessageContentContainer p6(m2.container(ids.at(1)));

    QCOMPARE(p6.contentType().toLower(), p2ContentType.toLower());
    QCOMPARE(p6.contentSubType().toLower(), p2ContentSubType.toLower());
    QCOMPARE(p6.isContentAvailable(), true);
    QCOMPARE(p6.contentIds().count(), 2);

    ids = p6.contentIds();

    QMessageContentContainer p7(p6.container(ids.at(0)));

    QCOMPARE(p7.contentType().toLower(), p3ContentType.toLower());
    QCOMPARE(p7.contentSubType().toLower(), p3ContentSubType.toLower());
    QCOMPARE(p7.isContentAvailable(), true);
    QCOMPARE(p7.contentIds().count(), 0);
    QCOMPARE(p7.content(), p3ContentData);

    QMessageContentContainer p8(p6.container(ids.at(1)));

    QCOMPARE(p8.contentType().toLower(), p4ContentType.toLower());
    QCOMPARE(p8.contentSubType().toLower(), p4ContentSubType.toLower());
    QCOMPARE(p8.isContentAvailable(), true);
    QCOMPARE(p8.contentIds().count(), 0);
    QCOMPARE(p8.content(), p4ContentData);
}
#endif

void tst_QMessage::testType()
{
    QMessage msg;
    QCOMPARE(msg.type(), QMessage::NoType);

    msg.setType(QMessage::Email);
    QCOMPARE(msg.type(), QMessage::Email);

    msg.setType(QMessage::Mms);
    QCOMPARE(msg.type(), QMessage::Mms);
}

void tst_QMessage::testParentAccountId()
{
    QMessage msg;
    QCOMPARE(msg.parentAccountId(), QMessageAccountId());

    msg.setParentAccountId(testAccountId);
    QCOMPARE(msg.parentAccountId(), testAccountId);
}
#ifdef QMESSAGING_OPTIONAL_FOLDER
void tst_QMessage::testParentFolderId()
{
    QMessage msg;
    QCOMPARE(msg.parentFolderId(), QMessageFolderId());

    // We can't set the parent folder directly, but we can verify that it has been modified
    msg.setStandardFolder(QMessage::TrashFolder);
    QVERIFY(msg.parentFolderId() != QMessageFolderId());
}

void tst_QMessage::testStandardFolder()
{
    QMessage msg;
    QCOMPARE(msg.standardFolder(), QMessage::InboxFolder);

    msg.setStandardFolder(QMessage::SentFolder);
    QCOMPARE(msg.standardFolder(), QMessage::SentFolder);

    msg.setStandardFolder(QMessage::TrashFolder);
    QCOMPARE(msg.standardFolder(), QMessage::TrashFolder);
}
#endif
void tst_QMessage::testFrom()
{
    QMessage msg;
    QCOMPARE(msg.from(), QMessageAddress());

    QMessageAddress addr("alice@example.org", QMessageAddress::Email);
    msg.setFrom(addr);
    QCOMPARE(msg.from(), addr);

    addr = QMessageAddress("bob@example.org", QMessageAddress::Xmpp);
    msg.setFrom(addr);
    QCOMPARE(msg.from(), addr);
}

void tst_QMessage::testSubject()
{
    QMessage msg;
    QCOMPARE(msg.subject(), QString());

    QString subject("Short message");
    msg.setSubject(subject);
    QCOMPARE(msg.subject(), subject);

    subject = QString("A slightly longer message than the predecessor message");
    msg.setSubject(subject);
    QCOMPARE(msg.subject(), subject);
}

void tst_QMessage::testDate()
{
    QMessage msg;
    QCOMPARE(msg.date(), QDateTime());

    QDateTime now(QDateTime::fromString(QDateTime::currentDateTime().toString(Qt::ISODate), Qt::ISODate));
    msg.setDate(now);
    QCOMPARE(msg.date(), now);

    now = QDateTime::fromString("2000-01-01T00:00:01Z", Qt::ISODate);
    msg.setDate(now);
    QCOMPARE(msg.date(), now);
}

void tst_QMessage::testReceivedDate()
{
    QMessage msg;
    QCOMPARE(msg.receivedDate(), QDateTime());

    QDateTime now(QDateTime::fromString(QDateTime::currentDateTime().toString(Qt::ISODate), Qt::ISODate));
    msg.setReceivedDate(now);
    QCOMPARE(msg.receivedDate(), now);

    now = QDateTime::fromString("2000-01-01T00:00:01Z", Qt::ISODate);
    msg.setReceivedDate(now);
    QCOMPARE(msg.receivedDate(), now);
}

void tst_QMessage::testTo()
{
    QMessage msg;
    QCOMPARE(msg.to(), QMessageAddressList());

    QMessageAddressList addresses;
    addresses.append(QMessageAddress("alice@example.org", QMessageAddress::Email));
    addresses.append(QMessageAddress("bob@example.org", QMessageAddress::Email));

    msg.setTo(addresses);
    QCOMPARE(msg.to(), addresses);

    addresses = QMessageAddressList();
    addresses.append(QMessageAddress("charlie@example.org", QMessageAddress::System));
    msg.setTo(addresses);
    QCOMPARE(msg.to(), addresses);
}

void tst_QMessage::testCc()
{
    QMessage msg;
    QCOMPARE(msg.cc(), QMessageAddressList());

    QMessageAddressList addresses;
    addresses.append(QMessageAddress("alice@example.org", QMessageAddress::Email));
    addresses.append(QMessageAddress("bob@example.org", QMessageAddress::Email));

    msg.setCc(addresses);
    QCOMPARE(msg.cc(), addresses);

    addresses = QMessageAddressList();
    addresses.append(QMessageAddress("charlie@example.org", QMessageAddress::Phone));
    msg.setCc(addresses);
    QCOMPARE(msg.cc(), addresses);
}

void tst_QMessage::testBcc()
{
    QMessage msg;
    QCOMPARE(msg.bcc(), QMessageAddressList());

    QMessageAddressList addresses;
    addresses.append(QMessageAddress("alice@example.org", QMessageAddress::Email));
    addresses.append(QMessageAddress("bob@example.org", QMessageAddress::Email));

    msg.setBcc(addresses);
    QCOMPARE(msg.bcc(), addresses);

    addresses = QMessageAddressList();
    addresses.append(QMessageAddress("charlie@example.org", QMessageAddress::Xmpp));
    msg.setBcc(addresses);
    QCOMPARE(msg.bcc(), addresses);
}

void tst_QMessage::testStatus()
{
    QMessage msg;
    QCOMPARE(msg.status(), static_cast<QMessage::StatusFlags>(0));

    msg.setStatus(QMessage::Removed);
    QCOMPARE(msg.status(), QMessage::Removed);

    msg.setStatus(QMessage::Read | QMessage::HasAttachments);
    QCOMPARE(msg.status(), QMessage::Read | QMessage::HasAttachments);
}

void tst_QMessage::testPriority()
{
    QMessage msg;
    QCOMPARE(msg.priority(), QMessage::NormalPriority);

    msg.setPriority(QMessage::HighPriority);
    QCOMPARE(msg.priority(), QMessage::HighPriority);

    msg.setPriority(QMessage::LowPriority);
    QCOMPARE(msg.priority(), QMessage::LowPriority);
}

#ifdef QMESSAGING_OPTIONAL
void tst_QMessage::testOriginatorPort()
{
    QMessage msg;
    QCOMPARE(msg.originatorPort(), 0u);

    msg.setOriginatorPort(0x1111u);
    QCOMPARE(msg.originatorPort(), 0x1111u);

    msg.setOriginatorPort(0xdeadu);
    QCOMPARE(msg.originatorPort(), 0xdeadu);
}

void tst_QMessage::testDestinationPort()
{
    QMessage msg;
    QCOMPARE(msg.destinationPort(), 0u);

    msg.setDestinationPort(0x1111u);
    QCOMPARE(msg.destinationPort(), 0x1111u);

    msg.setDestinationPort(0xdeadu);
    QCOMPARE(msg.destinationPort(), 0xdeadu);
}

void tst_QMessage::testCustomField()
{
    QMessage msg;
    QCOMPARE(msg.customFields(), QStringList());
    QCOMPARE(msg.customField("testing"), QString());

    msg.setCustomField("testing", "1-2-3");
    QCOMPARE(msg.customFields(), QStringList(( QSet<QString>() << "testing" ).toList()));
    QCOMPARE(msg.customField("testing"), QString("1-2-3"));

    msg.setCustomField("check", "one, two");
    QCOMPARE(msg.customFields(), QStringList(( QSet<QString>() << "testing" << "check" ).toList()));
    QCOMPARE(msg.customField("check"), QString("one, two"));

    // TODO: Is this what we want?
    msg.setCustomField("testing", "");
    QCOMPARE(msg.customFields(), QStringList(( QSet<QString>() << "testing" << "check" ).toList()));
    QCOMPARE(msg.customField("testing"), QString());

    msg.setCustomField("testing", QString());
    QCOMPARE(msg.customFields(), QStringList(( QSet<QString>() << "check" ).toList()));
    QCOMPARE(msg.customField("testing"), QString());
}

#endif

