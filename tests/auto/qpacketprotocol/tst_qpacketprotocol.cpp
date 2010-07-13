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

//TESTED_COMPONENT=src/publishsubscribe

#include <qpacketprotocol_p.h>

#include <QByteArray>
#include <QBuffer>
#include <QLocalServer>
#include <QLocalSocket>

#include <QtTest/QTest>
#include <QSignalSpy>

#include <QDebug>

#define QTRY_COMPARE(a,e)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if ((a) == (e)) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QCOMPARE(a, e)

#define QTRY_VERIFY(a)                       \
    for (int _i = 0; _i < 5000; _i += 100) {    \
        if (a) break;                  \
        QTest::qWait(100);                      \
    }                                           \
    QVERIFY(a)

Q_DECLARE_METATYPE(QList<int>)

QTM_USE_NAMESPACE
class tst_QPacketProtocol : public QObject
{
    Q_OBJECT

private slots:
    void constructor();

    void maximumPacketSize_data();
    void maximumPacketSize();

    void sendReceive();
};

void tst_QPacketProtocol::constructor()
{
    QByteArray data;
    QBuffer buffer(&data);

    {
        QPacketProtocol protocol(&buffer);

        QCOMPARE(protocol.device(), (QIODevice*)&buffer);
    }
}

#define ROW_FOR_SIZE(x) x << x << (QList<int>() << 1 << 1 << (x - 4) << 1 << (x - 3) << 0)

void tst_QPacketProtocol::maximumPacketSize_data()
{
    QTest::addColumn<qint32>("maxPacketSize");
    QTest::addColumn<qint32>("actualMaxPacketSize");
    QTest::addColumn<QList<int> >("packetSizes");    // pairs of packetSize, readCount

#ifndef Q_OS_WIN
    //Don't perform these tests under windows
    //because they time out due to QLocalSocket
    //being too slow.  Packet sizes typically
    //won't be this large anyway.
    QTest::newRow("10MB") << ROW_FOR_SIZE(10 * 1024 * 1024);
    QTest::newRow("1MB") << ROW_FOR_SIZE(1024 * 1024);
#endif
    QTest::newRow("100kB") << ROW_FOR_SIZE(100 * 1024);
    QTest::newRow("10kB") << ROW_FOR_SIZE(10 * 1024);
    QTest::newRow("1kB") << ROW_FOR_SIZE(1024);
    QTest::newRow("100B") << ROW_FOR_SIZE(100);
    QTest::newRow("10B") << ROW_FOR_SIZE(10);
    QTest::newRow("5B") << ROW_FOR_SIZE(5);
    QTest::newRow("4B") << 4 << 0x7FFFFFFF << QList<int>();
    QTest::newRow("0B") << 0 << 0x7FFFFFFF << QList<int>();
    QTest::newRow("-1") << -1 << 0x7FFFFFFF << QList<int>();
}

void tst_QPacketProtocol::maximumPacketSize()
{
    QFETCH(qint32, maxPacketSize);
    QFETCH(qint32, actualMaxPacketSize);
    QFETCH(QList<int>, packetSizes);

    QLocalServer server;
    server.listen("tst_QPacketProtocol");

    QLocalSocket socket;
    socket.connectToServer("tst_QPacketProtocol", QIODevice::WriteOnly);

    bool timedOut;
    server.waitForNewConnection(5000, &timedOut);
    QVERIFY(!timedOut);

    QLocalSocket *serverSocket = server.nextPendingConnection();

    QVERIFY(serverSocket);

    QPacketProtocol writeProtocol(&socket);
    QPacketProtocol readProtocol(serverSocket);

    readProtocol.setMaximumPacketSize(maxPacketSize);

    QCOMPARE(readProtocol.maximumPacketSize(), actualMaxPacketSize);

    if (maxPacketSize == actualMaxPacketSize) {
        QVERIFY(!packetSizes.isEmpty());
        QVERIFY(packetSizes.count() % 2 == 0);

        while (!packetSizes.isEmpty()) {
            int packetSize = packetSizes.takeFirst();
            int readCount = packetSizes.takeFirst();

            QByteArray b(packetSize, 'T');
            QPacket sendPacket;
            sendPacket.writeRawData(b.constData(), packetSize);
            writeProtocol.send(sendPacket);

            QSignalSpy spy(&readProtocol, SIGNAL(readyRead()));
            QSignalSpy invalidSpy(&readProtocol, SIGNAL(invalidPacket()));

            QTRY_COMPARE(spy.count(), readCount);
            QTRY_COMPARE(invalidSpy.count(), 1 - readCount);

            QCOMPARE(readProtocol.packetsAvailable(), qint64(readCount));

            QPacket packet = readProtocol.read();

            QCOMPARE(packet.device()->size(), qint64(readCount * packetSize));
        }
    }
}

