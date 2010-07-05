/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtTest/QtTest>
#include <QMetaType>

#include "qtcontacts.h"

//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QContact)
Q_DECLARE_METATYPE(QContactFilter)

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
    void relationshipFilter();
    void boringFilters();
    void idListFilter();
    void canonicalizedFilter();
    void canonicalizedFilter_data();
    void testFilter();
    void testFilter_data();

    void traits();

    void sortObject(); // should perhaps be in a different test :)
    void sortTraits();
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
    /* Test "casting" up and down the hierarchy */
    QContactDetailRangeFilter drf;
    QVERIFY(drf.type() == QContactFilter::ContactDetailRangeFilter);
    drf.setDetailDefinitionName("Frog", "Croak");
    drf.setRange(1, 20);

    QContactFilter f = drf;
    QVERIFY(f.type() == QContactFilter::ContactDetailRangeFilter);

    QContactDetailRangeFilter drf2 = f;
    QVERIFY(drf2.type() == QContactFilter::ContactDetailRangeFilter);
    QVERIFY(drf2.detailDefinitionName() == "Frog");
    QVERIFY(drf2.detailFieldName() == "Croak");
    QVERIFY(drf2.maxValue() == 20);
    QVERIFY(drf2.minValue() == 1);

    /* Now try to check if we dangle pointers at all */
    {
        QContactFilter f2 = drf2;
    }
    QVERIFY(drf2.type() == QContactFilter::ContactDetailRangeFilter);
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
    QVERIFY(drf2.type() == QContactFilter::ContactDetailRangeFilter);
    QVERIFY(drf2.detailDefinitionName() == "Frog");
    QVERIFY(drf2.detailFieldName() == "Croak");
    QVERIFY(drf2.maxValue() == 20);
    QVERIFY(drf2.minValue() == 1);

    /* Try creating a default filter and making sure we don't break */
    QContactFilter deff, deff2;

    QVERIFY(deff.type() == QContactFilter::DefaultFilter);
    QVERIFY(deff == deff);
    QVERIFY(deff == deff2);
    QVERIFY(deff != drf2);
    QVERIFY(drf2 != deff);

    QContactFilter fdeff = deff;
    QVERIFY(fdeff.type() == QContactFilter::DefaultFilter);
    QVERIFY(fdeff == deff);
    QVERIFY(fdeff == deff2);

    /* Now some "invalid" filters */
    QContactInvalidFilter iff, iff2;

    QVERIFY(iff.type() == QContactFilter::InvalidFilter);
    QVERIFY(iff == iff);
    QVERIFY(iff == iff2);
    QVERIFY(iff != drf2);
    QVERIFY(drf2 != iff);

    QContactFilter fiff = iff;
    QVERIFY(fiff.type() == QContactFilter::InvalidFilter);
    QVERIFY(fiff == iff);
    QVERIFY(fiff == iff2);

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

    QContactFilter f = df & df2;

    QVERIFY(bf == f);

    QContactFilter f2 = bf & df3;
    QVERIFY(f2.type() == QContactFilter::IntersectionFilter);
    QContactIntersectionFilter bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == bf);
    QVERIFY(bf2.filters().at(1) == df3);

    f2 = df3 & bf;
    QVERIFY(f2.type() == QContactFilter::IntersectionFilter);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == bf);

    /* Save this list */
    QList<QContactFilter> filterList = bf2.filters();

    f2 = df & df2 & df3;
    QVERIFY(f2.type() == QContactFilter::IntersectionFilter);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == (df & df2));
    QVERIFY(bf2.filters().at(1) == df3);

    /* Self assignment should do nothing */
    bf2 = bf2;
    QVERIFY(bf2 == f2);

    /* Test set filter */
    bf2.setFilters(filterList);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == bf);

    /* Test remove */
    bf2.remove(bf);
    QCOMPARE(bf2.filters().count(), 1);
    QVERIFY(bf2.filters().at(0) == df3);

    /* Double remove, should do nothing */
    bf2.remove(bf);
    QCOMPARE(bf2.filters().count(), 1);
    QVERIFY(bf2.filters().at(0) == df3);

    /* Append/prepend */
    QContactIntersectionFilter bf3;
    bf3.append(df);
    QVERIFY(bf3.filters().count() == 1);
    bf3.prepend(df2);
    QVERIFY(bf3.filters().count() == 2);
    QVERIFY(bf3.filters().at(0) == df2);
    QVERIFY(bf3.filters().at(1) == df);
    bf3.append(df3);
    QVERIFY(bf3.filters().count() == 3);
    QVERIFY(bf3.filters().at(0) == df2);
    QVERIFY(bf3.filters().at(1) == df);
    QVERIFY(bf3.filters().at(2) == df3);
    bf3.prepend(df3);
    QVERIFY(bf3.filters().count() == 4);
    QVERIFY(bf3.filters().at(0) == df3);
    QVERIFY(bf3.filters().at(1) == df2);
    QVERIFY(bf3.filters().at(2) == df);
    QVERIFY(bf3.filters().at(3) == df3);
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

    QContactFilter f = df | df2;

    QVERIFY(bf == f);

    QContactFilter f2 = bf | df3;
    QVERIFY(f2.type() == QContactFilter::UnionFilter);
    QContactUnionFilter bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df);
    QVERIFY(bf2.filters().at(1) == df2);
    QVERIFY(bf2.filters().at(2) == df3);

    f2 = df3 | bf;
    QVERIFY(f2.type() == QContactFilter::UnionFilter);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df);
    QVERIFY(bf2.filters().at(2) == df2);

    /* Save this list */
    QList<QContactFilter> filterList = bf2.filters();

    f2 = df | df2 | df3;
    QVERIFY(f2.type() == QContactFilter::UnionFilter);
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

    /* Append/prepend */
    QContactUnionFilter bf3;
    bf3.append(df);
    QVERIFY(bf3.filters().count() == 1);
    bf3.prepend(df2);
    QVERIFY(bf3.filters().count() == 2);
    QVERIFY(bf3.filters().at(0) == df2);
    QVERIFY(bf3.filters().at(1) == df);
    bf3.append(df3);
    QVERIFY(bf3.filters().count() == 3);
    QVERIFY(bf3.filters().at(0) == df2);
    QVERIFY(bf3.filters().at(1) == df);
    QVERIFY(bf3.filters().at(2) == df3);
    bf3.prepend(df3);
    QVERIFY(bf3.filters().count() == 4);
    QVERIFY(bf3.filters().at(0) == df3);
    QVERIFY(bf3.filters().at(1) == df2);
    QVERIFY(bf3.filters().at(2) == df);
    QVERIFY(bf3.filters().at(3) == df3);
}

