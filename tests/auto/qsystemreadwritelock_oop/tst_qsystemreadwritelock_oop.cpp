/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QDir>
#include <QProcess>
#include <QIODevice>
#include <QLocalServer>
#include <QLocalSocket>
#include "qsystemreadwritelock_p.h"
#include "common.h"

QTM_USE_NAMESPACE
class tst_QSystemReadWriteLock_oop : public QObject{
    Q_OBJECT
private slots:
    void initTestCase();
    void readLockBlockRelease();
    void writeLockBlockRelease();
    void multipleReadersBlockRelease();
    void multipleReadersLoop();
    void multipleWritersLoop();
    void exclusiveWriteTest();
    void writerPrecedence();

private:
    bool waitForLine(QIODevice *device, const QString &expectedLine,
                    bool print = false, int timeout=5000);

};

void tst_QSystemReadWriteLock_oop::initTestCase()
{
    // if an old writeLockExcl.tmp exists at the beginning of this test case, try to remove it,
    // otherwise test functions may fail
    QDir cwd;
    if (cwd.exists("writeLockExcl.tmp")) {
        QVERIFY(cwd.remove("writeLockExcl.tmp"));
    }
}

/*
    A writer aquires a write-lock, a reader tries to lock
    the writer releases the lock, the reader gets the lock
*/
void tst_QSystemReadWriteLock_oop::readLockBlockRelease()
{
    bool print = false;
    QSystemReadWriteLock testLock("Viper", QSystemReadWriteLock::Create);
    if (print)
        qDebug() << "Main process: About to lock for writing";
    testLock.lockForWrite();
    if (print)
        qDebug() << "Main process: After lock for writing";

    QLocalServer server;
    QString connectionName = "readLockBlockRelease";
    QVERIFY(QLocalServer::removeServer(connectionName));
    QVERIFY(server.listen(connectionName));

    QProcess reader;
    reader.setReadChannel(QProcess::StandardError);
    QStringList args;
    args << connectionName;
    args << "ReadLock";

    reader.start("./lackey", args);
    QVERIFY(reader.waitForStarted());

    QVERIFY(server.waitForNewConnection(5000));
    QLocalSocket *oopSocket = server.nextPendingConnection();

    QVERIFY(waitForLine(oopSocket, Lackey::BeforeLockForRead,print));
    QVERIFY(!waitForLine(oopSocket, Lackey::AfterLockForRead,false,1000));

    testLock.unlock();
    if (print)
        qDebug() << "Main process: After unlock(write)";

    QVERIFY(waitForLine(oopSocket, Lackey::AfterLockForRead, print));
    QVERIFY(waitForLine(oopSocket, Lackey::AfterUnlockForRead, print));
    QVERIFY(reader.waitForFinished());
}

/*
    writer1(main process) aquires a write-lock, writer2(lackey) blocks,
    writer1 releases the lock, writer2 gets the lock
*/
void tst_QSystemReadWriteLock_oop::writeLockBlockRelease()
{
    bool print = false;
    QSystemReadWriteLock testLock("Viper", QSystemReadWriteLock::Create);
    if (print)
        qDebug() << "Main process: About to lock for writing";
    testLock.lockForWrite();
    if (print)
        qDebug() << "Main process: After lock for writing";

    QLocalServer server;
    QString connectionName = "writeLockBlockRelease";
    QVERIFY(QLocalServer::removeServer(connectionName));
    QVERIFY(server.listen(connectionName));

    QProcess writer;
    writer.setReadChannel(QProcess::StandardError);
    QStringList args;
    args << connectionName;
    args << "WriteLock";
    writer.start("./lackey", args);
    QVERIFY(writer.waitForStarted());

    QVERIFY(server.waitForNewConnection(5000));
    QLocalSocket *oopSocket = server.nextPendingConnection();

    QVERIFY(waitForLine(oopSocket, Lackey::BeforeLockForWrite,print));
    QVERIFY(!waitForLine(oopSocket, Lackey::AfterUnlockForWrite,false, 1000));

    testLock.unlock();
    if (print)
        qDebug() << "Main process: After unlock(write)";

    QVERIFY(waitForLine(oopSocket, Lackey::AfterLockForWrite, print));
    QVERIFY(waitForLine(oopSocket, Lackey::AfterUnlockForWrite, print));
    QVERIFY(writer.waitForFinished());
}