void tst_QPacketProtocol::sendReceive()
{
    QLocalServer server;
    server.listen("tst_QPacketProtocol");

    QLocalSocket socket;
    socket.connectToServer("tst_QPacketProtocol", QIODevice::WriteOnly);

    bool timedOut;
    server.waitForNewConnection(5000, &timedOut);
    QVERIFY(!timedOut);

    QLocalSocket *serverSocket = server.nextPendingConnection();
    QVERIFY(serverSocket);

    QPacketProtocol writeProtocol(&socket);
    QPacketProtocol readProtocol(serverSocket);

    QByteArray dataString("Data string");

    // Test sending with ::send(QPacket).
    for (int i = 1; i <= 10; ++i) {
        QSignalSpy writeSpy(&writeProtocol, SIGNAL(packetWritten()));
        QSignalSpy readSpy(&readProtocol, SIGNAL(readyRead()));

        // Test sending an empty packet.
        writeProtocol.send(QPacket());

        QPacket sendPacket;
        sendPacket << dataString;
        for (int j = 0; j < i; ++j)
            writeProtocol.send(sendPacket);

        QTRY_COMPARE(writeSpy.count(), i);
        QTRY_COMPARE(readSpy.count(), i);

        QCOMPARE(readProtocol.packetsAvailable(), qint64(i));
        for (int j = 0; j < i; ++j) {
            QPacket packet = readProtocol.read();
            QByteArray r;
            packet >> r;
            QCOMPARE(r, dataString);
        }

        QCOMPARE(readProtocol.packetsAvailable(), qint64(0));
    }

    // Test sending with ::send() << var.
    for (int i = 1; i <= 10; ++i) {
        QSignalSpy writeSpy(&writeProtocol, SIGNAL(packetWritten()));
        QSignalSpy readSpy(&readProtocol, SIGNAL(readyRead()));

        // Test sending an empty packet.
        writeProtocol.send();

        for (int j = 0; j < i; ++j)
            writeProtocol.send() << dataString.constData();

        QTRY_COMPARE(writeSpy.count(), i);
        QTRY_COMPARE(readSpy.count(), i);

        QCOMPARE(readProtocol.packetsAvailable(), qint64(i));
        for (int j = 0; j < i; ++j) {
            QPacket packet = readProtocol.read();
            char *string;
            packet >> string;
            QByteArray r(string);
            QCOMPARE(r, dataString);
        }

        QCOMPARE(readProtocol.packetsAvailable(), qint64(0));
    }

    // Test ::clear().
    for (int i = 1; i <= 10; ++i) {
        QByteArray b("Data string");

        QSignalSpy writeSpy(&writeProtocol, SIGNAL(packetWritten()));
        QSignalSpy readSpy(&readProtocol, SIGNAL(readyRead()));

        QPacket sendPacket;
        sendPacket << b;
        for (int j = 0; j < i; ++j)
            writeProtocol.send(sendPacket);

        QTRY_COMPARE(writeSpy.count(), i);
        QTRY_COMPARE(readSpy.count(), i);

        QCOMPARE(readProtocol.packetsAvailable(), qint64(i));

        readProtocol.clear();

        QCOMPARE(readProtocol.packetsAvailable(), qint64(0));
    }

    {
        QSignalSpy writeSpy(&writeProtocol, SIGNAL(packetWritten()));
        QSignalSpy readSpy(&readProtocol, SIGNAL(readyRead()));
        QSignalSpy closeSpy(serverSocket, SIGNAL(aboutToClose()));

        writeProtocol.send() << dataString.constData();

        serverSocket->close();
        socket.close();

        QTRY_VERIFY(!closeSpy.isEmpty());

        QTRY_VERIFY(writeSpy.isEmpty());
        QTRY_VERIFY(readSpy.isEmpty());
    }
}

QTEST_MAIN(tst_QPacketProtocol)
#include "tst_qpacketprotocol.moc"
