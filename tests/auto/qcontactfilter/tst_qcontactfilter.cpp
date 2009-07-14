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
    void intersectionFilter();
    void unionFilter();
    void detailFilter();
    void detailRangeFilter();
    void changeLogFilter();
    void actionFilter();
    void groupMembershipFilter();

    void sortObject(); // should perhaps be in a different test :)
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

/* The bad filter doesn't set the d_ptr, since it can't */
class BadFilter : public QContactFilter
{
public:
    BadFilter() {}
};

void tst_QContactFilter::classHierarchy()
{
    /* Test "casting" up and down the hierarchy */
    QContactDetailRangeFilter drf;
    QVERIFY(drf.type() == QContactFilter::ContactDetailRange);
    drf.setDetailDefinitionName("Frog", "Croak");
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

    /* Try creating a bad filter and making sure we don't break */
    BadFilter bad, bad2;

    QVERIFY(bad.type() == QContactFilter::Invalid);
    QVERIFY(bad != bad2);
    QVERIFY(bad != drf2);
    QVERIFY(drf2 != bad);

    QContactFilter fbad = bad;
    QVERIFY(fbad.type() == QContactFilter::Invalid);
    QVERIFY(fbad != bad);
    QVERIFY(fbad != bad2);

    /* Now test some "cross casting" */

}

void tst_QContactFilter::intersectionFilter()
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

    /* Save this list */
    QList<QContactFilter> filterList = bf2.filters();

    f2 = df && df2 && df3;
    QVERIFY(f2.type() == QContactFilter::Intersection);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df);
    QVERIFY(bf2.filters().at(1) == df2);
    QVERIFY(bf2.filters().at(2) == df3);

    /* Self assignment should do nothing */
    bf2 = bf2;
    QVERIFY(bf2 == f2);

    /* Test set filter */
    bf2.setFilters(filterList);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df);
    QVERIFY(bf2.filters().at(2) == df2);

    /* Test remove */
    bf2.remove(df);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df2);

    /* Double remove, should do nothing */
    bf2.remove(df);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df2);
}

void tst_QContactFilter::unionFilter()
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

    /* Save this list */
    QList<QContactFilter> filterList = bf2.filters();

    f2 = df || df2 || df3;
    QVERIFY(f2.type() == QContactFilter::Union);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df);
    QVERIFY(bf2.filters().at(1) == df2);
    QVERIFY(bf2.filters().at(2) == df3);

    /* Self assignment should do nothing */
    bf2 = bf2;
    QVERIFY(bf2 == f2);

    /* Test set filter */
    bf2.setFilters(filterList);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df);
    QVERIFY(bf2.filters().at(2) == df2);

    /* Test remove */
    bf2.remove(df);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df2);

    /* Double remove, should do nothing */
    bf2.remove(df);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df2);
}

void tst_QContactFilter::actionFilter()
{
    QContactActionFilter af;

    /* Test initial conditions */
    QVERIFY(af.type() == QContactFilter::Action);
    QVERIFY(af.actionName().isEmpty());
    QVERIFY(af.value().isNull());


    af.setActionName("Action Name");
    QVERIFY(af.actionName() == "Action Name");

    af.setActionName(QString());
    QVERIFY(af.actionName().isEmpty());

    af.setValue(5);
    QVERIFY(af.value() == 5);

    af.setValue("This is a string");
    QVERIFY(af.value() == "This is a string");

    /* Test op= */
    QContactFilter f = af;
    QVERIFY(f == af);

    QContactActionFilter af2 = f;
    QVERIFY(af2 == af);

    /* Self assignment should do nothing */
    af2 = af2;
    QVERIFY(af2 == af);
}

