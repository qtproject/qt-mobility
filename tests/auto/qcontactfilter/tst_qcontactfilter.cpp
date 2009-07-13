/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMobility module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtTest/QtTest>

#include "qtcontacts.h"

//TESTED_CLASS=
//TESTED_FILES=

class tst_QContactFilter : public QObject
{
Q_OBJECT

public:
    tst_QContactFilter();
    virtual ~tst_QContactFilter();


public slots:
    void init();
    void cleanup();
private slots:
    void classHierarchy();
    void intersection();
    void unionfilter();
};

tst_QContactFilter::tst_QContactFilter()
{
}

tst_QContactFilter::~tst_QContactFilter()
{
}

void tst_QContactFilter::init()
{
}

void tst_QContactFilter::cleanup()
{
}

void tst_QContactFilter::classHierarchy()
{
    qDebug() << 1;
    QContactDetailRangeFilter drf;
    QVERIFY(drf.type() == QContactFilter::ContactDetailRange);
    drf.setDetailDefinitionName("Frog");
    drf.setDetailFieldName("Croak");
    drf.setRange(1, 20);

    QContactFilter f = drf;
    QVERIFY(f.type() == QContactFilter::ContactDetailRange);

    QContactDetailRangeFilter drf2 = f;
    QVERIFY(drf2.type() == QContactFilter::ContactDetailRange);
    QVERIFY(drf2.detailDefinitionName() == "Frog");
    QVERIFY(drf2.detailFieldName() == "Croak");
    QVERIFY(drf2.maxValue() == 20);
    QVERIFY(drf2.minValue() == 1);

    /* Now try to check if we dangle pointers at all */
    {
        QContactFilter f2 = drf2;
    }
    QVERIFY(drf2.type() == QContactFilter::ContactDetailRange);
    QVERIFY(drf2.detailDefinitionName() == "Frog");
    QVERIFY(drf2.detailFieldName() == "Croak");
    QVERIFY(drf2.maxValue() == 20);
    QVERIFY(drf2.minValue() == 1);

    {
        QContactDetailRangeFilter rf2 = drf2;
        rf2.setDetailDefinitionName("Toad");
        QVERIFY(rf2.detailDefinitionName() == "Toad");
        QVERIFY(drf2.detailDefinitionName() == "Frog");
    }
    QVERIFY(drf2.type() == QContactFilter::ContactDetailRange);
    QVERIFY(drf2.detailDefinitionName() == "Frog");
    QVERIFY(drf2.detailFieldName() == "Croak");
    QVERIFY(drf2.maxValue() == 20);
    QVERIFY(drf2.minValue() == 1);
}

void tst_QContactFilter::intersection()
{
    /* Test boolean ops */
    QContactDetailFilter df;
    df.setDetailDefinitionName("Frog");

    QContactDetailFilter df2;
    df2.setDetailDefinitionName("Toad");

    QContactDetailFilter df3;
    df3.setDetailDefinitionName("Hippopotamus");

    QContactIntersectionFilter bf;
    bf << df << df2;

    QContactFilter f = df && df2;

    QVERIFY(bf == f);

    QContactFilter f2 = bf && df3;
    QVERIFY(f2.type() == QContactFilter::Intersection);
    QContactIntersectionFilter bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df);
    QVERIFY(bf2.filters().at(1) == df2);
    QVERIFY(bf2.filters().at(2) == df3);

    f2 = df3 && bf;
    QVERIFY(f2.type() == QContactFilter::Intersection);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df);
    QVERIFY(bf2.filters().at(2) == df2);

    f2 = df && df2 && df3;
    QVERIFY(f2.type() == QContactFilter::Intersection);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df);
    QVERIFY(bf2.filters().at(1) == df2);
    QVERIFY(bf2.filters().at(2) == df3);
}

void tst_QContactFilter::unionfilter()
{
    /* Test boolean ops */
    QContactDetailFilter df;
    df.setDetailDefinitionName("Frog");

    QContactDetailFilter df2;
    df2.setDetailDefinitionName("Toad");

    QContactDetailFilter df3;
    df3.setDetailDefinitionName("Hippopotamus");

    QContactUnionFilter bf;
    bf << df << df2;

    QContactFilter f = df || df2;

    QVERIFY(bf == f);

    QContactFilter f2 = bf || df3;
    QVERIFY(f2.type() == QContactFilter::Union);
    QContactUnionFilter bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df);
    QVERIFY(bf2.filters().at(1) == df2);
    QVERIFY(bf2.filters().at(2) == df3);

    f2 = df3 || bf;
    QVERIFY(f2.type() == QContactFilter::Union);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df);
    QVERIFY(bf2.filters().at(2) == df2);

    f2 = df || df2 || df3;
    QVERIFY(f2.type() == QContactFilter::Union);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df);
    QVERIFY(bf2.filters().at(1) == df2);
    QVERIFY(bf2.filters().at(2) == df3);
}

QTEST_MAIN(tst_QContactFilter)
#include "tst_qcontactfilter.moc"