void tst_QContactFilter::changeLogFilter()
{
    QContactChangeLogFilter cf;
    QContactChangeLogFilter cfadded(QContactChangeLogFilter::EventAdded);
    QContactChangeLogFilter cfchanged(QContactChangeLogFilter::EventChanged);
    QContactChangeLogFilter cfremoved(QContactChangeLogFilter::EventRemoved);

    QVERIFY(cf.type() == QContactFilter::ChangeLogFilter);
    QVERIFY(cf.eventType() == QContactChangeLogFilter::EventAdded);

    QVERIFY(cfadded.type() == QContactFilter::ChangeLogFilter);
    QVERIFY(cfadded.eventType() == QContactChangeLogFilter::EventAdded);

    QVERIFY(cfchanged.type() == QContactFilter::ChangeLogFilter);
    QVERIFY(cfchanged.eventType() == QContactChangeLogFilter::EventChanged);

    QVERIFY(cfremoved.type() == QContactFilter::ChangeLogFilter);
    QVERIFY(cfremoved.eventType() == QContactChangeLogFilter::EventRemoved);


    /* Just to break the naming scheme */
    cfchanged.setEventType(QContactChangeLogFilter::EventAdded);
    QVERIFY(cfchanged.eventType() == QContactChangeLogFilter::EventAdded);

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

    QContactDetailFilter dfil;
    QContactChangeLogFilter cf3(dfil);
    QVERIFY(cf3.type() == QContactFilter::ChangeLogFilter);
    QContactChangeLogFilter cf4(cf);
    QVERIFY(cf4 == cf);
    cf = dfil;
    QVERIFY(cf == cf3);
    cf = cf3;
    cf.setEventType(QContactChangeLogFilter::EventRemoved); // force a detach
}

