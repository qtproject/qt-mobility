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

#include <QtTest/QtTest>
#include "qsystemreadwritelock_p.h"
#include <QThread>
#include <QDebug>

QTM_USE_NAMESPACE
bool volatile readThreadDone;
bool volatile writeThreadDone;
bool volatile readThreadStarted;
bool volatile writeThreadStarted;
bool volatile release;

const QString BeforeLockForRead("ReadThread: About to lock for reading");
const QString AfterLockForRead("ReadThread: After lock for reading");
const QString AfterUnlockForRead("ReadThread: After unlock(read)");

const QString BeforeLockForWrite("WriteThread: About to lock for writing");
const QString AfterLockForWrite("WriteThread: After lock for writing");
const QString AfterUnlockForWrite("WriteThread: After unlock(write)");

/*
    set readThreadStarted
    read-lock
    unlock
    set readThreadDone
*/
class ReadLockThread : public QThread
{
public:
    bool print;
    inline ReadLockThread(bool print=false)
        :print(print)
    {}
    void run()
    {
        QSystemReadWriteLock testRwLock("Viper");
        readThreadStarted = true;
        if (print)
            qDebug() << qPrintable(BeforeLockForRead);
        testRwLock.lockForRead();
        if (print)
            qDebug() << qPrintable(AfterLockForRead);
        testRwLock.unlock();
        if (print)
            qDebug() << qPrintable(AfterUnlockForRead);
        readThreadDone = true;
    }
};

/*
    set writeThreadStarted
    write-lock
    lock
    set writeThreadDone
*/
class WriteLockThread : public QThread
{
public:
    bool print;
    inline WriteLockThread(bool print = false)
        :print(print)
    {}
    void run()
    {
        QSystemReadWriteLock testRwLock("Viper");
        writeThreadStarted = true;
        if (print)
            qDebug() << qPrintable(BeforeLockForWrite);
        testRwLock.lockForWrite();
        if (print)
            qDebug() << qPrintable(AfterLockForWrite);
        testRwLock.unlock();
        if (print)
            qDebug() << qPrintable(AfterUnlockForWrite);
        writeThreadDone = true;
    }
};

/*
   read-lock
   wait for release==true
   unlock
*/
class ReadLockReleaseableThread : public QThread
{
public:
    bool print;
    inline ReadLockReleaseableThread(bool print = false)
        :print(print)
    {}
    void run()
    {
        QSystemReadWriteLock testRwLock("Viper");
    if (print)
            qDebug() << qPrintable(BeforeLockForRead);
        testRwLock.lockForRead();
        if (print)
            qDebug() << qPrintable(AfterLockForRead);
        while (release == false) {
            usleep(1);
        }
        testRwLock.unlock();
        if (print)
            qDebug() << qPrintable(AfterUnlockForRead);
    }
};

/*
    write-lock
    wait for release == true
    unlock
*/
class WriteLockReleaseableThread : public QThread
{
public:
    bool print;
    inline WriteLockReleaseableThread(bool print = false)
        :print(print)
    {}
    void run()
    {
        QSystemReadWriteLock testRwLock("Viper");
        writeThreadStarted = true;
        if (print)
            qDebug() << BeforeLockForWrite;
        testRwLock.lockForWrite();
        if (print)
            qDebug() << AfterLockForWrite;
        while(release == false) {
            usleep(1);
        }
        testRwLock.unlock();
        if (print)
            qDebug() << AfterUnlockForWrite;
        writeThreadDone = true;
    }
};

/*
    for(runTime msecs)
    read-lock
    msleep(holdTime msecs)
    release lock
    msleep(waitTime msecs)
*/
class ReadLockLoopThread : public QThread
{
public:
    int runTime;
    int holdTime;
    int waitTime;
    bool print;
    QTime t;
    inline ReadLockLoopThread(int runTime, int holdTime=0, int waitTime=0, bool print=false)
        : runTime(runTime)
         ,holdTime(holdTime)
         ,waitTime(waitTime)
         ,print(print)
    { }
    void run()
    {
        QSystemReadWriteLock testRwLock("Viper");
        t.start();
        while (t.elapsed()<runTime)  {
            testRwLock.lockForRead();
            //if (print) printf("reading\n");
            if (holdTime) msleep(holdTime);
            testRwLock.unlock();
            if (waitTime) msleep(waitTime);
        }
    }
};