void tst_QContactFilter::changeLogFilter()
{
    QContactChangeLogFilter cf;
    QContactChangeLogFilter cfadded(QContactChangeLogFilter::Added);
    QContactChangeLogFilter cfchanged(QContactChangeLogFilter::Changed);
    QContactChangeLogFilter cfremoved(QContactChangeLogFilter::Removed);

    QVERIFY(cf.type() == QContactFilter::ChangeLog);
    QVERIFY(cf.changeType() == QContactChangeLogFilter::Added);

    QVERIFY(cfadded.type() == QContactFilter::ChangeLog);
    QVERIFY(cfadded.changeType() == QContactChangeLogFilter::Added);

    QVERIFY(cfchanged.type() == QContactFilter::ChangeLog);
    QVERIFY(cfchanged.changeType() == QContactChangeLogFilter::Changed);

    QVERIFY(cfremoved.type() == QContactFilter::ChangeLog);
    QVERIFY(cfremoved.changeType() == QContactChangeLogFilter::Removed);


    /* Just to break the naming scheme */
    cfchanged.setChangeType(QContactChangeLogFilter::Added);
    QVERIFY(cfchanged.changeType() == QContactChangeLogFilter::Added);

    QVERIFY(cf.since() == QDateTime());

    QDateTime now = QDateTime::currentDateTime();
    cf.setSince(now);

    QVERIFY(cf.since() == now);

    cf.setSince(QDateTime());
    QVERIFY(cf.since() == QDateTime());

    /* Test op= */
    QContactFilter f = cf;
    QVERIFY(f == cf);

    QContactChangeLogFilter cf2 = f;
    QVERIFY(cf2 == cf);

    /* Self assignment should do nothing */
    cf2 = cf2;
    QVERIFY(cf2 == cf);
}

void tst_QContactFilter::detailFilter()
{
    QContactDetailFilter df;

    QVERIFY(df.type() == QContactFilter::ContactDetail);

    QVERIFY(df.detailDefinitionName().isEmpty());
    QVERIFY(df.detailFieldName().isEmpty());
    QVERIFY(df.matchFlags() == 0);
    QVERIFY(df.value().isNull());

    df.setDetailDefinitionName("Definition");
    QVERIFY(df.detailDefinitionName() == "Definition");
    QVERIFY(df.detailFieldName().isEmpty());
    QVERIFY(df.matchFlags() == 0);
    QVERIFY(df.value().isNull());

    df.setDetailDefinitionName("Definition", "Field");
    QVERIFY(df.detailDefinitionName() == "Definition");
    QVERIFY(df.detailFieldName() == "Field");
    QVERIFY(df.matchFlags() == 0);
    QVERIFY(df.value().isNull());

    df.setMatchFlags(Qt::MatchExactly);
    QVERIFY(df.matchFlags() == Qt::MatchExactly);

    df.setValue(5);
    QVERIFY(df.value() == 5);

    df.setValue("String value");
    QVERIFY(df.value() == "String value");

    /* Test op= */
    QContactFilter f = df;
    QVERIFY(f == df);

    QContactDetailFilter df2 = f;
    QVERIFY(df2 == df);

    /* Self assignment should do nothing */
    df2 = df2;
    QVERIFY(df2 == df);
}

void tst_QContactFilter::detailRangeFilter()
{
    QContactDetailRangeFilter rf;

    QVERIFY(rf.type() == QContactFilter::ContactDetailRange);

    QVERIFY(rf.detailDefinitionName().isEmpty());
    QVERIFY(rf.detailFieldName().isEmpty());
    QVERIFY(rf.matchFlags() == 0);

    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setDetailDefinitionName("Definition");
    QVERIFY(rf.detailDefinitionName() == "Definition");
    QVERIFY(rf.detailFieldName().isEmpty());
    QVERIFY(rf.matchFlags() == 0);

    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setDetailDefinitionName("Definition", "Field");
    QVERIFY(rf.detailDefinitionName() == "Definition");
    QVERIFY(rf.detailFieldName() == "Field");
    QVERIFY(rf.matchFlags() == 0);

    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setMatchFlags(Qt::MatchExactly);
    QVERIFY(rf.matchFlags() == Qt::MatchExactly);

    rf.setRange(5, 10);
    QVERIFY(rf.minValue() == 5);
    QVERIFY(rf.maxValue() == 10);
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setRange(QVariant(), 11);
    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue() == 11);
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setRange(6, QVariant());
    QVERIFY(rf.minValue() == 6);
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setRange(QVariant(), QVariant());
    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setRange(5, 10, QContactDetailRangeFilter::ExcludeLower);
    QVERIFY(rf.minValue() == 5);
    QVERIFY(rf.maxValue() == 10);
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::ExcludeLower));

    rf.setRange(QVariant(), 11, QContactDetailRangeFilter::IncludeUpper);
    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue() == 11);
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::IncludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setRange(6, QVariant(), QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper);
    QVERIFY(rf.minValue() == 6);
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::IncludeUpper | QContactDetailRangeFilter::ExcludeLower));

    rf.setRange(QVariant(), QVariant(), QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower);
    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    /* Test op= */
    QContactFilter f = rf;
    QVERIFY(f == rf);

    QContactDetailRangeFilter rf2 = f;
    QVERIFY(rf2 == rf);

    /* Self assignment should do nothing */
    rf2 = rf2;
    QVERIFY(rf2 == rf);
}