void tst_QContactFilter::detailFilter()
{
    QContactDetailFilter df;

    QVERIFY(df.type() == QContactFilter::ContactDetailFilter);

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

    df.setMatchFlags(QContactFilter::MatchExactly);
    QVERIFY(df.matchFlags() == QContactFilter::MatchExactly);

    df.setValue(5);
    QVERIFY(df.value() == 5);

    df.setValue("String value");
    QVERIFY(df.value() == "String value");

    /* Test op= */
    QContactFilter f = df;
    QVERIFY(f == df);

    QContactDetailFilter df2 = f;
    QVERIFY(df2 == df);
    QVERIFY(df2.detailDefinitionName() == "Definition");
    QVERIFY(df2.detailFieldName() == "Field");

    /* Self assignment should do nothing */
    df2 = df2;
    QVERIFY(df2 == df);

    /* Some cross casting */
    QContactDetailRangeFilter rf;

    /* Directly */
    df2 = rf;
    QVERIFY(df2.type() == QContactFilter::ContactDetailFilter);
    QVERIFY(df2.detailDefinitionName().isEmpty());
    QVERIFY(df2.detailFieldName().isEmpty());
    QVERIFY(df2.value().isNull());

    /* reset it */
    df2 = df;
    QVERIFY(df2.detailDefinitionName() == "Definition");
    QVERIFY(df2.detailFieldName() == "Field");

    /* Through base class */
    f = rf;
    df2 = f;
    QVERIFY(df2.detailDefinitionName().isEmpty());
    QVERIFY(df2.detailFieldName().isEmpty());
    QVERIFY(df2.value().isNull());

    /* Now test copy ctor */
    QContactDetailFilter df3(rf);
    QVERIFY(df3.type() == QContactFilter::ContactDetailFilter);
    QVERIFY(df3.detailDefinitionName().isEmpty());
    QVERIFY(df3.detailFieldName().isEmpty());
    QVERIFY(df3.value().isNull());

    /* reset it */
    df3 = df;
    QVERIFY(df3.detailDefinitionName() == "Definition");
    QVERIFY(df3.detailFieldName() == "Field");

    /* Now test copy ctor through base class */
    QContactDetailFilter df4(f);
    QVERIFY(df4.type() == QContactFilter::ContactDetailFilter);
    QVERIFY(df4.detailDefinitionName().isEmpty());
    QVERIFY(df4.detailFieldName().isEmpty());
    QVERIFY(df4.value().isNull());

    /* reset it */
    df4 = df;
    QVERIFY(df4.detailDefinitionName() == "Definition");
    QVERIFY(df4.detailFieldName() == "Field");
}

void tst_QContactFilter::detailRangeFilter()
{
    QContactDetailRangeFilter rf;

    QVERIFY(rf.type() == QContactFilter::ContactDetailRangeFilter);

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

    rf.setMatchFlags(QContactFilter::MatchExactly);
    QVERIFY(rf.matchFlags() == QContactFilter::MatchExactly);

    rf.setMatchFlags(QContactFilter::MatchCaseSensitive);
    QVERIFY(rf.matchFlags() == QContactFilter::MatchCaseSensitive);

    // Contains is not allowed
    rf.setMatchFlags(QContactFilter::MatchCaseSensitive | QContactFilter::MatchContains);
    QVERIFY(rf.matchFlags() == QContactFilter::MatchCaseSensitive);

    rf.setMatchFlags(QContactFilter::MatchEndsWith);
    QVERIFY(rf.matchFlags() == QContactFilter::MatchExactly); // 0

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

    rf2 = rf;
    QVERIFY(rf2 == f);

    /* Self assignment should do nothing */
    rf2 = rf2;
    QVERIFY(rf2 == rf);
}