/*
   for(runTime msecs)
   write-lock
   msleep(holdTime msecs)
   release lock
   msleep(waitTime msecs)
*/
class WriteLockLoopThread : public QThread
{
public:
    int runTime;
    int holdTime;
    int waitTime;
    bool print;
    QTime t;
    inline WriteLockLoopThread(int runTime, int holdTime=0, int waitTime=0, bool print=false)
        : runTime(runTime)
         ,holdTime(holdTime)
         ,waitTime(waitTime)
         ,print(print)
    { }
    void run()
    {
        QSystemReadWriteLock testRwLock("Viper");
        t.start();
        while (t.elapsed() < runTime)  {
            if (print) qDebug() << "About to lock for write";
            testRwLock.lockForWrite();
            if (print) qDebug() << "Locked for Write";
            if (holdTime) msleep(holdTime);
            testRwLock.unlock();
            if (print) qDebug() << "unlocked";
            if (waitTime) msleep(waitTime);
        }
    }
};

volatile int count=0;

/*
   for(runTime msecs)
   write-lock
   count to maxval
   set count to 0
   release lock
   msleep (waitTime)
*/
class WriteLockCountThread : public QThread
{
public:
    int runTime;
    int waitTime;
    int maxval;
    QTime t;
    inline WriteLockCountThread(int runTime, int waitTime, int maxval)
        : runTime(runTime)
         ,waitTime(waitTime)
         ,maxval(maxval)
    { }

    void run()
    {
        QSystemReadWriteLock testRwLock("Viper");
        t.start();
        while (t.elapsed() < runTime)  {
            testRwLock.lockForWrite();
            if(count)
                qFatal("Non-zero count at start of write! (%d)",count );
            for(int i = 0; i < maxval; ++i) {
                volatile int lc=count;
                ++lc;
                count=lc;
            }
            count=0;
            testRwLock.unlock();
            msleep(waitTime);
        }
    }
};

/*
   for(runTime msecs)
   read-lock
   verify count==0
   release lock
   msleep(waitTime)
*/
class ReadLockCountThread : public QThread
{
public:
    int runTime;
    int waitTime;
    QTime t;
    inline ReadLockCountThread(int runTime, int waitTime)
        : runTime(runTime)
         ,waitTime(waitTime)
    { }

    void run()
    {
        QSystemReadWriteLock testRwLock("Viper");
        t.start();
        while (t.elapsed() < runTime)  {
            testRwLock.lockForRead();
            if(count)
                qFatal("Non-zero count at Read! (%d)",count );
            testRwLock.unlock();
            msleep(waitTime);
        }
    }
};

class tst_QSystemReadWriteLock : public QObject {
    Q_OBJECT

private slots:
    void constructDestruct();
    void readLockUnlock();
    void writeLockUnlock();
    void readLockUnlockLoop();
    void writeLockUnlockLoop();
    void readLockLoop();
    void readWriteLockUnlockLoop();
    void readLockBlockRelease();
    void writeLockBlockRelease();
    void multipleReadersBlockRelease();
    void multipleReadersLoop();
    void multipleWritersLoop();
    void multipleReadersWritersLoop();
    void countingTest();
    void uncontendedLocks();
    void writerPrecedence();
    void errorCheck();
};

void tst_QSystemReadWriteLock::constructDestruct()
{
    {
        QSystemReadWriteLock rwlock("Viper");
        QCOMPARE(rwlock.key(), QLatin1String("Viper"));
    }
}

void tst_QSystemReadWriteLock::readLockUnlock()
{
    QSystemReadWriteLock rwlock("Viper");
    rwlock.lockForRead();
    rwlock.unlock();
}

void tst_QSystemReadWriteLock::writeLockUnlock()
{
    QSystemReadWriteLock rwlock("Viper");
    rwlock.lockForWrite();
    rwlock.unlock();
}

void tst_QSystemReadWriteLock::readLockUnlockLoop()
{
    QSystemReadWriteLock rwlock("Viper");
    int runs = 10000;
    for (int i=0; i < runs; ++i) {
        rwlock.lockForRead();
        rwlock.unlock();
    }
}

