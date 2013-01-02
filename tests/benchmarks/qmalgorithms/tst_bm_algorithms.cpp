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
