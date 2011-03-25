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


#include <QtTest/QtTest>
#include <QList>
#include "qmalgorithms.h"

Q_DECLARE_METATYPE(QList<int>)

QTM_USE_NAMESPACE

class tst_BenchmarkAlgorithms : public QObject
{
    Q_OBJECT

private slots:
    void benchmarking_data();
    void stableSort();
    void stableSort_data() { benchmarking_data(); }
    void sort();
    void sort_data() { benchmarking_data(); }
    void partialSort();
    void partialSort_data() { benchmarking_data(); }
};

void tst_BenchmarkAlgorithms::sort()
{
    QFETCH(QList<int>, list);
    QFETCH(int, from);
    QFETCH(int, to);

    QBENCHMARK {
        QList<int> sortable = list;
        qSort(sortable);
        QList<int> extracted = sortable.mid(from, to-from);
    }
}

void tst_BenchmarkAlgorithms::stableSort()
{
    QFETCH(QList<int>, list);
    QFETCH(int, from);
    QFETCH(int, to);

    QBENCHMARK {
        QList<int> sortable = list;
        qStableSort(sortable);
        QList<int> extracted = sortable.mid(from, to-from);
    }
}

void tst_BenchmarkAlgorithms::partialSort()
{
    QFETCH(QList<int>, list);
    QFETCH(int, from);
    QFETCH(int, to);

    QBENCHMARK {
        QList<int> sortable = list;
        qPartialSort(sortable, from, to);
        QList<int> extracted = sortable.mid(from, to-from);
    }
}

void tst_BenchmarkAlgorithms::benchmarking_data()
{
    QTest::addColumn<QList<int> >("list");
    QTest::addColumn<int>("from");
    QTest::addColumn<int>("to");

    QList<int> list;
    qsrand(42);
    for (int i = 0; i < 100; i ++) {
        list.append(qrand());
    }
    QTest::newRow("100 elements") << list << 0 << 10;
    QTest::newRow("100 elements") << list << 50 << 60;

    list.clear();
    qsrand(42);
    for (int i = 0; i < 1000; i ++) {
        list.append(qrand());
    }
    QTest::newRow("1000 elements") << list << 0 << 10;
    QTest::newRow("1000 elements") << list << 500 << 510;

    list.clear();
    qsrand(42);
    for (int i = 0; i < 10000; i ++) {
        list.append(qrand());
    }
    QTest::newRow("10000 elements") << list << 0 << 10;
    QTest::newRow("10000 elements") << list << 5000 << 5010;
}

QTEST_MAIN(tst_BenchmarkAlgorithms)
#include "tst_bm_algorithms.moc"