void tst_QSystemReadWriteLock::writeLockUnlockLoop()
{
    QSystemReadWriteLock rwlock("Viper");
    int runs = 100000;
    for (int i = 0; i < runs; ++i) {
        rwlock.lockForWrite();
        rwlock.unlock();
    }
}

void tst_QSystemReadWriteLock::readLockLoop()
{
    QSystemReadWriteLock rwlock("Viper");
    int runs = 10000;
    for (int i = 0; i < runs; ++i) {
        rwlock.lockForRead();
    }
    for (int i=0; i < runs; ++i) {
        rwlock.unlock();
    }
}

void tst_QSystemReadWriteLock::readWriteLockUnlockLoop()
{
    QSystemReadWriteLock rwlock("Viper");
    int runs = 10000;
    for (int i = 0; i < runs; ++i) {
        rwlock.lockForRead();
        rwlock.unlock();
        rwlock.lockForWrite();
        rwlock.unlock();
    }
}

/*
    A writer acquires a write-lock, a reader tries to lock.
    the writer releases the lock, the reader gets the lock
*/
void tst_QSystemReadWriteLock::readLockBlockRelease()
{
    bool print = false;
    QSystemReadWriteLock testLock("Viper", QSystemReadWriteLock::Create);
    if (print)
        qDebug() << "Main Thread: About to lock for writing";
    testLock.lockForWrite();
    if (print)
        qDebug() << "Main Thread: After lock for writing";
    readThreadStarted = false;
    readThreadDone = false;

    ReadLockThread rlt(print);
    rlt.start();
    QTest::qSleep(1000);
    QVERIFY(readThreadStarted);
    QVERIFY(!readThreadDone);

    testLock.unlock();
    if (print)
        qDebug() << "Main Thread:  After unlock(write)";
    rlt.wait();
    QVERIFY(readThreadDone);
}

/*
    writer1 acquires a write-lock, writer 2 blocks,
    writer1 releases the lock, writer 2 gets the lock
*/
void tst_QSystemReadWriteLock::writeLockBlockRelease()
{
    bool print = false;
    QSystemReadWriteLock testLock("Viper");
    if (print)
        qDebug() << "Main Thread:  About to lock for writing"; 
    testLock.lockForWrite();
    if (print)
        qDebug() << "Main Thread: After lock for writing";
    writeThreadStarted = false;
    writeThreadDone = false;

    WriteLockThread wlt(print);
    wlt.start();
    QTest::qSleep(1000);
    QVERIFY(writeThreadStarted);
    QVERIFY(!writeThreadDone);

    testLock.unlock();
    if (print)
        qDebug() << "MainThread: After unlock(write)";
    QVERIFY(wlt.wait(5000));
    QVERIFY(writeThreadDone);
}

/*
    Two readers aquire a read-lock, one writer attempts a write block,
    the readers release their locks, the writer gets the lock.
*/
void tst_QSystemReadWriteLock::multipleReadersBlockRelease()
{
    bool print = false;
    QSystemReadWriteLock testLock("Viper", QSystemReadWriteLock::Create);
    release = false;
    writeThreadStarted = false;
    writeThreadDone = false;

    ReadLockReleaseableThread rlt1(print);
    ReadLockReleaseableThread rlt2(print);
    rlt1.start();
    rlt2.start();
    QTest::qSleep(1000);
    WriteLockThread wlt(print);
    wlt.start();
    QTest::qSleep(1000);
    QVERIFY(writeThreadStarted);
    QVERIFY(!writeThreadDone);
    QVERIFY(wlt.isRunning());
    QVERIFY(rlt1.isRunning());
    QVERIFY(rlt2.isRunning());
    release = true;
    QVERIFY(wlt.wait());
    QVERIFY(rlt1.wait(2000));
    QVERIFY(rlt2.wait(2000));
    QVERIFY(writeThreadDone);
}