/*
    Two readers aquire a read-lock, one writer attempts a write lock and blocks,
    the readers release their locks, the writer gets the lock.
*/
void tst_QSystemReadWriteLock_oop::multipleReadersBlockRelease()
{
    bool print = false;
    //Use the Create enum to ensure synchronization primitives are reset
    //to a "clean" state
    QSystemReadWriteLock rwlock("Viper", QSystemReadWriteLock::Create);

    const int numReaders = 2;
    QProcess readers[numReaders];
    QLocalServer readerServers[numReaders];
    QLocalSocket *readerSockets[numReaders];

    QString connectionName = "multipleReadersBlockRelease";

    QStringList args;
    args << "ReadLockReleaseable";

    for( int i=0; i < numReaders; ++i) {
        QString readerConnectionName = connectionName.append("_reader_").append(QString::number(i));
        QVERIFY(QLocalServer::removeServer(readerConnectionName));
        QVERIFY(readerServers[i].listen(readerConnectionName));
        args.push_front(readerConnectionName);

        readers[i].setReadChannel(QProcess::StandardError);
        readers[i].start("./lackey", args);
        QVERIFY(readers[i].waitForStarted());

        QVERIFY(readerServers[i].waitForNewConnection(5000));
        readerSockets[i] = readerServers[i].nextPendingConnection();

        QVERIFY(waitForLine(readerSockets[i], Lackey::BeforeLockForRead, print));
        QVERIFY(waitForLine(readerSockets[i], Lackey::AfterLockForRead, print));

        args.pop_front();
    }

    QLocalServer server;
    QString writerConnectionName = connectionName.append("_writer");
    QVERIFY(QLocalServer::removeServer(writerConnectionName));
    QVERIFY(server.listen(writerConnectionName));

    QProcess writer;
    writer.setReadChannel(QProcess::StandardError);
    args.clear();
    args << writerConnectionName;
    args << "WriteLock";
    writer.start("./lackey", args);
    QVERIFY(writer.waitForStarted());

    QVERIFY(server.waitForNewConnection(5000));
    QLocalSocket *writerSocket = server.nextPendingConnection();

    QVERIFY(waitForLine(writerSocket, Lackey::BeforeLockForWrite, print));
    QVERIFY(!waitForLine(writerSocket, Lackey::AfterLockForWrite, false, 1000));

    for (int i = 0; i < numReaders; ++i) {
        readerSockets[i]->write("release\n");
        QVERIFY(waitForLine(readerSockets[i], Lackey::AfterUnlockForRead, print));
        if ( i < (numReaders - 1))
            QVERIFY(!waitForLine(writerSocket, Lackey::AfterLockForWrite, false, 1000));
    }
    QVERIFY(waitForLine(writerSocket, Lackey::AfterLockForWrite, print));
    QVERIFY(waitForLine(writerSocket, Lackey::AfterUnlockForWrite, print));

    for(int i = 0; i < numReaders; ++i) {
        QVERIFY(readers[i].waitForFinished());
    }
    QVERIFY(writer.waitForFinished());
}

/*
    Multiple readers locks and unlocks a lock.
*/
void tst_QSystemReadWriteLock_oop::multipleReadersLoop()
{
    //ensure synchronization primitives are reset to a "clean" state
    QSystemReadWriteLock testLock("Viper", QSystemReadWriteLock::Create);

    int runTime = 10;
    int holdTime = 10;
    int waitTime = 0;

    const int numReaders = 10;
    QProcess readers[numReaders];
    QStringList args;
    args << "NoComms" // no communications between test and lackey
        << "ReadLockLoop"
        << QString::number(runTime)
        << QString::number(holdTime)
        << QString::number(waitTime);

    for (int i = 0; i < numReaders; ++i) {
        readers[i].setProcessChannelMode(QProcess::ForwardedChannels);
        readers[i].start("./lackey", args);
    }

    for (int i=0; i < numReaders; ++i) {
        QVERIFY(readers[i].waitForFinished(5000));
        QCOMPARE(readers[i].exitCode(), 0);
        QCOMPARE(readers[i].exitStatus(), QProcess::NormalExit);
    }
}

