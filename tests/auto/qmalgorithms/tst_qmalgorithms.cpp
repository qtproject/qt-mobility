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
#include <QDebug>

#include "qmalgorithms.h"

QTM_USE_NAMESPACE

class tst_QMAlgorithms : public QObject
{
Q_OBJECT

public:
    tst_QMAlgorithms() {}
    ~tst_QMAlgorithms() {}
private slots:
    void initTestCase();
    void partialSortContainer();
    void partialSortIterator();
    void partialSortWithLessThan();
    void partialSortWithGreaterThan();
private:
    QList<int> originalList;
    QList<int> sorted;
};

void tst_QMAlgorithms::initTestCase() {
    originalList.clear();
    originalList << 21 << 38 << 19 << 36 << 12 << 3 << 0 << 14 << 16 << 6 << 31 << 24 << 25 << 5
        << 8 << 39 << 15 << 28 << 37 << 18 << 2 << 4 << 34 << 32 << 13 << 7 << 10 << 9 << 23 << 1
        << 30 << 17 << 22 << 20 << 11 << 29 << 33 << 27 << 26 << 35;
    sorted = originalList;
    qSort(sorted);
}

// Each of these tests are the same, just with different versions of partial sort

void tst_QMAlgorithms::partialSortContainer() {
    for (int i = 0; i < originalList.size(); i++) {
        for (int j = i; j < originalList.size(); j++) {
            QList<int> partiallySorted = originalList;
            qPartialSort(partiallySorted, i, j);
            for (int k = i; k < j; k++) {
                QCOMPARE(partiallySorted[k], sorted[k]);
            }
        }
    }
}

void tst_QMAlgorithms::partialSortIterator() {
    for (int i = 0; i < originalList.size(); i++) {
        for (int j = i; j < originalList.size(); j++) {
            QList<int> partiallySorted = originalList;
            qPartialSort(partiallySorted.begin(), partiallySorted.end(),
                    partiallySorted.begin() + i, partiallySorted.end() + j);
            for (int k = i; k < j; k++) {
                QCOMPARE(partiallySorted[k], sorted[k]);
            }
        }
    }
}

void tst_QMAlgorithms::partialSortWithLessThan() {
    for (int i = 0; i < originalList.size(); i++) {
        for (int j = i; j < originalList.size(); j++) {
            QList<int> partiallySorted = originalList;
            qPartialSort(partiallySorted.begin(), partiallySorted.end(),
                    partiallySorted.begin() + i, partiallySorted.end() + j, qLess<int>());
            for (int k = i; k < j; k++) {
                QCOMPARE(partiallySorted[k], sorted[k]);
            }
        }
    }
}

void tst_QMAlgorithms::partialSortWithGreaterThan() {
    qSort(sorted.begin(), sorted.end(), qGreater<int>());
    for (int i = 0; i < originalList.size(); i++) {
        for (int j = i; j < originalList.size(); j++) {
            QList<int> partiallySorted = originalList;
            qPartialSort(partiallySorted.begin(), partiallySorted.end(),
                    partiallySorted.begin()+i, partiallySorted.begin()+j, qGreater<int>());
            for (int k = i; k < j; k++) {
                QCOMPARE(partiallySorted[k], sorted[k]);
            }
        }
    }
}

QTEST_MAIN(tst_QMAlgorithms)
#include "tst_qmalgorithms.moc"
