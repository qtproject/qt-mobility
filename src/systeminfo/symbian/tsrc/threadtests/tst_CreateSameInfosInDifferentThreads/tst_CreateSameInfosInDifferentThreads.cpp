/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
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
#include <qsysteminfo.h>

QTM_USE_NAMESPACE

//#define SHOWDEBUGS

QSemaphore MainThreadRunningSemaphore;
QSemaphore OtherThreadRunningSemaphore;

class Thread : public QThread
{
    Q_OBJECT
public:
    void run() {
#ifdef SHOWDEBUGS
        qDebug()<<"Thread::run - create same object also in thread: "<<thread()->currentThreadId();
#endif//SHOWDEBUGS
        QSystemNetworkInfo networkInfo;
#ifdef SHOWDEBUGS
        qDebug()<<"Thread::run - read something directly from object in this thread: "
                <<networkInfo.currentMode();
#endif//SHOWDEBUGS
        MainThreadRunningSemaphore.release();
    }
};

class tst_QSystemInfo_CreateSameInfosInDifferentThreads : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testCase() {
#ifdef SHOWDEBUGS
        qDebug()<<"tst_QSystemInfo_CreateSameInfosInDifferentThreads::testCase - create objects - running in thread: "<<thread()->currentThreadId();
#endif//SHOWDEBUGS
        QSystemNetworkInfo networkInfo;
        Thread testThread;
        testThread.start();
        MainThreadRunningSemaphore.acquire();
#ifdef SHOWDEBUGS
        qDebug()<<"tst_QSystemInfo_CreateSameInfosInDifferentThreads::testCase - read something directly from object in this thread: "
                <<networkInfo.currentMode();
#endif//SHOWDEBUGS
        OtherThreadRunningSemaphore.release();
        testThread.wait();
    }
};

QTEST_MAIN(tst_QSystemInfo_CreateSameInfosInDifferentThreads);

#include "tst_CreateSameInfosInDifferentThreads.moc"