void tst_QContactFilter::relationshipFilter()
{
    QContactRelationshipFilter crf;

    QVERIFY(crf.type() == QContactFilter::RelationshipFilter);


    QVERIFY(crf.relationshipType() == QString());
    QVERIFY(crf.relatedContactId() == QContactId());

    QContactId newId;
    newId.setManagerUri("test");
    newId.setLocalId(QContactLocalId(5));
    crf.setRelatedContactId(newId);

    QVERIFY(crf.relationshipType() == QString());
    QVERIFY(crf.relatedContactId() == newId);

    crf.setRelatedContactRole(QContactRelationship::First);
    
    QVERIFY(crf.relationshipType() == QString());
    QVERIFY(crf.relatedContactId() == newId);

    crf.setRelationshipType(QContactRelationship::HasManager);

    QVERIFY(crf.relationshipType() == QContactRelationship::HasManager);
    QVERIFY(crf.relatedContactId() == newId);

    /* Test op= */
    QContactFilter f = crf;
    QVERIFY(f == crf);

    QContactRelationshipFilter crf2 = f;
    QVERIFY(crf2 == crf);

    /* Self assignment should do nothing */
    crf2 = crf2;
    QVERIFY(crf2 == crf);

    QContactDetailFilter dfil;
    QContactRelationshipFilter crf3(dfil);
    QVERIFY(crf3.type() == QContactFilter::RelationshipFilter); // should be a blank rel fil
    QContactRelationshipFilter crf4(crf);
    QVERIFY(crf4 == crf);
    crf = dfil;
    QVERIFY(crf == crf3);
    crf = crf3;
    crf.setRelationshipType("test"); // force a detach
}

void tst_QContactFilter::sortObject()
{
    QContactSortOrder sortorder;

    /* Defaults */
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QContactSortOrder());

    /* Blank Policy */
    sortorder.setBlankPolicy(QContactSortOrder::BlanksFirst);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksFirst);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder != QContactSortOrder());

    sortorder.setBlankPolicy(QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QContactSortOrder());

    /* Direction */
    sortorder.setDirection(Qt::DescendingOrder);
    QVERIFY(sortorder.direction() == Qt::DescendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder != QContactSortOrder());

    sortorder.setDirection(Qt::AscendingOrder);
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QContactSortOrder());

    /* Case sensitivity */
    sortorder.setCaseSensitivity(Qt::CaseInsensitive);
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseInsensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder != QContactSortOrder());

    sortorder.setCaseSensitivity(Qt::CaseSensitive);
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QContactSortOrder());

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
    QVERIFY(sortorder == QContactSortOrder());

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
    QVERIFY(other == sortorder);
    QVERIFY(!(other != sortorder));

    other.setDetailDefinitionName("Another Definition", "Detail");
    QVERIFY(other != sortorder);

    other.setDetailDefinitionName("Definition", "Another Detail");
    QVERIFY(other != sortorder);

    /* Assignment operator */
    QContactSortOrder another;
    another = other;
    QVERIFY(another.direction() == Qt::DescendingOrder);
    QVERIFY(another.blankPolicy() == QContactSortOrder::BlanksFirst);
    QVERIFY(another.detailDefinitionName() == "Definition");
    QVERIFY(another.detailFieldName() == "Another Detail");
    QVERIFY(another.isValid());
    QVERIFY(another == other);
    QVERIFY(!(other != another));

    /* Self assignment */
    another = another;
    QVERIFY(another.direction() == Qt::DescendingOrder);
    QVERIFY(another.blankPolicy() == QContactSortOrder::BlanksFirst);
    QVERIFY(another.detailDefinitionName() == "Definition");
    QVERIFY(another.detailFieldName() == "Another Detail");
    QVERIFY(another.isValid());
    QVERIFY(another == other);
    QVERIFY(!(other != another));
}

void tst_QContactFilter::boringFilters()
{
    QContactFilter all;
    QVERIFY(all.type() == QContactFilter::DefaultFilter);

    QContactInvalidFilter invalid;
    QVERIFY(invalid.type() == QContactFilter::InvalidFilter);

    QVERIFY(all != invalid);
    QVERIFY(!(all == invalid));

    /* Test op= */
    QContactFilter f = all;
    QVERIFY(f == all);

    QContactFilter f2;
    f2 = f;
    QVERIFY(f2 == all);

    /* Self assignment should do nothing */
    f2 = f2;
    QVERIFY(f2 == all);

    /* InvalidFilter, op= */
    QContactInvalidFilter inv2 = invalid;
    QVERIFY(inv2 == invalid);

    QContactInvalidFilter inv3;
    inv3 = inv2;
    QVERIFY(inv3 == invalid);

    inv3 = inv3;
    QVERIFY(inv3 == invalid);

    inv3 = all;
    QVERIFY(inv3 == invalid); // won't be all
}

