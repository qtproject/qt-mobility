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

#include <qmallocpool_p.h>

#include <QtTest/QTest>

#include <QDebug>

QTM_USE_NAMESPACE
class tst_QMallocPool : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testConstructor();
    void testShared();
};

void tst_QMallocPool::initTestCase()
{
}

void tst_QMallocPool::cleanupTestCase()
{
}

void tst_QMallocPool::testConstructor()
{
    {
        QMallocPool invalidPool;
        QVERIFY(!invalidPool.isValid());
    }

    {
        QMallocPool invalidPool(0, 0, QMallocPool::NewShared);
        QVERIFY(!invalidPool.isValid());
    }

    {
        QMallocPool invalidPool(0, 0, QMallocPool::Shared);
        QVERIFY(!invalidPool.isValid());
    }

    {
        const unsigned int poolLength = 1024 * 1024;
        char *poolBase = new char[poolLength];
        for (unsigned int i = 0; i < poolLength; ++i)
            poolBase[i] = 'X';

        QMallocPool pool(poolBase, poolLength, QMallocPool::Owned);

        QVERIFY(pool.isValid());

        void *alloc;

        // test malloc
        alloc = pool.malloc(1024);
        QVERIFY(alloc != 0);
        QVERIFY(pool.size_of(alloc) >= 1024);
        QVERIFY(pool.memoryStatistics().inuseBytes >= 1024);
        pool.free(alloc);
        QVERIFY(pool.memoryStatistics().inuseBytes == 0);

        // test malloc of poolLength, should fail.
        alloc = pool.malloc(poolLength);
        QVERIFY(alloc == 0);
        QVERIFY(pool.memoryStatistics().inuseBytes == 0);

        // test malloc of poolLength + 1, should fail.
        alloc = pool.malloc(poolLength + 1);
        QVERIFY(alloc == 0);
        QVERIFY(pool.memoryStatistics().inuseBytes == 0);

        // test calloc
        alloc = pool.calloc(1024, sizeof(char));
        QVERIFY(alloc != 0);
        QVERIFY(pool.size_of(alloc) >= 1024);
        QVERIFY(pool.memoryStatistics().inuseBytes >= 1024);
        for (int i = 0; i < 1024; ++i)
            QVERIFY(reinterpret_cast<char *>(alloc)[i] == '\0');
        pool.free(alloc);
        QVERIFY(pool.memoryStatistics().inuseBytes == 0);

        // test realloc
        alloc = pool.malloc(1024);
        QVERIFY(alloc != 0);
        QVERIFY(pool.size_of(alloc) >= 1024);
        QVERIFY(pool.memoryStatistics().inuseBytes >= 1024);
        alloc = pool.realloc(alloc, 2048);
        QVERIFY(alloc != 0);
        QVERIFY(pool.size_of(alloc) >= 2048);
        QVERIFY(pool.memoryStatistics().inuseBytes >= 2048);
        pool.free(alloc);
        QVERIFY(pool.memoryStatistics().inuseBytes == 0);

        delete[] poolBase;
    }
}

void tst_QMallocPool::testShared()
{
    const unsigned int poolLength = 1024 * 1024;
    char *poolBase = new char[poolLength];
    for (unsigned int i = 0; i < poolLength; ++i)
        poolBase[i] = 'X';

    QMallocPool pool(poolBase, poolLength, QMallocPool::NewShared);
    QMallocPool pool2(poolBase, poolLength, QMallocPool::Shared);

    QVERIFY(pool.isValid());
    QVERIFY(pool2.isValid());

    void *alloc;

    // test malloc
    alloc = pool.malloc(1024);
    QVERIFY(alloc != 0);
    QVERIFY(pool.size_of(alloc) >= 1024);
    QVERIFY(pool.size_of(alloc) == pool2.size_of(alloc));
    pool.free(alloc);

    // test malloc of poolLength, should fail.
    alloc = pool.malloc(poolLength);
    QVERIFY(alloc == 0);

    // test malloc of poolLength + 1, should fail.
    alloc = pool.malloc(poolLength + 1);
    QVERIFY(alloc == 0);

    // test calloc
    alloc = pool.calloc(1024, sizeof(char));
    QVERIFY(alloc != 0);
    QVERIFY(pool.size_of(alloc) >= 1024);
    QVERIFY(pool.size_of(alloc) == pool2.size_of(alloc));
    for (int i = 0; i < 1024; ++i)
        QVERIFY(reinterpret_cast<char *>(alloc)[i] == '\0');
    pool.free(alloc);

    // test realloc
    alloc = pool.malloc(1024);
    QVERIFY(alloc != 0);
    QVERIFY(pool.size_of(alloc) >= 1024);
    QVERIFY(pool.size_of(alloc) == pool2.size_of(alloc));
    alloc = pool.realloc(alloc, 2048);
    QVERIFY(alloc != 0);
    QVERIFY(pool.size_of(alloc) >= 2048);
    QVERIFY(pool.size_of(alloc) == pool2.size_of(alloc));
    pool.free(alloc);

    delete[] poolBase;
}

QTEST_MAIN(tst_QMallocPool)
#include "tst_qmallocpool.moc"