void tst_QContactFilter::groupMembershipFilter()
{
    QContactGroupMembershipFilter gf;

    QVERIFY(gf.type() == QContactFilter::GroupMembership);

    QVERIFY(gf.groupId() == 0);

    gf.setGroupId(546);
    QVERIFY(gf.groupId() == 546);

    /* Test op= */
    QContactFilter f = gf;
    QVERIFY(f == gf);

    QContactGroupMembershipFilter gf2 = f;
    QVERIFY(gf2 == gf);

    /* Self assignment should do nothing */
    gf2 = gf2;
    QVERIFY(gf2 == gf);
}

void tst_QContactFilter::sortObject()
{
    QContactSortOrder sortorder;

    /* Defaults */
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(!sortorder.isValid());

    /* Blank Policy */
    sortorder.setBlankPolicy(QContactSortOrder::BlanksFirst);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksFirst);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(!sortorder.isValid());

    sortorder.setBlankPolicy(QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(!sortorder.isValid());

    /* Direction */
    sortorder.setDirection(Qt::DescendingOrder);
    QVERIFY(sortorder.direction() == Qt::DescendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDirection(Qt::AscendingOrder);
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    /* Definitions */
    sortorder.setDetailDefinitionName(QString(), QString());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDetailDefinitionName("", QString());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDetailDefinitionName(QString(), "");
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDetailDefinitionName("", "");
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDetailDefinitionName("Definition", QString());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDetailDefinitionName("Definition", "Detail");
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName() == "Definition");
    QVERIFY(sortorder.detailFieldName() == "Detail");
    QVERIFY(sortorder.isValid());

    sortorder.setDetailDefinitionName(QString(), "Detail");
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    /* Copy ctor */
    sortorder.setDetailDefinitionName("Definition", "Detail");
    sortorder.setBlankPolicy(QContactSortOrder::BlanksFirst);
    sortorder.setDirection(Qt::DescendingOrder);
    QVERIFY(sortorder.direction() == Qt::DescendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksFirst);
    QVERIFY(sortorder.detailDefinitionName() == "Definition");
    QVERIFY(sortorder.detailFieldName() == "Detail");
    QVERIFY(sortorder.isValid());

    QContactSortOrder other(sortorder);
    QVERIFY(other.direction() == Qt::DescendingOrder);
    QVERIFY(other.blankPolicy() == QContactSortOrder::BlanksFirst);
    QVERIFY(other.detailDefinitionName() == "Definition");
    QVERIFY(other.detailFieldName() == "Detail");
    QVERIFY(other.isValid());

    /* Assignment operator */
    QContactSortOrder another;
    another = other;
    QVERIFY(another.direction() == Qt::DescendingOrder);
    QVERIFY(another.blankPolicy() == QContactSortOrder::BlanksFirst);
    QVERIFY(another.detailDefinitionName() == "Definition");
    QVERIFY(another.detailFieldName() == "Detail");
    QVERIFY(another.isValid());

    /* Self assignment */
    another = another;
    QVERIFY(another.direction() == Qt::DescendingOrder);
    QVERIFY(another.blankPolicy() == QContactSortOrder::BlanksFirst);
    QVERIFY(another.detailDefinitionName() == "Definition");
    QVERIFY(another.detailFieldName() == "Detail");
    QVERIFY(another.isValid());
}

QTEST_MAIN(tst_QContactFilter)
#include "tst_qcontactfilter.moc"