void tst_QContactFilter::idListFilter()
{
    QContactLocalIdFilter idf;

    QVERIFY(idf.type() == QContactFilter::LocalIdFilter);

    QVERIFY(idf.ids().count() == 0);

    QList<QContactLocalId> ids;
    ids << 5 << 6 << 17;

    idf.setIds(ids);
    QVERIFY(idf.ids() == ids);

    idf.setIds(QList<QContactLocalId>());
    QVERIFY(idf.ids().count() == 0);

    /* Test op= */
    idf.setIds(ids);
    QContactFilter f = idf;
    QVERIFY(f == idf);

    QContactLocalIdFilter idf2 = f;
    QVERIFY(idf2 == idf);
    QVERIFY(idf2.ids() == ids);

    idf2 = idf;
    QVERIFY(idf2 == f);

    /* Self assignment should do nothing */
    idf2 = idf2;
    QVERIFY(idf2 == idf);

    QContactDetailFilter dfil;
    QContactLocalIdFilter idf3(dfil);
    QVERIFY(idf3.type() == QContactFilter::LocalIdFilter); // should be a blank id list filter
    QContactLocalIdFilter idf4(idf);
    QVERIFY(idf4 == idf); // should be a copy of idf.
    idf = dfil; // now assign.
    QVERIFY(idf == idf3); // again, should be a blank id list filter.
    idf = idf3;
    idf.setIds(ids); // force a detach
}

void tst_QContactFilter::canonicalizedFilter()
{
    QFETCH(QContactFilter, in);
    QFETCH(QContactFilter, expected);

    QContactFilter out = QContactManagerEngine::canonicalizedFilter(in);
    QCOMPARE(out, expected);
}