/*
    Multiple writers lock and unlock a lock
*/
void tst_QSystemReadWriteLock_oop::multipleWritersLoop()
{
    //ensure synchronization primitives are reset to a "clean" state
    QSystemReadWriteLock testLock("Viper", QSystemReadWriteLock::Create);

    int runTime = 10;
    int holdTime = 10;
    int waitTime = 0;

    const int numWriters = 10;
    QProcess writers[numWriters];
    QStringList args;
    args << "NoComms" // no communications between test and lackey
        << "WriteLockLoop"
        << QString::number(runTime)
        << QString::number(holdTime)
        << QString::number(waitTime);

    for (int i = 0; i < numWriters; ++i) {
        writers[i].setProcessChannelMode(QProcess::ForwardedChannels);
        writers[i].start("./lackey", args);
    }

    for (int i = 0; i < numWriters; ++i) {
        QVERIFY(writers[i].waitForFinished(5000));
        QCOMPARE(writers[i].exitCode(), 0);
        QCOMPARE(writers[i].exitStatus(), QProcess::NormalExit);
    }
}

/*
    Writers create an exclusive file while they have a write lock
    and remove it on unlock.  Readers verify that that the file
    does not exist and writers verify that the file does not
    already exist when they obtain a lock.
*/
void tst_QSystemReadWriteLock_oop::exclusiveWriteTest()
{
    //ensure synchronization primitives are reset to a "clean" state
    QSystemReadWriteLock testLock("Viper", QSystemReadWriteLock::Create);

    int runTime=10000;
#if !defined(Q_OS_WINCE)
    const int numReaders = 20;
#else
    const int numReaders = 10;
#endif

    int readerHoldTime = 0;
    int readerWaitTime = 1;

    const int numWriters = 3;
    int writerHoldTime = 20;
    int writerWaitTime = 150;

    QProcess readers[numReaders];
    QProcess writers[numWriters];
    QStringList args;
    args << "NoComms" // no communications between test and lackey
        << "ReadLockExcl"
        << QString::number(runTime)
        << QString::number(readerHoldTime)
        << QString::number(readerWaitTime);

    for (int i = 0; i < numReaders; ++i) {
        readers[i].setProcessChannelMode(QProcess::ForwardedChannels);
        readers[i].start("./lackey", args);
    }

    args.clear();
    args << "NoComms" // no communications between test and lackey
        << "WriteLockExcl"
        << QString::number(runTime)
        << QString::number(writerHoldTime)
        << QString::number(writerWaitTime);
    for (int i = 0; i < numWriters; ++i) {
        writers[i].setProcessChannelMode(QProcess::ForwardedChannels);
        writers[i].start("./lackey", args);
    }

    // Finding the existence of the exclusive file will
    // result in a qFatal() in the lackey process
    // The exit status checks below will fail if this
    // is the case
    for (int i = 0; i < numReaders; ++i) {
        QVERIFY(readers[i].waitForFinished(2 * runTime));
        QCOMPARE(readers[i].exitCode(), 0);
        QCOMPARE(readers[i].exitStatus(), QProcess::NormalExit);
     }

    for (int i = 0; i < numWriters; ++i) {
        QVERIFY(writers[i].waitForFinished(2 * runTime));
        QCOMPARE(writers[i].exitCode(), 0);
        QCOMPARE(writers[i].exitStatus(), QProcess::NormalExit);
     }
}