/*
    Multiple readers locks and unlocks a lock.
*/
void tst_QSystemReadWriteLock::multipleReadersLoop()
{
    QSystemReadWriteLock testLock("Viper", QSystemReadWriteLock::Create);
    int time=500;
    int hold=250;
    int wait=0;
#if defined (Q_OS_HPUX)
    const int numthreads=50;
#else
    const int numthreads=75;
#endif
    ReadLockLoopThread *threads[numthreads];
    int i;
    for (i=0; i < numthreads; ++i)
        threads[i] = new ReadLockLoopThread(time, hold, wait,false);
    for (i=0; i<numthreads; ++i)
        threads[i]->start();
    for (i=0; i<numthreads; ++i)
        QVERIFY(threads[i]->wait());
    for (i=0; i<numthreads; ++i)
        delete threads[i];
}

/*
   Multiple writers locks and unlocks a lock.
*/
void tst_QSystemReadWriteLock::multipleWritersLoop()
{
    QSystemReadWriteLock testLock("Viper", QSystemReadWriteLock::Create);
    int time=500;
    int wait=0;
    int hold=0;
    const int numthreads=2;
    WriteLockLoopThread *threads[numthreads];
    int i;
    for (i = 0; i < numthreads; ++i)
        threads[i] = new WriteLockLoopThread(time, hold, wait,false);
    for (i = 0; i < numthreads; ++i)
        threads[i]->start();
    for (i = 0; i < numthreads; ++i)
        QVERIFY(threads[i]->wait(5000));
    for (i = 0; i < numthreads; ++i)
        delete threads[i];
}


typedef ReadLockLoopThread* ReadLockLoopThreadPtr;
typedef WriteLockLoopThread* WriteLockLoopThreadPtr;

/*
    Multiple readers and writers locks and unlocks a lock.
*/
void tst_QSystemReadWriteLock::multipleReadersWritersLoop()
{
    QSystemReadWriteLock testLock("Viper", QSystemReadWriteLock::Create);
    int time=10000;
    int numReaders=20;
    int readerWait=0;
    int readerHold=1;

    int numWriters=3;
    int writerWait=500;
    int writerHold=50;

    ReadLockLoopThreadPtr *readers = new ReadLockLoopThreadPtr[numReaders];
    WriteLockLoopThreadPtr *writers = new WriteLockLoopThreadPtr[numWriters];

    int i;

    for (i = 0; i < numReaders; ++i)
        readers[i] = new ReadLockLoopThread(time, readerHold, readerWait, false);
    for (i = 0; i < numWriters; ++i)
        writers[i] = new WriteLockLoopThread(time, writerHold, writerWait, false);

    for (i = 0; i < numReaders; ++i)
        readers[i]->start(QThread::NormalPriority);
    for (i = 0; i < numWriters; ++i)
        writers[i]->start(QThread::IdlePriority);

    for (i = 0; i < numReaders; ++i)
        QVERIFY(readers[i]->wait(time*2));
    for (i = 0; i < numWriters; ++i)
        QVERIFY(writers[i]->wait(time*2));

    for (i = 0; i < numReaders; ++i)
        delete readers[i];
    for (i = 0; i < numWriters; ++i)
        delete writers[i];
    delete[] readers;
    delete[] writers;
}


typedef ReadLockCountThread* ReadLockCountThreadPtr;
typedef WriteLockCountThread* WriteLockCountThreadPtr;
/*
   Writers increment a variable from 0 to maxval, then reset it to 0.
   Readers verify that the variable remains at 0.
*/
void tst_QSystemReadWriteLock::countingTest()
{
    QSystemReadWriteLock testLock("Viper", QSystemReadWriteLock::Create);
    //int time=INT_MAX;
    int time=10000;
    int numReaders=20;
    int readerWait=1;

    int numWriters=3;
    int writerWait=150;
    int maxval=10000;

    ReadLockCountThreadPtr  *readers = new ReadLockCountThreadPtr[numReaders];
    WriteLockCountThreadPtr *writers= new WriteLockCountThreadPtr[numWriters];
    int i;

    for (i = 0; i < numReaders; ++i)
        readers[i] = new ReadLockCountThread(time,  readerWait);
    for (i = 0; i < numWriters; ++i)
        writers[i] = new WriteLockCountThread(time,  writerWait, maxval);

    for (i = 0; i < numReaders; ++i)
        readers[i]->start(QThread::NormalPriority);
    for (i = 0; i < numWriters; ++i)
        writers[i]->start(QThread::LowestPriority);

    for (i = 0; i < numReaders; ++i)
        QVERIFY(readers[i]->wait(2 * time));
    for (i = 0; i < numWriters; ++i)
        QVERIFY(writers[i]->wait(2 * time));
    for (i = 0; i < numReaders; ++i)
        delete readers[i];
    for (i = 0; i < numWriters; ++i)
        delete writers[i];
    delete [] readers;
    delete [] writers;
}