void tst_QContactFilter::canonicalizedFilter_data()
{
    QTest::addColumn<QContactFilter>("in");
    QTest::addColumn<QContactFilter>("expected");

    QContactFilter detailFilter1 = QContactName::match("1");
    QContactFilter detailFilter2 = QContactName::match("2");
    QContactInvalidFilter invalidFilter;
    QContactFilter defaultFilter;

    {
        QTest::newRow("Normal detail filter")
                << static_cast<QContactFilter>(detailFilter1)
                << static_cast<QContactFilter>(detailFilter1);
    }

    {
        QContactIntersectionFilter qcif;
        qcif << detailFilter1;
        qcif << detailFilter2;
        QTest::newRow("Normal intersection filter")
                << static_cast<QContactFilter>(qcif)
                << static_cast<QContactFilter>(qcif);
    }

    {
        QContactUnionFilter qcuf;
        qcuf << detailFilter1;
        qcuf << detailFilter2;
        QTest::newRow("Normal intersection filter")
                << static_cast<QContactFilter>(qcuf)
                << static_cast<QContactFilter>(qcuf);
    }

    {
        QContactIntersectionFilter qcif;
        QTest::newRow("Empty intersection")
                << static_cast<QContactFilter>(qcif)
                << static_cast<QContactFilter>(defaultFilter);
    }

    {
        QContactUnionFilter qcuf;
        QTest::newRow("Empty union")
                << static_cast<QContactFilter>(qcuf)
                << static_cast<QContactFilter>(invalidFilter);
    }

    {
        QContactIntersectionFilter qcif;
        qcif << detailFilter1;
        QTest::newRow("Single entry intersection filter")
                << static_cast<QContactFilter>(qcif)
                << static_cast<QContactFilter>(detailFilter1);
    }

    {
        QContactUnionFilter qcuf;
        qcuf << detailFilter1;
        QTest::newRow("Single entry union filter")
                << static_cast<QContactFilter>(qcuf)
                << static_cast<QContactFilter>(detailFilter1);
    }

    {
        QContactIntersectionFilter qcif;
        qcif << invalidFilter;
        qcif << detailFilter1;
        qcif << detailFilter2;
        QTest::newRow("Intersection with invalid")
                << static_cast<QContactFilter>(qcif)
                << static_cast<QContactFilter>(invalidFilter);
    }

    {
        QContactIntersectionFilter qcif;
        qcif << defaultFilter;
        qcif << detailFilter1;
        qcif << detailFilter2;
        QContactIntersectionFilter expected;
        expected << detailFilter1;
        expected << detailFilter2;
        QTest::newRow("Intersection with default")
                << static_cast<QContactFilter>(qcif)
                << static_cast<QContactFilter>(expected);
    }

    {
        QContactUnionFilter qcuf;
        qcuf << invalidFilter;
        qcuf << detailFilter1;
        qcuf << detailFilter2;
        QContactUnionFilter expected;
        expected << detailFilter1;
        expected << detailFilter2;
        QTest::newRow("Union with invalid")
                << static_cast<QContactFilter>(qcuf)
                << static_cast<QContactFilter>(expected);
    }

    {
        QContactUnionFilter qcuf;
        qcuf << defaultFilter;
        qcuf << detailFilter1;
        qcuf << detailFilter2;
        QTest::newRow("Union with default")
                << static_cast<QContactFilter>(qcuf)
                << static_cast<QContactFilter>(defaultFilter);
    }

    {
        QContactLocalIdFilter qclif;
        QTest::newRow("Empty local id filter")
                << static_cast<QContactFilter>(qclif)
                << static_cast<QContactFilter>(invalidFilter);
    }

    {
        QContactLocalIdFilter qclif;
        qclif.setIds(QList<QContactLocalId>() << 1 << 2);
        QTest::newRow("Normal local id filter")
                << static_cast<QContactFilter>(qclif)
                << static_cast<QContactFilter>(qclif);
    }

    {
        QContactDetailRangeFilter qcdrf;
        qcdrf.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldCustomLabel);
        QContactDetailFilter expected;
        expected.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldCustomLabel);
        QTest::newRow("Null valued range filter")
                << static_cast<QContactFilter>(qcdrf)
                << static_cast<QContactFilter>(expected);
    }

    {
        QContactDetailRangeFilter qcdrf;
        qcdrf.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldCustomLabel);
        qcdrf.setRange(QLatin1String("a"), QLatin1String("a"));
        qcdrf.setMatchFlags(QContactFilter::MatchFixedString);
        QContactDetailFilter expected;
        expected.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldCustomLabel);
        expected.setValue(QLatin1String("a"));
        expected.setMatchFlags(QContactFilter::MatchFixedString);
        QTest::newRow("Equal valued range filter")
                << static_cast<QContactFilter>(qcdrf)
                << static_cast<QContactFilter>(expected);
    }

    {
        QContactDetailRangeFilter qcdrf;
        qcdrf.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldCustomLabel);
        qcdrf.setRange(QLatin1String("a"), QLatin1String("a"),
               QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::ExcludeUpper);
        qcdrf.setMatchFlags(QContactFilter::MatchFixedString);
        QTest::newRow("Equal valued range filter with excluded bounds")
                << static_cast<QContactFilter>(qcdrf)
                << static_cast<QContactFilter>(invalidFilter);
    }

    {
        QContactDetailRangeFilter qcdrf;
        qcdrf.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldCustomLabel);
        qcdrf.setRange(QLatin1String("a"), QLatin1String("b"));
        qcdrf.setMatchFlags(QContactFilter::MatchFixedString);
        QTest::newRow("Normal range filter")
                << static_cast<QContactFilter>(qcdrf)
                << static_cast<QContactFilter>(qcdrf);
    }

    {
        QContactDetailRangeFilter qcdrf;
        qcdrf.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldCustomLabel);
        qcdrf.setRange(QVariant(QVariant::String), QVariant(QVariant::String)); // null bounds
        qcdrf.setMatchFlags(QContactFilter::MatchFixedString);
        QContactDetailFilter qcdf;
        qcdf.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldCustomLabel);
        qcdf.setMatchFlags(QContactFilter::MatchFixedString);
        qcdf.setValue(QVariant(QVariant::String));
        QTest::newRow("Null valued range filter")
                << static_cast<QContactFilter>(qcdrf)
                << static_cast<QContactFilter>(qcdf);
    }

    {
        QContactDetailRangeFilter qcdrf;
        qcdrf.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldCustomLabel);
        qcdrf.setRange(QVariant(QVariant::String), QLatin1String("a")); // min is null
        qcdrf.setMatchFlags(QContactFilter::MatchFixedString);
        QTest::newRow("One sided range filter")
                << static_cast<QContactFilter>(qcdrf)
                << static_cast<QContactFilter>(qcdrf);
    }

    {
        QContactDetailRangeFilter qcdrf;
        QTest::newRow("Empty range filter")
                << static_cast<QContactFilter>(qcdrf)
                << static_cast<QContactFilter>(invalidFilter);
    }

    {
        QContactDetailFilter qcdf;
        QTest::newRow("Empty detail filter")
                << static_cast<QContactFilter>(qcdf)
                << static_cast<QContactFilter>(invalidFilter);
    }
}