/*
    Writer1(main process) acquires a write-lock
    Readers try to acquire read-locks but are blocked
    Writer2(lackey) tries to acquire a write-lock, but is blocked
    Writer1 releases the lock, writer2 gets the lock, readers still blocked
    Writer2 releases the lock, readers are unblocked and get their read-locks
*/
void tst_QSystemReadWriteLock_oop::writerPrecedence()
{
    bool print = false;
    QSystemReadWriteLock testRwLock("Viper", QSystemReadWriteLock::Create);
    if (print)
        qDebug() << "Main process: About to lock for writing";
    testRwLock.lockForWrite();
     if (print)
        qDebug() << "Main process: After lock for writing";

    const int numReaders = 5;
    QProcess readers[numReaders];
    QLocalServer readerServers[numReaders];
    QLocalSocket *readerSockets[numReaders];

    QString connectionName = "writePrecedence";

    QStringList args;
    args << "ReadLock";
    for (int i = 0; i < numReaders; ++i) {
        QString readerConnectionName = connectionName.append("_reader_").append(QString::number(i));
        QVERIFY(QLocalServer::removeServer(readerConnectionName));
        QVERIFY(readerServers[i].listen(readerConnectionName));
        args.push_front(readerConnectionName);

        readers[i].setReadChannel(QProcess::StandardError);
        readers[i].start("./lackey", args);
        QVERIFY(readers[i].waitForStarted());

        QVERIFY(readerServers[i].waitForNewConnection(5000));
        readerSockets[i] = readerServers[i].nextPendingConnection();

        QVERIFY(waitForLine(readerSockets[i], Lackey::BeforeLockForRead, print));
        QVERIFY(!waitForLine(readerSockets[i], Lackey::AfterLockForRead, false, 1000));

        args.pop_front();
    }

    QLocalServer server;
    QString writerConnectionName = connectionName.append("_writer");
    QVERIFY(QLocalServer::removeServer(writerConnectionName));
    QVERIFY(server.listen(writerConnectionName));

    QProcess writer;
    writer.setReadChannel(QProcess::StandardError);
    args.clear();
    args << writerConnectionName;
    args << "WriteLockReleaseable";
    writer.start("./lackey", args);

    QVERIFY(server.waitForNewConnection(5000));
    QLocalSocket *writerSocket = server.nextPendingConnection();

    QVERIFY(waitForLine(writerSocket, Lackey::BeforeLockForWrite, print));
    QVERIFY(!waitForLine(writerSocket, Lackey::AfterLockForWrite, false, 1000));

    testRwLock.unlock();
    QVERIFY(waitForLine(writerSocket, Lackey::AfterLockForWrite, print));
     if (print)
        qDebug() << "Main process: After unlock(write)";

   //verify the reader processes are still blocked
    QTest::qSleep(1000);
    for (int i = 0; i < numReaders; ++i) {
        QCOMPARE(readers[i].state(), QProcess::Running);
    }

    writerSocket->write("release\n");
    QVERIFY(waitForLine(writerSocket, Lackey::AfterUnlockForWrite, print));

    for (int i = 0; i < numReaders; ++i) {
        QVERIFY(waitForLine(readerSockets[i], Lackey::AfterLockForRead, print));
    }//Note: 2 loops (ie above and below) are used due to timing issues
    for (int i = 0; i < numReaders; ++i) {
        QVERIFY(waitForLine(readerSockets[i], Lackey::AfterUnlockForRead, print));
    }

    for (int i = 0; i < numReaders; ++i) {
        QVERIFY(readers[i].waitForFinished(5000));
    }
    QVERIFY(writer.waitForFinished(5000));
}


/*
   Waits for a given \a expectedLine to be written by \a process
   for a given \a timeout(msecs).  The line written by the process
   is printed to console if \a print is true.
*/
bool tst_QSystemReadWriteLock_oop::waitForLine(QIODevice * device,const QString &expectedLine, bool print, int timeout)
{
    if (!device->waitForReadyRead(timeout))
    {
        if (print)
            qWarning() << "Wait for ready read returned false";
        return false;
    }

    QString line = device->readLine();
    line = line.trimmed();
    if (print)
        qDebug() << qPrintable(line);
    if (line.compare(expectedLine) !=0) {
        qWarning() << "Expected Line: \t" << qPrintable(expectedLine);
        qWarning() << "Actual Line: \t" << qPrintable(line);
        return false;
    }

    return true;
}

QTEST_MAIN(tst_QSystemReadWriteLock_oop)
#include "tst_qsystemreadwritelock_oop.moc"
