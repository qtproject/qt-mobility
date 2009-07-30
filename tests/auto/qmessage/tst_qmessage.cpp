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

    void testFromTransmissionFormat_data();
    void testFromTransmissionFormat();

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
}

void tst_QMessage::cleanup()
{
}

void tst_QMessage::cleanupTestCase()
{
}

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
        QCOMPARE(body.contentAvailable(), true);
        QCOMPARE(body.decodedTextContent(), text);
    }
}