void tst_QContactFilter::testFilter()
{
    QFETCH(QContact, contact);
    QFETCH(QContactFilter, filter);
    QFETCH(bool, expected);

    QCOMPARE(QContactManagerEngine::testFilter(filter, contact), expected);
}

void tst_QContactFilter::testFilter_data()
{
    QTest::addColumn<QContact>("contact");
    QTest::addColumn<QContactFilter>("filter");
    QTest::addColumn<bool>("expected");

    {
        QContact contact;
        QContactName name;
        name.setFirstName(QLatin1String("first"));
        name.setMiddleName(QLatin1String("middle"));
        name.setLastName(QLatin1String("last"));
        name.setPrefix(QLatin1String("prefix"));
        name.setSuffix(QLatin1String("suffix"));
        contact.saveDetail(&name);

        QTest::newRow("QContactName::match firstname")
                << contact
                << QContactName::match("first")
                << true;
        QTest::newRow("QContactName::match lastname")
                << contact
                << QContactName::match("last")
                << true;
        QTest::newRow("QContactName::match middlename")
                << contact
                << QContactName::match("middle")
                << true;
        QTest::newRow("QContactName::match prefix")
                << contact
                << QContactName::match("prefix")
                << true;
        QTest::newRow("QContactName::match suffix")
                << contact
                << QContactName::match("suffix")
                << true;
        QTest::newRow("QContactName::match first last")
                << contact
                << QContactName::match(QLatin1String("first"), QLatin1String("last"))
                << true;
        QTest::newRow("QContactName::match substring")
                << contact
                << QContactName::match(QLatin1String("irs"))
                << true;
        QTest::newRow("QContactName::match first last substring")
                << contact
                << QContactName::match(QLatin1String("irs"), QLatin1String("as"))
                << true;
        QTest::newRow("QContactName::match negative")
                << contact
                << QContactName::match("foo")
                << false;
    }

    {
        QContact contact;
        QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("foo"));
        QTest::newRow("QContactDisplayLabel::match positive")
                << contact
                << QContactDisplayLabel::match("foo")
                << true;
        QTest::newRow("QContactDisplayLabel::match positive substring")
                << contact
                << QContactDisplayLabel::match("o")
                << true;
        QTest::newRow("QContactDisplayLabel::match negative")
                << contact
                << QContactDisplayLabel::match("bar")
                << false;
    }

    {
        QContact contact;
        QContactPhoneNumber phone;
        phone.setNumber("1234");
        contact.saveDetail(&phone);
        QTest::newRow("QContactPhoneNumber::match positive")
                << contact
                << QContactPhoneNumber::match("1234")
                << true;
        QTest::newRow("QContactPhoneNumber::match negative")
                << contact
                << QContactPhoneNumber::match("5678")
                << false;
    }

    {
        QContact contact;
        QContactEmailAddress email;
        email.setEmailAddress("foo");
        contact.saveDetail(&email);
        QTest::newRow("QContactEmailAddress::match positive")
                << contact
                << QContactEmailAddress::match("foo")
                << true;
        QTest::newRow("QContactEmailAddress::match positive substring")
                << contact
                << QContactEmailAddress::match("o")
                << true;
        QTest::newRow("QContactEmailAddress::match negative")
                << contact
                << QContactEmailAddress::match("bar")
                << false;
    }
}

void tst_QContactFilter::traits()
{
    QCOMPARE(sizeof(QContactFilter), sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QContactFilter)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

void tst_QContactFilter::sortTraits()
{
    QCOMPARE(sizeof(QContactSortOrder), sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QContactSortOrder)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}


QTEST_MAIN(tst_QContactFilter)
#include "tst_qcontactfilter.moc"
