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

QTM_USE_NAMESPACE
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
    void testMessageAddress_data();
    void testMessageAddress();
    void testHeaderFields();

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
#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
    if (!Support::mapiAvailable())
        QSKIP("Skipping tests because a MAPI subsystem does not appear to be available", SkipAll);
#endif

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
        p.insert("name", "Root");
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

    QMessageAddress addr(QMessageAddress::Email, "alice@example.org");
    msg.setFrom(addr);
    QCOMPARE(msg.from(), addr);
    QCOMPARE(msg.from() != QMessageAddress(), true);
    QCOMPARE(msg.isModified(), true);

    addr = QMessageAddress(QMessageAddress::InstantMessage, "bob@example.org");
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
    addresses.append(QMessageAddress(QMessageAddress::Email, "alice@example.org"));
    addresses.append(QMessageAddress(QMessageAddress::Email, "bob@example.org"));

    msg.setTo(addresses);
    QCOMPARE(msg.to(), addresses);
    QCOMPARE(msg.isModified(), true);

    addresses = QMessageAddressList();
    addresses.append(QMessageAddress(QMessageAddress::System, "charlie@example.org"));
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
    addresses.append(QMessageAddress(QMessageAddress::Email, "alice@example.org"));
    addresses.append(QMessageAddress(QMessageAddress::Email, "bob@example.org"));

    msg.setCc(addresses);
    QCOMPARE(msg.cc(), addresses);
    QCOMPARE(msg.isModified(), true);

    addresses = QMessageAddressList();
    addresses.append(QMessageAddress(QMessageAddress::Phone, "charlie@example.org"));
    msg.setCc(addresses);
    QCOMPARE(msg.cc(), addresses);
}

void tst_QMessage::testBcc()
{
    QMessage msg;
    QCOMPARE(msg.bcc(), QMessageAddressList());
    QCOMPARE(msg.isModified(), false);

    QMessageAddressList addresses;
    addresses.append(QMessageAddress(QMessageAddress::Email, "alice@example.org"));
    addresses.append(QMessageAddress(QMessageAddress::Email, "bob@example.org"));

    msg.setBcc(addresses);
    QCOMPARE(msg.bcc(), addresses);
    QCOMPARE(msg.isModified(), true);

    addresses = QMessageAddressList();
    addresses.append(QMessageAddress(QMessageAddress::InstantMessage, "charlie@example.org"));
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

void tst_QMessage::testMessageAddress_data()
{
    QTest::addColumn<QString>("from");
    QTest::addColumn<QString>("targetName");
    QTest::addColumn<QString>("targetAddress");

    QTest::newRow("no angle brackets") 
        << "wizard@oz.test" 
        << "wizard@oz.test" 
        << "wizard@oz.test";

    QTest::newRow("name and address") 
        << "Wizard of Oz <wizard@oz.test>" 
        << "Wizard of Oz" 
        << "wizard@oz.test";

    QTest::newRow("quoted name and address") 
        << "\"First Last\" <first.last@example.com>"
        << "\"First Last\""
        << "first.last@example.com";

    QTest::newRow("quoted name with bracket and address") 
        << "\"First <Middle> Last\" <first.last@example.com>"
        << "\"First <Middle> Last\""
        << "first.last@example.com";
}

void tst_QMessage::testMessageAddress()
{
    QFETCH(QString, from); 
    QFETCH(QString, targetName); 
    QFETCH(QString, targetAddress); 

    QString name;
    QString address;
    QMessageAddress::parseEmailAddress(from, &name, &address);
    QCOMPARE(targetName, name);
    QCOMPARE(targetAddress, address);
}

void tst_QMessage::testHeaderFields()
{
    QMessage msg;
    
    QCOMPARE(msg.headerFields().isEmpty(), true);
    QCOMPARE(msg.headerFieldValues("Subject").isEmpty(), true);
    QCOMPARE(msg.headerFieldValues("From").isEmpty(), true);
    QCOMPARE(msg.headerFieldValues("To").isEmpty(), true);
    QCOMPARE(msg.headerFieldValues("X-None").isEmpty(), true);
    QCOMPARE(msg.headerFieldValue("Subject").isEmpty(), true);
    QCOMPARE(msg.headerFieldValue("From").isEmpty(), true);
    QCOMPARE(msg.headerFieldValue("To").isEmpty(), true);
    QCOMPARE(msg.headerFieldValue("X-None").isEmpty(), true);
}

