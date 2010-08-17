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

#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include "qsystemreadwritelock_p.h"
#include "../common.h"
#include <QFile>
#include <QIODevice>
#include <QLocalSocket>
#include <QtTest/QtTest>

QTM_USE_NAMESPACE

void write(QIODevice * device, const char * str)
{
    device->write(str);
    device->waitForBytesWritten(30000);
}

void readLine(QIODevice * device)
{
    if (device->waitForReadyRead(30000))
        device->readLine();
}

int main(int argc, char * argv[])
{
    QCoreApplication app(argc, argv);

    QStringList args;
    for (int i = 1; i < argc; ++i)
        args << QString(argv[i]);

    if (args.count() == 0 )
        qFatal("No connection name supplied to lackey process");

    // This is a workaround for the fact that we cannot use stdin/stdout/stderr
    // to communicate with a process on WinCE or Symbian systems.
    // We use sockets instead, and the first argument given is the port number.

    QString connectionName = args.takeFirst();
    QLocalSocket oopSocket;

    if (connectionName != QString("NoComms")) {
        oopSocket.connectToServer(connectionName);
        oopSocket.waitForConnected(-1);
    }

    if (args.count() == 0 )
        qFatal("No arguments supplied to lackey process");

    int retVal = 0;

    if (args[0] == "ReadLock") {
        // 1)read-lock
        // 2)unlock
        QSystemReadWriteLock testRwLock("Viper");
        write(&oopSocket, qPrintable(Lackey::BeforeLockForRead));
        testRwLock.lockForRead();
        write(&oopSocket, qPrintable(Lackey::AfterLockForRead));

        testRwLock.unlock();
        QTest::qSleep(1000);
        write(&oopSocket, qPrintable(Lackey::AfterUnlockForRead));
    } else if (args[0] == "WriteLock") {
        // 1) write-lock
        // 2) unlock
        QSystemReadWriteLock testRwLock("Viper");
        write(&oopSocket, qPrintable(Lackey::BeforeLockForWrite));
        testRwLock.lockForWrite();
        write(&oopSocket, qPrintable(Lackey::AfterLockForWrite));

        testRwLock.unlock();
        QTest::qSleep(1000);
        write(&oopSocket, qPrintable(Lackey::AfterUnlockForWrite));
    } else if (args[0] == "ReadLockReleaseable") {
        // 1) read-lock
        // 2) wait for input on stdin
        // 3) unlock
        QSystemReadWriteLock testRwLock("Viper");
        write(&oopSocket, qPrintable(Lackey::BeforeLockForRead));
        testRwLock.lockForRead();
        QTest::qSleep(1000);
        write(&oopSocket, qPrintable(Lackey::AfterLockForRead));

        readLine(&oopSocket);

        testRwLock.unlock();
        write(&oopSocket, qPrintable(Lackey::AfterUnlockForRead));
    } else if (args[0] == "WriteLockReleaseable") {
        // 1) write-lock
        // 2) wait for input on stdin
        // 3) unlock
        QSystemReadWriteLock testRwLock("Viper");
        write(&oopSocket, qPrintable(Lackey::BeforeLockForWrite));
        testRwLock.lockForWrite();
        write(&oopSocket, qPrintable(Lackey::AfterLockForWrite));

        readLine(&oopSocket);

        testRwLock.unlock();
        write(&oopSocket, qPrintable(Lackey::AfterUnlockForWrite));
    } else if (args[0] == "ReadLockLoop") {
        // for(runTime msecs):
        // 1) read-lock
        // 2) sleep(holdTime msecs)
        // 3) unlock
        // 4) sleep(waitTime msecs)
        Q_ASSERT(args.count() == 4);
        int runTime = args[1].toInt();
        int holdTime = args[2].toInt();
        int waitTime = args[3].toInt();

        QSystemReadWriteLock testRwLock("Viper");
        QTime t;
        t.start();
        while(t.elapsed() < runTime) {
            testRwLock.lockForRead();
            if (holdTime)
                QTest::qSleep(holdTime);
            testRwLock.unlock();
            if(waitTime)
                QTest::qSleep(waitTime);
        }
    } else if (args[0]  == "WriteLockLoop") {
        // for(runTime msecs):
        // 1) read-lock
        // 2) sleep(holdTime msecs)
        // 3) unlock
        // 4) sleep(waitTime msecs)
        Q_ASSERT(args.count() == 4);
        int runTime = args[1].toInt();
        int holdTime = args[2].toInt();
        int waitTime = args[3].toInt();

        QSystemReadWriteLock testRwLock("Viper");
        QTime t;
        t.start();
        while (t.elapsed() < runTime) {
            testRwLock.lockForWrite();
            if (holdTime)
                QTest::qSleep(holdTime);
            testRwLock.unlock();
            if (waitTime)
                QTest::qSleep(waitTime);
        }
    } else if (args[0] == "ReadLockExcl") {
        // for (runTime msecs):
        // 1) read-lock
        // 2) check that the exclusive file does not
        //    exist, if it does qFatal
        // 3) sleep(holdTime msecs)
        // 4) unlock
        // 5) sleep(waitTime msecs)
        Q_ASSERT(args.count() == 4);
        int runTime = args[1].toInt();
        int holdTime = args[2].toInt();
        int waitTime = args[3].toInt();

        QSystemReadWriteLock testRwLock("Viper");
        QTime t;
        t.start();
        while (t.elapsed() < runTime) {
            testRwLock.lockForRead();
            QDir cwd;
            if (cwd.exists("writeLockExcl.tmp"))
                qFatal("writeLockExcl.tmp file found during read!");
            if (holdTime)
                QTest::qSleep(holdTime);
            testRwLock.unlock();
            if (waitTime)
                QTest::qSleep(waitTime);
        }
    } else if (args[0] == "WriteLockExcl") {
        // for(runTime msecs)
        // 1) write-lock
        // 2) check that exclusive file does not
        //      exist, if it does qFatal
        // 3) create the exclusive file,
        //    sleep(holdTime msecs), delete the file
        // 4) unlock
        // 5) sleep(waitTime msecs)
        Q_ASSERT(args.count() == 4);
        int runTime = args[1].toInt();
        int holdTime = args[2].toInt();
        int waitTime = args[3].toInt();

        QSystemReadWriteLock testRwLock("Viper");
        QTime t;
        t.start();
        while(t.elapsed() < runTime) {
            testRwLock.lockForWrite();
            QDir cwd;
            if (cwd.exists("writeLockExcl.tmp"))
                qFatal("writeLockExcl.tmp file found during write!");
            QFile file("writeLockExcl.tmp");
            file.open(QIODevice::ReadWrite);
            if (holdTime)
                QTest::qSleep(holdTime);
            file.close();
            cwd.remove("writeLockExcl.tmp");
            testRwLock.unlock();
            if(waitTime)
                QTest::qSleep(waitTime);
        }
    } else {
        retVal = 1;
    }

    if (connectionName != QString("NoComms")) {
        oopSocket.disconnectFromServer();
        oopSocket.waitForDisconnected(-1);
    }

    return retVal;
}
