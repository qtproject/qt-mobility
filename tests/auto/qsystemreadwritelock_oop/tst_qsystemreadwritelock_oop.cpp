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

#include <QtTest/QtTest>
#include <QProcess>
#include "qsystemreadwritelock_p.h"
#include "common.h"

class tst_QSystemReadWriteLock_oop : public QObject{
    Q_OBJECT
private slots:
    void readLockBlockRelease();
    void writeLockBlockRelease();
    void multipleReadersBlockRelease();
    void multipleReadersLoop();
    void multipleWritersLoop();
    void exclusiveWriteTest();
    void writerPrecedence();

private:
    bool waitForLine(QProcess *process, const QString &expectedLine,
                    bool print = false, int timeout=5000);
};

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

    QProcess reader;
    reader.setReadChannel(QProcess::StandardError);
    QStringList args;
    args << "ReadLock";
    reader.start("./lackey", args);
    QVERIFY(reader.waitForStarted());

    QVERIFY(waitForLine(&reader, Lackey::BeforeLockForRead,print));
    QVERIFY(!waitForLine(&reader, Lackey::AfterLockForRead,false,1000));

    testLock.unlock();
    if (print)
        qDebug() << "Main process: After unlock(write)";

    QVERIFY(waitForLine(&reader, Lackey::AfterLockForRead, print));
    QVERIFY(waitForLine(&reader, Lackey::AfterUnlockForRead, print));
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

    QProcess writer;
    writer.setReadChannel(QProcess::StandardError);
    QStringList args;
    args << "WriteLock";
    writer.start("./lackey", args);
    QVERIFY(writer.waitForStarted());

    QVERIFY(waitForLine(&writer, Lackey::BeforeLockForWrite,print));
    QVERIFY(!waitForLine(&writer, Lackey::AfterUnlockForWrite,false, 1000));

    testLock.unlock();
    if (print)
        qDebug() << "Main process: After unlock(write)";

    QVERIFY(waitForLine(&writer, Lackey::AfterLockForWrite, print));
    QVERIFY(waitForLine(&writer, Lackey::AfterUnlockForWrite, print));
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
    QStringList args;
    args << "ReadLockReleaseable";

    for( int i=0; i < numReaders; ++i) {
        readers[i].setReadChannel(QProcess::StandardError);
        readers[i].start("./lackey", args);
        QVERIFY(readers[i].waitForStarted());
        QVERIFY(waitForLine(&readers[i], Lackey::BeforeLockForRead, print));
        QVERIFY(waitForLine(&readers[i], Lackey::AfterLockForRead, print));
    }

    QProcess writer;
    writer.setReadChannel(QProcess::StandardError);
    args.clear();
    args << "WriteLock";
    writer.start("./lackey", args);
    QVERIFY(writer.waitForStarted());

    QVERIFY(waitForLine(&writer, Lackey::BeforeLockForWrite, print));
    QVERIFY(!waitForLine(&writer, Lackey::AfterLockForWrite, false, 1000));

    for (int i = 0; i < numReaders; ++i) {
        readers[i].write("release\n");
        QVERIFY(waitForLine(&readers[i], Lackey::AfterUnlockForRead, print));
        if ( i < (numReaders - 1))
            QVERIFY(!waitForLine(&writer, Lackey::AfterLockForWrite, false, 1000));
    }
    QVERIFY(waitForLine(&writer, Lackey::AfterLockForWrite, print));
    QVERIFY(waitForLine(&writer, Lackey::AfterUnlockForWrite, print));

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
    args << "ReadLockLoop"
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
    args << "WriteLockLoop"
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
    const int numReaders = 20;

    int readerHoldTime = 0;
    int readerWaitTime = 1;

    const int numWriters = 3;
    int writerHoldTime = 20;
    int writerWaitTime = 150;

    QProcess readers[numReaders];
    QProcess writers[numWriters];
    QStringList args;
    args << "ReadLockExcl"
        << QString::number(runTime)
        << QString::number(readerHoldTime)
        << QString::number(readerWaitTime);

    for (int i = 0; i < numReaders; ++i) {
        readers[i].setProcessChannelMode(QProcess::ForwardedChannels);
        readers[i].start("./lackey", args);
    }

    args.clear();
    args << "WriteLockExcl"
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

    QStringList args;
    args << "ReadLock";
    for (int i = 0; i < numReaders; ++i) {
        readers[i].setReadChannel(QProcess::StandardError);
        readers[i].start("./lackey", args);
        QVERIFY(waitForLine(&readers[i], Lackey::BeforeLockForRead, print));
        QVERIFY(!waitForLine(&readers[i], Lackey::AfterLockForRead, false, 1000));
    }

    QProcess writer;
    writer.setReadChannel(QProcess::StandardError);
    args.clear();
    args << "WriteLockReleaseable";
    writer.start("./lackey", args);

    QVERIFY(waitForLine(&writer, Lackey::BeforeLockForWrite, print));
    QVERIFY(!waitForLine(&writer, Lackey::AfterLockForWrite, false, 1000));

    testRwLock.unlock();
    QVERIFY(waitForLine(&writer, Lackey::AfterLockForWrite, print));
     if (print)
        qDebug() << "Main process: After unlock(write)";

   //verify the reader processes are still blocked
    QTest::qSleep(1000);
    for (int i = 0; i < numReaders; ++i) {
        QCOMPARE(readers[i].state(), QProcess::Running);
    }

    writer.write("release\n");
    QVERIFY(waitForLine(&writer, Lackey::AfterUnlockForWrite, print));

    for (int i = 0; i < numReaders; ++i) {
        QVERIFY(waitForLine(&readers[i], Lackey::AfterLockForRead, print));
    }//Note: 2 loops (ie above and below) are used due to timing issues
    for (int i = 0; i < numReaders; ++i) {
        QVERIFY(waitForLine(&readers[i], Lackey::AfterUnlockForRead, print));
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
bool tst_QSystemReadWriteLock_oop::waitForLine(QProcess * process,const QString &expectedLine, bool print, int timeout)
{
    if (!process->waitForReadyRead(timeout))
    {
        if (print)
            qWarning() << "Wait for ready read returned false";
        return false;
    }

    QString line = process->readLine();
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