/*
    Performance test to see how many uncontended
    locks and unlocks can be performed in a given
    timeframe.
*/
void tst_QSystemReadWriteLock::uncontendedLocks()
{
    uint read=0;
    uint write=0;
    uint count=0;
    int millisecs=1000;
    {
        QTime t;
        t.start();
        while(t.elapsed() <millisecs)
        {
            ++count;
        }
    }
    {
        QSystemReadWriteLock rwLock("Viper", QSystemReadWriteLock::Create);
        QTime t;
        t.start();
        while(t.elapsed() <millisecs)
        {
            rwLock.lockForRead();
            rwLock.unlock();
            ++read;
        }
    }
    {
        QSystemReadWriteLock rwLock("Viper", QSystemReadWriteLock::Create);
        QTime t;
        t.start();
        while(t.elapsed() <millisecs)
        {
            rwLock.lockForWrite();
            rwLock.unlock();
            ++write;
        }
    }

    qDebug("during %d millisecs:", millisecs);
    qDebug("counted to %u", count);
    qDebug("%u uncontended read locks/unlocks", read);
    qDebug("%u uncontended write locks/unlocks", write);
}

/*
   Writer1 acquires a write-lock
   Readers try to acquire read-locks but are blocked
   Writer2 tries to acquire a write-lock, but is blocked
   Writer1 releases the lock, writer2 gets the lock, readers still blocked
   Writer2 releases the lock, readers are unblocked and get their read-locks
*/
void tst_QSystemReadWriteLock::writerPrecedence()
{
    bool print = false;
    writeThreadStarted = false;
    writeThreadDone = false;
    release = false;

    QSystemReadWriteLock testRwLock("Viper", QSystemReadWriteLock::Create);
    if (print)
        qDebug() << "Main Thread: About to lock for writing";
    testRwLock.lockForWrite();
    if (print)
        qDebug() << "Main Thread: After lock for writing";

    const int numReaders = 5;
    ReadLockThread *readers[numReaders];
    for (int i = 0; i < numReaders; ++i) {
        readers[i] = new ReadLockThread(print);
        readers[i]->start();
    }

    //check that the readers are still running
    //and so are likely to be correctly blocking
    QTest::qSleep(1000);
    for (int i = 0; i < numReaders; ++i) {
        QVERIFY(!readers[i]->isFinished());
    }

    // Start the writer2 thread and ensure it blocks
    WriteLockReleaseableThread *writer = new WriteLockReleaseableThread(print);
    writer->start();
    QTest::qSleep(1000);
    QVERIFY(writeThreadStarted == true);
    QVERIFY(writeThreadDone == false);
    QVERIFY(!writer->isFinished());

    // Unlock writer1 and verify the readers are still blocked
    testRwLock.unlock();
    if (print)
        qDebug() << "Main Thread: After unlock(write)";
    QTest::qSleep(1000);
    for (int i = 0; i < numReaders; ++i) {
        QVERIFY(!readers[i]->isFinished());
    }

    //Release writer2 and verify that it and the readers
    //finish executing.
    QVERIFY(!writer->isFinished());
    release = true;
    QTest::qSleep(1000);
    QVERIFY(writeThreadDone == true);
    QVERIFY(writer->isFinished());
    for (int i = 0; i < numReaders; ++i ) {
        QVERIFY(readers[i]->isFinished());
        delete readers[i];
    }
    delete writer;
}

/*
    Completeness error test - need to replace with proper error-inducing code
*/
void tst_QSystemReadWriteLock::errorCheck()
{
    QSystemReadWriteLock testLock("Viper");
    QVERIFY(testLock.error() == 0);
    QCOMPARE(testLock.errorString(), QString(""));
}

QTEST_MAIN(tst_QSystemReadWriteLock)
#include "tst_qsystemreadwritelock.moc"
