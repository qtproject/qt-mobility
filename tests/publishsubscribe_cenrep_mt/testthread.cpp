/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include "testthread.h"

#include <QValueSpaceSubscriber>
#include <QValueSpacePublisher>
#include <QMutex>
#include <QSemaphore>

ConfigurabilityTestThread::ConfigurabilityTestThread(QSemaphore *sem, QMutex *mutex, int testcase)
    : mSemaphore(sem), mMutex(mutex), error(false), mTestCase(testcase)
{
}

void ConfigurabilityTestThread::setPath(QString path, QString key)
{
    mPath = path;
    mKey = key;
}

void ConfigurabilityTestThread::run()
{
    switch (mTestCase) {
    case 1:
        case1();
        break;
    case 2:
        case2();
        break;
    default:
        break;
    }
    mSemaphore->release(1);
}

// several threads access same cenrep key. since the write/read operations are protected
// by a mutex, this doesn't really test simultaneous access.
void ConfigurabilityTestThread::case1()
{
    for (int i=0; i<100; i++) {
        QtMobility::QValueSpaceSubscriber subscriber("/publishsubscribe_mt/cenrep");
        QtMobility::QValueSpacePublisher publisher("/publishsubscribe_mt/cenrep");

        mMutex->lock();
        publisher.setValue("int", i);
        publisher.sync();
        int val = subscriber.value("int").toInt();
        mMutex->unlock();

        if (val != i) {
            error = true;
            errorString = QString("subscriber.value = ") + val + QString(", i = ") + i;
            break;
        }
    }
}

// same as before, but without mutex. each thread should have a different key it is
// accessing (setPath needs to be called before starting the thread).
void ConfigurabilityTestThread::case2()
{
    for (int i=0; i<100; i++) {
        QtMobility::QValueSpaceSubscriber subscriber(mPath);
        QtMobility::QValueSpacePublisher publisher(mPath);

        publisher.setValue(mKey, i);
        publisher.sync();
        int val = subscriber.value(mKey).toInt();

        if (val != i) {
            error = true;
            errorString = QString("subscriber.value = ") + QString::number(val)
                          + QString(", i = ") + QString::number(i)
                          + QString(", mPath = ") + mPath + QString(", mKey = ") + mKey;
            break;
        }
    }
}
